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

#include <stdlib.h>
#include <rtm/SystemLogger.h>
#include <coil/NonCopyable.h>
#include <coil/TimeMeasure.h>
#include <rtm/idl/RTCSkel.h>
#include <rtm/StateMachine.h>
#include <assert.h>
#include <iostream>

#define NUM_OF_LIFECYCLESTATE 4

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif
namespace RTC
{
  class RTObject_impl;
}
namespace RTC_impl
{
  typedef RTC::ExecutionContextHandle_t Ecid;
  typedef RTC::LifeCycleState ExecContextState;
  typedef RTC_Utils::StateHolder<ExecContextState> ExecContextStates;

  class RTObjectStateMachine
    : private coil::NonCopyable 
  {
  public:
    RTObjectStateMachine(RTC::ExecutionContextHandle_t id,
                         RTC::LightweightRTObject_ptr comp);
    virtual ~RTObjectStateMachine(void);
//    RTObjectStateMachine(const RTObjectStateMachine& rtobjsm);
//    RTObjectStateMachine& operator=(const RTObjectStateMachine& rtobjsm);
//    void swap(RTObjectStateMachine& rtobjsm) throw();

    // functions for stored RTObject reference
    RTC::LightweightRTObject_ptr getRTObject();
    bool isEquivalent(RTC::LightweightRTObject_ptr comp);

    RTC::ExecutionContextHandle_t getExecutionContextHandle();

    // RTC::ComponentAction operations
    void onStartup(void);
    void onShutdown(void);
    void onActivated(const ExecContextStates& st);
    void onDeactivated(const ExecContextStates& st);
    void onAborting(const ExecContextStates& st);
    void onError(const ExecContextStates& st);
    void onReset(const ExecContextStates& st);

    // RTC::DataflowComponentAction
    void onExecute(const ExecContextStates& st);
    void onStateUpdate(const ExecContextStates& st);
    RTC::ReturnCode_t onRateChanged(void);

    // FsmParticipantAction
    void onAction(const ExecContextStates& st);

    // MultiModeComponentAction
    void onModeChanged(const ExecContextStates& st);

    // Getting state of the context
    ExecContextState getState(void);
    ExecContextStates getStates(void);
    bool isCurrentState(ExecContextState state);
    bool isNextState(ExecContextState state);
    void goTo(ExecContextState state);

    // Workers
    void workerPreDo(void);
    void workerDo(void);
    void workerPostDo(void);

    bool activate();
    bool deactivate();
    bool reset();

  protected:
    void setComponentAction(const RTC::LightweightRTObject_ptr comp);
    void setDataFlowComponentAction(const RTC::LightweightRTObject_ptr comp);
    void setFsmParticipantAction(const RTC::LightweightRTObject_ptr comp);
    void setMultiModeComponentAction(const RTC::LightweightRTObject_ptr comp);

  private: // member variables
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

    //    char dara[1000];
    // Component action invoker
    coil::TimeMeasure m_svtMeasure;
    coil::TimeMeasure m_refMeasure;
  };
}; // namespace RTC

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif // RTC_RTOBJECTSTATEMACHINE_H
