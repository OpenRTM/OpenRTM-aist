// -*- C++ -*-
/*!
 * @file VxWorksInterruptExecutionContext.h
 * @brief VxWorksInterruptExecutionContext class
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

#ifndef RTC_VXWORKSINTERRUPTEXECUTIONCONTEXT_H
#define RTC_VXWORKSINTERRUPTEXECUTIONCONTEXT_H

#include <rtm/RTC.h>

#include <coil/Mutex.h>
#include <coil/Condition.h>
#include <coil/Task.h>

#include <rtm/ExecutionContextBase.h>

#include <intLib.h>

namespace RTC
{
  /*!
   * @if jp
   * @class VxWorksInterruptExecutionContext
   * @brief ハードウェア割り込み、ソフトウェア割り込みで実行する ExecutionContext クラス
   *
   * 
   * ExecutionContextクラス。
   * 割り込みによるメソッド呼びだしによって時間が1周期づつ進む。
   *
   * @since 1.2.0
   *
   * @else
   * @brief 
   *
   * 
   *
   * @since 1.2.0
   *
   * @endif
   */
  class VxWorksInterruptExecutionContext
    : public virtual POA_RTC::ExecutionContextService,
      public virtual PortableServer::RefCountServantBase,
      public RTC::ExecutionContextBase,
      public coil::Task
  {
    typedef coil::Mutex Mutex;
    typedef coil::Condition<Mutex> Condition;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    VxWorksInterruptExecutionContext();
    
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
    virtual ~VxWorksInterruptExecutionContext(void);

    /*!
     * @if jp
     * @brief ExecutionContextの初期化を行う
     *
     * ExecutionContextの初期化処理
     *
     * @else
     * @brief Initialize the ExecutionContext
     *
     * This operation initialize the ExecutionContext
     *
     * @endif
     */
    virtual  void init(coil::Properties& props);
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
    virtual int open(void *args);

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
    virtual int close(unsigned long flags);

    /*!
     * @if jp
     * @brief 各 Component の処理を呼び出す。
     *
     * ExecutionContext に attach されている各 Component の処理を呼び出す。
     * 全 Component の処理を呼び出した後、次の呼出が発生するまで休止する。
     *
     * @return 処理結果
     *
     * @else
     * @brief Invoke each component's operation
     *
     * Invoke each component's operation which is attached this
     * ExecutionContext.  Stop until the next operation is invoked
     * after all component operations are invoked.
     *
     * @return Operation result
     *
     * @endif
     */
    virtual int svc(void);
    
    //============================================================
    // ExecutionContextService
    //============================================================
    /*!
     * @if jp
     * @brief 処理を1ステップ進める
     *
     * ExecutionContextの処理を1周期分進める。
     *
     * @else
     * @brief Move forward one step of ExecutionContext
     *
     * Move forward one step of the ExecutionContext processing.
     *
     * @endif
     */
    virtual void tick();
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
    virtual CORBA::Boolean is_running(void)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t start(void)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t stop(void)
      throw (CORBA::SystemException);

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
    virtual CORBA::Double get_rate(void)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t  set_rate(CORBA::Double rate)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t
    activate_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException);
    
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
    virtual RTC::ReturnCode_t
    deactivate_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t
    reset_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException);

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
    virtual RTC::LifeCycleState
    get_component_state(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException);

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
    virtual RTC::ExecutionKind get_kind(void)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t add_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException);

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
    virtual RTC::ReturnCode_t
    remove_component(RTC::LightweightRTObject_ptr comp)
      throw (CORBA::SystemException);

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
    virtual RTC::ExecutionContextProfile* get_profile(void)
      throw (CORBA::SystemException);

  protected:
    template <class T>
    void getProperty(coil::Properties& prop, const char* key, T& value)
    {
      if (prop.findNode(key) != 0)
        {
          T tmp;
          if (coil::stringTo(tmp, prop[key].c_str()))
            {
              value = tmp;
            }
        }
    }
    /*!
     * @brief onStarted() template function
     */
    virtual RTC::ReturnCode_t onStarted();
    // template virtual functions adding/removing component
    /*!
     * @brief onAddedComponent() template function
     */
     virtual RTC::ReturnCode_t
     onAddedComponent(RTC::LightweightRTObject_ptr rtobj);
    /*!
     * @brief onRemovedComponent() template function
     */
    virtual RTC::ReturnCode_t
    onRemovedComponent(RTC::LightweightRTObject_ptr rtobj);
    /*!
     * @brief onWaitingActivated() template function
     */
    virtual RTC::ReturnCode_t
    onWaitingActivated(RTC_impl::RTObjectStateMachine* comp, long int count);
    /*!
     * @brief onWaitingDeactivated() template function
     */
    virtual RTC::ReturnCode_t
    onWaitingDeactivated(RTC_impl::RTObjectStateMachine* comp, long int count);
    /*!
     * @brief onWaitingReset() template function
     */
    virtual RTC::ReturnCode_t
    onWaitingReset(RTC_impl::RTObjectStateMachine* comp, long int count);

  private:
    bool threadRunning()
    {
      std::lock_guard<coil::Mutex> guard(m_svcmutex);
      return m_svc;
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
    STATUS setInterrupt(coil::Properties& props);
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
    Mutex m_svcmutex;

    /*!
     * @if jp
     * @brief worker 用状態変数クラス
     * @else
     * @brief Condition variable class for worker
     * @endif
     */
    struct Worker
    {
      Worker() : cond_(mutex_), ticked_(false) {};
      Mutex mutex_;
      Condition cond_;
      bool ticked_;
    };
    // A condition variable for external triggered worker
    Worker m_worker;
    int exception_number;
  };  // class VxWorksInterruptExecutionContext
} // namespace RTC




extern "C"
{
  /*!
   * @if jp
   * @brief 当該 ExecutionContext 用Factoryクラスの登録。
   *
   * このExecutionContextを生成するFactoryクラスを
   * ExecutionContext管理用ObjectManagerに登録する。
   *
   * @else
   * @brief Register Factory class for this ExecutionContext
   *
   * Register the Factory class to create this ExecutionContext
   * to the ObjectManager for management ExecutionContext
   *
   * @endif
   */
  void VxWorksInterruptExecutionContextInit(RTC::Manager* manager);
};

#endif // RTC_VXWORKSINTERRUPTEXECUTIONCONTEXT_H

