// -*- C++ -*-
/*!
 * @file  ROS2Transport.h
 * @brief ROS2Transport class
 * @date  $Date: 2019-02-04 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_ROS2TRANSPORT_H
#define RTC_ROS2TRANSPORT_H



#include <rtm/Manager.h>



extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * ROS2シリアライザを登録する
   *
   * @else
   * @brief Module initialization
   *
   * 
   *
   * @endif
   */
  DLL_EXPORT void ROS2TransportInit(RTC::Manager* manager);
}

#endif // RTC_ROS2TRANSPORT_H

