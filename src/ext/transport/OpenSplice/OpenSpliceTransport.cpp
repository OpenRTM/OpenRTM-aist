// -*- C++ -*-
/*!
 * @file  OpenSpliceOutPort.cpp
 * @brief OpenSpliceOutPort class
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


#include <coil/Factory.h>
#include "OpenSpliceTransport.h"
#include "OpenSpliceOutPort.h"
#include "OpenSpliceInPort.h"
#include "OpenSpliceMessageInfo.h"
#include "OpenSpliceManagerFunc.h"


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
    //終了処理をすると異常終了するため一旦コメントアウトする
    //RTC_OpenSplice::shutdown();
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
  void OpenSpliceTransportInit(RTC::Manager* manager)
  {
      {
          RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
          factory.addFactory("opensplice",
              ::coil::Creator< ::RTC::InPortConsumer,
              ::RTC::OpenSpliceOutPort>);
      }
      {
          RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
          factory.addFactory("opensplice",
              ::coil::Creator< ::RTC::InPortProvider,
              ::RTC::OpenSpliceInPort>);
      }
      
      RTC_OpenSplice::start();
      OpenSpliceMessageInfoInit(manager->getConfig());
    
      ManagerActionListener *listener = new ManagerActionListener();
      manager->addManagerActionListener(listener);
      

  }
  
};


