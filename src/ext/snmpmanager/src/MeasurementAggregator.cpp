// -*- C++ -*-
/*!
 * @file MeasurementAggregator.cpp
 * @brief Saving of measurements by the managers (same aggregation and placement.xml as the Python version snmpmgr_measure.py)
 *
 * @~japanese
 * マネージャによる実測値の保存 (Python 版 snmpmgr_measure.py と同じまとめ方・同じ placement.xml)
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

#include "MeasurementAggregator.hpp"
#include "SNMPManagerUtil.hpp"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <set>

namespace snmpmgr
{
  namespace measure
  {
    namespace
    {
      /*!
       * @~japanese
       * @brief JSON の数値を読む
       *
       * @param j JSON
       * @param key キー
       * @param def 既定値
       * @return 値
       * @~english
       * @brief Reads a number from JSON
       *
       * @param j JSON
       * @param key key
       * @param def default value
       * @return value
       * @~
       */
      double num(const json& j, const char* key, double def = 0)
      {
        if (!j.is_object() || !j.contains(key) || !j[key].is_number()) { return def; }
        return j[key].get<double>();
      }

      /*!
       * @~japanese
       * @brief 小数点以下 digits 桁に丸める
       *
       * @param v 値
       * @param digits 桁数
       * @return 丸めた値
       * @~english
       * @brief Rounds to digits decimal places
       *
       * @param v value
       * @param digits number of digits
       * @return rounded value
       * @~
       */
      double round_to(double v, int digits)
      {
        double f = std::pow(10.0, digits);
        return std::round(v * f) / f;
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
      std::string num_str(double v)
      {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.6f", v);
        std::string s = buf;
        while (!s.empty() && s.back() == '0') { s.pop_back(); }
        if (!s.empty() && s.back() == '.') { s += "0"; }
        return s;
      }

      /*!
       * @~japanese
       * @brief XML の特殊文字をエスケープする
       *
       * @param s 文字列
       * @return エスケープした文字列
       * @~english
       * @brief Escapes special characters of XML
       *
       * @param s string
       * @return escaped string
       * @~
       */
      std::string xml_escape(const std::string& s)
      {
        std::string out;
        for (char c : s)
          {
            switch (c)
              {
              case '&': out += "&amp;"; break;
              case '<': out += "&lt;"; break;
              case '>': out += "&gt;"; break;
              case '"': out += "&quot;"; break;
              default: out += c;
              }
          }
        return out;
      }

      /*!
       * @~japanese
       * @brief XML の属性の文字列を作る (plc: の名前空間)
       *
       * @param name 属性名
       * @param value 値
       * @return 属性の文字列
       * @~english
       * @brief Makes an XML attribute string (plc: namespace)
       *
       * @param name attribute name
       * @param value value
       * @return attribute string
       * @~
       */
      std::string a(const std::string& name, const std::string& value)
      {
        return " plc:" + name + "=\"" + xml_escape(value) + "\"";
      }
      /*!
       * @~japanese
       * @brief XML の属性の文字列を作る (数値)
       *
       * @param name 属性名
       * @param value 値
       * @return 属性の文字列
       * @~english
       * @brief Makes an XML attribute string (number)
       *
       * @param name attribute name
       * @param value value
       * @return attribute string
       * @~
       */
      std::string a(const std::string& name, double value) { return a(name, num_str(value)); }
    } // namespace

    std::map<std::string, double> attribute_cpu(double proc_cpu, const std::map<std::string, double>& ec_cpu)
    {
      std::map<std::string, double> out;
      if (ec_cpu.empty()) { return out; }
      double total = 0;
      for (auto& [k, v] : ec_cpu) { total += std::max(0.0, v); }
      double rest = std::max(0.0, proc_cpu - total);
      for (auto& [k, v0] : ec_cpu)
        {
          double v = std::max(0.0, v0);
          double share = total > 0 ? v / total : 1.0 / static_cast<double>(ec_cpu.size());
          out[k] = v + rest * share;
        }
      return out;
    }

    std::map<std::string, double> attribute_memory(double uss_mb, double baseline_mb,
                                                   const std::map<std::string, double>& increments)
    {
      std::map<std::string, double> out;
      if (increments.empty()) { return out; }
      double grown = std::max(0.0, uss_mb - baseline_mb), total = 0;
      for (auto& [k, v] : increments) { total += std::max(0.0, v); }
      for (auto& [k, v] : increments)
        {
          out[k] = grown * (total > 0 ? std::max(0.0, v) / total : 1.0 / static_cast<double>(increments.size()));
        }
      return out;
    }

    json summarize(std::vector<double> values)
    {
      if (values.empty()) { return {{"avg", nullptr}, {"max", nullptr}, {"p95", nullptr}}; }
      std::sort(values.begin(), values.end());
      double sum = 0;
      for (double v : values) { sum += v; }
      size_t i95 = std::min(values.size() - 1,
                            static_cast<size_t>(std::llround(0.95 * static_cast<double>(values.size() - 1))));
      auto r3 = [](double v) { return std::round(v * 1000.0) / 1000.0; };
      return {{"avg", r3(sum / static_cast<double>(values.size()))}, {"max", r3(values.back())}, {"p95", r3(values[i95])}};
    }

    std::string safe_name(const std::string& text)
    {
      std::string out;
      for (char c : text) { out += (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.') ? c : '_'; }
      return out.empty() ? "_" : out;
    }

    std::vector<std::string> update_runs(json& runs, const std::string& robot, const json& data)
    {
      std::vector<std::string> changed;
      if (!runs.is_object()) { runs = json::object(); }
      if (!data.contains("components") || !data["components"].is_object()) { return changed; }
      std::string manager = data.value("manager", "");
      for (auto& [key0, c] : data["components"].items())
        {
          std::string inst = c.value("instance", key0);
          long long created = static_cast<long long>(num(c, "created"));
          std::string key = manager + "/" + inst + "/" + std::to_string(created);
          json rec = c;
          rec["robot"] = robot;
          rec["manager"] = manager;
          rec["instance"] = inst;
          rec["baseline_uss_mb"] = data.value("baseline_uss_mb", json());
          rec["updated"] = data.value("updated", json());
          if (runs.contains(key) && runs[key].value("samples", json()) == rec.value("samples", json())
              && runs[key].value("ports", json()) == rec.value("ports", json()))
            {
              continue;
            }
          runs[key] = rec;
          changed.push_back(key);
        }
      return changed;
    }

    std::map<std::pair<std::string, std::string>, json> aggregate(const json& runs, int min_samples, int max_runs,
                                                                 const std::string& cpu_stat)
    {
      std::map<std::pair<std::string, std::string>, std::vector<json>> groups;
      for (auto& [k, r] : runs.items())
        {
          if (num(r, "samples") < min_samples) { continue; }
          if (!r.contains("cpu") || !r["cpu"].contains("max") || !r["cpu"]["max"].is_number()) { continue; }
          std::string lang = r.value("language", "");
          if (lang.empty()) { lang = "Python"; }
          groups[{lang, r.value("type", "")}].push_back(r);
        }
      std::map<std::pair<std::string, std::string>, json> out;
      for (auto& [key, rs] : groups)
        {
          std::stable_sort(rs.begin(), rs.end(), [](const json& x, const json& y) { return num(x, "updated") < num(y, "updated"); });
          if (static_cast<int>(rs.size()) > max_runs) { rs.erase(rs.begin(), rs.end() - max_runs); }
          double n = 0;
          for (auto& r : rs) { n += num(r, "samples"); }
          auto wavg = [&](const char* field) {
            double s = 0;
            for (auto& r : rs) { s += num(r[field], "avg") * num(r, "samples"); }
            return round_to(s / n, 3);
          };
          json cpu = {{"avg", wavg("cpu")}, {"max", 0.0}, {"p95", 0.0}};
          json mem = {{"avg", wavg("memory_mb")}, {"max", 0.0}};
          double baseline = 0, updated = 0;
          std::set<std::string> robots, instances;
          json ports = json::object();
          for (auto& r : rs)
            {
              cpu["max"] = std::max(cpu["max"].get<double>(), num(r["cpu"], "max"));
              cpu["p95"] = std::max(cpu["p95"].get<double>(), num(r["cpu"], "p95"));
              mem["max"] = std::max(mem["max"].get<double>(), num(r["memory_mb"], "max"));
              baseline = std::max(baseline, num(r, "baseline_uss_mb"));
              updated = std::max(updated, num(r, "updated"));
              if (!r.value("robot", "").empty()) { robots.insert(r.value("robot", "")); }
              instances.insert(r.value("instance", ""));
              if (!r.contains("ports") || !r["ports"].is_object()) { continue; }
              for (auto& [name, p] : r["ports"].items())
                {
                  if (!ports.contains(name))
                    {
                      ports[name] = {{"direction", p.value("direction", "")}, {"msgs", 0}, {"bytes", 0},
                                     {"bytes_max", 0}, {"hz", 0.0}};
                    }
                  auto& q = ports[name];
                  q["msgs"] = q["msgs"].get<long long>() + static_cast<long long>(num(p, "msgs"));
                  q["bytes"] = q["bytes"].get<long long>() + static_cast<long long>(num(p, "bytes"));
                  q["bytes_max"] = std::max(q["bytes_max"].get<long long>(), static_cast<long long>(num(p, "bytes_max")));
                  q["hz"] = std::max(q["hz"].get<double>(), num(p, "hz"));
                }
            }
          for (auto& [name, q] : ports.items())
            {
              long long m = q["msgs"].get<long long>();
              q["bytes_per_msg"] = m ? json(round_to(static_cast<double>(q["bytes"].get<long long>()) / m, 2)) : json();
            }
          double cpu_value = cpu.contains(cpu_stat) ? cpu[cpu_stat].get<double>() : cpu["max"].get<double>();
          out[key] = {{"language", key.first}, {"type", key.second}, {"impl", rs.back().value("impl", key.second)},
                      {"runs", rs.size()}, {"samples", n}, {"interval", rs.back().value("interval", json())},
                      {"cpu", cpu}, {"memory_mb", mem}, {"cpu_value", cpu_value}, {"cpu_stat", cpu_stat},
                      {"baseline_uss_mb", baseline}, {"robots", robots}, {"instances", instances},
                      {"updated", updated}, {"ports", ports}};
        }
      return out;
    }

    std::string render_placement(const json& agg, const std::string& environment, const std::string& robot)
    {
      double updated = num(agg, "updated");
      std::time_t t = updated > 0 ? static_cast<std::time_t>(updated) : std::time(nullptr);
      char date[64];
      std::strftime(date, sizeof(date), "%Y-%m-%dT%H:%M:%S%z", std::localtime(&t));
      double interval = num(agg, "interval");
      double samples = num(agg, "samples");
      std::string instances;
      for (auto& i : agg["instances"]) { instances += (instances.empty() ? "" : ", ") + i.get<std::string>(); }
      std::string L;
      L += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
      L += "<!-- SNMPManager が実行中に計測した値 (snmpmgr_measure)。" + robot + " の RTC: " + instances + " -->\n";
      L += "<plc:RtcPlacement xmlns:plc=\"urn:snmpmanager:placement:1\"" + a("rtc", agg.value("type", "")) +
           a("rtcId", agg.value("impl", agg.value("type", ""))) + a("language", agg.value("language", "")) +
           a("origin", "runtime") + a("robot", robot) + ">\n";
      L += "  <plc:Measurements>\n";
      L += "    <plc:Measurement" + a("environment", environment) + a("status", "ok") + a("date", date) +
           a("samples", std::to_string(static_cast<long long>(samples))) + a("runs", std::to_string(agg.value("runs", 0))) +
           a("intervalSec", interval > 0 ? num_str(interval) : std::string()) + a("durationSec", round_to(samples * interval, 1)) + ">\n";
      L += "      <plc:Resources" + a("cpu", round_to(num(agg, "cpu_value"), 2)) +
           a("memoryMb", round_to(num(agg["memory_mb"], "max"), 2)) +
           a("basis", "measured by the manager: cpu=" + agg.value("cpu_stat", std::string("max")) +
                          " of the RTC share, memory=max USS - empty manager USS (RTC share)") + "/>\n";
      L += "      <plc:Cpu" + a("avg", num(agg["cpu"], "avg")) + a("max", num(agg["cpu"], "max")) +
           a("p95", num(agg["cpu"], "p95")) + a("unit", "percent-of-one-core") + "/>\n";
      L += "      <plc:Memory plc:unit=\"MB\">\n";
      L += "        <plc:Uss" + a("avg", num(agg["memory_mb"], "avg")) + a("max", num(agg["memory_mb"], "max")) +
           a("baseline", round_to(num(agg, "baseline_uss_mb"), 2)) + "/>\n";
      L += "      </plc:Memory>\n";
      for (auto& [name, p] : agg["ports"].items())
        {
          if (num(p, "msgs") <= 0) { continue; }
          L += "      <plc:PortObservation" + a("name", name) + a("bytesPerMsg", num(p, "bytes_per_msg")) +
               a("bytesPerMsgMax", std::to_string(static_cast<long long>(num(p, "bytes_max")))) + a("hz", num(p, "hz")) +
               a("msgs", std::to_string(static_cast<long long>(num(p, "msgs")))) + "/>\n";
        }
      L += "    </plc:Measurement>\n  </plc:Measurements>\n  <plc:Ports>\n";
      for (auto& [name, p] : agg["ports"].items())
        {
          std::string size = p.contains("bytes_per_msg") && p["bytes_per_msg"].is_number() ? a("bytesPerMsg", num(p, "bytes_per_msg")) : "";
          L += "    <plc:Port" + a("name", name) + a("direction", p.value("direction", "")) + a("basis", "measured") + size + "/>\n";
        }
      L += "  </plc:Ports>\n</plc:RtcPlacement>\n";
      return L;
    }
  } // namespace measure
} // namespace snmpmgr
