// -*- C++ -*-
/*!
 * @file ComponentObserverConsumer.h
 * @brief Component observer SDO service consumer implementation
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
 * $Id: SdoConfiguration.cpp 1971 2010-06-03 08:46:40Z n-ando $
 *
 */

#include <coil/stringutil.h>
#include <rtm/Manager.h>
#include <rtm/Typename.h>
#include "ComponentObserverSkel.h"
#include "ComponentObserverConsumer.h"
#include <iostream>

namespace RTC
{
  /*!
   * @if jp
   * @brief ctor of ComponentObserverConsumer
   * @else
   * @brief ctor of ComponentObserverConsumer
   * @endif
   */
  ComponentObserverConsumer::ComponentObserverConsumer()
  {
    for (bool & observed : m_observed)
      {
        observed = false;
      }
  }

  /*!
   * @if jp
   * @brief dtor
   * @else
   * @brief dtor
   * @endif
   */
  ComponentObserverConsumer::~ComponentObserverConsumer()
  {
      {
        std::lock_guard<std::mutex> guard(mutex);
        unsetComponentProfileListeners();
        unsetComponentStatusListeners();
        unsetPortProfileListeners();
        unsetExecutionContextListeners();
        unsetConfigurationListeners();
        unsetHeartbeat();
      }

      {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> guard(mutex);
      }
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief Initialization
   * @endif
   */
  bool
  ComponentObserverConsumer::init(RTObject_impl& rtobj,
                                  const SDOPackage::ServiceProfile& profile)
  {
    if (!m_observer.setObject(profile.service))
      {
        // narrowing failed
        return false;
      }

    m_rtobj = &rtobj;
    m_profile = profile;
    coil::Properties prop;
    NVUtil::copyToProperties(prop, profile.properties);
    setHeartbeat(prop);
    setDataPortInterval(prop);
    setListeners(prop);
    return true;
  }

  /*!
   * @if jp
   * @brief 再初期化
   * @else
   * @brief Re-initialization
   * @endif
   */
  bool
  ComponentObserverConsumer::reinit(const SDOPackage::ServiceProfile& profile)
  {
    if (!m_observer._ptr()->_is_equivalent(profile.service))
      {
        CorbaConsumer<OpenRTM::ComponentObserver> tmp;
        if (!tmp.setObject(profile.service))
          {
            return false;
          }
        m_observer.releaseObject();
        m_observer.setObject(profile.service);
      }
    m_profile= profile;
    coil::Properties prop;
    NVUtil::copyToProperties(prop, profile.properties);
    setHeartbeat(prop);
    setListeners(prop);
    return true;
  }

  /*!
   * @if jp
   * @brief ServiceProfile を取得する
   * @else
   * @brief getting ServiceProfile
   * @endif
   */
  const SDOPackage::ServiceProfile&
  ComponentObserverConsumer::getProfile() const
  {
    return m_profile;
  }  

  /*!
   * @if jp
   * @brief 終了処理
   * @else
   * @brief Finalization
   * @endif
   */
  void ComponentObserverConsumer::finalize()
  {
    unsetComponentProfileListeners();
    unsetComponentStatusListeners();
    unsetPortProfileListeners();
    unsetExecutionContextListeners();
    unsetConfigurationListeners();
  }

  //============================================================
  // protected functions

  /*!
   * @if jp
   * @brief RTObjectへのリスナ接続処理
   * @else
   * @brief Connectiong listeners to RTObject
   * @endif
   */
  void ComponentObserverConsumer::setListeners(coil::Properties& prop)
  {
    if (prop["observed_status"].empty())
      {
        prop["observed_status"] = "ALL";
      }

    coil::vstring observedlist(coil::split(prop["observed_status"], ","));
    bool flags[OpenRTM::STATUS_KIND_NUM];
    for (bool & flag : flags)
      {
        flag = false;
      }
    for (auto&& observed : observedlist)
      {
        observed = coil::toUpper(std::move(observed));
        if (observed == "COMPONENT_PROFILE")
          {
            flags[OpenRTM::COMPONENT_PROFILE] = true;
          }
        else if (observed == "RTC_STATUS")
          {
            flags[OpenRTM::RTC_STATUS] = true;
          }
        else if (observed == "EC_STATUS")
          {
            flags[OpenRTM::EC_STATUS] = true;
          }
        else if (observed == "PORT_PROFILE")
          {
            flags[OpenRTM::PORT_PROFILE] = true;
          }
        else if (observed == "CONFIGURATION")
          {
            flags[OpenRTM::CONFIGURATION] = true;
          }
        else if (observed == "ALL")
          {
            for (bool & flag : flags)
              {
                flag = true;
              }
            break;
          }
      }
  
    switchListeners(flags[OpenRTM::COMPONENT_PROFILE],
                    m_observed[OpenRTM::COMPONENT_PROFILE],
                    &ComponentObserverConsumer::setComponentProfileListeners,
                    &ComponentObserverConsumer::unsetComponentProfileListeners);
    switchListeners(flags[OpenRTM::RTC_STATUS],
                    m_observed[OpenRTM::RTC_STATUS],
                    &ComponentObserverConsumer::setComponentStatusListeners,
                    &ComponentObserverConsumer::unsetComponentStatusListeners);
    switchListeners(flags[OpenRTM::EC_STATUS],
                    m_observed[OpenRTM::EC_STATUS],
                    &ComponentObserverConsumer::setExecutionContextListeners,
                    &ComponentObserverConsumer::unsetExecutionContextListeners);
    switchListeners(flags[OpenRTM::PORT_PROFILE],
                    m_observed[OpenRTM::PORT_PROFILE],
                    &ComponentObserverConsumer::setPortProfileListeners,
                    &ComponentObserverConsumer::unsetPortProfileListeners);
    switchListeners(flags[OpenRTM::CONFIGURATION],
                    m_observed[OpenRTM::CONFIGURATION],
                    &ComponentObserverConsumer::setConfigurationListeners,
                    &ComponentObserverConsumer::unsetConfigurationListeners);

  }

  /*!
   * @if jp
   * @brief リスナ接続・切断スイッチング処理
   * @else
   * @brief Switching listeners connecting/disconnecting
   * @endif
   */
  void ComponentObserverConsumer::
  switchListeners(bool& next, bool& pre,
                  void (ComponentObserverConsumer::*setfunc)(), 
                  void (ComponentObserverConsumer::*unsetfunc)())
  {
    if (!pre && next)
      {
        (this->*setfunc)();
        pre = true;
      }
    else if (pre && !next)
      {
        (this->*unsetfunc)();
        pre = false;
      }
  }

  //============================================================
  // Heartbeat related functions

  /*!
   * @if jp
   * @brief ハートビートを設定する
   * @else
   * @brief Setting heartbeat
   * @endif
   */
  void ComponentObserverConsumer::setHeartbeat(coil::Properties& prop)
  {
    unsetHeartbeat();
    if (coil::toBool(prop["heartbeat.enable"], "YES", "NO", false))
      {
        std::chrono::nanoseconds interval{std::chrono::seconds(1)};
        if (prop["heartbeat.interval"].empty()
            || !coil::stringTo(interval, prop["heartbeat.interval"].c_str()))
          {
            interval = std::chrono::seconds(1);
          }
        m_heartbeat = true;
        m_hbtaskid = Manager::instance().addTask([this]{
          if (m_heartbeat) { updateStatus(OpenRTM::HEARTBEAT, ""); }
        }, interval);
}
  }

  /*!
   * @if jp
   * @brief データポートイベントの間隔を設定する
   * @else
   * @brief Setting interval of dataport events
   * @endif
   */
  void ComponentObserverConsumer::setDataPortInterval(coil::Properties& prop)
  {
    std::chrono::microseconds outportInterval;
    if (coil::stringTo(outportInterval,
                      prop["port_profile.send_event.min_interval"].c_str()))
      {
        m_outportInterval = outportInterval;
      }
    std::chrono::microseconds inportInterval;
    if (coil::stringTo(inportInterval,
                      prop["port_profile.receive_event.min_interval"].c_str()))
      {
        m_inportInterval = inportInterval;
      }
  }

  /*!
   * @if jp
   * @brief ハートビートを解除する
   * @else
   * @brief Unsetting heartbeat
   * @endif
   */
  void ComponentObserverConsumer::unsetHeartbeat()
  {
    if(m_heartbeat)
      {
        Manager::instance().removeTask(m_hbtaskid);
        m_heartbeat = false;
      }
  }


  //============================================================
  // Component status

  /*!
   * @if jp
   * @brief RTC状態変化リスナの設定処理
   * @else
   * @brief Setting RTC status listeners
   * @endif
   */
  void ComponentObserverConsumer::setComponentStatusListeners()
  {
    if (m_compstat.activatedListener == nullptr)
      {
        m_compstat.activatedListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_ACTIVATED,
                                                  m_compstat,
                                                  &CompStatMsg::onActivated);
      }
    if (m_compstat.deactivatedListener == nullptr)
      {
        m_compstat.deactivatedListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_DEACTIVATED,
                                                  m_compstat,
                                                  &CompStatMsg::onDeactivated);
      }
    if (m_compstat.resetListener == nullptr)
      {
        m_compstat.resetListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_RESET,
                                                  m_compstat,
                                                  &CompStatMsg::onReset);
      }
    if (m_compstat.abortingListener == nullptr)
      {
        m_compstat.abortingListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_ABORTING,
                                                  m_compstat,
                                                  &CompStatMsg::onAborting);
      }
    if (m_compstat.finalizeListener == nullptr)
      {
        m_compstat.finalizeListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_FINALIZE,
                                                  m_compstat,
                                                  &CompStatMsg::onFinalize);
      }
  }
  
  /*!
   * @if jp
   * @brief RTC状態変化リスナの解除処理
   * @else
   * @brief Unsetting RTC status listeners
   * @endif
   */
  void ComponentObserverConsumer::unsetComponentStatusListeners()
  {
    if (m_compstat.activatedListener != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_ACTIVATED,
                                                 m_compstat.activatedListener);
        m_compstat.activatedListener = nullptr;
      }
    if (m_compstat.deactivatedListener != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_DEACTIVATED,
                                               m_compstat.deactivatedListener);
        m_compstat.deactivatedListener = nullptr;
      }
    if (m_compstat.resetListener != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_RESET,
                                                   m_compstat.resetListener);
        m_compstat.resetListener = nullptr;
      }
    if (m_compstat.abortingListener != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_ABORTING,
                                                   m_compstat.abortingListener);
        m_compstat.abortingListener = nullptr;
      }
    if (m_compstat.finalizeListener != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_FINALIZE,
                                                   m_compstat.finalizeListener);
        m_compstat.finalizeListener = nullptr;
      }
  }

  //============================================================
  // Port profile
  /*!
   * @if jp
   * @brief Portプロファイル変化リスナの設定処理
   * @else
   * @brief Setting port profile listener
   * @endif
   */
  void ComponentObserverConsumer::
  setPortProfileListeners()
  {
    if (m_portaction.portAddListener == nullptr)
      {
        m_portaction.portAddListener =
          m_rtobj->addPortActionListener(ADD_PORT,
                                         m_portaction,
                                         &PortAction::onAddPort);
      }
    if (m_portaction.portRemoveListener == nullptr)
      {
        m_portaction.portRemoveListener =
          m_rtobj->addPortActionListener(REMOVE_PORT,
                                         m_portaction,
                                         &PortAction::onRemovePort);
      }
    if (m_portaction.portConnectListener == nullptr)
      {
        m_portaction.portConnectListener =
          m_rtobj->addPortConnectRetListener(ON_CONNECTED,
                                             m_portaction,
                                             &PortAction::onConnect);
      }
    if (m_portaction.portDisconnectListener == nullptr)
      {
        m_portaction.portDisconnectListener =
          m_rtobj->addPortConnectRetListener(ON_DISCONNECTED,
                                             m_portaction,
                                             &PortAction::onDisconnect);
      }

    const std::vector<InPortBase*>& inports = m_rtobj->getInPorts();
    for (auto inport : inports)
      {
        std::string msg("RECEIVE:InPort:");
        msg += inport->getName();
        DataPortAction *action = new DataPortAction(*this, msg,
                                                    m_inportInterval);
        inport->addConnectorDataListener(ON_RECEIVED,
                                                    action);
        m_recievedactions.emplace_back(action);

      }
    const std::vector<OutPortBase*>& outports = m_rtobj->getOutPorts();
    for (auto outport : outports)
      {
        std::string msg("SEND:OutPort:");
        msg += outport->getName();
        DataPortAction *action = new DataPortAction(*this, msg,
                                                    m_outportInterval);
        outport->addConnectorDataListener(ON_SEND,
                                              action);
        m_sendactions.emplace_back(action);
      }
  }

  /*!
   * @if jp
   * @brief Portプロファイル変化リスナの解除処理
   * @else
   * @brief Unsetting port profile listener
   * @endif
   */
  void ComponentObserverConsumer::unsetPortProfileListeners()
  {
    if (m_portaction.portAddListener != nullptr)
      {
        m_rtobj->removePortActionListener(ADD_PORT,
                                          m_portaction.portAddListener);
        m_portaction.portAddListener = nullptr;
      }
    if (m_portaction.portRemoveListener != nullptr)
      {
        m_rtobj->removePortActionListener(REMOVE_PORT,
                                          m_portaction.portRemoveListener);
        m_portaction.portRemoveListener = nullptr;
      }
    if (m_portaction.portConnectListener != nullptr)
      {
        m_rtobj->removePortConnectRetListener(ON_CONNECTED,
                                              m_portaction.portConnectListener);
        m_portaction.portConnectListener = nullptr;
      }
    if (m_portaction.portDisconnectListener != nullptr)
      {
        m_rtobj->removePortConnectRetListener(ON_DISCONNECTED,
                                           m_portaction.portDisconnectListener);
        m_portaction.portDisconnectListener = nullptr;
      }


    const std::vector<InPortBase*>& inports = m_rtobj->getInPorts();
    for (auto inport : inports)
      {
        for (auto & m_recievedaction : m_recievedactions)
          {
            inport->removeConnectorDataListener(ON_RECEIVED, m_recievedaction);
          }
      }
    const std::vector<OutPortBase*>& outports = m_rtobj->getOutPorts();
    for (auto outport : outports)
      {
        for (auto & m_sendaction : m_sendactions)
          {
            outport->removeConnectorDataListener(ON_SEND, m_sendaction);
          }
      }
  }

  //============================================================
  // ExecutionContext profile

  /*!
   * @if jp
   * @brief ECの状態変化リスナの設定
   * @else
   * @brief Setting EC status listener
   * @endif
   */
  void ComponentObserverConsumer::setExecutionContextListeners()
  {
    if (m_ecaction.ecAttached == nullptr)
      {
        m_ecaction.ecAttached =
          m_rtobj->addExecutionContextActionListener(EC_ATTACHED,
                                                     m_ecaction,
                                                     &ECAction::onAttached);
      }
    if (m_ecaction.ecDetached == nullptr)
      {
        m_ecaction.ecDetached = 
          m_rtobj->addExecutionContextActionListener(EC_DETACHED,
                                                     m_ecaction,
                                                     &ECAction::onDetached);
      }
    if (m_ecaction.ecRatechanged == nullptr)
      {
        m_ecaction.ecRatechanged = 
          m_rtobj->addPostComponentActionListener(POST_ON_RATE_CHANGED,
                                                  m_ecaction,
                                                  &ECAction::onRateChanged);
      }
    if (m_ecaction.ecStartup == nullptr)
      {
        m_ecaction.ecStartup = 
          m_rtobj->addPostComponentActionListener(POST_ON_STARTUP,
                                                  m_ecaction,
                                                  &ECAction::onStartup);
      }
    if (m_ecaction.ecShutdown == nullptr)
      {
        m_ecaction.ecShutdown = 
          m_rtobj->addPostComponentActionListener(POST_ON_SHUTDOWN,
                                                  m_ecaction,
                                                  &ECAction::onShutdown);
      }
  }

  /*!
   * @if jp
   * @brief ECの状態変化リスナの解除
   * @else
   * @brief Unsetting EC status listener
   * @endif
   */
  void ComponentObserverConsumer::unsetExecutionContextListeners()
  {
    if (m_ecaction.ecAttached != nullptr)
      {
        m_rtobj->removeExecutionContextActionListener(EC_ATTACHED,
                                                      m_ecaction.ecAttached);
      }
    if (m_ecaction.ecDetached != nullptr)
      {
        m_rtobj->removeExecutionContextActionListener(EC_DETACHED,
                                                      m_ecaction.ecDetached);
      }
    if (m_ecaction.ecRatechanged != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_RATE_CHANGED,
                                                   m_ecaction.ecRatechanged);
      }
    if (m_ecaction.ecStartup != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_STARTUP,
                                                   m_ecaction.ecStartup);
      }
    if (m_ecaction.ecShutdown != nullptr)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_SHUTDOWN,
                                                   m_ecaction.ecShutdown);
      }
  }

  //============================================================
  // ComponentProfile related functions
  /*!
   * @if jp
   * @brief ComponentProfile状態変化リスナの設定
   * @else
   * @brief Setting ComponentProfile listener
   * @endif
   */
  void ComponentObserverConsumer::setComponentProfileListeners()
  {
  }

  /*!
   * @if jp
   * @brief ComponentProfile状態変化リスナの解除
   * @else
   * @brief Unsetting ComponentProfile listener
   * @endif
   */
  void ComponentObserverConsumer::unsetComponentProfileListeners()
  {
  }


  //============================================================
  // Configuration

  void ComponentObserverConsumer::setConfigurationListeners()
  {
    m_configMsg.updateConfigParamListener = 
      m_rtobj->addConfigurationParamListener(ConfigurationParamListenerType::ON_UPDATE_CONFIG_PARAM,
                                             m_configMsg,
                                             &ConfigAction::updateConfigParam);
    m_configMsg.setConfigSetListener = 
      m_rtobj->addConfigurationSetListener(ConfigurationSetListenerType::ON_SET_CONFIG_SET,
                                             m_configMsg,
                                             &ConfigAction::setConfigSet);
    m_configMsg.addConfigSetListener = 
      m_rtobj->addConfigurationSetListener(ConfigurationSetListenerType::ON_ADD_CONFIG_SET,
                                             m_configMsg,
                                             &ConfigAction::addConfigSet);
    m_configMsg.updateConfigSetListener = 
      m_rtobj->addConfigurationSetNameListener(ConfigurationSetNameListenerType::ON_UPDATE_CONFIG_SET,
                                               m_configMsg,
                                               &ConfigAction::updateConfigSet);
    m_configMsg.removeConfigSetListener = 
      m_rtobj->addConfigurationSetNameListener(ConfigurationSetNameListenerType::ON_REMOVE_CONFIG_SET,
                                               m_configMsg,
                                               &ConfigAction::removeConfigSet);
    m_configMsg.activateConfigSetListener = 
      m_rtobj->addConfigurationSetNameListener(ConfigurationSetNameListenerType::ON_ACTIVATE_CONFIG_SET,
                                               m_configMsg,
                                              &ConfigAction::activateConfigSet);
  }

  /*!
   * @if jp
   * @brief Configuration状態変化リスナの解除
   * @else
   * @brief Unsetting Configurationlistener
   * @endif
   */
  void ComponentObserverConsumer::unsetConfigurationListeners()
  {

    if (m_configMsg.updateConfigParamListener != nullptr)
      {
        m_rtobj->
          removeConfigurationParamListener(ConfigurationParamListenerType::ON_UPDATE_CONFIG_PARAM,
                                      m_configMsg.updateConfigParamListener);
        m_configMsg.updateConfigParamListener = nullptr;
      }
    if (m_configMsg.setConfigSetListener != nullptr)
      {
        m_rtobj->removeConfigurationSetListener(ConfigurationSetListenerType::ON_SET_CONFIG_SET,
                                           m_configMsg.setConfigSetListener);
        m_configMsg.setConfigSetListener = nullptr;
      }
    if (m_configMsg.addConfigSetListener != nullptr)
      {
        m_rtobj->removeConfigurationSetListener(ConfigurationSetListenerType::ON_ADD_CONFIG_SET,
                                            m_configMsg.addConfigSetListener);
        m_configMsg.addConfigSetListener = nullptr;
      }
    if (m_configMsg.updateConfigSetListener != nullptr)
      {
        m_rtobj->removeConfigurationSetNameListener(ConfigurationSetNameListenerType::ON_UPDATE_CONFIG_SET,
                                          m_configMsg.updateConfigSetListener);
        m_configMsg.updateConfigSetListener = nullptr;
      }
    if (m_configMsg.removeConfigSetListener != nullptr)
      {
        m_rtobj->removeConfigurationSetNameListener(ConfigurationSetNameListenerType::ON_REMOVE_CONFIG_SET,
                                          m_configMsg.removeConfigSetListener);
        m_configMsg.removeConfigSetListener = nullptr;
      }
    if (m_configMsg.activateConfigSetListener != nullptr)
      {
        m_rtobj->removeConfigurationSetNameListener(ConfigurationSetNameListenerType::ON_ACTIVATE_CONFIG_SET,
                                        m_configMsg.activateConfigSetListener);
        m_configMsg.activateConfigSetListener = nullptr;
      }
  }
  
  
} // namespace RTC

extern "C"
{
  void ComponentObserverConsumerInit()
  {
    RTC::SdoServiceConsumerFactory& factory
      = RTC::SdoServiceConsumerFactory::instance();
#ifndef ORB_IS_RTORB
    factory.addFactory(CORBA_Util::toRepositoryId<OpenRTM::ComponentObserver>(),
#else
    factory.addFactory(CORBA_Util::
                       toRepositoryIdOfObject<OpenRTM::ComponentObserver>(),
#endif
                       ::coil::Creator< ::RTC::SdoServiceConsumerBase,
                       ::RTC::ComponentObserverConsumer>);
  }
}
