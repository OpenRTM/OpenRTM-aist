// -*- C++ -*-
/*!
 * @file  ROSOutPort.cpp
 * @brief ROSOutPort class
 * @date  $Date: 2019-01-31 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <WinSock2.h>
#endif
#include "ROSTransport.h"
#include "ROSSerializer.h"
#include "ROSOutPort.h"
#include "ROSInPort.h"
#include "ROSTopicManager.h"


namespace ROSRTM
{
  static const char* const ros_sub_option[] =
  {
    "topic.__value__", "chatter",
    "topic.__widget__", "text",
    "topic.__constraint__", "none",
    "roscore.host.__value__", "",
    "roscore.host.__widget__", "text",
    "roscore.host.__constraint__", "none",
    "roscore.port.__value__", "",
    "roscore.port.__widget__", "text",
    "roscore.port.__constraint__", "none",
    "node.name.__value__", "",
    "node.name.__widget__", "text",
    "node.name.__constraint__", "none",
    "node.anonymous.__value__", "NO",
    "node.anonymous.__widget__", "radio",
    "node.anonymous.__constraint__", "(YES, NO)",
    "tcp_nodelay.__value__", "YES",
    "tcp_nodelay.__widget__", "radio",
    "tcp_nodelay.__constraint__", "(YES, NO)",
    "so_keepalive.__value__", "YES",
    "so_keepalive.__widget__", "radio",
    "so_keepalive.__constraint__", "(YES, NO)",
    "tcp_keepcnt.__value__", "9",
    "tcp_keepcnt.__widget__", "spin",
    "tcp_keepcnt.__constraint__", "1 <= x <= 10000",
    "tcp_keepidle.__value__", "60",
    "tcp_keepidle.__widget__", "spin",
    "tcp_keepidle.__constraint__", "1 <= x <= 10000",
    "tcp_keepintvl.__value__", "10",
    "tcp_keepintvl.__widget__", "spin",
    "tcp_keepintvl.__constraint__", "1 <= x <= 10000",
    ""
  };

  static const char* const ros_pub_option[] =
  {
    "topic.__value__", "chatter",
    "topic.__widget__", "text",
    "topic.__constraint__", "none",
    "roscore.host.__value__", "",
    "roscore.host.__widget__", "text",
    "roscore.host.__constraint__", "none",
    "roscore.port.__value__", "",
    "roscore.port.__widget__", "text",
    "roscore.port.__constraint__", "none",
    "node.name.__value__", "",
    "node.name.__widget__", "text",
    "node.name.__constraint__", "none",
    "node.anonymous.__value__", "NO",
    "node.anonymous.__widget__", "radio",
    "node.anonymous.__constraint__", "(YES, NO)",
    "tcp_nodelay.__value__", "YES",
    "tcp_nodelay.__widget__", "radio",
    "tcp_nodelay.__constraint__", "(YES, NO)",
    ""
  };

  ManagerActionListener::ManagerActionListener()
  {

  }
  ManagerActionListener::~ManagerActionListener()
  {

  }
  void ManagerActionListener::preShutdown()
  {

  }
  void ManagerActionListener::postShutdown()
  {
      RTC::ROSTopicManager::shutdown_global();
  }
  void ManagerActionListener::postReinit()
  {

  }

  void ManagerActionListener::preReinit()
  {

  }

}

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void ROSTransportInit(RTC::Manager* manager)
  {
    (void)manager;
    {
      coil::Properties prop(ROSRTM::ros_sub_option);
      RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
      factory.addFactory("ros",
                        ::coil::Creator< ::RTC::InPortProvider,
                                          ::RTC::ROSInPort>,
                        ::coil::Destructor< ::RTC::InPortProvider,
                                            ::RTC::ROSInPort>,
                        prop);
    }

    {
      coil::Properties prop(ROSRTM::ros_pub_option);
      RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
      factory.addFactory("ros",
                        ::coil::Creator< ::RTC::InPortConsumer,
                                          ::RTC::ROSOutPort>,
                        ::coil::Destructor< ::RTC::InPortConsumer,
                                            ::RTC::ROSOutPort>,
                        prop);
    }
    ROSSerializerInit(manager);

    RTC::ROSTopicManager::init();

    ROSRTM::ManagerActionListener *listener = new ROSRTM::ManagerActionListener();
    manager->addManagerActionListener(listener);
  }
  
}


