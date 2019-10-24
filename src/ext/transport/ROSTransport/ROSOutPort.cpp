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
#include <rtm/NVUtil.h>
#include <ros/xmlrpc_manager.h>
#include <coil/UUID.h>
#include "ROSOutPort.h"
#include "ROSTopicManager.h"



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
    : rtclog("ROSOutPort"), m_start(false), m_subnum(0), m_roscoreport(11311)
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

    m_messageType = prop.getProperty("marshaling_type", "ros2:std_msgs/Float32");

    m_topic = prop.getProperty("ros.topic", "chatter");
    m_topic = "/" + m_topic;

    m_roscorehost = prop.getProperty("ros.roscore.host", "localhost");
    std::string tmp_port = prop.getProperty("ros.roscore.port", "11311");
    coil::stringTo<unsigned int>(m_roscoreport, tmp_port.c_str());

    RTC_VERBOSE(("topic name: %s", m_topic.c_str()));
    RTC_VERBOSE(("roscore address: %s:%d", m_roscorehost.c_str(), m_roscoreport));
    

    m_callerid = prop.getProperty("ros.node.name");
    if(m_callerid.empty())
    {
      coil::UUID_Generator uugen;
	    uugen.init();
	    std::unique_ptr<coil::UUID> uuid(uugen.generateUUID(2, 0x01));
      m_callerid = uuid->to_string();
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
      return;
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
        boost::shared_array<uint8_t> buffer_(new uint8_t[length]);
        memcpy(buffer_.get(), data.getBuffer(), length);

        RTC_VERBOSE(("Data size:%d", length));
        
        for(std::map<std::string, SubscriberLink>::iterator itr = m_tcp_connecters.begin(); itr != m_tcp_connecters.end(); ++itr) 
        {
          if(!itr->second.getConnection()->isDropped())
          {
            RTC_VERBOSE(("Data Write"));
            itr->second.getConnection()->write(buffer_, static_cast<uint32_t>(length), boost::bind(&ROSOutPort::onMessageWritten, this, _1), true);
          }
          else
          {
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
   * @brief ROSTCPでパブリッシャーとサブスクライバーを接続する
   *
   *
   * @param transport TransportTCP
   *
   * @return True：接続成功、False：接続失敗
   * 接続済みの場合はFalse
   * 
   * @else
   * @brief 
   *
   *
   * @param transport 
   * 
   * @return
   *
   * @endif
   */
  bool ROSOutPort::connectTCP(const ros::TransportTCPPtr& transport)
  {
    RTC_PARANOID(("connectTCP()"));

    if (m_tcp_connecters.count(transport->getClientURI()) > 0){
      RTC_VERBOSE(("Connector already exists."));
      return false;
    }

    ros::ConnectionPtr tcp_connecter = boost::make_shared<ros::Connection>();
    
    m_tcp_connecters[transport->getClientURI()] = SubscriberLink(tcp_connecter, m_subnum);
    m_subnum++;
    tcp_connecter->initialize(transport, true, boost::bind(&ROSOutPort::onConnectionHeaderReceived, this, _1, _2));
    
    if(!tcp_connecter->isDropped())
    {
      RTC_VERBOSE(("Connector created."));      
      return true;
    }
    else
    {
      m_tcp_connecters.erase(transport->getClientURI());
      RTC_VERBOSE(("Connector creation failed."));
      return false;
    }

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
      for (auto & con : m_tcp_connecters)
      {
          con.second.getConnection()->drop(ros::Connection::Destructing);
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
    return m_callerid;
  }
  /*!
   * @if jp
   * @brief コネクタの情報を取得
   *
   *
   * @param info 情報を格納する変数
   * data[0]：コネクタID
   * data[1]：接続先のノード名
   * data[2]："o"
   * data[3]：TCPROS or UDPROS
   * data[4]：トピック名
   * data[5]：true
   * data[6]：接続情報
   * 
   * @else
   * @brief 
   *
   *
   * @param info
   * 
   *
   * @endif
   */
  void ROSOutPort::getInfo(XmlRpc::XmlRpcValue& info)
  {
    int count = 0;
    for (auto & con : m_tcp_connecters)
    {
      XmlRpc::XmlRpcValue data;
      data[0] = con.second.getNum();
      data[1] = con.second.getNodeName();
      data[2] = std::string("o");
      data[3] = std::string(con.second.getConnection()->getTransport()->getType());
      data[4] = m_topic;
      data[5] = true;
      data[6] = con.second.getConnection()->getTransport()->getTransportInfo();
      info[count] = data;
      count++;
    }

  }

  /*!
   * @if jp
   * @brief コンストラクタ
   *
   * @else
   * @brief Constructor
   *
   * @endif
   */
  ROSOutPort::SubscriberLink::SubscriberLink() : m_num(0)
  {
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   *
   * @param conn ros::Connection
   * @param num コネクタのID
   *
   * @else
   * @brief Constructor
   *
   * @param conn 
   * @param num 
   *
   * @endif
   */
  ROSOutPort::SubscriberLink::SubscriberLink(ros::ConnectionPtr conn, int num)
  {
    m_conn = conn;
    m_num = num;
  }
  /*!
   * @if jp
   * @brief コピーコンストラクタ
   *
   * @param obj コピー元 
   *
   * @else
   * @brief Copy Constructor
   *
   * @param obj
   *
   * @endif
   */
  ROSOutPort::SubscriberLink::SubscriberLink(const SubscriberLink &obj)
  {
    m_conn = obj.m_conn;
    m_nodename = obj.m_nodename;
    m_num = obj.m_num;
  }
  /*!
   * @if jp
   * @brief デストラクタ
   *
   *
   * @else
   * @brief Destructor
   *
   *
   * @endif
   */
  ROSOutPort::SubscriberLink::~SubscriberLink()
  {

  }
  /*!
   * @if jp
   * @brief 接続先のノード名を設定
   *
   * @param name ノード名
   *
   * @else
   * @brief 
   *
   * @param name
   *
   * @endif
   */
  void ROSOutPort::SubscriberLink::setNoneName(std::string& name)
  {
    m_nodename = name;
  }
  /*!
   * @if jp
   * @brief 接続先のノード名を取得
   *
   * @return ノード名
   *
   * @else
   * @brief 
   *
   * @return
   *
   * @endif
   */
  const std::string ROSOutPort::SubscriberLink::getNodeName() const
  {
    return m_nodename;
  }
  /*!
   * @if jp
   * @brief ros::Connectionを設定
   *
   * @param conn ros::Connection
   *
   * @else
   * @brief 
   *
   * @param conn
   *
   * @endif
   */
  void ROSOutPort::SubscriberLink::setConnection(ros::ConnectionPtr conn)
  {
    m_conn = conn;
  }
  /*!
   * @if jp
   * @brief ros::Connectionを取得
   *
   * @return ros::Connection
   *
   * @else
   * @brief 
   *
   * @return
   *
   * @endif
   */
  ros::ConnectionPtr ROSOutPort::SubscriberLink::getConnection()
  {
    return m_conn;
  }
  /*!
   * @if jp
   * @brief コネクタのID取得
   *
   * @return コネクタのID
   *
   * @else
   * @brief 
   *
   * @return
   *
   * @endif
   */
  int ROSOutPort::SubscriberLink::getNum()
  {
    return m_num;
  }
  
} // namespace RTC
