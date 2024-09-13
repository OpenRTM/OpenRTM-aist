// -*- C++ -*-
/*!
 * @file  OpenSpliceManager.cpp
 * @brief OpenSpliceManager class
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

#include "OpenSpliceManager.h"

#include <coil/Process.h>
#include <coil/stringutil.h>
#include <rtm/SystemLogger.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_iterators.hpp>
#include <iostream>
#include <fstream>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
#endif

namespace RTC
{
  OpenSpliceManager *OpenSpliceManager::manager = nullptr;
  std::mutex OpenSpliceManager::mutex;

  template <typename Ch = char>
  class children
  {
    using iterator_type = rapidxml::node_iterator<Ch>;

  public:
    explicit children(rapidxml::xml_node<Ch> *node) : node_(node) {}
    iterator_type begin() const { return iterator_type(node_); }
    iterator_type end() const { return iterator_type(); }

  private:
    rapidxml::xml_node<Ch> *node_;
  };

  static const std::string RetCodeName[13] =
      {
          "DDS_RETCODE_OK", "DDS_RETCODE_ERROR", "DDS_RETCODE_UNSUPPORTED",
          "DDS_RETCODE_BAD_PARAMETER", "DDS_RETCODE_PRECONDITION_NOT_MET",
          "DDS_RETCODE_OUT_OF_RESOURCES", "DDS_RETCODE_NOT_ENABLED",
          "DDS_RETCODE_IMMUTABLE_POLICY", "DDS_RETCODE_INCONSISTENT_POLICY",
          "DDS_RETCODE_ALREADY_DELETED", "DDS_RETCODE_TIMEOUT", "DDS_RETCODE_NO_DATA",
          "DDS_RETCODE_ILLEGAL_OPERATION"};

  std::once_flag OpenSpliceManager::m_once;

  /*!
   * @if jp
   * @brief DDS::ReturnCode_tの値がRETCODE_OK、RETCODE_NO_DATA以外の場合にエラー出力
   *
   * @param status リターンコード
   * @param info エラーの情報
   * @return statusががRETCODE_OK、RETCODE_NO_DATAの場合はtrue、それ以外はfalse
   *
   * @else
   * @brief
   *
   * @param status
   * @param info
   * @return
   *
   *
   * @endif
   */
  bool OpenSpliceManager::checkStatus(DDS::ReturnCode_t status, const char *info)
  {
    if (status != DDS::RETCODE_OK && status != DDS::RETCODE_NO_DATA)
    {
      Logger rtclog("OpenSpliceManager");
      RTC_ERROR(("Error in %s  with return code : %s", info, RetCodeName[status].c_str()));
      return false;
    }
    return true;
  }

  /*!
   * @if jp
   * @brief 参照がNULLの場合にエラー出力
   *
   * @param handle ポインタ
   * @param info エラーの情報
   * @return handleがnullptrではない場合はtrue、nullptrの場合はfalse
   *
   * @else
   * @brief
   *
   * @param handle
   * @param info
   * @return
   *
   *
   * @endif
   */
  bool OpenSpliceManager::checkHandle(void *handle, const char *info)
  {
    if (!handle)
    {
      Logger rtclog("OpenSpliceManager");
      RTC_ERROR(("Error in %s  : Creation failed: invalid handle", info));
      return false;
    }
    return true;
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
  OpenSpliceManager::OpenSpliceManager() : m_factory(nullptr), m_domain(DDS::DOMAIN_ID_DEFAULT),
                                           m_participant(nullptr), m_publisher(nullptr),
                                           m_subscriber(nullptr), m_qos_provider(nullptr)

  {
  }

  /*!
   * @if jp
   * @brief コピーコンストラクタ
   *
   * @param manager OpenSpliceManager
   *
   * @else
   * @brief Copy Constructor
   *
   * @param manager OpenSpliceManager
   *
   * @endif
   */
  OpenSpliceManager::OpenSpliceManager(const OpenSpliceManager & /*mgr*/) : m_factory(nullptr), m_domain(DDS::DOMAIN_ID_DEFAULT),
                                                                            m_participant(nullptr), m_publisher(nullptr),
                                                                            m_subscriber(nullptr), m_qos_provider(nullptr)
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
  OpenSpliceManager::~OpenSpliceManager()
  {
  }

  /*!
   * @if jp
   * @brief マネージャ開始
   *
   * @param prop 設定プロパティ
   *
   * @else
   * @brief
   *
   * @param prop
   *
   * @endif
   */
  void OpenSpliceManager::start(coil::Properties &prop)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::start()"));
    RTC_INFO_STR((prop));
    m_factory = DDS::DomainParticipantFactory::get_instance();
    if (!checkHandle(m_factory.in(), "get_instance() failed"))
    {
      throw;
    }

    coil::stringTo<DDS::DomainId_t>(m_domain, prop["domain.id"].c_str());

    RTC_DEBUG(("Domain ID: %d", m_domain));

    DDS::DomainParticipantQos qos(PARTICIPANT_QOS_DEFAULT);

    std::string uri(prop["uri"]);
    std::string profile(prop["profile"]);
    if (!uri.empty() && !profile.empty())
    {
      RTC_DEBUG(("Create QosProvider: uri=%s, profile=%s", uri.c_str(), profile.c_str()));
      m_qos_provider = new DDS::QosProvider(uri.c_str(), profile.c_str());

      std::string participant_name(prop["participant_qos.name"]);
      DDS::ReturnCode_t ret(DDS::RETCODE_OK);
      if (participant_name.empty())
      {
        ret = m_qos_provider->get_participant_qos(qos, nullptr);
      }
      else
      {
        RTC_DEBUG(("Participant ID: %s", participant_name.c_str()));
        ret = m_qos_provider->get_participant_qos(qos, participant_name.c_str());
      }
      if (ret == DDS::RETCODE_OK)
      {
        RTC_INFO(("DomainParticipantQos initialisation successful"));
        m_participant = m_factory->create_participant(m_domain, qos, nullptr, DDS::STATUS_MASK_NONE);
      }
      else
      {
        RTC_INFO(("DomainParticipantQos initialisation failed"));
        throw;
      }
    }
    else
    {
      RTC_INFO(("DomainParticipantQos has been set to the default value."));

      qos.entity_factory.autoenable_created_entities = coil::toBool(prop["participant_qos.entity_factory.autoenable_created_entities"], "YES", "NO", qos.entity_factory.autoenable_created_entities);

      std::string listener_scheduling_scheduling_class_kind = prop["participant_qos.listener_scheduling.scheduling_class.kind"];

      if (listener_scheduling_scheduling_class_kind == "SCHEDULE_DEFAULT")
      {
        qos.listener_scheduling.scheduling_class.kind = DDS::SCHEDULE_DEFAULT;
      }
      else if (listener_scheduling_scheduling_class_kind == "SCHEDULE_TIMESHARING")
      {
        qos.listener_scheduling.scheduling_class.kind = DDS::SCHEDULE_TIMESHARING;
      }
      else if (listener_scheduling_scheduling_class_kind == "SCHEDULE_REALTIME")
      {
        qos.listener_scheduling.scheduling_class.kind = DDS::SCHEDULE_REALTIME;
      }

      coil::stringTo<DDS::Long>(qos.listener_scheduling.scheduling_priority, prop["participant_qos.listener_scheduling.scheduling_priority"].c_str());

      std::string listener_scheduling_scheduling_priority_kind_kind = prop["participant_qos.listener_scheduling.scheduling_priority_kind.kind"];

      if (listener_scheduling_scheduling_priority_kind_kind == "PRIORITY_RELATIVE")
      {
        qos.listener_scheduling.scheduling_priority_kind.kind = DDS::PRIORITY_RELATIVE;
      }
      else if (listener_scheduling_scheduling_priority_kind_kind == "PRIORITY_ABSOLUTE")
      {
        qos.listener_scheduling.scheduling_priority_kind.kind = DDS::PRIORITY_ABSOLUTE;
      }

      // qos.user_data.value;

      std::string watchdog_scheduling_scheduling_class_kind = prop["participant_qos.watchdog_scheduling.scheduling_class.kind"];

      if (watchdog_scheduling_scheduling_class_kind == "SCHEDULE_DEFAULT")
      {
        qos.watchdog_scheduling.scheduling_class.kind = DDS::SCHEDULE_DEFAULT;
      }
      else if (watchdog_scheduling_scheduling_class_kind == "SCHEDULE_TIMESHARING")
      {
        qos.watchdog_scheduling.scheduling_class.kind = DDS::SCHEDULE_TIMESHARING;
      }
      else if (watchdog_scheduling_scheduling_class_kind == "SCHEDULE_REALTIME")
      {
        qos.watchdog_scheduling.scheduling_class.kind = DDS::SCHEDULE_REALTIME;
      }

      coil::stringTo<DDS::Long>(qos.watchdog_scheduling.scheduling_priority, prop["participant_qos.watchdog_scheduling.scheduling_priority"].c_str());

      std::string watchdog_scheduling_scheduling_priority_kind_kind = prop["participant_qos.watchdog_scheduling.scheduling_priority_kind.kind"];

      if (watchdog_scheduling_scheduling_priority_kind_kind == "PRIORITY_RELATIVE")
      {
        qos.watchdog_scheduling.scheduling_priority_kind.kind = DDS::PRIORITY_RELATIVE;
      }
      else if (watchdog_scheduling_scheduling_priority_kind_kind == "PRIORITY_ABSOLUTE")
      {
        qos.watchdog_scheduling.scheduling_priority_kind.kind = DDS::PRIORITY_ABSOLUTE;
      }

      m_participant = m_factory->create_participant(m_domain, qos, nullptr, DDS::STATUS_MASK_NONE);
    }

    if (!checkHandle(m_participant.in(), "create_participant() failed"))
    {
      RTC_ERROR(("Domain participant creation failed"));
      throw;
    }

    RTC_INFO(("Participant created"));

    RTC_DEBUG(("DomainParticipantQos setting: participant.entity_factory.autoenable_created_entities: %s", (qos.entity_factory.autoenable_created_entities ? "true" : "false")));
    RTC_DEBUG(("DomainParticipantQos setting: participant.listener_scheduling.scheduling_class.kind: %d", qos.listener_scheduling.scheduling_class.kind));
    RTC_DEBUG(("DomainParticipantQos setting: participant.listener_scheduling.scheduling_priority: %d", qos.listener_scheduling.scheduling_priority));
    RTC_DEBUG(("DomainParticipantQos setting: participant.listener_scheduling.scheduling_priority_kind.kind: %d", qos.listener_scheduling.scheduling_priority_kind.kind));
    RTC_DEBUG(("DomainParticipantQos setting: participant.watchdog_scheduling.scheduling_class.kind: %d", qos.watchdog_scheduling.scheduling_class.kind));
    RTC_DEBUG(("DomainParticipantQos setting: participant.watchdog_scheduling.scheduling_priority: %d", qos.watchdog_scheduling.scheduling_priority));
    RTC_DEBUG(("DomainParticipantQos setting: participant.watchdog_scheduling.scheduling_priority_kind.kind: %d", qos.watchdog_scheduling.scheduling_priority_kind.kind));

    createPublisher(prop);
    createSubscriber(prop);
  }

  /*!
   * @if jp
   * @brief 終了処理
   * topic、publisher、subscriber、participantの削除を行う
   *
   *
   * @else
   * @brief
   *
   * @return
   *
   * @endif
   */
  void OpenSpliceManager::finalize()
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::finalize()"));
    DDS::ReturnCode_t result;

    if (m_participant.in() != nullptr)
    {
      if (m_subscriber.in() != nullptr)
      {
        result = m_participant->delete_subscriber(m_subscriber);
        RTC_INFO(("Deleted subscriber"));
        if (!checkStatus(result, "delete_subscriber() failed"))
        {
        }
      }
      if (m_publisher.in() != nullptr)
      {
        result = m_participant->delete_publisher(m_publisher);
        RTC_INFO(("Deleted publisher"));
        if (!checkStatus(result, "delete_publisher() failed"))
        {
        }
      }
      for (auto &topic : m_topics)
      {
        result = m_participant->delete_topic(topic.second);
        RTC_INFO(("Deleted topic: %s", topic.first.c_str()));
        if (!checkStatus(result, "delete_topic() failed"))
        {
        }
      }

      RTC_INFO(("Deleted participan"));
      result = m_factory->delete_participant(m_participant);

      if (!checkStatus(result, "delete_participant() failed"))
      {
        return;
      }
    }
  }

  /*!
   * @if jp
   * @brief Publisher生成
   *
   * @param prop 設定プロパティ
   *
   * @return true：生成成功、false：エラー
   *
   * @else
   * @brief create Publisher
   *
   * @param prop
   *
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::createPublisher(coil::Properties &prop)
  {

    if (m_publisher.in() != nullptr)
    {
      return true;
    }

    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::createPublisher()"));

    DDS::PublisherQos pQos;

    DDS::ReturnCode_t result = m_participant->get_default_publisher_qos(pQos);
    if (!checkStatus(result, "get_default_publisher_qos() failed"))
    {
      return false;
    }

    if (m_qos_provider.in() == nullptr)
    {
      RTC_INFO(("PublisherQos has been set to the default value."));
      pQos.entity_factory.autoenable_created_entities = coil::toBool(prop["publisher_qos.entity_factory.autoenable_created_entities"], "YES", "NO", pQos.entity_factory.autoenable_created_entities);

      // pQos.group_data.value;
      // pQos.partition.name;

      std::string presentation_access_scope = prop["publisher_qos.presentation.access_scope"];

      if (presentation_access_scope == "INSTANCE_PRESENTATION_QOS")
      {
        pQos.presentation.access_scope = DDS::INSTANCE_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "TOPIC_PRESENTATION_QOS")
      {
        pQos.presentation.access_scope = DDS::TOPIC_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "GROUP_PRESENTATION_QOS")
      {
        pQos.presentation.access_scope = DDS::GROUP_PRESENTATION_QOS;
      }

      pQos.presentation.coherent_access = coil::toBool(prop["publisher_qos.presentation.coherent_access"], "YES", "NO", pQos.presentation.coherent_access);

      pQos.presentation.ordered_access = coil::toBool(prop["publisher_qos.presentation.ordered_access"], "YES", "NO", pQos.presentation.ordered_access);
    }
    else
    {
      RTC_INFO(("QoSProvider set PublisherQos"));
      DDS::ReturnCode_t ret(DDS::RETCODE_OK);
      std::string pub_id(prop["publisher_qos.id"]);
      if (pub_id.empty())
      {
        ret = m_qos_provider->get_publisher_qos(pQos, nullptr);
      }
      else
      {
        RTC_DEBUG(("Publisher ID: %s", pub_id.c_str()));
        ret = m_qos_provider->get_publisher_qos(pQos, pub_id.c_str());
      }
      if (ret != DDS::RETCODE_OK)
      {
        RTC_ERROR(("PublisherQos initialisation failed: %s", RetCodeName[ret].c_str()));
        return false;
      }
    }

    RTC_DEBUG(("PublisherQos setting: publisher_qos.entity_factory.autoenable_created_entities: %s", (pQos.entity_factory.autoenable_created_entities ? "true" : "false")));
    RTC_DEBUG(("PublisherQos setting: publisher_qos.presentation.access_scope: %d", pQos.presentation.access_scope));
    RTC_DEBUG(("PublisherQos setting: publisher_qos.presentation.coherent_access: %s", (pQos.presentation.coherent_access ? "true" : "false")));
    RTC_DEBUG(("PublisherQos setting: publisher_qos.presentation.ordered_access: %s", (pQos.presentation.ordered_access ? "true" : "false")));

    m_publisher = m_participant->create_publisher(pQos, nullptr, DDS::STATUS_MASK_NONE);

    if (!checkHandle(m_publisher.in(), "create_publisher() failed"))
    {
      return false;
    }

    RTC_INFO(("Publisher created"));

    return true;
  }
  /*!
   * @if jp
   * @brief Subscriber生成
   *
   * @param prop 設定プロパティ
   *
   * @return true：生成成功、false：エラー
   *
   * @else
   * @brief create Subscriber
   *
   * @param prop
   *
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::createSubscriber(coil::Properties &prop)
  {
    if (m_subscriber.in() != nullptr)
    {
      return true;
    }

    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::createSubscriber()"));

    DDS::SubscriberQos sQos;

    DDS::ReturnCode_t result = m_participant->get_default_subscriber_qos(sQos);
    if (!checkStatus(result, "get_default_subscriber_qos() failed"))
    {
      return false;
    }

    if (m_qos_provider.in() == nullptr)
    {
      RTC_INFO(("SubscriberQos has been set to the default value."));
      sQos.entity_factory.autoenable_created_entities = coil::toBool(prop["subscriber_qos.entity_factory.autoenable_created_entities"], "YES", "NO", sQos.entity_factory.autoenable_created_entities);

      // sQos.group_data.value;
      // sQos.partition.name;

      std::string presentation_access_scope = prop["subscriber_qos.presentation.access_scope"];

      if (presentation_access_scope == "INSTANCE_PRESENTATION_QOS")
      {
        sQos.presentation.access_scope = DDS::INSTANCE_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "TOPIC_PRESENTATION_QOS")
      {
        sQos.presentation.access_scope = DDS::TOPIC_PRESENTATION_QOS;
      }
      else if (presentation_access_scope == "GROUP_PRESENTATION_QOS")
      {
        sQos.presentation.access_scope = DDS::GROUP_PRESENTATION_QOS;
      }

      sQos.presentation.coherent_access = coil::toBool(prop["subscriber_qos.presentation.coherent_access"], "YES", "NO", sQos.presentation.coherent_access);

      sQos.presentation.ordered_access = coil::toBool(prop["subscriber_qos.presentation.ordered_access"], "YES", "NO", sQos.presentation.ordered_access);

      sQos.share.enable = coil::toBool(prop["subscriber_qos.share.enable"], "YES", "NO", sQos.share.enable);
    }
    else
    {
      RTC_INFO(("QoSProvider set SubscriberQos"));
      DDS::ReturnCode_t ret(DDS::RETCODE_OK);
      std::string sub_id(prop["subscriber_qos.id"]);
      if (sub_id.empty())
      {
        ret = m_qos_provider->get_subscriber_qos(sQos, nullptr);
      }
      else
      {
        RTC_DEBUG(("Subscriber ID: %s", sub_id.c_str()));
        ret = m_qos_provider->get_subscriber_qos(sQos, sub_id.c_str());
      }
      if (ret != DDS::RETCODE_OK)
      {
        RTC_ERROR(("SubscriberQos initialisation failed: %s", RetCodeName[ret].c_str()));
        return false;
      }
    }

    RTC_DEBUG(("SubscriberQos setting: subscriber_qos.entity_factory.autoenable_created_entities: %s", (sQos.entity_factory.autoenable_created_entities ? "true" : "false")));
    RTC_DEBUG(("SubscriberQos setting: subscriber_qos.presentation.access_scope: %d", sQos.presentation.access_scope));
    RTC_DEBUG(("SubscriberQos setting: subscriber_qos.presentation.coherent_access: %s", (sQos.presentation.coherent_access ? "true" : "false")));
    RTC_DEBUG(("SubscriberQos setting: subscriber_qos.presentation.ordered_access: %s", (sQos.presentation.ordered_access ? "true" : "false")));
    RTC_DEBUG(("SubscriberQos setting: subscriber_qos.share.enable: %s", (sQos.share.enable ? "true" : "false")));

    // sQos.share.name;

    m_subscriber = m_participant->create_subscriber(sQos, nullptr, DDS::STATUS_MASK_NONE);
    if (!checkHandle(m_subscriber.in(), "create_subscriber() failed"))
    {
      return false;
    }

    RTC_INFO(("Subscriber created"));

    return true;
  }

  /*!
   * @if jp
   * @brief Participant取得
   *
   * @return Participant
   *
   * @else
   * @brief get Participant
   *
   * @return Participant
   *
   * @endif
   */
  DDS::DomainParticipant_ptr OpenSpliceManager::getParticipant()
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::getParticipant()"));
    return DDS::DomainParticipant::_duplicate(m_participant.in());
  }

  /*!
   * @if jp
   * @brief DataWriter生成
   *
   * @param topic_name トピック名
   * @param listener 書き込み時リスナ
   * @param prop 設定プロパティ
   *
   * @return DataWriter
   *
   * @else
   * @brief create DataWriter
   *
   * @param topic_name
   * @param listener
   * @param prop
   *
   * @return DataWriter
   *
   * @endif
   */
  DDS::DataWriter_ptr OpenSpliceManager::createWriter(const std::string &topic_name, DDS::DataWriterListener_ptr listener, coil::Properties &prop)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::createWriter(topic_name=%s)", topic_name.c_str()));

    if (m_topics.count(topic_name) == 0)
    {
      RTC_INFO(("Topic not found: %s", topic_name.c_str()));
      return nullptr;
    }

    DDS::DataWriterQos wQos;

    DDS::ReturnCode_t result = m_publisher->get_default_datawriter_qos(wQos);
    if (!checkStatus(result, "get_default_datawriter_qos() failed"))
    {
      return nullptr;
    }

    DDS::TopicQos tQos;
    result = m_topics[topic_name]->get_qos(tQos);
    if (!checkStatus(result, "get_qos() failed"))
    {
      return nullptr;
    }

    result = m_publisher->copy_from_topic_qos(wQos, tQos);
    if (!checkStatus(result, "copy_from_topic_qos() failed"))
    {
      return nullptr;
    }

    if (m_qos_provider.in() == nullptr)
    {
      RTC_INFO(("DataWriterQos has been set to the default value."));
      std::string durability_kind = prop["writer_qos.durability.kind"];

      if (durability_kind == "VOLATILE_DURABILITY_QOS")
      {
        wQos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
      {
        wQos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
      {
        wQos.durability.kind = DDS::TRANSIENT_DURABILITY_QOS;
      }
      else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
      {
        wQos.durability.kind = DDS::PERSISTENT_DURABILITY_QOS;
      }

      setDuration(prop.getNode("writer_qos.deadline.period"), wQos.deadline.period);

      setDuration(prop.getNode("writer_qos.latency_budget.duration"), wQos.latency_budget.duration);

      std::string liveliness_kind = prop["writer_qos.liveliness.kind"];

      if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
      {
        wQos.liveliness.kind = DDS::AUTOMATIC_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
      {
        wQos.liveliness.kind = DDS::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
      {
        wQos.liveliness.kind = DDS::MANUAL_BY_TOPIC_LIVELINESS_QOS;
      }

      setDuration(prop.getNode("writer_qos.liveliness.lease_duration"), wQos.liveliness.lease_duration);

      std::string reliability_kind = prop["writer_qos.reliability.kind"];

      if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
      {
        wQos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;
      }
      else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
      {
        wQos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
      }

      wQos.reliability.max_blocking_time = DDS::DURATION_INFINITE;
      setDuration(prop.getNode("writer_qos.reliability.max_blocking_time"), wQos.reliability.max_blocking_time);

      wQos.reliability.synchronous = coil::toBool(prop["writer_qos.reliability.synchronous"], "YES", "NO", wQos.reliability.synchronous);

      std::string destination_order_kind = prop["writer_qos.destination_order.kind"];

      if (destination_order_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        wQos.destination_order.kind = DDS::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
      }
      else if (destination_order_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        wQos.destination_order.kind = DDS::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
      }

      std::string history_kind = prop["writer_qos.history.kind"];

      if (history_kind == "KEEP_LAST_HISTORY_QOS")
      {
        wQos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
      }
      else if (history_kind == "KEEP_ALL_HISTORY_QOS")
      {
        wQos.history.kind = DDS::KEEP_ALL_HISTORY_QOS;
      }

      coil::stringTo<DDS::Long>(wQos.history.depth, prop["writer_qos.history.depth"].c_str());

      coil::stringTo<DDS::Long>(wQos.resource_limits.max_samples, prop["writer_qos.resource_limits.max_samples"].c_str());
      coil::stringTo<DDS::Long>(wQos.resource_limits.max_instances, prop["writer_qos.resource_limits.max_instances"].c_str());
      coil::stringTo<DDS::Long>(wQos.resource_limits.max_samples_per_instance, prop["writer_qos.resource_limits.max_samples_per_instance"].c_str());

      coil::stringTo<DDS::Long>(wQos.transport_priority.value, prop["writer_qos.transport_priority.value"].c_str());

      setDuration(prop.getNode("writer_qos.lifespan.duration"), wQos.lifespan.duration);

      std::string ownership_kind = prop["writer_qos.ownership.kind"];

      if (ownership_kind == "SHARED_OWNERSHIP_QOS")
      {
        wQos.ownership.kind = DDS::SHARED_OWNERSHIP_QOS;
      }
      else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
      {
        wQos.ownership.kind = DDS::EXCLUSIVE_OWNERSHIP_QOS;
      }

      coil::stringTo<DDS::Long>(wQos.ownership_strength.value, prop["writer_qos.ownership_strength.value"].c_str());

      wQos.writer_data_lifecycle.autodispose_unregistered_instances = coil::toBool(prop["writer_qos.writer_data_lifecycle.autodispose_unregistered_instances"],
                                                                                   "YES", "NO", wQos.writer_data_lifecycle.autodispose_unregistered_instances);

      setDuration(prop.getNode("writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay"), wQos.writer_data_lifecycle.autopurge_suspended_samples_delay);
      setDuration(prop.getNode("writer_qos.writer_data_lifecycle.autounregister_instance_delay"), wQos.writer_data_lifecycle.autounregister_instance_delay);

      // wQos.writer_data_lifecycle.autodispose_unregistered_instances = false;
    }
    else
    {
      DDS::ReturnCode_t ret(DDS::RETCODE_OK);
      std::string writer_id(prop["writer_qos.id"]);
      if (writer_id.empty())
      {
        ret = m_qos_provider->get_datawriter_qos(wQos, nullptr);
      }
      else
      {
        RTC_DEBUG(("Writer ID: %s", writer_id.c_str()));
        ret = m_qos_provider->get_datawriter_qos(wQos, writer_id.c_str());
      }
      if (ret != DDS::RETCODE_OK)
      {
        RTC_INFO(("QoSProvider set DataWriterQos"));
        RTC_ERROR(("DataWriterQos initialisation failed: %s", RetCodeName[ret].c_str()));
        return nullptr;
      }
    }

    RTC_DEBUG(("DataWriterQos setting: writer_qos.durability.kind: %d", wQos.durability.kind));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.deadline.period: sec=%d nanosec=%u", wQos.deadline.period.sec, wQos.deadline.period.nanosec));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.latency_budget.duration: sec=%d nanosec=%u", wQos.latency_budget.duration.sec, wQos.latency_budget.duration.nanosec));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.liveliness.kind: %d", wQos.liveliness.kind));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.liveliness.lease_duration: sec=%d nanosec=%u", wQos.liveliness.lease_duration.sec, wQos.liveliness.lease_duration.nanosec));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.reliability.kind: %d", wQos.reliability.kind));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.reliability.max_blocking_time: sec=%d nanosec=%u", wQos.reliability.max_blocking_time.sec, wQos.reliability.max_blocking_time.nanosec));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.reliability.synchronous: %s", (wQos.reliability.synchronous ? "true" : "false")));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.destination_order.kind: %d", wQos.destination_order.kind));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.history.kind: %d", wQos.history.kind));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.history.depth: %d", wQos.history.depth));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.resource_limits.max_samples: %d", wQos.resource_limits.max_samples));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.resource_limits.max_instances: %d", wQos.resource_limits.max_instances));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.resource_limits.max_samples_per_instance: %d", wQos.resource_limits.max_samples_per_instance));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.transport_priority.value: %d", wQos.transport_priority.value));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.lifespan.duration: sec=%d nanosec=%u", wQos.lifespan.duration.sec, wQos.lifespan.duration.nanosec));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.ownership.kind: %d", wQos.ownership.kind));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.ownership_strength.value: %d", wQos.ownership_strength.value));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.writer_data_lifecycle.autodispose_unregistered_instances: %s", (wQos.writer_data_lifecycle.autodispose_unregistered_instances ? "true" : "false")));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay: sec=%d nanosec=%u", wQos.writer_data_lifecycle.autopurge_suspended_samples_delay.sec, wQos.writer_data_lifecycle.autopurge_suspended_samples_delay.nanosec));
    RTC_DEBUG(("DataWriterQos setting: writer_qos.writer_data_lifecycle.autounregister_instance_delay: sec=%d nanosec=%u", wQos.writer_data_lifecycle.autounregister_instance_delay.sec, wQos.writer_data_lifecycle.autounregister_instance_delay.nanosec));

    DDS::DataWriter_var writer = m_publisher->create_datawriter(m_topics[topic_name].in(), wQos, listener, DDS::STATUS_MASK_NONE);
    if (!checkHandle(writer.in(), "create_datawriter() failed"))
    {
      return nullptr;
    }

    RTC_INFO(("Writer created: %s", topic_name.c_str()));

    return DDS::DataWriter::_duplicate(writer.in());
  }

  /*!
   * @if jp
   * @brief DataReader生成
   *
   * @param topic_name トピック名
   * @param listener 読み込み時リスナ
   * @param prop 設定プロパティ
   *
   * @return DataReader
   *
   * @else
   * @brief create DataReader
   *
   * @param topic_name
   * @param listener
   * @param prop
   *
   * @return DataReader
   *
   * @endif
   */
  DDS::DataReader_ptr OpenSpliceManager::createReader(const std::string &topic_name, DDS::DataReaderListener_ptr listener, coil::Properties &prop)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::createReader(topic_name=%s)", topic_name.c_str()));
    if (m_topics.count(topic_name) == 0)
    {
      return nullptr;
    }

    if (!checkHandle(m_topics[topic_name].in(), "find_topic() failed"))
    {
      return nullptr;
    }
    DDS::DataReaderQos rQos;
    DDS::ReturnCode_t result = m_subscriber->get_default_datareader_qos(rQos);
    if (!checkStatus(result, "get_default_datareader_qos() failed"))
    {
      return nullptr;
    }

    DDS::TopicQos tQos;
    result = m_topics[topic_name]->get_qos(tQos);
    if (!checkStatus(result, "get_default_datareader_qos() failed"))
    {
      return nullptr;
    }

    result = m_subscriber->copy_from_topic_qos(rQos, tQos);
    if (!checkStatus(result, "copy_from_topic_qos() failed"))
    {
      return nullptr;
    }

    if (m_qos_provider.in() == nullptr)
    {
      RTC_INFO(("DataReaderQos has been set to the default value."));
      std::string durability_kind = prop["reader_qos.durability.kind"];

      if (durability_kind == "VOLATILE_DURABILITY_QOS")
      {
        rQos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
      {
        rQos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
      {
        rQos.durability.kind = DDS::TRANSIENT_DURABILITY_QOS;
      }
      else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
      {
        rQos.durability.kind = DDS::PERSISTENT_DURABILITY_QOS;
      }

      setDuration(prop.getNode("reader_qos.deadline.period"), rQos.deadline.period);

      setDuration(prop.getNode("reader_qos.latency_budget.duration"), rQos.latency_budget.duration);

      std::string liveliness_kind = prop["reader_qos.liveliness.kind"];

      if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
      {
        rQos.liveliness.kind = DDS::AUTOMATIC_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
      {
        rQos.liveliness.kind = DDS::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
      {
        rQos.liveliness.kind = DDS::MANUAL_BY_TOPIC_LIVELINESS_QOS;
      }

      setDuration(prop.getNode("reader_qos.liveliness.lease_duration"), rQos.liveliness.lease_duration);

      std::string reliability_kind = prop["reader_qos.reliability.kind"];

      if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
      {
        rQos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;
      }
      else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
      {
        rQos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
      }

      rQos.reliability.max_blocking_time = DDS::DURATION_INFINITE;
      setDuration(prop.getNode("reader_qos.reliability.max_blocking_time"), rQos.reliability.max_blocking_time);

      rQos.reliability.synchronous = coil::toBool(prop["reader_qos.reliability.synchronous"], "YES", "NO", rQos.reliability.synchronous);

      std::string destination_order_kind = prop["reader_qos.destination_order.kind"];

      if (destination_order_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        rQos.destination_order.kind = DDS::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
      }
      else if (destination_order_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        rQos.destination_order.kind = DDS::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
      }

      std::string history_kind = prop["reader_qos.history.kind"];

      if (history_kind == "KEEP_LAST_HISTORY_QOS")
      {
        rQos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
      }
      else if (history_kind == "KEEP_ALL_HISTORY_QOS")
      {
        rQos.history.kind = DDS::KEEP_ALL_HISTORY_QOS;
      }

      coil::stringTo<DDS::Long>(rQos.history.depth, prop["reader_qos.history.depth"].c_str());

      coil::stringTo<DDS::Long>(rQos.resource_limits.max_samples, prop["reader_qos.resource_limits.max_samples"].c_str());
      coil::stringTo<DDS::Long>(rQos.resource_limits.max_instances, prop["reader_qos.resource_limits.max_instances"].c_str());
      coil::stringTo<DDS::Long>(rQos.resource_limits.max_samples_per_instance, prop["reader_qos.resource_limits.max_samples_per_instance"].c_str());

      std::string ownership_kind = prop["reader_qos.ownership.kind"];

      if (ownership_kind == "SHARED_OWNERSHIP_QOS")
      {
        rQos.ownership.kind = DDS::SHARED_OWNERSHIP_QOS;
      }
      else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
      {
        rQos.ownership.kind = DDS::EXCLUSIVE_OWNERSHIP_QOS;
      }

      setDuration(prop.getNode("reader_qos.time_based_filter.minimum_separation"), rQos.time_based_filter.minimum_separation);

      setDuration(prop.getNode("reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay"), rQos.reader_data_lifecycle.autopurge_disposed_samples_delay);

      rQos.reader_data_lifecycle.autopurge_dispose_all = coil::toBool(prop["reader_qos.reader_data_lifecycle.autopurge_dispose_all"], "YES", "NO", rQos.reader_data_lifecycle.autopurge_dispose_all);

      setDuration(prop.getNode("reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay"), rQos.reader_data_lifecycle.autopurge_nowriter_samples_delay);

      rQos.reader_data_lifecycle.enable_invalid_samples = coil::toBool(prop["reader_qos.reader_data_lifecycle.enable_invalid_samples"], "YES", "NO", rQos.reader_data_lifecycle.enable_invalid_samples);

      std::string reader_data_lifecycle_invalid_sample_visibility_kind = prop["reader_qos.reader_data_lifecycle.invalid_sample_visibility.kind"];

      if (ownership_kind == "NO_INVALID_SAMPLES")
      {
        rQos.reader_data_lifecycle.invalid_sample_visibility.kind = DDS::NO_INVALID_SAMPLES;
      }
      else if (ownership_kind == "MINIMUM_INVALID_SAMPLES")
      {
        rQos.reader_data_lifecycle.invalid_sample_visibility.kind = DDS::MINIMUM_INVALID_SAMPLES;
      }
      else if (ownership_kind == "ALL_INVALID_SAMPLES")
      {
        rQos.reader_data_lifecycle.invalid_sample_visibility.kind = DDS::ALL_INVALID_SAMPLES;
      }
    }
    else
    {
      RTC_INFO(("QoSProvider set DataReaderQos"));
      DDS::ReturnCode_t ret(DDS::RETCODE_OK);
      std::string reader_id(prop["reader_qos.id"]);
      if (reader_id.empty())
      {
        ret = m_qos_provider->get_datareader_qos(rQos, nullptr);
      }
      else
      {
        RTC_DEBUG(("Reader ID: %s", reader_id.c_str()));
        ret = m_qos_provider->get_datareader_qos(rQos, reader_id.c_str());
      }
      if (ret != DDS::RETCODE_OK)
      {
        RTC_ERROR(("DataReaderQos initialisation failed: %s", RetCodeName[ret].c_str()));
        return nullptr;
      }
    }

    RTC_DEBUG(("DataReaderQos setting: reader_qos.durability.kind: %d", rQos.durability.kind));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.deadline.period: sec=%d nanosec=%u", rQos.deadline.period.sec, rQos.deadline.period.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.latency_budget.duration: sec=%d nanosec=%u", rQos.latency_budget.duration.sec, rQos.latency_budget.duration.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.liveliness.kind: %d", rQos.liveliness.kind));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.liveliness.lease_duration: sec=%d nanosec=%u", rQos.liveliness.lease_duration.sec, rQos.liveliness.lease_duration.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reliability.kind: %d", rQos.reliability.kind));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reliability.max_blocking_time: sec=%d nanosec=%u", rQos.reliability.max_blocking_time.sec, rQos.reliability.max_blocking_time.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reliability.synchronous: %s", (rQos.reliability.synchronous ? "true" : "false")));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.destination_order.kind: %d", rQos.destination_order.kind));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.history.kind: %d", rQos.history.kind));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.history.depth: %d", rQos.history.depth));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.resource_limits.max_samples: %d", rQos.resource_limits.max_samples));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.resource_limits.max_instances: %d", rQos.resource_limits.max_instances));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.resource_limits.max_samples_per_instance: %d", rQos.resource_limits.max_samples_per_instance));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.ownership.kind: %d", rQos.ownership.kind));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.time_based_filter.minimum_separation: sec=%d nanosec=%u", rQos.time_based_filter.minimum_separation.sec, rQos.time_based_filter.minimum_separation.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay: sec=%d nanosec=%u", rQos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec, rQos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reader_data_lifecycle.autopurge_dispose_all: %s", (rQos.reader_data_lifecycle.autopurge_dispose_all ? "true" : "false")));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay: sec=%d nanosec=%u", rQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec, rQos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reader_data_lifecycle.enable_invalid_samples: %s", (rQos.reader_data_lifecycle.enable_invalid_samples ? "true" : "false")));
    RTC_DEBUG(("DataReaderQos setting: reader_qos.reader_data_lifecycle.invalid_sample_visibility.kind: %d", rQos.reader_data_lifecycle.invalid_sample_visibility.kind));

    DDS::StatusMask mask =
        DDS::DATA_AVAILABLE_STATUS | DDS::REQUESTED_DEADLINE_MISSED_STATUS;

    DDS::DataReader_var reader = m_subscriber->create_datareader(m_topics[topic_name].in(), rQos, listener, mask);
    if (!checkHandle(reader.in(), "create_datareader() failed"))
    {
      return nullptr;
    }

    RTC_INFO(("Reader created: %s", topic_name.c_str()));

    return DDS::DataReader::_duplicate(reader.in());
  }
  /*!
   * @if jp
   * @brief DataWriter削除
   *
   * @return DataWriter
   *
   * @else
   * @brief delete DataWriter
   *
   * @return DataWriter
   *
   * @endif
   */
  DDS::ReturnCode_t OpenSpliceManager::deleteWriter(DDS::DataWriter_ptr writer)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::deleteWriter()"));
    return m_publisher->delete_datawriter(writer);
  }
  /*!
   * @if jp
   * @brief DataReader削除
   *
   * @return DataReader
   *
   * @else
   * @brief delete DataReader
   *
   * @return DataReader
   *
   * @endif
   */
  DDS::ReturnCode_t OpenSpliceManager::deleteReader(DDS::DataReader_ptr reader)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::deleteReader()"));
    return m_subscriber->delete_datareader(reader);
  }
  /*!
   * @if jp
   * @brief トピック生成
   *
   * @param prop 設定プロパティ
   *
   * @return true：生成成功
   *
   * @else
   * @brief create Topic
   *
   * @param prop
   *
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::createTopic(const std::string &topic_name, const std::string &typeName, coil::Properties &prop)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::createTopic(topic_name=%s, typeName=%s)", topic_name.c_str(), typeName.c_str()));

    DDS::ReturnCode_t result;
    DDS::TopicQos tQos;
    result = m_participant->get_default_topic_qos(tQos);
    if (!checkStatus(result, "get_default_topic_qos() failed"))
    {
      return false;
    }

    if (m_qos_provider.in() == nullptr)
    {
      RTC_INFO(("TopicQos has been set to the default value."));
      tQos.durability.kind = DDS::TRANSIENT_DURABILITY_QOS;
      std::string durability_kind = prop["topic_qos.durability.kind"];

      if (durability_kind == "VOLATILE_DURABILITY_QOS")
      {
        tQos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_LOCAL_DURABILITY_QOS")
      {
        tQos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;
      }
      else if (durability_kind == "TRANSIENT_DURABILITY_QOS")
      {
        tQos.durability.kind = DDS::TRANSIENT_DURABILITY_QOS;
      }
      else if (durability_kind == "PERSISTENT_DURABILITY_QOS")
      {
        tQos.durability.kind = DDS::PERSISTENT_DURABILITY_QOS;
      }

      setDuration(prop.getNode("topic_qos.deadline.period"), tQos.deadline.period);

      setDuration(prop.getNode("topic_qos.latency_budget.duration"), tQos.latency_budget.duration);

      std::string liveliness_kind = prop["topic_qos.liveliness.kind"];

      if (liveliness_kind == "AUTOMATIC_LIVELINESS_QOS")
      {
        tQos.liveliness.kind = DDS::AUTOMATIC_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS")
      {
        tQos.liveliness.kind = DDS::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
      }
      else if (liveliness_kind == "MANUAL_BY_TOPIC_LIVELINESS_QOS")
      {
        tQos.liveliness.kind = DDS::MANUAL_BY_TOPIC_LIVELINESS_QOS;
      }

      setDuration(prop.getNode("topic_qos.liveliness.lease_duration"), tQos.liveliness.lease_duration);

      tQos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
      std::string reliability_kind = prop["topic_qos.reliability.kind"];

      if (reliability_kind == "BEST_EFFORT_RELIABILITY_QOS")
      {
        tQos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;
      }
      else if (reliability_kind == "RELIABLE_RELIABILITY_QOS")
      {
        tQos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
      }

      tQos.reliability.max_blocking_time = DDS::DURATION_INFINITE;
      setDuration(prop.getNode("topic_qos.reliability.max_blocking_time"), tQos.reliability.max_blocking_time);

      tQos.reliability.synchronous = coil::toBool(prop["topic_qos.reliability.synchronous"], "YES", "NO", tQos.reliability.synchronous);

      std::string destination_order_kind = prop["topic_qos.destination_order.kind"];

      if (destination_order_kind == "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        tQos.destination_order.kind = DDS::BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS;
      }
      else if (destination_order_kind == "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS")
      {
        tQos.destination_order.kind = DDS::BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS;
      }

      std::string history_kind = prop["topic_qos.history.kind"];

      if (history_kind == "KEEP_LAST_HISTORY_QOS")
      {
        tQos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
      }
      else if (history_kind == "KEEP_ALL_HISTORY_QOS")
      {
        tQos.history.kind = DDS::KEEP_ALL_HISTORY_QOS;
      }

      coil::stringTo<DDS::Long>(tQos.history.depth, prop["topic_qos.history.depth"].c_str());

      coil::stringTo<DDS::Long>(tQos.resource_limits.max_samples, prop["topic_qos.resource_limits.max_samples"].c_str());
      coil::stringTo<DDS::Long>(tQos.resource_limits.max_instances, prop["topic_qos.resource_limits.max_instances"].c_str());
      coil::stringTo<DDS::Long>(tQos.resource_limits.max_samples_per_instance, prop["topic_qos.resource_limits.max_samples_per_instance"].c_str());

      coil::stringTo<DDS::Long>(tQos.transport_priority.value, prop["topic_qos.transport_priority.value"].c_str());

      setDuration(prop.getNode("topic_qos.lifespan.duration"), tQos.lifespan.duration);

      std::string ownership_kind = prop["topic_qos.ownership.kind"];

      if (ownership_kind == "SHARED_OWNERSHIP_QOS")
      {
        tQos.ownership.kind = DDS::SHARED_OWNERSHIP_QOS;
      }
      else if (ownership_kind == "EXCLUSIVE_OWNERSHIP_QOS")
      {
        tQos.ownership.kind = DDS::EXCLUSIVE_OWNERSHIP_QOS;
      }

      // tQos.topic_data.value
      coil::stringTo<DDS::Long>(tQos.durability_service.history_depth, prop["topic_qos.durability_service.history_depth"].c_str());

      std::string durability_service_history_kind = prop["topic_qos.durability_service.history_kind"];

      if (durability_service_history_kind == "KEEP_LAST_HISTORY_QOS")
      {
        tQos.durability_service.history_kind = DDS::KEEP_LAST_HISTORY_QOS;
      }
      else if (durability_service_history_kind == "KEEP_ALL_HISTORY_QOS")
      {
        tQos.durability_service.history_kind = DDS::KEEP_ALL_HISTORY_QOS;
      }

      coil::stringTo<DDS::Long>(tQos.durability_service.max_instances, prop["topic_qos.durability_service.max_instances"].c_str());
      coil::stringTo<DDS::Long>(tQos.durability_service.max_samples, prop["topic_qos.durability_service.max_samples"].c_str());
      coil::stringTo<DDS::Long>(tQos.durability_service.max_samples_per_instance, prop["topic_qos.durability_service.max_samples_per_instance"].c_str());
      setDuration(prop.getNode("topic_qos.durability_service.service_cleanup_delay"), tQos.durability_service.service_cleanup_delay);
    }
    else
    {
      RTC_INFO(("QoSProvider set TopicQos"));
      DDS::ReturnCode_t ret(DDS::RETCODE_OK);
      std::string topic_id(prop["topic_qos.id"]);
      if (topic_id.empty())
      {
        ret = m_qos_provider->get_topic_qos(tQos, nullptr);
      }
      else
      {
        RTC_DEBUG(("Topic ID: %s", topic_id.c_str()));
        ret = m_qos_provider->get_topic_qos(tQos, topic_id.c_str());
      }
      if (ret != DDS::RETCODE_OK)
      {
        return false;
      }
    }

    RTC_DEBUG(("TopicQos setting: topic_qos.durability.kind: %d", tQos.durability.kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.deadline.period: sec=%d nanosec=%u", tQos.deadline.period.sec, tQos.deadline.period.nanosec));
    RTC_DEBUG(("TopicQos setting: topic_qos.latency_budget.duration: sec=%d nanosec=%u", tQos.latency_budget.duration.sec, tQos.latency_budget.duration.nanosec));
    RTC_DEBUG(("TopicQos setting: topic_qos.liveliness.kind: %d", tQos.liveliness.kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.liveliness.lease_duration: sec=%d nanosec=%u", tQos.liveliness.lease_duration.sec, tQos.liveliness.lease_duration.nanosec));
    RTC_DEBUG(("TopicQos setting: topic_qos.reliability.kind: %d", tQos.reliability.kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.reliability.max_blocking_time: sec=%d nanosec=%u", tQos.reliability.max_blocking_time.sec, tQos.reliability.max_blocking_time.nanosec));
    RTC_DEBUG(("TopicQos setting: topic_qos.reliability.synchronous: %s", (tQos.reliability.synchronous ? "true" : "false")));
    RTC_DEBUG(("TopicQos setting: topic_qos.destination_order.kind: %d", tQos.destination_order.kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.history.kind: %d", tQos.history.kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.history.depth: %d", tQos.history.depth));
    RTC_DEBUG(("TopicQos setting: topic_qos.resource_limits.max_samples: %d", tQos.resource_limits.max_samples));
    RTC_DEBUG(("TopicQos setting: topic_qos.resource_limits.max_instances: %d", tQos.resource_limits.max_instances));
    RTC_DEBUG(("TopicQos setting: topic_qos.resource_limits.max_samples_per_instance: %d", tQos.resource_limits.max_samples_per_instance));
    RTC_DEBUG(("TopicQos setting: topic_qos.transport_priority.value: %d", tQos.transport_priority.value));
    RTC_DEBUG(("TopicQos setting: topic_qos.lifespan.duration: sec=%d nanosec=%u", tQos.lifespan.duration.sec, tQos.lifespan.duration.nanosec));
    RTC_DEBUG(("TopicQos setting: topic_qos.ownership.kind: %d", tQos.ownership.kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.durability_service.history_depth: %d", tQos.durability_service.history_depth));
    RTC_DEBUG(("TopicQos setting: topic_qos.durability_service.history_kind: %d", tQos.durability_service.history_kind));
    RTC_DEBUG(("TopicQos setting: topic_qos.durability_service.max_instances: %d", tQos.durability_service.max_instances));
    RTC_DEBUG(("TopicQos setting: topic_qos.durability_service.max_samples: %d", tQos.durability_service.max_samples));
    RTC_DEBUG(("TopicQos setting: topic_qos.durability_service.max_samples_per_instance: %d", tQos.durability_service.max_samples_per_instance));
    RTC_DEBUG(("TopicQos setting: topic_qos.durability_service.service_cleanup_delay: sec=%d nanosec=%u", tQos.durability_service.service_cleanup_delay.sec, tQos.durability_service.service_cleanup_delay.nanosec));

    DDS::Topic_var topic = m_participant->create_topic(topic_name.c_str(), typeName.c_str(), tQos, nullptr, DDS::STATUS_MASK_NONE);

    if (!checkHandle(topic.in(), "create_topic() failed"))
    {
      return false;
    }

    m_topics[topic_name] = DDS::Topic::_duplicate(topic.in());
    RTC_INFO(("Topic created: %s", topic_name.c_str()));
    return true;
  }

  /*!
   * @if jp
   * @brief 型の登録
   *
   * @param type 登録するデータのインスタンス
   * @return true：登録成功、false：登録失敗
   *
   * @else
   * @brief
   *
   * @param type
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::registerType(const std::string &datatype, const std::string &idlpath)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::registerType(datatype=%s, idlpath=%s)", datatype.c_str(), idlpath.c_str()));
    if (m_typesupports.count(datatype))
    {
      return true;
    }

    coil::vstring comout;
    std::string comin = "idlpp -l pythondesc ";

    std::ifstream infile;
    infile.open(idlpath.c_str(), std::ios::in);

    if (infile.fail())
    {
      return false;
    }

    comin.append(idlpath);

    RTC_DEBUG(("create_process(%s)", comin.c_str()));
    coil::create_process(comin, comout);

    std::string tmp;
    for (auto const &c : comout)
    {
      tmp.append(c);
    }
    std::string xmlstr{coil::eraseBothEndsBlank(std::move(tmp))};

    rapidxml::xml_document<> doc;
    try
    {
      doc.parse<rapidxml::parse_trim_whitespace>(const_cast<char *>(xmlstr.c_str()));
    }
    catch (rapidxml::parse_error &)
    {
      return false;
    }

    rapidxml::xml_node<> *topics = doc.first_node("topics");

    DDS::String_var id_;
    DDS::String_var keys_;
    DDS::String_var descriptor_;
    if (topics != nullptr)
    {
      for (auto &topictype : children<>(topics))
      {
        rapidxml::xml_node<> *id = topictype.first_node("id");
        rapidxml::xml_node<> *keys = topictype.first_node("keys");
        rapidxml::xml_node<> *descriptor = topictype.first_node("descriptor");

        if (id != nullptr && keys != nullptr && descriptor != nullptr)
        {
          rapidxml::xml_node<> *child = descriptor->first_node();
          if (child != nullptr)
          {
            if (strncmp(id->value(), datatype.c_str(), id->value_size()) == 0)
            {
              id_ = DDS::string_dup(id->value());
              keys_ = DDS::string_dup(keys->value());
              descriptor_ = DDS::string_dup(child->value());
            }
          }
        }
      }
    }

    if (id_.in() == nullptr)
    {
      RTC_ERROR(("Registration failure(%s)", datatype.c_str()));
      return false;
    }

    if (!checkHandle(m_participant.in(), "lookup_participant() failed"))
    {
      return false;
    }

    OpenRTM_OpenSplice::CORBACdrDataTypeSupport_var typesupport = new OpenRTM_OpenSplice::CORBACdrDataTypeSupport(id_.inout(), keys_.inout(), descriptor_.inout());

    DDS::String_var typeName = typesupport->get_type_name();
    DDS::ReturnCode_t result = typesupport->register_type(m_participant.in(), typeName.in());

    if (!checkStatus(result, "register_type() failed"))
    {
      return false;
    }
    m_typesupports[datatype] = OpenRTM_OpenSplice::CORBACdrDataTypeSupport::_duplicate(typesupport.in());
    RTC_INFO(("%s has been registered.", datatype.c_str()));
    return true;
  }

  /*!
   * @if jp
   * @brief 型の登録解除
   *
   * @param name 型名
   * @return true：解除成功、false：解除失敗
   *
   * @else
   * @brief
   *
   * @param name
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::unregisterType(const std::string & /*name*/)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::unregisterType()"));
    return false;
  }

  /*!
   * @if jp
   * @brief 型が登録済みかを確認
   *
   * @param name 型名
   * @return true：登録済み、false：未登録
   *
   * @else
   * @brief
   *
   * @param name
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::registeredType(const std::string &name)
  {
    Logger rtclog("OpenSpliceManager");
    RTC_INFO(("OpenSpliceManager::registeredType(%s)", name.c_str()));
    if (m_typesupports.count(name) == 0)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  /*!
   * @if jp
   * @brief 初期化関数
   *
   * @param prop 設定プロパティ
   *
   * @return インスタンス
   *
   * @else
   * @brief
   *
   * @param prop
   *
   * @return
   *
   *
   * @endif
   */
  OpenSpliceManager *OpenSpliceManager::init(coil::Properties &prop)
  {
    std::call_once(m_once, [&]
                   {
      manager = new OpenSpliceManager();
      manager->start(prop); });
    return manager;
  }

  /*!
   * @if jp
   * @brief インスタンス取得
   *
   * @param prop 設定プロパティ
   *
   * @return インスタンス
   *
   * @else
   * @brief
   *
   * @param prop
   *
   * @return
   *
   *
   * @endif
   */
  OpenSpliceManager &OpenSpliceManager::instance()
  {
    std::call_once(m_once, [&]
                   {
      manager = new OpenSpliceManager();
      coil::Properties prop;
      manager->start(prop); });
    return *manager;
  }

  /*!
   * @if jp
   * @brief OpenSpliceManagerが初期化されている場合に終了処理を呼び出す
   *
   *
   * @else
   * @brief
   *
   * @return
   *
   *
   * @endif
   */
  void OpenSpliceManager::shutdown_global()
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (manager)
    {
      manager->finalize();
    }
  }

  /*!
   * @if jp
   * @brief プロパティからDDS::Durationを設定する
   *
   * @param prop プロパティ(sec、nanosecの要素に値を格納する)
   * @param time DDS::Duration
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
  void OpenSpliceManager::setDuration(coil::Properties &prop, DDS::Duration_t &time)
  {
    std::string sec_str = prop["sec"];
    std::string nanosec_str = prop["nanosec"];

    if (sec_str == "2147483647" && nanosec_str == "2147483647")
    {
      time = DDS::DURATION_INFINITE;
      return;
    }
    if (sec_str == "0" && nanosec_str == "0")
    {
      time = DDS::DURATION_ZERO;
      return;
    }

    coil::stringTo<DDS::Long>(time.sec, sec_str.c_str());
    coil::stringTo<DDS::ULong>(time.nanosec, nanosec_str.c_str());
  }

}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif