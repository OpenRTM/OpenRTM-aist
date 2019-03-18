// -*- C++ -*-
/*!
 * @file  OutPortDirectConsumer.cpp
 * @brief OutPortDirectConsumer class
 * @date  $Date: 2018-01-17 07:52:40 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#include <rtm/OutPortDirectConsumer.h>
#include <rtm/NVUtil.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OutPortDirectConsumer::OutPortDirectConsumer()
  {
    rtclog.setName("OutPortDirectConsumer");
  }
    
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortDirectConsumer::~OutPortDirectConsumer()
  {
  } 

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void OutPortDirectConsumer::init(coil::Properties&  /*prop*/)
  {
    RTC_TRACE(("OutPortDirectConsumer::init()"));
  }

  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void OutPortDirectConsumer::setBuffer(CdrBufferBase*  /*buffer*/)
  {
    RTC_TRACE(("OutPortDirectConsumer::setBuffer()"));
  }

  /*!
   * @if jp
   * @brief リスナを設定する。
   * @else
   * @brief Set the listener. 
   * @endif
   */
  void OutPortDirectConsumer::setListener(ConnectorInfo&  /*info*/,
                                            ConnectorListeners*  /*listeners*/)
  {
    RTC_TRACE(("OutPortDirectConsumer::setListener()"));
  }

  /*!
   * @if jp
   * @brief データを読み出す
   * @else
   * @brief Read data
   * @endif
   */
  OutPortConsumer::ReturnCode
  OutPortDirectConsumer::get(ByteData&  /*data*/)
  {
    RTC_PARANOID(("get(): never called."));
    return UNKNOWN_ERROR;
  }
    
  /*!
   * @if jp
   * @brief データ受信通知への登録
   * @else
   * @brief Subscribe the data receive notification
   * @endif
   */
  bool OutPortDirectConsumer::
  subscribeInterface(const SDOPackage::NVList&  /*properties*/)
  {
    RTC_TRACE(("subscribeInterface(): do nothing"));
    return true;
  }
  
  /*!
   * @if jp
   * @brief データ受信通知からの登録解除
   * @else
   * @brief Unsubscribe the data receive notification
   * @endif
   */
  void OutPortDirectConsumer::
  unsubscribeInterface(const SDOPackage::NVList& /*properties*/)
  {
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
  void OutPortDirectConsumerInit(void)
  {
    RTC::OutPortConsumerFactory&
      factory(RTC::OutPortConsumerFactory::instance());
    factory.addFactory("direct",
                       ::coil::Creator< ::RTC::OutPortConsumer,
                                        ::RTC::OutPortDirectConsumer>,
                       ::coil::Destructor< ::RTC::OutPortConsumer,
                                           ::RTC::OutPortDirectConsumer>);
  }
};
