// -*- C++ -*-
/*!
 * @file  InPortDirectConsumer.h
 * @brief InPortDirectConsumer class
 * @date  $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2015
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/NVUtil.h>
#include <rtm/InPortDirectConsumer.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @param buffer The buffer object that is attached to this Consumer
   * @endif
   */
  InPortDirectConsumer::InPortDirectConsumer()
    : rtclog("InPortDirectConsumer")
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortDirectConsumer::~InPortDirectConsumer()
  {
    RTC_PARANOID(("~InPortDirectConsumer()"));
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void InPortDirectConsumer::init(coil::Properties& prop)
  {
    m_properties = prop;
  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus InPortDirectConsumer::
     put(ByteData&  /*data*/)
  {
    RTC_PARANOID(("put(): never called."));
    return DataPortStatus::UNKNOWN_ERROR;
  }

  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void InPortDirectConsumer::
  publishInterfaceProfile(SDOPackage::NVList&  /*properties*/)
  {
    return;
  }

  /*!
   * @if jp
   * @brief データ送信通知への登録
   * @else
   * @brief Subscribe to the data sending notification
   * @endif
   */
  bool InPortDirectConsumer::
  subscribeInterface(const SDOPackage::NVList&  /*properties*/)
  {
    RTC_TRACE(("subscribeInterface(): do nothing"));
    return true;
  }

  /*!
   * @if jp
   * @brief データ送信通知からの登録解除
   * @else
   * @brief Unsubscribe the data send notification
   * @endif
   */
  void InPortDirectConsumer::
  unsubscribeInterface(const SDOPackage::NVList&  /*properties*/)
  {
    RTC_TRACE(("unsubscribeInterface(): do nothing"));
  }
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void InPortDirectConsumerInit(void)
  {
    RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
    factory.addFactory("direct",
                       ::coil::Creator< ::RTC::InPortConsumer,
                                        ::RTC::InPortDirectConsumer>,
                       ::coil::Destructor< ::RTC::InPortConsumer,
                                           ::RTC::InPortDirectConsumer>);
  }
}
