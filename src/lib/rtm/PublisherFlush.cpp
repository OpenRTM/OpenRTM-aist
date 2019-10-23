// -*- C++ -*-
/*!
 * @file  PublisherFlush.cpp
 * @brief PublisherFlush class
 * @date  $Date: 2007-12-31 03:08:06 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
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

#include <coil/Properties.h>
#include <rtm/RTC.h>
#include <rtm/PublisherBase.h>
#include <rtm/PublisherFlush.h>
#include <rtm/InPortConsumer.h>
#include <rtm/ConnectorListener.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  PublisherFlush::PublisherFlush()
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  PublisherFlush::~PublisherFlush()
  {
    RTC_TRACE(("~PublisherFlush()"));
    // "consumer" should be deleted in the Connector
    m_consumer = nullptr;
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief initialization
   * @endif
   */
  DataPortStatus PublisherFlush::init(coil::Properties&  /*prop*/)
  {
    RTC_TRACE(("init()"));
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief InPortコンシューマのセット
   * @else
   * @brief Store InPort consumer
   * @endif
   */
  DataPortStatus
  PublisherFlush::setConsumer(InPortConsumer* consumer)
  {
    RTC_TRACE(("setConsumer()"));

    if (consumer == nullptr)
      {
        return DataPortStatus::INVALID_ARGS;
      }
    m_consumer = consumer;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief バッファのセット
   * @else
   * @brief Setting buffer pointer
   * @endif
   */
  DataPortStatus PublisherFlush::setBuffer(CdrBufferBase*  /*buffer*/)
  {
    RTC_TRACE(("setBuffer()"));

    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief リスナのセット
   * @else
   * @brief Setting buffer pointer
   * @endif
   */
  ::RTC::DataPortStatus
  PublisherFlush::setListener(ConnectorInfo& info,
                              RTC::ConnectorListenersBase* listeners)
  {
    RTC_TRACE(("setListeners()"));

    if (listeners == nullptr)
      {
        RTC_ERROR(("setListeners(listeners == 0): invalid argument"));
        return DataPortStatus::INVALID_ARGS;
      }

    m_profile = info;
    m_listeners = listeners;

    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief データを書き込む
   * @else
   * @brief Write data
   * @endif
   */
  DataPortStatus PublisherFlush::write(ByteDataStreamBase* data,
                                                  std::chrono::nanoseconds /* timeout */)
  {
    RTC_PARANOID(("write()"));

    if (m_consumer == nullptr) { return DataPortStatus::PRECONDITION_NOT_MET; }
    if (m_listeners == nullptr) { return DataPortStatus::PRECONDITION_NOT_MET; }

    if (m_retcode == DataPortStatus::CONNECTION_LOST)
      {
        RTC_DEBUG(("write(): connection lost."));
        return m_retcode;
      }
    m_data = *data;


    onSend(m_data);
    DataPortStatus ret(m_consumer->put(m_data));
    // consumer::put() returns
    //  {PORT_OK, PORT_ERROR, SEND_FULL, SEND_TIMEOUT, UNKNOWN_ERROR}

    switch (ret)
      {
      case DataPortStatus::PORT_OK:
        onReceived(m_data);
        return ret;
      case DataPortStatus::PORT_ERROR:
        onReceiverError(m_data);
        return ret;
      case DataPortStatus::SEND_FULL:
        onReceiverFull(m_data);
        return ret;
      case DataPortStatus::SEND_TIMEOUT:
        onReceiverTimeout(m_data);
        return ret;
      case DataPortStatus::CONNECTION_LOST:
        onReceiverTimeout(m_data);
        return ret;
      case DataPortStatus::UNKNOWN_ERROR:
        onReceiverError(m_data);
        return ret;
      case DataPortStatus::BUFFER_ERROR:         /* FALLTHROUGH */
      case DataPortStatus::BUFFER_FULL:          /* FALLTHROUGH */
      case DataPortStatus::BUFFER_EMPTY:         /* FALLTHROUGH */
      case DataPortStatus::BUFFER_TIMEOUT:       /* FALLTHROUGH */
      case DataPortStatus::RECV_EMPTY:           /* FALLTHROUGH */
      case DataPortStatus::RECV_TIMEOUT:         /* FALLTHROUGH */
      case DataPortStatus::INVALID_ARGS:         /* FALLTHROUGH */
      case DataPortStatus::PRECONDITION_NOT_MET: /* FALLTHROUGH */
      default:
        onReceiverError(m_data);
        return ret;
      }
  }

  /*!
   * @if jp
   * @brief アクティブ化確認
   * @else
   * @brief Confirm to activate
   * @endif
   */
  bool PublisherFlush::isActive()
  {
    return m_active;
  }

  /*!
   * @if jp
   * @brief アクティブ化
   * @else
   * @brief activation
   * @endif
   */
  DataPortStatus PublisherFlush::activate()
  {
    m_active = true;
    return DataPortStatus::PORT_OK;
  }

  /*!
   * @if jp
   * @brief 非アクティブ化
   * @else
   * @brief deactivation
   * @endif
   */
  DataPortStatus PublisherFlush::deactivate()
  {
    m_active = false;
    return DataPortStatus::PORT_OK;
  }

} // namespace RTC


extern "C"
{
  void PublisherFlushInit()
  {
    ::RTC::PublisherFactory::
      instance().addFactory("flush",
                            ::coil::Creator< ::RTC::PublisherBase,
                                             ::RTC::PublisherFlush>);
    // FSM4RTC formal/16-04-01 p.25
    // In the FSM4RTC specification, publisher type is defined as "io_mode"
    // Publisher "flush" is functionally equals to "io_mode: block".
    ::RTC::PublisherFactory::
      instance().addFactory("block",
                            ::coil::Creator< ::RTC::PublisherBase,
                                             ::RTC::PublisherFlush>);
  }
}
