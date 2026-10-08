// -*- C++ -*-
/*!
 * @file  FastDDSOutPort.cpp
 * @brief FastDDSOutPort class
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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4819)
#endif

#include <rtm/NVUtil.h>
#include <coil/UUID.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif
#include "FastDDSOutPort.h"
#include "FastDDSManager.h"
#include "FastDDSMessageInfo.h"
#include <stdexcept>
#include <vector>
#include <string>





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
  FastDDSOutPort::FastDDSOutPort(void)
    : rtclog("FastDDSOutPort")
  {
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  FastDDSOutPort::~FastDDSOutPort(void)
  {
    RTC_PARANOID(("~FastDDSOutPort()"));

    auto* participant = FastDDSManager::instance().getParticipant();
    if (participant == nullptr)
    {
        return;
    }

    if (m_writer != nullptr && m_publisher != nullptr)
    {
        if (m_publisher->delete_datawriter(m_writer)
            != eprosima::fastdds::dds::RETCODE_OK)
        {
            RTC_ERROR(("Failed to delete DataWriter"));
            return;
        }
        m_writer = nullptr;
    }

    if (m_publisher != nullptr)
    {
        if (participant->delete_publisher(m_publisher)
            != eprosima::fastdds::dds::RETCODE_OK)
        {
            RTC_ERROR(("Failed to delete Publisher"));
            return;
        }
        m_publisher = nullptr;
    }

    if (m_dds_topic != nullptr)
    {
        if (participant->delete_topic(m_dds_topic)
            != eprosima::fastdds::dds::RETCODE_OK)
        {
            RTC_ERROR(("Failed to delete Topic"));
            return;
        }
        m_dds_topic = nullptr;
    }    
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void FastDDSOutPort::init(coil::Properties& prop)
  { 
    RTC_PARANOID(("FastDDSOutPort::init()"));
    
    if(prop.propertyNames().size() == 0)
    {
      RTC_DEBUG(("Property is empty."));
      return;
    }

    FastDDSManager& topicmgr = FastDDSManager::instance();
    eprosima::fastdds::dds::DomainParticipant* participant = topicmgr.getParticipant();

    if (participant == nullptr)
    {
        RTC_ERROR(("Can not initialize Fast-RTPS"));
        throw std::bad_alloc();
    }

    std::string marshaling_type = prop.getProperty("marshaling_type", "corba");
    coil::Properties& fastdds_prop = prop.getNode("fast-dds");
    m_topic = fastdds_prop.getProperty("topic", "chatter");


    const std::string str_corba = "corba";

    bool is_serializer_cdr = !(marshaling_type.compare(0, str_corba.size(), str_corba) != 0);

    if (!is_serializer_cdr)
    {
        FastDDSMessageInfoBase* info = GlobalFastDDSMessageInfoList::instance().getInfo(marshaling_type);

        if (!info)
        {
            RTC_ERROR(("Can not find message type(%s)", marshaling_type.c_str()));
            throw std::bad_alloc();
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
        if (!endian.empty())
        {	
	    if (endian[0] == "little")
            {
                type->setEndian(true);
            }
            else if (endian[0] == "big")
            {
                type->setEndian(false);
            }
	}

        topicmgr.registerType(type);
    }

    eprosima::fastdds::dds::DataWriterQos Wparam = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;

    std::string publisher_name = fastdds_prop.getProperty("publisher.name");

    // Topic生成
    m_dds_topic = participant->create_topic(m_topic, m_dataType, 
		    eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);
    if (m_dds_topic == nullptr)
    {
        RTC_ERROR(("Failed to create Topic: %s", m_topic.c_str()));
        throw std::runtime_error("Topic creation failed");
    }

    // Publisher生成
    m_publisher = participant->create_publisher(eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT);
    if (m_publisher == nullptr)
    {
        participant->delete_topic(m_dds_topic);
        m_dds_topic = nullptr;

        RTC_ERROR(("Failed to create Publisher"));
        throw std::runtime_error("Publisher creation failed");
    }

    // DataWriter QoS設定
    if (publisher_name.empty() || !topicmgr.xmlConfigured())
    {
        // 既存のsetPubParam()はポインタを受け取るため
        // ここでは&Wparamを渡す
        setPubParam(fastdds_prop, &Wparam);
    }
    else
    {
        RTC_INFO(("DataWriter QoS profile: %s",
                  publisher_name.c_str()));

        auto ret = m_publisher->get_datawriter_qos_from_profile(
            publisher_name, Wparam);

        if (ret != eprosima::fastdds::dds::RETCODE_OK)
        {
            RTC_ERROR(("Failed to load DataWriter QoS"));

            participant->delete_publisher(m_publisher);
            m_publisher = nullptr;

            participant->delete_topic(m_dds_topic);
            m_dds_topic = nullptr;

            throw std::runtime_error("DataWriter QoS error");
        }
    }    
    
    outputLog(Wparam);
    
    // DataWriter生成
    m_writer = m_publisher->create_datawriter(m_dds_topic, Wparam, &m_listener);
    if (m_writer == nullptr)
    {
        RTC_ERROR(("Failed to create DataWriter"));

        participant->delete_publisher(m_publisher);
        m_publisher = nullptr;

        participant->delete_topic(m_dds_topic);
        m_dds_topic = nullptr;

        throw std::runtime_error("DataWriter creation failed");
    }

    RTC_INFO(("FastDDSOutPort initialized: topic=%s, type=%s",
              m_topic.c_str(), m_dataType.c_str()));
  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus FastDDSOutPort::put(ByteData& data)
  {
    
    RTC_PARANOID(("put()"));
    RTC_VERBOSE(("Data size:%d", data.getDataLength()));
    if (m_publisher == nullptr)
    {
        RTC_VERBOSE(("Publisher is None"));
        return DataPortStatus::PRECONDITION_NOT_MET;
    }
    //if (m_publisher->write(&data))
    if (m_writer->write(&data))
    {
        RTC_PARANOID(("write:OK"));
        return DataPortStatus::PORT_OK;
    }
    else
    {
        RTC_ERROR(("write:ERROR"));
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
  void FastDDSOutPort::
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
  bool FastDDSOutPort::
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
  void FastDDSOutPort::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));

    if (m_publisher != nullptr)
    {
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
  void FastDDSOutPort::setDuration(coil::Properties& prop, eprosima::fastdds::dds::Duration_t& time)
  {
    std::string sec_str = prop["seconds"];
    std::string nanosec_str = prop["nanosec"];

    if (sec_str == "2147483647" && nanosec_str == "4294967295")
    {
      time = eprosima::fastdds::dds::c_TimeInfinite;
      return;
    }
    else if (sec_str == "0" && nanosec_str == "0")
    {
      time = eprosima::fastdds::dds::c_TimeZero;
      return;
    }


    coil::stringTo<int32_t>(time.seconds, sec_str.c_str());
    coil::stringTo<uint32_t>(time.nanosec, nanosec_str.c_str());
  }

  /*!
   * @if jp
   * @brief プロパティからeprosima::fastrtps::PublisherAttributesを設定する
   *
   * @param fastrtps_prop プロパティ
   * @param Rparam Publisherの属性
   *
   * @else
   * @brief
   *
   * @param fastrtps_prop
   * @param Rparam
   *
   *
   * @endif
   */
  void FastDDSOutPort::setPubParam(coil::Properties& fastdds_prop, eprosima::fastdds::dds::DataWriterQos* Wparam)
  {
    setDuration(fastdds_prop.getNode("publisher.qos.deadline.period"), Wparam->deadline().period);

    std::string destinationOrder_kind = fastdds_prop.getProperty("publisher.qos.destinationOrder.kind", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS");

    if (destinationOrder_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
    {
      Wparam->destination_order().kind = eprosima::fastdds::dds::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
    }
    else if (destinationOrder_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
    {
      Wparam->destination_order().kind = eprosima::fastdds::dds::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
    }

    Wparam->reliable_writer_qos().disable_positive_acks.enabled = coil::toBool(fastdds_prop["publisher.qos.disablePositiveACKs.enabled"], "YES", "NO", Wparam->reliable_writer_qos().disable_positive_acks.enabled);

    setDuration(fastdds_prop.getNode("publisher.qos.disablePositiveACKs.duration"), Wparam->reliable_writer_qos().disable_positive_acks.duration);


    std::string durability_kind = fastdds_prop.getProperty("publisher.qos.durability.kind", "VOLATILE_DURABILITY_QOS");

    if (durability_kind == "VOLATILE_DURABILITY_QOS")
    {
      Wparam->durability().kind = eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS;
    }
    else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
    {
      Wparam->durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    }
    else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
    {
      Wparam->durability().kind = eprosima::fastdds::dds::TRANSIENT_DURABILITY_QOS;
    }
    else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
    {
      Wparam->durability().kind = eprosima::fastdds::dds::PERSISTENT_DURABILITY_QOS;
    }

    coil::stringTo<int32_t>(Wparam->durability_service().history_depth, fastdds_prop["publisher.qos.durabilityService.history_depth"].c_str());


    std::string durabilityService_history_kind = fastdds_prop.getProperty("publisher.qos.durabilityService.history_kind", "KEEP_LAST_HISTORY_QOS");

    if (durabilityService_history_kind == "KEEP_LAST_HISTORY_QOS")
    {
      Wparam->durability_service().history_kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    }
    else if (durabilityService_history_kind == "KEEP_ALL_HISTORY_QOS")
    {
      Wparam->durability_service().history_kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
    }

    coil::stringTo<int32_t>(Wparam->durability_service().max_instances, fastdds_prop["publisher.qos.durabilityService.max_instances"].c_str());
    coil::stringTo<int32_t>(Wparam->durability_service().max_samples, fastdds_prop["publisher.qos.durabilityService.max_samples"].c_str());
    coil::stringTo<int32_t>(Wparam->durability_service().max_samples_per_instance, fastdds_prop["publisher.qos.durabilityService.max_samples_per_instance"].c_str());

    setDuration(fastdds_prop.getNode("publisher.qos.durabilityService.service_cleanup_delay"), Wparam->durability_service().service_cleanup_delay);


    //Wparam->qos.m_groupData;

    setDuration(fastdds_prop.getNode("publisher.qos.latencyBudget.duration"), Wparam->latency_budget().duration);

    setDuration(fastdds_prop.getNode("publisher.qos.lifespan.duration"), Wparam->lifespan().duration);

    setDuration(fastdds_prop.getNode("publisher.qos.liveliness.announcement_period"), Wparam->liveliness().announcement_period);

    std::string liveliness_kind = fastdds_prop.getProperty("publisher.qos.liveliness.kind", "AUTOMATIC_LIVELINESS_QOS");

    if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
    {
      Wparam->liveliness().kind = eprosima::fastdds::dds::AUTOMATIC_LIVELINESS_QOS;
    }
    else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
    {
      Wparam->liveliness().kind = eprosima::fastdds::dds::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
    }
    else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
    {
      Wparam->liveliness().kind = eprosima::fastdds::dds::MANUAL_BY_TOPIC_LIVELINESS_QOS;
    }

    setDuration(fastdds_prop.getNode("publisher.qos.liveliness.lease_duration"), Wparam->liveliness().lease_duration);


    std::string ownership_kind = fastdds_prop.getProperty("publisher.qos.ownership.kind", "SHARED_OWNERSHIP_QOS");

    if (ownership_kind == "SHARED_OWNERSHIP_QOS")
    {
      Wparam->ownership().kind = eprosima::fastdds::dds::SHARED_OWNERSHIP_QOS;
    }
    else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
    {
      Wparam->ownership().kind = eprosima::fastdds::dds::EXCLUSIVE_OWNERSHIP_QOS;
    }


    coil::stringTo<uint32_t>(Wparam->ownership_strength().value, fastdds_prop["publisher.qos.ownershipStrength.value"].c_str());

    //Wparam->qos.m_partition

    eprosima::fastdds::dds::PublisherQos Pqos = eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT;
    std::string presentation_access_scope = fastdds_prop.getProperty("publisher.qos.presentation.access_scope", "INSTANCE_PRESENTATION_QOS");

    if (presentation_access_scope == "INSTANCE_PRESENTATION_QOS")
    {
      Pqos.presentation().access_scope = eprosima::fastdds::dds::INSTANCE_PRESENTATION_QOS;
    }
    else if (presentation_access_scope == "TOPIC_PRESENTATION_QOS")
    {
      Pqos.presentation().access_scope = eprosima::fastdds::dds::TOPIC_PRESENTATION_QOS;
    }
    else if (presentation_access_scope == "GROUP_PRESENTATION_QOS")
    {
      Pqos.presentation().access_scope = eprosima::fastdds::dds::GROUP_PRESENTATION_QOS;
    }

    Pqos.presentation().coherent_access = coil::toBool(fastdds_prop["publisher.qos.presentation.coherent_access"], "YES", "NO", Pqos.presentation().coherent_access);
    Pqos.presentation().ordered_access = coil::toBool(fastdds_prop["publisher.qos.presentation.ordered_access"], "YES", "NO", Pqos.presentation().ordered_access);


    std::string publishMode_kind = fastdds_prop.getProperty("publisher.qos.publishMode.kind", "SYNCHRONOUS_PUBLISH_MODE");

    if (publishMode_kind == "SYNCHRONOUS_PUBLISH_MODE")
    {
      Wparam->publish_mode().kind = eprosima::fastdds::dds::SYNCHRONOUS_PUBLISH_MODE;
    }
    else if (publishMode_kind == "ASYNCHRONOUS_PUBLISH_MODE")
    {
      Wparam->publish_mode().kind = eprosima::fastdds::dds::ASYNCHRONOUS_PUBLISH_MODE;
    }


    std::string reliability_kind = fastdds_prop.getProperty("publisher.qos.reliability.kind", "BEST_EFFORT_RELIABILITY_QOS");

    if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
    {
      Wparam->reliability().kind = eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    }
    else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
    {
      Wparam->reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    }

    setDuration(fastdds_prop.getNode("publisher.qos.reliability.max_blocking_time"), Wparam->reliability().max_blocking_time);

    std::string history_memory_policy = fastdds_prop.getProperty("publisher.history_memory_policy", "PREALLOCATED_WITH_REALLOC_MEMORY_MODE");
    if (history_memory_policy == "PREALLOCATED_MEMORY_MODE")
    {
      Wparam->endpoint().history_memory_policy = eprosima::fastdds::rtps::PREALLOCATED_MEMORY_MODE;
    }
    else if (history_memory_policy == "PREALLOCATED_WITH_REALLOC_MEMORY_MODE")
    {
      Wparam->endpoint().history_memory_policy = eprosima::fastdds::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
    }
    else if (history_memory_policy == "DYNAMIC_RESERVE_MEMORY_MODE")
    {
      Wparam->endpoint().history_memory_policy = eprosima::fastdds::rtps::DYNAMIC_RESERVE_MEMORY_MODE;
    }

    coil::stringTo<int32_t>(Wparam->history().depth, fastdds_prop["publisher.topic.historyQos.depth"].c_str());

    std::string topic_historyQos_kind = fastdds_prop.getProperty("publisher.topic.historyQos.kind", "KEEP_LAST_HISTORY_QOS");
    if (history_memory_policy == "KEEP_LAST_HISTORY_QOS")
    {
      Wparam->history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    }
    else if (history_memory_policy == "KEEP_ALL_HISTORY_QOS")
    {
      Wparam->history().kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
    }

    setDuration(fastdds_prop.getNode("publisher.times.heartbeatPeriod"), Wparam->reliable_writer_qos().times.heartbeat_period);
    setDuration(fastdds_prop.getNode("publisher.times.initialHeartbeatDelay"), Wparam->reliable_writer_qos().times.initial_heartbeat_delay);
    setDuration(fastdds_prop.getNode("publisher.times.nackResponseDelay"), Wparam->reliable_writer_qos().times.nack_response_delay);
    setDuration(fastdds_prop.getNode("publisher.times.nackSupressionDuration"), Wparam->reliable_writer_qos().times.nack_supression_duration);


  }

  /*!
   * @if jp
   * @brief eprosima::fastrtps::PublisherAttributesのパラメータをログに出力する
   *
   * @param Rparam Publisherの属性
   *
   * @else
   * @brief
   *
   * @param Rparam
   *
   *
   * @endif
   */
  void FastDDSOutPort::outputLog(const eprosima::fastdds::dds::DataWriterQos& Wparam)
  {
    eprosima::fastdds::dds::PublisherQos Pqos = eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT;
    RTC_DEBUG(("WriterQos setting: publisher.deadline.period: sec=%d nanosec=%u", Wparam.deadline().period.seconds, Wparam.deadline().period.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.destinationOrder.kind: %hhu", Wparam.destination_order().kind));
    RTC_DEBUG(("WriterQos setting: publisher.disablePositiveACKs.enabled: %s", (Wparam.reliable_writer_qos().disable_positive_acks.enabled ? "true" : "false")));
    RTC_DEBUG(("WriterQos setting: publisher.disablePositiveACKs.duration: sec=%d nanosec=%u", Wparam.reliable_writer_qos().disable_positive_acks.duration.seconds, Wparam.reliable_writer_qos().disable_positive_acks.duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.durability.kind: %hhu", Wparam.durability().kind));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.history_depth: %d", Wparam.durability_service().history_depth));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.history_kind: %hhu", Wparam.durability_service().history_kind));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.max_instances: %d", Wparam.durability_service().max_instances));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.max_samples: %d", Wparam.durability_service().max_samples));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.max_samples_per_instance: %d", Wparam.durability_service().max_samples_per_instance));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.service_cleanup_delay: sec=%d nanosec=%u", Wparam.durability_service().service_cleanup_delay.seconds, Wparam.durability_service().service_cleanup_delay.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.latencyBudget.duration: sec=%d nanosec=%u", Wparam.latency_budget().duration.seconds, Wparam.latency_budget().duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.lifespan.duration: sec=%d nanosec=%u", Wparam.lifespan().duration.seconds, Wparam.lifespan().duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.liveliness.announcement_period: sec=%d nanosec=%u", Wparam.liveliness().announcement_period.seconds, Wparam.liveliness().announcement_period.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.liveliness.kind: %hhu", Wparam.liveliness().kind));
    RTC_DEBUG(("WriterQos setting: publisher.liveliness.lease_duration: sec=%d nanosec=%u", Wparam.liveliness().lease_duration.seconds, Wparam.liveliness().lease_duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.ownership.kind: %hhu", Wparam.ownership().kind));
    RTC_DEBUG(("WriterQos setting: publisher.ownershipStrength.value: %u", Wparam.ownership_strength().value));
    RTC_DEBUG(("WriterQos setting: publisher.presentation.access_scope: %hhu", Pqos.presentation().access_scope));
    RTC_DEBUG(("WriterQos setting: publisher.presentation.coherent_access: %s", (Pqos.presentation().coherent_access ? "true" : "false")));
    RTC_DEBUG(("WriterQos setting: publisher.presentation.ordered_access: %s", (Pqos.presentation().ordered_access ? "true" : "false")));
    RTC_DEBUG(("WriterQos setting: publisher.publishMode.kind: %hhu", Wparam.publish_mode().kind));
    RTC_DEBUG(("WriterQos setting: publisher.reliability.kind: %hhu", Wparam.reliability().kind));
    RTC_DEBUG(("WriterQos setting: publisher.reliability.max_blocking_time: sec=%d nanosec=%u", Wparam.reliability().max_blocking_time.seconds, Wparam.reliability().max_blocking_time.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.history_memory_policy: %d", Wparam.endpoint().history_memory_policy));
    RTC_DEBUG(("WriterQos setting: publisher.times.heartbeatPeriod: sec=%d nanosec=%u", Wparam.reliable_writer_qos().times.heartbeat_period.seconds, Wparam.reliable_writer_qos().times.heartbeat_period.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.times.initialHeartbeatDelay: sec=%d nanosec=%u", Wparam.reliable_writer_qos().times.initial_heartbeat_delay.seconds, Wparam.reliable_writer_qos().times.initial_heartbeat_delay.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.times.nackResponseDelay: sec=%d nanosec=%u", Wparam.reliable_writer_qos().times.nack_response_delay.seconds, Wparam.reliable_writer_qos().times.nack_response_delay.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.times.nackSupressionDuration: sec=%d nanosec=%u", Wparam.reliable_writer_qos().times.nack_supression_duration.seconds, Wparam.reliable_writer_qos().times.nack_supression_duration.nanosec));

  }

  /*!
   * @if jp
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   *
   * @else
   * @brief Constructor
   *
   * Constructor
   *
   *
   * @endif
   */
  FastDDSOutPort::PubListener::PubListener()
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
  FastDDSOutPort::PubListener::~PubListener()
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
  void FastDDSOutPort::PubListener::on_publication_matched(eprosima::fastdds::dds::DataWriter* /*pub*/, const eprosima::fastdds::dds::PublicationMatchedStatus& /*info*/)
  {
  }



  
} // namespace RTC

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif
