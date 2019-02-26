// -*- C++ -*-
/*!
 * @file ConnectorBase.h
 * @brief Connector base class
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

#ifndef RTC_CONNECTORBASE_H
#define RTC_CONNECTORBASE_H

#include <coil/stringutil.h>
#include <coil/Properties.h>

#include <rtm/RTC.h>
#include <rtm/CdrBufferBase.h>
#include <rtm/DataPortStatus.h>
#include <rtm/SystemLogger.h>

#include <string>
#include <utility>
#include <vector>

namespace RTC
{
  /*!
   * @if jp
   * @class ConnectorInfo クラス
   * @brief ConnectorInfo クラス
   *
   * @else
   *
   * @class ConnectorInfo class
   * @brief ConnectorInfo class
   *
   * @endif
   */
  class ConnectorInfo
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param name_ 接続名前
     * @param id_ 接続ID
     * @param ports_ 接続ポートIOR
     * @param properties_ プロパティ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param name_ connection name
     * @param id_ connection ID
     * @param ports_ connection Ports
     * @param properties_ connection properties
     *
     * @endif
     */
    ConnectorInfo(const char* name_, const char* id_,
                  coil::vstring ports_, const coil::Properties& properties_)
      : name(name_), id(id_)
      , ports(std::move(ports_)), properties(properties_)
    {
    }
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    ConnectorInfo()
    {
    }
    /*!
     * @if jp
     * @brief  接続名前
     * @else
     * @brief  Connection name
     * @endif
     */
    std::string name;
    /*!
     * @if jp
     * @brief  接続ID
     * @else
     * @brief  ConnectionID
     * @endif
     */
    std::string id;
    /*!
     * @if jp
     * @brief  接続ポートIOR
     * @else
     * @brief  Connection ports
     * @endif
     */
    coil::vstring ports;
    /*!
     * @if jp
     * @brief  プロパティ
     * @else
     * @brief  Connection properties
     * @endif
     */
    coil::Properties properties;
  };

  typedef std::vector<ConnectorInfo> ConnectorInfoList;

  class ConnectorBase;
  typedef std::vector<ConnectorBase*> ConnectorList;


  /*!
   * @if jp
   * @class ConnectorBase
   * @brief Connector 基底クラス
   *
   * InPort/OutPort, Push/Pull 各種 Connector を派生させるための
   * 基底クラス。
   *
   * @since 1.0.0
   *
   * @else
   * @class ConnectorBase
   * @brief Connector Base class
   *
   * The base class to derive subclasses for InPort/OutPort,
   * Push/Pull Connectors
   *
   * @since 1.0.0
   *
   * @endif
   */
  class ConnectorBase
    : public DataPortStatus
  {
  public:
    DATAPORTSTATUS_ENUM

    /*!
     * @if jp
     * @class Profile
     * @brief Connector profile ローカル構造体
     *
     * ConnectorBase およびその派生クラスで扱う ConnectorProfile 構造体。
     *
     * @since 1.0.0
     *
     * @else
     * @class Profile
     * @brief local representation of Connector profile
     *
     * ConnectorProfile struct for ConnectorBase and its subclasses.
     *
     * @since 1.0.0
     *
     * @endif
     */



    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ConnectorBase() {}

   /*!
     * @if jp
     * @brief Profile 取得
     *
     * Connector Profile を取得する
     *
     * @else
     * @brief Getting Profile
     *
     * This operation returns Connector Profile
     *
     * @endif
     */
    virtual const ConnectorInfo& profile() = 0;

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
    virtual const char* id() = 0;

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
    virtual const char* name() = 0;

    /*!
     * @if jp
     * @brief 接続解除関数
     *
     * Connector が保持している接続を解除する
     *
     * @else
     * @brief Disconnect connection
     *
     * This operation disconnect this connection
     *
     * @endif
     */
    virtual ReturnCode disconnect() = 0;

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
    virtual CdrBufferBase* getBuffer() = 0;

    /*!
     * @if jp
     * @brief アクティブ化
     *
     * このコネクタをアクティブ化する
     *
     * @else
     *
     * @brief Connector activation
     *
     * This operation activates this connector
     *
     * @endif
     */
    virtual void activate() = 0;

    /*!
     * @if jp
     * @brief 非アクティブ化
     *
     * このコネクタを非アクティブ化する
     *
     * @else
     *
     * @brief Connector deactivation
     *
     * This operation deactivates this connector
     *
     * @endif
     */
    virtual void deactivate() = 0;

  private:
    // non-copyable class
    //    ConnectorBase(const ConnectorBase& x);
    //    ConnectorBase& operator=(const ConnectorBase& x);
  };
};  // namespace RTC

#endif  // RTC_CONNECTORBASE_H
