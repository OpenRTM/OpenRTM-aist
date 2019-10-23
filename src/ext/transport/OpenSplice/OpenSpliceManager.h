// -*- C++ -*-
/*!
 * @file  OpenSpliceManager.h
 * @brief OpenSpliceManager class
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

#ifndef RTC_OPENSPLICEMANAGER_H
#define RTC_OPENSPLICEMANAGER_H

#include <vector>
#include <mutex>
#include <map>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <ccpp_dds_dcps.h>
#include "ccpp_CORBACdrData.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif

namespace RTC
{
    /*!
     * @if jp
     * @class OpenSpliceManager
     * @brief OpenSpliceManager クラス
     *
     * OpenSpliceを管理するクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceManager
     * @brief OpenSpliceManager class
     *
     * 
     *
     * @since 2.0.0
     *
     * @endif
     */
    class OpenSpliceManager
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
        OpenSpliceManager();
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
        OpenSpliceManager(const OpenSpliceManager &manager);
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
        ~OpenSpliceManager();
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
        void start();

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
        DDS::DomainParticipant_ptr getParticipant();
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
        DDS::DataWriter_ptr createWriter(std::string& topic_name, DDS::DataWriterListener_ptr listener);
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
        DDS::DataReader_ptr createReader(std::string& topic_name, DDS::DataReaderListener_ptr listener);
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
        DDS::ReturnCode_t deleteWriter(DDS::DataWriter_ptr writer);
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
        DDS::ReturnCode_t deleteReader(DDS::DataReader_ptr reader);
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
        bool createTopic(std::string& topic_name, std::string& typeName);
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
        bool createPublisher();
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
        bool createSubscriber();
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
        bool registerType(std::string& datatype, std::string& idlpath);
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
        bool unregisterType(std::string& name);
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
        bool registeredType(std::string& name);

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
        void finalize();


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
        static OpenSpliceManager* init();
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
        static OpenSpliceManager& instance();

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
        static void shutdown_global();
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
        static bool checkStatus(DDS::ReturnCode_t status, const char *info);

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
        static bool checkHandle(void *handle, const char* info);
    private:
        static OpenSpliceManager* manager;
        static std::mutex mutex;
        DDS::DomainParticipantFactory_var m_factory;
        DDS::DomainId_t m_domain;
        DDS::DomainParticipant_var m_participant;
        DDS::Publisher_var m_publisher;
        DDS::Subscriber_var m_subscriber;
        std::map <std::string, DDS::Topic_var> m_topics;
        std::map <std::string, OpenRTM_OpenSplice::CORBACdrDataTypeSupport_var> m_typesupports;
    protected:
    };

}


#endif // RTC_OPENSPLICEMANAGER_H

