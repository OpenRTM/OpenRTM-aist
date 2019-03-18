// -*- C++ -*-
/*!
 * @file RTPreemptEC.cpp
 * @brief RTPreemptEC class
 * @date $Date: 2008-01-14 07:53:01 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008,2012
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: RTPreemptEC.cpp 2307 2012-02-05 21:29:15Z n-ando $
 *
 */

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sched.h>
#include <sys/mman.h>

#include <algorithm>
#include <iostream>

#include <coil/Time.h>
#include <coil/TimeValue.h>

#include <rtm/RTObjectStateMachine.h>
#include "RTPreemptEC.h"

#define MAX_SAFE_STACK (8*1024)
#define NSEC_PER_SEC 1000000000
#define DEEFAULT_PERIOD 0.000001

namespace RTC_exp
{
  /*!
   * @if jp
   * @brief デフォルトコンストラクタ
   * @else
   * @brief Default constructor
   * @endif
   */
  RTPreemptEC::
  RTPreemptEC()
    : ExecutionContextBase("periodic_ec"),
      rtclog("periodic_ec"),
      m_svc(false), m_nowait(false)
  {
    RTC_TRACE(("RTPreemptEC()"));

    // getting my reference
    setObjRef(this->_this());

    // profile initialization
    setKind(RTC::PERIODIC);
    setRate(1.0 / DEEFAULT_PERIOD);

    RTC_DEBUG(("Actual period: %d [sec], %d [usec]",
               m_profile.getPeriod().sec(), m_profile.getPeriod().usec()));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  RTPreemptEC::~RTPreemptEC()
  {
    RTC_TRACE(("~RTPreemptEC()"));
    {
      Guard guard(m_svcmutex);
      m_svc = false;
    }
    {
      Guard guard(m_workerthread.mutex_);
      m_workerthread.running_ = true;
      m_workerthread.cond_.signal();
    }
    wait();
  }

  void RTPreemptEC::init(coil::Properties& props)
  {
    RTC_TRACE(("init()"));
    ExecutionContextBase::init(props);

    setPriority(props);
    setPolicy(props);
    setWaitOffset(props);

    RTC_DEBUG(("init() done"));
  }


  /*------------------------------------------------------------
   * Start activity
   * ACE_Task class method over ride.
   *------------------------------------------------------------*/
  /*!
   * @if jp
   * @brief ExecutionContext用アクティビティスレッドを生成する
   * @else
   * @brief Generate internal activity thread for ExecutionContext
   * @endif
   */
  int RTPreemptEC::open(void * /*args*/)
  {
    RTC_TRACE(("open()"));
    activate();
    return 0;
  }

  /*------------------------------------------------------------
   * Run by a daemon thread to handle deferred processing
   * ACE_Task class method over ride.
   *------------------------------------------------------------*/
  /*!
   * @if jp
   * @brief ExecutionContext 用のスレッド実行関数
   * @else
   * @brief Thread execution function for ExecutionContext
   * @endif
   */
  bool RTPreemptEC::prepareThread()
  {
    struct sched_param param;
    param.sched_priority = m_priority;
    if(sched_setscheduler(0, m_policy, &param) == -1)
      {
        std::cerr << "sched_setscheduler failed" << std::endl;
        return false;
      }

    // memory locking
    if(mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
      {
        std::cerr << "mlockall failed" << std::endl;
        return false;
      }
    return true;
  }
  int RTPreemptEC::svc()
  {
    RTC_TRACE(("svc()"));
    if (!prepareThread()) { return -1; }

    // stack preallocation
    unsigned char dummy[MAX_SAFE_STACK];
    memset(&dummy, 0, MAX_SAFE_STACK);

    struct timespec ts0, ts1, ts2, ts3;
    int count(0);
    do
      {
        ExecutionContextBase::invokeWorkerPreDo();
        {
          Guard guard(m_workerthread.mutex_);
          while (!m_workerthread.running_)
            {
              m_workerthread.cond_.wait();
            }
        }
        clock_gettime(CLOCK_MONOTONIC ,&ts0);
        coil::TimeValue t0(ts0.tv_sec, ts0.tv_nsec * 1000);
        ExecutionContextBase::invokeWorkerDo();
        ExecutionContextBase::invokeWorkerPostDo();
        clock_gettime(CLOCK_MONOTONIC ,&ts1);
        coil::TimeValue t1(ts1.tv_sec, ts1.tv_nsec * 1000);

        coil::TimeValue period(getPeriod());
        if (count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", (double)period));
            RTC_PARANOID(("Execution: %f [s]", (double)(t1 - t0)));
            RTC_PARANOID(("Sleep:     %f [s]", (double)(period - (t1 - t0))));
          }
        clock_gettime(CLOCK_MONOTONIC ,&ts2);
        coil::TimeValue t2(ts2.tv_sec, ts2.tv_nsec * 1000);
        if (m_nowait) { ++count; continue; }

        struct timespec sleeptime;
        if (getSleepTime(sleeptime, ts0, ts1))
          {
            clock_nanosleep(CLOCK_MONOTONIC, static_cast<int>(!TIMER_ABSTIME),
                            &sleeptime, nullptr);
            if (count > 1000)
              {
                clock_gettime(CLOCK_MONOTONIC ,&ts3);
                coil::TimeValue t3(ts3.tv_sec, ts3.tv_nsec * 1000);
                RTC_PARANOID(("Slept:     %f [s]", (double)(t3 - t2)));
                count = 0;
              }
          }
        ++count;
      } while (threadRunning());
    RTC_DEBUG(("Thread terminated."));
    return 0;
  }

  /*!
   * @if jp
   * @brief ExecutionContext 用のスレッド実行関数
   * @else
   * @brief Thread execution function for ExecutionContext
   * @endif
   */
  int RTPreemptEC::close(unsigned long  /*flags*/)
  {
    RTC_TRACE(("close()"));
    // At this point, this component have to be finished.
    // Current state and Next state should be RTC_EXITING.
    return 0;
  }


  //============================================================
  // ExecutionContext CORBA operations
  //============================================================
  /*!
   * @if jp
   * @brief ExecutionContext 実行状態確認関数
   * @else
   * @brief Check for ExecutionContext running state
   * @endif
   */
  CORBA::Boolean RTPreemptEC::is_running()
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::isRunning();
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行を開始
   * @else
   * @brief Start the ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::start()
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::start();
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行を停止
   * @else
   * @brief Stop the ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::stop()
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::stop();
  }



  /*!
   * @if jp
   * @brief ExecutionContext の実行周期(Hz)を取得する
   * @else
   * @brief Get execution rate(Hz) of ExecutionContext
   * @endif
   */
  CORBA::Double RTPreemptEC::get_rate()
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::getRate();
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行周期(Hz)を設定する
   * @else
   * @brief Set execution rate(Hz) of ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::set_rate(CORBA::Double rate)
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::setRate(rate);
  }

  /*!
   * @if jp
   * @brief RTコンポーネントを追加する
   * @else
   * @brief Add an RT-Component
   * @endif
   */
  RTC::ReturnCode_t
  RTPreemptEC::add_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::addComponent(comp);
  }

  /*!
   * @if jp
   * @brief コンポーネントをコンポーネントリストから削除する
   * @else
   * @brief Remove the RT-Component from participant list
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::
  remove_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::removeComponent(comp);
  }

  /*!
   * @if jp
   * @brief RTコンポーネントをアクティブ化する
   * @else
   * @brief Activate an RT-Component
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::
  activate_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::activateComponent(comp);
  }

  /*!
   * @if jp
   * @brief RTコンポーネントを非アクティブ化する
   * @else
   * @brief Deactivate an RT-Component
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::
  deactivate_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::deactivateComponent(comp);
  }

  /*!
   * @if jp
   * @brief RTコンポーネントをリセットする
   * @else
   * @brief Reset the RT-Component
   * @endif
   */
  RTC::ReturnCode_t RTPreemptEC::
  reset_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::resetComponent(comp);
  }

  /*!
   * @if jp
   * @brief RTコンポーネントの状態を取得する
   * @else
   * @brief Get RT-Component's state
   * @endif
   */
  RTC::LifeCycleState RTPreemptEC::
  get_component_state(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
    RTC::LifeCycleState ret = ExecutionContextBase::getComponentState(comp);
    return ret;
  }

  /*!
   * @if jp
   * @brief ExecutionKind を取得する
   * @else
   * @brief Get the ExecutionKind
   * @endif
   */
  RTC::ExecutionKind RTPreemptEC::get_kind()
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::getKind();
  }

  //------------------------------------------------------------
  // ExecutionContextService interfaces
  //------------------------------------------------------------
  /*!
   * @if jp
   * @brief ExecutionContextProfile を取得する
   * @else
   * @brief Get the ExecutionContextProfile
   * @endif
   */
  RTC::ExecutionContextProfile* RTPreemptEC::get_profile()
    throw (CORBA::SystemException)
  {
    return ExecutionContextBase::getProfile();
  }


  //============================================================
  // protected functions
  //============================================================
  /*!
   * @brief onStarted() template function
   */
  RTC::ReturnCode_t RTPreemptEC::onStarted()
  {
    // change EC thread state
    {
      Guard guard(m_svcmutex);
      if (!m_svc)
        {
          m_svc = true;
          this->open(nullptr);
        }
    }
    if (isAllNextState(RTC::INACTIVE_STATE))
      {
        Guard guard(m_workerthread.mutex_);
        m_workerthread.running_ = false;
      }
    else
      {
        Guard guard(m_workerthread.mutex_);
        m_workerthread.running_ = true;
        m_workerthread.cond_.signal();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onStopping() template function
   */
  RTC::ReturnCode_t RTPreemptEC::onStopping()
  {
    // stop thread
    Guard guard(m_workerthread.mutex_);
    m_workerthread.running_ = false;
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingActivated() template function
   */
  RTC::ReturnCode_t RTPreemptEC::
  onWaitingActivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingActivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    // Now comp's next state must be ACTIVE state
    // If worker thread is stopped, restart worker thread.
    Guard guard(m_workerthread.mutex_);
    if (!m_workerthread.running_)
      {
        m_workerthread.running_ = true;
        m_workerthread.cond_.signal();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onActivated() template function
   */
  RTC::ReturnCode_t RTPreemptEC::
  onActivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onActivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    // count = -1; Asynch mode. Since onWaitingActivated is not
    // called, onActivated() have to send restart singnal to worker
    // thread.
    // count > 0: Synch mode.

    // Now comp's next state must be ACTIVE state
    // If worker thread is stopped, restart worker thread.
    Guard guard(m_workerthread.mutex_);
    if (!m_workerthread.running_)
      {
        m_workerthread.running_ = true;
        m_workerthread.cond_.signal();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingDeactivated() template function
   */
  RTC::ReturnCode_t RTPreemptEC::
  onWaitingDeactivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingDeactivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    if (isAllNextState(RTC::INACTIVE_STATE))
      {
        Guard guard(m_workerthread.mutex_);
        if (m_workerthread.running_)
          {
            m_workerthread.running_ = false;
            RTC_TRACE(("All RTCs are INACTIVE. Stopping worker thread."));
          }
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onDeactivated() template function
   */
  RTC::ReturnCode_t RTPreemptEC::
  onDeactivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onDeactivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    if (isAllNextState(RTC::INACTIVE_STATE))
      {
        Guard guard(m_workerthread.mutex_);
        if (m_workerthread.running_)
          {
            m_workerthread.running_ = false;
            RTC_TRACE(("All RTCs are INACTIVE. Stopping worker thread."));
          }
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingReset() template function
   */
  RTC::ReturnCode_t RTPreemptEC::
  onWaitingReset(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingReset(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    if (isAllNextState(RTC::INACTIVE_STATE))
      {
        Guard guard(m_workerthread.mutex_);
        if (m_workerthread.running_)
          {
            m_workerthread.running_ = false;
            RTC_TRACE(("All RTCs are INACTIVE. Stopping worker thread."));
          }
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onReset() template function
   */
  RTC::ReturnCode_t RTPreemptEC::
  onReset(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onReset(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    if (isAllNextState(RTC::INACTIVE_STATE))
      {
        Guard guard(m_workerthread.mutex_);
        if (m_workerthread.running_)
          {
            m_workerthread.running_ = false;
            RTC_TRACE(("All RTCs are INACTIVE. Stopping worker thread."));
          }
      }
    return RTC::RTC_OK;
  }

  //============================================================
  // private functions
  //============================================================
  void RTPreemptEC::
  setPriority(coil::Properties& prop)
  {
    RTC_TRACE(("setPriority()"));
    getProperty(prop, "priority", m_priority);


    RTC_DEBUG(("setPriority(): priority: %d", m_priority));
  }
  void RTPreemptEC::
  setPolicy(coil::Properties& prop)
  {
    RTC_TRACE(("setPolicy()"));
    std::string policy;
    getProperty(prop, "policy", policy);
    if (!policy.empty())
      {
        coil::normalize(policy);
        if (policy == "rr")   { m_policy = SCHED_RR; }
        if (policy == "fifo") { m_policy = SCHED_FIFO; }
        RTC_DEBUG(("Scheduling policy: %s", policy.c_str()));
      }
    else
      {
        RTC_DEBUG(("Scheduling policy: fifo"));
        m_policy = SCHED_FIFO;
      }
    RTC_DEBUG(("setPolicy(): policy: %s", policy.c_str()));
  }
  void RTPreemptEC::
  setWaitOffset(coil::Properties& prop)
  {
    RTC_TRACE(("setWaitOffset()"));
    getProperty(prop, "wait_offset", m_waitoffset);
    RTC_DEBUG(("setWaitOffset(): offset: %d", m_waitoffset));
  }

  bool RTPreemptEC::getSleepTime(struct timespec& ts,
                                 const struct timespec& t0,
                                 const struct timespec& t1)
  {
    coil::TimeValue period(getPeriod());
    if (t0.tv_nsec > t1.tv_nsec)
      {
        ts.tv_nsec = period.usec() * 1000
          - (NSEC_PER_SEC - t0.tv_nsec + t1.tv_nsec) + m_waitoffset;
        ts.tv_sec  = period.sec() - (t1.tv_sec - 1 - t0.tv_sec);
      }
    else
      {
        ts.tv_nsec = period.usec() * 1000
          - (t1.tv_nsec - t0.tv_nsec) + m_waitoffset;
        ts.tv_sec  = period.sec() - (t1.tv_sec - t0.tv_sec);
      }
    if (ts.tv_nsec < 0 || ts.tv_sec < 0)
      {
        std::cerr << "faital error: deadline passed. " << std::endl;
        std::cerr << "Wait time: ";
        std::cerr << ts.tv_sec << "[s], ";
        std::cerr << ts.tv_nsec << "[ns]" << std::endl;
        std::cerr << "Next wait time force to: 0.0 [s]"
                  << std::endl;
        return false; // sleeptime < 0
      }
    return true; // sleeptime >= 0
  }
} // namespace RTC_exp

extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */

 void RTPreemptECInit(RTC::Manager*  /*manager*/)
  {
    RTC::ExecutionContextFactory::
      instance().addFactory("RTPreemptEC",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC_exp::RTPreemptEC>,
                            ::coil::Destructor< ::RTC::ExecutionContextBase,
                            ::RTC_exp::RTPreemptEC>);
    RTC::ExecutionContextFactory::
      instance().addFactory("rtpreempt_ec",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC_exp::RTPreemptEC>,
                            ::coil::Destructor< ::RTC::ExecutionContextBase,
                            ::RTC_exp::RTPreemptEC>);
  }
};

