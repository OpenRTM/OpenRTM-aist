// -*- C++ -*-
/*!
 * @file ComponentObserverConsumer.h
 * @brief Component observer SDO service consumer implementation
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: SdoConfiguration.cpp 1971 2010-06-03 08:46:40Z n-ando $
 *
 */

#include <coil/stringutil.h>
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
    : m_rtobj(NULL),
      m_compstat(*this), m_portaction(*this),
      m_ecaction(*this), m_configMsg(*this),
      m_fsmaction(*this),
      m_rtcInterval(0, 100000), m_rtcHeartbeat(false),
      m_rtcHblistenerid(NULL),
      m_ecInterval(0, 100000), m_ecHeartbeat(false),
      m_ecHblistenerid(NULL),
      m_timer(m_rtcInterval)
  {
    for (size_t i(0); i < RTC::STATUS_KIND_NUM; ++i)
      {
        m_observed[i] = false;
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
    unsetComponentProfileListeners();
    unsetComponentStatusListeners();
    unsetPortProfileListeners();
    unsetExecutionContextListeners();
    unsetConfigurationListeners();
    unsetRTCHeartbeat();
    unsetECHeartbeat();
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
    setRTCHeartbeat(prop);
    setECHeartbeat(prop);
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
        CorbaConsumer<RTC::ComponentObserver> tmp;
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
    setRTCHeartbeat(prop);
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
    unsetRTCHeartbeat();
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

    coil::vstring observed(coil::split(prop["observed_status"], ","));
    bool flags[RTC::STATUS_KIND_NUM];
    for (int i(0); i < RTC::STATUS_KIND_NUM; ++i)
      {
        flags[i] = false;
      }
    for(auto & o : observed)
      {
        coil::toUpper(o);
        if (o == "COMPONENT_PROFILE")
          {
            flags[RTC::COMPONENT_PROFILE] = true;
          }
        else if (o == "RTC_STATUS")
          {
            flags[RTC::RTC_STATUS] = true;
          }
        else if (o == "EC_STATUS")
          {
            flags[RTC::EC_STATUS] = true;
          }
        else if (o == "PORT_PROFILE")
          {
            flags[RTC::PORT_PROFILE] = true;
          }
        else if (o == "CONFIGURATION")
          {
            flags[RTC::CONFIGURATION] = true;
          }
        else if (o == "FSM_PROFILE")
          {
            flags[RTC::FSM_PROFILE] = true;
          }
        else if (o == "FSM_STATUS")
          {
            flags[RTC::FSM_STATUS] = true;
          }
        else if (o == "FSM_STRUCTURE")
          {
            flags[RTC::FSM_STRUCTURE] = true;
          }
        else if (o == "ALL")
          {
            for (int j(0); j < RTC::STATUS_KIND_NUM; ++j)
              {
                flags[j] = true;
              }
            break;
          }
      }
  
    switchListeners(flags[RTC::COMPONENT_PROFILE],
                    m_observed[RTC::COMPONENT_PROFILE],
                    &ComponentObserverConsumer::setComponentProfileListeners,
                    &ComponentObserverConsumer::unsetComponentProfileListeners);
    switchListeners(flags[RTC::RTC_STATUS],
                    m_observed[RTC::RTC_STATUS],
                    &ComponentObserverConsumer::setComponentStatusListeners,
                    &ComponentObserverConsumer::unsetComponentStatusListeners);
    switchListeners(flags[RTC::EC_STATUS],
                    m_observed[RTC::EC_STATUS],
                    &ComponentObserverConsumer::setExecutionContextListeners,
                    &ComponentObserverConsumer::unsetExecutionContextListeners);
    switchListeners(flags[RTC::PORT_PROFILE],
                    m_observed[RTC::PORT_PROFILE],
                    &ComponentObserverConsumer::setPortProfileListeners,
                    &ComponentObserverConsumer::unsetPortProfileListeners);
    switchListeners(flags[RTC::CONFIGURATION],
                    m_observed[RTC::CONFIGURATION],
                    &ComponentObserverConsumer::setConfigurationListeners,
                    &ComponentObserverConsumer::unsetConfigurationListeners);
    switchListeners(flags[RTC::FSM_PROFILE],
                    m_observed[RTC::FSM_PROFILE],
                    &ComponentObserverConsumer::setFSMProfileListeners,
                    &ComponentObserverConsumer::unsetFSMProfileListeners);
    switchListeners(flags[RTC::FSM_STATUS],
                    m_observed[RTC::FSM_STATUS],
                    &ComponentObserverConsumer::setFSMStatusListeners,
                    &ComponentObserverConsumer::unsetFSMStatusListeners);
    switchListeners(flags[RTC::FSM_STRUCTURE],
                    m_observed[RTC::FSM_STRUCTURE],
                    &ComponentObserverConsumer::setFSMStructureListeners,
                    &ComponentObserverConsumer::unsetFSMStructureListeners);

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
  // RTC Heartbeat related functions

  /*!
   * @if jp
   * @brief ハートビートをオブザーバに伝える
   * @else
   * @brief Sending a heartbeart signal to observer
   * @endif
   */
  void ComponentObserverConsumer::rtcHeartbeat()
  {
    updateStatus(RTC::RTC_HEARTBEAT, "");
  }

  /*!
   * @if jp
   * @brief ハートビートを設定する
   * @else
   * @brief Setting heartbeat
   * @endif
   */
  void ComponentObserverConsumer::setRTCHeartbeat(coil::Properties& prop)
  {
    // if rtc_heartbeat is set, use it.
    if (prop.hasKey("rtc_heartbeat.enable") != NULL)
      {
        prop["heartbeat.enable"] = prop["rtc_heartbeat.enable"];
      }
    if (prop.hasKey("rtc_heartbeat.interval") != NULL)
      {
        prop["heartbeat.interval"] = prop["rtc_heartbeat.interval"];
      }
    if (coil::toBool(prop["heartbeat.enable"], "YES", "NO", false))
      {
        std::string interval(prop["heartbeat.interval"]);
        if (interval.empty())
          {
            m_rtcInterval = 1.0;
          }
        else
          {
            double tmp;
            coil::stringTo(tmp, interval.c_str());
            m_rtcInterval = tmp;
          }
        coil::TimeValue tm;
        tm = m_rtcInterval;
        m_rtcHblistenerid = m_timer.
          registerListenerObj(this, &ComponentObserverConsumer::rtcHeartbeat,
                              tm);
        m_timer.start();
      }
    else
      {
        if (m_rtcHeartbeat == true && m_rtcHblistenerid != 0)
          {
            unsetRTCHeartbeat();
            m_timer.stop();
          }
      }
  }

  /*!
   * @if jp
   * @brief ハートビートを解除する
   * @else
   * @brief Unsetting heartbeat
   * @endif
   */
  void ComponentObserverConsumer::unsetRTCHeartbeat()
  {
    m_timer.unregisterListener(m_rtcHblistenerid);
    m_rtcHeartbeat = false;
    m_rtcHblistenerid = 0;
    m_timer.stop();
  }


  //============================================================
  // EC Heartbeat related functions

  /*!
   * @if jp
   * @brief ハートビートをオブザーバに伝える
   * @else
   * @brief Sending a heartbeart signal to observer
   * @endif
   */
  void ComponentObserverConsumer::ecHeartbeat()
  {
    updateStatus(RTC::EC_HEARTBEAT, "");
  }

  /*!
   * @if jp
   * @brief ハートビートを設定する
   * @else
   * @brief Setting heartbeat
   * @endif
   */
  void ComponentObserverConsumer::setECHeartbeat(coil::Properties& prop)
  {
    // if rtc_heartbeat is set, use it.
    if (coil::toBool(prop["ec_heartbeat.enable"], "YES", "NO", false))
      {
        std::string interval(prop["ec_heartbeat.interval"]);
        if (interval.empty())
          {
            m_ecInterval = 1.0;
          }
        else
          {
            double tmp;
            coil::stringTo(tmp, interval.c_str());
            m_ecInterval = tmp;
          }
        coil::TimeValue tm;
        tm = m_rtcInterval;
        m_ecHblistenerid = m_timer.
          registerListenerObj(this, &ComponentObserverConsumer::ecHeartbeat,
                              tm);
        m_timer.start();
      }
    else
      {
        if (m_ecHeartbeat == true && m_ecHblistenerid != 0)
          {
            unsetECHeartbeat();
            m_timer.stop();
          }
      }
  }

  /*!
   * @if jp
   * @brief ハートビートを解除する
   * @else
   * @brief Unsetting heartbeat
   * @endif
   */
  void ComponentObserverConsumer::unsetECHeartbeat()
  {
    m_timer.unregisterListener(m_ecHblistenerid);
    m_ecHeartbeat = false;
    m_ecHblistenerid = 0;
    m_timer.stop();
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
    if (m_compstat.activatedListener == NULL)
      {
        m_compstat.activatedListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_ACTIVATED,
                                                  m_compstat,
                                                  &CompStatMsg::onActivated);
      }
    if (m_compstat.deactivatedListener == NULL)
      {
        m_compstat.deactivatedListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_DEACTIVATED,
                                                  m_compstat,
                                                  &CompStatMsg::onDeactivated);
      }
    if (m_compstat.resetListener == NULL)
      {
        m_compstat.resetListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_RESET,
                                                  m_compstat,
                                                  &CompStatMsg::onReset);
      }
    if (m_compstat.abortingListener == NULL)
      {
        m_compstat.abortingListener = 
          m_rtobj->addPostComponentActionListener(POST_ON_ABORTING,
                                                  m_compstat,
                                                  &CompStatMsg::onAborting);
      }
    if (m_compstat.finalizeListener == NULL)
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
    if (m_compstat.activatedListener != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_ACTIVATED,
                                                 m_compstat.activatedListener);
        m_compstat.activatedListener = NULL;
      }
    if (m_compstat.deactivatedListener != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_DEACTIVATED,
                                               m_compstat.deactivatedListener);
        m_compstat.deactivatedListener = NULL;
      }
    if (m_compstat.resetListener != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_RESET,
                                                   m_compstat.resetListener);
        m_compstat.resetListener = NULL;
      }
    if (m_compstat.abortingListener != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_ABORTING,
                                                   m_compstat.abortingListener);
        m_compstat.abortingListener = NULL;
      }
    if (m_compstat.finalizeListener != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_FINALIZE,
                                                   m_compstat.finalizeListener);
        m_compstat.finalizeListener = NULL;
      }
  }

  /*!
   * @if jp
   * @brief RTC状態変化リスナの設定処理
   * @else
   * @brief Setting RTC status listeners
   * @endif
   */
  void ComponentObserverConsumer::setFSMStatusListeners()
  {
    // TODO: here should be inmplemented after FSM API defined.
  }
  
  /*!
   * @if jp
   * @brief RTC状態変化リスナの解除処理
   * @else
   * @brief Unsetting RTC status listeners
   * @endif
   */
  void ComponentObserverConsumer::unsetFSMStatusListeners()
  {
    // TODO: here should be inmplemented after FSM API defined.
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
    if (m_portaction.portAddListener == NULL)
      {
        m_portaction.portAddListener =
          m_rtobj->addPortActionListener(ADD_PORT,
                                         m_portaction,
                                         &PortAction::onAddPort);
      }
    if (m_portaction.portRemoveListener == NULL)
      {
        m_portaction.portRemoveListener =
          m_rtobj->addPortActionListener(REMOVE_PORT,
                                         m_portaction,
                                         &PortAction::onRemovePort);
      }
    if (m_portaction.portConnectListener == NULL)
      {
        m_portaction.portConnectListener =
          m_rtobj->addPortConnectRetListener(ON_CONNECTED,
                                             m_portaction,
                                             &PortAction::onConnect);
      }
    if (m_portaction.portDisconnectListener == NULL)
      {
        m_portaction.portDisconnectListener =
          m_rtobj->addPortConnectRetListener(ON_DISCONNECTED,
                                             m_portaction,
                                             &PortAction::onDisconnect);
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
    if (m_portaction.portAddListener != NULL)
      {
        m_rtobj->removePortActionListener(ADD_PORT,
                                          m_portaction.portAddListener);
        m_portaction.portAddListener = NULL;
      }
    if (m_portaction.portRemoveListener != NULL)
      {
        m_rtobj->removePortActionListener(REMOVE_PORT,
                                          m_portaction.portRemoveListener);
        m_portaction.portRemoveListener = NULL;
      }
    if (m_portaction.portConnectListener != NULL)
      {
        m_rtobj->removePortConnectRetListener(ON_CONNECTED,
                                              m_portaction.portConnectListener);
        m_portaction.portConnectListener = NULL;
      }
    if (m_portaction.portDisconnectListener != NULL)
      {
        m_rtobj->removePortConnectRetListener(ON_DISCONNECTED,
                                           m_portaction.portDisconnectListener);
        m_portaction.portDisconnectListener = NULL;
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
    if (m_ecaction.ecAttached == NULL)
      {
        m_ecaction.ecAttached =
          m_rtobj->addExecutionContextActionListener(EC_ATTACHED,
                                                     m_ecaction,
                                                     &ECAction::onAttached);
      }
    if (m_ecaction.ecDetached == NULL)
      {
        m_ecaction.ecDetached = 
          m_rtobj->addExecutionContextActionListener(EC_DETACHED,
                                                     m_ecaction,
                                                     &ECAction::onDetached);
      }
    if (m_ecaction.ecRatechanged == NULL)
      {
        m_ecaction.ecRatechanged = 
          m_rtobj->addPostComponentActionListener(POST_ON_RATE_CHANGED,
                                                  m_ecaction,
                                                  &ECAction::onRateChanged);
      }
    if (m_ecaction.ecStartup == NULL)
      {
        m_ecaction.ecStartup = 
          m_rtobj->addPostComponentActionListener(POST_ON_STARTUP,
                                                  m_ecaction,
                                                  &ECAction::onStartup);
      }
    if (m_ecaction.ecShutdown == NULL)
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
    if (m_ecaction.ecAttached != NULL)
      {
        m_rtobj->removeExecutionContextActionListener(EC_ATTACHED,
                                                      m_ecaction.ecAttached);
      }
    if (m_ecaction.ecDetached != NULL)
      {
        m_rtobj->removeExecutionContextActionListener(EC_DETACHED,
                                                      m_ecaction.ecDetached);
      }
    if (m_ecaction.ecRatechanged != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_RATE_CHANGED,
                                                   m_ecaction.ecRatechanged);
      }
    if (m_ecaction.ecStartup != NULL)
      {
        m_rtobj->removePostComponentActionListener(POST_ON_STARTUP,
                                                   m_ecaction.ecStartup);
      }
    if (m_ecaction.ecShutdown != NULL)
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
  // FsmProfile related functions
  /*!
   * @if jp
   * @brief FsmProfile状態変化リスナの設定
   * @else
   * @brief Setting FsmProfile listener
   * @endif
   */
  void ComponentObserverConsumer::setFSMProfileListeners()
  {
  }

  /*!
   * @if jp
   * @brief FsmProfile状態変化リスナの解除
   * @else
   * @brief Unsetting FsmProfile listener
   * @endif
   */
  void ComponentObserverConsumer::unsetFSMProfileListeners()
  {
  }

  //============================================================
  // FsmStructure related functions
  /*!
   * @if jp
   * @brief FsmStructure状態変化リスナの設定
   * @else
   * @brief Setting FsmStructure listener
   * @endif
   */
  void ComponentObserverConsumer::setFSMStructureListeners()
  {
    m_fsmaction.preOnFsmInitListener =
      m_rtobj->addPreFsmActionListener(PRE_ON_INIT,
                                       m_fsmaction,
                                       &FSMAction::preInit);
    m_fsmaction.preOnFsmEntryListener =
      m_rtobj->addPreFsmActionListener(PRE_ON_ENTRY,
                                       m_fsmaction,
                                       &FSMAction::preEntry);
    m_fsmaction.preOnFsmDoListener =
      m_rtobj->addPreFsmActionListener(PRE_ON_DO,
                                       m_fsmaction,
                                       &FSMAction::preDo);
    m_fsmaction.preOnFsmExitListener =
      m_rtobj->addPreFsmActionListener(PRE_ON_EXIT,
                                       m_fsmaction,
                                       &FSMAction::preExit);
    m_fsmaction.preOnFsmStateChangeListener =
      m_rtobj->addPreFsmActionListener(PRE_ON_STATE_CHANGE,
                                       m_fsmaction,
                                       &FSMAction::preStateChange);

    m_fsmaction.postOnFsmInitListener =
      m_rtobj->addPostFsmActionListener(POST_ON_INIT,
                                       m_fsmaction,
                                       &FSMAction::postInit);
    m_fsmaction.postOnFsmEntryListener =
      m_rtobj->addPostFsmActionListener(POST_ON_ENTRY,
                                       m_fsmaction,
                                       &FSMAction::postEntry);
    m_fsmaction.postOnFsmDoListener =
      m_rtobj->addPostFsmActionListener(POST_ON_DO,
                                       m_fsmaction,
                                       &FSMAction::postDo);
    m_fsmaction.postOnFsmExitListener =
      m_rtobj->addPostFsmActionListener(POST_ON_EXIT,
                                        m_fsmaction,
                                        &FSMAction::postExit);
    m_fsmaction.postOnFsmStateChangeListener =
      m_rtobj->addPostFsmActionListener(POST_ON_STATE_CHANGE,
                                        m_fsmaction,
                                        &FSMAction::postStateChange);
  }

  /*!
   * @if jp
   * @brief FsmStructure状態変化リスナの解除
   * @else
   * @brief Unsetting FsmStructure listener
   * @endif
   */
  void ComponentObserverConsumer::unsetFSMStructureListeners()
  {
      m_rtobj->
        removePreFsmActionListener(PRE_ON_INIT,
                                   m_fsmaction.preOnFsmInitListener);
      m_rtobj->
        removePreFsmActionListener(PRE_ON_ENTRY,
                                   m_fsmaction.preOnFsmEntryListener);
      m_rtobj->
        removePreFsmActionListener(PRE_ON_DO,
                                   m_fsmaction.preOnFsmDoListener);
      m_rtobj->
        removePreFsmActionListener(PRE_ON_EXIT,
                                   m_fsmaction.preOnFsmExitListener);
      m_rtobj->
        removePreFsmActionListener(PRE_ON_STATE_CHANGE,
                                   m_fsmaction.preOnFsmStateChangeListener);
      m_rtobj->
        removePostFsmActionListener(POST_ON_INIT,
                                    m_fsmaction.postOnFsmInitListener);
      m_rtobj->
        removePostFsmActionListener(POST_ON_ENTRY,
                                    m_fsmaction.postOnFsmEntryListener);
      m_rtobj->
        removePostFsmActionListener(POST_ON_DO,
                                    m_fsmaction.postOnFsmDoListener);
      m_rtobj->
        removePostFsmActionListener(POST_ON_EXIT,
                                    m_fsmaction.postOnFsmExitListener);
      m_rtobj->
        removePostFsmActionListener(POST_ON_EXIT,
                                    m_fsmaction.postOnFsmStateChangeListener);
  }

  //============================================================
  // Configuration

  void ComponentObserverConsumer::setConfigurationListeners()
  {
    m_configMsg.updateConfigParamListener = 
      m_rtobj->addConfigurationParamListener(ON_UPDATE_CONFIG_PARAM,
                                             m_configMsg,
                                             &ConfigAction::updateConfigParam);
    m_configMsg.setConfigSetListener = 
      m_rtobj->addConfigurationSetListener(ON_SET_CONFIG_SET,
                                             m_configMsg,
                                             &ConfigAction::setConfigSet);
    m_configMsg.addConfigSetListener = 
      m_rtobj->addConfigurationSetListener(ON_ADD_CONFIG_SET,
                                             m_configMsg,
                                             &ConfigAction::addConfigSet);
    m_configMsg.updateConfigSetListener = 
      m_rtobj->addConfigurationSetNameListener(ON_UPDATE_CONFIG_SET,
                                               m_configMsg,
                                               &ConfigAction::updateConfigSet);
    m_configMsg.removeConfigSetListener = 
      m_rtobj->addConfigurationSetNameListener(ON_REMOVE_CONFIG_SET,
                                               m_configMsg,
                                               &ConfigAction::removeConfigSet);
    m_configMsg.activateConfigSetListener = 
      m_rtobj->addConfigurationSetNameListener(ON_ACTIVATE_CONFIG_SET,
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

    if (m_configMsg.updateConfigParamListener != NULL)
      {
        m_rtobj->
          removeConfigurationParamListener(ON_UPDATE_CONFIG_PARAM,
                                      m_configMsg.updateConfigParamListener);
        m_configMsg.updateConfigParamListener = NULL;
      }
    if (m_configMsg.setConfigSetListener != NULL)
      {
        m_rtobj->removeConfigurationSetListener(ON_SET_CONFIG_SET,
                                           m_configMsg.setConfigSetListener);
        m_configMsg.setConfigSetListener = NULL;
      }
    if (m_configMsg.addConfigSetListener != NULL)
      {
        m_rtobj->removeConfigurationSetListener(ON_ADD_CONFIG_SET,
                                            m_configMsg.addConfigSetListener);
        m_configMsg.addConfigSetListener = NULL;
      }
    if (m_configMsg.updateConfigSetListener != NULL)
      {
        m_rtobj->removeConfigurationSetNameListener(ON_UPDATE_CONFIG_SET,
                                          m_configMsg.updateConfigSetListener);
        m_configMsg.updateConfigSetListener = NULL;
      }
    if (m_configMsg.removeConfigSetListener != NULL)
      {
        m_rtobj->removeConfigurationSetNameListener(ON_REMOVE_CONFIG_SET,
                                          m_configMsg.removeConfigSetListener);
        m_configMsg.removeConfigSetListener = NULL;
      }
    if (m_configMsg.activateConfigSetListener != NULL)
      {
        m_rtobj->removeConfigurationSetNameListener(ON_ACTIVATE_CONFIG_SET,
                                        m_configMsg.activateConfigSetListener);
        m_configMsg.activateConfigSetListener = NULL;
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
    factory.addFactory(CORBA_Util::toRepositoryId<RTC::ComponentObserver>(),
#else
    factory.addFactory(CORBA_Util::
                       toRepositoryIdOfObject<RTC::ComponentObserver>(),
#endif
                       ::coil::Creator< ::RTC::SdoServiceConsumerBase,
                       ::RTC::ComponentObserverConsumer>,
                       ::coil::Destructor< ::RTC::SdoServiceConsumerBase,
                       ::RTC::ComponentObserverConsumer>);
                       std::cout << "Init()" << std::endl;
  }
};
