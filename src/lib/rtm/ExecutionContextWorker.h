// -*- C++ -*-
/*!
 * @file ExecutionContextWorker.h
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

#ifndef RTC_EXECUTIONCONTEXTWORKER_H
#define RTC_EXECUTIONCONTEXTWORKER_H

#include <coil/Mutex.h>
#include <coil/Condition.h>
#include <vector>

#include <rtm/idl/RTCSkel.h>
#include <rtm/SystemLogger.h>
//#include <rtm/StateMachine.h>
//#include <rtm/RTObjectStateMachine.h>

#define NUM_OF_LIFECYCLESTATE 4

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC
{
  class RTObject_impl;
};
namespace RTC_impl
{
  class RTObjectStateMachine;
  typedef RTC::LightweightRTObject_ptr LightweightRTObject_ptr;
  typedef RTC::LightweightRTObject_var LightweightRTObject_var;
  /*!
   * @if jp
   * @class ExecutionContextWorker
   * @brief ExecutionContextWorker クラス
   *
   * RTCの状態マシン・プロキシクラス RTObjectStateMachine を保持・管理
   * し、コールバックを実際にコールするためのワーカークラスインターフェー
   * スクラス。ECの実装者は、ExecutionContextBase を継承し、メンバ変数
   * として保持されているこの ExecutionContextWorker クラスのインター
   * フェース経由でRTObjectのコールバックをコールすることになる。
   *
   * - ECのオブジェクトリファレンスのセッタ・ゲッター
   *   - void setECRef(RTC::ExecutionContextService_ptr ref);
   *   - RTC::ExecutionContextService_ptr getECRef();
   *   ECの具象クラス初期化時には、アクティブ化したECのオブジェクトリファ
   *   レンスをsetECRef() でセットする。
   *
   * - ECの実行・停止関係関数
   *   - CORBA::Boolean isRunning(void);
   *   - RTC::ReturnCode_t start(void);
   *   - RTC::ReturnCode_t stop(void);
   *
   * - RTCのアクティブ化・非アクティブ化関数
   *   - RTC::ReturnCode_t activateComponent(RTC::LightweightRTObject_ptr comp,
   *                                         RTObjectStateMachine*& rtobj);
   *   - RTC::ReturnCode_t waitActivateComplete(RTObjectStateMachine*& rtobj,
   *                                            coil::TimeValue timeout = 1.0,
   *                                            long int cycle = 1000);
   *
   *   - RTC::ReturnCode_t deactivateComponent(RTC::LightweightRTObject_ptr comp,  *                                           RTObjectStateMachine*& rtobj);
   *   - RTC::ReturnCode_t waitDeactivateComplete(RTObjectStateMachine*& rtobj,
   *                                              coil::TimeValue timeout = 1.0,
   *                                              long int cycle = 1000);
   *   -  RTC::ReturnCode_t resetComponent(RTC::LightweightRTObject_ptr com,
   *                                       RTObjectStateMachine*& rtobj);
   *   -  RTC::ReturnCode_t waitResetComplete(RTObjectStateMachine*& rtobj,
   *                                          coil::TimeValue timeout = 1.0,
   *                                          long int cycle = 1000);
   *   - RTC::LifeCycleState getComponentState(RTC::LightweightRTObject_ptr comp);
   *   - const char* getStateString(RTC::LifeCycleState state)
   *
   * - RTC追加・削除系関数
   *   - RTC::ReturnCode_t addComponent(RTC::LightweightRTObject_ptr comp);
   *   - RTC::ReturnCode_t bindComponent(RTC::RTObject_impl* rtc);
   *   - RTC::ReturnCode_t removeComponent(RTC::LightweightRTObject_ptr comp);
   *   - RTObjectStateMachine* findComponent(RTC::LightweightRTObject_ptr comp);
   *
   * - 状態問い合わせ系関数
   *   - bool isAllCurrentState(RTC::LifeCycleState state);
   *   - bool isAllNextState(RTC::LifeCycleState state);
   *   - bool isOneOfCurrentState(RTC::LifeCycleState state);
   *   - bool isOneOfNextState(RTC::LifeCycleState state);
   *
   * - Workerの駆動およびコールバック
   *   - void invokeWorker();
   *   - void invokeWorkerPreDo();
   *   - void invokeWorkerDo();
   *   - void invokeWorkerPostDo();
   *
   * @since 1.2
   *
   * @else
   * @class ExecutionContextWorker
   * @brief ExecutionContextWorker class
   *
   * Periodic Sampled Data Processing (for the execution cycles)
   * ExecutionContext class
   *
   * @since 0.4.0
   *
   * @endif
   */
  class ExecutionContextWorker
  {
    typedef coil::Guard<coil::Mutex> Guard;

  public:
    /*!
     * @if jp
     * @brief デフォルトコンストラクタ
     *
     * デフォルトコンストラクタ
     * プロファイルに以下の項目を設定する。
     *  - kind : PERIODIC
     *  - rate : 0.0
     *
     * @else
     * @brief Default Constructor
     *
     * Default Constructor
     * Set the following items to profile.
     *  - kind : PERIODIC
     *  - rate : 0.0
     *
     * @endif
     */
    ExecutionContextWorker();


    /*!
     * @if jp
     * @brief デストラクタ
     *
     * デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    virtual ~ExecutionContextWorker(void);

    //============================================================
    // Object reference to EC
    //============================================================
    void setECRef(RTC::ExecutionContextService_ptr ref);
    RTC::ExecutionContextService_ptr getECRef();

    //============================================================
    // ExecutionContext
    //============================================================
    /*!
     * @if jp
     * @brief ExecutionContext 実行状態確認関数
     *
     * この操作は ExecutionContext が Runnning 状態の場合に true を返す。
     * Executioncontext が Running の間、当該 Executioncontext に参加し
     * ている全てのアクティブRTコンポーネントが、ExecutionContext の実
     * 行種類に応じて実行される。
     *
     * @return 状態確認関数(動作中:true、停止中:false)
     *
     * @else
     *
     * @brief Check for ExecutionContext running state
     *
     * This operation shall return true if the context is in the
     * Running state.  While the context is Running, all Active RTCs
     * participating in the context shall be executed according to the
     * context’s execution kind.
     *
     * @return Check state function (Running:true、Stopping:false)
     *
     * @endif
     */
    CORBA::Boolean isRunning(void);

    /*!
     * @if jp
     * @brief ExecutionContext の実行を開始
     *
     * ExecutionContext の実行状態を Runnning とするためのリクエストを
     * 発行する。ExecutionContext の状態が遷移すると
     * ComponentAction::on_startup が呼び出される。参加しているRTコンポー
     * ネントが、初期化されるまで ExecutionContext を開始することはでき
     * ない。ExecutionContext は複数回開始/停止を繰り返すことができる。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Start the ExecutionContext
     *
     * Request that the context enter the Running state.  Once the
     * state transition occurs, the ComponentAction::on_startup
     * operation will be invoked.  An execution context may not be
     * started until the RT-Components that participate in it have
     * been initialized.  An execution context may be started and
     * stopped multiple times.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t start(void);

    /*!
     * @if jp
     * @brief ExecutionContext の実行を停止
     *
     * ExecutionContext の状態を Stopped とするためのリクエストを発行す
     * る。遷移が発生した場合は、ComponentAction::on_shutdown が呼び出
     * される。参加しているRTコンポーネントが終了する前に
     * ExecutionContext を停止する必要がある。ExecutionContext は複数回
     * 開始/停止を繰り返すことができる。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Stop the ExecutionContext
     *
     * Request that the context enter the Stopped state.  Once the
     * transition occurs, the ComponentAction::on_shutdown operation
     * will be invoked.  An execution context must be stopped before
     * the RT components that participate in it are finalized.  An
     * execution context may be started and stopped multiple times.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t stop(void);

    RTC::ReturnCode_t startThread(void);

    RTC::ReturnCode_t stopThread(void);

    
    /*!
     * @if jp
     * @brief ExecutionContext の実行周期が変更された
     *
     * ExecutionContext の実行周期が変更された場合のコールバックを呼び
     * 出す。ECの実装クラスの set_rate() オペレーションが呼び出された場
     * 合、ExecutionContextProfile::setRate() が呼び出され、プロファイ
     * ルの周期値が更新されるとともに、この関数が呼ばれ、
     * RTObjectStateMachine::onRateChagned() が呼び出される。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Execution rate of ExecutionContext chnaged
     *
     * A call back for execution rate chang when ExecutionContext's
     * execution rate. If set_rate() operation is called,
     * ExecutionContextProfile::setRate() will be called, execution
     * rate in the profile will be updated, and
     * RTObjectStateMachine::onRateChagned() will be called with this
     * member function.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t rateChanged();

    /*!
     * @if jp
     * @brief RTコンポーネントをアクティブ化する
     *
     * Inactive 状態にあるRTコンポーネントをActive に遷移させ、アクティ
     * ブ化する。この操作が呼ばれた結果、on_activate が呼び出される。指
     * 定したRTコンポーネントが参加者リストに含まれない場合は、
     * BAD_PARAMETER が返される。指定したRTコンポーネントの状態が
     * Inactive 以外の場合は、PRECONDITION_NOT_MET が返される。
     *
     * @param comp アクティブ化対象RTコンポーネント
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Activate an RT-component
     *
     * The given participant RTC is Inactive and is therefore not
     * being invoked according to the execution context’s execution
     * kind. This operation shall cause the RTC to transition to the
     * Active state such that it may subsequently be invoked in this
     * execution context.  The callback on_activate shall be called as
     * a result of calling this operation. This operation shall not
     * return until the callback has returned, and shall result in an
     * error if the callback does.
     *
     * @param comp The target RT-Component for activation
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t activateComponent(RTC::LightweightRTObject_ptr comp,
                                        RTObjectStateMachine*& rtobj);
    RTC::ReturnCode_t waitActivateComplete(RTObjectStateMachine*& rtobj,
                                           coil::TimeValue timeout = 1.0,
                                           long int cycle = 1000);
    /*!
     * @if jp
     * @brief RTコンポーネントを非アクティブ化する
     *
     * Inactive 状態にあるRTコンポーネントを非アクティブ化し、Inactive
     * に遷移させる。この操作が呼ばれた結果、on_deactivate が呼び出され
     * る。指定したRTコンポーネントが参加者リストに含まれない場合は、
     * BAD_PARAMETER が返される。指定したRTコンポーネントの状態が
     * Active 以外の場合は、PRECONDITION_NOT_MET が返される。
     *a
     * @param comp 非アクティブ化対象RTコンポーネント
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Deactivate an RT-component
     *
     * The given RTC is Active in the execution context. Cause it to
     * transition to the Inactive state such that it will not be
     * subsequently invoked from the context unless and until it is
     * activated again.  The callback on_deactivate shall be called as
     * a result of calling this operation. This operation shall not
     * return until the callback has returned, and shall result in an
     * error if the callback does.
     *
     * @param comp The target RT-Component for deactivate
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t deactivateComponent(RTC::LightweightRTObject_ptr comp,
                                          RTObjectStateMachine*& rtobj);
    RTC::ReturnCode_t waitDeactivateComplete(RTObjectStateMachine*& rtobj,
                                             coil::TimeValue timeout = 1.0,
                                             long int cycle = 1000);

    /*!
     * @if jp
     * @brief RTコンポーネントをリセットする
     *
     * Error 状態のRTコンポーネントの復帰を試みる。この操作が呼ばれた結
     * 果、on_reset が呼び出される。指定したRTコンポーネントが参加者リ
     * ストに含まれない場合は、BAD_PARAMETER が返される。指定したRTコン
     * ポーネントの状態が Error 以外の場合は、PRECONDITION_NOT_MET が返
     * される。
     *
     * @param comp リセット対象RTコンポーネント
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Reset the RT-component
     *
     * Attempt to recover the RTC when it is in Error.  The
     * ComponentAction::on_reset callback shall be invoked. This
     * operation shall not return until the callback has returned, and
     * shall result in an error if the callback does. If possible, the
     * RTC developer should implement that callback such that the RTC
     * may be returned to a valid state.
     *
     * @param comp The target RT-Component for reset
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t resetComponent(RTC::LightweightRTObject_ptr com,
                                     RTObjectStateMachine*& rtobj);
    RTC::ReturnCode_t waitResetComplete(RTObjectStateMachine*& rtobj,
                                        coil::TimeValue timeout = 1.0,
                                        long int cycle = 1000);

    /*!
     * @if jp
     * @brief RTコンポーネントの状態を取得する
     *
     * 指定したRTコンポーネントの状態(LifeCycleState)を取得する。指定し
     * たRTコンポーネントが参加者リストに含まれない場合は、
     * UNKNOWN_STATE が返される。
     *
     * @param comp 状態取得対象RTコンポーネント
     *
     * @return 現在の状態(LifeCycleState)
     *
     * @else
     *
     * @brief Get RT-component's state
     *
     * This operation shall report the LifeCycleState of the given
     * participant RTC.  UNKNOWN_STATE will be returned, if the given
     * RT-Component is not inclued in the participant list.
     *
     * @param comp The target RT-Component to get the state
     *
     * @return The current state of the target RT-Component(LifeCycleState)
     *
     * @endif
     */
    RTC::LifeCycleState getComponentState(RTC::LightweightRTObject_ptr comp);
    const char* getStateString(RTC::LifeCycleState state)
    {
      const char* st[] = {
        "CREATED_STATE",
        "INACTIVE_STATE",
        "ACTIVE_STATE",
        "ERROR_STATE"
      };
      return state >= RTC::CREATED_STATE && state <= RTC::ERROR_STATE ?
        st[state] : "";
    }
    /*!
     * @if jp
     * @brief RTコンポーネントを追加する
     *
     * 指定したRTコンポーネントを"参加者リスト"に追加する。追加された
     * RTコンポーネントは attach_context が呼ばれ、Inactive 状態に遷移
     * する。指定されたRTコンポーネントが null の場合は、BAD_PARAMETER が
     * 返される。指定されたRTコンポーネントが DataFlowComponent 以外の
     * 場合は、BAD_PARAMETER が返される。
     *
     * この関数が呼ばれた段階では、他の RTC は StateMachine 内のコール
     * バック関数を呼び出し中の可能性があり、まずは "参加者リスト:
     * m_addedComps" に RTC を追加しておく。コールバック呼び出しが一旦
     * 終わった段階で同期的に updateComponentList() 関数が呼ばれ、この
     * Worker に正式に RTC が参加した状態となる。
     *
     * @param comp 追加対象RTコンポーネント
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Add an RT-component
     *
     * The operation causes the given RTC to begin participating in
     * the execution context.  The newly added RTC will receive a call
     * to LightweightRTComponent::attach_context and then enter the
     * Inactive state.  BAD_PARAMETER will be invoked, if the given
     * RT-Component is null or if the given RT-Component is other than
     * DataFlowComponent.
     *
     * When right after this operation is called, since other RTCs
     * might being called their callbacks of the StateMachine, at
     * first the RTC is added into the "Participants: m_addedComps."
     * After RTCs callbacks are called, updateComponentList() function
     * is invoked and RTCs are officially a member of the worker.
     *
     * @param comp The target RT-Component for add
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t addComponent(RTC::LightweightRTObject_ptr comp);

    /*!
     * @if jp
     * @brief コンポーネントをバインドする。
     *
     * コンポーネントに owned context をバインドする。引数に渡される
     * RTObject_impl は、この EC の owner であり、内部的には RTObject
     * の bindContext() をコール氏、ec_idを取得する。ec_id は
     * ECOTHER_OFFSET よりも小さい。その後、RTObject のプロキシである
     * RTObjectStateMachine を生成し、コンポーネントのリストに加える。
     *
     * @param rtc RTコンポーネント
     * @return ReturnCode_t 型のリターンコード
     * @else
     * @brief Bind the component.
     *
     * This operation bind a RT-Component into owned context. Given
     * argument RTObject_impl is the owner of the EC, and internally
     * RTObject's bindContext() member function is called and obtain
     * ec_id. The ec_id must be smaller than ECOTHER_OFFSET. After
     * that proxy of RTObject RTObjectStateMachine is created, and it
     * is added to the component list.
     *
     * @param rtc RT-Component's instances
     * @return The return code of ReturnCode_t type
     * @endif
     */
    RTC::ReturnCode_t bindComponent(RTC::RTObject_impl* rtc);

    /*!
     * @if jp
     * @brief RTコンポーネントを参加者リストから削除する
     *
     * 指定したRTコンポーネントを参加者リストから削除する。削除された
     * RTコンポーネントは detach_context が呼ばれる。指定されたRTコンポー
     * ネントが参加者リストに登録されていない場合は、BAD_PARAMETER が返
     * される。
     *
     * @param comp 削除対象RTコンポーネント
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Remove the RT-Component from participant list
     *
     * This operation causes a participant RTC to stop participating in the
     * execution context.
     * The removed RTC will receive a call to
     * LightweightRTComponent::detach_context.
     * BAD_PARAMETER will be returned, if the given RT-Component is not 
     * participating in the participant list.
     *
     * @param comp The target RT-Component for delete
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t removeComponent(RTC::LightweightRTObject_ptr comp);
    RTObjectStateMachine* findComponent(RTC::LightweightRTObject_ptr comp);

    bool isAllCurrentState(RTC::LifeCycleState state);
    bool isAllNextState(RTC::LifeCycleState state);
    bool isOneOfCurrentState(RTC::LifeCycleState state);
    bool isOneOfNextState(RTC::LifeCycleState state);

    void invokeWorker();
    void invokeWorkerPreDo();
    void invokeWorkerDo();
    void invokeWorkerPostDo();

    /*!
     * @if jp
     * @brief コンポーネントリストの更新
     * @else
     * @brief Updating component list
     * @endif
     */
    void updateComponentList();

    //------------------------------------------------------------
    // member variables
  protected:
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    RTC::Logger rtclog;

    RTC::ExecutionContextService_var m_ref;

    /*!
     * @if jp
     * @brief ExecutionContext の実行状態
     * true: running, false: stopped
     * @else
     * @brief The running state of ExecutionContext
     * true: running, false: stopped
     * @endif
     */
    bool m_running;

    /*!
     * @if jp
     * @brief コンポーネントの参加者リスト
     * @else
     * @brief List of the participating component
     * @endif
     */
    std::vector<RTC_impl::RTObjectStateMachine*> m_comps;
    mutable coil::Mutex m_mutex;
    std::vector<RTC_impl::RTObjectStateMachine*> m_addedComps;
    mutable coil::Mutex m_addedMutex;
    std::vector<RTC_impl::RTObjectStateMachine*> m_removedComps;
    mutable coil::Mutex m_removedMutex;
    typedef std::vector<RTC_impl::RTObjectStateMachine*>::iterator CompItr;

  }; // class PeriodicExecutionContext
}; // namespace RTC

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif // RTC_PERIODICEXECUTIONCONTEXT_H
