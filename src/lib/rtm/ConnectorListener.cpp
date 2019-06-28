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
  ConnectorDataListener::~ConnectorDataListener() {}

  /*!
   * @if jp
   * @class ConnectorListener クラス
   * @else
   * @class ConnectorListener class
   * @endif
   */
  ConnectorListener::~ConnectorListener() {}

  /*!
   * @if jp
   * @class ConnectorDataListener ホルダクラス
   * @else
   * @class ConnectorDataListener holder class
   * @endif
   */
  ConnectorDataListenerHolder::ConnectorDataListenerHolder()
  {
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


  /*!
   * @if jp
   * @class ConnectorListener ホルダクラス
   * @else
   * @class ConnectorListener holder class
   * @endif
   */
  ConnectorListenerHolder::ConnectorListenerHolder()
  {
  }


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
  ConnectorListeners::~ConnectorListeners()
  {
  }
} // namespace RTC


