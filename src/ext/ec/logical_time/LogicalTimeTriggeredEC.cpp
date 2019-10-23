// -*- C++ -*-
/*!
 * @file LogicalTimeTriggeredEC.cpp
 * @brief LogicalTimeTriggeredEC class
 * @date $Date: 2008-01-14 07:49:14 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2007-2008
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: LogicalTimeTriggeredEC.cpp 2307 2012-02-05 21:29:15Z n-ando $
 *
 */

#include <mutex>
#include <coil/ClockManager.h>

#include <rtm/ECFactory.h>
#include <rtm/RTObjectStateMachine.h>
#include "LogicalTimeTriggeredEC.h"

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  LogicalTimeTriggeredEC::LogicalTimeTriggeredEC()
    : ExecutionContextBase("exttrig_async_ec")
  {
    RTC_TRACE(("LogicalTimeTriggeredEC()"));

    // getting my reference
    setObjRef(this->_this());

    // profile initialization
    setKind(RTC::PERIODIC);
    setRate(DEFAULT_EXECUTION_RATE);

    RTC_DEBUG(("Actual period: %lld [nsec]", m_profile.getPeriod().count()));
  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  LogicalTimeTriggeredEC::~LogicalTimeTriggeredEC()
  {
    RTC_TRACE(("~LogicalTimeTriggeredEC()"));
    {
      {
        std::lock_guard<std::mutex> guard(m_svcmutex);
        m_svc = false;
      }
      {
        std::lock_guard<std::mutex> guard(m_worker.mutex_);
        m_worker.ticked_ = true;
        m_worker.cond_.notify_one();
      }
    }
    wait();
  }

  void LogicalTimeTriggeredEC::init(coil::Properties& props)
  {
    RTC_TRACE(("init()"));
    ExecutionContextBase::init(props);
    if (props.findNode("sync_tick") != nullptr)
      {
        m_syncTick = coil::toBool(props["sync_tick"], "YES", "NO", true);
        RTC_DEBUG(("Tick mode: %s",
                   m_syncTick ? "Synchronous" : "Asynchronous"));
      }
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
  int LogicalTimeTriggeredEC::open(void * /*args*/)
  {
    RTC_TRACE(("open()"));
    activate();
    return 0;
  }

  /*!
   * @if jp
   * @brief 各 Component の処理を呼び出す。
   * @else
   * @brief Invoke each component's operation
   * @endif
   */
  int LogicalTimeTriggeredEC::svc()
  {
    RTC_TRACE(("svc()"));
    unsigned int count(0);
    do
      {
        {
          std::unique_lock<std::mutex> guard(m_worker.mutex_);
          RTC_DEBUG(("Start of worker invocation. ticked = %s",
                     m_worker.ticked_ ? "true" : "false"));
          while (!m_worker.ticked_)
            {
              m_worker.cond_.wait(guard); // wait for tick
              RTC_DEBUG(("Thread was woken up."));
            }
          if (!m_worker.ticked_) { continue; }
        }
        auto t0 = std::chrono::steady_clock::now();
        ExecutionContextBase::invokeWorkerPreDo();
        ExecutionContextBase::invokeWorkerDo();
        ExecutionContextBase::invokeWorkerPostDo();
        auto t1 = std::chrono::steady_clock::now();
        {
          std::lock_guard<std::mutex> guard(m_worker.mutex_);
          m_worker.ticked_ = false;
        }
        auto period = getPeriod();
        auto rest = period - (t1 - t0);
        if (true) //count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", std::chrono::duration<double>(period).count()));
            RTC_PARANOID(("Execution: %f [s]", std::chrono::duration<double>(t1 - t0).count()));
            RTC_PARANOID(("Sleep:     %f [s]", std::chrono::duration<double>(rest).count()));
          }
        auto t2 = std::chrono::steady_clock::now();
        if (rest > std::chrono::seconds::zero())
          {
            if (true /*count > 1000*/) { RTC_PARANOID(("sleeping...")); }
            std::this_thread::sleep_until(t0 + period);
          }
        if (true) //count > 1000)
          {
            auto t3 = std::chrono::steady_clock::now();
            RTC_PARANOID(("Slept:       %f [s]", std::chrono::duration<double>(t3 - t2).count()));
            count = 0;
          }
        ++count;
      } while (threadRunning());

    return 0;
  }

  /*!
   * @if jp
   * @brief ExecutionContext 用のスレッド実行関数
   * @else
   * @brief Thread execution function for ExecutionContext
   * @endif
   */
  int LogicalTimeTriggeredEC::close(unsigned long  /*flags*/)
  {
    RTC_TRACE(("close()"));
    // At this point, this component have to be finished.
    // Current state and Next state should be RTC_EXITING.
    return 0;
  }

  //============================================================
  // LogicalTimeTriggeredECService
  //============================================================
  /*!
   * @if jp
   * @brief 処理を1ステップ進める
   * @else
   * @brief Move forward one step of ExecutionContext
   * @endif
   */
  void LogicalTimeTriggeredEC::
  tick(::CORBA::ULong sec, ::CORBA::ULong usec)
  {
    RTC_TRACE(("tick(sec = %d, usec = %d)", sec, usec));
    m_clock.settime(std::chrono::seconds(sec)
                    + std::chrono::microseconds(usec));

    if (!isRunning())
      {
        RTC_DEBUG(("EC is not running. do nothing."));
        return;
      }

    if (m_syncTick) // Synchronous tick mode
      {
        invokeWorker();
      }
    else            // Asynchronous tick mode
      {
        std::lock_guard<std::mutex> guard(m_worker.mutex_);
        m_worker.ticked_ = true;
        m_worker.cond_.notify_one();
        RTC_PARANOID(("EC was ticked. Signal was sent to worker thread."));
      }
    return;
  }

  void LogicalTimeTriggeredEC::
  get_time(::CORBA::ULong& sec, ::CORBA::ULong& usec)
  {
    std::chrono::nanoseconds time(m_clock.gettime());
    auto time_s = std::chrono::duration_cast<std::chrono::seconds>(time);
    auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(time);
    sec  = static_cast<::CORBA::ULong>(time_s.count());
    usec = static_cast<::CORBA::ULong>((time_us - time_s).count());
  }

  //============================================================
  // ExecutionContextService
  //============================================================
  /*!
   * @if jp
   * @brief ExecutionContext 実行状態確認関数
   * @else
   * @brief Check for ExecutionContext running state
   * @endif
   */
  CORBA::Boolean LogicalTimeTriggeredEC::is_running()
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::start()
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::stop()
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
  CORBA::Double LogicalTimeTriggeredEC::get_rate()
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::set_rate(CORBA::Double rate)
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
  LogicalTimeTriggeredEC::add_component(RTC::LightweightRTObject_ptr comp)
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
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
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
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
  RTC::LifeCycleState LogicalTimeTriggeredEC::
  get_component_state(RTC::LightweightRTObject_ptr comp)
  {
    return ExecutionContextBase::getComponentState(comp);
  }

  /*!
   * @if jp
   * @brief ExecutionKind を取得する
   * @else
   * @brief Get the ExecutionKind
   * @endif
   */
  RTC::ExecutionKind LogicalTimeTriggeredEC::get_kind()
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
  RTC::ExecutionContextProfile* LogicalTimeTriggeredEC::get_profile()
  {
    return ExecutionContextBase::getProfile();
  }

  //============================================================
  // protected functions
  //============================================================
  /*!
   * @brief calling worker
   */
  void LogicalTimeTriggeredEC::invokeWorker()
  {
    RTC_TRACE(("invokeWorker()"));
    if (!isRunning()) { return; }
    std::lock_guard<std::mutex> guard(m_tickmutex);

    ExecutionContextBase::invokeWorkerPreDo(); // update state
    auto t0 = std::chrono::steady_clock::now();
    ExecutionContextBase::invokeWorkerDo();
    auto t1 = std::chrono::steady_clock::now();
    ExecutionContextBase::invokeWorkerPostDo();
    auto t2 = std::chrono::steady_clock::now();

    auto period = getPeriod();
    auto rest = period - (t2 - t0);
    if (m_count > 1000)
      {
        RTC_PARANOID(("Period:      %f [s]", std::chrono::duration<double>(period).count()));
        RTC_PARANOID(("Exec-Do:     %f [s]", std::chrono::duration<double>(t1 - t0).count()));
        RTC_PARANOID(("Exec-PostDo: %f [s]", std::chrono::duration<double>(t2 - t0).count()));
        RTC_PARANOID(("Sleep:       %f [s]", std::chrono::duration<double>(rest).count()));
      }
    auto t3 = std::chrono::steady_clock::now();
    if (rest > std::chrono::seconds::zero())
      {
        if (m_count > 1000) { RTC_PARANOID(("sleeping...")); }
        std::this_thread::sleep_until(t0 + period);
      }
    if (m_count > 1000)
      {
        auto t4 = std::chrono::steady_clock::now();
        RTC_PARANOID(("Slept:       %f [s]", std::chrono::duration<double>(t4 - t3).count()));
        m_count = 0;
      }
    ++m_count;
    return;
  }
  /*!
   * @brief onStarted() template function
   */
  RTC::ReturnCode_t LogicalTimeTriggeredEC::onStarted()
  {
    // change EC thread state
    std::lock_guard<std::mutex> guard(m_svcmutex);
    if (m_syncTick) { return RTC::RTC_OK; }
    if (!m_svc)
      { // If start() is called first time, start the worker thread.
        m_svc = true;
        this->open(nullptr);
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingActivated() template function
   */
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
  onWaitingActivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingActivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    // Now comp's next state must be ACTIVE state
    // If worker thread is stopped, restart worker thread.

    if (m_syncTick) // Synchronous tick mode
      {
      }
    else            // Asynchronous tick mode
      {
        std::lock_guard<std::mutex> guard(m_worker.mutex_);
        m_worker.ticked_ = true;
        m_worker.cond_.notify_one();
      }
    return RTC::RTC_OK;
  }


  /*!
   * @brief onWaitingDeactivated() template function
   */
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
  onWaitingDeactivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingDeactivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    if (m_syncTick) // Synchronous tick mode
      {
      }
    else            // Asynchronous tick mode
      {
        std::lock_guard<std::mutex> guard(m_worker.mutex_);
        m_worker.ticked_ = true;
        m_worker.cond_.notify_one();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingReset() template function
   */
  RTC::ReturnCode_t LogicalTimeTriggeredEC::
  onWaitingReset(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingReset(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    if (m_syncTick) // Synchronous tick mode
      {
      }
    else            // Asynchronous tick mode
      {
        std::lock_guard<std::mutex> guard(m_worker.mutex_);
        m_worker.ticked_ = true;
        m_worker.cond_.notify_one();
      }
    return RTC::RTC_OK;
  }
} // namespace RTC


extern "C"
{
  /*!
   * @if jp
   * @brief 当該 ExecutionContext 用Factoryクラスの登録。
   * @else
   * @brief Register Factory class for this ExecutionContext
   * @endif
   */
  void LogicalTimeTriggeredECInit(RTC::Manager*  /*manager*/)
  {
    RTC::ExecutionContextFactory::
      instance().addFactory("ltt_ec",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC::LogicalTimeTriggeredEC>);
  }
}
