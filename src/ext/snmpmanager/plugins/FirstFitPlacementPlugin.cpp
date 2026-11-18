// -*- C++ -*-
/*!
 * @file FirstFitPlacementPlugin.cpp
 * @brief Example placement plugin: the first_fit logic (same as the Python version FirstFitPlacementPlugin.py)
 *
 * @~japanese
 * 配置 (placement) プラグインの例: first_fit のロジック (Python 版 FirstFitPlacementPlugin.py と同じ)
 *
 * @code
 *   "placement_plugin": "first_fit", "placement_args": {"order": ["robot1", "robot2", "robot3"]}
 * @endcode
 * @~english
 * @code
 *   "placement_plugin": "first_fit", "placement_args": {"order": ["robot1", "robot2", "robot3"]}
 * @endcode
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
 * @brief プラグインを登録する (配置のロジック first_fit を登録する)
 *
 * @param ctx プラグインのコンテキスト
 * @~english
 * @brief Registers the plugin (registers the placement logic first_fit)
 *
 * @param ctx plugin context
 * @~
 */
SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char*)
{
  ctx->register_placement("first_fit", [](snmpmgr::PluginContext&, const std::vector<snmpmgr::PlacementItem>& items,
                                          const std::vector<snmpmgr::PlacementRobot>& robots,
                                          const std::vector<snmpmgr::PlacementEdge>&, const std::string& primary,
                                          const json& args) {
    std::vector<std::string> order;
    if (args.contains("order")) { order = args["order"].get<std::vector<std::string>>(); }
    else
      {
        order.push_back(primary);
        for (auto& r : robots) { if (r.name != primary) { order.push_back(r.name); } }
      }
    std::map<std::string, const snmpmgr::PlacementRobot*> rmap;
    std::map<std::string, std::tuple<double, double, std::set<std::string>>> used;
    for (auto& r : robots) { rmap[r.name] = &r; used[r.name] = {0, 0, {r.languages.begin(), r.languages.end()}}; }
    std::map<std::string, std::string> assign;
    for (auto& it : items)
      {
        bool placed = false;
        for (auto& n : order)
          {
            if (!rmap.count(n) || std::find(it.candidates.begin(), it.candidates.end(), n) == it.candidates.end()) { continue; }
            auto& [cpu, mem, langs] = used[n];
            double extra = langs.count(it.language) ? 0.0 : it.slave_overhead_mb;
            if (cpu + it.cpu <= rmap[n]->cpu_free && mem + it.memory_mb + extra <= rmap[n]->mem_free_mb)
              {
                cpu += it.cpu;
                mem += it.memory_mb + extra;
                langs.insert(it.language);
                assign[it.name] = n;
                placed = true;
                break;
              }
          }
        if (!placed) { throw snmpmgr::PlacementError("first_fit: " + it.name + " を置けるロボットがありません"); }
      }
    return assign;
  });
}
