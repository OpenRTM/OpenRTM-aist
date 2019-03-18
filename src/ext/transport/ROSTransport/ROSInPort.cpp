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


#include "ROSInPort.h"
#include <xmlrpcpp/XmlRpc.h>
#include <xmlrpcpp/XmlRpcSocket.h>
#include <ros/xmlrpc_manager.h>
#include <ros/network.h>
#include <ros/poll_manager.h>
#include <ros/connection_manager.h>
#include <coil/UUID.h>
#include "RosTopicManager.h"



#ifdef WIN32
#pragma warning( disable : 4290 )
#endif



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
   : m_buffer(0),
     m_roscoreport(11311)
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

    for(auto & con : m_tcp_connecters)
    {
      con.second->drop(ros::Connection::Destructing);
    }

    RosTopicManager& topicmgr = RosTopicManager::instance();
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

    RosTopicManager& topicmgr = RosTopicManager::instance();
    if(topicmgr.existSubscriber(this))
    {
      RTC_VERBOSE(("Subscriber already exists."));
      return;
    }

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


    RTC_VERBOSE(("Publisher URI: %s", std::string(ros::XMLRPCManager::instance()->getServerURI()).c_str()));

    

    

    topicmgr.addSubscriber(this);


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


    bool b = master->execute("registerSubscriber", request, response);


    if(!b)
    {
      RTC_ERROR(("XML-RCP Error"));
      return;
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
                                           ConnectorListeners* listeners)
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
  void ROSInPort::connectTCP(const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri)
  {
    RTC_PARANOID(("connectTCP(%s, %s, %s)",caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));


    if(topic != m_topic)
    {
      return;
    }

    if (m_tcp_connecters.count(xmlrpc_uri) > 0)
    {
      RTC_WARN(("%s already connected", xmlrpc_uri.c_str()));
      if(m_tcp_connecters[xmlrpc_uri]->isDropped())
      {
        RTC_ERROR(("delete connector: %s", xmlrpc_uri.c_str()));
        m_tcp_connecters.erase(xmlrpc_uri);
      }
      else
      {
        return;
      }
    }

    std::string peer_host;
    uint32_t peer_port;

    if (!ros::network::splitURI(xmlrpc_uri, peer_host, peer_port))
    {
      RTC_ERROR(("Network Error: %s:%d", peer_host.c_str(), peer_port));
      return;
    }
    
    RTC_VERBOSE(("Connect Publisher: %s:%d", peer_host.c_str(), peer_port));

    XmlRpc::XmlRpcClient *c = ros::XMLRPCManager::instance()->getXMLRPCClient(peer_host, peer_port, "/");
    XmlRpc::XmlRpcValue tcpros_array, protos_array, params;

    tcpros_array[0] = std::string("TCPROS");
    protos_array[0] = tcpros_array;


    params[0] = caller_id;
    params[1] = topic;
    params[2] = protos_array;

    XmlRpc::XmlRpcValue response;

    bool b = c->execute("requestTopic", params, response);
    if(!b)
    {
      RTC_ERROR(("requestTopic Error"));
      return;
    }

    int code = response[0];
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

      

      m_tcp_connecters[xmlrpc_uri] = connection;
      ROSMessageInfoBase* info = ROSMessageInfoFactory::instance().createObject(m_messageType);

      if(!info)
      {
        RTC_ERROR(("Can not find message type(%s)", m_messageType.c_str()));
        return;
      }

      ros::M_string header;
      header["topic"] = topic;
      header["md5sum"] = info->md5sum();
      header["callerid"] = caller_id;
      header["type"] = info->type();
      header["tcp_nodelay"] = "0";

      RTC_VERBOSE(("writeHeader()"));
      RTC_VERBOSE(("Message Type:%s", info->type().c_str()));
      RTC_VERBOSE(("Md5sum:%s", info->md5sum().c_str()));
      RTC_VERBOSE(("Message Definition:%s", info->message_definition().c_str()));
      RTC_VERBOSE(("Caller ID:%s", caller_id.c_str()));
      RTC_VERBOSE(("Topic Name:%s", topic.c_str()));
      RTC_VERBOSE(("TCPTransPort created"));

      ROSMessageInfoFactory::instance().deleteObject(info);



      connection->writeHeader(header, boost::bind(&ROSInPort::onHeaderWritten, this, _1));

    }
  }

  /*!
   * @if jp
   * @brief TCPコネクタの削除
   *
   *
   * @param caller_id 呼び出しID
   * @param topic トピック名
   * @param xmlrpc_uri パブリッシャーのURI
   *
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
  void ROSInPort::deleteTCPConnector(const std::string &caller_id, const std::string &topic, const std::string &xmlrpc_uri)
  {
    RTC_PARANOID(("deleteTCPConnector(%s, %s, %s)", caller_id.c_str(), topic.c_str(), xmlrpc_uri.c_str()));
    if(topic != m_topic)
    {
      RTC_VERBOSE(("Topic names do not match: %s %s", m_topic.c_str(), topic.c_str()));
      return;
    }

    if (m_tcp_connecters.count(xmlrpc_uri) > 0){
      RTC_VERBOSE(("Delete Connection: %s", xmlrpc_uri.c_str()));
      m_tcp_connecters[xmlrpc_uri]->drop(ros::Connection::Destructing);
      m_tcp_connecters.erase(xmlrpc_uri);
    }
  }

  /*!
   * @if jp
   * @brief リターンコード変換
   * @else
   * @brief Return codes conversion
   * @endif
   */
  void ROSInPort::convertReturn(BufferStatus::Enum status,
                                        ByteData& data)
  {
    switch (status)
      {
      case BufferStatus::BUFFER_OK:
        onBufferWrite(data);
        return;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        return;

      case BufferStatus::BUFFER_FULL:
        onBufferFull(data);
        onReceiverFull(data);
        return;

      case BufferStatus::BUFFER_EMPTY:
        // never come here
        return;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        return;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        return;

      default:
        onReceiverError(data);
        return;
      }
  }

} // namespace RTC
