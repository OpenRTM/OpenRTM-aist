// -*- C++ -*-
/*!
 * @file  OutPortProvider.cpp
 * @brief OutPortProvider class
 * @date  $Date: 2007-12-31 03:08:05 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2009
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

#include <rtm/RTC.h>
#include <rtm/OutPortProvider.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortProvider::~OutPortProvider()
  {
  }

  /*!
   * @if jp
   * @brief 設定初期化
   * @else
   * @brief Initializing configuration
   * @endif
   */
  void OutPortProvider::init(coil::Properties& /*prop*/)
  {
  }

  /*!
   * @if jp
   * @brief InterfaceProfile情報を公開する
   * @else
   * @brief Publish InterfaceProfile information
   * @endif
   */
  void OutPortProvider::publishInterfaceProfile(SDOPackage::NVList& prop)
  {
#ifdef ORB_IS_RTORB
    SDOPackage_NVList prop_ptr(*prop.cobj());
    NVUtil::appendStringValue(prop_ptr, "dataport.interface_type",
                              m_interfaceType.c_str());
#else  // ORB_IS_RTORB
    NVUtil::appendStringValue(prop, "dataport.interface_type",
                             m_interfaceType.c_str());
#endif  // ORB_IS_RTORB
    NVUtil::append(prop, m_properties);
  }

  /*!
   * @if jp
   * @brief Interface情報を公開する
   * @else
   * @brief Publish interface information
   * @endif
   */
  bool OutPortProvider::publishInterface(SDOPackage::NVList& prop)
  {
    if (!NVUtil::isStringValue(prop,
                               "dataport.interface_type",
                               m_interfaceType.c_str()))
      {
        return false;
      }

    NVUtil::append(prop, m_properties);
    return true;
  }

  /*!
   * @if jp
   * @brief ポートタイプを設定する
   * @else
   * @brief Set the port type
   * @endif
   */
  void OutPortProvider::setPortType(const char* port_type)
  {
    m_portType = port_type;
  }

  /*!
   * @if jp
   * @brief データタイプを設定する
   * @else
   * @brief Set the data type
   * @endif
   */
  void OutPortProvider::setDataType(const char* data_type)
  {
    m_dataType = data_type;
  }

  /*!
   * @if jp
   * @brief インターフェースタイプを設定する
   * @else
   * @brief Set the interface type
   * @endif
   */
  void OutPortProvider::setInterfaceType(const char* interface_type)
  {
    m_interfaceType = interface_type;
  }

  /*!
   * @if jp
   * @brief データフロータイプを設定する
   * @else
   * @brief Set the data flow type
   * @endif
   */
  void OutPortProvider::setDataFlowType(const char* dataflow_type)
  {
    m_dataflowType = dataflow_type;
  }

  /*!
   * @if jp
   * @brief サブスクリプションタイプを設定する
   * @else
   * @brief Set the subscription type
   * @endif
   */
  void OutPortProvider::setSubscriptionType(const char* subs_type)
  {
    m_subscriptionType = subs_type;
  }
} // namespace RTC
