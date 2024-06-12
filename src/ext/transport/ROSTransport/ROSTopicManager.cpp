// -*- C++ -*-
/*!
 * @file  ROSTopicManager.cpp
 * @brief ROSTopicManager class
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
#include <WinSock2.h>
#endif
#include "ROSTopicManager.h"
#include <ros/xmlrpc_manager.h>
#include <ros/network.h>
#include <xmlrpcpp/XmlRpcSocket.h>
#include <ros/connection.h>
#include <ros/connection_manager.h>
#include <coil/OS.h>


namespace ros
{
  namespace network
  {
    void init(const M_string& remappings);
  }
}


namespace RTC
{
  ROSTopicManager* ROSTopicManager::manager = nullptr;
  std::mutex ROSTopicManager::mutex;
  std::once_flag ROSTopicManager::m_once;

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
  ROSTopicManager::ROSTopicManager()
  : rtclog("ROSTopicManager"),
    m_subnum(0),
    m_pubnum(0)
  {

  }

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
  ROSTopicManager::ROSTopicManager(const ROSTopicManager &/*mgr*/)
  : rtclog("ROSTopicManager")
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
  ROSTopicManager::~ROSTopicManager()
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
  void ROSTopicManager::start()
  {
    ros::M_string remappings;
    ros::network::init(remappings);

    m_xmlrpc_manager = ros::XMLRPCManager::instance();

    m_xmlrpc_manager->bind("requestTopic", boost::bind(&ROSTopicManager::requestTopicCallback, this, _1, _2));
    m_xmlrpc_manager->bind("publisherUpdate", boost::bind(&ROSTopicManager::pubUpdateCallback, this, _1, _2));
    m_xmlrpc_manager->bind("getSubscriptions", boost::bind(&ROSTopicManager::getSubscriptionsCallback, this, _1, _2));
    m_xmlrpc_manager->bind("getPublications", boost::bind(&ROSTopicManager::getPublicationsCallback, this, _1, _2));
    m_xmlrpc_manager->bind("getBusStats", boost::bind(&ROSTopicManager::getBusStatsCallback, this, _1, _2));
    m_xmlrpc_manager->bind("getBusInfo", boost::bind(&ROSTopicManager::getBusInfoCallback, this, _1, _2));
    m_xmlrpc_manager->bind("getPid", boost::bind(&ROSTopicManager::getPidCallback, this, _1, _2));

    m_poll_manager = ros::PollManager::instance();
    m_poll_manager->start();
    m_tcpserver_transport = boost::make_shared<ros::TransportTCP>(&m_poll_manager->getPollSet());
    
    m_tcpserver_transport->listen(ros::network::getTCPROSPort(), 
                            100, 
                            boost::bind(&ROSTopicManager::tcprosAcceptConnection, this, _1));

    m_xmlrpc_manager->start();
  }

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
  void ROSTopicManager::shutdown()
  {
      m_xmlrpc_manager->shutdown();
      m_tcpserver_transport->close();
      m_poll_manager->shutdown();
  }

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
  void ROSTopicManager::requestTopicCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
  {
      if (params.getType() != XmlRpc::XmlRpcValue::TypeArray)
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          return;
      }
      if (params.size() < 3)
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          return;
      }

      if (params[0].getType() != XmlRpc::XmlRpcValue::TypeString)
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          return;
      }
      if (params[1].getType() != XmlRpc::XmlRpcValue::TypeString)
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          return;
      }

      std::string topic = params[1];

      if(!hasPublisher(topic))
      {
        XmlRpc::XmlRpcValue tcpros_params;

        result.setSize(3);
        tcpros_params.setSize(0);
        result[0] = int(-1);
        result[1] = std::string("Not a publisher of [") + topic + std::string("]");
        result[2] = tcpros_params;
        return;
      }
      
      XmlRpc::XmlRpcValue protocols = params[2];
      if (protocols.getType() != XmlRpc::XmlRpcValue::TypeArray)
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          return;
      }
      for(int i=0;i < protocols.size();i++)
      {
        if (protocols[i].getType() != XmlRpc::XmlRpcValue::TypeArray)
        {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          return;
        }
        if (protocols[i][0].getType() != XmlRpc::XmlRpcValue::TypeString)
        {
            result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
            return;
        }
        std::string protocol = std::string(protocols[i][0]);
      
        if (protocol == std::string("TCPROS"))
        {
          XmlRpc::XmlRpcValue tcpros_params;
          
          tcpros_params[0] = std::string("TCPROS");
          tcpros_params[1] = ros::network::getHost();
          tcpros_params[2] = int(m_tcpserver_transport->getServerPort());
          result.setSize(3);
          result[0] = int(1);
          result[1] = std::string("ready on ") + ros::network::getHost() + std::string(" ")
                           + std::string(coil::otos(static_cast<int>(m_tcpserver_transport->getServerPort())));
          result[2] = tcpros_params;
          return;
        }
      }
      result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);

  }
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
  void ROSTopicManager::getSubscriptionsCallback(XmlRpc::XmlRpcValue& /*params*/, XmlRpc::XmlRpcValue& result)
  {
    result[0] = 1;
    result[1] = std::string("subscriptions");
    XmlRpc::XmlRpcValue subs;
    subs.setSize(0);
    {
      std::lock_guard<std::mutex> guards(m_sub_mutex);
      for(auto & subscriber : m_subscribers)
      {
        XmlRpc::XmlRpcValue sub;
        sub[0] = subscriber->getTopicName();
        sub[1] = subscriber->datatype();
        subs[subs.size()] = sub;
      }
    }
    result[2] = subs;
  }
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
  void ROSTopicManager::getPublicationsCallback(XmlRpc::XmlRpcValue& /*params*/, XmlRpc::XmlRpcValue& result)
  {
    result[0] = 1;
    result[1] = std::string("publications");
    XmlRpc::XmlRpcValue pubs;
    pubs.setSize(0);
    {
      std::lock_guard<std::mutex> guardp(m_pub_mutex);
      for(auto & publisher : m_publishers)
      {
        XmlRpc::XmlRpcValue pub;
        pub[0] = publisher->getTopicName();
        pub[1] = publisher->datatype();
        pubs[pubs.size()] = pub;
      }
    }
    result[2] = pubs;
  }
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
  void ROSTopicManager::getBusStatsCallback(XmlRpc::XmlRpcValue& /*params*/, XmlRpc::XmlRpcValue& result)
  {
    result[0] = 1;
    result[1] = std::string("");

    XmlRpc::XmlRpcValue stats;
    stats.setSize(3);

    XmlRpc::XmlRpcValue publishersData;
    publishersData.setSize(0);
    {
      int count = 0;
      std::lock_guard<std::mutex> guardp(m_pub_mutex);
      for(auto & publisher : m_publishers)
      {
        publisher->getStats(publishersData[count]);
        count++;
      }
    }

    XmlRpc::XmlRpcValue subscribersData;
    subscribersData.setSize(0);
    {
      int count = 0;
      std::lock_guard<std::mutex> guards(m_sub_mutex);
      for(auto & subscribers : m_subscribers)
      {
        subscribers->getStats(subscribersData[count]);
        count++;
      }
    }

    XmlRpc::XmlRpcValue serviceData;
    serviceData.setSize(0);

    stats[0] = publishersData;
    stats[1] = subscribersData;
    stats[2] = serviceData;
    result[2] = stats;
  }

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
  void ROSTopicManager::getBusInfoCallback(XmlRpc::XmlRpcValue& /*params*/, XmlRpc::XmlRpcValue& result)
  {
    result[0] = 1;
    result[1] = std::string("bus info");
    XmlRpc::XmlRpcValue response;
    response.setSize(0);

    int count = 0;
    {
      std::lock_guard<std::mutex> guardsl(m_sublink_mutex);
      for(auto & con : m_tcp_sub_connecters)
      {
        con.getInfo(response[count]);
        count++;
      }
    }

    {
      std::lock_guard<std::mutex> guardpl(m_publink_mutex);
      for(auto & con : m_tcp_pub_connecters)
      {
        con.getInfo(response[count]);
        count++;
      }
    }

    result[2] = response;

  }
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
  void ROSTopicManager::getPidCallback(XmlRpc::XmlRpcValue& /*params*/, XmlRpc::XmlRpcValue& result)
  {
    result[0] = 1;
    result[1] = std::string("");
    result[2] = static_cast<int>(coil::getpid());
  }

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
  void ROSTopicManager::pubUpdateCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
  {
    if (params.getType() != XmlRpc::XmlRpcValue::TypeArray)
    {
        result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
        return;
    }
    if (params.size() < 3)
    {
        result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
        return;
    }

    if (params[0].getType() != XmlRpc::XmlRpcValue::TypeString)
    {
        result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
        return;
    }
    if (params[1].getType() != XmlRpc::XmlRpcValue::TypeString)
    {
        result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
        return;
    }

    std::string caller_id = params[0];
    std::string topic = params[1];
    
    
    if (params[2].getType() != XmlRpc::XmlRpcValue::TypeArray)
    {
      result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
      return;
    }
    
    std::vector<PublisherLink> new_;
    std::vector<PublisherLink> old_;

    {
      std::lock_guard<std::mutex> guardpl(m_publink_mutex);
      for(auto & con : m_tcp_pub_connecters)
      {
        if(con.getCallerID() == caller_id && con.getTopic() == topic)
        {
          old_.emplace_back(con);
        }
      }
    }
    
    for (int i = 0; i < params[2].size(); i++)
    {
      if (params[2][i].getType() != XmlRpc::XmlRpcValue::TypeString)
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          break;
      }
      std::string xmlrpc_uri = params[2][i];
      if (xmlrpc_uri.empty())
      {
          result = ros::xmlrpc::responseInt(0, ros::console::g_last_error_message, 0);
          break;
      }
      
      bool already_connected = false;
      {
        std::lock_guard<std::mutex> guardpl(m_publink_mutex);
        auto con = m_tcp_pub_connecters.begin();
        while(con != m_tcp_pub_connecters.end())
        {
          if(con->getCallerID() == caller_id && con->getTopic() == topic && con->getURI() == xmlrpc_uri)
          {
            RTC_WARN(("Caller ID=%s, Topic=%s, URL=%s already connected", caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));
            if(con->getConnection()->isDropped())
            {
              RTC_ERROR(("delete connector: Caller ID=%s, Topic=%s, URL=%s", caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));
              {
                std::lock_guard<std::mutex> guards(m_sub_mutex);
                for(auto & subscriber : m_subscribers)
                {
                  subscriber->removePublisherLink(con->getConnection());
                }
              }
              con = m_tcp_pub_connecters.erase(con);
            }
            else
            {
              ++con;
              already_connected = true;
              break;
            }
          }
          else
          {
            ++con;
          }
        }
      }
      
      if(!already_connected)
      {
        std::lock_guard<std::mutex> guards(m_sub_mutex);
        for(auto & subscriber : m_subscribers)
        {
          RTC_INFO(("connect ID=%s, Topic=%s, URL=%s", caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));
          subscriber->connectTCP(caller_id, topic, xmlrpc_uri);
        }
      }
      PublisherLink pub;
      pub.setInfo(caller_id, topic, xmlrpc_uri);
      new_.emplace_back(pub);
    }

  
    for(auto & old_con : old_)
    {
      std::vector<PublisherLink>::iterator itr_new = std::find(new_.begin(), new_.end(), old_con);
      size_t index_new = std::distance(new_.begin(), itr_new);
      if (index_new == new_.size())
      {
        std::lock_guard<std::mutex> guardpl(m_publink_mutex);
        std::vector<PublisherLink>::iterator itr_pub = std::find(m_tcp_pub_connecters.begin(), m_tcp_pub_connecters.end(), old_con);
        if(itr_pub != m_tcp_pub_connecters.end())
        {
          RTC_INFO(("delete connector: Caller ID=%s, Topic=%s", caller_id.c_str(), topic.c_str()));

          itr_pub->getConnection()->drop(ros::Connection::Destructing);
          m_tcp_pub_connecters.erase(itr_pub);
          {
            std::lock_guard<std::mutex> guards(m_sub_mutex);
            for(auto & subscriber : m_subscribers)
            {
              subscriber->removePublisherLink(itr_pub->getConnection());
            }
          }
        }

      }
      
    }
    
    result = ros::xmlrpc::responseInt(1, "", 0);
 
  }



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
  void ROSTopicManager::addPublisher(ROSOutPort *publisher)
  {
    std::lock_guard<std::mutex> guardp(m_pub_mutex);
    std::vector<ROSOutPort*>::iterator itr = std::find(m_publishers.begin(), m_publishers.end(), publisher);
    size_t index = std::distance( m_publishers.begin(), itr );
    
    if(index == m_publishers.size())
    {
      m_publishers.emplace_back(publisher);
    }
  }


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
  void ROSTopicManager::addSubscriber(ROSInPort *subscriber)
  {
    std::lock_guard<std::mutex> guards(m_sub_mutex);
    std::vector<ROSInPort*>::iterator itr = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
    size_t index = std::distance( m_subscribers.begin(), itr );
    if(index == m_subscribers.size())
    {
      m_subscribers.emplace_back(subscriber);
    }
    
  }

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
  bool ROSTopicManager::removePublisher(ROSOutPort *publisher)
  {
    std::lock_guard<std::mutex> guardp(m_pub_mutex);
    std::vector<ROSOutPort*>::iterator itr = std::find(m_publishers.begin(), m_publishers.end(), publisher);
    size_t index = std::distance( m_publishers.begin(), itr );
    
    if(index != m_publishers.size())
    {
      m_publishers.erase(remove(m_publishers.begin(), m_publishers.end(), publisher), m_publishers.end());
      return true;
    }
    return false;
  }

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
  bool ROSTopicManager::removeSubscriber(ROSInPort *subscriber)
  {
    std::lock_guard<std::mutex> guards(m_sub_mutex);
    std::vector<ROSInPort*>::iterator itr = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
    size_t index = std::distance( m_subscribers.begin(), itr );
    if(index != m_subscribers.size())
    {
      m_subscribers.erase(remove(m_subscribers.begin(), m_subscribers.end(), subscriber), m_subscribers.end());
      return true;
    }
    return false;
  }


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
  bool ROSTopicManager::existPublisher(ROSOutPort *publisher)
  {
    std::lock_guard<std::mutex> guardp(m_pub_mutex);
    std::vector<ROSOutPort*>::iterator itr = std::find(m_publishers.begin(), m_publishers.end(), publisher);
    size_t index = std::distance( m_publishers.begin(), itr );
    
    return (index != m_publishers.size());
  }


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
  bool ROSTopicManager::existSubscriber(ROSInPort *subscriber)
  {
    std::lock_guard<std::mutex> guards(m_sub_mutex);
    std::vector<ROSInPort*>::iterator itr = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
    size_t index = std::distance( m_subscribers.begin(), itr );
    return (index != m_subscribers.size());
  }

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
  bool ROSTopicManager::hasPublisher(const std::string& topic)
  {
    std::lock_guard<std::mutex> guardp(m_pub_mutex);
    for(auto & publisher : m_publishers)
    {
      if(publisher->getTopicName() == topic)
      {
        return true;
      }
    }
    return false;
  }

  /*!
   * @if jp
   * @brief Publisher
   *
   * @return 
   *
   * @else
   * @brief
   *
   *
   * @return 
   *
   * @endif
   */
  std::vector<PublisherLink> & ROSTopicManager::getPublisherLinkList()
  {
    std::lock_guard<std::mutex> guardpl(m_publink_mutex);
    return m_tcp_pub_connecters;
  }
  /*!
   * @if jp
   * @brief Subscriber
   *
   * @return 
   *
   * @else
   * @brief
   *
   *
   * @return 
   *
   * @endif
   */
  std::vector<SubscriberLink> & ROSTopicManager::getSubscriberLinkList()
  {
    std::lock_guard<std::mutex> guardsl(m_sublink_mutex);
    return m_tcp_sub_connecters;
  }


  /*!
   * @if jp
   * @brief  PublisherLinkを追加する
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
  bool ROSTopicManager::addPublisherLink(ros::ConnectionPtr& connection, const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri)
  {
    std::lock_guard<std::mutex> guardpl(m_publink_mutex);
    m_tcp_pub_connecters.push_back(PublisherLink(connection, m_pubnum, caller_id, topic, xmlrpc_uri));
    m_pubnum++;
    return true;
  }
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
  bool ROSTopicManager::removePublisherLink(const ros::ConnectionPtr& connection)
  {
    std::lock_guard<std::mutex> guardpl(m_publink_mutex);
    for (auto con = m_tcp_pub_connecters.begin(); con != m_tcp_pub_connecters.end(); con++)
    {
      if(con->getConnection() == connection)
      {
        con->getConnection()->drop(ros::Connection::Destructing);
        m_tcp_pub_connecters.erase(con);
        return true;
      }
    }
    return false;
  }

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
  bool ROSTopicManager::existPublisherLink(const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri)
  {
    std::lock_guard<std::mutex> guardpl(m_publink_mutex);
    for (auto & con : m_tcp_pub_connecters)
    {
      if(con.getCallerID() == caller_id && con.getTopic() == topic && con.getURI() == xmlrpc_uri)
      {
        return true;
      }
    }
    return false;
  }

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
  bool ROSTopicManager::addSubscriberLink(ros::ConnectionPtr& connection)
  {
    std::lock_guard<std::mutex> guardsl(m_sublink_mutex);
    m_tcp_sub_connecters.push_back(SubscriberLink(connection, m_subnum));
    m_subnum++;
    return true;
  }
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
  bool ROSTopicManager::removeSubscriberLink(const ros::ConnectionPtr& connection)
  {
    std::lock_guard<std::mutex> guardsl(m_sublink_mutex);
    for (auto con = m_tcp_sub_connecters.begin(); con != m_tcp_sub_connecters.end(); con++)
    {
      if(con->getConnection() == connection)
      {
        con->getConnection()->drop(ros::Connection::Destructing);
        m_tcp_sub_connecters.erase(con);
        return true;
      }
    }
    return false;
  }


  /*!
   * @if jp
   * @brief 指定のros::ConnectionオブジェクトからPublisherLinkを取得する
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
  PublisherLink* ROSTopicManager::getPublisherLink(const ros::ConnectionPtr& connection)
  {
    std::lock_guard<std::mutex> guardpl(m_publink_mutex);
    for(auto & tcp_connecter : m_tcp_pub_connecters)
    {
      if(tcp_connecter.getConnection() == connection)
      {
        return &tcp_connecter;
      }
    }
    return nullptr;
  }
  /*!
   * @if jp
   * @brief 指定のros::ConnectionオブジェクトからSubscriberLinkを取得する
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
  SubscriberLink* ROSTopicManager::getSubscriberLink(const ros::ConnectionPtr& connection)
  {
    std::lock_guard<std::mutex> guardsl(m_sublink_mutex);
    for(auto & tcp_connecter : m_tcp_sub_connecters)
    {
      if(tcp_connecter.getConnection() == connection)
      {
        return &tcp_connecter;
      }
    }
    return nullptr;
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
  ROSTopicManager* ROSTopicManager::init()
  {
    std::call_once(m_once, [] {
      manager = new ROSTopicManager();
      manager->start();
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
  ROSTopicManager& ROSTopicManager::instance()
  {
    std::call_once(m_once, [] {
      manager = new ROSTopicManager();
      manager->start();
    });
    return *manager;
  }

  /*!
   * @if jp
   * @brief ROSTopicManagerが初期化されている場合に終了処理を呼び出す
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
  void ROSTopicManager::shutdown_global()
  {
      std::lock_guard<std::mutex> guard(mutex);
      if (manager)
      {
          manager->shutdown();
      }
  }

}

