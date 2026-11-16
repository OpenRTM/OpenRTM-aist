// -*- C++ -*-

/*!
 * @file  FastDDSInPort.cpp
 * @brief FastDDSInPort class
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

#include "FastDDSInPort.h"
#include "FastDDSManager.h"
#include "FastDDSMessageInfo.h"
#include <coil/UUID.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/rtps/common/ChangeKind_t.hpp>
#include <stdexcept>
#include <vector>


namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  FastDDSInPort::FastDDSInPort(void)
   : m_buffer(nullptr), m_listeners(nullptr), m_connector(nullptr), m_listener(this)
  {
    // PortProfile setting
    setInterfaceType("fast-dds");
    // ConnectorProfile setting

  }

  

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  FastDDSInPort::~FastDDSInPort(void)
  {
    using namespace eprosima::fastdds::dds;
    RTC_PARANOID(("~FastDDSInPort()"));

    auto* participant =
        FastDDSManager::instance().getParticipant();
    if (participant == nullptr)
    {
      return;
    }

    if (m_subscriber != nullptr)
    {
      if (participant->delete_subscriber(m_subscriber)
            == eprosima::fastdds::dds::RETCODE_OK)
      {
        m_subscriber = nullptr;
      }
      else
      {
        RTC_ERROR(("Failed to delete Subscriber"));
        return;
      }
    }

    if (m_dds_topic != nullptr)
    {
      if (participant->delete_topic(m_dds_topic)
		      == eprosima::fastdds::dds::RETCODE_OK)
      {
        m_dds_topic = nullptr;
      }
      else
      {
        RTC_ERROR(("Failed to delete Topic"));
      }
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
  void FastDDSInPort::init(coil::Properties& prop)
  {
    using namespace eprosima::fastdds::dds;
    RTC_PARANOID(("FastDDSInPort::init()"));

    if (prop.propertyNames().empty())
    {
        RTC_DEBUG(("Property is empty."));
        return;
    }

    // init()が再度呼ばれた場合の暫定対応。
    // QoS変更を伴う再初期化は別途実装する。
    if (m_reader != nullptr)
    {
        RTC_DEBUG(("FastDDSInPort is already initialized."));
        return;
    }

    FastDDSManager& topicmgr = FastDDSManager::instance();
    DomainParticipant* participant = topicmgr.getParticipant();

    if (participant == nullptr)
    {
        RTC_ERROR(("Can not initialize Fast-DDS"));
        throw std::runtime_error("DomainParticipant is null");
    }

    // OpenRTMの設定情報を取得
    std::string marshaling_type =
        prop.getProperty("marshaling_type", "corba");

    coil::Properties& fastdds_prop = prop.getNode("fast-dds");

    m_topic = fastdds_prop.getProperty("topic", "chatter");

    // データ型の決定
    const std::string str_corba = "corba";
    bool is_serializer_cdr =
        (marshaling_type.compare(
            0, str_corba.size(), str_corba) == 0);

    if (!is_serializer_cdr)
    {
        FastDDSMessageInfoBase* info =
            GlobalFastDDSMessageInfoList::instance()
                .getInfo(marshaling_type);

        if (info == nullptr)
        {
            RTC_ERROR(("Can not find message type(%s)",
                       marshaling_type.c_str()));
            throw std::runtime_error("Message type not found");
        }

        m_dataType = info->data_type();
        m_topic = info->topic_name(m_topic);
    }
    else
    {
        std::string data =
            prop.getProperty("data_type", "IDL:RTC/CDR_Data:1.0");

        coil::vstring typelist = coil::split(data, ":");

        if (typelist.size() == 3)
        {
            m_dataType =
                coil::replaceString(
                    std::move(typelist[1]), "/", "::");
        }
        else
        {
            m_dataType = data;
        }
    }

    // DDSデータ型の登録
    if (!topicmgr.registeredType(m_dataType.c_str()))
    {
        CORBACdrDataPubSubType* type =
            new CORBACdrDataPubSubType();

        type->init(m_dataType, !is_serializer_cdr);

        std::string endian_type = coil::normalize(
            prop.getProperty("serializer.cdr.endian", ""));

        std::vector<std::string> endian =
            coil::split(endian_type, ",");

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

        if (!topicmgr.registerType(type))
        {
            RTC_ERROR(("Failed to register DDS data type"));
            // registerType()の所有権規約に従って
            // 失敗時のtype解放を処理すること。
            throw std::runtime_error("Type registration failed");
        }
    }

    using namespace eprosima::fastdds::dds;
    // DataReader QoS設定
    DataReaderQos Rparam = DATAREADER_QOS_DEFAULT;

    std::string subscriber_name =
        fastdds_prop.getProperty("subscriber.name");

    // Topic生成
    m_dds_topic = participant->create_topic(
        m_topic,
        m_dataType,
        TOPIC_QOS_DEFAULT);

    if (m_dds_topic == nullptr)
    {
        RTC_ERROR(("Failed to create Topic: %s",
                   m_topic.c_str()));
        throw std::runtime_error("Topic creation failed");
    }

    // Subscriber生成
    m_subscriber = participant->create_subscriber(
        SUBSCRIBER_QOS_DEFAULT);

    if (m_subscriber == nullptr)
    {
        participant->delete_topic(m_dds_topic);
        m_dds_topic = nullptr;

        RTC_ERROR(("Failed to create Subscriber"));
        throw std::runtime_error("Subscriber creation failed");
    }

    // QoS設定
    if (subscriber_name.empty() || !topicmgr.xmlConfigured())
    {
        setSubParam(fastdds_prop, Rparam);
    }
    else
    {
        RTC_INFO(("DataReader QoS profile: %s",
                  subscriber_name.c_str()));
        auto ret = m_subscriber->get_datareader_qos_from_profile(subscriber_name, Rparam);
        if (ret != eprosima::fastdds::dds::RETCODE_OK)
        {
            participant->delete_subscriber(m_subscriber);
            m_subscriber = nullptr;
            participant->delete_topic(m_dds_topic);
            m_dds_topic = nullptr;	    
            throw std::runtime_error("DataReader QoS error");
        }
    }

    outputLog(Rparam);

    // DataReader生成
    m_reader = m_subscriber->create_datareader(
        m_dds_topic,
        Rparam,
        &m_listener);

    if (m_reader == nullptr)
    {
        participant->delete_subscriber(m_subscriber);
        m_subscriber = nullptr;

        participant->delete_topic(m_dds_topic);
        m_dds_topic = nullptr;

        RTC_ERROR(("Failed to create DataReader"));
        throw std::runtime_error("DataReader creation failed");
    }

    RTC_INFO(("FastDDSInPort initialized: topic=%s, type=%s",
              m_topic.c_str(), m_dataType.c_str()));
    
  }


  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void FastDDSInPort::
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
  void FastDDSInPort::setListener(ConnectorInfo& info,
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
  void FastDDSInPort::setConnector(InPortConnector* connector)
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
  void FastDDSInPort::put(ByteData& cdr)
  {
      RTC_PARANOID(("FastDDSInPort::put()"));
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
  FastDDSInPort::SubListener::SubListener(FastDDSInPort* provider) : rtclog("FastDDSInPort::SubListener"), m_provider(provider)
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
  FastDDSInPort::SubListener::~SubListener()
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
  void FastDDSInPort::SubListener::on_subscription_matched(eprosima::fastdds::dds::DataReader* /*sub*/, const eprosima::fastdds::dds::SubscriptionMatchedStatus& /*info*/)
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
  void FastDDSInPort::SubListener::on_data_available(eprosima::fastdds::dds::DataReader* sub)
  {
    RTC_PARANOID(("onNewDataMessage()"));

    RTC_PARANOID(("take_next_sample"));
    if(sub->take_next_sample(&m_data, &m_info))
    {
      if(m_info.valid_data == eprosima::fastdds::rtps::ALIVE)
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
  void FastDDSInPort::convertReturn(BufferStatus status,
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
   * @brief プロパティからfastdds::dds::Duration_tを設定する
   *
   * @param prop プロパティ(seconds、nanosecの要素に値を格納する)
   * @param time fastdds::dds::Duration_t
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
  void FastDDSInPort::setDuration(coil::Properties& prop, eprosima::fastdds::dds::Duration_t& time)
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
   * @brief プロパティからeprosima::fastrtps::SubscriberAttributesを設定する
   *
   * @param fastrtps_prop プロパティ
   * @param Rparam Subscriberの属性
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
  void FastDDSInPort::setSubParam(coil::Properties& fastdds_prop, eprosima::fastdds::dds::DataReaderQos& Rparam)
  {
    setDuration(fastdds_prop.getNode("subscriber.qos.deadline.period"), Rparam.deadline().period);

    std::string destinationOrder_kind = fastdds_prop.getProperty("subscriber.qos.destinationOrder.kind", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS");

    if (destinationOrder_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
    {
      Rparam.destination_order().kind = eprosima::fastdds::dds::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
    }
    else if (destinationOrder_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
    {
      Rparam.destination_order().kind = eprosima::fastdds::dds::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
    }

    Rparam.reliable_reader_qos().disable_positive_acks.enabled = coil::toBool(fastdds_prop["subscriber.qos.disablePositiveACKs.enabled"], "YES", "NO", Rparam.reliable_reader_qos().disable_positive_acks.enabled);

    setDuration(fastdds_prop.getNode("subscriber.qos.disablePositiveACKs.duration"), Rparam.reliable_reader_qos().disable_positive_acks.duration);

    std::string durability_kind = fastdds_prop.getProperty("subscriber.qos.durability.kind", "VOLATILE_DURABILITY_QOS");

    if (durability_kind == "VOLATILE_DURABILITY_QOS")
    {
      Rparam.durability().kind = eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS;
    }
    else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
    {
      Rparam.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    }
    else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
    {
      Rparam.durability().kind = eprosima::fastdds::dds::TRANSIENT_DURABILITY_QOS;
    }
    else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
    {
      Rparam.durability().kind = eprosima::fastdds::dds::PERSISTENT_DURABILITY_QOS;
    }

    coil::stringTo<int32_t>(Rparam.durability_service().history_depth, fastdds_prop["subscriber.qos.durabilityService.history_depth"].c_str());


    std::string durabilityService_history_kind = fastdds_prop.getProperty("subscriber.qos.durabilityService.history_kind", "KEEP_LAST_HISTORY_QOS");

    if (durabilityService_history_kind == "KEEP_LAST_HISTORY_QOS")
    {
      Rparam.durability_service().history_kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    }
    else if (durabilityService_history_kind == "KEEP_ALL_HISTORY_QOS")
    {
      Rparam.durability_service().history_kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
    }

    coil::stringTo<int32_t>(Rparam.durability_service().max_instances, fastdds_prop["subscriber.qos.durabilityService.max_instances"].c_str());
    coil::stringTo<int32_t>(Rparam.durability_service().max_samples, fastdds_prop["subscriber.qos.durabilityService.max_samples"].c_str());
    coil::stringTo<int32_t>(Rparam.durability_service().max_samples_per_instance, fastdds_prop["subscriber.qos.durabilityService.max_samples_per_instance"].c_str());

    setDuration(fastdds_prop.getNode("subscriber.qos.durabilityService.service_cleanup_delay"), Rparam.durability_service().service_cleanup_delay);



    //Rparam->qos.m_groupData

    setDuration(fastdds_prop.getNode("subscriber.qos.latencyBudget.duration"), Rparam.latency_budget().duration);

    setDuration(fastdds_prop.getNode("subscriber.qos.lifespan.duration"), Rparam.lifespan().duration);

    setDuration(fastdds_prop.getNode("subscriber.qos.liveliness.announcement_period"), Rparam.liveliness().announcement_period);

    std::string liveliness_kind = fastdds_prop.getProperty("subscriber.qos.liveliness.kind", "AUTOMATIC_LIVELINESS_QOS");

    if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
    {
      Rparam.liveliness().kind = eprosima::fastdds::dds::AUTOMATIC_LIVELINESS_QOS;
    }
    else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
    {
      Rparam.liveliness().kind = eprosima::fastdds::dds::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
    }
    else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
    {
      Rparam.liveliness().kind = eprosima::fastdds::dds::MANUAL_BY_TOPIC_LIVELINESS_QOS;
    }

    setDuration(fastdds_prop.getNode("subscriber.qos.liveliness.lease_duration"), Rparam.liveliness().lease_duration);


    std::string ownership_kind = fastdds_prop.getProperty("subscriber.qos.ownership.kind", "SHARED_OWNERSHIP_QOS");

    if (ownership_kind == "SHARED_OWNERSHIP_QOS")
    {
      Rparam.ownership().kind = eprosima::fastdds::dds::SHARED_OWNERSHIP_QOS;
    }
    else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
    {
      Rparam.ownership().kind = eprosima::fastdds::dds::EXCLUSIVE_OWNERSHIP_QOS;
    }


    //Rparam->qos.m_partition


    std::string presentation_access_scope = fastdds_prop.getProperty("subscriber.qos.presentation.access_scope", "INSTANCE_PRESENTATION_QOS");
    eprosima::fastdds::dds::SubscriberQos Sqos = eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;
    if (presentation_access_scope == "INSTANCE_PRESENTATION_QOS")
    {
      Sqos.presentation().access_scope = eprosima::fastdds::dds::INSTANCE_PRESENTATION_QOS;
    }
    else if (presentation_access_scope == "TOPIC_PRESENTATION_QOS")
    {
      Sqos.presentation().access_scope = eprosima::fastdds::dds::TOPIC_PRESENTATION_QOS;
    }
    else if (presentation_access_scope == "GROUP_PRESENTATION_QOS")
    {
      Sqos.presentation().access_scope = eprosima::fastdds::dds::GROUP_PRESENTATION_QOS;
    }

    Sqos.presentation().coherent_access = coil::toBool(fastdds_prop["subscriber.qos.presentation.coherent_access"], "YES", "NO", Sqos.presentation().coherent_access);
    Sqos.presentation().ordered_access = coil::toBool(fastdds_prop["subscriber.qos.presentation.ordered_access"], "YES", "NO", Sqos.presentation().ordered_access);

    std::string reliability_kind = fastdds_prop.getProperty("subscriber.qos.reliability.kind", "BEST_EFFORT_RELIABILITY_QOS");

    if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
    {
      Rparam.reliability().kind = eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    }
    else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
    {
      Rparam.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    }


    setDuration(fastdds_prop.getNode("subscriber.qos.reliability.max_blocking_time"), Rparam.reliability().max_blocking_time);

    setDuration(fastdds_prop.getNode("subscriber.qos.timeBasedFilter.minimum_separation"), Rparam.time_based_filter().minimum_separation);


    //Rparam->qos.m_topicData
    //Rparam->qos.m_userData
    //Rparam->qos.representation.m_value

    Rparam.type_consistency().m_force_type_validation = coil::toBool(fastdds_prop["subscriber.qos.type_consistency.force_type_validation"], "YES", "NO", Rparam.type_consistency().m_force_type_validation);

    Rparam.type_consistency().m_ignore_member_names = coil::toBool(fastdds_prop["subscriber.qos.type_consistency.ignore_member_names"], "YES", "NO", Rparam.type_consistency().m_ignore_member_names);

    Rparam.type_consistency().m_ignore_sequence_bounds = coil::toBool(fastdds_prop["subscriber.qos.type_consistency.ignore_sequence_bounds"], "YES", "NO", Rparam.type_consistency().m_ignore_sequence_bounds);

    Rparam.type_consistency().m_ignore_string_bounds = coil::toBool(fastdds_prop["subscriber.qos.type_consistency.ignore_string_bounds"], "YES", "NO", Rparam.type_consistency().m_ignore_string_bounds);

    std::string type_consistency_kind = fastdds_prop.getProperty("subscriber.qos.type_consistency.kind", "ALLOW_TYPE_COERCION");

    if (type_consistency_kind == "DISALLOW_TYPE_COERCION")
    {
      Rparam.type_consistency().m_kind
        = eprosima::fastdds::dds::DISALLOW_TYPE_COERCION;
    }
    else if (type_consistency_kind == "ALLOW_TYPE_COERCION")
    {
      Rparam.type_consistency().m_kind
        = eprosima::fastdds::dds::ALLOW_TYPE_COERCION;
    }

    Rparam.type_consistency().m_prevent_type_widening = coil::toBool(fastdds_prop["subscriber.qos.type_consistency.prevent_type_widening"], "YES", "NO", Rparam.type_consistency().m_prevent_type_widening);


    std::string history_memory_policy = fastdds_prop.getProperty("subscriber.history_memory_policy", "PREALLOCATED_WITH_REALLOC_MEMORY_MODE");
    if (history_memory_policy == "PREALLOCATED_MEMORY_MODE")
    {
      Rparam.endpoint().history_memory_policy = eprosima::fastdds::rtps::PREALLOCATED_MEMORY_MODE;
    }
    else if (history_memory_policy == "PREALLOCATED_WITH_REALLOC_MEMORY_MODE")
    {
      Rparam.endpoint().history_memory_policy = eprosima::fastdds::rtps::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;
    }
    else if (history_memory_policy == "DYNAMIC_RESERVE_MEMORY_MODE")
    {
      Rparam.endpoint().history_memory_policy = eprosima::fastdds::rtps::DYNAMIC_RESERVE_MEMORY_MODE;
    }

    coil::stringTo<int32_t>(Rparam.history().depth, fastdds_prop["subscriber.topic.historyQos.depth"].c_str());

    std::string topic_historyQos_kind = fastdds_prop.getProperty("subscriber.topic.historyQos.kind", "KEEP_LAST_HISTORY_QOS");
    if (history_memory_policy == "KEEP_LAST_HISTORY_QOS")
    {
      Rparam.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    }
    else if (history_memory_policy == "KEEP_ALL_HISTORY_QOS")
    {
      Rparam.history().kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
    }

    setDuration(fastdds_prop.getNode("subscriber.times.heartbeatResponseDelay"), Rparam.reliable_reader_qos().times.heartbeat_response_delay);
    setDuration(fastdds_prop.getNode("subscriber.times.initialAcknackDelay"), Rparam.reliable_reader_qos().times.initial_acknack_delay);
  }


  /*!
   * @if jp
   * @brief eprosima::fastrtps::SubscriberAttributesのパラメータをログに出力する
   *
   * @param Rparam Subscriberの属性
   *
   * @else
   * @brief
   *
   * @param Rparam
   *
   *
   * @endif
   */
  void FastDDSInPort::outputLog(const eprosima::fastdds::dds::DataReaderQos& Rparam)
  {
    eprosima::fastdds::dds::SubscriberQos Sqos = eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;
    RTC_DEBUG(("ReaderQos setting: subscriber.deadline.period: sec=%d nanosec=%u", Rparam.deadline().period.seconds, Rparam.deadline().period.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.destinationOrder.kind: %hhu", Rparam.destination_order().kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.disablePositiveACKs.enabled: %s", (Rparam.reliable_reader_qos().disable_positive_acks.enabled ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.disablePositiveACKs.duration: sec=%d nanosec=%u", Rparam.reliable_reader_qos().disable_positive_acks.duration.seconds, Rparam.reliable_reader_qos().disable_positive_acks.duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.durability.kind: %hhu", Rparam.durability().kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.history_depth: %d", Rparam.durability_service().history_depth));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.history_kind: %hhu", Rparam.durability_service().history_kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.max_instances: %d", Rparam.durability_service().max_instances));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.max_samples: %d", Rparam.durability_service().max_samples));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.max_samples_per_instance: %d", Rparam.durability_service().max_samples_per_instance));
    RTC_DEBUG(("ReaderQos setting: subscriber.durabilityService.service_cleanup_delay: sec=%d nanosec=%u", Rparam.durability_service().service_cleanup_delay.seconds, Rparam.durability_service().service_cleanup_delay.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.latencyBudget.duration: sec=%d nanosec=%u", Rparam.latency_budget().duration.seconds, Rparam.latency_budget().duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.lifespan.duration: sec=%d nanosec=%u", Rparam.lifespan().duration.seconds, Rparam.lifespan().duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.liveliness.announcement_period: sec=%d nanosec=%u", Rparam.liveliness().announcement_period.seconds, Rparam.liveliness().announcement_period.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.liveliness.kind: %hhu", Rparam.liveliness().kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.liveliness.lease_duration: sec=%d nanosec=%u", Rparam.liveliness().lease_duration.seconds, Rparam.liveliness().lease_duration.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.ownership.kind: %hhu", Rparam.ownership().kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.presentation.access_scope: %hhu", Sqos.presentation().access_scope));
    RTC_DEBUG(("ReaderQos setting: subscriber.presentation.coherent_access: %s", (Sqos.presentation().coherent_access ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.presentation.ordered_access: %s", (Sqos.presentation().ordered_access ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.reliability.kind: %hhu", Rparam.reliability().kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.reliability.max_blocking_time: sec=%d nanosec=%u", Rparam.reliability().max_blocking_time.seconds, Rparam.reliability().max_blocking_time.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.timeBasedFilter.minimum_separation: sec=%d nanosec=%u", Rparam.time_based_filter().minimum_separation.seconds, Rparam.time_based_filter().minimum_separation.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.force_type_validation: %s", (Rparam.type_consistency().m_force_type_validation ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_member_names: %s", (Rparam.type_consistency().m_ignore_member_names ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_sequence_bounds: %s", (Rparam.type_consistency().m_ignore_sequence_bounds ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.ignore_string_bounds: %s", (Rparam.type_consistency().m_ignore_string_bounds ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.kind: %hhu", Rparam.type_consistency().m_kind));
    RTC_DEBUG(("ReaderQos setting: subscriber.type_consistency.prevent_type_widening: %s", (Rparam.type_consistency().m_prevent_type_widening ? "true" : "false")));
    RTC_DEBUG(("ReaderQos setting: subscriber.history_memory_policy: %d", Rparam.endpoint().history_memory_policy));
    RTC_DEBUG(("ReaderQos setting: subscriber.times.heartbeatResponseDelay: sec=%d nanosec=%u", Rparam.reliable_reader_qos().times.heartbeat_response_delay.seconds, Rparam.reliable_reader_qos().times.heartbeat_response_delay.nanosec));
    RTC_DEBUG(("ReaderQos setting: subscriber.times.initialAcknackDelay: sec=%d nanosec=%u", Rparam.reliable_reader_qos().times.initial_acknack_delay.seconds, Rparam.reliable_reader_qos().times.initial_acknack_delay.nanosec));
  }

} // namespace RTC

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif
