// -*- C++ -*-

/*!
 * @file  SSMInPort.cpp
 * @brief SSMInPort class
 * @date  $Date: 2020-03-11 03:08:03 $
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


#include <rtm/Manager.h>
#include "SSMInPort.h"
#include <rtm/NVUtil.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <libssm.h>
#ifdef __cplusplus
}
#endif




namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  SSMInPort::SSMInPort(void)
   : m_buffer(nullptr),
     m_sens_sid(nullptr),
     m_stream_id(0)
  {
   rtclog.setName("SSMInPort");

  }

  

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  SSMInPort::~SSMInPort(void) = default;


  /*!
   * @if jp
   * @brief 設定初期化
   *
   * 
   * @param prop 設定情報
   *
   * @else
   *
   * @brief Initializing configuration
   *
   *
   * @param prop Configuration information
   *
   * @endif
   */
  void SSMInPort::init(coil::Properties& prop)
  {
    RTC_PARANOID(("SSMInPort::init()"));
    m_stream_name = prop.getProperty("ssm.stream_name", "sensor_test");
    std::string stream_id_str = prop.getProperty("ssm.stream_id", "0");
    if(!coil::stringTo(m_stream_id, stream_id_str.c_str()))
    {
      RTC_ERROR(("stream_id is invalid value"));
      throw std::bad_alloc();
    }

  }


  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void SSMInPort::
  setBuffer(CdrBufferBase* buffer)
  {
    m_buffer = buffer;
  }

  /*!
   * @if jp
   * @brief リスナを設定する
   * @else
   * @brief Set the listener
   * @endif
   */
  void SSMInPort::setListener(ConnectorInfo& info,
                                           ConnectorListenersBase* listeners)
  {
    RTC_TRACE(("SSMInPort::setListener()"));
    m_profile = info;
    m_listeners = listeners;
  }

  /*!
   * @if jp
   * @brief データを読み出す
   * @else
   * @brief Read data
   * @endif
   */
  DataPortStatus
  SSMInPort::get(ByteData& data)
  {
    ssmTimeT measured_time = 0.0;
    ssmTimeT time = 0.0;
    std::lock_guard<std::mutex> guard(m_mutex);
    if(m_sens_sid == nullptr)
    {
      m_sens_sid = openSSM(m_stream_name.c_str(), m_stream_id, 0);
      if(m_sens_sid == nullptr)
      {
        return DataPortStatus::PRECONDITION_NOT_MET;
      }
      return DataPortStatus::PORT_OK;
    }

    data.setDataLength(shm_get_address(m_sens_sid)->size);
    SSM_tid tid = readSSM_time(m_sens_sid, reinterpret_cast<char*>(data.getBuffer()), measured_time, &time);

    RTC_PARANOID(("data length: %d",  data.getDataLength()));
    RTC_PARANOID(("data count: %d",  tid));

    if (m_buffer->full())
    {
      RTC_INFO(("InPort buffer is full."));
      onBufferFull(data);
      onReceiverFull(data);
    }
    m_buffer->put(data);
    m_buffer->advanceWptr();
    m_buffer->advanceRptr();

    return DataPortStatus::PORT_OK;

  }



  /*!
   * @if jp
   * @brief データ受信通知への登録
   * @else
   * @brief Subscribe the data receive notification
   * @endif
   */
  bool SSMInPort::
  subscribeInterface(const SDOPackage::NVList& /*properties*/)
  {
    RTC_TRACE(("SSMInPort::subscribeInterface()"));
    return true;
  }

  /*!
   * @if jp
   * @brief データ受信通知からの登録解除
   * @else
   * @brief Unsubscribe the data receive notification
   * @endif
   */
  void SSMInPort::
  unsubscribeInterface(const SDOPackage::NVList& /*properties*/)
  {
    RTC_TRACE(("SSMInPort::unsubscribeInterface()"));
    closeSSM(&m_sens_sid);
  }


  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  void
  SSMInPort::convertReturnCode(DataPortStatus ret, ByteData&  /*data*/)
  {
    switch (ret)
      {
      case DataPortStatus::PORT_OK:
        break;
      case DataPortStatus::PORT_ERROR:
        onSenderError();
        break;
      case DataPortStatus::SEND_FULL:
        break;
      case DataPortStatus::BUFFER_FULL:
        break;
      case DataPortStatus::BUFFER_EMPTY:
        onSenderEmpty();
        break;
      case DataPortStatus::BUFFER_TIMEOUT:
        onSenderTimeout();
        break;
      case DataPortStatus::UNKNOWN_ERROR:
      case DataPortStatus::BUFFER_ERROR:
      case DataPortStatus::SEND_TIMEOUT:
      case DataPortStatus::RECV_EMPTY:
      case DataPortStatus::RECV_TIMEOUT:
      case DataPortStatus::INVALID_ARGS:
      case DataPortStatus::PRECONDITION_NOT_MET:
      case DataPortStatus::CONNECTION_LOST:
      default:
        onSenderError();
        break;
      }
  }

} // namespace RTC
