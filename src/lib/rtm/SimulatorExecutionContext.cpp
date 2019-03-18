// -*- C++ -*-
/*!
 * @file SimulatorExecutionContext.cpp
 * @brief SimulatorExecutionContext class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 20017
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */


#include "SimulatorExecutionContext.h"
#include <rtm/ECFactory.h>

#include <rtm/RTObjectStateMachine.h>




namespace RTC
{

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  SimulatorExecutionContext::SimulatorExecutionContext()
		: RTC::OpenHRPExecutionContext()
  {
	  
	  
	
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor 
   * @endif
   */
  SimulatorExecutionContext::~SimulatorExecutionContext()
  {

  }


  /*!
   * @if jp
   * @brief RTコンポーネントをアクティブ化する
   *
   * 内部でinvokeWorkerPreDo関数を呼ぶため、即座に
   * 状態を遷移させることができる。
   * 現在tick実行中の場合は実行終了まで待つ
   *
   * @param comp アクティブ化対象RTコンポーネント
   *
   * @return ReturnCode_t 型のリターンコード
   *
   * @else
   *
   * @brief Activate an RT-component
   *
   * @param comp The target RT-Component for activation
   *
   * @return The return code of ReturnCode_t type
   *
   * @endif
   */
  RTC::ReturnCode_t SimulatorExecutionContext::
	  activate_component(RTC::LightweightRTObject_ptr comp)
	  throw (CORBA::SystemException)
  {
	Guard guard(m_tickmutex);

	RTC_impl::RTObjectStateMachine* rtobj = m_worker.findComponent(comp);

	if (rtobj == nullptr)
	{
		return RTC::BAD_PARAMETER;
	}
	if (!(rtobj->isCurrentState(RTC::INACTIVE_STATE)))
	{
		return RTC::PRECONDITION_NOT_MET;
	}
	m_syncActivation = false;

	ExecutionContextBase::activateComponent(comp);
	invokeWorkerPreDo();
	if ((rtobj->isCurrentState(RTC::ACTIVE_STATE)))
	{
		return RTC::RTC_OK;
	}
	return RTC::RTC_ERROR;

  }


  /*!
   * @if jp
   * @brief RTコンポーネントを非アクティブ化する
   *
   * 内部でinvokeWorkerPreDo関数を呼ぶため、即座に
   * 状態を遷移させることができる。
   * 現在tick実行中の場合は実行終了まで待つ
   *
   * @param comp 非アクティブ化対象RTコンポーネント
   *
   * @return ReturnCode_t 型のリターンコード
   *
   * @else
   *
   * @brief Activate an RT-component
   *
   * @param comp The target RT-Component for deactivate
   *
   * @return The return code of ReturnCode_t type
   *
   * @endif
   */
  RTC::ReturnCode_t SimulatorExecutionContext::
  deactivate_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
	Guard guard(m_tickmutex);

	RTC_impl::RTObjectStateMachine* rtobj = m_worker.findComponent(comp);

	if (rtobj == nullptr)
	{
		  return RTC::BAD_PARAMETER;
	}
	if (!(rtobj->isCurrentState(RTC::ACTIVE_STATE)))
	{
		  return RTC::PRECONDITION_NOT_MET;
	}
	m_syncDeactivation = false;
	ExecutionContextBase::deactivateComponent(comp);
	invokeWorkerPreDo();
	invokeWorkerDo();
	invokeWorkerPostDo();

	if ((rtobj->isCurrentState(RTC::INACTIVE_STATE)))
	{
		return RTC::RTC_OK;
	}
	return RTC::RTC_ERROR;

  }


  /*!
   * @if jp
   * @brief RTコンポーネントをリセットする
   *
   * 内部でinvokeWorkerPreDo関数を呼ぶため、即座に
   * 状態を遷移させることができる。
   * 現在tick実行中の場合は実行終了まで待つ
   *
   * @param comp リセット対象RTコンポーネント
   *
   * @return ReturnCode_t 型のリターンコード
   *
   * @else
   *
   * @brief Activate an RT-component
   *
   * @param comp The target RT-Component for reset
   *
   * @return The return code of ReturnCode_t type
   *
   * @endif
   */
  RTC::ReturnCode_t SimulatorExecutionContext::
  reset_component(RTC::LightweightRTObject_ptr comp)
    throw (CORBA::SystemException)
  {
	Guard guard(m_tickmutex);

	RTC_impl::RTObjectStateMachine* rtobj = m_worker.findComponent(comp);

	if (rtobj == nullptr)
	{
		return RTC::BAD_PARAMETER;
	}
	if (!(rtobj->isCurrentState(RTC::ERROR_STATE)))
	{
		return RTC::PRECONDITION_NOT_MET;
	}
	m_syncReset = false;
	ExecutionContextBase::resetComponent(comp);
	invokeWorkerPreDo();
	invokeWorkerDo();
	invokeWorkerPostDo();

	if ((rtobj->isCurrentState(RTC::INACTIVE_STATE)))
	{
		return RTC::RTC_OK;
	}
	return RTC::RTC_ERROR;


  }

} // namespace RTC




extern "C"
{
  void SimulatorExecutionContextInit(RTC::Manager*  /*manager*/)
  {
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */
    RTC::ExecutionContextFactory::
      instance().addFactory("SimulatorExecutionContext",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC::SimulatorExecutionContext>,
                            ::coil::Destructor< ::RTC::ExecutionContextBase,
                            ::RTC::SimulatorExecutionContext>);
  }
};