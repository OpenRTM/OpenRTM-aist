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
      RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
      factory.addFactory("ros",
                        ::coil::Creator< ::RTC::InPortProvider,
                                          ::RTC::ROSInPort>);
    }

    {
      RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
      factory.addFactory("ros",
                        ::coil::Creator< ::RTC::InPortConsumer,
                                          ::RTC::ROSOutPort>);
    }
    ROSSerializerInit(manager);

    ManagerActionListener *listener = new ManagerActionListener();
    manager->addManagerActionListener(listener);
  }
  
};


