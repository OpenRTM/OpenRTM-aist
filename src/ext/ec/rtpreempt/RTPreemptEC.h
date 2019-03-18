// -*- C++ -*-
/*!
 * @file RTPreemptEC.h
 * @brief RTPreemptEC class
 * @date $Date: 2008-01-14 07:53:05 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008,2012
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: RTPreemptEC.h 2307 2012-02-05 21:29:15Z n-ando $
 *
 */

#ifndef RTC_RTPREEMPTEC2_H
#define RTC_RTPREEMPTEC2_H

#include <vector>
#include <iostream>

#include <coil/Task.h>
#include <coil/Mutex.h>
#include <coil/Condition.h>

#include <rtm/ExecutionContextBase.h>

#define NUM_OF_LIFECYCLESTATE 4

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC_exp
{
  /*!
   * @if jp
   * @class RTPreemptEC
   * @brief RTPreemptEC クラス
   *
   * RT-Preempt kernel を利用した、リアルタイム実行コンテキストクラス。
   * この実行コンテキストは、RT-Preempt Patch を適用した Linux kernel、
   * 又は、これが組み込まれた Linux kernel によるリアルタイムスケジュー
   * リング機能を利用した実行コンテキストである。
   *
   * この実行コンテキストを利用するには、rtc.conf に下記のように記述する。
   *
   * <pre>
   * execution_contexts: rtpreempt_ec
   * ec.rtpreempt_ec.rate: 1000
   * ec.rtpreempt_ec.priority: 50
   * manager.modules.load_path: <RTPreemptRC.so がある場所へのパス>
   * manager.modules.preload: RTPreemptEC.so
   * </pre>
   *
   * さらに、実行時には root 権限が必要となるので、root としてコンポー
   * ネントを実行する必要がある。
   *
   * このECに特有なオプションは以下のとおりである。
   *
   * - ec.rtpreempt_ec.priority: (default: 49) <br>
   *      スレッドの実行優先度 1 (最低) から 99 (最高)<br>
   *      Linux sched_setscheduler(2) を参照のこと。<br>
   *
   * - ec.rtpreempt_ec.sched_policy:  (default: fifo)<br>
   *      スケジューリングのポリシ。<br>
   *      rr: ラウンドロビン, fifo: FIFO 型 (default: fifo)<br>
   *      Linux sched_setscheduler(2) を参照のこと。<br>
   *
   * - ec.rtpeempt_ec.wait_offset: (default: -10000)<br>
   *      ウェイト時間のオフセット。[ns] 単位で指定する。 <br>
   *      1周期あたり数十 us 程度の定常的な遅れが発生する場合があるので、
   *      この値を調整することで、より正確な周期で実行させることができる。
   *
   * 注意事項: このECを比較的速い周期 (数十ms以上) で実行させる場合は、
   * ログレベルを DEBUG よりも下 (logger.log_level: NORMAL 等) に設定し
   * て実行する必要がある。また、logger.enable: NO に設定されていても、
   * logger.log_level: PARANOID に設定されている場合には、onExecute()
   * の実行に時間がかかり、デッドラインを守れなくなる可能性があるので注
   * 意が必要である。
   *
   * @since 1.0.1
   *
   * @else
   * @class RTPreemptEC
   * @brief RTPreemptEC クラス
   *
   * This class is real-time ExecutionContext which utilizes RT-Prempt
   * kernel.  This ExecutionContext is a real-time ExecutionContext
   * which utilizes real-time scheduler functionality of "RT-Preempt"
   * Linux kernel (patched or originally embedded).
   *
   * Give the following configurations in your rtc.conf to use this EC.
   * <pre>
   * exec_cxt.periodic.type: RTPreemptEC
   * exec_cxt.periodic.rate: 1000
   * exec_cxt.priority: 50
   * manager.modules.load_path: <path to RTPreemptRC.so>
   * manager.modules.preload: RTPreemptEC.so
   * </pre>
   *
   * Since this functionality requires root authority, your RTC which
   * uses this EC have to be executed as root.
   *
   * The following RTPreemptEC specific options are available.
   *
   * - exec_cxt.periodic.priority: (default: 49)<br>
   * - exec_cxt.periodic.rtpreempt.priority: (default: 49)<br>
   *     Execution priority of threads from 1 (lowest) to 99 (highest)<br>
   *     See Linux sched_setscheduler(2).
   *
   * - exec_cxt.periodic.rtpreempt.sched_policy:  (default: fifo)<br>
   *      Scheduling policy.<br>
   *      rr: round-robin, fifo: FIFO type scheduling (default: fifo)<br>
   *      See Linux sched_setscheduler(2).
   *
   * - exec_cxt.periodic.rtpreempt.wait_offset: (default: -10000)<br>
   *      Offset time of wait. It can be given [ns] unit. <br> 
   *      Adjusting this value, If dozens micro seconds delay in one
   *      execution cycle constantly, more accurate periodic execution
   *      can be achieved.
   *
   * NOTICE: When performing comparatively quick cycle (tens of ms or
   * more) using this EC, log-level have to be lower than DEBUG
   * (logger.log_level: NORMAL or etc.). Moreover, even if
   * "logger.enable=No" is set, if "logger.log_level=PRANOID" is set,
   * execution of onExecute() takes longer time and it may cause
   * deadline-miss. So caution is needed.
   *
   * @since 1.0.1
   *
   * @endif
   */
  class RTPreemptEC
    : public virtual POA_RTC::ExecutionContextService,
      public virtual PortableServer::RefCountServantBase,
      public RTC::ExecutionContextBase,
      public coil::Task
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
    RTPreemptEC();

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
    ~RTPreemptEC() override;

    /*!
     * @if jp
     * @brief ExecutionContextの処理を進める
     *
     * ExecutionContextの処理を１周期分進める。
     *
     * @else
     * @brief Proceed with tick of ExecutionContext
     *
     * Proceed with tick of ExecutionContext for one period.
     *
     * @endif
     */
    void init(coil::Properties& props) override;

    /*!
     * @if jp
     * @brief ExecutionContext用アクティビティスレッドを生成する
     *
     * Executioncontext 用の内部アクティビティスレッドを生成し起動する。
     * これは coil::Task サービスクラスメソッドのオーバーライド。
     *
     * @param args 通常は0
     *
     * @return 生成処理実行結果
     *
     * @else
     *
     * @brief Generate internal activity thread for ExecutionContext
     *
     * Generate internal activity thread and run.  This is coil::Task
     * class method's override.
     *
     * @param args Usually give 0
     *
     * @return The generation result
     *
     * @endif
     */
    int open(void *args) override;

    /*!
     * @if jp
     * @brief ExecutionContext 用のスレッド実行関数
     *
     * ExecutionContext 用のスレッド実行関数。登録されたコンポーネント
     * の処理を呼び出す。
     *
     * @return 実行結果
     *
     * @else
     * @brief Thread execution function for ExecutionContext
     *
     * Thread execution function for ExecutionContext.  Invoke the
     * registered components operation.
     *
     * @return The execution result
     *
     * @endif
     */
    int svc() override;

    /*!
     * @if jp
     * @brief ExecutionContext 用のスレッド実行関数
     *
     * ExecutionContext 用のスレッド終了時に呼ばれる。コンポーネントオ
     * ブジェクトの非アクティブ化、マネージャへの通知を行う。これは
     * coil::Task サービスクラスメソッドのオーバーライド。
     *
     * @param flags 終了処理フラグ
     *
     * @return 終了処理結果
     *
     * @else
     *
     * @brief Thread execution function for ExecutionContext
     *
     * This function is invoked when activity thread for
     * ExecutionContext exits.  Deactivate the component object and
     * notify it to manager.  This is coil::Task class method's
     * override.
     *
     * @param flags Flag of the close
     *
     * @return The close result
     *
     * @endif
     */
    int close(unsigned long flags) override;

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
    CORBA::Boolean is_running()
      throw (CORBA::SystemException) override;

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
    RTC::ReturnCode_t start()
      throw (CORBA::SystemException) override;

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
    RTC::ReturnCode_t stop()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief ExecutionContext の実行周期(Hz)を取得する
     *
     * Active 状態にてRTコンポーネントが実行される周期(単位:Hz)を取得す
     * る。
     *
     * @return 処理周期(単位:Hz)
     *
     * @else
     *
     * @brief Get execution rate(Hz) of ExecutionContext
     *
     * This operation shall return the rate (in hertz) at which its
     * Active participating RTCs are being invoked.
     *
     * @return Execution cycle(Unit:Hz)
     *
     * @endif
     */
    CORBA::Double get_rate()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief ExecutionContext の実行周期(Hz)を設定する
     *
     * Active 状態にてRTコンポーネントが実行される周期(単位:Hz)を設定す
     * る。実行周期の変更は、DataFlowComponentAction の
     * on_rate_changed によって各RTコンポーネントに伝達される。
     *
     * @param rate 処理周期(単位:Hz)
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Set execution rate(Hz) of ExecutionContext
     *
     * This operation shall set the rate (in hertz) at which this
     * context’s Active participating RTCs are being called.  If the
     * execution kind of the context is PERIODIC, a rate change shall
     * result in the invocation of on_rate_changed on any RTCs
     * realizing DataFlowComponentAction that are registered with any
     * RTCs participating in the context.
     *
     * @param rate Execution cycle(Unit:Hz)
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t  set_rate(CORBA::Double rate)
      throw (CORBA::SystemException) override;

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
    RTC::ReturnCode_t
    activate_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief RTコンポーネントを非アクティブ化する
     *
     * Inactive 状態にあるRTコンポーネントを非アクティブ化し、Inactive
     * に遷移させる。この操作が呼ばれた結果、on_deactivate が呼び出され
     * る。指定したRTコンポーネントが参加者リストに含まれない場合は、
     * BAD_PARAMETER が返される。指定したRTコンポーネントの状態が
     * Active 以外の場合は、PRECONDITION_NOT_MET が返される。
     *
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
    RTC::ReturnCode_t
    deactivate_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException) override;

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
    RTC::ReturnCode_t
    reset_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException) override;

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
    RTC::LifeCycleState
    get_component_state(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief ExecutionKind を取得する
     *
     * 本 ExecutionContext の ExecutionKind を取得する
     *
     * @return ExecutionKind
     *
     * @else
     *
     * @brief Get the ExecutionKind
     *
     * This operation shall report the execution kind of the execution
     * context.
     *
     * @return ExecutionKind
     *
     * @endif
     */
    RTC::ExecutionKind get_kind()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief RTコンポーネントを追加する
     *
     * 指定したRTコンポーネントを参加者リストに追加する。追加されたRTコ
     * ンポーネントは attach_context が呼ばれ、Inactive 状態に遷移する。
     * 指定されたRTコンポーネントがnullの場合は、BAD_PARAMETER が返され
     * る。指定されたRTコンポーネントが DataFlowComponent 以外の場合は、
     * BAD_PARAMETER が返される。
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
     * @param comp The target RT-Component for add
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t
    add_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException) override;

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
     * This operation causes a participant RTC to stop participating
     * in the execution context.  The removed RTC will receive a call
     * to LightweightRTComponent::detach_context.  BAD_PARAMETER will
     * be returned, if the given RT-Component is not participating in
     * the participant list.
     *
     * @param comp The target RT-Component for delete
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    RTC::ReturnCode_t
    remove_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief ExecutionContextProfile を取得する
     *
     * 本 ExecutionContext のプロファイルを取得する。
     *
     * @return ExecutionContextProfile
     *
     * @else
     *
     * @brief Get the ExecutionContextProfile
     *
     * This operation provides a profile “descriptor” for the execution
     * context.
     *
     * @return ExecutionContextProfile
     *
     * @endif
     */
    RTC::ExecutionContextProfile* get_profile()
      throw (CORBA::SystemException) override;

  protected:
    /*!
     * @brief onStarted() template function
     */
    RTC::ReturnCode_t onStarted() override;
    /*!
     * @brief onStopping() template function
     */
    RTC::ReturnCode_t onStopping() override;
    /*!
     * @brief onWaitingActivated() template function
     */
    RTC::ReturnCode_t
    onWaitingActivated(RTC_impl::RTObjectStateMachine* comp, long int count) override;
    /*!
     * @brief onActivated() template function
     */
    RTC::ReturnCode_t
    onActivated(RTC_impl::RTObjectStateMachine* comp, long int count) override;
    /*!
     * @brief onWaitingDeactivated() template function
     */
    RTC::ReturnCode_t
    onWaitingDeactivated(RTC_impl::RTObjectStateMachine* comp,
                         long int count) override;
    /*!
     * @brief onDeactivated() template function
     */
    RTC::ReturnCode_t 
    onDeactivated(RTC_impl::RTObjectStateMachine* comp, long int count) override;
    /*!
     * @brief onWaitingReset() template function
     */
    RTC::ReturnCode_t
    onWaitingReset(RTC_impl::RTObjectStateMachine* comp, long int count) override;
    /*!
     * @brief onReset() template function
     */
    RTC::ReturnCode_t 
    onReset(RTC_impl::RTObjectStateMachine* comp, long int count) override;

    bool threadRunning()
    {
      Guard guard(m_svcmutex);
      return m_svc;
    }

    /*!
     * @if jp
     * @brief あるキーを持つプロパティを取得する
     *
     * @param ExecutionContext 用のスレッド実行関数。
     * 登録されたコンポーネントの処理を呼び出す。
     *
     * @return 実行結果
     *
     * @else
     * @brief Thread execution function for ExecutionContext
     *
     * Thread execution function for ExecutionContext.
     * Invoke the registered components operation.
     *
     * @return The execution result
     *
     * @endif
     */
    template <class T>
    void getProperty(coil::Properties& prop, const char* key, T& value)
    {
    if (prop.findNode(key) != nullptr)
      {
        T tmp;
        if (coil::stringTo(tmp, prop[key].c_str()))
          {
            value = tmp;
          }
      }
    }

  private:
    bool prepareThread();
    void setPriority(coil::Properties& prop);
    void setPolicy(coil::Properties& prop);
    void setWaitOffset(coil::Properties& prop);
    bool getSleepTime(struct timespec& ts,
                      const struct timespec& t0,
                      const struct timespec& t1);

  protected:
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    RTC::Logger rtclog;

    /*!
     * @if jp
     * @brief ExecutionContext のスレッド実行フラグ
     * @else
     * @brief The thread running flag of ExecutionContext
     * @endif
     */
    bool m_svc;
    coil::Mutex m_svcmutex;
    
    /*!
     * @if jp
     * @brief worker 用状態変数クラス
     * @else
     * @brief Condition variable class for worker
     * @endif
     */
    struct WorkerThreadCtrl
    {
      WorkerThreadCtrl() : cond_(mutex_), running_(false) {}
      coil::Mutex mutex_;
      coil::Condition<coil::Mutex> cond_;
      bool running_;
    };

    /*!
     * @if jp
     * @brief svn用の状態変数 
     * @else
     * @brief A condition variable for external triggered worker
     * @endif
     */
    WorkerThreadCtrl m_workerthread;

    /*!
     * @if jp
     * @brief ExecutionContext 即時実行(wait無し実行)フラグ
     * @else
     * @brief Flag of ExecutionContext to run immediately
     *        (to run without waiting)
     * @endif
     */
    bool m_nowait;

  private:
    int m_priority;
    int m_policy;
    int m_waitoffset;
  }; // class RTPreemptEC
} // namespace RTC_exp

#ifdef WIN32
#pragma warning( default : 4290 )
#endif


extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */
  void RTPreemptECInit(RTC::Manager* manager);
};

#endif // RTC_RTPREEMPTEC_H
