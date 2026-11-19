// -*- C++ -*-
/*!
 * @file LinkAwareObjectivePlugin.cpp
 * @brief Example objective plugin: an objective logic that weighs the link quality (same as the Python version LinkAwareObjectivePlugin.py)
 *
 * @~japanese
 * 目的関数 (objective) プラグインの例: 通信状態を重く見る objective のロジック (Python 版 LinkAwareObjectivePlugin.py と同じ)
 *
 * @code
 *   "logic": "objective",
 *   "objective_plugin": "link_aware",
 *   "objective_args": {"latency_weight": 1.0, "loss_weight": 1.0}
 * @endcode
 * @verbatim
 * 値 = 置いた後に残る CPU とメモリの割合
 *      - ロボットをまたぐ通信量 (KB/s) x (1 + そのロボットのレイテンシ ms x latency_weight / 10)
 *      - そのロボットのパケットロス (%) x loss_weight / 10
 * @endverbatim
 * @~english
 * @code
 *   "logic": "objective",
 *   "objective_plugin": "link_aware",
 *   "objective_args": {"latency_weight": 1.0, "loss_weight": 1.0}
 * @endcode
 * @verbatim
 * value = ratio of CPU and memory left after placing
 *         - traffic across robots (KB/s) x (1 + latency of the robot in ms x latency_weight / 10)
 *         - packet loss of the robot (%) x loss_weight / 10
 * @endverbatim
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
 * @brief プラグインを登録する (目的関数 link_aware を登録する)
 *
 * @param ctx プラグインのコンテキスト
 * @param config_json プラグインの設定 (JSON。latency_weight、loss_weight の既定値)
 * @~english
 * @brief Registers the plugin (registers the objective function link_aware)
 *
 * @param ctx plugin context
 * @param config_json plugin configuration (JSON; defaults of latency_weight and loss_weight)
 * @~
 */
SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char* config_json)
{
  json config = json::parse(config_json ? config_json : "{}", nullptr, false);
  if (!config.is_object()) { config = json::object(); }
  ctx->register_objective("link_aware", [config](snmpmgr::PluginContext&, const snmpmgr::PlacementItem& item,
                                                 const snmpmgr::PlacementRobot& robot,
                                                 const snmpmgr::ObjectiveState& state, const json& args) {
    double lw = args.value("latency_weight", config.value("latency_weight", 1.0));
    double pw = args.value("loss_weight", config.value("loss_weight", 1.0));
    snmpmgr::ObjectiveState::Usage u;
    if (auto it = state.usage.find(robot.name); it != state.usage.end()) { u = it->second; }
    double cpu_left = robot.cpu_free > 0 ? (robot.cpu_free - u.cpu - item.cpu) / robot.cpu_free : 0.0;
    double mem_left = robot.mem_free_mb > 0 ? (robot.mem_free_mb - u.mem - item.memory_mb) / robot.mem_free_mb : 0.0;
    double traffic = 0;
    for (auto& e : state.edges)
      {
        std::string other = e.a == item.name ? e.b : (e.b == item.name ? e.a : "");
        if (other.empty()) { continue; }
        auto a = state.assign.find(other);
        if (a != state.assign.end() && a->second != robot.name) { traffic += e.weight; }
      }
    auto get = [&](const char* k) { auto v = robot.link.find(k); return v == robot.link.end() ? 0.0 : v->second; };
    return cpu_left + mem_left - traffic / 1000.0 * (1.0 + get("latency_ms") * lw / 10.0) - get("packet_loss") * pw / 10.0;
  });
}
