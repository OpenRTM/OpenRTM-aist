// -*- C++ -*-
/*!
 * @file ConnectorListener.cpp
 * @brief connector listener class
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

#include <rtm/ConnectorListener.h>
#include <cstdint>

namespace RTC
{
  ConnectorListenerStatus::Enum operator|(ConnectorListenerStatus::Enum L,
                                          ConnectorListenerStatus::Enum R)
  {
    return static_cast<ConnectorListenerStatus::Enum>(static_cast<uint64_t>(L) |
                                                      static_cast<uint64_t>(R));
  }
  ConnectorListenerStatus::Enum operator&(ConnectorListenerStatus::Enum L,
                                          ConnectorListenerStatus::Enum R)
  {
    return static_cast<ConnectorListenerStatus::Enum>(static_cast<uint64_t>(L) &
                                                      static_cast<uint64_t>(R));
  }
  /*!
   * @if jp
   * @class ConnectorDataListener クラス
   * @else
   * @class ConnectorDataListener class
   * @endif
   */
  ConnectorDataListener::~ConnectorDataListener() = default;

  /*!
   * @if jp
   * @class ConnectorListener クラス
   * @else
   * @class ConnectorListener class
   * @endif
   */
  ConnectorListener::~ConnectorListener() = default;

  /*!
   * @if jp
   * @class ConnectorDataListener ホルダクラス
   * @else
   * @class ConnectorDataListener holder class
   * @endif
   */
  ConnectorDataListenerHolder::ConnectorDataListenerHolder()
  {
      SerializerFactory::instance().deleteObject(m_cdr);
  }


  ConnectorDataListenerHolder::~ConnectorDataListenerHolder()
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        if (listener.second)
          {
            delete listener.first;
          }
      }
  }


  void ConnectorDataListenerHolder::
  addListener(ConnectorDataListener* listener, bool autoclean)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConnectorDataListenerHolder::
  removeListener(ConnectorDataListener* listener)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    std::vector<Entry>::iterator it(m_listeners.begin());
    for (; it != m_listeners.end(); ++it)
      {
        if (it->first == listener)
          {
            if (it->second)
              {
                delete it->first;
              }
            m_listeners.erase(it);
            return;
          }
      }

  }

  size_t ConnectorDataListenerHolder::size()
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_listeners.size();
  }

  ConnectorDataListenerHolder::ReturnCode
    ConnectorDataListenerHolder::notify(ConnectorInfo& info,
                                                 ByteData& cdrdata, const std::string& marshalingtype)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    ConnectorListenerHolder::ReturnCode ret(NO_CHANGE);
    for (auto & listener : m_listeners)
      {
        ret = ret | listener.first->operator()(info, cdrdata, marshalingtype);
      }
    return ret;
  }

  ConnectorListenerHolder::ReturnCode ConnectorDataListenerHolder::notifyIn(ConnectorInfo& info, ByteData& data)
  {
      std::string type = info.properties.getProperty("marshaling_type", "cdr");
      std::string marshaling_type{ coil::eraseBothEndsBlank(
        info.properties.getProperty("inport.marshaling_type", type)) };
      return notify(info, data, marshaling_type);
  }

  ConnectorListenerHolder::ReturnCode ConnectorDataListenerHolder::notifyOut(ConnectorInfo& info, ByteData& data)
  {
      std::string type = info.properties.getProperty("marshaling_type", "cdr");
      std::string marshaling_type{ coil::eraseBothEndsBlank(
        info.properties.getProperty("outport.marshaling_type", type)) };
      return notify(info, data, marshaling_type);
  }

  /*!
   * @if jp
   * @class ConnectorListener ホルダクラス
   * @else
   * @class ConnectorListener holder class
   * @endif
   */
  ConnectorListenerHolder::ConnectorListenerHolder() = default;


  ConnectorListenerHolder::~ConnectorListenerHolder()
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        if (listener.second)
          {
            delete listener.first;
          }
      }
  }


  void ConnectorListenerHolder::addListener(ConnectorListener* listener,
                                            bool autoclean)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConnectorListenerHolder::removeListener(ConnectorListener* listener)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    std::vector<Entry>::iterator it(m_listeners.begin());

    for (; it != m_listeners.end(); ++it)
      {
        if (it->first == listener)
          {
            if (it->second)
              {
                delete it->first;
              }
            m_listeners.erase(it);
            return;
          }
      }
  }

  size_t ConnectorListenerHolder::size()
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_listeners.size();
  }

  ConnectorListenerHolder::ReturnCode
    ConnectorListenerHolder::notify(ConnectorInfo& info)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    ConnectorListenerHolder::ReturnCode ret(NO_CHANGE);
    for (auto & listener : m_listeners)
      {
        ret = ret | listener.first->operator()(info);
      }
    return ret;
  }


  /*!
   * @if jp
   * @class ConnectorListeners クラス
   * @else
   * @class ConnectorListeners class
   * @endif
   */

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  ConnectorListeners::ConnectorListeners()
  {
  }
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ConnectorListeners::~ConnectorListeners() = default;

  /*!
   * @if jp
   *
   * @brief リスナーへ通知する(InPort側)
   * 指定の種類のリスナのコールバックメソッドを呼び出す。
   * InPortとOutPortでシリアライザの種類が違う場合があるため、
   * InPort側ではnotifyOut関数を使用する必要がある。
   *
   * @param type リスナの種類
   * @param info ConnectorInfo
   * @param data バイト列のデータ
   * @return リターンコード
   * @else
   *
   * @brief Notify listeners. (Typed data version)
   *
   * @param type
   * @param info ConnectorInfo
   * @param data Data
   * @return
   * @endif
   */
  ::RTC::ConnectorListenerStatus::Enum ConnectorListeners::notifyIn(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data)
  {
      if (static_cast<uint8_t>(type) < connectorData_.size())
      {
          return connectorData_[static_cast<uint8_t>(type)].notifyIn(info, data);
      }
      return ConnectorListenerStatus::NO_CHANGE;
  }

  /*!
   * @if jp
   *
   * @brief リスナーへ通知する(OutPort側)
   * 指定の種類のリスナのコールバックメソッドを呼び出す。
   * InPortとOutPortでシリアライザの種類が違う場合があるため、
   * OutPort側ではnotifyOut関数を使用する必要がある。
   *
   * @param type リスナの種類
   * @param info ConnectorInfo
   * @param data バイト列のデータ
   * @return リターンコード
   * @else
   *
   * @brief Notify listeners. (Typed data version)
   *
   * @param type
   * @param info ConnectorInfo
   * @param data Data
   * @return
   * @endif
   */
  ::RTC::ConnectorListenerStatus::Enum ConnectorListeners::notifyOut(ConnectorDataListenerType type, ConnectorInfo& info, ByteData& data)
  {
      if (static_cast<uint8_t>(type) < connectorData_.size())
      {
          return connectorData_[static_cast<uint8_t>(type)].notifyOut(info, data);
      }
      return ConnectorListenerStatus::NO_CHANGE;
  }

  /*!
   * @if jp
   *
   * @brief リスナーへ通知する
   *
   * データポートの Connector において発生する各種イベントに対するコー
   * ルバックメソッド
   *
   * @param type リスナの種類
   * @param info ConnectorInfo
   * @return リターンコード
   *
   * @else
   *
   * @brief Virtual Callback method
   *
   * @param type リスナの種類
   * @param info ConnectorInfo
   * @return リターンコード
   *
   * @return
   * @endif
   */
  ::RTC::ConnectorListenerStatus::Enum ConnectorListeners::notify(ConnectorListenerType type, ConnectorInfo& info)
  {
      if (static_cast<uint8_t>(type) < connector_.size())
      {
          return connector_[static_cast<uint8_t>(type)].notify(info);
      }
      return ConnectorListenerStatus::NO_CHANGE;
  }

  /*!
   * @if jp
   *
   * @brief リスナーの追加
   *
   * 指定の種類のConnectorDataListenerを追加する。
   *
   * @param type リスナの種類
   * @param listener 追加するリスナ
   * @param autoclean true:デストラクタで削除する,
   *                  false:デストラクタで削除しない
   * @return false：指定の種類のリスナが存在しない
   * @else
   *
   * @brief Add the listener.
   *
   *
   *
   * @param type
   * @param listener Added listener
   * @param autoclean true:The listener is deleted at the destructor.,
   *                  false:The listener is not deleted at the destructor.
   * @return
   * @endif
   */
  bool ConnectorListeners::addListener(ConnectorDataListenerType type, ConnectorDataListener* listener, bool autoclean)
  {
      if (static_cast<uint8_t>(type) < connectorData_.size())
      {
          connectorData_[static_cast<uint8_t>(type)].addListener(listener, autoclean);
          return true;
      }
      return false;
  }

  /*!
   * @if jp
   *
   * @brief リスナーの追加
   *
   * 指定の種類のConnectorListenerを追加する。
   *
   * @param type リスナの種類
   * @param listener 追加するリスナ
   * @param autoclean true:デストラクタで削除する,
   *                  false:デストラクタで削除しない
   * @return false：指定の種類のリスナが存在しない
   * @else
   *
   * @brief Add the listener.
   *
   *
   *
   * @param type
   * @param listener Added listener
   * @param autoclean true:The listener is deleted at the destructor.,
   *                  false:The listener is not deleted at the destructor.
   * @return
   * @endif
   */
  bool ConnectorListeners::addListener(ConnectorListenerType type, ConnectorListener* listener, bool autoclean)
  {
      if (static_cast<uint8_t>(type) < connector_.size())
      {
          connector_[static_cast<uint8_t>(type)].addListener(listener, autoclean);
          return true;
      }
      return false;
  }

  /*!
   * @if jp
   *
   * @brief リスナーの削除
   *
   * 指定の種類のConnectorDataListenerを削除する。
   *
   * @param type リスナの種類
   * @param listener 削除するリスナ
   * @return false：指定の種類のリスナが存在しない
   *
   * @else
   *
   * @brief Remove the listener.
   *
   *
   * @param type
   * @param listener
   * @return
   *
   * @endif
   */
  bool ConnectorListeners::removeListener(ConnectorDataListenerType type, ConnectorDataListener* listener)
  {
      if (static_cast<uint8_t>(type) < connectorData_.size())
      {
          connectorData_[static_cast<uint8_t>(type)].removeListener(listener);
          return true;
      }
      return false;
  }

  /*!
   * @if jp
   *
   * @brief リスナーの削除
   *
   * 指定の種類のConnectorListenerを削除する。
   *
   * @param type リスナの種類
   * @param listener 削除するリスナ
   * @return false：指定の種類のリスナが存在しない
   *
   * @else
   *
   * @brief Remove the listener.
   *
   *
   * @param type
   * @param listener
   * @return
   *
   * @endif
   */
  bool ConnectorListeners::removeListener(ConnectorListenerType type, ConnectorListener* listener)
  {
      if (static_cast<uint8_t>(type) < connector_.size())
      {
          connector_[static_cast<uint8_t>(type)].removeListener(listener);
          return true;
      }
      return false;
  }

  /*!
  * @if jp
  * @brief デストラクタ
  * @else
  * @brief Destructor
  * @endif
  */
  ConnectorListenersBase::~ConnectorListenersBase() = default;

  /*!
   * @if jp
   *
   * @brief 指定の種類のConnectorDataListenerHolderを取得する
   *
   *
   * @param type リスナの種類
   * @return ConnectorDataListenerHolder
   *
   * @else
   *
   * @brief Remove the listener.
   *
   *
   * @param type
   * @param listener
   * @return
   *
   * @endif
   */
  ConnectorDataListenerHolder* ConnectorListeners::getDataListenerHolder(ConnectorDataListenerType type)
  {
      if (static_cast<uint8_t>(type) < connectorData_.size())
      {
          return &connectorData_[static_cast<uint8_t>(type)];
      }
      return nullptr;
  }
} // namespace RTC


