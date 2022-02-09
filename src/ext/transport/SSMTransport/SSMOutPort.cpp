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

#include <rtm/NVUtil.h>
#include <coil/UUID.h>
#include "SSMOutPort.h"



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
  SSMOutPort::SSMOutPort(void)
    : rtclog("SSMOutPort"),
      m_sens_sid(nullptr),
      m_stream_id(0),
      m_stream_size(0),
      m_life_ssm_time(5.0),
      m_cycle_ssm_time(0.05)
  {
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  SSMOutPort::~SSMOutPort(void)
  {
    RTC_PARANOID(("~SSMOutPort()"));
    
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void SSMOutPort::init(coil::Properties& prop)
  { 
    RTC_PARANOID(("SSMOutPort::init()"));
    
    m_stream_name = prop.getProperty("ssm.stream_name", "sensor_test");
    std::string stream_id_str = prop.getProperty("ssm.stream_id", "0");
    if(!coil::stringTo(m_stream_id, stream_id_str.c_str()))
    {
      RTC_ERROR(("stream_id is invalid value"));
    }
    std::string stream_size_str = prop.getProperty("ssm.stream_size", "0");
    if(!coil::stringTo(m_stream_size, stream_size_str.c_str()))
    {
      RTC_ERROR(("stream_size is invalid value"));
    }
    std::string life_ssm_time_str = prop.getProperty("ssm.life_ssm_time", "5.0");
    if(!coil::stringTo(m_life_ssm_time, life_ssm_time_str.c_str()))
    {
      RTC_ERROR(("life_ssm_time is invalid value"));
    }
    std::string cycle_ssm_time_str = prop.getProperty("ssm.cycle_ssm_time", "0.05");
    if(!coil::stringTo(m_cycle_ssm_time, cycle_ssm_time_str.c_str()))
    {
      RTC_ERROR(("cycle_ssm_time is invalid value"));
    }

    if(m_stream_size > 0)
    {
      m_sens_sid = createSSM_time(m_stream_name.c_str(), m_stream_id, m_stream_size, m_life_ssm_time, m_cycle_ssm_time);
    }

  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus SSMOutPort::put(ByteData& data)
  {
    RTC_PARANOID(("put()"));
    RTC_VERBOSE(("Data size:%d", data.getDataLength()));
    std::lock_guard<std::mutex> guard(m_mutex);
    if(m_sens_sid == nullptr)
    {
      m_sens_sid = createSSM_time(m_stream_name.c_str(), m_stream_id, data.getDataLength(), m_life_ssm_time, m_cycle_ssm_time);
      if(m_sens_sid == nullptr)
      {
        return DataPortStatus::PRECONDITION_NOT_MET;
      }
    }
    ssmTimeT measured_time = 0;
    writeSSM(m_sens_sid, reinterpret_cast<char*>(data.getBuffer()), measured_time);
    return DataPortStatus::PORT_OK;
    
  }

  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void SSMOutPort::
  publishInterfaceProfile(SDOPackage::NVList& /*properties*/)
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
  bool SSMOutPort::
  subscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    
    
    return true;
  }
  
  /*!
   * @if jp
   * @brief データ送信通知からの登録解除
   * @else
   * @brief Unsubscribe the data send notification
   * @endif
   */
  void SSMOutPort::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));

    releaseSSM(&m_sens_sid);
  }
  
} // namespace RTC
