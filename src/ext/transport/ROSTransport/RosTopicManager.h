// -*- C++ -*-
/*!
 * @file  RosTopicManager.h
 * @brief RosTopicManager class
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
#include <map>
#include <vector>
#include <coil/Mutex.h>
#include <xmlrpcpp/XmlRpc.h>
#include <ros/poll_manager.h>
#include <ros/transport/transport_tcp.h>

namespace RTC
{
    /*!
     * @if jp
     * @class RosTopicManager
     * @brief RosTopicManager クラス
     *
     * ROSのトピックを管理するクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class RosTopicManager
     * @brief RosTopicManager class
     *
     * 
     *
     * @since 2.0.0
     *
     * @endif
     */
    class RosTopicManager
    {
        typedef coil::Mutex Mutex;
        typedef coil::Guard<Mutex> Guard;
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
        RosTopicManager();
        /*!
         * @if jp
         * @brief コピーコンストラクタ
         *
         * @param manager RosTopicManager
         *
         * @else
         * @brief Copy Constructor
         *
         * @param manager RosTopicManager
         *
         * @endif
         */
        RosTopicManager(const RosTopicManager &manager);
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
        ~RosTopicManager();
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
        void start();
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
            //RTC_PARANOID(("tcprosAcceptConnection()"));
            for(auto & publisher : m_publishers) 
            {
                publisher->connectTCP(transport);
            }
        };

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
        static RosTopicManager* init();
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
        static RosTopicManager& instance();
    private:
        static RosTopicManager* manager;
        static Mutex mutex;
        std::vector<ROSOutPort*> m_publishers;
        std::vector<ROSInPort*> m_subscribers;
        ros::PollManagerPtr m_poll_manager;
        ros::TransportTCPPtr m_tcpserver_transport;
        ros::XMLRPCManagerPtr m_xmlrpc_manager;
        std::map<std::string, std::vector<std::string>> m_cons;
    protected:
        //mutable Logger rtclog;
    };
}




#endif // RTC_ROSTOPICMANAGER_H

