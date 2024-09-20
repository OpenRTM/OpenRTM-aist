// -*- C++ -*-
/*!
 * @file InPortConnector.cpp
 * @brief InPortConnector base class
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

#include <rtm/InPortConnector.h>
#include <rtm/OutPortBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  InPortConnector::InPortConnector(ConnectorInfo& info,
                                   ConnectorListenersBase* listeners,
                                   CdrBufferBase* buffer)
    : rtclog("InPortConnector"), m_profile(info),
      m_listeners(listeners), m_buffer(buffer), m_littleEndian(true), m_outPortListeners(nullptr), m_directOutPort(nullptr), m_marshaling_type("cdr"), m_cdr(nullptr)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  InPortConnector::~InPortConnector()
  {
      SerializerFactory::instance().deleteObject(m_cdr);
  }

  /*!
   * @if jp
   * @brief ConnectorInfo 取得
   *
   * Connector ConnectorInfo を取得する
   *
   * @else
   * @brief Getting ConnectorInfo
   *
   * This operation returns Connector ConnectorInfo
   *
   * @endif
   */
  const ConnectorInfo& InPortConnector::profile()
  {
    RTC_TRACE(("profile()"));
    return m_profile;
  }

  /*!
   * @if jp
   * @brief Connector ID 取得
   *
   * Connector ID を取得する
   *
   * @else
   * @brief Getting Connector ID
   *
   * This operation returns Connector ID
   *
   * @endif
   */
  const char* InPortConnector::id()
  {
    RTC_TRACE(("id() = %s", profile().id.c_str()));
    return profile().id.c_str();
  }

  /*!
   * @if jp
   * @brief Connector 名取得
   *
   * Connector 名を取得する
   *
   * @else
   * @brief Getting Connector name
   *
   * This operation returns Connector name
   *
   * @endif
   */
  const char* InPortConnector::name()
  {
    RTC_TRACE(("name() = %s", profile().name.c_str()));
    return profile().name.c_str();
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
  CdrBufferBase* InPortConnector::getBuffer()
  {
    return m_buffer;
  }

  /*!
   * @if jp
   * @brief endianタイプ設定
   *
   * endianタイプを設定する
   *
   * @else
   * @brief Setting an endian type
   *
   * This operation set this connector's endian type
   *
   * @endif
   */
  void InPortConnector::setEndian(const bool endian_type)
  {
    RTC_TRACE(("setEndian() = %s", endian_type ? "little":"big"));
    m_littleEndian = endian_type;
  }

  /*!
   * @if jp
   * @brief endian 設定がlittleか否か返す
   * @else
   * @brief return it whether endian setting is little
   * @endif
   */
  bool InPortConnector::isLittleEndian()
  {
    return m_littleEndian;
  }

  bool InPortConnector::setOutPort(OutPortBase* directOutPort)
  {
    {
      if (directOutPort == nullptr)
      {
        return false;
      }
      m_directOutPort = directOutPort;
    
      m_outPortListeners = directOutPort->getListeners();
     return true;
    }
  }

  BufferStatus InPortConnector::write(ByteData & /*cdr*/)
  {
      return BufferStatus::OK;
  }

  void InPortConnector::unsubscribeInterface(const coil::Properties& /*prop*/)
  {

  }

} // namespace RTC
