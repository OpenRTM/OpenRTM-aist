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

#include <coil/process.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <rapidxml_iterators.hpp>
#include <iostream>
#include <fstream>


namespace RTC
{
  OpenSpliceManager* OpenSpliceManager::manager = nullptr;
  std::mutex OpenSpliceManager::mutex;

  template<typename Ch = char>
  class children {
      using iterator_type = rapidxml::node_iterator<Ch>;
  public:
      explicit children(rapidxml::xml_node<Ch>* node) : node_(node) {}
      iterator_type begin() const { return iterator_type(node_); }
      iterator_type end() const { return iterator_type(); }
  private:
      rapidxml::xml_node<Ch>* node_;
  };


  static const std::string RetCodeName[13] =
  {
      "DDS_RETCODE_OK", "DDS_RETCODE_ERROR", "DDS_RETCODE_UNSUPPORTED",
      "DDS_RETCODE_BAD_PARAMETER", "DDS_RETCODE_PRECONDITION_NOT_MET",
      "DDS_RETCODE_OUT_OF_RESOURCES", "DDS_RETCODE_NOT_ENABLED",
      "DDS_RETCODE_IMMUTABLE_POLICY", "DDS_RETCODE_INCONSISTENT_POLICY",
      "DDS_RETCODE_ALREADY_DELETED", "DDS_RETCODE_TIMEOUT", "DDS_RETCODE_NO_DATA",
      "DDS_RETCODE_ILLEGAL_OPERATION"
  };

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
      if (status != DDS::RETCODE_OK && status != DDS::RETCODE_NO_DATA) {
          std::cerr << "Error in " << info << " with return code : " << RetCodeName[status].c_str() << std::endl;
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
  bool OpenSpliceManager::checkHandle(void *handle, const char* info)
  {
      if (!handle) {
          std::cerr << "Error in " << info << ": Creation failed: invalid handle" << std::endl;
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
  OpenSpliceManager::OpenSpliceManager()// : rtclog("OpenSpliceManager")
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
  OpenSpliceManager::OpenSpliceManager(const OpenSpliceManager &/*mgr*/)// : rtclog("OpenSpliceManager")
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
   *
   * @else
   * @brief 
   *
   * 
   *
   * @endif
   */
  void OpenSpliceManager::start()
  {

      m_factory = DDS::DomainParticipantFactory::get_instance();
      if(!checkHandle(m_factory.in(), "get_instance() failed"))
      {
          return;
      }
      
      m_domain = DDS::DOMAIN_ID_DEFAULT;
      m_participant = m_factory->create_participant(m_domain, PARTICIPANT_QOS_DEFAULT, nullptr, DDS::STATUS_MASK_NONE);
      if(!checkHandle(m_participant.in(), "create_participant() failed"))
      {
          return;
      }

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
      DDS::ReturnCode_t result;

      if (m_participant.in() != nullptr)
      {
          if (m_subscriber.in() != nullptr)
          {
              result = m_participant->delete_subscriber(m_subscriber);
              checkStatus(result, "delete_subscriber() failed");
          }
          if (m_publisher.in() != nullptr)
          {
              result = m_participant->delete_publisher(m_publisher);
              if (!checkStatus(result, "delete_publisher() failed"))
              {
                  return;
              }
          }
          for (auto& topic : m_topics)
          {
              result = m_participant->delete_topic(topic.second);
              if (!checkStatus(result, "delete_topic() failed"))
              {
              }
          }
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
 * @return true：生成成功、false：エラー
 *
 * @else
 * @brief create Publisher
 *
 * @return
 *
 * @endif
 */
  bool OpenSpliceManager::createPublisher()
  {
      if (m_publisher.in() != nullptr)
      {
          return true;
      }

      DDS::PublisherQos pQos;
      DDS::ReturnCode_t result = m_participant->get_default_publisher_qos(pQos);
      if (!checkStatus(result, "get_default_publisher_qos() failed"))
      {
          return false;
      }


      m_publisher = m_participant->create_publisher(pQos, nullptr, DDS::STATUS_MASK_NONE);
      if (!checkHandle(m_publisher.in(), "create_publisher() failed"))
      {
          return false;
      }
      return true;
  }
  /*!
   * @if jp
   * @brief Subscriber生成
   *
   * @return true：生成成功、false：エラー
   *
   * @else
   * @brief create Subscriber
   *
   * @return
   *
   * @endif
   */
  bool OpenSpliceManager::createSubscriber()
  {
      if (m_subscriber.in() != nullptr)
      {
          return true;
      }

      DDS::SubscriberQos sQos;
      DDS::ReturnCode_t result = m_participant->get_default_subscriber_qos(sQos);
      if (!checkStatus(result, "get_default_subscriber_qos() failed"))
      {
          return false;
      }

      m_subscriber = m_participant->create_subscriber(sQos, nullptr, DDS::STATUS_MASK_NONE);
      if (!checkHandle(m_subscriber.in(), "create_subscriber() failed"))
      {
          return false;
      }

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
      return DDS::DomainParticipant::_duplicate(m_participant.in());
  }

  /*!
   * @if jp
   * @brief DataWriter生成
   *
   * @return DataWriter
   *
   * @else
   * @brief create DataWriter
   *
   * @return DataWriter
   *
   * @endif
   */
  DDS::DataWriter_ptr OpenSpliceManager::createWriter(std::string& topic_name, DDS::DataWriterListener_ptr listener)
  {
      DDS::Duration_t timeout = DDS::DURATION_INFINITE;

      if (m_topics.count(topic_name) == 0)
      {
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
      if (!checkStatus(result, "get_default_datawriter_qos() failed"))
      {
          return nullptr;
      }

      result = m_publisher->copy_from_topic_qos(wQos, tQos);
      if (!checkStatus(result, "copy_from_topic_qos() failed"))
      {
          return nullptr;
      }

      wQos.writer_data_lifecycle.autodispose_unregistered_instances = false;

      DDS::DataWriter_var writer = m_publisher->create_datawriter(m_topics[topic_name].in(), wQos, listener, DDS::STATUS_MASK_NONE);
      if (!checkHandle(writer.in(), "create_datawriter() failed"))
      {
          return nullptr;
      }

      return DDS::DataWriter::_duplicate(writer.in());
  }

  /*!
   * @if jp
   * @brief DataReader生成
   *
   * @return DataReader
   *
   * @else
   * @brief create DataReader
   *
   * @return DataReader
   *
   * @endif
   */
  DDS::DataReader_ptr OpenSpliceManager::createReader(std::string &topic_name, DDS::DataReaderListener_ptr listener)
  {
      if (m_topics.count(topic_name) == 0)
      {
          return nullptr;
      }

      DDS::Duration_t timeout = DDS::DURATION_INFINITE;

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
      if (!checkStatus(result, "get_default_datawriter_qos() failed"))
      {
          return nullptr;
      }

      result = m_subscriber->copy_from_topic_qos(rQos, tQos);
      if (!checkStatus(result, "copy_from_topic_qos() failed"))
      {
          return nullptr;
      }
      DDS::StatusMask mask =
          DDS::DATA_AVAILABLE_STATUS | DDS::REQUESTED_DEADLINE_MISSED_STATUS;

      DDS::DataReader_var reader = m_subscriber->create_datareader(m_topics[topic_name].in(), rQos, listener, mask);
      if (!checkHandle(reader.in(), "create_datareader() failed"))
      {
          return nullptr;
      }

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
      return m_subscriber->delete_datareader(reader);
  }
  /*!
   * @if jp
   * @brief トピック生成
   *
   * @return DataReader
   *
   * @else
   * @brief create Topic
   *
   * @return DataReader
   *
   * @endif
   */
  bool OpenSpliceManager::createTopic(std::string& topic_name, std::string& typeName)
  {
      DDS::ReturnCode_t result;
      DDS::TopicQos tQos;
      result = m_participant->get_default_topic_qos(tQos);
      if(!checkStatus(result, "get_default_topic_qos() failed"))
      {
          return false;
      }
      tQos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
      tQos.durability.kind = DDS::TRANSIENT_DURABILITY_QOS;
      
      DDS::Topic_var topic = m_participant->create_topic(topic_name.c_str(), typeName.c_str(), tQos, nullptr, DDS::STATUS_MASK_NONE);
      
      if (!checkHandle(topic.in(), "create_topic() failed"))
      {
          return false;
      }

      m_topics[topic_name] = DDS::Topic::_duplicate(topic.in());
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
  bool OpenSpliceManager::registerType(std::string& datatype, std::string& idlpath)
  {
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
      
      
      coil::create_process(comin, comout);
      
      std::string tmp;
      for (auto const& c : comout)
      {
          tmp.append(c);
      }
      std:: string xmlstr{coil::eraseBothEndsBlank(std::move(tmp))};

      rapidxml::xml_document<> doc;
      try
      {
          doc.parse<rapidxml::parse_trim_whitespace>(const_cast<char*>(xmlstr.c_str()));
      }
      catch (rapidxml::parse_error&)
      {
          return false;
      }

      rapidxml::xml_node<>* topics = doc.first_node("topics");
      
      DDS::String_var id_;
      DDS::String_var keys_;
      DDS::String_var descriptor_;
      if (topics != nullptr)
      {
          for (auto& topictype : children<>(topics))
          {
              rapidxml::xml_node<>* id = topictype.first_node("id");
              rapidxml::xml_node<>* keys = topictype.first_node("keys");
              rapidxml::xml_node<>* descriptor = topictype.first_node("descriptor");

              if (id != nullptr && keys != nullptr && descriptor != nullptr)
              {
                  rapidxml::xml_node<>* child = descriptor->first_node();
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
  bool OpenSpliceManager::unregisterType(std::string& /*name*/)
  {
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
  bool OpenSpliceManager::registeredType(std::string& name)
  {
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
   * @return インスタンス
   *
   * @else
   * @brief 
   *
   * @return 
   * 
   *
   * @endif
   */
  OpenSpliceManager* OpenSpliceManager::init()
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (!manager)
    {
      manager = new OpenSpliceManager();
      manager->start();
    }
    return manager;
  }

  /*!
   * @if jp
   * @brief インスタンス取得
   * 
   * @return インスタンス
   *
   * @else
   * @brief 
   *
   * @return 
   * 
   *
   * @endif
   */
  OpenSpliceManager& OpenSpliceManager::instance()
  {
    std::lock_guard<std::mutex> guard(mutex);
    if (!manager)
    {
      manager = new OpenSpliceManager();
      manager->start();
    }
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
}

