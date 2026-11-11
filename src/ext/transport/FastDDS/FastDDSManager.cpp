// -*- C++ -*-
/*!
 * @file  FastDDSManager.cpp
 * @brief FastDDSManager class
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

#include "FastDDSManager.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>




namespace RTC
{
  FastDDSManager* FastDDSManager::manager = nullptr;
  std::mutex FastDDSManager::mutex;
  std::once_flag FastDDSManager::m_once;

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
  FastDDSManager::FastDDSManager() : m_participant(nullptr)
  {
  }

  /*!
   * @if jp
   * @brief コピーコンストラクタ
   *  
   * @param manager FastDDSManager
   *
   * @else
   * @brief Copy Constructor
   *
   * @param manager FastDDSManager
   *
   * @endif
   */
  FastDDSManager::FastDDSManager(const FastDDSManager &/*mgr*/) : m_participant(nullptr)
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
  FastDDSManager::~FastDDSManager()
  {
    
  }

  /*!
   * @if jp
   * @brief トピックマネージャ開始
   *
   *
   * @else
   * @brief 
   *
   * 
   *
   * @endif
   */
  void FastDDSManager::start(coil::Properties& prop)
  {
    using namespace eprosima::fastdds::dds;
    Logger rtclog("FastDDSManager");
    RTC_INFO(("FastDDSManager::start()"));
    RTC_INFO_STR((prop));
      
    // DomainParticipantFactory のインスタンスを取得
    auto* factory = DomainParticipantFactory::get_instance();

    uint32_t domain_id = 0;
    coil::stringTo<uint32_t>(domain_id, prop["domain.id"].c_str());
    RTC_INFO(("Domain ID: %d", domain_id));

    const std::string participant_name = prop.getProperty("participant.name", "participant_openrtm");
    RTC_INFO(("Participant name: %s", participant_name.c_str()));

    m_xml_profile_file = prop.getProperty("xmlprofile.filename");
    if (!m_xml_profile_file.empty())
    {
      // Fast DDS では factory 経由で XML プロファイルをロード
      auto ret = factory->load_XML_profiles_file(m_xml_profile_file);
      if (ret != eprosima::fastdds::dds::RETCODE_OK)
      {
        RTC_ERROR(("Failed to load XML profile"));
	return;
      }
      RTC_INFO(("Load XMl file: %s", m_xml_profile_file.c_str()));

      // XML のプロファイル名を指定して Participant を生成
      m_participant = factory->create_participant_with_profile(domain_id, participant_name);
    }
    else
    {
      // QoS オブジェクトの作成とパラメータ設定
      DomainParticipantQos pqos = PARTICIPANT_QOS_DEFAULT;
      pqos.name(participant_name.c_str());
       
      // セキュリティパラメータ等の追加設定（setParticipantSecParamもDomainParticipantQos受け取りに要修正）	
      setParticipantSecParam(prop, pqos);

      m_participant = factory->create_participant(domain_id, pqos);
    }
    if (m_participant == nullptr)
    {
      RTC_ERROR(("Failed to create DomainParticipant"));
    }
  }

  /*!
   * @if jp
   * @brief マネージャ終了
   *
   *
   * @else
   * @brief
   *
   *
   *
   * @endif
   */
  void FastDDSManager::shutdown()
  {
    using namespace eprosima::fastdds::dds;

    Logger rtclog("FastDDSManager");
    RTC_INFO(("FastDDSManager::shutdown()"));
    if (m_participant == nullptr)
    {
      return;
    }

    auto* factory = DomainParticipantFactory::get_instance();
    auto ret = factory->delete_participant(m_participant);
    if (ret == eprosima::fastdds::dds::RETCODE_OK)
    {
      m_participant = nullptr;
    }
    else
    {
      RTC_ERROR(("Failed to delete DomainParticipant"));
    }
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
  eprosima::fastdds::dds::DomainParticipant* FastDDSManager::getParticipant()
  {
      std::lock_guard<std::mutex> guard(mutex);
      return m_participant;
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
  bool FastDDSManager::registerType(eprosima::fastdds::dds::TopicDataType* type)
  {
      using namespace eprosima::fastdds::dds;

      if (type == nullptr)
      {
          return false;
      }

      std::lock_guard<std::mutex> guard(mutex);
      if (m_participant == nullptr)
      {
          return false;
      }

      const std::string name = type->get_name();
      if (name.empty())
      {
          return false;
      }
      if (m_participant->find_type(name))
      {
        return true;
      }

      TypeSupport type_support(type);      
      
      return type_support.register_type(m_participant) == eprosima::fastdds::dds::RETCODE_OK;
  }

  /*!
   * @if jp
   * @brief 指定名の型が登録済みかを判定する
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
  bool FastDDSManager::registeredType(const char* name)
  {
    if (name == nullptr || name[0] == '\0')
    {
        return false;
    }

    std::lock_guard<std::mutex> guard(mutex);

    if (m_participant == nullptr)
    {
        return false;
    }

    return static_cast<bool>(m_participant->find_type(name));
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
  bool FastDDSManager::unregisterType(const char* name)
  {
      using namespace eprosima::fastdds::dds;

      if (name == nullptr || name[0] == '\0')
      {
          return false;
      }

      std::lock_guard<std::mutex> guard(mutex);

      if (m_participant == nullptr)
      {
          return false;
      }

      if (!m_participant->find_type(name))
      {
          return false;
      }

      return m_participant->unregister_type(name)
        == eprosima::fastdds::dds::RETCODE_OK;
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
  FastDDSManager* FastDDSManager::init(coil::Properties& prop)
  {
    std::call_once(m_once, [&] {
      manager = new FastDDSManager();
      manager->start(prop);
      });
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
  FastDDSManager& FastDDSManager::instance()
  {
    std::call_once(m_once, [&] {
      coil::Properties prop;
      manager = new FastDDSManager();
      manager->start(prop);
      });
    return *manager;
  }

  /*!
   * @if jp
   * @brief FastDDSManagerが初期化されている場合に終了処理を呼び出す
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
  void FastDDSManager::shutdown_global()
  {
      std::lock_guard<std::mutex> guard(mutex);
      if (manager)
      {
          manager->shutdown();
      }
  }


  /*!
   * @if jp
   * @brief プロパティからeprosima::fastrtps::ParticipantAttributesを設定する
   *
   * @param prop プロパティ
   * @param PParam Participantの属性
   *
   * @else
   * @brief
   *
   * @param prop
   * @param PParam
   *
   *
   * @endif
   */
  void FastDDSManager::setParticipantSecParam(coil::Properties& prop, eprosima::fastdds::dds::DomainParticipantQos &PParam)
  {
    const std::vector<std::string> plugins = {
        "dds.sec.auth.plugin",
        "dds.sec.access.plugin",
        "dds.sec.crypto.plugin",
        "dds.sec.log.plugin" 
    };

    for (const auto& plugin_key : plugins)
    {
      const std::string plugin_name = prop.getProperty(plugin_key);
      if (plugin_name.empty())
      {
        continue;
      }

      PParam.properties().properties().emplace_back(
            plugin_key, plugin_name);

      const std::string path =
            plugin_key.substr(0, plugin_key.size() - 7)
            + "." + plugin_name;

      const std::vector<coil::Properties*>& leaves =
            prop.getNode(path).getLeaf();

      for (auto* leaf : leaves)
      {
        if (leaf == nullptr)
        {
          continue;
        }

        const std::string key = path + "." + leaf->getName();	
        PParam.properties().properties().emplace_back(key, leaf->getValue());
      }
    }
  }

  /*!
   * @if jp
   * @brief XMLファイルが設定済みかを判定
   *
   * @return true：設定済み、false：未設定
   *
   * @else
   * @brief
   *
   * @return
   *
   *
   * @endif
   */
  bool FastDDSManager::xmlConfigured()
  {
    return !m_xml_profile_file.empty();
  }
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif
