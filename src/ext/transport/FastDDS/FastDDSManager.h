// -*- C++ -*-
/*!
 * @file  FastDDSManager.h
 * @brief FastDDSManager class
 * @date  $Date: 2019-02-04 03:08:03 $
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

#ifndef RTC_FASTDDSMANAGER_H
#define RTC_FASTDDSMANAGER_H

#include "FastDDSInPort.h"
#include "FastDDSOutPort.h"
#include <map>
#include <vector>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <mutex>
#include <string>
#include <vector>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif

namespace RTC
{
    /*!
     * @if jp
     * @class FastDDSManager
     * @brief FastDDSManager クラス
     *
     * FastDDSを管理するクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class FastDDSManager
     * @brief FastDDSManager class
     *
     * 
     *
     * @since 2.0.0
     *
     * @endif
     */
    class FastDDSManager
    {
    public:
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
        FastDDSManager();
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
        FastDDSManager(const FastDDSManager &manager);
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
        ~FastDDSManager();
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
        void start(coil::Properties& prop);
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
        void shutdown();

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
        eprosima::fastdds::dds::DomainParticipant *getParticipant();
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
        bool registerType(eprosima::fastdds::dds::TopicDataType* type);
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
        bool registeredType(const char* name);
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
        bool unregisterType(const char* name);


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
        static FastDDSManager* init(coil::Properties &prop);
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
        static FastDDSManager& instance();
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
        static void shutdown_global();
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
        bool xmlConfigured();
    private:
        /*!
         * @if jp
         * @brief プロパティからeprosima::fastrtps::ParticipantAttributesを設定する
         * @brief プロパティからeprosima::fastdds::dds:ParticipantAttributesを設定する
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
        static void setParticipantSecParam(coil::Properties& prop, eprosima::fastdds::dds::DomainParticipantQos &PParam);
        static FastDDSManager* manager;
        static std::mutex mutex;
        eprosima::fastdds::dds::DomainParticipant *m_participant;
        std::string m_xml_profile_file;
        static std::once_flag m_once;
    protected:
    };
}




#endif // RTC_FASTRTPSMANAGER_H

