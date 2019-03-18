// -*- C++ -*-
/*!
 * @file ExtendedFsmServiceProvider.h
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
 * $Id$
 *
 */

#include <coil/stringutil.h>
#include <rtm/Typename.h>
#include <rtm/idl/ExtendedFsmServiceStub.h>
#include "ExtendedFsmServiceProvider.h"
#include <iostream>

namespace RTC
{
  /*!
   * @if jp
   * @brief ctor of ExtendedFsmServiceProvider
   * @else
   * @brief ctor of ExtendedFsmServiceProvider
   * @endif
   */
  ExtendedFsmServiceProvider::ExtendedFsmServiceProvider()
    : m_rtobj(nullptr)
  {
    std::cout << "ExtendedFsmServiceProvider()" << std::endl;

    // dummy code
    m_fsmStructure.name = CORBA::string_dup("dummy_name");
    m_fsmStructure.structure = CORBA::string_dup(
    "<scxml xmlns=\"http://www.w3.org/2005/07/scxml\""
    "           version=\"1.0\""
    "           initial=\"airline-ticket\">"
    "  <state id=\"state0\">"
    "    <datamodel>"
    "      <data id=\"data0\">"
    "      </data>"
    "    </datamodel>"
    "    <transition event=\"toggle\" target=\"state1\" />"
    "  </state>"
    "  <state id=\"state1\">"
    "    <datamodel>"
    "      <data id=\"data1\">"
    "      </data>"
    "    </datamodel>"
    "    <transition event=\"toggle\" target=\"state0\" />"
    "  </state>"
    " </scxml>"
    );
    m_fsmStructure.event_profiles.length(1);
    FsmEventProfile event0;
    event0.name = CORBA::string_dup("toggle");
    event0.data_type = CORBA::string_dup("TimedShort");
    m_fsmStructure.event_profiles[0] = event0;
    NVUtil::appendStringValue(m_fsmStructure.properties,
                              "fsm_structure.format",
                              "scxml");
    std::cout << m_fsmStructure.structure << std::endl;
  }

  /*!
   * @if jp
   * @brief dtor
   * @else
   * @brief dtor
   * @endif
   */
  ExtendedFsmServiceProvider::~ExtendedFsmServiceProvider()
  {
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief Initialization
   * @endif
   */
  bool
  ExtendedFsmServiceProvider::init(RTObject_impl& rtobj,
                                   const SDOPackage::ServiceProfile& profile)
  {
    m_rtobj = &rtobj;
    m_profile = profile;
    //    coil::Properties prop;
    //    NVUtil::copyToProperties(prop, profile.properties);
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
  ExtendedFsmServiceProvider::reinit(const SDOPackage::ServiceProfile& profile)
  {
    m_profile= profile;
    //    coil::Properties prop;
    //    NVUtil::copyToProperties(prop, profile.properties);
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
  ExtendedFsmServiceProvider::getProfile() const
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
  void ExtendedFsmServiceProvider::finalize()
  {
  }

  //============================================================
  // CORBA operations
  //
  // string get_current_state();
  // ReturnCode_t set_fsm_structure(in FsmStructure fsm_structure);
  // ReturnCode_t get_fsm_structure(out FsmStructure fsm_structure);
  //============================================================

  /*!
   * @if jp
   * @brief FSMの現在の状態を取得
   * @else
   * @brief Get Current FSM State
   * @endif
   */
  char* ExtendedFsmServiceProvider::get_current_state()
  {
    return CORBA::string_dup(m_fsmState.c_str());
  }

  /*!
   * @if jp
   * @brief FSMの構造を設定する
   * @else
   * @brief Set FSM Structure
   * @endif
   */
  ReturnCode_t ExtendedFsmServiceProvider::
  set_fsm_structure(const ::RTC::FsmStructure& fsm_structure)
  {
    m_fsmStructure = fsm_structure; 
    return RTC::RTC_OK;
  }


  /*!
   * @if jp
   * @brief FSMの構造を取得する
   * @else
   * @brief Set FSM Structure
   * @endif
   */
  ReturnCode_t ExtendedFsmServiceProvider::
  get_fsm_structure(::RTC::FsmStructure_out fsm_structure)
  {
    fsm_structure = new FsmStructure(m_fsmStructure);
    return RTC::RTC_OK;
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
  void ExtendedFsmServiceProvider::setListeners(coil::Properties& /*prop*/)
  {
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
  void ExtendedFsmServiceProvider::changeStatus(std::string state)
  {
    m_fsmState = state;
  }

  /*!
   * @if jp
   * @brief ハートビートをオブザーバに伝える
   * @else
   * @brief Sending a heartbeart signal to observer
   * @endif
   */
  void ExtendedFsmServiceProvider::changeStructure(std::string fsm_structure)
  {
    m_fsmStructure.structure = fsm_structure.c_str();
  }

  //============================================================
  // FSM status related functions
  /*!
   * @if jp
   * @brief RTC状態変化リスナの設定処理
   * @else
   * @brief Setting RTC status listeners
   * @endif
   */
  void ExtendedFsmServiceProvider::setFSMStatusListeners()
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
  void ExtendedFsmServiceProvider::unsetFSMStatusListeners()
  {
    // TODO: here should be inmplemented after FSM API defined.
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
  void ExtendedFsmServiceProvider::setFSMProfileListeners()
  {
  }

  /*!
   * @if jp
   * @brief FsmProfile状態変化リスナの解除
   * @else
   * @brief Unsetting FsmProfile listener
   * @endif
   */
  void ExtendedFsmServiceProvider::unsetFSMProfileListeners()
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
  void ExtendedFsmServiceProvider::setFSMStructureListeners()
  {
  }

  /*!
   * @if jp
   * @brief FsmStructure状態変化リスナの解除
   * @else
   * @brief Unsetting FsmStructure listener
   * @endif
   */
  void ExtendedFsmServiceProvider::unsetFSMStructureListeners()
  {
  }

} // namespace RTC

extern "C"
{
  void ExtendedFsmServiceProviderInit()
  {
    RTC::SdoServiceProviderFactory& factory
      = RTC::SdoServiceProviderFactory::instance();
#ifndef ORB_IS_RTORB
    factory.addFactory(CORBA_Util::toRepositoryId<RTC::ExtendedFsmService>(),
#else
    factory.addFactory(CORBA_Util::
                       toRepositoryIdOfObject<RTC::ExtendedFsmService>(),
#endif
                       ::coil::Creator< ::RTC::SdoServiceProviderBase,
                       ::RTC::ExtendedFsmServiceProvider>,
                       ::coil::Destructor< ::RTC::SdoServiceProviderBase,
                       ::RTC::ExtendedFsmServiceProvider>);
                       std::cout << "Init()" << std::endl;
  }
};
