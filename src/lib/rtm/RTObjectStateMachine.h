// -*- C++ -*-
/*!
 * @file RTObjectStateMachine.h
 * @brief ExecutionContext's state machine worker class
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

#ifndef RTC_IMPL_RTOBJECTSTATEMACHINE_H
#define RTC_IMPL_RTOBJECTSTATEMACHINE_H

#include <cstdlib>
#include <rtm/SystemLogger.h>
#include <coil/TimeMeasure.h>
#include <rtm/idl/RTCSkel.h>
#include <rtm/StateMachine.h>
#include <cassert>
#include <iostream>

#define NUM_OF_LIFECYCLESTATE 4
namespace RTC
{
  class RTObject_impl;
} // namespace RTC
namespace RTC_impl
{
  using Ecid = RTC::ExecutionContextHandle_t;
  using ExecContextState = RTC::LifeCycleState;
  using ExecContextStates = RTC_Utils::StateHolder<ExecContextState>;

  class RTObjectStateMachine
  {
  public:
    RTObjectStateMachine(RTC::ExecutionContextHandle_t id,
                         RTC::LightweightRTObject_ptr comp);
    RTObjectStateMachine(RTObjectStateMachine const&) = delete;
    virtual ~RTObjectStateMachine();
    RTObjectStateMachine& operator=(RTObjectStateMachine const&) = delete;

    // functions for stored RTObject reference
    RTC::LightweightRTObject_ptr getRTObject();
    bool isEquivalent(RTC::LightweightRTObject_ptr comp);

    RTC::ExecutionContextHandle_t getExecutionContextHandle();

    // RTC::ComponentAction operations
    void onStartup();
    void onShutdown();
    void onActivated(const ExecContextStates& st);
    void onDeactivated(const ExecContextStates& st);
    void onAborting(const ExecContextStates& st);
    void onError(const ExecContextStates& st);
    void onReset(const ExecContextStates& st);

    // RTC::DataflowComponentAction
    void onExecute(const ExecContextStates& st);
    void onStateUpdate(const ExecContextStates& st);
    RTC::ReturnCode_t onRateChanged();

    // FsmParticipantAction
    void onAction(const ExecContextStates& st);

    // MultiModeComponentAction
    void onModeChanged(const ExecContextStates& st);

    // Getting state of the context
    ExecContextState getState();
    ExecContextStates getStates();
    bool isCurrentState(ExecContextState state);
    bool isNextState(ExecContextState state);
    void goTo(ExecContextState state);

    // Workers
    void workerPreDo();
    void workerDo();
    void workerPostDo();

    bool activate();
    bool deactivate();
    bool reset();

  protected:
    void setComponentAction(RTC::LightweightRTObject_ptr comp);
    void setDataFlowComponentAction(RTC::LightweightRTObject_ptr comp);
    void setFsmParticipantAction(RTC::LightweightRTObject_ptr comp);
    void setMultiModeComponentAction(RTC::LightweightRTObject_ptr comp);

  private:  // member variables
    RTC::Logger rtclog;
    // ExecutionContext id
    RTC::ExecutionContextHandle_t m_id;
    // Associated RTObject reference
    RTC::LightweightRTObject_var m_rtobj;
    // State machine
    RTC_Utils::StateMachine<ExecContextState,
                            RTObjectStateMachine> m_sm;
    bool m_ca;
    bool m_dfc;
    bool m_fsm;
    bool m_mode;
    RTC::ComponentAction_var          m_caVar;
    RTC::DataFlowComponentAction_var  m_dfcVar;
    RTC::FsmParticipantAction_var     m_fsmVar;
    RTC::MultiModeComponentAction_var m_modeVar;
    RTC::RTObject_impl* m_rtobjPtr;
    bool m_measure;
    // Component action invoker
    coil::TimeMeasure m_svtMeasure;
    coil::TimeMeasure m_refMeasure;
  };
} // namespace RTC_impl

#endif  // RTC_RTOBJECTSTATEMACHINE_H
