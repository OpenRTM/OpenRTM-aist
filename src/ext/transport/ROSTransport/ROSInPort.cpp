// -*- C++ -*-

/*!
 * @file  ROSInPort.cpp
 * @brief ROSInPort class
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
#include <chrono>
#include <xmlrpcpp/XmlRpc.h>
#include <xmlrpcpp/XmlRpcSocket.h>
#include <ros/xmlrpc_manager.h>
#include <ros/network.h>
#include <ros/poll_manager.h>
#include <ros/connection_manager.h>
#include "ROSInPort.h"
#include "ROSTopicManager.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef getpid
#undef getpid
#endif
#endif
#include <coil/OS.h>
#include <coil/stringutil.h>


#define ROS_MASTER_URI "ROS_MASTER_URI"
#define ROS_DEFAULT_MASTER_ADDRESS "localhost"
#define ROS_DEFAULT_MASTER_PORT 11311


namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  ROSInPort::ROSInPort(void)
   : m_buffer(nullptr),
     m_tcp_nodelay(true),
     m_roscoreport(ROS_DEFAULT_MASTER_PORT)
  {
    // PortProfile setting
    setInterfaceType("ros");
    // ConnectorProfile setting

  }

  

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ROSInPort::~ROSInPort(void)
  {
    RTC_PARANOID(("~ROSInPort()"));

    ROSTopicManager& topicmgr = ROSTopicManager::instance();
    
    {
      std::lock_guard<std::mutex> guardc(m_con_mutex);
      for(auto & con : m_tcp_connecters)
      {
          topicmgr.removePublisherLink(con.getConnection());
      }
    }
    topicmgr.removeSubscriber(this);

    if(!m_roscorehost.empty())
    {
        XmlRpc::XmlRpcValue request;
        XmlRpc::XmlRpcValue response;

        XmlRpc::XmlRpcClient *master = ros::XMLRPCManager::instance()->getXMLRPCClient(m_roscorehost, m_roscoreport, "/");

        request[0] = m_callerid;
        request[1] = m_topic;
        request[2] = std::string(ros::XMLRPCManager::instance()->getServerURI());

        RTC_PARANOID(("unregisterSubscriber:%s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));
        bool b = master->execute("unregisterSubscriber", request, response);
        if(!b)
        {
          RTC_ERROR(("unregisterSubscriber Error"));
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
  void ROSInPort::init(coil::Properties& prop)
  {

    RTC_PARANOID(("ROSInPort::init()"));

    if(prop.propertyNames().size() == 0)
    {
      RTC_DEBUG(("Property is empty."));
      return;
    }

    ROSTopicManager& topicmgr = ROSTopicManager::instance();
    if(topicmgr.existSubscriber(this))
    {
      RTC_VERBOSE(("Subscriber already exists."));
      return;
    }
    
    m_messageType = prop.getProperty("marshaling_type", "ros:std_msgs/Float32");
    m_topic = prop.getProperty("ros.topic", "chatter");
    m_topic = "/" + m_topic;


    m_tcp_nodelay = coil::toBool(prop["ros.tcp_nodelay"], "YES", "NO", true);


    m_roscorehost = prop.getProperty("ros.roscore.host");
    std::string tmp_port = prop.getProperty("ros.roscore.port");
    if(m_roscorehost.empty() && tmp_port.empty())
    {
      RTC_VERBOSE(("Get the IP address and port number of ros master from environment variable %s.", ROS_MASTER_URI));
      std::string env;
      if (coil::getenv(ROS_MASTER_URI, env))
      {
        RTC_VERBOSE(("$%s: %s", ROS_MASTER_URI, env.c_str()));
        if(!env.empty())
        {
          env = coil::replaceString(env, "http://", "");
          env = coil::replaceString(env, "https://", "");
          coil::vstring envsplit = coil::split(env, ":");

          m_roscorehost = envsplit[0];

          if(envsplit.size() >= 2)
          {
            tmp_port = envsplit[1];
          }
        }
      }
    }
    if(m_roscorehost.empty())
    {
      m_roscorehost = ROS_DEFAULT_MASTER_ADDRESS;
    }
    if(!tmp_port.empty())
    {
      if(!coil::stringTo<unsigned int>(m_roscoreport, tmp_port.c_str()))
      {
        RTC_ERROR(("%s cannot be converted to an int value", tmp_port.c_str()));
        m_roscoreport = ROS_DEFAULT_MASTER_PORT;
      }
    }
    else
    {
      m_roscoreport = ROS_DEFAULT_MASTER_PORT;
    }


    RTC_VERBOSE(("topic name: %s", m_topic.c_str()));
    RTC_VERBOSE(("roscore address: %s:%d", m_roscorehost.c_str(), m_roscoreport));
    
    m_callerid = prop.getProperty("ros.node.name", "rtcomp");
    if (coil::toBool(prop["ros.node.anonymous"], "YES", "NO", false))
    {
      coil::pid_t pid = coil::getpid();
      std::string pidc = coil::otos(pid);
      auto now = std::chrono::system_clock::now().time_since_epoch();
      auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(now);
      std::string timec = coil::otos(static_cast<int>(msec.count()));
      m_callerid = m_callerid + std::string("_") + pidc + std::string("_") + timec;
    }
    m_callerid = std::string("/")+m_callerid;

    RTC_VERBOSE(("caller id: %s", m_callerid.c_str()));


    RTC_VERBOSE(("Publisher URI: %s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));

    

    

    topicmgr.addSubscriber(this);


    XmlRpc::XmlRpcValue request;
    XmlRpc::XmlRpcValue response;


    XmlRpc::XmlRpcClient *master = ros::XMLRPCManager::instance()->getXMLRPCClient(m_roscorehost, m_roscoreport, "/");

    ROSMessageInfoBase* info = GlobalROSMessageInfoList::instance().getInfo(m_messageType);

    if(!info)
    {
      RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
      throw;
    }

    request[0] = m_callerid;
    request[1] = m_topic;
    m_datatype = info->type();
    request[2] = m_datatype;
    request[3] = std::string(ros::XMLRPCManager::instance()->getServerURI());


    bool b = master->execute("registerSubscriber", request, response);


    if(!b)
    {
      RTC_ERROR(("XML-RCP Error"));
      throw;
    }
    

    for(int i=0;i < response[2].size();i++)
    {
      connectTCP(m_callerid, m_topic, std::string(response[2][i]));
    }
  }


  /*!
   * @if jp
   * @brief バッファをセットする
   * @else
   * @brief Setting outside buffer's pointer
   * @endif
   */
  void ROSInPort::
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
  void ROSInPort::setListener(ConnectorInfo& info,
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
  void ROSInPort::setConnector(InPortConnector* connector)
  {
    m_connector = connector;
  }

  /*!
   * @if jp
   * @brief ROSTCPでサブスクライバーとパブリッシャーを接続する
   *
   *
   * @param caller_id 呼び出しID
   * @param topic トピック名
   * @param xmlrpc_uri パブリッシャーのURI
   *
   * @return 
   * 
   * @else
   * @brief 
   *
   *
   * @param caller_id 
   * @param topic 
   * @param xmlrpc_uri 
   * 
   * @return
   *
   * @endif
   */
  bool ROSInPort::connectTCP(const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri)
  {
    RTC_PARANOID(("connectTCP(%s, %s, %s)",caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));

    if(topic != m_topic)
    {
      return false;
    }

    ROSTopicManager& topicmgr = ROSTopicManager::instance();

    {
      std::lock_guard<std::mutex> guardc(m_con_mutex);
      for(auto & con : m_tcp_connecters)
      {
        if(con.getCallerID() == caller_id && con.getTopic() == topic && con.getURI() == xmlrpc_uri)
        {
          RTC_WARN(("Already connected (%s, %s, %s)",caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));
          return true;
        }
      }
    }

    std::string peer_host;
    uint32_t peer_port;

    if (!ros::network::splitURI(xmlrpc_uri, peer_host, peer_port))
    {
      RTC_ERROR(("Network Error: %s:%d", peer_host.c_str(), peer_port));
      return false;
    }
    
    RTC_VERBOSE(("Connect Publisher: %s:%d", peer_host.c_str(), peer_port));

    XmlRpc::XmlRpcClient *c = ros::XMLRPCManager::instance()->getXMLRPCClient(peer_host, peer_port, "/");
    XmlRpc::XmlRpcValue tcpros_array, protos_array, params;

    tcpros_array[0] = std::string("TCPROS");
    protos_array[0] = tcpros_array;


    params[0] = m_callerid;
    params[1] = topic;
    params[2] = protos_array;

    XmlRpc::XmlRpcValue response;

    bool b = c->execute("requestTopic", params, response);
    if(!b)
    {
      RTC_ERROR(("requestTopic Error"));
      return false;
    }

    std::string msg = response[1];

    std::string id = response[2][0];
    std::string dest_addr = response[2][1];
    int dest_port = response[2][2];

    ros::TransportTCPPtr transport(boost::make_shared<ros::TransportTCP>(&ros::PollManager::instance()->getPollSet()));

    RTC_VERBOSE(("TCP Connect:%s:%d", dest_addr.c_str(), dest_port));

    if (transport->connect(dest_addr, dest_port))
    {
      ros::ConnectionPtr connection = ros::ConnectionPtr(boost::make_shared<ros::Connection>());
      connection->initialize(transport, false, ros::HeaderReceivedFunc());
      connection->setHeaderReceivedCallback(boost::bind(&ROSInPort::onHeaderReceived, this, _1, _2));

      topicmgr.addPublisherLink(connection, caller_id, topic, xmlrpc_uri);
      PublisherLink* sub = topicmgr.getPublisherLink(connection);

      if (sub != nullptr)
      {
        std::lock_guard<std::mutex> guardc(m_con_mutex);
        m_tcp_connecters.push_back(*sub);
      }

      ROSMessageInfoBase* info = GlobalROSMessageInfoList::instance().getInfo(m_messageType);

      if(!info)
      {
        RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
        return false;
      }

      ros::M_string header;
      header["topic"] = topic;
      header["md5sum"] = info->md5sum();
      header["callerid"] = m_callerid;
      header["type"] = info->type();
      if(m_tcp_nodelay)
      {
        header["tcp_nodelay"] = "1";
      }
      else
      {
        header["tcp_nodelay"] = "0";
      }

      RTC_VERBOSE(("writeHeader()"));
      RTC_VERBOSE(("Message Type:%s", info->type().c_str()));
      RTC_VERBOSE(("Md5sum:%s", info->md5sum().c_str()));
      RTC_VERBOSE(("Message Definition:%s", info->message_definition().c_str()));
      RTC_VERBOSE(("Caller ID:%s", m_callerid.c_str()));
      RTC_VERBOSE(("Topic Name:%s", topic.c_str()));
      RTC_VERBOSE(("TCPTransPort created"));

      connection->writeHeader(header, boost::bind(&ROSInPort::onHeaderWritten, this, _1));
      return true;
    }
    return false;
  }



  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  void ROSInPort::convertReturn(BufferStatus status,
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
   * @brief メッセージ型の取得
   *
   *
   * @return メッセージ型
   * 
   * @else
   * @brief 
   *
   * 
   * @return
   *
   * @endif
   */
  const std::string& ROSInPort::datatype() const
  {
    RTC_VERBOSE(("datatype()"));
    return m_datatype;
  }
  /*!
   * @if jp
   * @brief トピック名の取得
   *
   *
   * @return トピック名
   * 
   * @else
   * @brief 
   *
   * 
   * @return
   *
   * @endif
   */
  const std::string& ROSInPort::getTopicName() const
  {
    RTC_VERBOSE(("getTopicName()"));
    return m_topic;
  }
  /*!
   * @if jp
   * @brief ノード名の取得
   *
   *
   * @return ノード名
   * 
   * @else
   * @brief 
   *
   * 
   * @return
   *
   * @endif
   */
  const std::string& ROSInPort::getName() const
  {
    RTC_VERBOSE(("getName()"));
    return m_callerid;
  }

  /*!
   * @if jp
   * @brief 受信データの統計情報取得
   *
   *
   * @param data 受信データの統計情報
   * 
   * @else
   * @brief 
   *
   * 
   * @param data
   *
   * @endif
   */
  void ROSInPort::getStats(XmlRpc::XmlRpcValue& result)
  {
    RTC_VERBOSE(("getStats()"));
    result[0] = m_topic;
    int count = 0;
    XmlRpc::XmlRpcValue stats;
    stats.setSize(0);
    std::lock_guard<std::mutex> guardc(m_con_mutex);
    for(auto & con : m_tcp_connecters)
    {
      con.getStats(stats[count]);
      count++;
    }
    result[1] = stats;
  }


  /*!
   * @if jp
   * @brief PublisherLinkの削除
   *
   *
   * @param connection ros::Connectionオブジェクト
   * @return true：削除成功
   * 
   * @else
   * @brief 
   *
   * 
   * @param connection 
   * @return
   *
   * @endif
   */
  bool ROSInPort::removePublisherLink(const ros::ConnectionPtr& connection)
  {
    RTC_PARANOID(("removePublisherLink()"));

    auto con = m_tcp_connecters.begin();
    while(con != m_tcp_connecters.end())
    {
      if(con->getConnection() == connection)
      {
        m_tcp_connecters.erase(con);
        return true;
      }
      else
      {
        ++con;
      }
    }

    return false;

  }

} // namespace RTC
