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

#include <rtm/NVUtil.h>
#include <ros/xmlrpc_manager.h>
#include <coil/UUID.h>
#include "ROSOutPort.h"
#include "RosTopicManager.h"



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
    : rtclog("ROSOutPort")
  {
    m_start = false;
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
    
    for(auto & con : m_tcp_connecters)
    {
      con.second->drop(ros::Connection::Destructing);
    }
    RosTopicManager& topicmgr = RosTopicManager::instance();
    topicmgr.removePublisher(this);

    if(!m_roscorehost.empty())
    {

      XmlRpc::XmlRpcValue request;
      XmlRpc::XmlRpcValue response;


      XmlRpc::XmlRpcClient *master = ros::XMLRPCManager::instance()->getXMLRPCClient(m_roscorehost, m_roscoreport, "/");

      
      request[0] = m_callerid;
      request[1] = m_topic;
      request[2] = std::string(ros::XMLRPCManager::instance()->getServerURI());

      

      RTC_PARANOID(("unregisterPublisher:%s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));
      bool b = master->execute("unregisterPublisher", request, response);
      if(!b)
      {
        RTC_ERROR(("unregisterPublisher Error"));
      }
      
    }

    
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


    RosTopicManager& topicmgr = RosTopicManager::instance();

    if(topicmgr.existPublisher(this))
    {
      RTC_VERBOSE(("Publisher already exists."));
      return;
    }

    m_properties = prop;

    m_messageType = prop.getProperty("marshaling_type", "ROSFloat32");

    m_topic = prop.getProperty("topic", "chatter");
    m_topic = "/" + m_topic;

    m_roscorehost = prop.getProperty("roscore_host", "localhost");
    std::string tmp_port = prop.getProperty("roscore_port", "11311");
    coil::stringTo<unsigned int>(m_roscoreport, tmp_port.c_str());

    RTC_VERBOSE(("topic name: %s", m_topic.c_str()));
    RTC_VERBOSE(("roscore address: %s:%d", m_roscorehost.c_str(), m_roscoreport));
    


    if(m_callerid.empty())
    {
      coil::UUID_Generator uugen;
	    uugen.init();
	    std::auto_ptr<coil::UUID> uuid(uugen.generateUUID(2, 0x01));
      m_callerid = uuid->to_string();
    }

    RTC_VERBOSE(("caller id: %s", m_callerid.c_str()));

    
    RTC_VERBOSE(("Subscriber URI: %s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));

    

    
    

    topicmgr.addPublisher(this);

    XmlRpc::XmlRpcValue request;
    XmlRpc::XmlRpcValue response;

    
    XmlRpc::XmlRpcClient *master = ros::XMLRPCManager::instance()->getXMLRPCClient(m_roscorehost, m_roscoreport, "/");

    ROSMessageInfoBase* info = ROSMessageInfoFactory::instance().createObject(m_messageType);

    if(!info)
    {
      RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
      return;
    }


    request[0] = m_callerid;
    request[1] = m_topic;
    request[2] = info->type();
    request[3] = std::string(ros::XMLRPCManager::instance()->getServerURI());

    ROSMessageInfoFactory::instance().deleteObject(info);

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
  InPortConsumer::ReturnCode ROSOutPort::put(ByteData& data)
  {
    
    RTC_PARANOID(("put()"));
    {
      Guard guard(m_mutex);
      if(m_start)
      {
        size_t length = (size_t)data.getDataLength();
        boost::shared_array<uint8_t> buffer_(new uint8_t[length]);
        memcpy(buffer_.get(), data.getBuffer(), length);

        RTC_VERBOSE(("Data size:%d", length));
        
        for(std::map<std::string, ros::ConnectionPtr>::iterator itr = m_tcp_connecters.begin(); itr != m_tcp_connecters.end(); ++itr) 
        {
          if(!itr->second->isDropped())
          {
            RTC_VERBOSE(("Data Write"));
            itr->second->write(buffer_, length, boost::bind(&ROSOutPort::onMessageWritten, this, _1), true);
          }
          else
          {
            /*
            RTC_ERROR(("Disconnect:%s", itr->first.c_str()));
            itr = m_tcp_connecters.erase(itr);
            */
          }
          
        }
        
      }
      else
      {
        RTC_WARN(("Connection has not been established yet."));
      }
      
    }
    return PORT_OK;
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

    /*
    for(std::map<std::string, ros::ConnectionPtr>::iterator itr = m_tcp_connecters.begin(); itr != m_tcp_connecters.end(); ++itr) 
    {
      if(itr->second->isDropped())
      {
        RTC_ERROR(("Disconnect:%s", itr->first.c_str()));
        itr = m_tcp_connecters.erase(itr);
      }
    }
    */
    if (m_tcp_connecters.count(transport->getClientURI()) > 0){
      RTC_VERBOSE(("Connector already exists."));
      return false;
    }

    ros::ConnectionPtr tcp_connecter = boost::make_shared<ros::Connection>();
    
    tcp_connecter->initialize(transport, true, boost::bind(&ROSOutPort::onConnectionHeaderReceived, this, _1, _2));
    
    if(!tcp_connecter->isDropped())
    {
      RTC_VERBOSE(("Connector created."));
      m_tcp_connecters[transport->getClientURI()] = tcp_connecter;
      return true;
    }
    else
    {
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
  publishInterfaceProfile(SDOPackage::NVList& properties)
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
    RTC_TRACE(("unsubscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    
  }


  
} // namespace RTC
