// -*- C++ -*-
/*!
 * @file  SSMOutPort.cpp
 * @brief SSMOutPort class
 * @date  $Date: 2020-03-10 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2020
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */


#include <ssm.h>
#include "SSMTransport.h"
#include "SSMOutPort.h"
#include "SSMInPort.h"

namespace SSMRTM
{
  static const char* const ssm_inport_option[] =
  {
    "stream_name.__value__", "sensor_test",
    "stream_name.__widget__", "text",
    "stream_id.__value__", "0",
    "stream_id.__widget__", "spin",
    "stream_id.__constraint__", "0 <= x <= 10000",
    ""
  };
  static const char* const ssm_outport_option[] =
  {
    "stream_name.__value__", "sensor_test",
    "stream_name.__widget__", "text",
    "stream_id.__value__", "0",
    "stream_id.__widget__", "spin",
    "stream_id.__constraint__", "0 <= x <= 10000",
    "stream_size.__value__", "0",
    "stream_size.__widget__", "spin",
    "stream_size.__constraint__", "0 <= x <= 2147483647",
    "life_ssm_time.__value__", "5.0",
    "life_ssm_time.__widget__", "slider.0.01",
    "life_ssm_time.__constraint__", "0.00 <= x <= 1000.00",
    "cycle_ssm_time.__value__", "0.05",
    "cycle_ssm_time.__widget__", "slider.0.01",
    "cycle_ssm_time.__constraint__", "0.00 <= x <= 1000.00",
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
    endSSM();
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
  void SSMTransportInit(RTC::Manager* manager)
  {
    if(!initSSM())
    {
      throw;
    }

    {
      coil::Properties prop(SSMRTM::ssm_inport_option);
      RTC::OutPortConsumerFactory& factory(RTC::OutPortConsumerFactory::instance());
      factory.addFactory("ssm",
                        ::coil::Creator< ::RTC::OutPortConsumer,
                                          ::RTC::SSMInPort>,
                        ::coil::Destructor< ::RTC::OutPortConsumer,
                                            ::RTC::SSMInPort>,
                        prop);
    }

    {
      coil::Properties prop(SSMRTM::ssm_outport_option);
      RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
      factory.addFactory("ssm",
                        ::coil::Creator< ::RTC::InPortConsumer,
                                          ::RTC::SSMOutPort>,
                        ::coil::Destructor< ::RTC::InPortConsumer,
                                            ::RTC::SSMOutPort>,
                        prop);
    }



    SSMRTM::ManagerActionListener *listener = new SSMRTM::ManagerActionListener();
    manager->addManagerActionListener(listener);
    
  }
  
}


