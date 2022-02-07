// -*- C++ -*-
/*!
 * @file  FastRTPSOutPort.cpp
 * @brief FastRTPSOutPort class
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
#include <coil/UUID.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/xmlparser/XMLProfileManager.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif
#include "FastRTPSOutPort.h"
#include "FastRTPSManager.h"
#include "FastRTPSMessageInfo.h"






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
  FastRTPSOutPort::FastRTPSOutPort(void)
    : rtclog("FastRTPSOutPort")
  {
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  FastRTPSOutPort::~FastRTPSOutPort(void)
  {
    RTC_PARANOID(("~FastRTPSOutPort()"));
    
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void FastRTPSOutPort::init(coil::Properties& prop)
  { 
    RTC_PARANOID(("FastRTPSOutPort::init()"));
    
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

    eprosima::fastrtps::PublisherAttributes Wparam;

    std::string publisher_name = fastrtps_prop.getProperty("publisher.name");
    if(publisher_name.empty())
    {
      Wparam.topic.topicKind = eprosima::fastrtps::rtps::NO_KEY;
      Wparam.topic.topicDataType = m_dataType;
      Wparam.topic.topicName = m_topic;
      
      
      setDuration(fastrtps_prop.getNode("publisher.qos.deadline.period"), Wparam.qos.m_deadline.period);

      std::string destinationOrder_kind = fastrtps_prop.getProperty("publisher.qos.destinationOrder.kind", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS");
      
      if (destinationOrder_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        Wparam.qos.m_destinationOrder.kind = eprosima::fastdds::dds::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
      }
      else if (destinationOrder_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        Wparam.qos.m_destinationOrder.kind = eprosima::fastdds::dds::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
      }

      Wparam.qos.m_disablePositiveACKs.enabled = coil::toBool(fastrtps_prop["publisher.qos.disablePositiveACKs.enabled"], "YES", "NO", Wparam.qos.m_disablePositiveACKs.enabled);
      
      setDuration(fastrtps_prop.getNode("publisher.qos.disablePositiveACKs.duration"), Wparam.qos.m_disablePositiveACKs.duration);


      std::string durability_kind = fastrtps_prop.getProperty("publisher.qos.durability.kind", "VOLATILE_DURABILITY_QOS");

      if (durability_kind == "VOLATILE_DURABILITY_QOS")
      {
        Wparam.qos.m_durability.kind = eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
      {
        Wparam.qos.m_durability.kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
      {
        Wparam.qos.m_durability.kind = eprosima::fastdds::dds::TRANSIENT_DURABILITY_QOS;
      }
      else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
      {
        Wparam.qos.m_durability.kind = eprosima::fastdds::dds::PERSISTENT_DURABILITY_QOS;
      }

      coil::stringTo<int32_t>(Wparam.qos.m_durabilityService.history_depth, fastrtps_prop["publisher.qos.durabilityService.history_depth"].c_str());


      std::string durabilityService_history_kind = fastrtps_prop.getProperty("publisher.qos.durabilityService.history_kind", "KEEP_LAST_HISTORY_QOS");

      if (durabilityService_history_kind == "KEEP_LAST_HISTORY_QOS")
      {
        Wparam.qos.m_durabilityService.history_kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
      }
      else if (durabilityService_history_kind == "KEEP_ALL_HISTORY_QOS")
      {
        Wparam.qos.m_durabilityService.history_kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
      }
      
      coil::stringTo<int32_t>(Wparam.qos.m_durabilityService.max_instances, fastrtps_prop["publisher.qos.durabilityService.max_instances"].c_str());
      coil::stringTo<int32_t>(Wparam.qos.m_durabilityService.max_samples, fastrtps_prop["publisher.qos.durabilityService.max_samples"].c_str());
      coil::stringTo<int32_t>(Wparam.qos.m_durabilityService.max_samples_per_instance, fastrtps_prop["publisher.qos.durabilityService.max_samples_per_instance"].c_str());

      setDuration(fastrtps_prop.getNode("publisher.qos.durabilityService.service_cleanup_delay"), Wparam.qos.m_durabilityService.service_cleanup_delay);

      
      //Wparam.qos.m_groupData;

      setDuration(fastrtps_prop.getNode("publisher.qos.latencyBudget.duration"), Wparam.qos.m_latencyBudget.duration);

      setDuration(fastrtps_prop.getNode("publisher.qos.lifespan.duration"), Wparam.qos.m_lifespan.duration);

      setDuration(fastrtps_prop.getNode("publisher.qos.liveliness.announcement_period"), Wparam.qos.m_liveliness.announcement_period);

      std::string liveliness_kind = fastrtps_prop.getProperty("publisher.qos.liveliness.kind", "AUTOMATIC_LIVELINESS_QOS");

      if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
      {
        Wparam.qos.m_liveliness.kind = eprosima::fastdds::dds::AUTOMATIC_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
      {
        Wparam.qos.m_liveliness.kind = eprosima::fastdds::dds::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
      {
        Wparam.qos.m_liveliness.kind = eprosima::fastdds::dds::MANUAL_BY_TOPIC_LIVELINESS_QOS;
      }

      setDuration(fastrtps_prop.getNode("publisher.qos.liveliness.lease_duration"), Wparam.qos.m_liveliness.lease_duration);


      std::string ownership_kind = fastrtps_prop.getProperty("publisher.qos.ownership.kind", "SHARED_OWNERSHIP_QOS");

      if (ownership_kind == "SHARED_OWNERSHIP_QOS")
      {
        Wparam.qos.m_ownership.kind = eprosima::fastdds::dds::SHARED_OWNERSHIP_QOS;
      }
      else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
      {
        Wparam.qos.m_ownership.kind = eprosima::fastdds::dds::EXCLUSIVE_OWNERSHIP_QOS;
      }


      coil::stringTo<uint32_t>(Wparam.qos.m_ownershipStrength.value, fastrtps_prop["publisher.qos.ownershipStrength.value"].c_str());

      //Wparam.qos.m_partition


      std::string presentation_access_scope = fastrtps_prop.getProperty("publisher.qos.presentation.access_scope", "INSTANCE_PRESENTATION_QOS");

      if (presentation_access_scope == "INSTANCE_PRESENTATION_QOS")
      {
        Wparam.qos.m_presentation.access_scope = eprosima::fastdds::dds::INSTANCE_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "TOPIC_PRESENTATION_QOS")
      {
        Wparam.qos.m_presentation.access_scope = eprosima::fastdds::dds::TOPIC_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "GROUP_PRESENTATION_QOS")
      {
        Wparam.qos.m_presentation.access_scope = eprosima::fastdds::dds::GROUP_PRESENTATION_QOS;
      }

      Wparam.qos.m_presentation.coherent_access = coil::toBool(fastrtps_prop["publisher.qos.presentation.coherent_access"], "YES", "NO", Wparam.qos.m_presentation.coherent_access);
      Wparam.qos.m_presentation.ordered_access = coil::toBool(fastrtps_prop["publisher.qos.presentation.ordered_access"], "YES", "NO", Wparam.qos.m_presentation.ordered_access);


      std::string publishMode_kind = fastrtps_prop.getProperty("publisher.qos.publishMode.kind", "SYNCHRONOUS_PUBLISH_MODE");

      if (publishMode_kind == "SYNCHRONOUS_PUBLISH_MODE")
      {
        Wparam.qos.m_publishMode.kind = eprosima::fastdds::dds::SYNCHRONOUS_PUBLISH_MODE;
      }
      else if (publishMode_kind == "ASYNCHRONOUS_PUBLISH_MODE")
      {
        Wparam.qos.m_publishMode.kind = eprosima::fastdds::dds::ASYNCHRONOUS_PUBLISH_MODE;
      }


      std::string reliability_kind = fastrtps_prop.getProperty("publisher.qos.reliability.kind", "BEST_EFFORT_RELIABILITY_QOS");

      if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
      {
        Wparam.qos.m_reliability.kind = eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS;
      }
      else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
      {
        Wparam.qos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
      }

      setDuration(fastrtps_prop.getNode("publisher.qos.reliability.max_blocking_time"), Wparam.qos.m_reliability.max_blocking_time);

      setDuration(fastrtps_prop.getNode("publisher.qos.timeBasedFilter.minimum_separation"), Wparam.qos.m_timeBasedFilter.minimum_separation);
      

      std::string history_memory_policy = fastrtps_prop.getProperty("publisher.history_memory_policy", "PREALLOCATED_WITH_REALLOC_MEMORY_MODE");
      if (history_memory_policy == "PREALLOCATED_MEMORY_MODE")
      {
        Wparam.historyMemoryPolicy = eprosima::fastrtps::rtps::PREALLOCATED_MEMORY_MODE;
      }
      else if (history_memory_policy == "PREALLOCATED_WITH_REALLOC_MEMORY_MODE")
      {
        Wparam.historyMemoryPolicy = eprosima::fastrtps::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
      }
      else if (history_memory_policy == "DYNAMIC_RESERVE_MEMORY_MODE")
      {
        Wparam.historyMemoryPolicy = eprosima::fastrtps::rtps::DYNAMIC_RESERVE_MEMORY_MODE;
      }
      else if (history_memory_policy == "DYNAMIC_REUSABLE_MEMORY_MODE")
      {
        Wparam.historyMemoryPolicy = eprosima::fastrtps::rtps::DYNAMIC_REUSABLE_MEMORY_MODE;
      }

      coil::stringTo<int32_t>(Wparam.topic.historyQos.depth, fastrtps_prop["publisher.topic.historyQos.depth"].c_str());

      std::string topic_historyQos_kind = fastrtps_prop.getProperty("publisher.topic.historyQos.kind", "KEEP_LAST_HISTORY_QOS");
      if (history_memory_policy == "KEEP_LAST_HISTORY_QOS")
      {
        Wparam.topic.historyQos.kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
      }
      else if (history_memory_policy == "KEEP_ALL_HISTORY_QOS")
      {
        Wparam.topic.historyQos.kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
      }
    }
    else
    {
      RTC_INFO(("Publisher name:", publisher_name.c_str()));
      if(eprosima::fastrtps::xmlparser::XMLP_ret::XML_ERROR == eprosima::fastrtps::xmlparser::XMLProfileManager::fillPublisherAttributes(publisher_name, Wparam))
      {
        RTC_ERROR(("xml file load failed"));
        throw;
      }
      Wparam.topic.topicDataType = m_dataType;
      //Wparam.topic.topicName = m_topic;
      //Wparam.qos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
      
    }


    RTC_DEBUG(("WriterQos setting: publisher.deadline.period: sec=%d nanosec=%u", Wparam.qos.m_deadline.period.seconds, Wparam.qos.m_deadline.period.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.destinationOrder.kind: %hhu", Wparam.qos.m_destinationOrder.kind));
    RTC_DEBUG(("WriterQos setting: publisher.disablePositiveACKs.enabled: %s", (Wparam.qos.m_disablePositiveACKs.enabled ? "true" : "false")));
    RTC_DEBUG(("WriterQos setting: publisher.disablePositiveACKs.duration: sec=%d nanosec=%u", Wparam.qos.m_disablePositiveACKs.duration.seconds, Wparam.qos.m_disablePositiveACKs.duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.durability.kind: %hhu", Wparam.qos.m_durability.kind));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.history_depth: %d", Wparam.qos.m_durabilityService.history_depth));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.history_kind: %hhu", Wparam.qos.m_durabilityService.history_kind));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.max_instances: %d", Wparam.qos.m_durabilityService.max_instances));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.max_samples: %d", Wparam.qos.m_durabilityService.max_samples));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.max_samples_per_instance: %d", Wparam.qos.m_durabilityService.max_samples_per_instance));
    RTC_DEBUG(("WriterQos setting: publisher.durabilityService.service_cleanup_delay: sec=%d nanosec=%u", Wparam.qos.m_durabilityService.service_cleanup_delay.seconds, Wparam.qos.m_durabilityService.service_cleanup_delay.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.latencyBudget.duration: sec=%d nanosec=%u", Wparam.qos.m_latencyBudget.duration.seconds, Wparam.qos.m_latencyBudget.duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.lifespan.duration: sec=%d nanosec=%u", Wparam.qos.m_lifespan.duration.seconds, Wparam.qos.m_lifespan.duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.liveliness.announcement_period: sec=%d nanosec=%u", Wparam.qos.m_liveliness.announcement_period.seconds, Wparam.qos.m_liveliness.announcement_period.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.liveliness.kind: %hhu", Wparam.qos.m_liveliness.kind));
    RTC_DEBUG(("WriterQos setting: publisher.liveliness.lease_duration: sec=%d nanosec=%u", Wparam.qos.m_liveliness.lease_duration.seconds, Wparam.qos.m_liveliness.lease_duration.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.ownership.kind: %hhu", Wparam.qos.m_ownership.kind));
    RTC_DEBUG(("WriterQos setting: publisher.ownershipStrength.value: %u", Wparam.qos.m_ownershipStrength.value));
    RTC_DEBUG(("WriterQos setting: publisher.presentation.access_scope: %hhu", Wparam.qos.m_presentation.access_scope));
    RTC_DEBUG(("WriterQos setting: publisher.presentation.coherent_access: %s", (Wparam.qos.m_presentation.coherent_access ? "true" : "false")));
    RTC_DEBUG(("WriterQos setting: publisher.presentation.ordered_access: %s", (Wparam.qos.m_presentation.ordered_access ? "true" : "false")));
    RTC_DEBUG(("WriterQos setting: publisher.publishMode.kind: %hhu", Wparam.qos.m_publishMode.kind));
    RTC_DEBUG(("WriterQos setting: publisher.reliability.kind: %hhu", Wparam.qos.m_reliability.kind));
    RTC_DEBUG(("WriterQos setting: publisher.reliability.max_blocking_time: sec=%d nanosec=%u", Wparam.qos.m_reliability.max_blocking_time.seconds, Wparam.qos.m_reliability.max_blocking_time.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.timeBasedFilter.minimum_separation: sec=%d nanosec=%u", Wparam.qos.m_timeBasedFilter.minimum_separation.seconds, Wparam.qos.m_timeBasedFilter.minimum_separation.nanosec));
    RTC_DEBUG(("WriterQos setting: publisher.history_memory_policy: %d", Wparam.historyMemoryPolicy));

    m_publisher = eprosima::fastrtps::Domain::createPublisher(participant, Wparam, (eprosima::fastrtps::PublisherListener*)&m_listener);

    
    if (m_publisher == nullptr)
    {
        RTC_ERROR(("Publisher initialize failed"));
        throw;
    }

  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus FastRTPSOutPort::put(ByteData& data)
  {
    
    RTC_PARANOID(("put()"));
    RTC_VERBOSE(("Data size:%d", data.getDataLength()));
    if (m_publisher == nullptr)
    {
        RTC_VERBOSE(("Publisher is None"));
        return DataPortStatus::PRECONDITION_NOT_MET;
    }
    if (m_publisher->write(&data))
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
  void FastRTPSOutPort::
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
  bool FastRTPSOutPort::
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
  void FastRTPSOutPort::
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
#if (FASTRTPS_VERSION_MAJOR <= 1) && (FASTRTPS_VERSION_MINOR <= 7)
  void FastRTPSOutPort::setDuration(coil::Properties& prop, eprosima::fastrtps::rtps::Duration_t& time)
#else
  void FastRTPSOutPort::setDuration(coil::Properties& prop, eprosima::fastrtps::Duration_t& time)
#endif
  {
    std::string sec_str = prop["seconds"];
    std::string nanosec_str = prop["nanosec"];


    coil::stringTo<int32_t>(time.seconds, sec_str.c_str());
    coil::stringTo<uint32_t>(time.nanosec, nanosec_str.c_str());
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
  FastRTPSOutPort::PubListener::PubListener()
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
  FastRTPSOutPort::PubListener::~PubListener()
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
  void FastRTPSOutPort::PubListener::onPublicationMatched(eprosima::fastrtps::Publisher* /*pub*/, eprosima::fastrtps::rtps::MatchingInfo& /*info*/)
  {
  }


  
} // namespace RTC
