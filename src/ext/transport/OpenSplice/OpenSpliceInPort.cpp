// -*- C++ -*-

/*!
 * @file  OpenSpliceInPort.cpp
 * @brief OpenSpliceInPort class
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


#include "OpenSpliceInPort.h"


#include <rtm/BufferBase.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>
#include <rtm/SystemLogger.h>
#include <coil/UUID.h>
#include "OpenSpliceMessageInfo.h"








namespace RTC
{


  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OpenSpliceInPort::OpenSpliceInPort(void)
   : m_buffer(nullptr), m_inport(nullptr)
  {
    // PortProfile setting
    setInterfaceType("opensplice");
    // ConnectorProfile setting

  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OpenSpliceInPort::~OpenSpliceInPort(void)
  {
    RTC_PARANOID(("~OpenSpliceInPort()"));

    if (m_inport != nullptr)
    {
        m_inport->finalize();
    }
  }


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
  void OpenSpliceInPort::init(coil::Properties& prop)
  {
      
    RTC_PARANOID(("OpenSpliceInPort::init()"));

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

    m_inport = createOpenSpliceInPort(this, dataType, idlPath, topic, prop.getNode("opensplice"), endian, corbamode);

    if (m_inport == nullptr)
    {
        RTC_ERROR(("Failed initialize inport"));
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
  void OpenSpliceInPort::
  setBuffer(BufferBase<ByteData>* buffer)
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
  void OpenSpliceInPort::setListener(ConnectorInfo& info,
                                           ConnectorListenersBase* listeners)
  {
    m_profile = info;
    m_listeners = listeners;
  }

  /*!
   * @if jp
   * @brief Connectorを設定する。
   * @else
   * @brief set Connector
   * @endif
   */
  void OpenSpliceInPort::setConnector(InPortConnector* connector)
  {
    m_connector = connector;
  }

  /*!
   * @if jp
   * @brief 受信データをバッファに書き込む
   *
   *
   *
   * @param cdr データ
   *
   * @else
   * @brief
   *
   * @param cdr
   *
   *
   * @endif
   */
  void OpenSpliceInPort::put(RTC::ByteData& data)
  {
      RTC_PARANOID(("OpenSpliceInPort::put()"));
      RTC_VERBOSE(("read data length:%d", data.getDataLength()));

      if (m_connector == NULL)
      {
          onReceiverError(data);
      }
      else
      {
          
          onReceived(data);
          BufferStatus ret = m_connector->write(data);
          convertReturn(ret, data);
      }

  }

  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  void OpenSpliceInPort::convertReturn(BufferStatus status,
                                        ByteData& data)
  {
    switch (status)
      {
      case BufferStatus::OK:
        onBufferWrite(data);
        return;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        return;

      case BufferStatus::FULL:
        onBufferFull(data);
        onReceiverFull(data);
        return;

      case BufferStatus::EMPTY:
        // never come here
        return;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        return;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        return;

      case BufferStatus::NOT_SUPPORTED:
        onReceiverError(data);
        return;

      default:
        onReceiverError(data);
        return;
      }
  }


} // namespace RTC
