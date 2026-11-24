// -*- C++ -*-
/*!
 * @file EventLogPlugin.cpp
 * @brief Example event plugin: appends events to a JSON Lines file (same as the Python version EventLogPlugin.py)
 *
 * @~japanese
 * イベント (event) プラグインの例: イベントを JSON Lines のファイルに追記する (Python 版 EventLogPlugin.py と同じ)
 *
 * @code
 *   {"module": "event_log", "config": {"path": "...", "kinds": ["down", "up"]}}
 * @endcode
 * @~english
 * @code
 *   {"module": "event_log", "config": {"path": "...", "kinds": ["down", "up"]}}
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

#include <fstream>
#include <mutex>

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
 * @brief プラグインを登録する (config の kinds のイベントを購読し、ファイルに書く)
 *
 * @param ctx プラグインのコンテキスト
 * @param config_json プラグインの設定 (JSON。path、kinds)
 * @~english
 * @brief Registers the plugin (subscribes to the event kinds in config and writes them to the file)
 *
 * @param ctx plugin context
 * @param config_json plugin configuration (JSON; path, kinds)
 * @~
 */
SNMPMGR_PLUGIN_EXPORT void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char* config_json)
{
  json config = json::parse(config_json);
  std::string path = config.value("path", "plugin_events.jsonl");
  auto mutex = std::make_shared<std::mutex>();
  auto write = [path, mutex](snmpmgr::PluginContext&, const snmpmgr::Event& e) {
    std::lock_guard<std::mutex> g(*mutex);
    std::ofstream(path, std::ios::app)
        << json{{"time", e.time}, {"observer", e.observer}, {"robot", e.robot}, {"kind", e.kind}, {"detail", e.detail}}.dump()
        << "\n";
  };
  for (auto& k : config.value("kinds", json::array({"*"}))) { ctx->subscribe(k.get<std::string>(), write); }
}
