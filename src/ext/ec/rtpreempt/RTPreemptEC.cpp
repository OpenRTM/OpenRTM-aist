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


#include <rtm/RTObjectStateMachine.h>
#include "RTPreemptEC.h"

#define MAX_SAFE_STACK (8*1024)
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
    : ExecutionContextBase("periodic_ec")
  {
    RTC_TRACE(("RTPreemptEC()"));

    // getting my reference
    setObjRef(this->_this());

    // profile initialization
    setKind(RTC::PERIODIC);
    setRate(1.0 / DEEFAULT_PERIOD);

    RTC_DEBUG(("Actual period: %lld [nsec]", m_profile.getPeriod().count()));
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
      std::lock_guard<std::mutex> guard(m_svcmutex);
      m_svc = false;
    }
    {
      std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
      m_workerthread.running_ = true;
      m_workerthread.cond_.notify_one();
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

    int count(0);
    do
      {
        ExecutionContextBase::invokeWorkerPreDo();
        {
          std::unique_lock<std::mutex> guard(m_workerthread.mutex_);
          while (!m_workerthread.running_)
            {
              m_workerthread.cond_.wait(guard);
            }
        }
        auto t0 = std::chrono::high_resolution_clock::now();
        ExecutionContextBase::invokeWorkerDo();
        ExecutionContextBase::invokeWorkerPostDo();
        auto t1 = std::chrono::high_resolution_clock::now();

        auto period = getPeriod();
        if (count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", std::chrono::duration<double>(period).count()));
            RTC_PARANOID(("Execution: %f [s]", std::chrono::duration<double>(t1 - t0).count()));
            RTC_PARANOID(("Sleep:     %f [s]", std::chrono::duration<double>(period - (t1 - t0)).count()));
          }
        auto t2 = std::chrono::high_resolution_clock::now();
        if (m_nowait) { ++count; continue; }

        std::chrono::high_resolution_clock::time_point wakeup_point;
        if (getWakeUpTime(wakeup_point, t0, t1))
          {
            std::this_thread::sleep_until(wakeup_point);
            if (count > 1000)
              {
                auto t3 = std::chrono::high_resolution_clock::now();
                RTC_PARANOID(("Slept:     %f [s]", std::chrono::duration<double>(t3 - t2).count()));
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
      std::lock_guard<std::mutex> guard(m_svcmutex);
      if (!m_svc)
        {
          m_svc = true;
          this->open(nullptr);
        }
    }
    if (isAllNextState(RTC::INACTIVE_STATE))
      {
        std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
        m_workerthread.running_ = false;
      }
    else
      {
        std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
        m_workerthread.running_ = true;
        m_workerthread.cond_.notify_one();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onStopping() template function
   */
  RTC::ReturnCode_t RTPreemptEC::onStopping()
  {
    // stop thread
    std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
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
    std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
    if (!m_workerthread.running_)
      {
        m_workerthread.running_ = true;
        m_workerthread.cond_.notify_one();
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
    std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
    if (!m_workerthread.running_)
      {
        m_workerthread.running_ = true;
        m_workerthread.cond_.notify_one();
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
        std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
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
        std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
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
        std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
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
        std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
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
        policy = coil::normalize(std::move(policy));
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

  bool RTPreemptEC::getWakeUpTime(std::chrono::high_resolution_clock::time_point& tw,
                                  const std::chrono::high_resolution_clock::time_point& t0,
                                  const std::chrono::high_resolution_clock::time_point& t1)
  {
    tw = t0 + getPeriod() + std::chrono::nanoseconds(m_waitoffset);
    if (tw < t1)
      {
        std::cerr << "faital error: deadline passed. " << std::endl;
        std::cerr << "Wait time: ";
        std::cerr << (tw - t1).count() << "[ns]" << std::endl;
        std::cerr << "Next wait time force to: 0.0 [s]"
                  << std::endl;
        tw = t0;
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
                            ::RTC_exp::RTPreemptEC>);
    RTC::ExecutionContextFactory::
      instance().addFactory("rtpreempt_ec",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC_exp::RTPreemptEC>);
  }
}
