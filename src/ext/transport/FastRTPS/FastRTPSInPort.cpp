// -*- C++ -*-

/*!
 * @file  FastRTPSInPort.cpp
 * @brief FastRTPSInPort class
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


#include "FastRTPSInPort.h"
#include <coil/UUID.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastrtps/Domain.h>
#include "FastRTPSManager.h"
#include "FastRTPSMessageInfo.h"








namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  FastRTPSInPort::FastRTPSInPort(void)
   : m_buffer(nullptr), m_listener(this)
  {
    // PortProfile setting
    setInterfaceType("fast-rtps");
    // ConnectorProfile setting

  }

  

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  FastRTPSInPort::~FastRTPSInPort(void)
  {
    RTC_PARANOID(("~FastRTPSInPort()"));

    if (m_subscriber != nullptr)
    {
        eprosima::fastrtps::Domain::removeSubscriber(m_subscriber);
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
  void FastRTPSInPort::init(coil::Properties& prop)
  {
      
    RTC_PARANOID(("FastRTPSInPort::init()"));

    if(prop.propertyNames().size() == 0)
    {
      RTC_DEBUG(("Property is empty."));
      return;
    }

    std::string profile_xml = prop.getProperty("fastrtps.QoSXML", "");
    FastRTPSManager& topicmgr = FastRTPSManager::instance(profile_xml);
    eprosima::fastrtps::Participant* participant = topicmgr.getParticipant();

    if (participant == nullptr)
    {
        RTC_ERROR(("Can not initialize Fast-RTPS"));
        return;
    }

    

    std::string marshaling_type = prop.getProperty("marshaling_type", "corba");
    m_topic = prop.getProperty("fastrtps.topic", "chatter");
    

    const std::string str_corba = "corba";

    bool is_serializer_cdr = !(marshaling_type.compare(0, str_corba.size(), str_corba) != 0);

    if (!is_serializer_cdr)
    {
        FastRTPSMessageInfoBase* info = GlobalFastRTPSMessageInfoList::instance().getInfo(marshaling_type);

        if (!info)
        {
            RTC_ERROR(("Can not find message type(%s)", marshaling_type.c_str()));
            return;
        }

        m_dataType = info->data_type();
        m_topic = info->topic_name(m_topic);
    }
    else
    {
        std::string data = prop.getProperty("data_type", "IDL:RTC/CDR_Data:1.0");

        coil::vstring typelist = coil::split(data, ":");
        if (typelist.size() == 3)
        {
            m_dataType = coil::replaceString(std::move(typelist[1]), "/", "::");
        }
        else
        {
            m_dataType = data;
        }
    }

    if (!topicmgr.registeredType(m_dataType.c_str()))
    {
        CORBACdrDataPubSubType* type = new CORBACdrDataPubSubType();
        if (!is_serializer_cdr)
        {
            type->init(m_dataType, true);
        }
        else
        {
            type->init(m_dataType, false);
        }

        std::string endian_type{coil::normalize(
          prop.getProperty("serializer.cdr.endian", ""))};
        std::vector<std::string> endian(coil::split(endian_type, ","));
        if (endian[0] == "little")
        {
            type->setEndian(true);
        }
        else if (endian[0] == "big")
        {
            type->setEndian(false);
        }


        topicmgr.registerType(type);
    }

    std::string subscriber_name = prop.getProperty("fastrtps.subscriber.name");
    if(subscriber_name.empty())
    {
      eprosima::fastrtps::SubscriberAttributes *Rparam = new eprosima::fastrtps::SubscriberAttributes();
      Rparam->topic.topicKind = eprosima::fastrtps::rtps::NO_KEY;
      Rparam->topic.topicDataType = m_dataType;
      Rparam->topic.topicName = m_topic;
      Rparam->historyMemoryPolicy = eprosima::fastrtps::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
      Rparam->qos.m_reliability.kind = eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS;
      m_subscriber = eprosima::fastrtps::Domain::createSubscriber(participant,*Rparam,(eprosima::fastrtps::SubscriberListener*)&m_listener);
      delete Rparam;
    }
    else
    {
      eprosima::fastrtps::SubscriberAttributes *Rparam = new eprosima::fastrtps::SubscriberAttributes();
      if(eprosima::fastrtps::xmlparser::XMLP_ret::XML_ERROR == eprosima::fastrtps::xmlparser::XMLProfileManager::fillSubscriberAttributes(subscriber_name, *Rparam))
      {
        RTC_ERROR(("xml file load failed"));
        delete Rparam;
        return;
      }
      Rparam->topic.topicDataType = m_dataType;
      Rparam->topic.topicName = m_topic;
      Rparam->qos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
      m_subscriber = eprosima::fastrtps::Domain::createSubscriber(participant,*Rparam,(eprosima::fastrtps::SubscriberListener*)&m_listener);
      delete Rparam;
    }
    if(m_subscriber == nullptr)
    {
        return;
    }

    

  }


  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void FastRTPSInPort::
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
  void FastRTPSInPort::setListener(ConnectorInfo& info,
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
  void FastRTPSInPort::setConnector(InPortConnector* connector)
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
  void FastRTPSInPort::put(ByteData& cdr)
  {
      RTC_PARANOID(("FastRTPSInPort::put()"));
      RTC_VERBOSE(("read data length:%d", cdr.getDataLength()));
      if (m_connector == nullptr)
      {
          onReceiverError(cdr);
      }
      else
      {
          onReceived(cdr);
          BufferStatus ret = m_connector->write(cdr);
          convertReturn(ret, cdr);
      }

  }

  /*!
   * @if jp
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   * @else
   * @brief Constructor
   *
   * Constructor
   *
   * @endif
   */
  FastRTPSInPort::SubListener::SubListener(FastRTPSInPort* provider) : rtclog("FastRTPSInPort::SubListener"), m_provider(provider)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   *
   * デストラクタ
   *
   * @else
   * @brief Destructor
   *
   * Destructor
   *
   * @endif
   */
  FastRTPSInPort::SubListener::~SubListener()
  {
  }

  /*!
   * @if jp
   * @brief
   *
   * 同じトピックのPublisherを検出したときのコールバック関数
   *
   * @param sub Subscriber
   * @param info 一致情報
   *
   * @else
   * @brief
   *
   * @param sub
   * @param info
   *
   *
   * @endif
   */
  void FastRTPSInPort::SubListener::onSubscriptionMatched(eprosima::fastrtps::Subscriber* /*sub*/, eprosima::fastrtps::rtps::MatchingInfo& /*info*/)
  {
  }
  /*!
   * @if jp
   * @brief
   *
   * 新規にメッセージを取得したときのコールバック関数
   *
   * @param sub Subscriber
   *
   * @else
   * @brief
   *
   * @param sub
   *
   *
   * @endif
   */
  void FastRTPSInPort::SubListener::onNewDataMessage(eprosima::fastrtps::Subscriber* sub)
  {
    RTC_PARANOID(("onNewDataMessage()"));

    RTC_PARANOID(("takeNextData"));
    if(sub->takeNextData(&m_data, &m_info))
    {
      if(m_info.sampleKind == eprosima::fastrtps::rtps::ALIVE)
      {
          m_provider->put(m_data);
      }
    }
  }


  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  void FastRTPSInPort::convertReturn(BufferStatus status,
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

      default:
        onReceiverError(data);
        return;
      }
  }


} // namespace RTC
