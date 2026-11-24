// -*- C++ -*-
/*!
 * @file RTCPlacement.cpp
 * @brief Automatic placement of RTCs
 *
 * @~japanese
 * RTC の自動配置
 * @~
 *
 * @date $Date: 2026-11-13 16:24:28 $
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

#include "RTCPlacement.hpp"
#include "SNMPManagerUtil.hpp"

#include <tinyxml2.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <functional>

namespace snmpmgr
{
  namespace
  {
    const double kServiceWeight = 1000.0;
    const size_t kExhaustiveLimit = 12;

    /*!
     * @~japanese
     * @brief "plc:Measurement" の接頭辞を除いた名前を返す (tinyxml2 は名前空間を扱わないので、接頭辞を無視して要素名・属性名を比べる)
     *
     * @param n 名前
     * @return 接頭辞のない名前
     * @~english
     * @brief Returns the name without the prefix of "plc:Measurement" (tinyxml2 does not handle namespaces, so element and attribute names are compared ignoring prefixes)
     *
     * @param n name
     * @return name without prefix
     * @~
     */
    std::string local(const char* n)
    {
      std::string s = n ? n : "";
      auto p = s.find(':');
      return p == std::string::npos ? s : s.substr(p + 1);
    }

    /*!
     * @~japanese
     * @brief 属性の値を返す (接頭辞を無視する)
     *
     * @param e 要素
     * @param name 属性名
     * @return 値 (なければ nullptr)
     * @~english
     * @brief Returns the value of an attribute (ignoring prefixes)
     *
     * @param e element
     * @param name attribute name
     * @return value (nullptr if not found)
     * @~
     */
    const char* attr(const tinyxml2::XMLElement* e, const std::string& name)
    {
      for (auto* a = e->FirstAttribute(); a; a = a->Next())
        {
          if (local(a->Name()) == name) { return a->Value(); }
        }
      return nullptr;
    }

    /*!
     * @~japanese
     * @brief 属性の値を数値で返す
     *
     * @param e 要素
     * @param name 属性名
     * @param def 既定値
     * @return 値
     * @~english
     * @brief Returns the value of an attribute as a number
     *
     * @param e element
     * @param name attribute name
     * @param def default value
     * @return value
     * @~
     */
    double attr_d(const tinyxml2::XMLElement* e, const std::string& name, double def = 0)
    {
      const char* v = attr(e, name);
      auto d = v ? to_double(v) : std::nullopt;
      return d ? *d : def;
    }

    /*!
     * @~japanese
     * @brief 名前の子要素ごとに fn を呼ぶ
     *
     * @param e 要素
     * @param name 子要素の名前
     * @param fn 呼ぶ関数
     * @~english
     * @brief Calls fn for each child element with the name
     *
     * @param e element
     * @param name name of child elements
     * @param fn function to call
     * @~
     */
    void each(const tinyxml2::XMLElement* e, const std::string& name,
              const std::function<void(const tinyxml2::XMLElement*)>& fn)
    {
      for (auto* c = e->FirstChildElement(); c; c = c->NextSiblingElement())
        {
          if (local(c->Name()) == name) { fn(c); }
          each(c, name, fn);
        }
    }

    /*!
     * @~japanese
     * @brief 名前の最初の子要素を返す
     *
     * @param e 要素
     * @param name 子要素の名前
     * @return 子要素 (なければ nullptr)
     * @~english
     * @brief Returns the first child element with the name
     *
     * @param e element
     * @param name name of the child element
     * @return child element (nullptr if not found)
     * @~
     */
    const tinyxml2::XMLElement* child(const tinyxml2::XMLElement* e, const std::string& name)
    {
      for (auto* c = e->FirstChildElement(); c; c = c->NextSiblingElement())
        {
          if (local(c->Name()) == name) { return c; }
        }
      return nullptr;
    }

    /*!
     * @~japanese
     * @brief v に s があるか
     *
     * @param v 文字列のリスト
     * @param s 文字列
     * @return あれば true
     * @~english
     * @brief Whether v contains s
     *
     * @param v list of strings
     * @param s string
     * @return true if contained
     * @~
     */
    bool contains(const std::vector<std::string>& v, const std::string& s)
    {
      return std::find(v.begin(), v.end(), s) != v.end();
    }

    /*!
     * @~japanese
     * @brief ロボットをまたぐ接続の両端のロボットが、ロボット間の制約 (通信状態) を満たすか
     *
     * @param assign RTC 名 -> ロボット名
     * @param edges 接続
     * @param link ロボット名 -> 通信状態が制約を満たすか
     * @return 満たせば true
     * @~english
     * @brief Whether the robots at both ends of each connection across robots satisfy the inter-robot constraints (link quality)
     *
     * @param assign RTC name -> robot name
     * @param edges connections
     * @param link robot name -> whether the link satisfies the constraints
     * @return true if satisfied
     * @~
     */
    bool links_ok(const std::map<std::string, std::string>& assign, const std::vector<PlacementEdge>& edges,
                  const std::map<std::string, bool>& link)
    {
      for (auto& e : edges)
        {
          auto a = assign.find(e.a), b = assign.find(e.b);
          if (a == assign.end() || b == assign.end() || a->second == b->second) { continue; }
          auto la = link.find(a->second), lb = link.find(b->second);
          if ((la != link.end() && !la->second) || (lb != link.end() && !lb->second)) { return false; }
        }
      return true;
    }
  } // namespace

  std::string module_name_of(const std::string& create)
  {
    std::string base = create.substr(0, create.find('?'));
    if (base.find(':') != std::string::npos)
      {
        // "RTC:vendor:category:impl:lang:ver" (空の要素も位置として数える)
        std::vector<std::string> raw;
        std::string cur;
        for (char c : base) { if (c == ':') { raw.push_back(cur); cur.clear(); } else { cur += c; } }
        raw.push_back(cur);
        return raw.size() > 3 ? raw[3] : raw.back();
      }
    return base;
  }

  // ------------------------------------------------------------------ ProfileDB
  ProfileDB::ProfileDB(const std::string& root, std::string environment, bool prefer_measured, int min_samples)
      : env_(std::move(environment)), prefer_measured_(prefer_measured), min_samples_(min_samples)
  {
    for (auto& r : split(root, ',')) { load(r); }
  }

  void ProfileDB::load(const std::string& root)
  {
    namespace fs = std::filesystem;
    tinyxml2::XMLDocument envdoc;
    if (envdoc.LoadFile((fs::path(root) / "environments.xml").string().c_str()) == tinyxml2::XML_SUCCESS)
      {
        for (auto* e = envdoc.RootElement() ? envdoc.RootElement()->FirstChildElement() : nullptr; e;
             e = e->NextSiblingElement())
          {
            if (attr(e, "id") && !perf_.count(attr(e, "id"))) { perf_[attr(e, "id")] = attr_d(e, "perfScore"); }
          }
      }
    std::error_code ec;
    if (!fs::exists(root, ec)) { return; }
    std::vector<fs::path> files;
    for (auto& entry : fs::recursive_directory_iterator(root, ec))
      {
        if (entry.is_regular_file() && entry.path().filename() == "placement.xml") { files.push_back(entry.path()); }
      }
    std::sort(files.begin(), files.end());
    for (auto& path : files)
      {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(path.string().c_str()) != tinyxml2::XML_SUCCESS) { continue; }
        auto* rootel = doc.RootElement();
        if (!rootel || !attr(rootel, "rtc")) { continue; }
        std::string rtc = attr(rootel, "rtc");
        std::string lang = attr(rootel, "language") ? attr(rootel, "language") : "";
        bool runtime = attr(rootel, "origin") && std::string(attr(rootel, "origin")) == "runtime";
        std::set<std::string> required_tags;
        each(rootel, "Tag", [&](const tinyxml2::XMLElement* t) { if (attr(t, "name")) { required_tags.insert(attr(t, "name")); } });
        std::map<std::string, std::tuple<std::string, double>> port_sizes;   // name -> (direction, bytes)
        each(rootel, "Port", [&](const tinyxml2::XMLElement* p) {
          double b = attr(p, "bytesPerMsg") ? attr_d(p, "bytesPerMsg")
                   : attr(p, "referenceBytesPerMsg") ? attr_d(p, "referenceBytesPerMsg") : attr_d(p, "bytesPerMsgMin");
          port_sizes[attr(p, "name") ? attr(p, "name") : ""] = {attr(p, "direction") ? attr(p, "direction") : "", b};
        });
        std::vector<RtcProfile> per_env;
        each(rootel, "Measurement", [&](const tinyxml2::XMLElement* m) {
          const char* st = attr(m, "status");
          auto* res = child(m, "Resources");
          if (!st || std::string(st) != "ok" || !res) { return; }
          RtcProfile p;
          p.rtc = rtc;
          p.language = lang;
          p.environment = attr(m, "environment") ? attr(m, "environment") : "";
          p.cpu = attr_d(res, "cpu");
          p.memory_mb = attr_d(res, "memoryMb");
          if (auto* mem = child(m, "Memory")) { if (auto* u = child(mem, "Uss")) { p.slave_overhead_mb = attr_d(u, "baseline"); } }
          p.required_tags = required_tags;
          p.source = path.string();
          p.measured = runtime;
          p.samples = static_cast<int>(attr_d(m, "samples"));
          double rate = attr_d(m, "executionRate", 1000.0);
          std::map<std::string, double> hz;
          for (auto* o = m->FirstChildElement(); o; o = o->NextSiblingElement())
            {
              if (local(o->Name()) == "PortObservation" && attr(o, "name")) { hz[attr(o, "name")] = attr_d(o, "hz"); }
            }
          for (auto& [name, v] : port_sizes)
            {
              p.ports[name] = PortProfile{std::get<0>(v), std::get<1>(v), hz.count(name) ? hz[name] : rate};
            }
          per_env.push_back(p);
        });
        if (!per_env.empty()) { profiles_.emplace(std::make_pair(lang, rtc), Entry{runtime, per_env}); }
      }
  }

  std::optional<RtcProfile> ProfileDB::get(const std::string& type_name, const std::string& language) const
  {
    std::vector<const Entry*> statics, runtimes;
    auto range = profiles_.equal_range({language, type_name});
    for (auto it = range.first; it != range.second; ++it) { (it->second.runtime ? runtimes : statics).push_back(&it->second); }
    auto static_prof = best(statics);
    if (!prefer_measured_ || runtimes.empty()) { return static_prof; }
    std::vector<RtcProfile> exact, scaled;
    for (auto* e : runtimes)
      {
        auto p = best({e});
        if (!p || p->samples < min_samples_) { continue; }
        (p->scaled_from.empty() ? exact : scaled).push_back(*p);
      }
    if (exact.empty() && scaled.empty()) { return static_prof; }
    return merge_measured(static_prof, exact.empty() ? scaled : exact);
  }

  std::optional<RtcProfile> ProfileDB::best(const std::vector<const Entry*>& entries) const
  {
    std::optional<RtcProfile> fallback;
    for (auto* e : entries)
      {
        for (auto& p : e->per_env)
          {
            if (p.environment == env_) { return p; }
          }
        if (!fallback && !e->per_env.empty())
          {
            RtcProfile p = e->per_env.front();
            p.scaled_from = p.environment;
            auto src = perf_.find(p.environment), dst = perf_.find(env_);
            if (src != perf_.end() && dst != perf_.end() && dst->second > 0)
              {
                p.cpu = std::round(p.cpu * src->second / dst->second * 100.0) / 100.0;
              }
            fallback = p;
          }
      }
    return fallback;
  }

  RtcProfile merge_measured(const std::optional<RtcProfile>& static_prof, const std::vector<RtcProfile>& runtime)
  {
    RtcProfile out = runtime.front();
    std::map<std::string, PortProfile> ports = static_prof ? static_prof->ports : std::map<std::string, PortProfile>{};
    std::set<std::string> measured_ports;
    for (auto& r : runtime)
      {
        for (auto& [name, p] : r.ports)
          {
            if (p.bytes <= 0) { continue; }   // 送受信がなかったポートは静的な値のまま
            auto cur = ports.find(name);
            if (cur == ports.end() || !measured_ports.count(name) || p.bytes * p.hz > cur->second.bytes * cur->second.hz)
              {
                ports[name] = p;
                measured_ports.insert(name);
              }
          }
      }
    out.ports = ports;
    out.cpu = 0;
    out.memory_mb = 0;
    out.slave_overhead_mb = 0;
    out.samples = 0;
    std::string src;
    std::set<std::string> tags;
    for (auto& r : runtime)
      {
        out.cpu = std::max(out.cpu, r.cpu);
        out.memory_mb = std::max(out.memory_mb, r.memory_mb);
        out.slave_overhead_mb = std::max(out.slave_overhead_mb, r.slave_overhead_mb);
        out.samples += r.samples;
        src += (src.empty() ? "" : ", ") + r.source;
        tags.insert(r.required_tags.begin(), r.required_tags.end());
      }
    if (out.slave_overhead_mb <= 0 && static_prof) { out.slave_overhead_mb = static_prof->slave_overhead_mb; }
    out.required_tags = static_prof ? static_prof->required_tags : tags;
    out.source = "measured: " + src;
    out.measured = true;
    return out;
  }

  // ------------------------------------------------------------------ solver
  std::vector<PlacementEdge> build_edges(const json& connections, const std::vector<PlacementItem>& items,
                                         const std::map<std::string, RtcProfile>& profiles)
  {
    std::set<std::string> names;
    for (auto& i : items) { names.insert(i.name); }
    std::map<std::pair<std::string, std::string>, PlacementEdge> merged;
    for (auto& c : connections)
      {
        std::string s = c["source"]["component"], t = c["target"]["component"];
        if (!names.count(s) || !names.count(t)) { continue; }
        std::string sp = c["source"]["port"], tp = c["target"]["port"];
        const PortProfile* port = nullptr;
        if (auto it = profiles.find(s); it != profiles.end())
          {
            if (auto p = it->second.ports.find(sp); p != it->second.ports.end()) { port = &p->second; }
          }
        PlacementEdge e;
        if (port && (port->direction == "out" || port->direction == "in"))
          {
            const PortProfile* src = port;
            if (port->direction == "in")
              {
                src = nullptr;
                if (auto it = profiles.find(t); it != profiles.end())
                  {
                    if (auto p = it->second.ports.find(tp); p != it->second.ports.end()) { src = &p->second; }
                  }
              }
            e.weight = std::max(src ? src->bytes * src->hz : 0.0, 1.0);
            e.kind = "data";
          }
        else
          {
            e.weight = kServiceWeight;
            e.kind = "service";
          }
        auto key = std::minmax(s, t);
        auto& m = merged[{key.first, key.second}];
        if (m.a.empty()) { m = e; m.a = key.first; m.b = key.second; }
        else { m.weight += e.weight; }
      }
    std::vector<PlacementEdge> out;
    for (auto& [k, e] : merged) { out.push_back(e); }
    return out;
  }

  std::optional<PlacementResult> evaluate(const std::map<std::string, std::string>& assign,
                                          const std::vector<PlacementItem>& items,
                                          const std::vector<PlacementRobot>& robots,
                                          const std::vector<PlacementEdge>& edges, const std::string& primary)
  {
    std::map<std::string, std::pair<double, double>> usage;
    std::map<std::string, std::set<std::string>> langs;
    for (auto& r : robots) { usage[r.name] = {0, 0}; langs[r.name] = {r.languages.begin(), r.languages.end()}; }
    for (auto& it : items)
      {
        auto a = assign.find(it.name);
        if (a == assign.end() || !usage.count(a->second)) { return std::nullopt; }
        auto& u = usage[a->second];
        u.first += it.cpu;
        u.second += it.memory_mb;
        if (!langs[a->second].count(it.language)) { u.second += it.slave_overhead_mb; langs[a->second].insert(it.language); }
      }
    double max_sec = 0;
    std::map<std::string, bool> link;
    for (auto& r : robots)
      {
        auto& u = usage[r.name];
        if (u.first > r.cpu_free + 1e-9 || u.second > r.mem_free_mb + 1e-9) { return std::nullopt; }
        if (r.name != primary && r.cpu_free > 0) { max_sec = std::max(max_sec, u.first / r.cpu_free); }
        link[r.name] = r.link_ok;
      }
    if (!links_ok(assign, edges, link)) { return std::nullopt; }
    PlacementResult p;
    p.assign = assign;
    double cross = 0;
    for (auto& e : edges)
      {
        if (assign.at(e.a) != assign.at(e.b)) { p.cross.push_back(e); cross += e.weight; }
      }
    int n_primary = 0;
    for (auto& it : items) { if (assign.at(it.name) == primary) { ++n_primary; } }
    p.usage = usage;
    p.score = {std::round(cross * 1000) / 1000, -n_primary, -std::round(usage[primary].first * 1000) / 1000,
               std::round(max_sec * 10000) / 10000};
    return p;
  }

  std::vector<std::string> capacity_report(const std::vector<PlacementItem>& items,
                                           const std::vector<PlacementRobot>& robots)
  {
    double cpu = 0, mem = 0;
    for (auto& i : items) { cpu += i.cpu; mem += i.memory_mb; }
    std::vector<std::string> lines{"必要量の合計: CPU " + fmt(cpu) + "%, メモリ " + fmt(mem) + "MB (slave マネージャの分を除く)"};
    for (auto& r : robots)
      {
        lines.push_back(r.name + ": 空き CPU " + fmt(r.cpu_free) + "%, メモリ " + fmt(r.mem_free_mb) + "MB" +
                        (r.reserved_mb > 0 ? " (予約 " + fmt(r.reserved_mb) + "MB)" : "") +
                        (r.link_ok ? "" : " 通信状態がロボット間の制約を満たさない"));
      }
    for (auto& i : items)
      {
        bool fits = false;
        for (auto& r : robots)
          {
            if (contains(i.candidates, r.name) && i.cpu <= r.cpu_free && i.memory_mb + i.slave_overhead_mb <= r.mem_free_mb) { fits = true; }
          }
        if (!fits) { lines.push_back(i.name + " (CPU " + fmt(i.cpu) + "%) は単独でもどのロボットにも入らない"); }
      }
    return lines;
  }

  const std::vector<std::string>& logics()
  {
    static const std::vector<std::string> l{"optimal", "first_fit", "objective"};
    return l;
  }

  const std::map<std::string, double>& default_weights()
  {
    static const std::map<std::string, double> w{{"cpu", 1.0}, {"memory", 1.0}, {"traffic", 1.0}, {"primary", 0.0}, {"link", 0.0}};
    return w;
  }

  bool link_ok(const std::map<std::string, double>& link, const json& rules)
  {
    if (!rules.is_object()) { return true; }
    struct Check { const char* rule; const char* key; bool upper; };
    for (auto c : {Check{"max_latency_ms", "latency_ms", true}, Check{"max_jitter_ms", "jitter_ms", true},
                   Check{"max_packet_loss", "packet_loss", true}, Check{"min_rssi_dbm", "rssi_dbm", false}})
      {
        if (!rules.contains(c.rule) || !rules[c.rule].is_number()) { continue; }
        auto v = link.find(c.key);
        if (v == link.end()) { continue; }
        double t = rules[c.rule].get<double>();
        if (c.upper ? v->second > t : v->second < t) { return false; }
      }
    return true;
  }

  double objective_value(const PlacementItem& item, const PlacementRobot& robot, const ObjectiveState& state,
                         const std::map<std::string, double>& weights)
  {
    auto w = default_weights();
    for (auto& [k, v] : weights) { w[k] = v; }
    ObjectiveState::Usage u;
    if (auto it = state.usage.find(robot.name); it != state.usage.end()) { u = it->second; }
    double extra = contains(u.langs, item.language) ? 0.0 : item.slave_overhead_mb;
    double cpu_left = robot.cpu_free > 0 ? (robot.cpu_free - u.cpu - item.cpu) / robot.cpu_free : 0.0;
    double mem_left = robot.mem_free_mb > 0 ? (robot.mem_free_mb - u.mem - item.memory_mb - extra) / robot.mem_free_mb : 0.0;
    double traffic = 0;
    for (auto& e : state.edges)
      {
        std::string other = e.a == item.name ? e.b : (e.b == item.name ? e.a : "");
        if (other.empty()) { continue; }
        auto a = state.assign.find(other);
        if (a != state.assign.end() && a->second != robot.name) { traffic += e.weight; }
      }
    double badness = 0;
    for (const char* k : {"latency_ms", "jitter_ms", "packet_loss"})
      {
        auto v = robot.link.find(k);
        if (v != robot.link.end()) { badness += v->second / 10.0; }
      }
    return w["cpu"] * cpu_left + w["memory"] * mem_left - w["traffic"] * traffic / 1000.0 +
           w["primary"] * (robot.name == state.primary ? 1.0 : 0.0) - w["link"] * badness;
  }

  namespace
  {
    /*!
     * @~japanese
     * @brief first_fit と objective のロジック。RTC を 1 つずつ置く
     *
     * @param items 配置する RTC
     * @param robots ロボット
     * @param edges 接続
     * @param primary 主ロボットの名前
     * @param logic first_fit / objective
     * @param objective 目的関数
     * @return PlacementResult
     * @~english
     * @brief The first_fit and objective logics. Places RTCs one by one
     *
     * @param items RTCs to place
     * @param robots robots
     * @param edges connections
     * @param primary name of the primary robot
     * @param logic first_fit / objective
     * @param objective objective function
     * @return PlacementResult
     * @~
     */
    PlacementResult sequential(const std::vector<PlacementItem>& items, const std::vector<PlacementRobot>& robots,
                               const std::vector<PlacementEdge>& edges, const std::string& primary,
                               const std::string& logic, const Objective& objective)
    {
      ObjectiveState state;
      state.edges = edges;
      state.primary = primary;
      state.robots = robots;
      std::map<std::string, bool> link;
      for (auto& r : robots)
        {
          state.usage[r.name].langs = r.languages;
          link[r.name] = r.link_ok;
        }
      // 待機モジュールと既存の RTC (置き場所が決まっているもの) を先に置く
      std::vector<const PlacementItem*> order;
      for (auto& it : items) { if (!it.standby.empty() || it.fixed) { order.push_back(&it); } }
      for (auto& it : items) { if (it.standby.empty() && !it.fixed) { order.push_back(&it); } }
      for (auto* it : order)
        {
          std::vector<const PlacementRobot*> feasible;
          for (auto& r : robots)
            {
              if (!contains(it->candidates, r.name)) { continue; }
              auto& u = state.usage[r.name];
              double extra = contains(u.langs, it->language) ? 0.0 : it->slave_overhead_mb;
              if (u.cpu + it->cpu > r.cpu_free + 1e-9 || u.mem + it->memory_mb + extra > r.mem_free_mb + 1e-9) { continue; }
              // CHECK: 通信する RTC が別のロボットにあるなら、ロボット間の制約を満たすこと
              auto trial = state.assign;
              trial[it->name] = r.name;
              if (!links_ok(trial, edges, link)) { continue; }
              feasible.push_back(&r);
              if (logic == "first_fit") { break; }
            }
          if (feasible.empty())
            {
              throw PlacementError(it->name + " (" + it->module + ") を置けるロボットがありません (ロジック " + logic + ")",
                                   capacity_report(items, robots));
            }
          const PlacementRobot* chosen = feasible.front();
          if (logic != "first_fit" && feasible.size() > 1)
            {
              double best = 0;
              bool first = true;
              for (auto* r : feasible)   // 同じ値ならリストの先のロボット
                {
                  double v = objective ? objective(*it, *r, state) : objective_value(*it, *r, state);
                  if (first || v > best) { best = v; chosen = r; first = false; }
                }
            }
          auto& u = state.usage[chosen->name];
          u.cpu += it->cpu;
          u.mem += it->memory_mb + (contains(u.langs, it->language) ? 0.0 : it->slave_overhead_mb);
          if (!contains(u.langs, it->language)) { u.langs.push_back(it->language); }
          state.assign[it->name] = chosen->name;
        }
      auto result = evaluate(state.assign, items, robots, edges, primary);
      if (!result) { throw PlacementError("ロジック " + logic + " の結果が制約を満たしません", capacity_report(items, robots)); }
      return *result;
    }
  } // namespace

  PlacementResult solve(const std::vector<PlacementItem>& items, const std::vector<PlacementRobot>& robots,
                        const std::vector<PlacementEdge>& edges, const std::string& primary,
                        const std::string& logic, const Objective& objective)
  {
    std::vector<std::string> reasons;
    for (auto& it : items)
      {
        if (it.candidates.empty()) { reasons.push_back(it.name + " (" + it.module + ") を置けるロボットがない"); }
      }
    if (!reasons.empty()) { throw PlacementError("配置できない RTC があります", reasons); }
    if (!contains(logics(), logic))
      {
        throw PlacementError("未知の配置ロジック " + logic + " (使えるのは optimal, first_fit, objective)");
      }
    if (logic != "optimal") { return sequential(items, robots, edges, primary, logic, objective); }

    std::vector<const PlacementItem*> order;
    for (auto& i : items) { order.push_back(&i); }
    std::sort(order.begin(), order.end(), [](auto* a, auto* b) { return a->cpu > b->cpu; });
    std::map<std::string, const PlacementRobot*> rmap;
    for (auto& r : robots) { rmap[r.name] = &r; }

    std::optional<PlacementResult> best;
    std::map<std::string, std::string> assign;
    std::map<std::string, std::tuple<double, double, std::set<std::string>>> used;
    for (auto& r : robots) { used[r.name] = {0, 0, {r.languages.begin(), r.languages.end()}}; }

    auto partial_cross = [&] {
      double c = 0;
      for (auto& e : edges)
        {
          auto a = assign.find(e.a), b = assign.find(e.b);
          if (a != assign.end() && b != assign.end() && a->second != b->second) { c += e.weight; }
        }
      return std::round(c * 1000) / 1000;
    };

    std::function<void(size_t)> rec = [&](size_t k) {
      if (best && partial_cross() > std::get<0>(best->score)) { return; }
      if (k == order.size())
        {
          auto p = evaluate(assign, items, robots, edges, primary);
          if (p && (!best || p->score < best->score)) { best = p; }
          return;
        }
      const PlacementItem& it = *order[k];
      for (auto& rn : it.candidates)
        {
          auto& [cpu, mem, langs] = used[rn];
          bool new_lang = !langs.count(it.language);
          double extra = new_lang ? it.slave_overhead_mb : 0.0;
          if (cpu + it.cpu > rmap[rn]->cpu_free + 1e-9 || mem + it.memory_mb + extra > rmap[rn]->mem_free_mb + 1e-9) { continue; }
          cpu += it.cpu; mem += it.memory_mb + extra;
          if (new_lang) { langs.insert(it.language); }
          assign[it.name] = rn;
          rec(k + 1);
          assign.erase(it.name);
          cpu -= it.cpu; mem -= it.memory_mb + extra;
          if (new_lang) { langs.erase(it.language); }
        }
    };

    if (items.size() <= kExhaustiveLimit)
      {
        rec(0);
      }
    else
      {
        // 大きい問題: primary から順に、CPU の大きい順に入るところへ入れる
        std::vector<std::string> robot_order{primary};
        for (auto& r : robots) { if (r.name != primary) { robot_order.push_back(r.name); } }
        for (auto* it : order)
          {
            bool placed = false;
            for (auto& rn : robot_order)
              {
                if (!contains(it->candidates, rn)) { continue; }
                assign[it->name] = rn;
                std::vector<PlacementItem> sub;
                for (auto& i : items) { if (assign.count(i.name)) { sub.push_back(i); } }
                if (evaluate(assign, sub, robots, edges, primary)) { placed = true; break; }
                assign.erase(it->name);
              }
            if (!placed) { throw PlacementError("リソースの制限内に全 RTC を配置できません", capacity_report(items, robots)); }
          }
        best = evaluate(assign, items, robots, edges, primary);
      }
    if (!best) { throw PlacementError("リソースの制限内に全 RTC を配置できません", capacity_report(items, robots)); }
    return *best;
  }

  std::vector<std::string> describe(const PlacementResult& p, const std::vector<PlacementItem>& items,
                                    const std::vector<PlacementRobot>& robots, const std::string& primary,
                                    const std::map<std::string, RtcProfile>& profiles)
  {
    std::vector<std::string> lines;
    bool any_fixed = false;
    for (auto& r : robots)
      {
        std::string names;
        for (auto& i : items)
          {
            if (p.assign.at(i.name) == r.name) { names += (names.empty() ? "" : ", ") + i.name + (i.fixed ? "*" : ""); }
            any_fixed = any_fixed || i.fixed;
          }
        auto u = p.usage.at(r.name);
        lines.push_back(r.name + (r.name == primary ? " (primary)" : "") + ": " + (names.empty() ? "-" : names) +
                        "  CPU " + fmt(u.first) + "/" + fmt(r.cpu_free) + "%  メモリ " + fmt(u.second) + "/" +
                        fmt(r.mem_free_mb) + "MB");
      }
    if (any_fixed) { lines.push_back("(* は既存の RTC。動かさない)"); }
    if (p.cross.empty()) { lines.push_back("ロボットをまたぐ接続: なし"); }
    else
      {
        std::string s = "ロボットをまたぐ接続: ";
        for (size_t i = 0; i < p.cross.size(); ++i)
          {
            s += (i ? ", " : "") + p.cross[i].a + "-" + p.cross[i].b + " (" + p.cross[i].kind + ", " +
                 fmt(p.cross[i].weight, 0) + "B/s)";
          }
        lines.push_back(s);
      }
    for (auto& i : items)
      {
        if (!i.standby.empty()) { lines.push_back(i.name + ": 待機モジュール " + i.standby + " (" + p.assign.at(i.name) + ") を使う"); }
        auto pr = profiles.find(i.name);
        if (pr != profiles.end() && !pr->second.scaled_from.empty() && !i.fixed)
          {
            lines.push_back(i.name + ": 環境 " + pr->second.scaled_from + " の計測値を perfScore の比で換算 (CPU " + fmt(i.cpu, 2) + "%)");
          }
      }
    return lines;
  }
} // namespace snmpmgr
