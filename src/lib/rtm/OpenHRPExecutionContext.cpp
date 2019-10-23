// -*- C++ -*-
/*!
 * @file OpenHRPExecutionContext.cpp
 * @brief OpenHRPExecutionContext class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008,2012
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/OpenHRPExecutionContext.h>
#include <rtm/ECFactory.h>
#include <thread>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OpenHRPExecutionContext::OpenHRPExecutionContext()
    :  ExecutionContextBase("exttrig_sync_ec")
  {
    RTC_TRACE(("OpenHRPExecutionContext()"));

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
  OpenHRPExecutionContext::~OpenHRPExecutionContext()
  {
    RTC_TRACE(("~OpenHRPExecutionContext()"));
    std::lock_guard<std::mutex> guard(m_tickmutex);
  }

  //============================================================
  // OpenHRPExecutionContextService
  //============================================================
  /*!
   * @if jp
   * @brief 処理を1ステップ進める
   * @else
   * @brief Move forward one step of ExecutionContext
   * @endif
   */
  void OpenHRPExecutionContext::tick()
  {
    RTC_TRACE(("tick()"));
    if (!isRunning()) { return; }
    std::lock_guard<std::mutex> guard(m_tickmutex);

    ExecutionContextBase::invokeWorkerPreDo();  // update state
    auto t0 = std::chrono::high_resolution_clock::now();
    ExecutionContextBase::invokeWorkerDo();
    auto t1 = std::chrono::high_resolution_clock::now();
    ExecutionContextBase::invokeWorkerPostDo();
    auto t2 = std::chrono::high_resolution_clock::now();

    auto period = getPeriod();
    auto rest = period - (t2 - t0);
    if (m_count > 1000)
      {
        RTC_PARANOID(("Period:      %f [s]", std::chrono::duration<double>(period).count()));
        RTC_PARANOID(("Exec-Do:     %f [s]", std::chrono::duration<double>(t1 - t0).count()));
        RTC_PARANOID(("Exec-PostDo: %f [s]", std::chrono::duration<double>(t2 - t1).count()));
        RTC_PARANOID(("Sleep:       %f [s]", std::chrono::duration<double>(rest).count()));
      }
    auto t3 = std::chrono::high_resolution_clock::now();
    if (rest > std::chrono::seconds::zero())
      {
        if (m_count > 1000) { RTC_PARANOID(("sleeping...")); }
        std::this_thread::sleep_until(t0 + period);
      }
    if (m_count > 1000)
      {
        auto t4 = std::chrono::high_resolution_clock::now();
        RTC_PARANOID(("Slept:       %f [s]", std::chrono::duration<double>(t4 - t3).count()));
        m_count = 0;
      }
    ++m_count;
    return;
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
  CORBA::Boolean OpenHRPExecutionContext::is_running()
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
  RTC::ReturnCode_t OpenHRPExecutionContext::start()
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
  RTC::ReturnCode_t OpenHRPExecutionContext::stop()
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
  CORBA::Double OpenHRPExecutionContext::get_rate()
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
  RTC::ReturnCode_t OpenHRPExecutionContext::set_rate(CORBA::Double rate)
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
  OpenHRPExecutionContext::add_component(RTC::LightweightRTObject_ptr comp)
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
  RTC::ReturnCode_t OpenHRPExecutionContext::
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
  RTC::ReturnCode_t OpenHRPExecutionContext::
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
  RTC::ReturnCode_t OpenHRPExecutionContext::
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
  RTC::ReturnCode_t OpenHRPExecutionContext::
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
  RTC::LifeCycleState OpenHRPExecutionContext::
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
  RTC::ExecutionKind OpenHRPExecutionContext::get_kind()
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
  RTC::ExecutionContextProfile* OpenHRPExecutionContext::get_profile()
  {
    return ExecutionContextBase::getProfile();
  }


  // template virtual functions adding/removing component
  /*!
  * @brief onAddedComponent() template function
  */
  RTC::ReturnCode_t OpenHRPExecutionContext::
	  onAddedComponent(RTC::LightweightRTObject_ptr  /*rtobj*/)
  {
	  std::lock_guard<std::mutex> guard(m_tickmutex);

	  ExecutionContextBase::m_worker.updateComponentList();

	  return RTC::RTC_OK;
  }
  /*!
  * @brief onRemovedComponent() template function
  */
  RTC::ReturnCode_t OpenHRPExecutionContext::
	  onRemovedComponent(RTC::LightweightRTObject_ptr  /*rtobj*/)
  {
	  std::lock_guard<std::mutex> guard(m_tickmutex);

	  ExecutionContextBase::m_worker.updateComponentList();

	  return RTC::RTC_OK;
  }
} // namespace RTC


extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */
  void OpenHRPExecutionContextInit(RTC::Manager*  /*manager*/)
  {
    RTC::ExecutionContextFactory::
      instance().addFactory("SynchExtTriggerEC",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC::OpenHRPExecutionContext>);
  }
}
