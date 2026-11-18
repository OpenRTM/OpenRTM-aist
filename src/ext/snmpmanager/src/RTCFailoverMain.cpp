// -*- C++ -*-
/*!
 * @file RTCFailoverMain.cpp
 * @brief Entry point of rtcd_failover (C++ version of rtcd_failover.py)
 *
 * @~japanese
 * rtcd_failover (rtcd_failover.py の C++ 版) の入口
 *
 *   rtcd_failover --failover-config rtc_failover.json [--log-level DEBUG|INFO|WARNING|ERROR] -f rtc.conf [-o key:value ...]
 *
 * --failover-config と --log-level 以外の引数は、OpenRTM-aist のマネージャに渡す。
 * @~english
 *   rtcd_failover --failover-config rtc_failover.json [--log-level DEBUG|INFO|WARNING|ERROR] -f rtc.conf [-o key:value ...]
 *
 * Arguments other than --failover-config and --log-level are passed to the OpenRTM-aist manager.
 * @~
 *
 * @date $Date: 2026-11-17 09:00:00 $
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

#include <rtm/Manager.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#include "RTCFailover.hpp"

/*!
 * @~japanese
 * @brief 入口 (マネージャを起動し、RTCFailover を始めて、マネージャが終わるまで待つ)
 *
 * @param argc 引数の数
 * @param argv 引数
 * @return 終了コード
 * @~english
 * @brief Entry point (starts the manager and RTCFailover, then waits until the manager ends)
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return exit code
 * @~
 */
int main(int argc, char** argv)
{
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);   // ログ (UTF-8) をそのまま表示する
#endif
  std::string config_path;
  std::string level = "INFO";
  std::vector<char*> rtm_args{argv[0]};
  for (int i = 1; i < argc; ++i)
    {
      std::string a = argv[i];
      if (a == "--failover-config" && i + 1 < argc) { config_path = argv[++i]; }
      else if (a.rfind("--failover-config=", 0) == 0) { config_path = a.substr(18); }
      else if (a == "--log-level" && i + 1 < argc) { level = argv[++i]; }
      else if (a.rfind("--log-level=", 0) == 0) { level = a.substr(12); }
      else { rtm_args.push_back(argv[i]); }
    }
  rtcfailover::set_log_level(level);
  nlohmann::json config;
  try
    {
      config = rtcfailover::load_config(config_path);
    }
  catch (std::exception& e)
    {
      std::cerr << "rtcd_failover: invalid configuration: " << e.what() << std::endl;
      return 1;
    }
  int rtm_argc = static_cast<int>(rtm_args.size());
  RTC::Manager* manager = RTC::Manager::init(rtm_argc, rtm_args.data());
  manager->activateManager();
  // ORB を先に動かしておき、オブザーバーの通知 (update_status) を受けられるようにする
  manager->runManager(true);
  auto controller = std::make_unique<rtcfailover::FailoverController>(*manager, config);
  try
    {
      auto results = controller->start();
      std::string s;
      for (auto& [gid, ok] : results) { s += (s.empty() ? "" : ", ") + gid + ": " + (ok ? "True" : "False"); }
      rtcfailover::log(rtcfailover::Level::Info, "initial connections: {" + s + "}");
    }
  catch (std::exception& e)
    {
      rtcfailover::log(rtcfailover::Level::Error, std::string("start failed: ") + e.what());
    }
  catch (CORBA::SystemException& e)
    {
      rtcfailover::log(rtcfailover::Level::Error, std::string("start failed: CORBA ") + e._name());
    }
  manager->join();   // マネージャが終了するまで戻らない
  controller->stop();
  return 0;
}
