// -*- C++ -*-
/*!
 * @file PortConnectListener.cpp
 * @brief port's internal action listener classes
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <array>
#include <rtm/PortConnectListener.h>

namespace RTC
{

  //============================================================
  /*!
   * @if jp
   * @brief PortConnectListenerType を文字列に変換
   * @else
   * @brief Convert PortConnectListenerType into the string.
   * @endif
   */
  const char*
  PortConnectListener::toString(PortConnectListenerType type)
  {
    static const std::array<const char*, 4> typeString =
      {{
        "ON_NOTIFY_CONNECT",
        "ON_NOTIFY_DISCONNECT",
        "ON_UNSUBSCRIBE_INTERFACES",
        ""
      }};
    type = type < PortConnectListenerType::PORT_CONNECT_LISTENER_NUM 
                ? type 
                : PortConnectListenerType::PORT_CONNECT_LISTENER_NUM;
    return typeString[static_cast<uint8_t>(type)];
  }

  /*!
   * @if jp
   * @class PortConnectListener クラス
   * @else
   * @class PortConnectListener class
   * @endif
   */
  PortConnectListener::~PortConnectListener() = default;

  /*!
   * @if jp
   * @brief PortConnectRetListenerType を文字列に変換
   * @else
   * @brief Convert PortConnectRetListenerType into the string.
   * @endif
   */
  const char*
  PortConnectRetListener::toString(PortConnectRetListenerType type)
  {
    static const std::array<const char*, 7> typeString =
      {{
        "ON_PUBLISH_INTERFACES",
        "ON_CONNECT_NEXTPORT",
        "ON_SUBSCRIBE_INTERFACES",
        "ON_CONNECTED",
        "ON_DISCONNECT_NEXT",
        "ON_DISCONNECTED",
        ""
      }};
    type = type < PortConnectRetListenerType::PORT_CONNECT_RET_LISTENER_NUM 
                ? type 
                : PortConnectRetListenerType::PORT_CONNECT_RET_LISTENER_NUM;
      return typeString[static_cast<uint8_t>(type)];
  }

  /*!
   * @if jp
   * @class PortConnectRetListener クラス
   * @else
   * @class PortConnectRetListener class
   * @endif
   */
  PortConnectRetListener::~PortConnectRetListener() = default;

  //============================================================
  /*!
   * @if jp
   * @class PortConnectListener ホルダクラス
   * @else
   * @class PortConnectListener holder class
   * @endif
   */
  PortConnectListenerHolder::PortConnectListenerHolder() = default;


  PortConnectListenerHolder::~PortConnectListenerHolder()
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


  void PortConnectListenerHolder::addListener(PortConnectListener* listener,
                                              bool autoclean)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void PortConnectListenerHolder::removeListener(PortConnectListener* listener)
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


  void PortConnectListenerHolder::notify(const char* portname,
                                         RTC::ConnectorProfile& profile)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        listener.first->operator()(portname, profile);
      }
  }

  //============================================================
  /*!
   * @if jp
   * @class PortConnectRetListener ホルダクラス
   * @else
   * @class PortConnectRetListener holder class
   * @endif
   */
  PortConnectRetListenerHolder::PortConnectRetListenerHolder() = default;


  PortConnectRetListenerHolder::~PortConnectRetListenerHolder()
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


  void PortConnectRetListenerHolder::
  addListener(PortConnectRetListener* listener, bool autoclean)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void PortConnectRetListenerHolder::
  removeListener(PortConnectRetListener* listener)
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


  void PortConnectRetListenerHolder::notify(const char* portname,
                                            RTC::ConnectorProfile& profile,
                                            ReturnCode_t ret)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        listener.first->operator()(portname, profile, ret);
      }
  }

  /*!
   * @if jp
   * @class PortConnectListeners クラス
   * @else
   * @class PortConnectListeners  class
   * @endif
   */
  PortConnectListeners::PortConnectListeners() = default;
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  PortConnectListeners::~PortConnectListeners() = default;

  /*!
   * @if jp
   *
   * @brief リスナーの追加
   *
   * 指定の種類のPortConnectListenerを追加する。
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
  bool PortConnectListeners::addListener(PortConnectListenerType type, PortConnectListener* listener, bool autoclean)
  {
      if (static_cast<uint8_t>(type) < portconnect_.size())
      {
          portconnect_[static_cast<uint8_t>(type)].addListener(listener, autoclean);
          return true;
      }
      return false;
  }

  /*!
   * @if jp
   *
   * @brief リスナーの削除
   *
   * 指定の種類のPortConnectListenerを削除する。
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
  bool PortConnectListeners::removeListener(PortConnectListenerType type, PortConnectListener* listener)
  {
      if (static_cast<uint8_t>(type) < portconnect_.size())
      {
          portconnect_[static_cast<uint8_t>(type)].removeListener(listener);
          return true;
      }
      return false;
  }
  /*!
   * @if jp
   *
   * @brief リスナーの追加
   *
   * 指定の種類のPortConnectRetListenerを追加する。
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
  bool PortConnectListeners::addListener(PortConnectRetListenerType type, PortConnectRetListener* listener, bool autoclean)
  {
      if (static_cast<uint8_t>(type) < portconnret_.size())
      {
          portconnret_[static_cast<uint8_t>(type)].addListener(listener, autoclean);
          return true;
      }
      return false;
  }

  /*!
   * @if jp
   *
   * @brief リスナーの削除
   *
   * 指定の種類のPortConnectRetListenerを削除する。
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
  bool PortConnectListeners::removeListener(PortConnectRetListenerType type, PortConnectRetListener* listener)
  {
      if (static_cast<uint8_t>(type) < portconnret_.size())
      {
          portconnret_[static_cast<uint8_t>(type)].removeListener(listener);
          return true;
      }
      return false;
  }

} // namespace RTC

