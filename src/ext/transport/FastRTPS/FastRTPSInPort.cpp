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
   : m_buffer(nullptr), m_listeners(nullptr), m_connector(nullptr), m_listener(this)
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

    FastRTPSManager& topicmgr = FastRTPSManager::instance();
    eprosima::fastrtps::Participant* participant = topicmgr.getParticipant();

    if (participant == nullptr)
    {
        RTC_ERROR(("Can not initialize Fast-RTPS"));
        return;
    }

    

    std::string marshaling_type = prop.getProperty("marshaling_type", "corba");
    coil::Properties& fastrtps_prop = prop.getNode("fast-rtps");
    m_topic = fastrtps_prop.getProperty("topic", "chatter");
    

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

    std::string subscriber_name = fastrtps_prop.getProperty("subscriber.name");
    eprosima::fastrtps::SubscriberAttributes Rparam;
    if(subscriber_name.empty())
    {
      
      Rparam.topic.topicKind = eprosima::fastrtps::rtps::NO_KEY;
      Rparam.topic.topicDataType = m_dataType;
      Rparam.topic.topicName = m_topic;
      
      //setSubParam(fastrtps_prop, Rparam);
      
    }
    else
    {
      RTC_INFO(("Subscriber name:", subscriber_name.c_str()));
      if(eprosima::fastrtps::xmlparser::XMLP_ret::XML_ERROR == eprosima::fastrtps::xmlparser::XMLProfileManager::fillSubscriberAttributes(subscriber_name, Rparam))
      {
        RTC_ERROR(("xml file load failed"));
        throw;
      }
      Rparam.topic.topicDataType = m_dataType;
      //Rparam.topic.topicName = m_topic;
      //Rparam.qos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
    }

   /*
    RTC_DEBUG(("ReaderQos setting: subscriber.deadline.period: sec=%d nanosec=%u", Rparam.qos.m_deadline.period.seconds, Rparam.qos.m_deadline.period.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.destinationOrder.kind: %hhu", Rparam.qos.m_destinationOrder.kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.disablePositiveACKs.enabled: %s", (Rparam.qos.m_disablePositiveACKs.enabled ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.disablePositiveACKs.duration: sec=%d nanosec=%u", Rparam.qos.m_disablePositiveACKs.duration.seconds, Rparam.qos.m_disablePositiveACKs.duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.durability.kind: %hhu", Rparam.qos.m_durability.kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.history_depth: %d", Rparam.qos.m_durabilityService.history_depth));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.history_kind: %hhu", Rparam.qos.m_durabilityService.history_kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.max_instances: %d", Rparam.qos.m_durabilityService.max_instances));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.max_samples: %d", Rparam.qos.m_durabilityService.max_samples));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.max_samples_per_instance: %d", Rparam.qos.m_durabilityService.max_samples_per_instance));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.service_cleanup_delay: sec=%d nanosec=%u", Rparam.qos.m_durabilityService.service_cleanup_delay.seconds, Rparam.qos.m_durabilityService.service_cleanup_delay.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.latencyBudget.duration: sec=%d nanosec=%u", Rparam.qos.m_latencyBudget.duration.seconds, Rparam.qos.m_latencyBudget.duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.lifespan.duration: sec=%d nanosec=%u", Rparam.qos.m_lifespan.duration.seconds, Rparam.qos.m_lifespan.duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.liveliness.announcement_period: sec=%d nanosec=%u", Rparam.qos.m_liveliness.announcement_period.seconds, Rparam.qos.m_liveliness.announcement_period.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.liveliness.kind: %hhu", Rparam.qos.m_liveliness.kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.liveliness.lease_duration: sec=%d nanosec=%u", Rparam.qos.m_liveliness.lease_duration.seconds, Rparam.qos.m_liveliness.lease_duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.ownership.kind: %hhu", Rparam.qos.m_ownership.kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.presentation.access_scope: %hhu", Rparam.qos.m_presentation.access_scope));
    RTC_DEBUG(("ReaderQos setting: subscriber.presentation.coherent_access: %s", (Rparam.qos.m_presentation.coherent_access ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.presentation.ordered_access: %s", (Rparam.qos.m_presentation.ordered_access ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.reliability.kind: %hhu", Rparam.qos.m_reliability.kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.reliability.max_blocking_time: sec=%d nanosec=%u", Rparam.qos.m_reliability.max_blocking_time.seconds, Rparam.qos.m_reliability.max_blocking_time.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.timeBasedFilter.minimum_separation: sec=%d nanosec=%u", Rparam.qos.m_timeBasedFilter.minimum_separation.seconds, Rparam.qos.m_timeBasedFilter.minimum_separation.nanosec));
#if (FASTRTPS_VERSION_MAJOR <= 1)
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.force_type_validation: %s", (Rparam.qos.m_typeConsistency.m_force_type_validation ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_member_names: %s", (Rparam.qos.m_typeConsistency.m_ignore_member_names ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_sequence_bounds: %s", (Rparam.qos.m_typeConsistency.m_ignore_sequence_bounds ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_string_bounds: %s", (Rparam.qos.m_typeConsistency.m_ignore_string_bounds ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.kind: %hhu", Rparam.qos.m_typeConsistency.m_kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.prevent_type_widening: %s", (Rparam.qos.m_typeConsistency.m_prevent_type_widening ? "true" : "false")));
#else
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.force_type_validation: %s", (Rparam.qos.type_consistency.m_force_type_validation ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_member_names: %s", (Rparam.qos.type_consistency.m_ignore_member_names ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_sequence_bounds: %s", (Rparam.qos.type_consistency.m_ignore_sequence_bounds ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_string_bounds: %s", (Rparam.qos.type_consistency.m_ignore_string_bounds ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.kind: %hhu", Rparam.qos.type_consistency.m_kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.prevent_type_widening: %s", (Rparam.qos.type_consistency.m_prevent_type_widening ? "true" : "false")));
#endif
    RTC_DEBUG(("ReaderQos setting: subscriber.history_memory_policy: %d", Rparam.historyMemoryPolicy));
   */


    m_subscriber = eprosima::fastrtps::Domain::createSubscriber(participant, Rparam, (eprosima::fastrtps::SubscriberListener*)&m_listener);

    if(m_subscriber == nullptr)
    {
      RTC_ERROR(("Subscriber initialize failed"));
      throw;
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

  /*!
   * @if jp
   * @brief プロパティからfastrtps::Duration_tを設定する
   *
   * @param prop プロパティ(seconds、nanosecの要素に値を格納する)
   * @param time fastrtps::Duration_t
   *
   * @else
   * @brief
   *
   * @param prop
   * @param time
   *
   *
   * @endif
   */
#if (FASTRTPS_VERSION_MAJOR <= 1) && (FASTRTPS_VERSION_MINOR <= 7)
  void FastRTPSInPort::setDuration(coil::Properties& prop, eprosima::fastrtps::rtps::Duration_t& time)
#else
  void FastRTPSInPort::setDuration(coil::Properties& prop, eprosima::fastrtps::Duration_t& time)
#endif
  {
    std::string sec_str = prop["seconds"];
    std::string nanosec_str = prop["nanosec"];


    coil::stringTo<int32_t>(time.seconds, sec_str.c_str());
    coil::stringTo<uint32_t>(time.nanosec, nanosec_str.c_str());
  }

  void FastRTPSInPort::setSubParam(coil::Properties& fastrtps_prop, eprosima::fastrtps::SubscriberAttributes& Rparam)
  {
      setDuration(fastrtps_prop.getNode("subscriber.qos.deadline.period"), Rparam.qos.m_deadline.period);

      std::string destinationOrder_kind = fastrtps_prop.getProperty("subscriber.qos.destinationOrder.kind", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS");

      if (destinationOrder_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        Rparam.qos.m_destinationOrder.kind = eprosima::fastrtps::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
      }
      else if (destinationOrder_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        Rparam.qos.m_destinationOrder.kind = eprosima::fastrtps::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
      }

      Rparam.qos.m_disablePositiveACKs.enabled = coil::toBool(fastrtps_prop["subscriber.qos.disablePositiveACKs.enabled"], "YES", "NO", Rparam.qos.m_disablePositiveACKs.enabled);

      setDuration(fastrtps_prop.getNode("subscriber.qos.disablePositiveACKs.duration"), Rparam.qos.m_disablePositiveACKs.duration);

      std::string durability_kind = fastrtps_prop.getProperty("subscriber.qos.durability.kind", "VOLATILE_DURABILITY_QOS");

      if (durability_kind == "VOLATILE_DURABILITY_QOS")
      {
        Rparam.qos.m_durability.kind = eprosima::fastrtps::VOLATILE_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
      {
        Rparam.qos.m_durability.kind = eprosima::fastrtps::TRANSIENT_LOCAL_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
      {
        Rparam.qos.m_durability.kind = eprosima::fastrtps::TRANSIENT_DURABILITY_QOS;
      }
      else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
      {
        Rparam.qos.m_durability.kind = eprosima::fastrtps::PERSISTENT_DURABILITY_QOS;
      }

      coil::stringTo<int32_t>(Rparam.qos.m_durabilityService.history_depth, fastrtps_prop["subscriber.qos.durabilityService.history_depth"].c_str());


      std::string durabilityService_history_kind = fastrtps_prop.getProperty("subscriber.qos.durabilityService.history_kind", "KEEP_LAST_HISTORY_QOS");

      if (durabilityService_history_kind == "KEEP_LAST_HISTORY_QOS")
      {
        Rparam.qos.m_durabilityService.history_kind = eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
      }
      else if (durabilityService_history_kind == "KEEP_ALL_HISTORY_QOS")
      {
        Rparam.qos.m_durabilityService.history_kind = eprosima::fastrtps::KEEP_ALL_HISTORY_QOS;
      }

      coil::stringTo<int32_t>(Rparam.qos.m_durabilityService.max_instances, fastrtps_prop["subscriber.qos.durabilityService.max_instances"].c_str());
      coil::stringTo<int32_t>(Rparam.qos.m_durabilityService.max_samples, fastrtps_prop["subscriber.qos.durabilityService.max_samples"].c_str());
      coil::stringTo<int32_t>(Rparam.qos.m_durabilityService.max_samples_per_instance, fastrtps_prop["subscriber.qos.durabilityService.max_samples_per_instance"].c_str());

      setDuration(fastrtps_prop.getNode("subscriber.qos.durabilityService.service_cleanup_delay"), Rparam.qos.m_durabilityService.service_cleanup_delay);



      //Rparam.qos.m_groupData

      setDuration(fastrtps_prop.getNode("subscriber.qos.latencyBudget.duration"), Rparam.qos.m_latencyBudget.duration);

      setDuration(fastrtps_prop.getNode("subscriber.qos.lifespan.duration"), Rparam.qos.m_lifespan.duration);

      setDuration(fastrtps_prop.getNode("subscriber.qos.liveliness.announcement_period"), Rparam.qos.m_liveliness.announcement_period);

      std::string liveliness_kind = fastrtps_prop.getProperty("subscriber.qos.liveliness.kind", "AUTOMATIC_LIVELINESS_QOS");

      if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
      {
        Rparam.qos.m_liveliness.kind = eprosima::fastrtps::AUTOMATIC_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
      {
        Rparam.qos.m_liveliness.kind = eprosima::fastrtps::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
      {
        Rparam.qos.m_liveliness.kind = eprosima::fastrtps::MANUAL_BY_TOPIC_LIVELINESS_QOS;
      }

      setDuration(fastrtps_prop.getNode("subscriber.qos.liveliness.lease_duration"), Rparam.qos.m_liveliness.lease_duration);


      std::string ownership_kind = fastrtps_prop.getProperty("subscriber.qos.ownership.kind", "SHARED_OWNERSHIP_QOS");

      if (ownership_kind == "SHARED_OWNERSHIP_QOS")
      {
        Rparam.qos.m_ownership.kind = eprosima::fastrtps::SHARED_OWNERSHIP_QOS;
      }
      else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
      {
        Rparam.qos.m_ownership.kind = eprosima::fastrtps::EXCLUSIVE_OWNERSHIP_QOS;
      }


      //Rparam.qos.m_partition


      std::string presentation_access_scope = fastrtps_prop.getProperty("subscriber.qos.presentation.access_scope", "INSTANCE_PRESENTATION_QOS");

      if (presentation_access_scope == "INSTANCE_PRESENTATION_QOS")
      {
        Rparam.qos.m_presentation.access_scope = eprosima::fastrtps::INSTANCE_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "TOPIC_PRESENTATION_QOS")
      {
        Rparam.qos.m_presentation.access_scope = eprosima::fastrtps::TOPIC_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "GROUP_PRESENTATION_QOS")
      {
        Rparam.qos.m_presentation.access_scope = eprosima::fastrtps::GROUP_PRESENTATION_QOS;
      }

      Rparam.qos.m_presentation.coherent_access = coil::toBool(fastrtps_prop["subscriber.qos.presentation.coherent_access"], "YES", "NO", Rparam.qos.m_presentation.coherent_access);
      Rparam.qos.m_presentation.ordered_access = coil::toBool(fastrtps_prop["subscriber.qos.presentation.ordered_access"], "YES", "NO", Rparam.qos.m_presentation.ordered_access);

      std::string reliability_kind = fastrtps_prop.getProperty("subscriber.qos.reliability.kind", "BEST_EFFORT_RELIABILITY_QOS");

      if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
      {
        Rparam.qos.m_reliability.kind = eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS;
      }
      else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
      {
        Rparam.qos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
      }


      setDuration(fastrtps_prop.getNode("subscriber.qos.reliability.max_blocking_time"), Rparam.qos.m_reliability.max_blocking_time);

      setDuration(fastrtps_prop.getNode("subscriber.qos.timeBasedFilter.minimum_separation"), Rparam.qos.m_timeBasedFilter.minimum_separation);


      //Rparam.qos.m_topicData
      //Rparam.qos.m_userData
      //Rparam.qos.representation.m_value
      
#if (FASTRTPS_VERSION_MAJOR <= 1)
      Rparam.qos.m_typeConsistency.m_force_type_validation = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.force_type_validation"], "YES", "NO", Rparam.qos.m_typeConsistency.m_force_type_validation);
#else
      Rparam.qos.type_consistency.m_force_type_validation = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.force_type_validation"], "YES", "NO", Rparam.qos.type_consistency.m_force_type_validation);
#endif

#if (FASTRTPS_VERSION_MAJOR <= 1)
      Rparam.qos.m_typeConsistency.m_ignore_member_names = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.ignore_member_names"], "YES", "NO", Rparam.qos.m_typeConsistency.m_ignore_member_names);
#else
      Rparam.qos.type_consistency.m_ignore_member_names = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.ignore_member_names"], "YES", "NO", Rparam.qos.type_consistency.m_ignore_member_names);
#endif

#if (FASTRTPS_VERSION_MAJOR <= 1)
      Rparam.qos.m_typeConsistency.m_ignore_sequence_bounds = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.ignore_sequence_bounds"], "YES", "NO", Rparam.qos.m_typeConsistency.m_ignore_sequence_bounds);
#else
      Rparam.qos.type_consistency.m_ignore_sequence_bounds = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.ignore_sequence_bounds"], "YES", "NO", Rparam.qos.type_consistency.m_ignore_sequence_bounds);
#endif

#if (FASTRTPS_VERSION_MAJOR <= 1)
      Rparam.qos.m_typeConsistency.m_ignore_string_bounds = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.ignore_string_bounds"], "YES", "NO", Rparam.qos.m_typeConsistency.m_ignore_string_bounds);
#else
      Rparam.qos.type_consistency.m_ignore_string_bounds = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.ignore_string_bounds"], "YES", "NO", Rparam.qos.type_consistency.m_ignore_string_bounds);
#endif

      std::string type_consistency_kind = fastrtps_prop.getProperty("subscriber.qos.type_consistency.kind", "ALLOW_TYPE_COERCION");

      if (type_consistency_kind == "DISALLOW_TYPE_COERCION")
      {
#if (FASTRTPS_VERSION_MAJOR <= 1)
        Rparam.qos.m_typeConsistency.m_kind
#else
        Rparam.qos.type_consistency.m_kind
#endif
            = eprosima::fastrtps::DISALLOW_TYPE_COERCION;
      }
      else if (type_consistency_kind == "ALLOW_TYPE_COERCION")
      {
#if (FASTRTPS_VERSION_MAJOR <= 1)
        Rparam.qos.m_typeConsistency.m_kind
#else
        Rparam.qos.type_consistency.m_kind
#endif
            = eprosima::fastrtps::ALLOW_TYPE_COERCION;
      }

#if (FASTRTPS_VERSION_MAJOR <= 1)
      Rparam.qos.m_typeConsistency.m_prevent_type_widening = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.prevent_type_widening"], "YES", "NO", Rparam.qos.m_typeConsistency.m_prevent_type_widening);
#else
      Rparam.qos.type_consistency.m_prevent_type_widening = coil::toBool(fastrtps_prop["subscriber.qos.type_consistency.prevent_type_widening"], "YES", "NO", Rparam.qos.type_consistency.m_prevent_type_widening);
#endif


      std::string history_memory_policy = fastrtps_prop.getProperty("subscriber.history_memory_policy", "PREALLOCATED_WITH_REALLOC_MEMORY_MODE");
      if (history_memory_policy == "PREALLOCATED_MEMORY_MODE")
      {
        Rparam.historyMemoryPolicy = eprosima::fastrtps::rtps::PREALLOCATED_MEMORY_MODE;
      }
      else if (history_memory_policy == "PREALLOCATED_WITH_REALLOC_MEMORY_MODE")
      {
        Rparam.historyMemoryPolicy = eprosima::fastrtps::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
      }
      else if (history_memory_policy == "DYNAMIC_RESERVE_MEMORY_MODE")
      {
        Rparam.historyMemoryPolicy = eprosima::fastrtps::rtps::DYNAMIC_RESERVE_MEMORY_MODE;
      }
#if (FASTRTPS_VERSION_MAJOR > 1)
      else if (history_memory_policy == "DYNAMIC_REUSABLE_MEMORY_MODE")
      {
        Rparam.historyMemoryPolicy = eprosima::fastrtps::rtps::DYNAMIC_REUSABLE_MEMORY_MODE;
      }
#endif

      coil::stringTo<int32_t>(Rparam.topic.historyQos.depth, fastrtps_prop["subscriber.topic.historyQos.depth"].c_str());

      std::string topic_historyQos_kind = fastrtps_prop.getProperty("subscriber.topic.historyQos.kind", "KEEP_LAST_HISTORY_QOS");
      if (history_memory_policy == "KEEP_LAST_HISTORY_QOS")
      {
        Rparam.topic.historyQos.kind = eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
      }
      else if (history_memory_policy == "KEEP_ALL_HISTORY_QOS")
      {
        Rparam.topic.historyQos.kind = eprosima::fastrtps::KEEP_ALL_HISTORY_QOS;
      }
  }

} // namespace RTC
