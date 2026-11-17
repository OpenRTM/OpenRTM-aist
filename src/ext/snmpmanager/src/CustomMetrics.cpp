// -*- C++ -*-
/*!
 * @file CustomMetrics.cpp
 * @brief Additional monitoring values (custom metrics). See CustomMetrics.hpp
 *
 * @~japanese
 * 追加の監視の値 (カスタムメトリクス)。説明は CustomMetrics.hpp
 * @~
 *
 * @date $Date: 2026-11-13 16:24:27 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2026
 *     Nobuhiko Miyamoto
 *
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include "CustomMetrics.hpp"

#include <cstdio>
#include <regex>
#include <set>
#include <stdexcept>

namespace snmpmgr
{
  namespace metrics
  {
    namespace
    {
      /*!
       * @~japanese
       * @brief 前後の空白と末尾の NUL を除く
       *
       * @param s 文字列
       * @return 文字列
       * @~english
       * @brief Removes leading/trailing whitespace and a trailing NUL
       *
       * @param s string
       * @return string
       * @~
       */
      std::string trim(std::string s)
      {
        while (!s.empty() && (s.back() == '\0' || std::isspace(static_cast<unsigned char>(s.back())))) { s.pop_back(); }
        size_t i = 0;
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) { ++i; }
        return s.substr(i);
      }

      /*!
       * @~japanese
       * @brief 数値を Python 版と同じ形の文字列にする
       *
       * @param v 値
       * @return 文字列
       * @~english
       * @brief Formats a number in the same way as the Python version
       *
       * @param v value
       * @return string
       * @~
       */
      std::string fmt(double v)
      {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%g", v);
        return buf;
      }
    } // namespace

    std::string extend_output_oid(const std::string& token)
    {
      std::string out = "1.3.6.1.4.1.8072.1.3.2.3.1.2." + std::to_string(token.size());
      for (unsigned char c : token) { out += "." + std::to_string(static_cast<int>(c)); }
      return out;
    }

    std::vector<std::pair<std::string, std::string>> parse_config(const Getter& get)
    {
      static const std::regex name_re("^[A-Za-z][A-Za-z0-9_]*$"), oid_re("^[0-9]+(\\.[0-9]+)+$");
      // ほかの監視の値と重なる名前は使えない (条件の metric で区別できなくなる)
      static const std::set<std::string> reserved{
          "wifi_strength", "wifi_signal_dbm", "wifi_signal_client_dbm", "wifi_signal_ap_dbm", "cpu_usage", "memory_usage",
          "network_in", "network_out", "battery_pct", "alive", "latency_ms", "jitter_ms", "packet_loss"};
      std::vector<std::pair<std::string, std::string>> out;
      std::set<std::string> seen;
      std::string list = get("snmp.metrics", "");
      size_t pos = 0;
      while (pos <= list.size())
        {
          size_t comma = list.find(',', pos);
          std::string name = trim(list.substr(pos, comma == std::string::npos ? std::string::npos : comma - pos));
          pos = comma == std::string::npos ? list.size() + 1 : comma + 1;
          if (name.empty()) { continue; }
          if (!std::regex_match(name, name_re)) { throw std::runtime_error("snmp.metrics: invalid metric name " + name); }
          if (reserved.count(name)) { throw std::runtime_error("snmp.metrics: " + name + " is a built-in metric"); }
          if (!seen.insert(name).second) { throw std::runtime_error("snmp.metrics: duplicate metric " + name); }
          std::string oid = trim(get("snmp.metric." + name + ".oid", ""));
          if (!oid.empty() && oid[0] == '.') { oid = oid.substr(1); }
          std::string ext = trim(get("snmp.metric." + name + ".extend", ""));
          if (oid.empty() == ext.empty()) { throw std::runtime_error("snmp.metric." + name + ": set exactly one of .oid and .extend"); }
          if (!oid.empty() && !std::regex_match(oid, oid_re)) { throw std::runtime_error("snmp.metric." + name + ".oid: invalid OID " + oid); }
          out.emplace_back(name, oid.empty() ? extend_output_oid(ext) : oid);
        }
      return out;
    }

    std::optional<double> parse_value(const std::string& raw)
    {
      std::string s = trim(raw);
      if (s.empty()) { return std::nullopt; }
      try
        {
          size_t used = 0;
          double v = std::stod(s, &used);
          if (used != s.size()) { return std::nullopt; }
          return v;
        }
      catch (...) { return std::nullopt; }
    }

    std::map<std::string, std::pair<std::string, std::string>> constraint_keys(const std::vector<std::string>& names)
    {
      std::map<std::string, std::pair<std::string, std::string>> out;
      for (auto& n : names)
        {
          out[n + "_max"] = {n, ">"};
          out[n + "_min"] = {n, "<"};
        }
      return out;
    }

    void validate_constraints(const json& constraints, const std::vector<std::string>& names, const std::string& where)
    {
      if (constraints.is_null()) { return; }
      if (!constraints.is_object()) { throw std::runtime_error(where + " must be an object like {\"<metric>_max\": value}"); }
      auto keys = constraint_keys(names);
      for (auto& [k, v] : constraints.items())
        {
          if (!keys.count(k)) { throw std::runtime_error(where + ": unknown key " + k); }
          if (!v.is_number()) { throw std::runtime_error(where + "." + k + " must be a number"); }
        }
    }

    std::vector<Violation> violations(const Values& values, const json& constraints, const std::vector<std::string>& names)
    {
      std::vector<Violation> out;
      if (!constraints.is_object()) { return out; }
      auto keys = constraint_keys(names);
      for (auto& [k, lim] : constraints.items())
        {
          auto it = keys.find(k);
          if (it == keys.end() || !lim.is_number()) { continue; }
          auto v = values.find(it->second.first);
          if (v == values.end() || !v->second) { continue; }
          double value = *v->second, limit = lim.get<double>();
          const std::string& op = it->second.second;
          if ((op == ">" && value > limit) || (op == "<" && value < limit)) { out.push_back({k, it->second.first, value, op, limit}); }
        }
      return out;
    }

    std::string describe(const Violation& v) { return v.name + " " + fmt(v.value) + " " + v.op + " " + fmt(v.limit); }
  } // namespace metrics
} // namespace snmpmgr
