// -*- C++ -*-
/*!
 * @file ExecutionContextWorker.cpp
 * @brief ExecutionContextWorker class
 * @date $Date: 2008-01-14 07:53:01 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008,2012,2016
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: ExecutionContextWorker.cpp 2271 2012-01-09 13:32:18Z n-ando $
 *
 */

#include <rtm/RTObject.h>
#include <rtm/RTObjectStateMachine.h>
#include <rtm/ExecutionContextWorker.h>

#include <algorithm>
#include <iostream>

#define DEEFAULT_PERIOD 0.000001
namespace RTC_impl
{
  /*!
   * @if jp
   * @brief デフォルトコンストラクタ
   * @else
   * @brief Default constructor
   * @endif
   */
  ExecutionContextWorker::ExecutionContextWorker()
  {
    RTC_TRACE(("ExecutionContextWorker()"));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ExecutionContextWorker::~ExecutionContextWorker()
  {
    RTC_TRACE(("~ExecutionContextWorker()"));
  }

  //============================================================
  // Object reference to EC
  //============================================================
  void ExecutionContextWorker::setECRef(RTC::ExecutionContextService_ptr ref)
  {
    m_ref = RTC::ExecutionContextService::_duplicate(ref);
  }
  RTC::ExecutionContextService_ptr ExecutionContextWorker::getECRef()
  {
    return RTC::ExecutionContextService::_duplicate(m_ref);
  }

  //============================================================
  // ExecutionContext
  //============================================================
  /*!
   * @if jp
   * @brief ExecutionContext 実行状態確認関数
   * @else
   * @brief Check for ExecutionContext running state
   * @endif
   */
  CORBA::Boolean ExecutionContextWorker::isRunning()
  {
    RTC_TRACE(("isRunning()"));
    return m_running;
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行を開始
   * @else
   * @brief Start the ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextWorker::start()
  {
    RTC_TRACE(("start()"));
    std::lock_guard<std::mutex> m_guard(m_mutex);
    if (m_running)
      {
        RTC_WARN(("ExecutionContext is already running."));
        return RTC::PRECONDITION_NOT_MET;
      }
    // invoke ComponentAction::on_startup for each comps.
    for (auto & comp : m_comps)
      {
        comp->onStartup();
      }
    RTC_DEBUG(("%d components started.", m_comps.size()));
    // change EC thread state
    m_running = true;

    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行を停止
   * @else
   * @brief Stop the ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextWorker::stop()
  {
    RTC_TRACE(("stop()"));
    std::lock_guard<std::mutex> m_guard(m_mutex);
    if (!m_running)
      {
        RTC_WARN(("ExecutionContext is already stopped."));
        return RTC::PRECONDITION_NOT_MET;
      }
    // stop thread
    m_running = false;

    // invoke on_shutdown for each comps.
    for (auto & comp : m_comps)
      {
        comp->onShutdown();
      }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief ExecutionContext の周期が変化した
   * @else
   * @brief Changing execution rate of the ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextWorker::rateChanged()
  {
    RTC_TRACE(("rateChanged()"));
    // invoke on_shutdown for each comps.
    RTC::ReturnCode_t ret(RTC::RTC_OK);
    for (auto & comp : m_comps)
      {
        RTC::ReturnCode_t tmp = comp->onRateChanged();
        if (tmp != RTC::RTC_OK) { ret = tmp; }
      }
    return ret;
  }


  /*!
   * @if jp
   * @brief RTコンポーネントをアクティブ化する
   * @else
   * @brief Activate an RT-Component
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextWorker::activateComponent(RTC::LightweightRTObject_ptr comp,
                                            RTObjectStateMachine*& rtobj)
  {
    RTC_TRACE(("activateComponent()"));
    RTObjectStateMachine* obj = findComponent(comp);
    if (obj == nullptr)
      {
        RTC_ERROR(("Given RTC is not participant of this EC."));
        return RTC::BAD_PARAMETER;
      }
    RTC_DEBUG(("Component found in the EC."));
    if (!(obj->activate()))
      {
        RTC_ERROR(("State of the RTC is not INACTIVE_STATE."));
        return RTC::PRECONDITION_NOT_MET;
      }
    RTC_DEBUG(("Component is in INACTIVE state. Going to ACTIVE state."));
    rtobj = obj;
    RTC_DEBUG(("activateComponent() done."));
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントを非アクティブ化する
   * @else
   * @brief Deactivate an RT-Component
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextWorker::deactivateComponent(RTC::LightweightRTObject_ptr comp,
                                              RTObjectStateMachine*& rtobj)
  {
    RTC_TRACE(("deactivateComponent()"));

    rtobj = findComponent(comp);
    if (rtobj == nullptr)
      {
        RTC_ERROR(("Given RTC is not participant of this EC."));
        return RTC::BAD_PARAMETER;
      }
    if (!(rtobj->deactivate()))
      {
        RTC_ERROR(("State of the RTC is not ACTIVE_STATE."));
        return RTC::PRECONDITION_NOT_MET;
      }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントをリセットする
   * @else
   * @brief Reset the RT-Component
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextWorker::resetComponent(RTC::LightweightRTObject_ptr comp,
                                         RTObjectStateMachine*& rtobj)
  {
    RTC_TRACE(("resetComponent()"));

    rtobj = findComponent(comp);
    if (rtobj == nullptr)
      {
        RTC_ERROR(("Given RTC is not participant of this EC."));
        return RTC::BAD_PARAMETER;
      }
    if (!(rtobj->reset()))
      {
        RTC_ERROR(("State of the RTC is not ERROR_STATE."));
        return RTC::PRECONDITION_NOT_MET;
      }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントの状態を取得する
   * @else
   * @brief Get RT-Component's state
   * @endif
   */
  RTC::LifeCycleState
  ExecutionContextWorker::getComponentState(RTC::LightweightRTObject_ptr comp)
  {
    RTC_TRACE(("getComponentState()"));

    RTObjectStateMachine* rtobj = findComponent(comp);
    if (rtobj == nullptr)
      {
        RTC_WARN(("Given RTC is not participant of this EC."));
        return RTC::CREATED_STATE;
      }
    RTC::LifeCycleState state = rtobj->getState();
    RTC_DEBUG(("getComponentState() = %s done", getStateString(state)));
    return state;
  }


  /*!
   * @if jp
   * @brief RTコンポーネントを追加する
   * @else
   * @brief Add an RT-Component
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextWorker::addComponent(RTC::LightweightRTObject_ptr comp)
  {
    RTC_TRACE(("addComponent()"));
    if (CORBA::is_nil(comp))
      {
        RTC_ERROR(("nil reference is given."));
        return RTC::BAD_PARAMETER;
      }
    try
      {
        std::lock_guard<std::mutex> guard(m_addedMutex);
        RTC::ExecutionContextService_var ec = getECRef();
        RTC::ExecutionContextHandle_t id = comp->attach_context(ec);
        m_addedComps.emplace_back(new RTObjectStateMachine(id, comp));
      }
    catch (CORBA::Exception& e)
      {
        (void)(e);
        RTC_ERROR(("addComponent() failed."));
        return RTC::RTC_ERROR;
      }
    RTC_DEBUG(("addComponent() succeeded."));

    // if EC is stopping, update component list immediately.
    std::lock_guard<std::mutex> guard(m_mutex);
    if (!m_running) { updateComponentList(); }

    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief コンポーネントをバインドする。
   * @else
   * @brief Bind the component.
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextWorker::
  bindComponent(RTC::RTObject_impl* rtc)
  {
    RTC_TRACE(("bindComponent()"));
    std::lock_guard<std::mutex> m_guard(m_mutex);
    if (rtc == nullptr)
      {
        RTC_ERROR(("NULL pointer is given."));
        return RTC::BAD_PARAMETER;
      }
    RTC::ExecutionContextService_var ec = getECRef();
    RTC::ExecutionContextHandle_t id = rtc->bindContext(ec);
    if (id < 0 || id > ECOTHER_OFFSET)
      {
        // id should be owned context id < ECOTHER_OFFSET
        RTC_ERROR(("bindContext returns invalid id: %d", id));
        return RTC::RTC_ERROR;
      }
    RTC_DEBUG(("bindContext returns id = %d", id));

    // rtc is owner of this EC
    RTC::LightweightRTObject_var comp
      = RTC::LightweightRTObject::_duplicate(rtc->getObjRef());
    m_comps.emplace_back(new RTObjectStateMachine(id, comp));
    RTC_DEBUG(("bindComponent() succeeded."));

    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief コンポーネントをコンポーネントリストから削除する
   * @else
   * @brief Remove the RT-Component from participant list
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextWorker::removeComponent(RTC::LightweightRTObject_ptr comp)
  {
    RTC_TRACE(("removeComponent()"));
    if (CORBA::is_nil(comp))
      {
        RTC_ERROR(("nil reference is given."));
        return RTC::BAD_PARAMETER;
      }

    RTObjectStateMachine* rtobj = findComponent(comp);

    if (rtobj == nullptr)
      {
        RTC_ERROR(("no RTC found in this context."));
        return  RTC::BAD_PARAMETER;
      }
    {
      std::lock_guard<std::mutex> removeGuard(m_removedMutex);
      m_removedComps.emplace_back(rtobj);
    }
    // if EC is stopping, update component list immediately.
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      if (!m_running) { updateComponentList(); }
    }

    return RTC::RTC_OK;
  }

  void ExecutionContextWorker::updateComponentList()
  {
    {    // adding component
      std::lock_guard<std::mutex> addedGuard(m_addedMutex);
      for (auto & m_addedComp : m_addedComps)
        {
          m_comps.emplace_back(std::move(m_addedComp));
          RTC_TRACE(("Component added."));
        }
      m_addedComps.clear();
    }
    {    // removing component
      std::lock_guard<std::mutex> removedGuard(m_removedMutex);
      for (auto rtobj : m_removedComps)
        {
          RTC::LightweightRTObject_var lwrtobj = rtobj->getRTObject();
          lwrtobj->detach_context(rtobj->getExecutionContextHandle());
          CompItr it;
          it = std::find(m_comps.begin(), m_comps.end(), rtobj);
          assert(*it == rtobj);
          m_comps.erase(it);
          delete rtobj;
          RTC_TRACE(("Component deleted."));
        }
      m_removedComps.clear();
    }
  }

  RTObjectStateMachine*
  ExecutionContextWorker::findComponent(RTC::LightweightRTObject_ptr comp)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & rtobj : m_comps)
      {
        if (rtobj->isEquivalent(comp))
          {
            return rtobj;
          }
      }
    return nullptr;
  }

  bool ExecutionContextWorker::
  isAllCurrentState(ExecContextState state)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & comp : m_comps)
      {
        if (!comp->isCurrentState(state)) { return false; }
      }
    return true;
  }

  bool ExecutionContextWorker::
  isAllNextState(ExecContextState state)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & comp : m_comps)
      {
        if (!comp->isNextState(state)) { return false; }
      }
    return true;
  }

  bool ExecutionContextWorker::
  isOneOfCurrentState(ExecContextState state)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & comp : m_comps)
      {
        if (comp->isCurrentState(state)) { return true; }
      }
    return false;
  }

  bool ExecutionContextWorker::
  isOneOfNextState(ExecContextState state)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto & comp : m_comps)
      {
        if (comp->isNextState(state)) { return true; }
      }
    return false;
  }

  void ExecutionContextWorker::invokeWorker()
  {
    RTC_PARANOID(("invokeWorker()"));
    // m_comps never changes its size here
    for (auto & comp : m_comps) { 
        comp->workerPreDo();  
    }
    for (auto & comp : m_comps) { 
        comp->workerDo();     
    }
    for (auto & comp : m_comps) { 
        comp->workerPostDo(); 
    }
    std::lock_guard<std::mutex> guard(m_mutex);
    updateComponentList();
  }

  void ExecutionContextWorker::invokeWorkerPreDo()
  {
    RTC_PARANOID(("invokeWorkerPreDo()"));
    // m_comps never changes its size here
    for (auto & comp : m_comps) { 
        comp->workerPreDo();  
    }
  }

  void ExecutionContextWorker::invokeWorkerDo()
  {
    RTC_PARANOID(("invokeWorkerDo()"));
    // m_comps never changes its size here
    for (auto & comp : m_comps) { 
        comp->workerDo();     
    }
  }

  void ExecutionContextWorker::invokeWorkerPostDo()
  {
    RTC_PARANOID(("invokeWorkerPostDo()"));
    // m_comps never changes its size here
    for (auto & comp : m_comps) { 
        comp->workerPostDo(); 
    }
    // m_comps might be changed here
    std::lock_guard<std::mutex> guard(m_mutex);
    updateComponentList();
  }

} // namespace RTC_impl

