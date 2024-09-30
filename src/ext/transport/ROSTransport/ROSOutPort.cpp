// -*- C++ -*-
/*!
 * @file  ROSOutPort.cpp
 * @brief ROSOutPort class
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
#include <rtm/NVUtil.h>
#include <ros/xmlrpc_manager.h>
#include <coil/OS.h>
#include <coil/stringutil.h>
#include "ROSOutPort.h"
#include "ROSTopicManager.h"

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
   * @param buffer The buffer object that is attached to this Consumer
   * @endif
   */
  ROSOutPort::ROSOutPort(void)
    : rtclog("ROSOutPort"), m_start(false), m_tcp_nodelay(true), m_roscoreport(ROS_DEFAULT_MASTER_PORT), m_message_data_sent(0)
  {
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ROSOutPort::~ROSOutPort(void)
  {
    RTC_PARANOID(("~ROSOutPort()"));
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void ROSOutPort::init(coil::Properties& prop)
  { 
    RTC_PARANOID(("ROSOutPort::init()"));

    if(prop.propertyNames().size() == 0)
    {
      RTC_DEBUG(("Property is empty."));
      return;
    }


    ROSTopicManager& topicmgr = ROSTopicManager::instance();

    if(topicmgr.existPublisher(this))
    {
      RTC_VERBOSE(("Publisher already exists."));
      return;
    }

    m_properties = prop;

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

    
    RTC_VERBOSE(("Subscriber URI: %s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));


    topicmgr.addPublisher(this);

    XmlRpc::XmlRpcValue request;
    XmlRpc::XmlRpcValue response;

    
    XmlRpc::XmlRpcClient *master = ros::XMLRPCManager::instance()->getXMLRPCClient(m_roscorehost, m_roscoreport, "/");

    ROSMessageInfoBase* info = GlobalROSMessageInfoList::instance().getInfo(m_messageType);

    if(!info)
    {
      RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
      throw std::bad_alloc();
    }


    request[0] = m_callerid;
    request[1] = m_topic;
    m_datatype = info->type();
    request[2] = m_datatype;
    request[3] = std::string(ros::XMLRPCManager::instance()->getServerURI());

    bool b = master->execute("registerPublisher", request, response);

    if(!b)
    {
      RTC_ERROR(("XML-RCP Error"));
      throw std::bad_alloc();
    }
  }

  /*!
   * @if jp
   * @brief バッファへのデータ書込
   * @else
   * @brief Write data into the buffer
   * @endif
   */
  DataPortStatus ROSOutPort::put(ByteData& data)
  {
    
    RTC_PARANOID(("put()"));
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      if(m_start)
      {
        size_t length = (size_t)data.getDataLength();
        m_message_data_sent += static_cast<uint64_t>(length);
        boost::shared_array<uint8_t> buffer(new uint8_t[length]);
        memcpy(buffer.get(), data.getBuffer(), length);

        RTC_VERBOSE(("Data size:%d", length));
        
        {
          std::lock_guard<std::mutex> guardc(m_con_mutex);
          for (auto & con : m_tcp_connecters)
          {
            RTC_VERBOSE(("Data Write"));
            con.write(buffer, static_cast<uint32_t>(length));
          }
        }
        
      }
      else
      {
        RTC_WARN(("Connection has not been established yet."));
      }
      
    }
    return DataPortStatus::PORT_OK;
  }
  
  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void ROSOutPort::
  publishInterfaceProfile(SDOPackage::NVList& /*properties*/)
  {
    return;
  }

  /*!
   * @if jp
   * @brief データ送信通知への登録
   * @else
   * @brief Subscribe to the data sending notification
   * @endif
   */
  bool ROSOutPort::
  subscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    
    
    return true;
  }
  
  /*!
   * @if jp
   * @brief データ送信通知からの登録解除
   * @else
   * @brief Unsubscribe the data send notification
   * @endif
   */
  void ROSOutPort::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
      RTC_DEBUG_STR((NVUtil::toString(properties)));
      {
        std::lock_guard<std::mutex> guardc(m_con_mutex);
        for (auto & con : m_tcp_connecters)
        {
          ROSTopicManager& topicmgr = ROSTopicManager::instance();
          topicmgr.removeSubscriberLink(con.getConnection());
        }
      }
      ROSTopicManager& topicmgr = ROSTopicManager::instance();
      topicmgr.removePublisher(this);

      if (!m_roscorehost.empty())
      {

          XmlRpc::XmlRpcValue request;
          XmlRpc::XmlRpcValue response;


          XmlRpc::XmlRpcClient *master = ros::XMLRPCManager::instance()->getXMLRPCClient(m_roscorehost, m_roscoreport, "/");


          request[0] = m_callerid;
          request[1] = m_topic;
          request[2] = std::string(ros::XMLRPCManager::instance()->getServerURI());



          RTC_PARANOID(("unregisterPublisher:%s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));
          bool b = master->execute("unregisterPublisher", request, response);
          if (!b)
          {
              RTC_ERROR(("unregisterPublisher Error"));
          }

      }
  }

  /*!
   * @if jp
   * @brief ヘッダ情報受信時のコールバック関数
   *
   *
   * @param conn ros::Connectionオブジェクト
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
  bool ROSOutPort::
  onConnectionHeaderReceived(const ros::ConnectionPtr& conn, const ros::Header& header)
  {
    RTC_VERBOSE(("onConnectionHeaderReceived()"));
    std::string topic;

    if (!header.getValue("topic", topic))
    {
        RTC_ERROR(("Topic name does not exist in header"));
        return false;
    }

    if(m_topic != topic)
    {
      RTC_VERBOSE(("Topic names do not match. %s %s", m_topic.c_str(), topic.c_str()));
      return false;
    }
    
    std::string client_callerid;
    if (!header.getValue("callerid", client_callerid))
    {
        RTC_ERROR(("Callse id does not exist in header"));
        return false;
    }

    std::string md5sum;
    if (!header.getValue("md5sum", md5sum))
    {
        RTC_ERROR(("Callse id does not exist in header"));
        return false;
    }

    ROSMessageInfoBase* info = GlobalROSMessageInfoList::instance().getInfo(m_messageType);
    
    if(!info)
    {
      RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
      return false;
    }




    ros::M_string m;
    m["type"] = info->type();
    m["md5sum"] = info->md5sum();
    m["message_definition"] = info->message_definition();
    m["callerid"] = m_callerid;
    m["latching"] = "0";
    m["topic"] = topic;
    if (m_tcp_nodelay)
    {
      m["tcp_nodelay"] = "1";
    }
    else
    {
      m["tcp_nodelay"] = "0";
    }
    

    RTC_VERBOSE(("TCPTransPort created"));
    RTC_VERBOSE(("Message Type:%s", info->type().c_str()));
    RTC_VERBOSE(("Md5sum:%s", info->md5sum().c_str()));
    RTC_VERBOSE(("Message Definition:%s", info->message_definition().c_str()));
    RTC_VERBOSE(("Caller ID:%s", m_callerid.c_str()));
    RTC_VERBOSE(("Topic Name:%s", topic.c_str()));
    RTC_VERBOSE(("TCP Nodelay:%s", (m_tcp_nodelay ? "true" : "false")));
    RTC_VERBOSE(("TCPTransPort created"));


    boost::shared_array<uint8_t> buffer;
    uint32_t len;
    ros::Header::write(m, buffer, len);

    ROSTopicManager& topicmgr = ROSTopicManager::instance();
    SubscriberLink* sub = topicmgr.getSubscriberLink(conn);

    if (sub != nullptr)
    {
      sub->setNoneName(client_callerid);
      sub->setCallerID(client_callerid);
      sub->setTopic(topic);
      sub->setStatBytes(static_cast<uint64_t>(len+4));
      std::lock_guard<std::mutex> guardc(m_con_mutex);
      m_tcp_connecters.push_back(*sub);
    }


    conn->writeHeader(m, boost::bind(&ROSOutPort::onHeaderWritten, this, _1));

    return true;
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
  const std::string& ROSOutPort::datatype() const
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
  const std::string& ROSOutPort::getTopicName() const
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
  const std::string& ROSOutPort::getName() const
  {
    RTC_VERBOSE(("getName()"));
    return m_callerid;
  }

  /*!
   * @if jp
   * @brief 送信データの統計情報取得
   *
   *
   * @param data 送信データの統計情報
   * 
   * @else
   * @brief 
   *
   * 
   * @param data
   *
   * @endif
   */
  void ROSOutPort::getStats(XmlRpc::XmlRpcValue& result)
  {
    RTC_VERBOSE(("getStats()"));
    result[0] = m_topic;
    result[1] = static_cast<int>(m_message_data_sent);
    int count = 0;
    XmlRpc::XmlRpcValue stats;
    stats.setSize(0);
    std::lock_guard<std::mutex> guardc(m_con_mutex);
    for(auto & con : m_tcp_connecters)
    {
      con.getStats(stats[count]);
      count++;
    }
    result[2] = stats;
  }


  /*!
   * @if jp
   * @brief SubscriberLinkの削除
   *
   * @param connection ros::Connectionオブジェクト
   * @return true：削除成功
   * 
   * @else
   * @brief 
   *
   * @param connection 
   * @return 
   *
   * @endif
   */
  bool ROSOutPort::removeSubscriberLink(const ros::ConnectionPtr& connection)
  {
    RTC_PARANOID(("removeSubscriberLink()"));

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
