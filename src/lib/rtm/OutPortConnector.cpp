// -*- C++ -*-
/*!
 * @file OutPortConnector.cpp
 * @brief OutPortConnector class
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

#include <rtm/OutPortConnector.h>
#include <rtm/InPortBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OutPortConnector::OutPortConnector(ConnectorInfo& info,
                                     ConnectorListenersBase* listeners)
    : rtclog("OutPortConnector"), m_profile(info), m_littleEndian(true),
      m_directInPort(nullptr), m_listeners(listeners), m_directMode(false), m_marshaling_type("cdr"), m_cdr(nullptr)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortConnector::~OutPortConnector()
  {
    delete m_cdr;
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
   * This operation returns ConnectorInfo
   *
   * @endif
   */
  const ConnectorInfo& OutPortConnector::profile()
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
  const char* OutPortConnector::id()
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
  const char* OutPortConnector::name()
  {
    RTC_TRACE(("name() = %s", profile().name.c_str()));
    return profile().name.c_str();
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
  void OutPortConnector::setEndian(const bool endian_type)
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
  bool OutPortConnector::isLittleEndian()
  {
    return m_littleEndian;
  }

  /*!
  * @if jp
  * @brief ダイレクト接続モードに設定
  *
  *
  * @else
  * @brief
  *
  *
  * @endif
  */
  void OutPortConnector::setPullDirectMode()
  {
    m_directMode = true;
  }

  /*!
  * @if jp
  * @brief ダイレクト接続モードかの判定
  *
  * @return True：ダイレクト接続モード,false：それ以外
  *
  * @else
  * @brief
  *
  *
  * @endif
  */
  bool OutPortConnector::pullDirectMode()
  {
    return m_directMode;
  }

  bool OutPortConnector::setInPort(InPortBase* directInPort)
  {
    if (directInPort == nullptr)
    {
      return false;
    }
    m_directInPort = directInPort;
    m_inPortListeners = directInPort->getListeners();
    return true;
  }

  BufferStatus OutPortConnector::read(ByteData&  /*data*/)
  {
    return BufferStatus::OK;
  }

  void OutPortConnector::unsubscribeInterface(const coil::Properties& /*prop*/)
  {

  }
} // namespace RTC
