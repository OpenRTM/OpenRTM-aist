// -*- C++ -*-
/*!
 * @file  ROSTopicManager.h
 * @brief ROSTopicManager class
 * @date  $Date: 2019-02-04 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_ROSTOPICMANAGER_H
#define RTC_ROSTOPICMANAGER_H

#include "ROSInPort.h"
#include "ROSOutPort.h"
#include <vector>
#include <map>
#include <mutex>
#include <xmlrpcpp/XmlRpc.h>
#include <ros/poll_manager.h>
#include <ros/transport/transport_tcp.h>


namespace RTC
{
    /*!
     * @if jp
     * @class ROSTopicManager
     * @brief ROSTopicManager クラス
     *
     * ROSのトピックを管理するクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class ROSTopicManager
     * @brief ROSTopicManager class
     *
     * 
     *
     * @since 2.0.0
     *
     * @endif
     */
    class ROSTopicManager
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
        ROSTopicManager();
        /*!
         * @if jp
         * @brief コピーコンストラクタ
         *
         * @param manager ROSTopicManager
         *
         * @else
         * @brief Copy Constructor
         *
         * @param manager ROSTopicManager
         *
         * @endif
         */
        ROSTopicManager(const ROSTopicManager &manager);
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
        ~ROSTopicManager();
        /*!
         * @if jp
         * @brief トピックマネージャ開始
         * XML-RPC、TCPサーバー、ポールマネージャを開始する
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
         * @brief 終了処理
         * XML-RPC、TCPサーバー、ポールマネージャを終了する
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
         * @brief requestTopic関数リモート呼び出し時のコールバック関数
         * サブスクライバーからの接続時に呼ばれる
         * 
         * @param params
         * @param result
         *
         * @else
         * @brief 
         *
         * @param params
         * @param result
         * 
         *
         * @endif
         */
        void requestTopicCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
        /*!
         * @if jp
         * @brief pubUpdate関数リモート呼び出し時のコールバック関数
         * パブリッシャーの更新時に呼ばれる
         * 
         * @param params
         * @param result
         *
         * @else
         * @brief 
         *
         * @param params
         * @param result
         * 
         *
         * @endif
         */
        void pubUpdateCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
        /*!
         * @if jp
         * @brief パブリッシャーの追加
         * 
         * @param publisher パブリッシャー
         *
         * @else
         * @brief add publisher
         *
         * @param publisher 
         * 
         *
         * @endif
         */
        void addPublisher(ROSOutPort *publisher);
        /*!
         * @if jp
         * @brief サブスクライバーの追加
         * 
         * @param subscriber サブスクライバー
         *
         * @else
         * @brief add subscriber
         *
         * @param subscriber 
         * 
         *
         * @endif
         */
        void addSubscriber(ROSInPort *subscriber);
        /*!
         * @if jp
         * @brief パブリッシャーの削除
         * 
         * @param publisher パブリッシャー
         *
         * @else
         * @brief remove publisher
         *
         * @param publisher 
         * 
         *
         * @endif
         */
        bool removePublisher(ROSOutPort *publisher);
        /*!
         * @if jp
         * @brief サブスクライバーの削除
         * 
         * @param subscriber サブスクライバー
         *
         * @else
         * @brief remove subscriber
         *
         * @param subscriber 
         * 
         *
         * @endif
         */
        bool removeSubscriber(ROSInPort *subscriber);
        /*!
         * @if jp
         * @brief パブリッシャーの存在確認
         * 
         * @param publisher パブリッシャー
         * @return True：存在する
         *
         * @else
         * @brief 
         *
         * @param publisher 
         * @return 
         * 
         *
         * @endif
         */
        bool existPublisher(ROSOutPort *publisher);
        /*!
         * @if jp
         * @brief サブスクライバーの存在確認
         * 
         * @param subscriber サブスクライバー
         * @return True：存在する
         *
         * @else
         * @brief 
         *
         * @param subscriber 
         * @return 
         * 
         *
         * @endif
         */
        bool existSubscriber(ROSInPort *subscriber);
        /*!
         * @if jp
         * @brief 指定トピック名のPublisherが存在するかの確認
         * 
         * @param topic トピック名
         * @return True：存在する
         *
         * @else
         * @brief 
         *
         * @param topic 
         * @return 
         * 
         *
         * @endif
         */
        bool hasPublisher(const std::string& topic);



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
        static ROSTopicManager* init();
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
        static ROSTopicManager& instance();
        /*!
         * @if jp
         * @brief ROSTopicManagerが初期化されている場合に終了処理を呼び出す
         *
         *
         * @else
         * @brief
         *
         *
         *
         * @endif
         */
        static void shutdown_global();
        /*!
         * @if jp
         * @brief Subscriber一覧を取得
         *
         * @param params 
         * @param result 
         *
         * @else
         * @brief
         *
         *
         * @param params 
         * @param result 
         *
         * @endif
         */
        void getSubscriptionsCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
        /*!
         * @if jp
         * @brief Publisher一覧を取得
         *
         * @param params 
         * @param result 
         *
         * @else
         * @brief
         *
         *
         * @param params 
         * @param result 
         *
         * @endif
         */
        void getPublicationsCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
        /*!
         * @if jp
         * @brief コネクタの状態取得
         *
         * @param params 
         * @param result 
         *
         * @else
         * @brief
         *
         *
         * @param params 
         * @param result 
         *
         * @endif
         */
        void getBusStatsCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
        /*!
         * @if jp
         * @brief コネクタの情報取得
         *
         * @param params 
         * @param result 
         *
         * @else
         * @brief
         *
         *
         * @param params 
         * @param result 
         *
         * @endif
         */
        void getBusInfoCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
        /*!
         * @if jp
         * @brief プロセスID取得
         *
         * @param params 
         * @param result 
         *
         * @else
         * @brief
         *
         *
         * @param params 
         * @param result 
         *
         * @endif
         */
        void getPidCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
    private:
        static ROSTopicManager* manager;
        static std::mutex mutex;
        std::vector<ROSOutPort*> m_publishers;
        std::vector<ROSInPort*> m_subscribers;
        ros::PollManagerPtr m_poll_manager;
        ros::TransportTCPPtr m_tcpserver_transport;
        ros::XMLRPCManagerPtr m_xmlrpc_manager;
        mutable Logger rtclog;
        unsigned int m_subnum;
        unsigned int m_pubnum;
        std::mutex m_pub_mutex;
        std::mutex m_sub_mutex;
        std::vector<PublisherLink> m_tcp_pub_connecters;
        std::vector<SubscriberLink> m_tcp_sub_connecters;
        std::mutex m_publink_mutex;
        std::mutex m_sublink_mutex;
        static std::once_flag m_once;
    public:
        /*!
         * @if jp
         * @brief PublisherLinkの一覧を取得する
         *
         * @return PublisherLinkの一覧
         *
         * @else
         * @brief
         *
         *
         * @return 
         *
         * @endif
         */
        std::vector<PublisherLink> & getPublisherLinkList();
        /*!
         * @if jp
         * @brief SubscriberLinkの一覧を取得する
         *
         * @return SubscriberLinkの一覧
         *
         * @else
         * @brief
         *
         *
         * @return 
         *
         * @endif
         */
        std::vector<SubscriberLink> & getSubscriberLinkList();
        /*!
         * @if jp
         * @brief PublisherLinkを追加する
         * 
         * @param connection ros::Connectionオブジェクト
         * @param caller_id 呼び出しID
         * @param topic トピック名
         * @param xmlrpc_uri 接続先のURI
         * @return true：追加成功
         *
         * @else
         * @brief 
         *
         * @param connection 
         * @param caller_id 
         * @param topic 
         * @param xmlrpc_uri 
         * @return 
         * 
         *
         * @endif
         */
        bool addPublisherLink(ros::ConnectionPtr& connection, const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri);
        /*!
         * @if jp
         * @brief PublisherLinkを削除する
         * 
         * @param connection ros::Connectionオブジェクト
         * @return false：指定のPublisherLinkがリストにないため削除失敗
         *
         * @else
         * @brief 
         *
         * @param connection 
         * @return 
         * 
         *
         * @endif
         */
        bool removePublisherLink(const ros::ConnectionPtr& connection);
        /*!
         * @if jp
         * @brief PublisherLinkの存在確認
         * 
         * @param caller_id 呼び出しID
         * @param topic トピック名
         * @param xmlrpc_uri 接続先のURI
         * @return true：存在する
         *
         * @else
         * @brief 
         *
         * @param caller_id 
         * @param topic 
         * @param xmlrpc_uri 
         * @return true：存在する
         * 
         *
         * @endif
         */
        bool existPublisherLink(const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri);
        /*!
         * @if jp
         * @brief 指定のros::ConnectionオブジェクトのPublisherLinkを取得する
         * 
         * @param connection ros::Connectionオブジェクト
         * @return PublisherLink
         *
         * @else
         * @brief 
         *
         * @param connection 
         * @return 
         * 
         *
         * @endif
         */
        PublisherLink* getPublisherLink(const ros::ConnectionPtr& connection);
        /*!
         * @if jp
         * @brief SubscriberLinkを追加する
         * 
         * @param connection ros::Connectionオブジェクト
         * @param caller_id 呼び出しID
         * @param topic トピック名
         * @param xmlrpc_uri 接続先のURI
         * @return true：存在する
         *
         * @else
         * @brief 
         *
         * @param connection 
         * @param caller_id 
         * @param caller_id 
         * @param xmlrpc_uri 
         * @return 
         * 
         *
         * @endif
         */
        bool addSubscriberLink(ros::ConnectionPtr& connection);
        /*!
         * @if jp
         * @brief SubscriberLinkを削除する
         * 
         * @param connection ros::Connectionオブジェクト
         * @return false：指定のSubscriberLinkがリストにないため削除失敗
         *
         * @else
         * @brief 
         *
         * @param connection 
         * @return 
         * 
         *
         * @endif
         */
        bool removeSubscriberLink(const ros::ConnectionPtr& connection);
        /*!
         * @if jp
         * @brief 指定のros::ConnectionオブジェクトのSubscriberLinkを取得する
         * 
         * @param connection ros::Connectionオブジェクト
         * @return SubscriberLink
         *
         * @else
         * @brief 
         *
         * @param connection 
         * @return 
         * 
         *
         * @endif
         */
        SubscriberLink* getSubscriberLink(const ros::ConnectionPtr& connection);
        /*!
         * @if jp
         * @brief TCP接続受け入れ時のコールバック関数
         * 
         * @param transport ros::Transport
         *
         * @else
         * @brief 
         *
         * @param transport 
         * @return 
         * 
         *
         * @endif
         */
        void tcprosAcceptConnection(const ros::TransportTCPPtr& transport)
        {
            RTC_PARANOID(("connectTCP()"));

            ros::ConnectionPtr tcp_connecter = boost::make_shared<ros::Connection>();
            
            addSubscriberLink(tcp_connecter);
            tcp_connecter->initialize(transport, true, boost::bind(&ROSTopicManager::onConnectionHeaderReceived, this, _1, _2));
            
            if(!tcp_connecter->isDropped())
            {
                RTC_VERBOSE(("Connector created."));      
                return;
            }
            else
            {
                removeSubscriberLink(tcp_connecter);
                RTC_VERBOSE(("Connector creation failed."));
                return;
            }
        }

       /*!
        * @if jp
        * @brief ヘッダ情報受信時のコールバック関数
        *
        *
        * @param conn ros::ConnectionPtr
        * @param header ヘッダ情報
        *
        * @return true：問題なし、false：ヘッダが不正
        * 
        * @else
        * @brief 
        *
        *
        * @param conn 
        * @param header 
        * 
        * @return
        *
        * @endif
        */
        bool onConnectionHeaderReceived(const ros::ConnectionPtr& conn, const ros::Header& header)
        {
            RTC_VERBOSE(("onConnectionHeaderReceived()"));

            bool ret = false;
            std::lock_guard<std::mutex> guardp(m_pub_mutex);
            for(auto & publisher : m_publishers) 
            {
                if(publisher->onConnectionHeaderReceived(conn, header) == true)
                {
                    ret = true;
                }
            }
            return ret;
        }
    protected:
    };
}




#endif // RTC_ROSTOPICMANAGER_H

