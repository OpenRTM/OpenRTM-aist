// -*- C++ -*-
/*!
 * @file ComponentMain.cpp
 * @if jp
 * @brief `ComponentMain`の状態と操作をまとめる。 実装。
 * @else
 * @brief Encapsulate the state and operations of ComponentMain. Implementation.
 * @endif
 * @date $Date: 2026-11-13 15:32:20 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 */

#include <rtm/Manager.h>
extern "C" void CSP_COMPONENT_INIT(RTC::Manager* manager);

/*!
 * @if jp
 * @brief サンプルのFactoryを登録して自RTCを生成する。
 * @else
 * @brief Register the sample factory and create its RTC.
 * @endif
 */
void MyModuleInit(RTC::Manager* manager) {
  CSP_COMPONENT_INIT(manager);
  manager->createComponent(CSP_COMPONENT_TYPE);
}

/*!
 * @if jp
 * @brief Managerを初期化してサンプルの実行ループを開始する。
 * @param argc `argc`の値。
 * @param argv `argv`の値。
 * @else
 * @brief Initialize the manager and start the sample execution loop.
 * @param argc Value of argc.
 * @param argv Value of argv.
 * @endif
 */
int main(int argc, char** argv) {
  RTC::Manager* manager = RTC::Manager::init(argc, argv);
  manager->setModuleInitProc(MyModuleInit);
  manager->activateManager();
  manager->runManager();
  return 0;
}
