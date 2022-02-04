// -*- C++ -*-
/*!
 * @file  FastRTPSManager.cpp
 * @brief FastRTPSManager class
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


#include "FastRTPSManager.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/Domain.h>




namespace RTC
{
  FastRTPSManager* FastRTPSManager::manager = nullptr;
  std::mutex FastRTPSManager::mutex;
  std::once_flag FastRTPSManager::m_once;

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
  FastRTPSManager::FastRTPSManager() : m_participant(nullptr)
  {
  }

  /*!
   * @if jp
   * @brief コピーコンストラクタ
   *  
   * @param manager FastRTPSManager
   *
   * @else
   * @brief Copy Constructor
   *
   * @param manager FastRTPSManager
   *
   * @endif
   */
  FastRTPSManager::FastRTPSManager(const FastRTPSManager &/*mgr*/) : m_participant(nullptr)
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
  FastRTPSManager::~FastRTPSManager()
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
  void FastRTPSManager::start(coil::Properties& prop)
  {
      Logger rtclog("FastRTPSManager");
      RTC_INFO(("FastRTPSManager::start()"));
      RTC_INFO_STR((prop));
      

      m_xml_profile_file = std::move(prop["xmlprofile.filename"]);
      if (!m_xml_profile_file.empty())
      {
        eprosima::fastrtps::Domain::loadXMLProfilesFile(m_xml_profile_file);
        RTC_INFO(("Load XMl file: %s", m_xml_profile_file.c_str()));
      }

      if(m_xml_profile_file.empty())
      {
        eprosima::fastrtps::ParticipantAttributes PParam;
#if (FASTRTPS_VERSION_MAJOR >= 2)
#else
        PParam.rtps.builtin.domainId = 0;
#endif
        PParam.rtps.setName("participant_openrtm");
        
        m_participant = eprosima::fastrtps::Domain::createParticipant(PParam);
      }
      else
      {
        const std::string paticipant_name = std::move(prop["xmlprofile.participant.name"]);
        RTC_INFO(("Create participant: %s", paticipant_name.c_str()));
        m_participant = eprosima::fastrtps::Domain::createParticipant(paticipant_name);
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
  void FastRTPSManager::shutdown()
  {
      Logger rtclog("FastRTPSManager");
      RTC_INFO(("FastRTPSManager::shutdown()"));
      eprosima::fastrtps::Domain::removeParticipant(m_participant);
      manager = nullptr;
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
  eprosima::fastrtps::Participant* FastRTPSManager::getParticipant()
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
  bool FastRTPSManager::registerType(eprosima::fastrtps::TopicDataType* type)
  {
      std::lock_guard<std::mutex> guard(mutex);
      if (registeredType(type->getName()))
      {
          return true;
      }
      return eprosima::fastrtps::Domain::registerType(m_participant,type);
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
  bool FastRTPSManager::registeredType(const char* name)
  {
      eprosima::fastrtps::TopicDataType* type_;
      return eprosima::fastrtps::Domain::getRegisteredType(m_participant, name, &type_);
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
  bool FastRTPSManager::unregisterType(const char* name)
  {
      std::lock_guard<std::mutex> guard(mutex);
      if (!registeredType(name))
      {
          return false;
      }
      return eprosima::fastrtps::Domain::unregisterType(m_participant, name);
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
  FastRTPSManager* FastRTPSManager::init(coil::Properties& prop)
  {
    std::call_once(m_once, [&] {
      manager = new FastRTPSManager();
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
  FastRTPSManager& FastRTPSManager::instance()
  {
    std::call_once(m_once, [&] {
      coil::Properties prop;
      manager = new FastRTPSManager();
      manager->start(prop);
      });
    return *manager;
  }

  /*!
   * @if jp
   * @brief FastRTPSManagerが初期化されている場合に終了処理を呼び出す
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
  void FastRTPSManager::shutdown_global()
  {
      std::lock_guard<std::mutex> guard(mutex);
      if (manager)
      {
          manager->shutdown();
      }
  }
}

