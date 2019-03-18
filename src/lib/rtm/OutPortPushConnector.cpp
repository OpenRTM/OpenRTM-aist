// -*- C++ -*-
/*!
 * @file OutPortPushConnector.cpp
 * @brief OutPortPush type connector class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/stringutil.h>

#include <rtm/OutPortPushConnector.h>
#include <rtm/ConnectorListener.h>

#include <string>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OutPortPushConnector::OutPortPushConnector(ConnectorInfo info,
                                             InPortConsumer* consumer,
                                             ConnectorListeners& listeners,
                                             CdrBufferBase* buffer)
    : OutPortConnector(info, listeners),
      m_consumer(consumer), m_publisher(nullptr),
      m_listeners(listeners), m_buffer(buffer)
  {
    // publisher/buffer creation. This may throw std::bad_alloc;
    m_publisher = createPublisher(info);
    if (m_buffer == nullptr)
      {
        m_buffer = createBuffer(info);
      }
    if (m_publisher == nullptr || m_buffer == nullptr || m_consumer == nullptr)
      { throw std::bad_alloc(); }

    if (m_publisher->init(info.properties) != PORT_OK)
      {
        throw std::bad_alloc();
      }
    m_buffer->init(info.properties.getNode("buffer"));
    m_consumer->init(info.properties);

    m_publisher->setConsumer(m_consumer);
    m_publisher->setBuffer(m_buffer);
    m_publisher->setListener(m_profile, &m_listeners);

    m_marshaling_type = info.properties.getProperty("marshaling_type", "corba");
    coil::eraseBothEndsBlank(m_marshaling_type);

    onConnect();
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortPushConnector::~OutPortPushConnector()
  {
    onDisconnect();
    disconnect();
  }

  /*!
   * @if jp
   * @brief データの書き込み
   * @else
   * @brief Writing data
   * @endif
   */
  ConnectorBase::ReturnCode
  OutPortPushConnector::write(RTC::ByteDataStreamBase* data)
  {
    RTC_TRACE(("write()"));
    RTC_PARANOID(("data size = %d bytes", data->getDataLength()));
    
    return m_publisher->write(data, 0, 0);
  }

  /*!
   * @if jp
   * @brief 接続解除
   * @else
   * @brief disconnect
   * @endif
   */
  ConnectorBase::ReturnCode OutPortPushConnector::disconnect()
  {
    RTC_TRACE(("disconnect()"));
    // delete publisher
    if (m_publisher != nullptr)
      {
        RTC_DEBUG(("delete publisher"));
        PublisherFactory& pfactory(PublisherFactory::instance());
        pfactory.deleteObject(m_publisher);
      }
    m_publisher = nullptr;

    // delete consumer
    if (m_consumer != nullptr)
      {
        RTC_DEBUG(("delete consumer"));
        InPortConsumerFactory& cfactory(InPortConsumerFactory::instance());
        cfactory.deleteObject(m_consumer);
      }
    m_consumer = nullptr;

    // delete buffer
    if (m_buffer != nullptr)
      {
        RTC_DEBUG(("delete buffer"));
        CdrBufferFactory& bfactory(CdrBufferFactory::instance());
        bfactory.deleteObject(m_buffer);
      }
    m_buffer = nullptr;
    RTC_TRACE(("disconnect() done"));
    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief アクティブ化
   * このコネクタをアクティブ化する
   * @else
   * @brief Connector activation
   * This operation activates this connector
   * @endif
   */
  void OutPortPushConnector::activate()
  {
    m_publisher->activate();
  }

  /*!
   * @if jp
   * @brief 非アクティブ化
   * このコネクタを非アクティブ化する
   * @else
   * @brief Connector deactivation
   * This operation deactivates this connector
   * @endif
   */
  void OutPortPushConnector::deactivate()
  {
    m_publisher->deactivate();
  }

  /*!
   * @if jp
   * @brief Buffer を取得する
   *
   * Connector が保持している Buffer を返す
   *
   * @else
   * @brief Getting Buffer
   *
   * This operation returns this connector's buffer
   *
   * @endif
   */
  CdrBufferBase* OutPortPushConnector::getBuffer()
  {
    return m_buffer;
  }

  /*!
   * @if jp
   * @brief Publisherの生成
   * @else
   * @brief create publisher
   * @endif
   */
  PublisherBase* OutPortPushConnector::createPublisher(ConnectorInfo& info)
  {
    std::string pub_type;
    pub_type = info.properties.getProperty("io_mode");
    if (pub_type.empty())
      {
        pub_type = info.properties.getProperty("subscription_type", "flush");
        if (pub_type == "flush")    { info.properties["io_mode"] = "block";    }
        else if (pub_type == "new") { info.properties["io_mode"] = "nonblock"; }
        else                        { info.properties["io_mode"] = pub_type;   }
      }
    coil::normalize(pub_type);
    return PublisherFactory::instance().createObject(pub_type);
  }

  /*!
   * @if jp
   * @brief Bufferの生成
   * @else
   * @brief create buffer
   * @endif
   */
  CdrBufferBase* OutPortPushConnector::createBuffer(ConnectorInfo& info)
  {
    std::string buf_type;
    buf_type = info.properties.getProperty("buffer_type",
                                              "ring_buffer");
    return CdrBufferFactory::instance().createObject(buf_type);
  }

  /*!
   * @if jp
   * @brief 接続確立時にコールバックを呼ぶ
   * @else
   * @brief Invoke callback when connection is established
   * @endif
   */
  void OutPortPushConnector::onConnect()
  {
    m_listeners.connector_[ON_CONNECT].notify(m_profile);
  }

  /*!
   * @if jp
   * @brief 接続切断時にコールバックを呼ぶ
   * @else
   * @brief Invoke callback when connection is destroied
   * @endif
   */
  void OutPortPushConnector::onDisconnect()
  {
    m_listeners.connector_[ON_DISCONNECT].notify(m_profile);
  }
} // namespace RTC

