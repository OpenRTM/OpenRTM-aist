// -*- C++ -*-
/*!
 * @file ExecutionContextBase.h
 * @brief ExecutionContext base class
 * @date $Date: 2008-01-14 07:48:55 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2007, 2011-2012
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_EXECUTIONCONTEXTBASE_H
#define RTC_EXECUTIONCONTEXTBASE_H

#include <coil/Factory.h>

#include <rtm/idl/RTCSkel.h>
#include <rtm/idl/OpenRTMSkel.h>
#include <rtm/Factory.h>
#include <rtm/ExecutionContextProfile.h>
#include <rtm/ExecutionContextWorker.h>

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#define DEFAULT_EXECUTION_RATE 1000

namespace coil
{
  class Properties;
} // namespace coil

namespace RTC
{
  /*!
   * @if jp
   * @class ExecutionContextBase
   * @brief ExecutionContext用基底クラス
   *
   * ECの実装クラスでは、この基底クラスを継承し、かつECのCORBAオペレー
   * ションを実装しなければならない。さらに、実際にロジックを駆動するた
   * め、幾つかの約束に則りExecutionContextBaseの関数を呼び出す必要があ
   * る。ECのCORBAオペレーションは以下のものがあり、それぞれ
   * ExecutionContextBaseのメンバ関数に対応している。
   *
   * - is_running(): ExecutionContextBase::isRunning()
   * - start(): ExecutionContextBase::start()
   * - stop(): ExecutionContextBase::stop()
   *
   * - get_rate(): ExecutionContextBase::gatRate()
   * - set_rate(): ExecutioinContextBase::setRate()
   *
   * - add_component(): ExecutionContextBase::addComponent()
   * - remove_component(): ExecutionContextBase::removeComponent()
   *
   * - activate_component(): ExecutionContextBase::activateComponent()
   * - deactivate_component(): ExecutionContextBase::deactivateComponent()
   * - reset_component(): ExecutionContextBase::resetComponent()
   *
   * - get_component_state(): ExecutionContextBase::getComponentState()
   * - get_kind(): ExecutionContextBase::getKind()
   * - get_profile(): ExecutionContextBase::getProfile()
   *
   * @par 実行状態に関係する関数と実装方法
   * - is_running(): ExecutionContextBase::isRunning()
   * - start(): ExecutionContextBase::start()
   * - stop(): ExecutionContextBase::stop()
   *
   * 実行状態に関係する関数は、is_running(), start(), stop() の3つがあ
   * る。ExecutionContextBaseでは単純に running/stopped のフラグを持っ
   * ており、start/stopでフラグのON/OFF切り替え、is_running()で状態読み
   * 出しを行っている。通常、ECの実装クラスでは、protected な仮想メン
   * バ関数 onStarting(), onStarted(), onStopping(), onStopped() 関数を
   * 実装したうえで、CORBAオペレーションを以下のように実装する必要がある。
   *
   * is_running() のCORBAオペレーションでは、単純に
   * ExecutionContextBase の isRunning() を呼び出すだけである。この関数
   * に関連する protected 仮想関数はonIsRunning() が用意されているが、
   * 通常特に実装する必要はない。あえて、現在の running/stopped 状態を
   * 書き換えたい場合にこの関数を利用することができるが推奨はされない。
   *
   * <pre>
   * public:
   *  CORBA::Boolean is_runing()
   *  {
   *    return ExecutionContextBase::isRunning();
   *  }
   * protected:
   *  CORBA::Boolean onIsRunning(CORBA::Boolean running)
   *  {
   *    return running;
   *  }
   * </pre>
   *
   * start(), stop() CORBAオペレーションでは、通常
   * ExecutionContextBase の start(), stop() 関数を呼び出すよう実装する。
   * この関数に関連する protected 仮想関数は、start() および stop() に
   * ついてそれぞれ2つづつの onStarting(), onStarted(), および
   * onStopping(), onStopped() 関数がある。ECの実装クラスにおいては、そ
   * れぞれ以下のように実装する。
   *
   * <pre>
   *  RTC::ReturnCode_t start()
   *  {
   *    return ExecutionContextBase::start();
   *  }
   *  RTC::ReturnCode_t stop()
   *  {
   *    return ExecutionContextBase::stop();
   *  }
   * protected:
   *  RTC::ReturnCode_t onStarting()
   *  {
   *    RTC::ReturnCode_t ret = // スレッドを開始する処理など
   *    return ret;
   *  }
   *  RTC::ReturnCode_t onStarted()
   *  {
   *    RTC::ReturnCode_t ret = // スレッドを開始する処理など
   *    return ret;
   *  }
   *  RTC::ReturnCode_t onStopping()
   *  {
   *    // スレッドを停止する処理など
   *    return retcode;
   *  }
   *  RTC::ReturnCode_t onStopped()
   *  {
   *    // スレッドを停止する処理など
   *    return retcode;
   *  }
   * </pre>
   *
   * @par 実行周期に関する関数と実装方法
   * - get_rate(): ExecutionContextBase::gatRate()
   * - set_rate(): ExecutioinContextBase::setRate()
   *
   * 実行周期に関する関数は set_rate(), get_rate() の2種類がある。実装
   * する実行コンテキストがもし set_rate() により指定される周期を利用する
   * 場合、テンプレート関数 onSetRate() をオーバーライドし実装する。
   * onSetRate() は引数に double 型の周期を取り、この値は正当な値である
   * ことが保証されている。onSetRate() がRTC::RTC_OK 以外の値を返した場
   * 合、ECのProfileの周期は設定される以前の値を保持することが保証され
   * る。
   *
   * set_rate() 同様 get_rate() 呼び出し時にonGetRate() が呼び出される
   * が、これは通常オーバーライド剃る必要はない。ただし、get_rate() が
   * 返す値を変更したい場合、onGetRate() をオーバーライドすることでその
   * 値を書き換えることができる。ただし、これは推奨されない。
   *
   * <pre>
   * public:
   *  RTC::ReturnCode_t set_rate(double rate)
   *  {
   *    return setRate(rate);
   *  }
   *  double get_rate(void) const
   *  {
   *    return getRate();
   *  }
   * protected:
   *  virtual RTC::ReturnCode_t onSetRate(double rate)
   *  {
   *    RTC::ReturnCode_t ret = // 周期を設定する何らかの処理
   *    if (ret != RTC::RTC_OK)
   *      {
   *        RTC_ERROR(("Error message"));
   *      }
   *    return ret;
   *  }
   *  virtual double onGetRate(rate)
   *  {
   *    // get_rate() が返す値を加工したい場合
   *    // 通常はこの関数を実装する必要はない。
   *    return rate;
   *  }
   * </pre>
   *
   * @par コンポーネントの追加と削除に関する関数
   * - add_component(): ExecutionContextBase::addComponent()
   * - remove_component(): ExecutionContextBase::removeComponent()
   *
   * コンポーネントの追加と削除に関する関数は、add_component(),
   * remove_component() の二種類がある。実行コンテキストの実装クラスに
   * おいては、ExecutionContextBase のそれぞれ addComponent(),
   * removeComponent() を呼び出す形で実装を行う。これらの関数に関連する
   * protected 仮想関数は onAddingComponent(), onAddedComponent(),
   * onRemovingComponent(), onRemovedComponent() の4種類ある。ただし、
   * これらの仮想関数は通常オーバーライドする必要はなく、使用は推奨され
   * ない。
   *
   * <pre>
   * public:
   *  RTC::ReturnCode_t add_component(RTC::LightweightRTObject_ptr comp)
   *  {
   *    return ExecutionContextBase::addComponent(comp);
   *  }
   *  RTC::ReturnCode_t remove_component(RTC::LightweightRTObject_ptr comp)
   *  {
   *    return ExecutionContextBase::removeComponent(comp);
   *  }
   * protected:
   *  virtual RTC::ReturnCode_t
   *  onAddingComponent(RTC::LightweightRTObject rtobj)
   *  {
   *     // コンポーネント追加時に実行したい処理を記述
   *     // RTC::RTC_OK 以外を返した場合、コンポーネントの追加は行われない。
   *     return RTC::RTC_OK;
   *  }
   *  virtual RTC::ReturnCode_t
   *  onAddedComponent(RTC::LightweightRTObject rtobj)
   *  {
   *     // コンポーネント追加時に実行したい処理を記述
   *     // RTC::RTC_OK 以外を返した場合、removeComponent() が呼び出され、
   *     // 追加されたコンポーネントが削除される。
   *     return RTC::RTC_OK;
   *  }
   *  virtual RTC::ReturnCode_t
   *  onRemovingComponent(RTC::LightweightRTObject rtobj)
   *  {
   *     // コンポーネント削除時に実行したい処理を記述
   *     // RTC::RTC_OK 以外を返した場合、コンポーネントの削除は行われない。
   *     return RTC::RTC_OK;
   *  }
   *  virtual RTC::ReturnCode_t
   *  onRemovedComponent(RTC::LightweightRTObject rtobj)
   *  {
   *     // コンポーネント追加時に実行したい処理を記述
   *     // RTC::RTC_OK 以外を返した場合、addComponent() が呼び出され、
   *     // 削除されたコンポーネントが再び追加される。
   *     return RTC::RTC_OK;
   *  }
   * </pre>
   *
   * @par コンポーネントのアクティブ化等に関する関数
   * - activate_component(): ExecutionContextBase::activateComponent()
   * - deactivate_component(): ExecutionContextBase::deactivateComponent()
   * - reset_component(): ExecutionContextBase::resetComponent()
   *
   * コンポーネントのアクティブ化等に関する関数は、
   * activate_component(), deactivate_component(), reset_component() の
   * 三種類がある。実行コンテキストの実装クラスにおいては、
   * ExecutionContextBase のそれぞれ activateComponent(),
   * deactivateComponent(), resetComponent() を呼び出す形で実装を行う。
   * これらの関数に関連する protected 仮想関数は
   * onActivatingComponent(), onAtivatingComponent(),
   * onActivatedComponent(), onDeactivatingComponent(),
   * onDeactivatedComponent(), onResettingComponent(),
   * onResetComponent() の6種類ある。ただし、これらの仮想関数は通常オー
   * バーライドする必要はなく、使用は推奨されない。
   *
   * <pre>
   * public:
   *  RTC::ReturnCode_t add_component(RTC::LightweightRTObject_ptr comp)
   *  {
   *    return ExecutionContextBase::addComponent(comp);
   *  }
   *  RTC::ReturnCode_t remove_component(RTC::LightweightRTObject_ptr comp)
   *  {
   *    return ExecutionContextBase::removeComponent(comp);
   *  }
   * protected:
   *  virtual RTC::ReturnCode_t
   *  onAddingComponent(RTC::LightweightRTObject rtobj)
   *  {
   *    // コンポーネント追加時に実行したい処理を記述
   *    // RTC::RTC_OK 以外を返した場合、コンポーネントの追加は行われない。
   *    return RTC::RTC_OK;
   *  }
   *  virtual RTC::ReturnCode_t
   *  onAddedComponent(RTC::LightweightRTObject rtobj)
   *  {
   *    // コンポーネント追加時に実行したい処理を記述
   *    // RTC::RTC_OK 以外を返した場合、removeComponent() が呼び出され、
   *    // 追加されたコンポーネントが削除される。
   *    return RTC::RTC_OK;
   *  }
   *  virtual RTC::ReturnCode_t
   *  onRemovingComponent(RTC::LightweightRTObject rtobj)
   *  {
   *    // コンポーネント削除時に実行したい処理を記述
   *    // RTC::RTC_OK 以外を返した場合、コンポーネントの削除は行われない。
   *    return RTC::RTC_OK;
   *  }
   *  virtual RTC::ReturnCode_t
   *  onRemovedComponent(RTC::LightweightRTObject rtobj)
   *  {
   *    // コンポーネント追加時に実行したい処理を記述
   *    // RTC::RTC_OK 以外を返した場合、addComponent() が呼び出され、
   *    // 削除されたコンポーネントが再び追加される。
   *    return RTC::RTC_OK;
   *  }
   * </pre>
   *
   * @par 実行コンテキストの情報取得に関する関数
   * - get_component_state(): ExecutionContextBase::getComponentState()
   * - get_kind(): ExecutionContextBase::getKind()
   * - get_profile(): ExecutionContextBase::getProfile()
   *
   * 実行コンテキストの情報取得に関する関数は、get_component_state(),
   * get_kind(), get_profile() の3種類がある。実行コンテキストの実装ク
   * ラスにおいては、ExecutionContextBase のそれぞれ
   * getComponentState(), getKind(), getProfile() を呼び出す形で実装を
   * 行う。これらの関数に関連する protected 仮想関数は
   * onGetComponentState(), onGetKind(), onGetProfile() の3種類ある。こ
   * れらの仮想関数は通常オーバーライドする必要はなく、使用は推奨されな
   * い。ただし、返す情報を変更したい場合は、これらの関数を適切に実装す
   * ることで、呼び出し側に返す値を上書きすることができる。
   *
   * <pre>
   * public:
   *  LifeCycleState get_component_state(RTC::LightweightRTObject_ptr comp)
   *  {
   *    return getComponentState(comp);
   *  }
   *  ExecutionKind PeriodicExecutionContext::get_kind()
   *  {
   *    return getKind();
   *  }
   *  ExecutionContextProfile* get_profile()
   *  {
   *    return getProfile();
   *  }
   *
   * protected:
   *  virtual LifeCycleState onGetComponentState(LifeCycleState state)
   *  { // 返すstateを書き換えたい場合はこの関数を実装する
   *    return state;
   *  }
   *  virtual ExecutionKind onGetKind(ExecutionKind kind)
   *  { // 返すkindを書き換えたい場合はこの関数を実装する
   *    return kind;
   *  }
   *  virtual ExecutionContextProfile*
   *  onGetProfile(ExecutionContextProfile*& profile)
   *  { // 返すprofileを書き換えたい場合はこの関数を実装する
   *    return profile;
   *  }
   * </pre>
   *
   * [ExecutionContextBase]
   * <>
   *  |
   *  +--(1 1) [ExecutionContextWorker ] : EC の worker
   *  |         <>
   *  |          +--(1 0..) [RTObjectStateMachine] : StateMachine
   *  |
   *  +--(1 1) [ExecutionContextProfile] : EC Profileの管理
   *
   * @since 0.4.0
   *
   * @else
   * @class ExecutionContextBase
   * @brief A base class for ExecutionContext
   *
   * A base class of ExecutionContext.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class ExecutionContextBase
  {
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
    explicit ExecutionContextBase(const char* name);

    /*!
     * @if jp
     * @brief 仮想デストラクタ
     *
     * 仮想デストラクタ
     *
     * @else
     * @brief Virtual Destructor
     *
     * Virtual Destructor
     *
     * @endif
     */
    virtual ~ExecutionContextBase();

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
    virtual void init(coil::Properties& props);

    /*!
     * @if jp
     * @brief コンポーネントをバインドする。
     *
     * コンポーネントをバインドする。
     *
     * @else
     * @brief Bind the component.
     *
     * Bind the component.
     *
     * @endif
     */
    virtual RTC::ReturnCode_t bindComponent(RTC::RTObject_impl* rtc);

    //============================================================
    // implementation functions for EC's CORBA operation
    //============================================================
    /*!
     * @if jp
     * @brief ExecutionContext 実行状態確認関数
     *
     * この操作は ExecutionContext が Runnning 状態の場合に true を返す。
     * 返り値は、start() 関数が呼ばれたあとはRunning状態となり
     * true を、stop() 関数が呼ばれたあとはStopped状態となりfalseを返す。
     *
     * @return 動作状態 (Running状態:true、Stopped状態: false)
     *
     * @else
     *
     * @brief Check for ExecutionContext running state
     *
     * This operation shall return true if the context is in the
     * Running state.  After calling the start() of this class, this
     * function returns true, after calling to stop function of it,
     * this function returns false.
     *
     * @return Working status (Running:true、Stopped:false)
     *
     * @endif
     */
    CORBA::Boolean isRunning();

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
    RTC::ReturnCode_t start();

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
    virtual RTC::ReturnCode_t stop();

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
    double getRate() const;
    coil::TimeValue getPeriod() const;

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
     *         RTC_OK: 正常終了
     *         BAD_PARAMETER: 設定値が負の値
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
     *         RTC_OK: Succeed
     *         BAD_PARAMETER: Invalid value. The value might be negative.
     *
     * @endif
     */
    RTC::ReturnCode_t setRate(double rate);

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
    RTC::ReturnCode_t addComponent(RTC::LightweightRTObject_ptr comp);

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
    RTC::ReturnCode_t activateComponent(RTC::LightweightRTObject_ptr comp);

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
     *
     *


     * |



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
    RTC::ReturnCode_t deactivateComponent(RTC::LightweightRTObject_ptr comp);

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
    RTC::ReturnCode_t resetComponent(RTC::LightweightRTObject_ptr comp);

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
    const char* getStateString(RTC::LifeCycleState state);

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
    RTC::ExecutionKind getKind() const;

    /*!
     * @if jp
     * @brief Profileを取得する
     *
     * RTC::ExecutionContextProfile を取得する。取得した
     * ExecutionContextProfile の所有権は呼び出し側にある。取得されたオ
     * ブジェクトが不要になった場合、呼び出し側が開放する責任を負う。
     *
     * @return RTC::ExecutionContextProfile
     *
     * @else
     * @brief Getting Profile
     *
     * This function gets RTC::ExecutionContextProfile.  The ownership
     * of the obtained ExecutionContextProfile is given to caller. The
     * caller should release obtained object when it is unneccessary
     * anymore.
     *
     * @return RTC::ExecutionContextProfile
     *
     * @endif
     */
    RTC::ExecutionContextProfile* getProfile();

    //============================================================
    // Delegated functions to ExecutionContextProfile
    //============================================================
    /*!
     * @if jp
     * @brief CORBA オブジェクト参照の取得
     *
     * 本オブジェクトの ExecutioncontextService としての CORBA オブジェ
     * クト参照を取得する。
     *
     * @return CORBA オブジェクト参照
     *
     * @else
     * @brief Get the reference to the CORBA object
     *
     * Get the reference to the CORBA object as
     * ExecutioncontextService of this object.
     *
     * @return The reference to CORBA object
     *
     * @endif
     */
    void setObjRef(RTC::ExecutionContextService_ptr ec_ptr);

    /*!
     * @if jp
     * @brief CORBA オブジェクト参照の取得
     *
     * 本オブジェクトの ExecutioncontextService としての CORBA オブジェ
     * クト参照を取得する。
     *
     * @return CORBA オブジェクト参照
     *
     * @else
     *
     * Get the reference to the CORBA object as
     * ExecutioncontextService of this object.
     *
     * @return The reference to CORBA object
     *
     * @endif
     */
    RTC::ExecutionContextService_ptr getObjRef() const;

    /*!
     * @if jp
     * @brief ExecutionKind を文字列化する
     *
     * RTC::ExecutionKind で定義されている PERIODIC, EVENT_DRIVEN,
     * OTHER を文字列化する。
     *
     * @param kind ExecutionKind
     * @return 文字列化されたExecutionKind
     *
     * @else
     *
     * @brief Converting ExecutionKind enum to string
     *
     * This function converts enumeration (PERIODIC, EVENT_DRIVEN,
     * OTHER) defined in RTC::ExecutionKind to string.
     *
     * @param kind ExecutionKind
     * @return String of ExecutionKind
     *
     * @endif
     */
    const char* getKindString(RTC::ExecutionKind kind) const;

    /*!
     * @if jp
     * @brief ExecutionKind を設定する
     *
     * この ExecutionContext の ExecutionKind を設定する
     *
     * @param kind ExecutionKind
     *
     * @else
     *
     * @brief Set the ExecutionKind
     *
     * This operation sets the kind of the execution context.
     *
     * @param kind ExecutionKind
     *
     * @endif
     */
    RTC::ReturnCode_t setKind(RTC::ExecutionKind kind);

    /*!
     * @if jp
     * @brief Ownerコンポーネントをセットする。
     *
     * このECのOwnerとなるRTCをセットする。
     *
     * @param comp OwnerとなるRTコンポーネント
     * @return ReturnCode_t 型のリターンコード
     * @else
     * @brief Setting owner component of the execution context
     *
     * This function sets an RT-Component to be owner of the execution context.
     *
     * @param comp an owner RT-Component of this execution context
     * @return The return code of ReturnCode_t type
     * @endif
     */
    RTC::ReturnCode_t setOwner(RTC::LightweightRTObject_ptr comp);

    /*!
     * @if jp
     * @brief Ownerコンポーネントの参照を取得する
     *
     * このECのOwnerであるRTCの参照を取得する。
     *
     * @return OwnerRTコンポーネントの参照
     * @else
     * @brief Getting a reference of the owner component
     *
     * This function returns a reference of the owner RT-Component of
     * this execution context
     *
     * @return a reference of the owner RT-Component
     * @endif
     */
    RTC::RTObject_ptr getOwner() const;

    /*!
     * @if jp
     * @brief RTコンポーネントの参加者リストを取得する
     *
     * 現在登録されている参加者RTCのリストを取得する。
     *
     * @return 参加者RTCのリスト
     *
     * @else
     *
     * @brief Getting participant RTC list
     *
     * This function returns a list of participant RTC of the execution context.
     *
     * @return Participants RTC list
     *
     * @endif
     */
    const RTC::RTCList& getComponentList() const;

    /*!
     * @if jp
     * @brief Propertiesをセットする
     *
     * ExecutionContextProfile::properties をセットする。
     *
     * @param props ExecutionContextProfile::properties にセットするプ
     *              ロパティー
     *
     * @else
     * @brief Setting Properties
     *
     * This function sets ExecutionContextProfile::properties by
     * coil::Properties.
     *
     * @param props Properties to be set to
     *              ExecutionContextProfile::properties.
     *
     * @endif
     */
    void setProperties(coil::Properties& props);

    /*!
     * @if jp
     * @brief Propertiesを取得する
     *
     * ExecutionContextProfile::properties を取得する。
     *
     * @return coil::Propertiesに変換された
     *              ExecutionContextProfile::properties
     *
     * @else
     * @brief Setting Properties
     *
     * This function sets ExecutionContextProfile::properties by
     * coil::Properties.
     *
     * @param props Properties to be set to ExecutionContextProfile::properties.
     *
     * @endif
     */
    const coil::Properties getProperties() const;

    /*!
     * @if jp
     * @brief Profileを取得する
     *
     * RTC::ExecutionContextProfile を取得する。
     *
     * @return RTC::ExecutionContextProfile
     *
     * @else
     * @brief Getting Profile
     *
     * This function gets RTC::ExecutionContextProfile.
     *
     * @return RTC::ExecutionContextProfile
     *
     * @endif
     */
    const RTC::ExecutionContextProfile& getProfile() const;
    // end of delegated functions to ExecutionContextProfile
    //============================================================

    //============================================================
    // Delegated functions to ExecutionContextWorker
    //============================================================
    bool isAllCurrentState(RTC::LifeCycleState state)
    {
      return m_worker.isAllCurrentState(state);
    }
    bool isAllNextState(RTC::LifeCycleState state)
    {
      return m_worker.isAllNextState(state);
    }
    bool isOneOfCurrentState(RTC::LifeCycleState state)
    {
      return m_worker.isOneOfCurrentState(state);
    }
    bool isOneOfNextState(RTC::LifeCycleState state)
    {
      return m_worker.isOneOfNextState(state);
    }

    void invokeWorker()       { m_worker.invokeWorker(); }
    void invokeWorkerPreDo()  { m_worker.invokeWorkerPreDo(); }
    void invokeWorkerDo()     { m_worker.invokeWorkerDo(); }
    void invokeWorkerPostDo() { m_worker.invokeWorkerPostDo(); }


  protected:
    // template virtual functions related to start/stop
    virtual bool onIsRunning(bool running) { return running; }
    virtual RTC::ReturnCode_t onStarting() { return RTC::RTC_OK; }
    virtual RTC::ReturnCode_t onStarted() { return RTC::RTC_OK; }
    virtual RTC::ReturnCode_t onStopping() { return RTC::RTC_OK; }
    virtual RTC::ReturnCode_t onStopped() { return RTC::RTC_OK; }

    // template virtual functions getting/setting execution rate
    virtual double onGetRate(double rate) const { return rate; }
    virtual double onSettingRate(double rate) { return rate; }
    virtual RTC::ReturnCode_t onSetRate(double  /*rate*/) { return RTC::RTC_OK; }

    // template virtual functions adding/removing component
    virtual RTC::ReturnCode_t
    onAddingComponent(RTC::LightweightRTObject_ptr  /*rtobj*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onAddedComponent(RTC::LightweightRTObject_ptr  /*rtobj*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onRemovingComponent(RTC::LightweightRTObject_ptr  /*rtobj*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onRemovedComponent(RTC::LightweightRTObject_ptr  /*rtobj*/)
    {
      return RTC::RTC_OK;
    }

    // template virtual functions related to activation/deactivation/reset
    virtual RTC::ReturnCode_t
    onActivating(RTC::LightweightRTObject_ptr  /*comp*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onWaitingActivated(RTC_impl::RTObjectStateMachine*  /*comp*/, long int  /*count*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onActivated(RTC_impl::RTObjectStateMachine*  /*comp*/,
                                     long int  /*count*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onDeactivating(RTC::LightweightRTObject_ptr  /*comp*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onWaitingDeactivated(RTC_impl::RTObjectStateMachine*  /*comp*/, long int  /*count*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onDeactivated(RTC_impl::RTObjectStateMachine*  /*comp*/, long int  /*count*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t onResetting(RTC::LightweightRTObject_ptr  /*comp*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onWaitingReset(RTC_impl::RTObjectStateMachine*  /*comp*/, long int  /*count*/)
    {
      return RTC::RTC_OK;
    }
    virtual RTC::ReturnCode_t
    onReset(RTC_impl::RTObjectStateMachine*  /*comp*/, long int  /*count*/)
    {
      return RTC::RTC_OK;
    }

    virtual RTC::LifeCycleState
    onGetComponentState(RTC::LifeCycleState state)
    {
      return state;
    }
    virtual RTC::ExecutionKind
    onGetKind(RTC::ExecutionKind kind) const
    {
      return kind;
    }
    virtual RTC::ExecutionContextProfile*
    onGetProfile(RTC::ExecutionContextProfile*& profile)
    {
      return profile;
    }

  private:
    /*!
     * @if jp
     * @brief Propertiesから実行コンテキストをセットする
     * @else
     * @brief Setting execution rate from given properties.
     * @endif
     */
    bool setExecutionRate(coil::Properties& props);
    /*!
     * @if jp
     * @brief Propertiesから状態遷移モードをセットする
     * @else
     * @brief Setting state transition mode from given properties.
     * @endif
     */
    bool setTransitionMode(coil::Properties& props,
                           const char* key, bool& flag);
    /*!
     * @if jp
     * @brief Propertiesから状態遷移Timeoutをセットする
     * @else
     * @brief Setting state transition timeout from given properties.
     * @endif
     */
    bool setTimeout(coil::Properties& props, const char* key,
                    coil::TimeValue& timevalue);

    RTC::ReturnCode_t waitForActivated(RTC_impl::RTObjectStateMachine* rtobj);
    RTC::ReturnCode_t waitForDeactivated(RTC_impl::RTObjectStateMachine* rtobj);
    RTC::ReturnCode_t waitForReset(RTC_impl::RTObjectStateMachine* rtobj);

  protected:
    mutable RTC::Logger rtclog;

    RTC_impl::ExecutionContextWorker m_worker;
    RTC_impl::ExecutionContextProfile m_profile;

    coil::TimeValue m_activationTimeout;
    coil::TimeValue m_deactivationTimeout;
    coil::TimeValue m_resetTimeout;

    bool m_syncActivation;
    bool m_syncDeactivation;
    bool m_syncReset;
  };  // class ExecutionContextBase

  typedef coil::GlobalFactory<ExecutionContextBase> ExecutionContextFactory;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  EXTERN template class DLL_PLUGIN coil::GlobalFactory<ExecutionContextBase>;
#endif
} // namespace RTC

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif  // RTC_EXECUTIONCONTEXTBASE_H
