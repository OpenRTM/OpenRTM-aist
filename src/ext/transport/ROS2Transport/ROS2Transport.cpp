// -*- C++ -*-
/*!
 * @file  ROS2OutPort.cpp
 * @brief ROS2OutPort class
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


#include "ROS2Transport.h"
#include "ROS2Serializer.h"



extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void ROS2TransportInit(RTC::Manager* manager)
  {
    (void)manager;
    ROS2SerializerInit(manager);
  }
  
}


