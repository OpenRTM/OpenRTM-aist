// -*- C++ -*-
/*!
 * @file ConfigurationListener.cpp
 * @brief Configuration related event listener classes
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

#include <rtm/ConfigurationListener.h>

namespace RTC
{

  //============================================================
  /*!
   * @if jp
   * @class ConfigurationParamListener クラス
   * @else
   * @class ConfigurationParamListener class
   * @endif
   */
  ConfigurationParamListener::~ConfigurationParamListener() {}

  /*!
   * @if jp
   * @class ConfigurationSetNameListener クラス
   * @else
   * @class ConfigurationSetNameListener class
   * @endif
   */
  ConfigurationSetNameListener::~ConfigurationSetNameListener() {}

  /*!
   * @if jp
   * @class ConfigurationSetListener クラス
   * @else
   * @class ConfigurationSetListener class
   * @endif
   */
  ConfigurationSetListener::~ConfigurationSetListener() {}


  //============================================================
  /*!
   * @if jp
   * @class ConfigurationParamListener ホルダクラス
   * @else
   * @class ConfigurationParamListener holder class
   * @endif
   */
  ConfigurationParamListenerHolder::ConfigurationParamListenerHolder()
  {
  }


  ConfigurationParamListenerHolder::~ConfigurationParamListenerHolder()
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


  void ConfigurationParamListenerHolder::
  addListener(ConfigurationParamListener* listener,
              bool autoclean)
  {
    Guard guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConfigurationParamListenerHolder::
  removeListener(ConfigurationParamListener* listener)
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


  void ConfigurationParamListenerHolder::notify(const char* config_set_name,
                                                const char* config_param_name)
  {
    Guard guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        listener.first->operator()(config_set_name, config_param_name);
      }
  }


  //============================================================
  /*!
   * @if jp
   * @class ConfigurationSetListener ホルダクラス
   * @else
   * @class ConfigurationSetListener holder class
   * @endif
   */
  ConfigurationSetListenerHolder::ConfigurationSetListenerHolder()
  {
  }


  ConfigurationSetListenerHolder::~ConfigurationSetListenerHolder()
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


  void ConfigurationSetListenerHolder::
  addListener(ConfigurationSetListener* listener,
              bool autoclean)
  {
    Guard guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConfigurationSetListenerHolder::
  removeListener(ConfigurationSetListener* listener)
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


  void ConfigurationSetListenerHolder::
  notify(const coil::Properties& config_set)
  {
    Guard guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        listener.first->operator()(config_set);
      }
  }

  //============================================================
  /*!
   * @if jp
   * @class ConfigurationSetNameListener ホルダクラス
   * @else
   * @class ConfigurationSetNameListener holder class
   * @endif
   */
  ConfigurationSetNameListenerHolder::ConfigurationSetNameListenerHolder()
  {
  }


  ConfigurationSetNameListenerHolder::~ConfigurationSetNameListenerHolder()
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


  void ConfigurationSetNameListenerHolder::
  addListener(ConfigurationSetNameListener* listener, bool autoclean)
  {
    Guard guard(m_mutex);
    m_listeners.emplace_back(listener, autoclean);
  }


  void ConfigurationSetNameListenerHolder::
  removeListener(ConfigurationSetNameListener* listener)
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


  void ConfigurationSetNameListenerHolder::notify(const char* config_set_name)
  {
    Guard guard(m_mutex);
    for (auto & listener : m_listeners)
      {
        listener.first->operator()(config_set_name);
      }
  }

} // namespace RTC


