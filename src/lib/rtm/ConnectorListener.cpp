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

// cstdint
#if defined (_MSC_VER) && (_MSC_VER <=1500) // VC2008(VC9.0) or before
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#if __cplusplus <= 199711L
#include <stdint.h>
#else
#include <cstdint>
#endif
#endif


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
    Guard guard(m_mutex);
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
    Guard guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConnectorDataListenerHolder::
  removeListener(ConnectorDataListener* listener)
  {
    Guard guard(m_mutex);
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
    Guard guard(m_mutex);
    return m_listeners.size();
  }

  ConnectorDataListenerHolder::ReturnCode
	  ConnectorDataListenerHolder::notify(ConnectorInfo& info,
                                                 ByteData& cdrdata)
  {
    Guard guard(m_mutex);
    ConnectorListenerHolder::ReturnCode ret(NO_CHANGE);
    for (auto & listener : m_listeners)
      {
        ret = ret | listener.first->operator()(info, cdrdata);
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
    Guard guard(m_mutex);
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
    Guard guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConnectorListenerHolder::removeListener(ConnectorListener* listener)
  {
    Guard guard(m_mutex);
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
    Guard guard(m_mutex);
    return m_listeners.size();
  }

  ConnectorListenerHolder::ReturnCode
	  ConnectorListenerHolder::notify(ConnectorInfo& info)
  {
    Guard guard(m_mutex);
    ConnectorListenerHolder::ReturnCode ret(NO_CHANGE);
    for (auto & listener : m_listeners)
      {
        ret = ret | listener.first->operator()(info);
      }
    return ret;
  }
} // namespace RTC


