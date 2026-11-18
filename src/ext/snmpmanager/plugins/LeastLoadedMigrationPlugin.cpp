// -*- C++ -*-
/*!
 * @file LeastLoadedMigrationPlugin.cpp
 * @brief Example action plugin: migrates RTCs to the alive robot with the lowest load at that time
 *
 * @~japanese
 * 処理 (action) プラグインの例: 移動先を、その時点で負荷が最も低い生きているロボットに決めて移す
 *
 * 使い方・args は Python 版 plugins/least_loaded_migration.py と同じ。
 * @~english
 * The usage and args are the same as the Python version plugins/least_loaded_migration.py.
 * @~
 *
 * @date $Date: 2026-11-13 16:24:26 $
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

#include <snmpmgr/Plugin.hpp>

#include <algorithm>
#include <set>

using snmpmgr::json;

namespace
{
  /*!
   * @~japanese
   * @brief ロボットの負荷を返す (cgroup の CPU 使用率 / 制限。なければ cpu_usage)
   *
   * @param r ロボットの状態
   * @return 負荷 (%)
   * @~english
   * @brief Returns the load of a robot (cgroup CPU usage / limit, otherwise cpu_usage)
   *
   * @param r robot state
   * @return load (%)
   * @~
   */
  double load_of(const snmpmgr::RobotInfo& r)
  {
    if (r.cg_cpu_limit_pct && *r.cg_cpu_limit_pct > 0) { return 100.0 * r.cg_cpu_usage_pct.value_or(0) / *r.cg_cpu_limit_pct; }
    return r.cpu_usage.value_or(0);
  }
}

/*!
 * @~japanese
 * @brief プラグインの API のバージョンを返す
 *
 * @return SNMPMGR_PLUGIN_API_VERSION
 * @~english
 * @brief Returns the version of the plugin API
 *
 * @return SNMPMGR_PLUGIN_API_VERSION
 * @~
 */
SNMPMGR_PLUGIN_EXPORT int snmpmgr_plugin_api_version() { return SNMPMGR_PLUGIN_API_VERSION; }

/*!
 * @~japanese
 * @brief プラグインを登録する (処理 least_loaded_migration を登録する)
 *
 * @param ctx プラグインのコンテキスト
 * @param config_json プラグインの設定 (JSON。load_step)
 * @~english
 * @brief Registers the plugin (registers the action least_loaded_migration)
 *
 * @param ctx plugin context
 * @param config_json plugin configuration (JSON; load_step)
 * @~
 */
SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char* config_json)
{
  json config = json::parse(config_json);
  double default_step = config.value("load_step", 10.0);

  ctx->register_action("least_loaded_migration", [default_step](snmpmgr::PluginContext& c, const json& args) {
    std::string source = args.at("from");
    std::set<std::string> exclude;
    for (auto& e : args.value("exclude", json::array({source}))) { exclude.insert(e.get<std::string>()); }
    double step = args.value("load_step", default_step);
    std::map<std::string, double> targets;
    for (auto& r : c.alive_robots()) { if (!exclude.count(r.name)) { targets[r.name] = load_of(r); } }
    if (targets.empty()) { c.log("移動先にできる生きているロボットがありません"); return false; }

    std::set<std::string> moved;
    json components = json::array(), remove = json::array();
    std::string detail;
    for (auto& comp : args.at("components"))
      {
        auto best = std::min_element(targets.begin(), targets.end(), [](auto& a, auto& b) {
          return a.second < b.second || (a.second == b.second && a.first < b.first);
        });
        std::string dest = best->first;
        best->second += step;
        json e = comp;
        e["manager"] = dest;
        components.push_back(e);
        remove.push_back({{"name", comp["name"]}, {"manager", source}});
        moved.insert(comp["name"].get<std::string>());
        detail += (detail.empty() ? "" : ", ") + comp["name"].get<std::string>() + "@" + dest;
        c.log(comp["name"].get<std::string>() + ": " + source + " -> " + dest);
      }
    // 接続先のうち移さない RTC は、生きているロボットから探して existing にする
    std::set<std::string> others;
    for (auto& conn : args.value("connections", json::array()))
      {
        for (const char* end : {"source", "target"})
          {
            std::string n = conn[end]["component"];
            if (!moved.count(n)) { others.insert(n); }
          }
      }
    for (auto& n : others)
      {
        std::string where;
        for (auto& r : c.alive_robots())
          {
            try { if (c.has_component(n, r.name)) { where = r.name; break; } } catch (...) {}
          }
        if (where.empty()) { c.log("接続先の " + n + " が見つかりません"); return false; }
        components.push_back({{"name", n}, {"manager", where}, {"existing", n},
                              {"activate", args.value("activate_existing", true)}});
      }
    c.apply_plan({{"components", components}, {"connections", args.value("connections", json::array())}, {"remove", remove}},
                 "least_loaded_migration");
    c.event(c.name(), "plugin_migration", detail);
    return true;
  });
}
