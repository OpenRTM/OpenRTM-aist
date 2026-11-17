// -*- C++ -*-
/*!
 * @file AliveCountPlugin.cpp
 * @brief Example condition plugin: decides by the number of alive robots (same as the Python version alive_count.py)
 *
 * @~japanese
 * 条件 (condition) プラグインの例: 生きているロボットの数で判定する (Python 版 alive_count.py と同じ)
 *
 * @code
 *   {"plugin": "alive_count", "args": {"op": ">=", "value": 3, "robots": [...]}}
 * @endcode
 * @~english
 * @code
 *   {"plugin": "alive_count", "args": {"op": ">=", "value": 3, "robots": [...]}}
 * @endcode
 * @~
 *
 * @date $Date: 2026-11-13 16:24:25 $
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
 * @brief プラグインを登録する (条件 alive_count を登録する)
 *
 * @param ctx プラグインのコンテキスト
 * @~english
 * @brief Registers the plugin (registers the condition alive_count)
 *
 * @param ctx plugin context
 * @~
 */
SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char*)
{
  ctx->register_condition("alive_count", [](snmpmgr::PluginContext& c, const json& args, double) {
    std::set<std::string> targets;
    for (auto& r : args.value("robots", json::array())) { targets.insert(r.get<std::string>()); }
    int n = 0;
    for (auto& r : c.alive_robots()) { if (targets.empty() || targets.count(r.name)) { ++n; } }
    int v = args.at("value").get<int>();
    std::string op = args.value("op", ">=");
    if (op == "<") { return n < v; }
    if (op == "<=") { return n <= v; }
    if (op == ">") { return n > v; }
    if (op == "==") { return n == v; }
    if (op == "!=") { return n != v; }
    return n >= v;
  });
}
