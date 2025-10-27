// -*- C++ -*-
/*!
 * @file  OpenrtmNamesPlugin.h
 * @brief OpenrtmNamesPlugin class
 * @date  $Date: 2025-10-17 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_OPENRTMNAMESPLUGIN_H
#define RTC_OPENRTMNAMESPLUGIN_H

#include <rtm/RTC.h>
#include <rtm/Manager.h>
#include "NamingContext.h"

namespace OpenRTMNames
{
  class ManagerActionListener : public RTM::ManagerActionListener
  {
  public:
    ManagerActionListener(RTC::Manager* manager);
    ~ManagerActionListener() override;
    void preShutdown() override;
    void postShutdown() override;
    void postReinit() override;
    void preReinit() override;
  private:
    PortableServer::Servant_var<RTM::NamingContext> m_nameservice;
    RTC::Manager* m_manager;
  };
}

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * ネーミングサービスの初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * 
   *
   * @endif
   */
  DLL_EXPORT void OpenrtmNamesPluginInit(RTC::Manager* manager);
}

#endif // RTC_OPENRTMNAMESPLUGIN_H

