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




#include <rtm/NVUtil.h>
#include <rtm/SystemLogger.h>
#include <coil/UUID.h>
#include <coil/stringutil.h>
#include "OpenSpliceOutPort.h"
#include "OpenSpliceMessageInfo.h"





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
  OpenSpliceOutPort::OpenSpliceOutPort(void)
    : rtclog("OpenSpliceOutPort"), m_outport(nullptr)
  {
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OpenSpliceOutPort::~OpenSpliceOutPort(void)
  {
    RTC_PARANOID(("~OpenSpliceOutPort()"));
    
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void OpenSpliceOutPort::init(coil::Properties& prop)
  { 
    RTC_PARANOID(("OpenSpliceOutPort::init()"));
    
    if(prop.propertyNames().size() == 0)
    {
      RTC_DEBUG(("Property is empty."));
      return;
    }

    std::string marshaling_type = prop.getProperty("marshaling_type", "corba");
    std::string topic = prop.getProperty("opensplice.topic", "chatter");


    std::string dataname;
    bool corbamode;

    const std::string str_corba = "corba";

    if (marshaling_type.compare(0, str_corba.size(), str_corba) != 0)
    {
        dataname = marshaling_type;
        corbamode = false;
    }
    else
    {
        dataname = prop.getProperty("data_type", "RTC::CDR_Data");
        corbamode = true;
    }


    OpenSpliceMessageInfoBase* info = GlobalOpenSpliceMessageInfoList::instance().getInfo(dataname);

    if (!info)
    {
        RTC_ERROR(("Can not find message type(%s)", marshaling_type.c_str()));
        throw std::bad_alloc();
    }

    std::string dataType = info->data_type();
    topic = info->topic_name(topic);
    std::string idlPath = info->idl_path();

    std::string endian_type{coil::normalize(
      prop.getProperty("serializer.cdr.endian", ""))};
    std::vector<std::string> endian_str(coil::split(endian_type, ","));

    bool endian = true;

    if (endian_str[0] == "little")
    {
        endian = true;
    }
    else if (endian_str[0] == "big")
    {
        endian = false;
    }

    m_outport = createOpenSpliceOutPort(dataType, idlPath, topic, prop.getNode("opensplice"), endian, corbamode);

    if(m_outport == nullptr)
    {
        RTC_ERROR(("Failed initialize writer"));
        throw std::bad_alloc();
    }
  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus OpenSpliceOutPort::put(ByteData& data)
  {
    
    RTC_PARANOID(("put()"));
    RTC_VERBOSE(("Data size:%d", data.getDataLength()));

    if (m_outport == nullptr)
    {
        RTC_ERROR(("Failed write data"));
    }
    if (m_outport->write(data))
    {
        return DataPortStatus::PORT_OK;
    }
    else
    {
        return DataPortStatus::PORT_ERROR;
    }

    
    
  }

  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void OpenSpliceOutPort::
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
  bool OpenSpliceOutPort::
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
  void OpenSpliceOutPort::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    
    if (m_outport != nullptr)
    {
        m_outport->finalize();
    }
  }
  
} // namespace RTC
