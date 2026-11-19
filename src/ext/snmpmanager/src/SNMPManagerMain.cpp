// -*- C++ -*-
/*!
 * @file SNMPManagerMain.cpp
 * @brief Master manager with SNMPManager (C++ version)
 *
 * @~japanese
 * SNMPManager (C++ 版) を組み込んだマスターマネージャ
 *
 * 使い方: snmpmanager -f rtc.conf [-o key:value ...]
 * rtc.conf の snmp.* と構成ファイル (snmp.migration_config) は Python 版と同じ。
 * @~english
 * Usage: snmpmanager -f rtc.conf [-o key:value ...]
 * snmp.* in rtc.conf and the configuration file (snmp.migration_config) are the same as the Python version.
 * @~
 *
 * @date $Date: 2026-11-13 16:24:32 $
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

#include <csignal>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

#include "SNMPManager.hpp"

/*!
 * @~japanese
 * @brief 入口 (マネージャを初期化し、SNMPManager を始めて、マネージャを動かす)
 *
 * @param argc 引数の数
 * @param argv 引数
 * @return 終了コード
 * @~english
 * @brief Entry point (initializes the manager, starts SNMPManager and runs the manager)
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
  RTC::Manager* manager = RTC::Manager::init(argc, argv);
  manager->activateManager();
  std::unique_ptr<snmpmgr::SNMPManager> snmp;
  try
    {
      snmp = std::make_unique<snmpmgr::SNMPManager>(*manager);
      if (!snmp->init())
        {
          std::cerr << "SNMPManager initialization failed" << std::endl;
          manager->terminate();   // shutdown() は protected なので terminate() で止める
          return 1;
        }
    }
  catch (std::exception& e)
    {
      std::cerr << "SNMPManager: " << e.what() << std::endl;
      manager->terminate();   // shutdown() は protected なので terminate() で止める
      return 1;
    }
  manager->runManager();   // マネージャが終了するまで戻らない
  snmp->stop();
  return 0;
}
