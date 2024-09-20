// -*- C++ -*-
/*!
 * @file VxWorksIOTrigExecutionContext.cpp
 * @brief VxWorksIOTrigExecutionContext class
 * @date $Date: 2017-07-11 15:49:16 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#include <coil/TimeValue.h>
#include <mutex>

#include <rtm/VxWorksInterruptExecutionContext.h>
#include <rtm/ECFactory.h>
#include <rtm/RTObjectStateMachine.h>

#include <iostream>


#if defined(__powerpc__)
#include <arch/ppc/ivPpc.h>
#define IV_CI 0
#define IV_SRI 4
#define IV_STI 5
#define IV_JRI 6
#define IV_JTI 7
#define IV_D0I 8
#define IV_D1I 9
#define IV_D2I 10
#define IV_D3I 11
#define IV_E0I 27
#define IV_E1I 28
#define IV_E2I 29
#define IV_E3I 30
#define IV_E4I 31
#else
#include <arch/simlinux/ivSimlinux.h>
#endif



namespace RTC
{
  static void tickHandler(int param)
  {
    VxWorksInterruptExecutionContext* ec = (VxWorksInterruptExecutionContext*)param;
    ec->tick();
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  VxWorksInterruptExecutionContext::VxWorksInterruptExecutionContext()
    : ExecutionContextBase("vxworks_interrupt_ec"),
      rtclog("vxworks_interrupt_ec"),
      m_svc(false),
      exception_number(-1)
  {
    RTC_TRACE(("VxWorksInterruptExecutionContext()"));

    // getting my reference
    setObjRef(this->_this());

    // profile initialization
    setKind(RTC::PERIODIC);
    setRate(DEFAULT_EXECUTION_RATE);

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
  VxWorksInterruptExecutionContext::~VxWorksInterruptExecutionContext()
  {
    RTC_TRACE(("~VxWorksInterruptExecutionContext()"));
    {
      {
        std::lock_guard<coil::Mutex> guard(m_svcmutex);
        m_svc = false;
      }
      {
        std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
        m_worker.ticked_ = true;
        m_worker.cond_.signal();
      }
    }
    wait();
  }

  void VxWorksInterruptExecutionContext::init(coil::Properties& props)
  {
    RTC_TRACE(("init()"));
    ExecutionContextBase::init(props);

    setInterrupt(props);

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
  int VxWorksInterruptExecutionContext::open(void *args)
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
  int VxWorksInterruptExecutionContext::svc(void)
  {
    RTC_TRACE(("svc()"));
    unsigned int count(0);
    do
      {
        {
          std::lock_guard<coil::Mutex> gurad(m_worker.mutex_);
          RTC_DEBUG(("Start of worker invocation. ticked = %s",
                     m_worker.ticked_ ? "true" : "false"));
          while (!m_worker.ticked_)
            {
              m_worker.cond_.wait(); // wait for tick
              RTC_DEBUG(("Thread was woken up."));
            }
          if (!m_worker.ticked_) { continue; }
        }
        auto t0 = std::chrono::high_resolution_clock::now();
        ExecutionContextBase::invokeWorkerPreDo();
        ExecutionContextBase::invokeWorkerDo();
        ExecutionContextBase::invokeWorkerPostDo();
        auto t1 = std::chrono::high_resolution_clock::now();
        {
          std::lock_guard<coil::Mutex> gurad(m_worker.mutex_);
          m_worker.ticked_ = false;
        }
        coil::TimeValue period(getPeriod());
        auto rest = period.microseconds() - (t1 - t0);
        if (1) //count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", (double)period));
            RTC_PARANOID(("Execution: %f [s]", std::chrono::duration<double>(t1 - t0).count()));
            RTC_PARANOID(("Sleep:     %f [s]", std::chrono::duration<double>(rest).count()));
          }
        auto t2 = std::chrono::high_resolution_clock::now();
        if (rest > std::chrono::seconds::zero())
          {
            if (1 /*count > 1000*/) { RTC_PARANOID(("sleeping...")); }
            std::this_thread::sleep_until(t0 + period.microseconds());
          }
        if (1) //count > 1000)
          {
            auto t3 = std::chrono::high_resolution_clock::now();
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
  int VxWorksInterruptExecutionContext::close(unsigned long flags)
  {
    RTC_TRACE(("close()"));
    // At this point, this component have to be finished.
    // Current state and Next state should be RTC_EXITING.
    return 0;
  }
  //============================================================
  // ExecutionContextService
  //============================================================
  /*!
   * @if jp
   * @brief 処理を1ステップ進める
   * @else
   * @brief Move forward one step of ExecutionContext
   * @endif
   */
  void VxWorksInterruptExecutionContext::tick()
  {
    if (!isRunning())
      {
        return;
      }
    std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
    m_worker.ticked_ = true;
    m_worker.cond_.signal();
    return;
  }
  /*!
   * @if jp
   * @brief ExecutionContext 実行状態確認関数
   * @else
   * @brief Check for ExecutionContext running state
   * @endif
   */
  CORBA::Boolean VxWorksInterruptExecutionContext::is_running()
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::start()
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::stop()
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
  CORBA::Double VxWorksInterruptExecutionContext::get_rate()
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::set_rate(CORBA::Double rate)
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
  VxWorksInterruptExecutionContext::add_component(RTC::LightweightRTObject_ptr comp)
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
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
  RTC::LifeCycleState VxWorksInterruptExecutionContext::
  get_component_state(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
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
  RTC::ExecutionKind VxWorksInterruptExecutionContext::get_kind()
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
  RTC::ExecutionContextProfile* VxWorksInterruptExecutionContext::get_profile()
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
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::onStarted()
  {
    // change EC thread state
    std::lock_guard<coil::Mutex> gurad(m_svcmutex);
    if (!m_svc)
      { // If start() is called first time, start the worker thread.
        m_svc = true;
        this->open(0);
      }
    return RTC::RTC_OK;
  }
  // template virtual functions adding/removing component
  /*!
   * @brief onAddedComponent() template function
   */
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
  onAddedComponent(RTC::LightweightRTObject_ptr rtobj)
  {
    std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
    if (m_worker.ticked_ == false)
      {
        ExecutionContextBase::m_worker.updateComponentList();
      }
    return RTC::RTC_OK;
  }
  /*!
   * @brief onRemovedComponent() template function
   */
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
  onRemovedComponent(RTC::LightweightRTObject_ptr rtobj)
  {
    std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
    if (m_worker.ticked_ == false)
      {
        ExecutionContextBase::m_worker.updateComponentList();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingActivated() template function
   */
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
  onWaitingActivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingActivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    // Now comp's next state must be ACTIVE state
    // If worker thread is stopped, restart worker thread.
    std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
    m_worker.ticked_ = true;
    m_worker.cond_.signal();
    return RTC::RTC_OK;
  }


  /*!
   * @brief onWaitingDeactivated() template function
   */
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
  onWaitingDeactivated(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingDeactivated(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
    m_worker.ticked_ = true;
    m_worker.cond_.signal();
    return RTC::RTC_OK;
  }

  /*!
   * @brief onWaitingReset() template function
   */
  RTC::ReturnCode_t VxWorksInterruptExecutionContext::
  onWaitingReset(RTC_impl::RTObjectStateMachine* comp, long int count)
  {
    RTC_TRACE(("onWaitingReset(count = %d)", count));
    RTC_PARANOID(("curr: %s, next: %s",
                  getStateString(comp->getStates().curr),
                  getStateString(comp->getStates().next)));
    std::lock_guard<coil::Mutex> guard(m_worker.mutex_);
    m_worker.ticked_ = true;
    m_worker.cond_.signal();
    return RTC::RTC_OK;
  }
  /*!
   * @if jp
   * @brief 割り込みの設定
   *
   * @param props
   *
   * @return 
   *
   * @else
   *
   * @brief set Interrupt
   *
   * @param props
   *
   * @return 
   *
   * @endif
   */
  STATUS VxWorksInterruptExecutionContext::setInterrupt(coil::Properties& props)
  {
    std::string int_param;
    int int_value = -1;
    getProperty(props, "interrupt", int_param);
#if defined(__powerpc__)
    if(int_param == "CI")
    {
        int_value = IV_CI;
    }
    else if(int_param == "SRI")
    {
        int_value = IV_SRI;
    }
    else if(int_param == "STI")
    {
        int_value = IV_STI;
    }
    else if(int_param == "JRI")
    {
        int_value = IV_JRI;
    }
    else if(int_param == "JTI")
    {
        int_value = IV_JTI;
    }
    else if(int_param == "D0I")
    {
        int_value = IV_D0I;
    }
    else if(int_param == "D1I")
    {
        int_value = IV_D1I;
    }
    else if(int_param == "D2I")
    {
        int_value = IV_D2I;
    }
    else if(int_param == "D3I")
    {
        int_value = IV_D3I;
    }
    else if(int_param == "E0I")
    {
        int_value = IV_E0I;
    }
    else if(int_param == "E1I")
    {
        int_value = IV_E1I;
    }
    else if(int_param == "E2I")
    {
        int_value = IV_E2I;
    }
    else if(int_param == "E3I")
    {
        int_value = IV_E3I;
    }
    else if(int_param == "E4I")
    {
        int_value = IV_E4I;
    }
#else
    if(int_param == "HUP")
    {
        int_value = IV_HUP;
    }
    else if(int_param == "INT")
    {
        int_value = IV_INT;
    }
    else if(int_param == "ILL")
    {
        int_value = IV_ILL;
    }
    else if(int_param == "TRAP")
    {
        int_value = IV_TRAP;
    }
    else if(int_param == "BUS")
    {
        int_value = IV_BUS;
    }
    else if(int_param == "FPE")
    {
        int_value = IV_FPE;
    }
    else if(int_param == "SEGV")
    {
        int_value = IV_SEGV;
    }
    else if(int_param == "CLOCK1")
    {
        int_value = IV_CLOCK1;
    }
    else if(int_param == "USR1")
    {
        int_value = IV_USR1;
    }
    else if(int_param == "USR2")
    {
        int_value = IV_USR2;
    }
    else if(int_param == "IO")
    {
        int_value = IV_IO;
    }
    else if(int_param == "CLOCK2")
    {
        int_value = IV_CLOCK2;
    }
    else if(int_param == "CLOCK3")
    {
        int_value = IV_CLOCK3;
    }
    else if(int_param == "IPI0")
    {
        int_value = IV_IPI0;
    }
    else if(int_param == "IPI1")
    {
        int_value = IV_IPI1;
    }
    else if(int_param == "IPI2")
    {
        int_value = IV_IPI2;
    }
    else if(int_param == "IPI3")
    {
        int_value = IV_IPI3;
    }
#endif
    if(int_value >= 0)
    {
        intConnect(INUM_TO_IVEC(int_value), (VOIDFUNCPTR)tickHandler, (int)this);
    }
  }
};


extern "C"
{
  /*!
   * @if jp
   * @brief 当該 ExecutionContext 用Factoryクラスの登録。
   * @else
   * @brief Register Factory class for this ExecutionContext
   * @endif
   */
  void VxWorksInterruptExecutionContextInit(RTC::Manager* manager)
  {
    RTC::ExecutionContextFactory::
      instance().addFactory("VxWorksInterruptExecutionContext",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC::VxWorksInterruptExecutionContext>,
                            ::coil::Destructor< ::RTC::ExecutionContextBase,
                            ::RTC::VxWorksInterruptExecutionContext>);
  }
};

