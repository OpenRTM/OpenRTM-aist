// -*- C++ -*-
/*!
 * @file RTObject.h
 * @brief RT component base class
 * @date $Date: 2008-01-14 07:57:18 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_RTOBJECT_H
#define RTC_RTOBJECT_H

// CORBA header include
#include <coil/Properties.h>

#include <rtm/RTC.h>
#include <rtm/idl/RTCSkel.h>
#include <rtm/idl/OpenRTMSkel.h>
#include <rtm/PortBase.h>
#include <rtm/PortAdmin.h>
#include <rtm/InPortBase.h>
#include <rtm/OutPortBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/ConfigAdmin.h>
#include <rtm/SystemLogger.h>
#include <rtm/ComponentActionListener.h>
#include <rtm/SdoServiceAdmin.h>
#include <rtm/PortConnectListener.h>
#include <rtm/FsmActionListener.h>

#include <string>
#include <vector>

#define ECOTHER_OFFSET 1000

namespace SDOPackage
{
  class Configuration_impl;
} // namespace SDOPackage

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC
{
  class Manager;
  class ExecutionContextBase;
  typedef ExecutionContextHandle_t UniqueId;

  /*!
   * @if jp
   * @brief RTコンポーネントクラス
   *
   * 各RTコンポーネントのベースとなるクラス。
   * Robotic Technology Component 仕様中の lightweightRTComponentの実装クラス。
   * コンポーネントの機能を提供する ComponentAction インターフェースと
   * コンポーネントのライフサイクル管理を行うための LightweightRTObject の実装を
   * 提供する。
   * 実際にユーザがコンポーネントを作成する場合には、Execution Semantics に対応
   * した各サブクラスを利用する。<BR>
   * (現状の実装では Periodic Sampled Data Processing のみサポートしているため、
   *  dataFlowComponent を直接継承している)
   *
   * @since 0.2.0
   *
   * @else
   * @brief RT-Component class
   *
   * This is a class to be a base of each RT-Component.
   * This is a implementation class of lightweightRTComponent in Robotic
   * Technology Component specification.
   * This provides a implementation of ComponentAction interface that offers
   * the the component's function and the implementation of LightweightRTObject
   * for management of the component's lifecycle.
   * When users actually create the components, they should use each subclass
   * corresponding to Execution Semantics.<BR>
   * (In current implementation, since only Periodic Sampled Data Processing is
   * supported, this class inherits dataFlowComponent directly.)
   *
   * @since 0.2.0
   *
   * @endif
   */
  class RTObject_impl
    : public virtual POA_OpenRTM::DataFlowComponent,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param manager マネージャオブジェクト
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param manager Manager object
     *
     * @endif
     */
    explicit RTObject_impl(Manager* manager);

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param orb ORB
     * @param poa POA
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param orb ORB
     * @param poa POA
     *
     * @endif
     */
    RTObject_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa);

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * @else
     *
     * @brief Virtual destructor
     *
     * @endif
     */
    ~RTObject_impl() override;

  protected:
    //============================================================
    // Overridden functions
    //============================================================
    /*!
     * @if jp
     *
     * @brief 初期化処理用コールバック関数
     *
     * ComponentAction::on_initialize が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の初期化処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to initialize
     *
     * This is a callback function that is executed when
     * ComponentAction::on_initialize was invoked.<BR>
     * As for actual initialization of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The initialize action (on CREATED->ALIVE transition)
    // formaer rtc_init_entry()
    virtual ReturnCode_t onInitialize();

    /*!
     * @if jp
     *
     * @brief 終了処理用コールバック関数
     *
     * ComponentAction::on_finalize が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の終了処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to finalize
     *
     * This is a callback function that is executed when
     * ComponentAction::on_finalize was invoked.<BR>
     * As for actual finalization of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The finalize action (on ALIVE->END transition)
    // formaer rtc_exiting_entry()
    virtual ReturnCode_t onFinalize();

    /*!
     * @if jp
     *
     * @brief 開始処理用コールバック関数
     *
     * ComponentAction::on_startup が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の開始処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function for startup action
     *
     * Callback function that is executed when
     * ComponentAction::on_startup was invoked.<BR>
     * As for actual startup of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The startup action when ExecutionContext startup
    // former rtc_starting_entry()
    virtual ReturnCode_t onStartup(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 停止処理用コールバック関数
     *
     * ComponentAction::on_shutdown が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の停止処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function for shutdown action
     *
     * Callback function that is executed when
     * ComponentAction::on_shutdown was invoked.<BR>
     * As for actual shutdown of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The shutdown action when ExecutionContext stop
    // former rtc_stopping_entry()
    virtual ReturnCode_t onShutdown(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 活性化処理用コールバック関数
     *
     * ComponentAction::on_activated が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の活性化処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to activate
     *
     * This is a callback function that is executed when
     * ComponentAction::on_activated was invoked.<BR>
     * As for actual activation of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The activated action (Active state entry action)
    // former rtc_active_entry()
    virtual ReturnCode_t onActivated(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 非活性化処理用コールバック関数
     *
     * ComponentAction::on_deactivated が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているの
     * で、各コンポーネントの実際の非活性化処理は、本関数をオーバーライ
     * ドして実装する必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to deactivate
     *
     * This is a callback function that is executed when
     * ComponentAction::on_deactivated was invoked.<BR>
     * As for actual deactivation of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The deactivated action (Active state exit action)
    // former rtc_active_exit()
    virtual ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 周期処理用コールバック関数
     *
     * DataFlowComponentAction::on_execute が呼ばれた際に実行される
     * コールバック関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の周期処理は、本関数をオーバーライドして実装する
     * 必要がある。<BR>
     * 本関数は Periodic Sampled Data Processing における Two-Pass Executionの
     * １回目の実行パスとして定期的に呼び出される。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to execute periodically
     *
     * This is a callback function that is executed when
     * DataFlowComponentAction::on_execute is invoked.<BR>
     * As for actual periodic execution of each component, since this
     * function is dummy-implemented to return RTC::RTC_OK
     * unconditionally, you need to implement this function by
     * overriding it.  This function is invoked periodically as the
     * first execution pass of Two-Pass Execution in Periodic Sampled
     * Data Processing.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual ReturnCode_t onExecute(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 中断処理用コールバック関数
     *
     * ComponentAction::on_aborting が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の中断処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to abort
     *
     * This is a callback function that is executed when
     * ComponentAction::on_aborting was invoked.<BR>
     * As for actual abortion of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The aborting action when main logic error occurred.
    // former rtc_aborting_entry()
    virtual ReturnCode_t onAborting(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief エラー処理用コールバック関数
     *
     * ComponentAction::on_error が呼ばれた際に実行されるコールバック関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際のエラー処理は、本関数をオーバーライドして実装する
     * 必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function for error handling
     *
     * This is a callback function that is executed when
     * ComponentAction::on_error was invoked.<BR>
     * As for actual error handling of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The error action in ERROR state
    // former rtc_error_do()
    virtual ReturnCode_t onError(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief リセット処理用コールバック関数
     *
     * ComponentAction::on_reset が呼ばれた際に実行されるコールバック関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際のリセット処理は、本関数をオーバーライドし
     * て実装する必要がある。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to reset
     *
     * This is a callback function that is executed when
     * ComponentAction::on_reset was invoked.<BR>
     * As for actual reset of each component, since this function is
     * dummy-implemented to return RTC::RTC_OK unconditionally, you need to
     * implement this function by overriding it.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The reset action that is invoked resetting
    // This is same but different the former rtc_init_entry()
    virtual ReturnCode_t onReset(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 状態変更処理用コールバック関数
     *
     * DataFlowComponentAction::on_state_update が呼ばれた際に実行される
     * コールバック関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の状態変更処理は、本関数をオーバーライドし
     * て実装する必要がある。<BR> 本関数は Periodic Sampled Data
     * Processing における Two-Pass Executionの２回目の実行パスとして定
     * 期的に呼び出される。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to update the state
     *
     * This is a callback function that is executed when
     * DataFlowComponentAction::on_state_update was invoked.<BR>
     * As for actual updating the state of each component, since this
     * function is dummy-implemented to return RTC::RTC_OK
     * unconditionally, you need to implement this function by
     * overriding it.<BR> This function is invoked periodically as the
     * second execution pass of Two-Pass Execution in Periodic Sampled
     * Data Processing.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The state update action that is invoked after onExecute() action
    // no corresponding operation exists in OpenRTm-aist-0.2.0
    virtual ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief 動作周期変更通知用コールバック関数
     *
     * DataFlowComponentAction::on_rate_changed が呼ばれた際に実行される
     * コールバック関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の状態変更処理は、本関数をオーバーライドし
     * て実装する必要がある。<BR> 本関数は Periodic Sampled Data
     * Processing において ExecutionContext の実行が更新された際に呼び
     * 出される。
     *
     * @param exec_handle 参加している ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief Callback function to change execution cycle
     *
     * This is a callback function that is executed when
     * DataFlowComponentAction::on_rate_changed was invoked.<BR>
     * As for actual changing execution cycle of each component, since this
     * function is dummy-implemented to return RTC::RTC_OK unconditionally,
     * you need to implement this function by overriding it.<BR>
     * This function is invoked when the execution of ExecutionContext
     * was updated in Periodic Sampled Data Processing.
     *
     * @param exec_handle ID of the participant ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    // The action that is invoked when execution context's rate is changed
    // no corresponding operation exists in OpenRTm-aist-0.2.0
    virtual ReturnCode_t onRateChanged(RTC::UniqueId ec_id);

  public:
    //============================================================
    // RTC::LightweightRTObject
    //============================================================
    /*!
     * @if jp
     *
     * @brief [CORBA interface] RTCを初期化する
     *
     * このオペレーション呼び出しの結果として、ComponentAction::on_initialize
     * コールバック関数が呼ばれる。
     *
     * 制約
     * - RTC は Created状態の場合み初期化が行われる。他の状態にいる場合には
     *   ReturnCode_t::PRECONDITION_NOT_MET が返され呼び出しは失敗する。
     * - このオペレーションは RTC のミドルウエアから呼ばれることを想定しており、
     *   アプリケーション開発者は直接このオペレーションを呼ぶことは想定
     *   されていない。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Initialize the RTC that realizes this interface.
     *
     * The invocation of this operation shall result in the invocation of the
     * callback ComponentAction::on_initialize.
     *
     * Constraints
     * - An RTC may be initialized only while it is in the Created state. Any
     *   attempt to invoke this operation while in another state shall fail
     *   with ReturnCode_t::PRECONDITION_NOT_MET.
     * - Application developers are not expected to call this operation
     *   directly; it exists for use by the RTC infrastructure.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t initialize()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] RTC を終了する
     *
     * このオペレーション呼び出しの結果として ComponentAction::on_finalize()
     * を呼び出す。
     *
     * 制約
     * - RTC が ExecutionContext に所属している間は終了されない。この場合は、
     *   まず最初に ExecutionContextOperations::remove によって参加を
     *   解除しなければならない。これ以外の場合は、このオペレーション呼び出しは
     *   いかなる場合も ReturnCode_t::PRECONDITION_NOT_ME で失敗する。
     * - RTC が Created 状態である場合、終了処理は行われない。
     *   この場合、このオペレーション呼び出しはいかなる場合も
     *   ReturnCode_t::PRECONDITION_NOT_MET で失敗する。
     * - このオペレーションはRTCのミドルウエアから呼ばれることを想定しており、
     *   アプリケーション開発者は直接このオペレーションを呼ぶことは想定
     *   されていない。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface] Finalize the RTC for destruction
     *
     * This invocation of this operation shall result in the invocation of the
     * callback ComponentAction::on_finalize.
     *
     * Constraints
     * - An RTC may not be finalized while it is participating in any execution
     *   context. It must first be removed with
     *   ExecutionContextOperations::remove. Otherwise, this operation
     *   shall fail with ReturnCode_t::PRECONDITION_NOT_MET.
     * - An RTC may not be finalized while it is in the Created state. Any
     *   attempt to invoke this operation while in that state shall fail with
     *   ReturnCode_t::PRECONDITION_NOT_MET.
     * - Application developers are not expected to call this
     *   operation directly; it exists for use by the RTC
     *   infrastructure.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t finalize()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] RTC がオーナーである ExecutionContext を
     *        停止させ、そのコンテンツと共に終了させる
     *
     * この RTC がオーナーであるすべての実行コンテキストを停止する。
     * この RTC が他の実行コンテキストを所有する RTC に属する実行コンテキスト
     * (i.e. 実行コンテキストを所有する RTC はすなわちその実行コンテキストの
     * オーナーである。)に参加している場合、当該 RTC はそれらのコンテキスト上
     * で非活性化されなければならない。
     * RTC が実行中のどの ExecutionContext でも Active 状態ではなくなった後、
     * この RTC とこれに含まれる RTC が終了する。
     *
     * 制約
     * - RTC が初期化されていなければ、終了させることはできない。
     *   Created 状態にある RTC に exit() を呼び出した場合、
     *   ReturnCode_t::PRECONDITION_NOT_MET で失敗する。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [CORBA interface]top the RTC's execution context(s) and finalize
     *        it along with its contents.
     *
     * Any execution contexts for which the RTC is the owner shall be stopped.
     * If the RTC participates in any execution contexts belonging to another
     * RTC that contains it, directly or indirectly (i.e. the containing RTC
     * is the owner of the ExecutionContext), it shall be deactivated in those
     * contexts.
     * After the RTC is no longer Active in any Running execution context, it
     * and any RTCs contained transitively within it shall be finalized.
     *
     * Constraints
     * - An RTC cannot be exited if it has not yet been initialized. Any
     *   attempt to exit an RTC that is in the Created state shall fail with
     *   ReturnCode_t::PRECONDITION_NOT_MET.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t exit()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] RTC が Alive 状態であるかどうか確認する。
     *
     * RTC が指定した ExecutionContext に対して Alive状態であるかどうか
     * 確認する。RTC の状態が Active であるか、Inactive であるか、
     * Error であるかは実行中のExecutionContext に依存する。すなわち、
     * ある ExecutionContext に対してはActive 状態であっても、他の
     * ExecutionContext に対しては Inactive 状態となる場合もありえる。
     * 従って、このオペレーションは指定されたExecutionContext に問い合
     * わせて、この RTC の状態が Active、Inactive、Error の場合には
     * Alive 状態として返す。
     *
     * @return Alive 状態確認結果
     *
     * @else
     *
     * @brief [CORBA interface] Confirm whether RTC is the alive state
     *
     * A component is alive or not regardless of the execution context from
     * which it is observed. However, whether or not it is Active, Inactive,
     * or in Error is dependent on the execution context(s) in which it is
     * running. That is, it may be Active in one context but Inactive in
     * another. Therefore, this operation shall report whether this RTC is
     * either Active, Inactive or in Error; which of those states a component
     * is in with respect to a particular context may be queried from the
     * context itself.
     *
     * @return Result of Alive state confirmation
     *
     * @endif
     */
    CORBA::Boolean is_alive(ExecutionContext_ptr exec_context)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief [CORBA interface] ExecutionContextを取得する
     *
     * 指定したハンドルの ExecutionContext を取得する。
     * ハンドルから ExecutionContext へのマッピングは、特定の RTC インスタンスに
     * 固有である。ハンドルはこの RTC を attach_context した際に取得できる。
     *
     * @param exec_handle 取得対象 ExecutionContext ハンドル
     *
     * @return ExecutionContext
     *
     * @else
     * @brief [CORBA interface] Get ExecutionContext.
     *
     * Obtain a reference to the execution context represented by the given
     * handle.
     * The mapping from handle to context is specific to a particular RTC
     * instance. The given handle must have been obtained by a previous call to
     * attach_context on this RTC.
     *
     * @param exec_handle ExecutionContext handle
     *
     * @return ExecutionContext
     *
     * @endif
     */
    ExecutionContext_ptr get_context(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief [CORBA interface] 所有する ExecutionContextListを 取得する
     *
     * この RTC が所有する ExecutionContext のリストを取得する。
     *
     * @return ExecutionContext リスト
     *
     * @else
     * @brief [CORBA interface] Get ExecutionContextList.
     *
     * This operation returns a list of all execution contexts owned by this
     * RTC.
     *
     * @return ExecutionContext List
     *
     * @endif
     */
    ExecutionContextList* get_owned_contexts()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief [CORBA interface] 参加している ExecutionContextList を取得する
     *
     * この RTC が参加している ExecutionContext のリストを取得する。
     *
     * @return ExecutionContext リスト
     *
     * @else
     * @brief [CORBA interface] Get participating ExecutionContextList.
     *
     * This operation returns a list of all execution contexts in
     * which this RTC participates.
     *
     * @return ExecutionContext List
     *
     * @endif
     */
    ExecutionContextList* get_participating_contexts()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief [CORBA interface] ExecutionContext のハンドルを返す
     *
     * 与えられた実行コンテキストに関連付けられたハンドルを返す。
     *
     * @else
     * @brief [CORBA interface] Return a handle of a ExecutionContext
     *
     * This operation returns a handle that is associated with the given
     * execution context.
     *
     * @endif
     */
    ExecutionContextHandle_t
    get_context_handle(ExecutionContext_ptr cxt)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     * @brief [CORBA interface] ExecutionContextをattachする
     *
     * 指定した ExecutionContext にこの RTC を所属させる。この RTC と関連する
     * ExecutionContext のハンドルを返す。
     * このオペレーションは、ExecutionContextOperations::add_component
     * が呼ばれた際に呼び出される。返されたハンドルは他のクライアントで
     * 使用することを想定していない。
     *
     * @param exec_context 所属先 ExecutionContext
     *
     * @return ExecutionContext ハンドル
     *
     * @else
     * @brief [CORBA interface] Attach ExecutionContext
     *
     * Inform this RTC that it is participating in the given execution context.
     * Return a handle that represents the association of this RTC with the
     * context.
     * This operation is intended to be invoked by
     * ExecutionContextOperations::add_component. It is not intended for use by
     * other clients.
     *
     * @param exec_context Participating ExecutionContext
     *
     * @return ExecutionContext Handle
     *
     * @endif
     */
    UniqueId attach_context(ExecutionContext_ptr exec_context)
      throw (CORBA::SystemException) override;

    UniqueId bindContext(ExecutionContext_ptr exec_context);

    /*!
     * @if jp
     * @brief [CORBA interface] ExecutionContextをdetachする
     *
     * 指定した ExecutionContext からこの RTC の所属を解除する。
     * このオペレーションは、ExecutionContextOperations::remove が呼ば
     * れた際に呼び出される。返されたハンドルは他のクライアントで使用することを
     * 想定していない。
     *
     * 制約
     * - 指定された ExecutionContext に RTC がすでに所属していない場合には、
     *   ReturnCode_t::PRECONDITION_NOT_MET が返される。
     * - 指定された ExecutionContext にたしいて対して RTC がActive 状態である場
     *   合には、 ReturnCode_t::PRECONDITION_NOT_MET が返される。
     *
     * @param exec_handle 解除対象 ExecutionContextハンドル
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     * @brief [CORBA interface] Detach ExecutionContext.
     *
     * Inform this RTC that it is no longer participating in the given
     * execution context.
     * This operation is intended to be invoked by
     * ExecutionContextOperations::remove. It is not intended for use
     * by other clients.
     * Constraints
     * - This operation may not be invoked if this RTC is not already
     *   participating in the execution context. Such a call shall fail with
     *   ReturnCode_t::PRECONDITION_NOT_MET.
     * - This operation may not be invoked if this RTC is Active in
     *   the indicated execution context. Otherwise, it shall fail
     *   with ReturnCode_t::PRECONDITION_NOT_MET.
     *
     * @param exec_handle Detaching ExecutionContext Handle
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t detach_context(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    //============================================================
    // RTC::RTObject
    //============================================================
    /*!
     * @if jp
     *
     * @brief [RTObject CORBA interface] コンポーネントプロファイルを取得する
     *
     * 当該コンポーネントのプロファイル情報を返す。
     *
     * @return コンポーネントプロファイル
     *
     * @else
     *
     * @brief [RTObject CORBA interface] Get RTC's profile
     *
     * This operation returns the ComponentProfile of the RTC.
     *
     * @return ComponentProfile
     *
     * @endif
     */
    ComponentProfile* get_component_profile()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [RTObject CORBA interface] ポートを取得する
     *
     * 当該コンポーネントが保有するポートの参照を返す。
     *
     * @return ポートリスト
     *
     * @else
     *
     * @brief [RTObject CORBA interface] Get Ports
     *
     * This operation returns the reference of ports held by RTC.
     *
     * @return PortServiceList
     *
     * @endif
     */
    PortServiceList* get_ports()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [RTObject CORBA interface] ExecutionContextAdmin を取得する
     *
     * このオペレーションは当該 RTC が所属する ExecutionContextに関連した
     * ExecutionContextService のリストを返す。
     *
     * @return ExecutionContextService リスト
     *
     * @else
     *
     * @brief [RTObject CORBA interface] Get ExecutionContextAdmin
     *
     * This operation returns a list containing an ExecutionContextAdmin for
     * every ExecutionContext owned by the RTC.
     *
     * @return ExecutionContextService List
     *
     * @endif
     */
    //    virtual ExecutionContextServiceList* get_execution_context_services()
    //      throw (CORBA::SystemException);

    //============================================================
    // RTC::ComponentAction
    //============================================================
    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC の初期化
     *
     * RTC が初期化され、Alive 状態に遷移する。
     * RTC 固有の初期化処理はここで実行する。
     * このオペレーション呼び出しの結果として onInitialize() コールバック関数が
     * 呼び出される。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Initialize RTC
     *
     * The RTC has been initialized and entered the Alive state.
     * Any RTC-specific initialization logic should be performed here.
     * As a result of this operation, onInitialize() callback function
     * is called.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_initialize()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC の終了
     *
     * RTC が破棄される。
     * RTC 固有の終了処理はここで実行する。
     * このオペレーション呼び出しの結果として onFinalize() コールバック関数が
     * 呼び出される。
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Finalize RTC
     *
     * The RTC is being destroyed.
     * Any final RTC-specific tear-down logic should be performed here.
     * As a result of this operation, onFinalize() callback function is called.
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_finalize()
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC の開始
     *
     * RTC が所属する ExecutionContext が Stopped 状態から Running 状態へ遷移
     * した場合に呼び出される。
     * このオペレーション呼び出しの結果として onStartup() コールバック関数が
     * 呼び出される。
     *
     * @param exec_handle 状態遷移した ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Startup RTC
     *
     * The given execution context, in which the RTC is participating, has
     * transitioned from Stopped to Running.
     * As a result of this operation, onStartup() callback function is called.
     *
     * @param exec_handle ID of ExecutionContext that transited to the state
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_startup(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC の停止
     *
     * RTC が所属する ExecutionContext が Running 状態から Stopped 状態へ遷移
     * した場合に呼び出される。
     * このオペレーション呼び出しの結果として onShutdown() コールバック関数が
     * 呼び出される。
     *
     * @param exec_handle 状態遷移した ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Shutdown RTC
     *
     * The given execution context, in which the RTC is participating, has
     * transitioned from Running to Stopped.
     * As a result of this operation, onShutdown() callback function is called.
     *
     * @param exec_handle ID of ExecutionContext that transited to the state
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_shutdown(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC の活性化
     *
     * 所属する ExecutionContext から RTC が活性化された際に呼び出される。
     * このオペレーション呼び出しの結果として onActivated() コールバック関数が
     * 呼び出される。
     *
     * @param exec_handle 活性化 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Activate RTC
     *
     * The RTC has been activated in the given execution context.
     * As a result of this operation, onActivated() callback function is called.
     *
     * @param exec_handle ID of activation ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_activated(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC の非活性化
     *
     * 所属する ExecutionContext から RTC が非活性化された際に呼び出される。
     * このオペレーション呼び出しの結果として onDeactivated() コールバック関数が
     * 呼び出される。
     *
     * @param exec_handle 非活性化 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Deactivate RTC
     *
     * The RTC has been deactivated in the given execution context.
     * As a result of this operation, onDeactivated() callback function
     * is called.
     *
     * @param exec_handle ID of deactivation ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_deactivated(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC のエラー状態への遷移
     *
     * RTC が所属する ExecutionContext が Active 状態から Error 状態へ遷移した
     * 場合に呼び出される。
     * このオペレーションは RTC が Error 状態に遷移した際に一度だけ呼び
     * 出される。このオペレーション呼び出しの結果として onAborting() コー
     * ルバック関数が呼び出される。
     *
     * @param exec_handle 状態遷移した ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Transition to Error State
     *
     * The RTC is transitioning from the Active state to the Error state in some
     * execution context.
     * This callback is invoked only a single time for time that the RTC
     * transitions into the Error state from another state. This behavior is in
     * contrast to that of on_error.
     * As a result of this operation, onAborting() callback function is invoked.
     *
     * @param exec_handle ID of ExecutionContext that transited to the state
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_aborting(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC のエラー処理
     *
     * RTC がエラー状態にいる際に呼び出される。
     * RTC がエラー状態の場合に、対象となる ExecutionContext のExecutionKind に
     * 応じたタイミングで呼び出される。例えば、
     * - ExecutionKind が PERIODIC の場合、本オペレーションは
     *   DataFlowComponentAction::on_execute と on_state_update の替わりに、
     *   設定された順番、設定された周期で呼び出される。
     * - ExecutionKind が EVENT_DRIVEN の場合、本オペレーションは
     *   FsmParticipantAction::on_action が呼ばれた際に、替わりに呼び出される。
     * このオペレーション呼び出しの結果として onError() コールバック関数が呼び出
     * される。
     *
     * @param exec_handle 対象 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Error Processing of RTC
     *
     * The RTC remains in the Error state.
     * If the RTC is in the Error state relative to some execution context when
     * it would otherwise be invoked from that context (according to the
     * context’s ExecutionKind), this callback shall be invoked instead.
     * For example,
     * - If the ExecutionKind is PERIODIC, this operation shall be invoked in
     *   sorted order at the rate of the context instead of
     *   DataFlowComponentAction::on_execute and on_state_update.
     * - If the ExecutionKind is EVENT_DRIVEN, this operation shall be invoked
     *   whenever FsmParticipantAction::on_action would otherwise have been
     *   invoked.
     * As a result of this operation, onError() callback function is invoked.
     *
     * @param exec_handle ID of target ExecutionContext
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_error(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [ComponentAction CORBA interface] RTC のリセット
     *
     * Error 状態にある RTC のリカバリ処理を実行し、Inactive 状態に復帰させる
     * 場合に呼び出される。
     * RTC のリカバリ処理が成功した場合は Inactive 状態に復帰するが、それ以外の
     * 場合には Error 状態に留まる。
     * このオペレーション呼び出しの結果として onReset() コールバック関数が呼び
     * 出される。
     *
     * @param exec_handle リセット対象 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [ComponentAction CORBA interface] Resetting RTC
     *
     * The RTC is in the Error state. An attempt is being made to
     * recover it such that it can return to the Inactive state.
     * If the RTC was successfully recovered and can safely return to
     * the Inactive state, this method shall complete with
     * ReturnCode_t::OK. Any other result shall indicate that the RTC
     * should remain in the Error state.  As a result of this
     * operation, onReset() callback function is invoked.
     *
     * @param exec_handle ID of target ExecutionContext for the reset
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_reset(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    //============================================================
    // RTC::DataFlowComponentAction
    //============================================================
    /*!
     * @if jp
     *
     * @brief [DataFlowComponentAction CORBA interface] RTC の定常処理(第一周期)
     *
     * 以下の状態が保持されている場合に、設定された周期で定期的に呼び出される。
     * - RTC は Alive 状態である。
     * - 指定された ExecutionContext が Running 状態である。
     * 本オペレーションは、Two-Pass Execution の第一周期で実行される。
     * このオペレーション呼び出しの結果として onExecute() コールバック関数が呼び
     * 出される。
     *
     * 制約
     * - 指定された ExecutionContext の ExecutionKind は、 PERIODIC でなければな
     *   らない
     *
     * @param exec_handle 定常処理対象 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [DataFlowComponentAction CORBA interface] Primary Periodic
     *        Operation of RTC
     *
     * This operation will be invoked periodically at the rate of the given
     * execution context as long as the following conditions hold:
     * - The RTC is Active.
     * - The given execution context is Running
     * This callback occurs during the first execution pass.
     * As a result of this operation, onExecute() callback function is invoked.
     *
     * Constraints
     * - The execution context of the given context shall be PERIODIC.
     *
     * @param exec_handle ID of target ExecutionContext for Primary
     *                    Periodic Operation
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_execute(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [DataFlowComponentAction CORBA interface] RTC の定常処理(第二周期)
     *
     * 以下の状態が保持されている場合に、設定された周期で定期的に呼び出される。
     * - RTC は Alive 状態である。
     * - 指定された ExecutionContext が Running 状態である。
     * 本オペレーションは、Two-Pass Execution の第二周期で実行される。
     * このオペレーション呼び出しの結果として onStateUpdate() コールバック関数が
     * 呼び出される。
     *
     * 制約
     * - 指定された ExecutionContext の ExecutionKind は、 PERIODIC でなければな
     *   らない
     *
     * @param exec_handle 定常処理対象 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [DataFlowComponentAction CORBA interface] Secondary Periodic
     *        Operation of RTC
     *
     * This operation will be invoked periodically at the rate of the given
     * execution context as long as the following conditions hold:
     * - The RTC is Active.
     * - The given execution context is Running
     * This callback occurs during the second execution pass.
     * As a result of this operation, onStateUpdate() callback function is
     * invoked.
     *
     * Constraints
     * - The execution context of the given context shall be PERIODIC.
     *
     * @param exec_handle ID of target ExecutionContext for
     *              Secondary Periodic Operation
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_state_update(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    /*!
     * @if jp
     *
     * @brief [DataFlowComponentAction CORBA interface] 実行周期変更通知
     *
     * 本オペレーションは、ExecutionContext の実行周期が変更されたことを通知する
     * 際に呼び出される。
     * このオペレーション呼び出しの結果として onRateChanged() コールバック関数が
     * 呼び出される。
     *
     * 制約
     * - 指定された ExecutionContext の ExecutionKind は、 PERIODIC でなければな
     *   らない
     *
     * @param exec_handle 定常処理対象 ExecutionContext の ID
     *
     * @return ReturnCode_t 型のリターンコード
     *
     * @else
     *
     * @brief [DataFlowComponentAction CORBA interface] Notify rate changed
     *
     * This operation is a notification that the rate of the indicated execution
     * context has changed.
     * As a result of this operation, onRateChanged() callback function is
     * called.
     *
     * Constraints
     * - The execution context of the given context shall be PERIODIC.
     *
     * @param exec_handle ID of target ExecutionContext for Operation
     *
     * @return The return code of ReturnCode_t type
     *
     * @endif
     */
    ReturnCode_t on_rate_changed(UniqueId ec_id)
      throw (CORBA::SystemException) override;

    //============================================================
    // SDOPackage::SdoSystemElement
    //============================================================
    /*!
     * @if jp
     *
     * @brief [SDO interface] Organization リストの取得
     *
     * SDOSystemElement は0個もしくはそれ以上の Organization を所有することが
     * 出来る。 SDOSystemElement が1つ以上の Organization を所有している場合
     * には、このオペレーションは所有する Organization のリストを返す。
     * もしOrganizationを一つも所有していないければ空のリストを返す。
     *
     * @return 所有している Organization リスト
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get Organization list
     *
     * SDOSystemElement can be the owner of zero or more organizations.
     * If the SDOSystemElement owns one or more Organizations, this operation
     * returns the list of Organizations that the SDOSystemElement owns.
     * If it does not own any Organization, it returns empty list.
     *
     * @return Owned Organization List
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    SDOPackage::OrganizationList* get_owned_organizations()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    //============================================================
    // SDOPackage::SDO
    //============================================================
    /*!
     * @if jp
     *
     * @brief [SDO interface] SDO ID の取得
     *
     * SDO ID を返すオペレーション。
     * このオペレーションは以下の型の例外を発生させる。
     *
     * @return    リソースデータモデルで定義されている SDO の ID
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get the SDO ID
     *
     * This operation returns id of the SDO.
     * This operation throws SDOException with one of the following types.
     *
     * @return    id of the SDO defined in the resource data model.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    char* get_sdo_id()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] SDO タイプの取得
     *
     * SDO Type を返すオペレーション。
     * このオペレーションは以下の型の例外を発生させる。
     *
     * @return    リソースデータモデルで定義されている SDO の Type
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get SDO type
     *
     * This operation returns sdoType of the SDO.
     * This operation throws SDOException with one of the following types.
     *
     * @return    Type of the SDO defined in the resource data model.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    char* get_sdo_type()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] SDO DeviceProfile リストの取得
     *
     * SDO の DeviceProfile を返すオペレーション。 SDO がハードウエアデバイス
     * に関連付けられていない場合には、空の DeviceProfile が返される。
     * このオペレーションは以下の型の例外を発生させる。
     *
     * @return    SDO DeviceProfile
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get SDO DeviceProfile list
     *
     * This operation returns the DeviceProfile of the SDO. If the SDO does not
     * represent any hardware device, then a DeviceProfile with empty values
     * are returned.
     * This operation throws SDOException with one of the following types.
     *
     * @return    The DeviceProfile of the SDO.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    SDOPackage::DeviceProfile* get_device_profile()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] SDO ServiceProfile の取得
     *
     * SDO が所有している Service の ServiceProfile を返すオペレーション。
     * SDO がサービスを一つも所有していない場合には、空のリストを返す。
     * このオペレーションは以下の型の例外を発生させる。
     *
     * @return    SDO が提供する全ての Service の ServiceProfile。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get SDO ServiceProfile
     *
     * This operation returns a list of ServiceProfiles that the SDO has.
     * If the SDO does not provide any service, then an empty list is returned.
     * This operation throws SDOException with one of the following types.
     *
     * @return    List of ServiceProfiles of all the services the SDO is
     *            providing.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    SDOPackage::ServiceProfileList* get_service_profiles()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] 特定のServiceProfileの取得
     *
     * 引数 "id" で指定された名前のサービスの ServiceProfile を返す。
     *
     * @param     id SDO Service の ServiceProfile に関連付けられた識別子。
     *
     * @return    指定された SDO Service の ServiceProfile。
     *
     * @exception InvalidParameter "id" で指定した ServiceProfile が存在しない。
     *                             "id" が null。
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get specified ServiceProfile
     *
     * This operation returns the ServiceProfile that is specified by the
     * argument "id."
     *
     * @param     id The identifier referring to one of the ServiceProfiles.
     *
     * @return    The profile of the specified service.
     *
     * @exception InvalidParameter The ServiceProfile that is specified by
     *                             the argument 'id' does not exist or if 'id'
     *                             is 'null.'
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    SDOPackage::ServiceProfile* get_service_profile(const char* id)
      throw (CORBA::SystemException,
             SDOPackage::InvalidParameter, SDOPackage::NotAvailable,
             SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] 指定された SDO Service の取得
     *
     * このオペレーションは引数 "id" で指定された名前によって区別される
     * SDO の Service へのオブジェクト参照を返す。 SDO により提供される
     * Service はそれぞれ一意の識別子により区別される。
     *
     * @param id SDO Service に関連付けられた識別子。
     *
     * @return 要求された SDO Service への参照。
     *
     *
     * @exception InvalidParameter "id" で指定した ServiceProfile が存在しない。
     *                             "id" が null。
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get specified SDO Service's reference
     *
     * This operation returns an object implementing an SDO's service that
     * is identified by the identifier specified as an argument. Different
     * services provided by an SDO are distinguished with different
     * identifiers. See OMG SDO specification Section 2.2.8, "ServiceProfile,"
     * on page 2-12 for more details.
     *
     * @param id The identifier referring to one of the SDO Service
     *
     * @return The reference requested to SDO Service.
     *
     * @exception InvalidParameter Argument “id” is null, or if the
     *                             ServiceProfile that is specified by argument
     *                            “id” does not exist.
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    SDOPackage::SDOService_ptr get_sdo_service(const char* id)
      throw (CORBA::SystemException,
             SDOPackage::InvalidParameter, SDOPackage::NotAvailable,
             SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] Configuration オブジェクトの取得
     *
     * このオペレーションは Configuration interface への参照を返す。
     * Configuration interface は各 SDO を管理するためのインターフェースの
     * ひとつである。このインターフェースは DeviceProfile, ServiceProfile,
     * Organization で定義された SDO の属性値を設定するために使用される。
     * Configuration インターフェースの詳細については、OMG SDO specification
     * の 2.3.5節, p.2-24 を参照のこと。
     *
     * @return SDO の Configuration インターフェースへの参照
     *
     * @exception InterfaceNotImplemented SDOはConfigurationインターフェースを
     *                                    持たない。
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get Configuration object
     *
     * This operation returns an object implementing the Configuration
     * interface. The Configuration interface is one of the interfaces that
     * each SDO maintains. The interface is used to configure the attributes
     * defined in DeviceProfile, ServiceProfile, and Organization.
     * See OMG SDO specification Section 2.3.5, "Configuration Interface,"
     * on page 2-24 for more details about the Configuration interface.
     *
     * @return The Configuration interface of an SDO.
     *
     * @exception InterfaceNotImplemented The target SDO has no Configuration
     *                                    interface.
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    SDOPackage::Configuration_ptr get_configuration()
      throw (CORBA::SystemException,
             SDOPackage::InterfaceNotImplemented, SDOPackage::NotAvailable,
             SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] Monitoring オブジェクトの取得
     *
     * このオペレーションは Monitoring interface への参照を返す。
     * Monitoring interface は SDO が管理するインターフェースの一つである。
     * このインターフェースは SDO のプロパティをモニタリングするために
     * 使用される。
     * Monitoring interface の詳細については OMG SDO specification の
     * 2.3.7節 "Monitoring Interface" p.2-35 を参照のこと。
     *
     * @return SDO の Monitoring interface への参照
     *
     * @exception InterfaceNotImplemented SDOはConfigurationインターフェースを
     *                                    持たない。
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get Monitoring object
     *
     * This operation returns an object implementing the Monitoring interface.
     * The Monitoring interface is one of the interfaces that each SDO
     * maintains. The interface is used to monitor the properties of an SDO.
     * See OMG SDO specification Section 2.3.7, "Monitoring Interface," on
     * page 2-35 for more details about the Monitoring interface.
     *
     * @return The Monitoring interface of an SDO.
     *
     * @exception InterfaceNotImplemented The target SDO has no Configuration
     *                                    interface.
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    SDOPackage::Monitoring_ptr get_monitoring()
      throw (CORBA::SystemException,
             SDOPackage::InterfaceNotImplemented, SDOPackage::NotAvailable,
             SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] Organization リストの取得
     *
     * SDO は0個以上の Organization (組織)に所属することができる。 もし SDO が
     * 1個以上の Organization に所属している場合、このオペレーションは所属する
     * Organization のリストを返す。SDO が どの Organization にも所属していない
     * 場合には、空のリストが返される。
     *
     * @return SDO が所属する Organization のリスト。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [SDO interface] Get Organization list
     *
     * An SDO belongs to zero or more organizations. If the SDO belongs to one
     * or more organizations, this operation returns the list of organizations
     * that the SDO belongs to. An empty list is returned if the SDO does not
     * belong to any Organizations.
     *
     * @return The list of Organizations that the SDO belong to.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    SDOPackage::OrganizationList* get_organizations()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] SDO Status リストの取得
     *
     * このオペレーションは SDO のステータスを表す NVList を返す。
     *
     * @return SDO のステータス。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [SDO interface] Get SDO Status list
     *
     * This operation returns an NVlist describing the status of an SDO.
     *
     * @return The actual status of an SDO.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     * @endif
     */
    SDOPackage::NVList* get_status_list()
      throw (CORBA::SystemException,
             SDOPackage::NotAvailable, SDOPackage::InternalError) override;

    /*!
     * @if jp
     *
     * @brief [SDO interface] SDO Status の取得
     *
     * This operation returns the value of the specified status parameter.
     *
     * @param name SDO のステータスを定義するパラメータ。
     *
     * @return 指定されたパラメータのステータス値。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InvalidParameter 引数 "name" が null あるいは存在しない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [SDO interface] Get SDO Status
     *
     * @param name One of the parameters defining the "status" of an SDO.
     *
     * @return The value of the specified status parameter.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The parameter defined by "name" is null or
     *                             does not exist.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     *
     *
     * @endif
     */
    CORBA::Any* get_status(const char* name)
      throw (CORBA::SystemException,
             SDOPackage::InvalidParameter, SDOPackage::NotAvailable,
             SDOPackage::InternalError) override;

    //============================================================
    // Local interfaces
    //============================================================
    /*!
     * @if jp
     *
     * @brief [local interface] インスタンス名の取得
     *
     * ComponentProfile に設定されたインスタンス名を返す。
     *
     * @return インスタンス名
     *
     * @else
     *
     * @brief [local interface] Get instance name
     *
     * Return the instance name that has been set in ComponentProfile.
     *
     * @return Instance name
     *
     * @endif
     */
    const char* getInstanceName()
    {
      RTC_TRACE(("getInstanceName()"));
      return m_profile.instance_name;
    }

    /*!
     * @if jp
     *
     * @brief [local interface] インスタンス名の設定
     *
     * ComponentProfile に指定されたインスタンス名を設定する。
     *
     * @param instance_name インスタンス名
     *
     * @else
     *
     * @brief [local interface] Set instance name
     *
     * Set the instance name specified in ComponentProfile.
     *
     * @param instance_name Instance name
     *
     * @endif
     */
    void setInstanceName(const char* instance_name);

    /*!
     * @if jp
     *
     * @brief [local interface] 型名の取得
     *
     * ComponentProfile に設定された型名を返す。
     *
     * @return 型名
     *
     * @else
     *
     * @brief [local interface] Get type name
     *
     * Get the type name has been set in ComponentProfile.
     *
     * @return Type name
     *
     * @endif
     */
    const char* getTypeName()
    {
      RTC_TRACE(("getTypeName()"));
      return m_profile.type_name;
    }

    /*!
     * @if jp
     *
     * @brief [local interface] Description の取得
     *
     * ComponentProfile に設定された Description を返す。
     *
     * @return Description
     *
     * @else
     *
     * @brief [local interface] GetDescription
     *
     * Get the Description has been set in ComponentProfile.
     *
     * @return Description
     *
     * @endif
     */
    const char* getDescription()
    {
      RTC_TRACE(("getDescription()"));
      return m_profile.description;
    }

    /*!
     * @if jp
     *
     * @brief [local interface] バージョン情報の取得
     *
     * ComponentProfile に設定されたバージョン情報を返す。
     *
     * @return バージョン情報
     *
     * @else
     *
     * @brief [local interface] Get version information
     *
     * Get the version information that has been set in
     * ComponentProfile.
     *
     * @return Version information
     *
     * @endif
     */
    const char* getVersion()
    {
      RTC_TRACE(("getVersion()"));
      return m_profile.version;
    }

    /*!
     * @if jp
     *
     * @brief [local interface] ベンダー情報の取得
     *
     * ComponentProfile に設定されたベンダー情報を返す。
     *
     * @return ベンダー情報
     *
     * @else
     *
     * @brief [local interface] Get vendor
     *
     * Get the vendor information that has been set in ComponentProfile.
     *
     * @return Vendor information
     *
     * @endif
     */
    const char* getVendor()
    {
      RTC_TRACE(("getVendor()"));
      return m_profile.vendor;
    }

    /*!
     * @if jp
     *
     * @brief [local interface] カテゴリ情報の取得
     *
     * ComponentProfile に設定されたカテゴリ情報を返す。
     *
     * @return カテゴリ情報
     *
     * @else
     *
     * @brief [local interface] Get category information
     *
     * Get the category information that has been set in ComponentProfile.
     *
     * @return Category information
     *
     * @endif
     */
    const char* getCategory()
    {
      RTC_TRACE(("getCategory()"));
      return m_profile.category;
    }

    /*!
     * @if jp
     *
     * @brief [local interface] Naming Server 情報の取得
     *
     * 設定された Naming Server 情報を返す。
     *
     * @return Naming Server リスト
     *
     * @else
     *
     * @brief [local interface] Get Naming Server information
     *
     * Get Naming Server information that has been set.
     *
     * @return Naming Server list
     *
     * @endif
     */
    std::vector<std::string> getNamingNames();

    /*!
     * @if jp
     *
     * @brief [local interface] オブジェクトリファレンスの設定
     *
     * RTC の CORBA オブジェクトリファレンスを設定する。
     *
     * @param rtobj オブジェクトリファレンス
     *
     * @else
     *
     * @brief [local interface] Set the object reference
     *
     * Set RTC's CORBA object reference.
     *
     * @param rtobj The object reference
     *
     * @endif
     */
    void setObjRef(RTObject_ptr rtobj);

    /*!
     * @if jp
     *
     * @brief [local interface] オブジェクトリファレンスの取得
     *
     * 設定された CORBA オブジェクトリファレンスを取得する。
     *
     * @return オブジェクトリファレンス
     *
     * @else
     *
     * @brief [local interface] Get the object reference
     *
     * Get CORBA object reference that has been set
     *
     * @return The object reference
     *
     * @endif
     */
    RTObject_ptr getObjRef() const;

    /*!
     * @if jp
     *
     * @brief [local interface] RTC のプロパティを設定する
     *
     * RTC が保持すべきプロパティを設定する。与えられるプロパティは、
     * ComponentProfile 等に設定されるべき情報を持たなければならない。
     * このオペレーションは通常 RTC が初期化される際に Manager から
     * 呼ばれることを意図している。
     *
     * @param prop RTC のプロパティ
     *
     * @else
     *
     * @brief [local interface] Set RTC property
     *
     * This operation sets the properties to the RTC. The given property
     * values should include information for ComponentProfile.
     * Generally, this operation is designed to be called from Manager, when
     * RTC is initialized
     *
     * @param prop Property for RTC.
     *
     * @endif
     */
    void setProperties(const coil::Properties& prop);

    /*!
     * @if jp
     *
     * @brief [local interface] RTC のプロパティを取得する
     *
     * RTC が保持しているプロパティを返す。
     * RTCがプロパティを持たない場合は空のプロパティが返される。
     *
     * @return RTC のプロパティ
     *
     * @else
     *
     * @brief [local interface] Get RTC property
     *
     * This operation returns the properties of the RTC.
     * Empty property would be returned, if RTC has no property.
     *
     * @return Property for RTC.
     *
     * @endif
     */
    coil::Properties& getProperties();

    /*!
     * @if jp
     *
     * @brief コンフィギュレーションパラメータの設定
     *
     * コンフィギュレーションパラメータと変数をバインドする
     * \<VarType\>としてコンフィギュレーションパラメータのデータ型を指定する。
     *
     * @param param_name コンフィギュレーションパラメータ名
     * @param var コンフィギュレーションパラメータ格納用変数
     * @param def_val コンフィギュレーションパラメータデフォルト値
     * @param trans コンフィギュレーションパラメータ文字列変換用関数
     *
     * @return 設定結果(設定成功:true，設定失敗:false)
     *
     * @else
     *
     * @brief Setup for configuration parameters
     *
     * Bind configuration parameter to its variable.
     * Specify the data type of configuration parameter as \<VarType\>.
     *
     * @param param_name Configuration parameter name
     * @param var Variables to store configuration parameter
     * @param def_val Default value of configuration parameter
     * @param trans Function to transform configuration parameter type into
     *        string format
     *
     * @return Setup result (Successful:true, Failed:false)
     *
     * @endif
     */
    template <typename VarType>
    bool bindParameter(const char* param_name, VarType& var,
                       const char* def_val,
                       bool (*trans)(VarType&, const char*) = coil::stringTo)
    {
      RTC_TRACE(("bindParameter(%s (default: %s))", param_name, def_val));
      m_configsets.bindParameter(param_name, var, def_val, trans);
      return true;
    }

    /*!
     * @if jp
     *
     * @brief コンフィギュレーションサービスを取得する
     *
     * コンフィギュレーションサービスオブジェクトを取得する。このサービ
     * スオブジェクトを利用して、コンフィギュレーションパラメータの操作
     * を行うことができる。主な操作としては、
     *
     * - unbindParameter(): パラメータのアンバインド
     * - update(): パラメータの更新
     * - update(set_name): 特定のセットの更新
     * - update(set_name, param_name): 特定のセットのパラメータの更新
     * - isExist(): パラメータの存在確認
     * - isChanged(): パラメータが変更されたかの確認
     * - changedParameters(): 変更されたパラメータのリスト
     * - getActiveId(): アクティブセット名の取得
     * - haveConfig(config_id): コンフィグセットを持っているかどうか
     * - getConfigurationSets(): 全コンフィギュレーションセットの取得
     * - getConfigurationSet(set_id): 特定セットを取得
     *
     * コールバック関連
     * - addConfigurationParamListener(): リスナの追加
     * - removeConfigurationParamListener(): リスナの削除
     * - addConfigurationSetListener(): リスナの追加
     * - removeConfigurationSetListener(): リスナの削除
     * - addConfigurationSetNameListener(): リスナの追加
     * - removeConfigurationSetNameListener(): リスナの削除
     *
     * 詳細はConfigAdminクラスリファレンスを参照のこと。
     *
     * @return ConfigAdmin object
     *
     * @else
     *
     * @brief Getting configuration service
     *
     * This operation returns configuration service object. By using
     * this service, user can manipulate configuration
     * parameters. Mainly the following operations are supported.
     *
     * - unbindParameter(): Unbinding parameters
     * - update(): Updateing parameters
     * - update(set_name): Updating a specific configuration set
     * - update(set_name, param_name): Updating specific parameter in a set
     * - isExist(): Checking existence of a parameter
     * - isChanged(): Check if a parameter was updated
     * - changedParameters(): Getting changed parameter list
     * - getActiveId(): Getting active configuration set name
     * - haveConfig(config_id): Checking if having a specified configuration set
     * - getConfigurationSets(): getting all the configuration sets
     * - getConfigurationSet(set_id): Getting a configuration set
     *
     * Callback related member functions
     * - addConfigurationParamListener(): Adding listener
     * - removeConfigurationParamListener(): Removing listener
     * - addConfigurationSetListener(): Adding listener
     * - removeConfigurationSetListener(): Removing listener
     * - addConfigurationSetNameListener(): Adding listener
     * - removeConfigurationSetNameListener(): Removing listener
     *
     * See details in the ConfigAdmin class reference
     *
     * @return ConfigAdmin object
     *
     * @endif
     */
    ConfigAdmin& getConfigService() { return m_configsets; }

    /*!
     * @if jp
     *
     * @brief コンフィギュレーションパラメータの更新(ID指定)
     *
     * 指定したIDのコンフィギュレーションセットに設定した値で、
     * コンフィギュレーションパラメータの値を更新する
     *
     * @param config_set 設定対象のコンフィギュレーションセットID
     *
     * @else
     *
     * @brief Update configuration parameters (by ID)
     *
     * Update configuration parameter value by the value that
     * set to a configuration set of specified ID.
     *
     * @param config_set The target configuration set's ID for setup
     *
     * @endif
     */
    void updateParameters(const char* config_set);

    /*!
     * @if jp
     *
     * @brief [local interface] Port を登録する
     *
     * RTC が保持するPortを登録する。
     * Port を外部からアクセス可能にするためには、このオペレーションにより
     * 登録されていなければならない。登録される Port はこの RTC 内部において
     * PortProfile.name により区別される。したがって、Port は RTC 内において、
     * ユニークな PortProfile.name を持たなければならない。
     * 登録された Port は内部で適切にアクティブ化された後、その参照と
     * オブジェクト参照がリスト内に保存される。
     *
     * @param port RTC に登録する Port
     * @return 登録結果(登録成功:true，登録失敗:false)
     *
     * @else
     *
     * @brief [local interface] Register Port
     *
     * This operation registers a Port held by this RTC.
     * In order to enable access to the Port from outside of RTC, the Port
     * must be registered by this operation. The Port that is registered by
     * this operation would be identified by PortProfile.name in the inside of
     * RTC. Therefore, the Port should have unique PortProfile.name in the RTC.
     * The registering Port would be activated properly, and the reference
     * and the object reference would be stored in lists in RTC.
     *
     * @param port Port which is registered to the RTC
     * @return Register result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool addPort(PortBase& port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port を登録する
     *
     * RTC が保持するPortを登録する。
     * Port を外部からアクセス可能にするためには、このオペレーションにより
     * 登録されていなければならない。登録される Port はこの RTC 内部において
     * PortProfile.name により区別される。したがって、Port は RTC 内において、
     * ユニークな PortProfile.name を持たなければならない。
     * 登録された Port は内部で適切にアクティブ化された後、その参照と
     * オブジェクト参照がリスト内に保存される。
     *
     * @param port RTC に登録する Port
     * @return 登録結果(登録成功:true，登録失敗:false)
     *
     * @else
     *
     * @brief [local interface] Register Port
     *
     * This operation registers a Port held by this RTC.
     * In order to enable access to the Port from outside of RTC, the Port
     * must be registered by this operation. The Port that is registered by
     * this operation would be identified by PortProfile.name in the inside of
     * RTC. Therefore, the Port should have unique PortProfile.name in the RTC.
     * The registering Port would be activated properly, and the reference
     * and the object reference would be stored in lists in RTC.
     *
     * @param port Port which is registered to the RTC
     * @return Register result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool addPort(PortService_ptr port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port を登録する
     *
     * RTC が保持するPortを登録する。
     * Port を外部からアクセス可能にするためには、このオペレーションにより
     * 登録されていなければならない。登録される Port はこの RTC 内部において
     * PortProfile.name により区別される。したがって、Port は RTC 内において、
     * ユニークな PortProfile.name を持たなければならない。
     * 登録された Port は内部で適切にアクティブ化された後、その参照と
     * オブジェクト参照がリスト内に保存される。
     *
     * @param port RTC に登録する Port
     * @return 登録結果(登録成功:true，登録失敗:false)
     *
     * @else
     *
     * @brief [local interface] Register Port
     *
     * This operation registers a Port held by this RTC.
     * In order to enable access to the Port from outside of RTC, the Port
     * must be registered by this operation. The Port that is registered by
     * this operation would be identified by PortProfile.name in the inside of
     * RTC. Therefore, the Port should have unique PortProfile.name in the RTC.
     * The registering Port would be activated properly, and the reference
     * and the object reference would be stored in lists in RTC.
     *
     * @param port Port which is registered to the RTC
     * @return Register result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool addPort(CorbaPort& port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port を登録する
     *
     * RTC が保持するPortを登録する。
     * Port を外部からアクセス可能にするためには、このオペレーションにより
     * 登録されていなければならない。登録される Port はこの RTC 内部において
     * PortProfile.name により区別される。したがって、Port は RTC 内において、
     * ユニークな PortProfile.name を持たなければならない。
     * 登録された Port は内部で適切にアクティブ化された後、その参照と
     * オブジェクト参照がリスト内に保存される。
     *
     * @param port RTC に登録する Port
     *
     * @else
     *
     * @brief [local interface] Register Port
     *
     * This operation registers a Port held by this RTC.
     * In order to enable access to the Port from outside of RTC, the Port
     * must be registered by this operation. The Port that is registered by
     * this operation would be identified by PortProfile.name in the inside of
     * RTC. Therefore, the Port should have unique PortProfile.name in the RTC.
     * The registering Port would be activated properly, and the reference
     * and the object reference would be stored in lists in RTC.
     *
     * @param port Port which is registered to the RTC
     *
     * @endif
     */
    void registerPort(PortBase& port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port を登録する
     *
     * RTC が保持するPortを登録する。
     * Port を外部からアクセス可能にするためには、このオペレーションにより
     * 登録されていなければならない。登録される Port はこの RTC 内部において
     * PortProfile.name により区別される。したがって、Port は RTC 内において、
     * ユニークな PortProfile.name を持たなければならない。
     * 登録された Port は内部で適切にアクティブ化された後、その参照と
     * オブジェクト参照がリスト内に保存される。
     *
     * @param port RTC に登録する Port
     *
     * @else
     *
     * @brief [local interface] Register Port
     *
     * This operation registers a Port held by this RTC.
     * In order to enable access to the Port from outside of RTC, the Port
     * must be registered by this operation. The Port that is registered by
     * this operation would be identified by PortProfile.name in the inside of
     * RTC. Therefore, the Port should have unique PortProfile.name in the RTC.
     * The registering Port would be activated properly, and the reference
     * and the object reference would be stored in lists in RTC.
     *
     * @param port Port which is registered to the RTC
     *
     * @endif
     */
    void registerPort(PortService_ptr port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port を登録する
     *
     * RTC が保持するPortを登録する。
     * Port を外部からアクセス可能にするためには、このオペレーションにより
     * 登録されていなければならない。登録される Port はこの RTC 内部において
     * PortProfile.name により区別される。したがって、Port は RTC 内において、
     * ユニークな PortProfile.name を持たなければならない。
     * 登録された Port は内部で適切にアクティブ化された後、その参照と
     * オブジェクト参照がリスト内に保存される。
     *
     * @param port RTC に登録する Port
     *
     * @else
     *
     * @brief [local interface] Register Port
     *
     * This operation registers a Port held by this RTC.
     * In order to enable access to the Port from outside of RTC, the Port
     * must be registered by this operation. The Port that is registered by
     * this operation would be identified by PortProfile.name in the inside of
     * RTC. Therefore, the Port should have unique PortProfile.name in the RTC.
     * The registering Port would be activated properly, and the reference
     * and the object reference would be stored in lists in RTC.
     *
     * @param port Port which is registered to the RTC
     *
     * @endif
     */
    void registerPort(CorbaPort& port);

    /*!
     * @if jp
     *
     * @brief [local interface] DataInPort を登録する
     *
     * RTC が保持する DataInPort を登録する。
     * Port のプロパティにデータポートであること("port.dataport")、
     * TCPを使用すること("tcp_any")を設定するとともに、 DataInPort の
     * インスタンスを生成し、登録する。
     *
     * @param name port 名称
     * @param inport 登録対象 DataInPort
     * @return 登録結果(登録成功:true，登録失敗:false)
     *
     * @else
     *
     * @brief [local interface] Register DataInPort
     *
     * This operation registers DataInPort held by this RTC.
     * Set "port.dataport" and "tcp_any" to property of Port, and
     * create instances of DataInPort and register it.
     *
     * @param name Port name
     * @param inport DataInPort which is registered to the RTC
     * @return Register result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool addInPort(const char* name, InPortBase& inport);
    const std::vector<InPortBase*>& getInPorts() const {return m_inports;}
    const std::vector<OutPortBase*>& getOutPorts() const {return m_outports;}
    /*!
     * @if jp
     *
     * @brief [local interface] DataInPort を登録する
     *
     * RTC が保持する DataInPort を登録する。
     * Port のプロパティにデータポートであること("port.dataport")、
     * TCPを使用すること("tcp_any")を設定するとともに、 DataInPort の
     * インスタンスを生成し、登録する。
     *
     * @param name port 名称
     * @param inport 登録対象 DataInPort
     *
     * @else
     *
     * @brief [local interface] Register DataInPort
     *
     * This operation registers DataInPort held by this RTC.
     * Set "port.dataport" and "tcp_any" to property of Port, and
     * create instances of DataInPort and register it.
     *
     * @param name Port name
     * @param inport DataInPort which is registered to the RTC
     *
     * @endif
     */
    void registerInPort(const char* name, InPortBase& inport);

    /*!
     * @if jp
     *
     * @brief [local interface] DataOutPort を登録する
     *
     * RTC が保持する DataOutPortを登録する。
     * Port のプロパティにデータポートであること("port.dataport")、
     * TCPを使用すること("tcp_any")を設定するとともに、 DataOutPort の
     * インスタンスを生成し、登録する。
     *
     * @param name port 名称
     * @param outport 登録対象 DataOutPort
     * @return 登録結果(登録成功:true，登録失敗:false)
     *
     * @else
     *
     * @brief [local interface] Register DataOutPort
     *
     * This operation registers DataOutPor held by this RTC.
     * Set "port.dataport" and "tcp_any" to property of Port, and then
     * create instances of DataOutPort and register it.
     *
     * @param name Port name
     * @param outport DataOutPort which is registered to the RTC
     * @return Register result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool addOutPort(const char* name, OutPortBase& outport);
    /*!
     * @if jp
     *
     * @brief [local interface] DataOutPort を登録する
     *
     * RTC が保持する DataOutPortを登録する。
     * Port のプロパティにデータポートであること("port.dataport")、
     * TCPを使用すること("tcp_any")を設定するとともに、 DataOutPort の
     * インスタンスを生成し、登録する。
     *
     * @param name port 名称
     * @param outport 登録対象 DataOutPort
     *
     * @else
     *
     * @brief [local interface] Register DataOutPort
     *
     * This operation registers DataOutPor held by this RTC.
     * Set "port.dataport" and "tcp_any" to property of Port, and then
     * create instances of DataOutPort and register it.
     *
     * @param name Port name
     * @param outport DataOutPort which is registered to the RTC
     *
     * @endif
     */
    void registerOutPort(const char* name, OutPortBase& outport);

    /*!
     * @if jp
     *
     * @brief [local interface] InPort の登録を削除する
     *
     * RTC が保持するInPortの登録を削除する。
     *
     * @param port 削除対象 Port
     * @return 削除結果(削除成功:true，削除失敗:false)
     *
     * @else
     *
     * @brief [local interface] Unregister InPort
     *
     * This operation unregisters a InPort held by this RTC.
     *
     * @param port Port which is unregistered
     * @return Unregister result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool removeInPort(InPortBase& port);

    /*!
     * @if jp
     *
     * @brief [local interface] OutPort の登録を削除する
     *
     * RTC が保持するOutPortの登録を削除する。
     *
     * @param port 削除対象 Port
     * @return 削除結果(削除成功:true，削除失敗:false)
     *
     * @else
     *
     * @brief [local interface] Unregister OutPort
     *
     * This operation unregisters a OutPort held by this RTC.
     *
     * @param port Port which is unregistered
     * @return Unregister result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool removeOutPort(OutPortBase& port);

    /*!
     * @if jp
     *
     * @brief [local interface] Port の登録を削除する
     *
     * RTC が保持するPortの登録を削除する。
     *
     * @param port 削除対象 Port
     * @return 削除結果(削除成功:true，削除失敗:false)
     *
     * @else
     *
     * @brief [local interface] Unregister Port
     *
     * This operation unregisters a Port held by this RTC.
     *
     * @param port Port which is unregistered
     * @return Unregister result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool removePort(PortBase& port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port の登録を削除する
     *
     * RTC が保持するPortの登録を削除する。
     *
     * @param port 削除対象 Port
     * @return 削除結果(削除成功:true，削除失敗:false)
     *
     * @else
     *
     * @brief [local interface] Unregister Port
     *
     * This operation unregisters a Port held by this RTC.
     *
     * @param port Port which is unregistered
     * @return Unregister result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool removePort(PortService_ptr port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port の登録を削除する
     *
     * RTC が保持するPortの登録を削除する。
     *
     * @param port 削除対象 Port
     * @return 削除結果(削除成功:true，削除失敗:false)
     *
     * @else
     *
     * @brief [local interface] Unregister Port
     *
     * This operation unregisters a Port held by this RTC.
     *
     * @param port Port which is unregistered
     * @return Unregister result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool removePort(CorbaPort& port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port の登録を削除する
     *
     * RTC が保持するPortの登録を削除する。
     *
     * @param port 削除対象 Port
     *
     * @else
     *
     * @brief [local interface] Unregister Port
     *
     * This operation unregisters a Port held by this RTC.
     *
     * @param port Port which is unregistered
     *
     * @endif
     */
    void deletePort(PortBase& port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port の登録を削除する
     *
     * RTC が保持するPortの登録を削除する。
     *
     * @param port 削除対象 Port
     *
     * @else
     *
     * @brief [local interface] Unregister Port
     *
     * This operation unregisters a Port held by this RTC.
     *
     * @param port Port which is unregistered
     *
     * @endif
     */
    void deletePort(PortService_ptr port);
    /*!
     * @if jp
     *
     * @brief [local interface] Port の登録を削除する
     *
     * RTC が保持するPortの登録を削除する。
     *
     * @param port 削除対象 Port
     *
     * @else
     *
     * @brief [local interface] Unregister Port
     *
     * This operation unregisters a Port held by this RTC.
     *
     * @param port Port which is unregistered
     *
     * @endif
     */
    void deletePort(CorbaPort& port);

    /*!
     * @if jp
     *
     * @brief [local interface] 名前指定により Port の登録を削除する
     *
     * 名称を指定して RTC が保持するPortの登録を削除する。
     *
     * @param port_name 削除対象 Port 名
     *
     * @else
     *
     * @brief [local interface] Delete Port by specifying its name
     *
     * Delete Port which RTC has by specifying its name.
     *
     * @param port_name Name of Port which is deleted
     *
     * @endif
     */
    void deletePortByName(const char* port_name);

    /*!
     * @if jp
     *
     * @brief [local interface] 実行コンテキストを取得する
     *
     * get_context() と同じ機能のローカル版。違いはない。
     * この関数は以下の関数内で呼ばれることを前提としている。
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * この関数の引数はこれらの関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     *
     * @else
     *
     * @brief [local interface] Getting current execution context
     *
     * This function is the local version of get_context(). completely
     * same as get_context() function. This function is assumed to be
     * called from the following functions.
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above functions.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     *
     * @endif
     */
    ExecutionContext_ptr getExecutionContext(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief [local interface] 実行コンテキストの実行レートを取得する
     *
     * 現在実行中の実行コンテキストの実行レートを取得する。実行コンテキ
     * ストのKindがPERIODIC以外の場合の動作は未定義である。この関数は以
     * 下の関数内で呼ばれることを前提としている。
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * この関数の引数はこれらの関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     *
     * @else
     *
     * @brief [local interface] Getting current context' execution rate
     *
     * This function returns current execution rate in this
     * context. If this context's kind is not PERIODC, behavior is not
     * defined. This function is assumed to be called from the
     * following functions.
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above functions.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     *
     * @endif
     */
    double getExecutionRate(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief [local interface] 実行コンテキストの実行レートを設定する
     *
     * 現在実行中の実行コンテキストの実行レートを設定する。実行コンテキ
     * ストのKindがPERIODIC以外の場合の動作は未定義である。この関数は以
     * 下の関数内で呼ばれることを前提としている。
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * この関数の引数はこれらの関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     * @param rate 実行レートを [Hz] で与える
     *
     * @else
     *
     * @brief [local interface] Setting current context' execution rate
     *
     * This function sets a execution rate in the context. If this
     * context's kind is not PERIODC, behavior is not defined. This
     * function is assumed to be called from the following functions.
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above functions.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     * @param rate Execution rate in [Hz].
     *
     * @endif
     */
    ReturnCode_t setExecutionRate(RTC::UniqueId ec_id, double rate);

    /*!
     * @if jp
     *
     * @brief [local interface] 実行コンテキストの所有権を調べる
     *
     * 現在実行中の実行コンテキストの所有権を調べる。この関数は以下の関
     * 数内で呼ばれることを前提としている。
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * この関数の引数はこれらの関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     * @return true: 自身の実行コンテキスト、false: 他の実行コンテキスト
     *
     * @else
     *
     * @brief [local interface] Checking if the current context is own context
     *
     * This function checks if the current context is own execution
     * context. This function is assumed to be called from the
     * following functions.
     *
     * - onStartup()
     * - onShutdown()
     * - onActivated()
     * - onDeactivated()
     * - onExecute()
     * - onAborting()
     * - onError()
     * - onReset()
     * - onStateUpdate()
     * - onRateChanged()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above functions.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     * @return true: Own context, false: other's context
     *
     * @endif
     */
    bool isOwnExecutionContext(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief [local interface] 状態を Inactive に遷移させる
     *
     * 状態を Active から Inactive に遷移させる。この関数は以下の関
     * 数内で呼ばれることを前提としている。
     *
     * - onActivated()
     * - onExecute()
     * - onStateUpdate()
     *
     * この関数の引数は上記の関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     * @return リターンコード
     *
     * @else
     *
     * @brief [local interface] Make transition to Inactive state
     *
     * This function makes transition from Active to Inactive
     * state. This function is assumed to be called from the following
     * functions.
     *
     * - onActivated()
     * - onExecute()
     * - onStateUpdate()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above function.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     * @return Return code
     *
     * @endif
     */
    ReturnCode_t deactivate(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief [local interface] 状態を Active に遷移させる
     *
     * 状態を Inactive から Active に遷移させる。この関数は以下の関
     * 数内で呼ばれることを前提としている。
     *
     * - onStartup()
     * - onDeactivated()
     *
     * この関数の引数は上記の関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     * @return リターンコード
     *
     * @else
     *
     * @brief [local interface] Make transition to Active state
     *
     * This function makes transition from Inactive to Active
     * state. This function is assumed to be called from the following
     * functions.
     *
     * - onStartup()
     * - onDeactivated()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above function.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     * @return Return code
     *
     * @endif
     */
    ReturnCode_t activate(RTC::UniqueId ec_id);

    /*!
     * @if jp
     *
     * @brief [local interface] 状態をリセットし Inactive に遷移させる
     *
     * 状態を Error から Inactive に遷移させる。この関数は以下の関
     * 数内で呼ばれることを前提としている。
     *
     * - onError()
     *
     * この関数の引数は上記の関数の引数 UniquieID exec_handle でなけ
     * ればならない。
     *
     * @param ec_id 上記関数の第1引数 exec_handle を渡す必要がある。
     * @return リターンコード
     *
     * @else
     *
     * @brief [local interface] Resetting and go to Inactive state
     *
     * This function reset RTC and makes transition from Error to Inactive
     * state. This function is assumed to be called from the following
     * functions.
     *
     * - onError()
     *
     * The argument of this function should be the first argument
     * (UniqueId ec_id) of the above function.
     *
     * @param ec_id The above functions' first argument "exec_handle."
     * @return Return code
     *
     * @endif
     */
    ReturnCode_t reset(RTC::UniqueId ec_id);

    /*!
     * @if jp
     * @brief [local interface] SDO service provider をセットする
     * @else
     * @brief [local interface] Set a SDO service provider
     * @endif
     */
    bool addSdoServiceProvider(const SDOPackage::ServiceProfile& prof,
                               SdoServiceProviderBase* provider);

    /*!
     * @if jp
     * @brief [local interface] SDO service provider を削除する
     * @else
     * @brief [local interface] Remove a SDO service provider
     * @endif
     */
    bool removeSdoServiceProvider(const char* id);

    /*!
     * @if jp
     * @brief [local interface] SDO service provider をセットする
     * @else
     * @brief [local interface] Set a SDO service provider
     * @endif
     */
    bool addSdoServiceConsumer(const SDOPackage::ServiceProfile& prof);

    /*!
     * @if jp
     * @brief [local interface] SDO service provider を削除する
     * @else
     * @brief [local interface] Remove a SDO service provider
     * @endif
     */
    bool removeSdoServiceConsumer(const char* id);

    /*!
     * @if jp
     * @brief [local interface] SDO service provider を別スレッドで削除する
     * @else
     * @brief [local interface] Remove a SDO service provider
     * @endif
     */
    void removeSdoServiceConsumerStartThread(const char* id);

    /*!
     * @if jp
     *
     * @brief 全 InPort のデータを読み込む。
     *
     * RTC が保持する全ての InPort のデータを読み込む。
     *
     * @return 読み込み結果(全ポートの読み込み成功:true，失敗:false)
     *
     * @else
     *
     * @brief Readout the value from All InPorts.
     *
     * This operation read the value from all InPort
     * registered in the RTC.
     *
     * @return result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool readAll();

    /*!
     * @if jp
     *
     * @brief 全 OutPort のwrite()メソッドをコールする。
     *
     * RTC が保持する全ての OutPort のwrite()メソッドをコールする。
     *
     * @return 読み込み結果(全ポートへの書き込み成功:true，失敗:false)
     *
     * @else
     *
     * @brief The write() method of all OutPort is called.
     *
     * This operation call the write() method of all OutPort
     * registered in the RTC.
     *
     * @return result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool writeAll();

    /*!
     * @if jp
     *
     * @brief onExecute()実行前でのreadAll()メソッドの呼出を有効または無効にする。
     *
     * このメソッドをパラメータをtrueとして呼ぶ事により、onExecute()実行前に
     * readAll()が呼出されるようになる。
     * パラメータがfalseの場合は、readAll()呼出を無効にする。
     *
     * @param read(default:true)
     *        (readAll()メソッド呼出あり:true, readAll()メソッド呼出なし:false)
     *
     * @param completion(default:false)
     *    readAll()にて、どれかの一つのInPortのread()が失敗しても全ての
     *    InPortのread()を呼び出す:true, readAll()にて、どれかの一つの
     *    InPortのread()が失敗した場合、すぐにfalseで抜ける:false
     *
     * @else
     *
     * @brief Set whether to execute the readAll() method.
     *
     * Set whether to execute the readAll() method.
     *
     * @param read(default:true)
     *        (readAll() is called:true, readAll() isn't called:false)
     *
     * @param completion(default:false)
     *     All InPort::read() calls are completed.:true,
     *     If one InPort::read() is False, return false.:false
     *
     * @param completion(default:false)
     *
     * @endif
     */
    void setReadAll(bool read = true, bool completion = false);

    /*!
     * @if jp
     *
     * @brief onExecute()実行後にwriteAll()メソッドの呼出を有効または無効にする。
     *
     * このメソッドをパラメータをtrueとして呼ぶ事により、onExecute()実行後に
     * writeAll()が呼出されるようになる。
     * パラメータがfalseの場合は、writeAll()呼出を無効にする。
     *
     * @param write(default:true)
     *        (writeAll()メソッド呼出あり:true, writeAll()メソッド呼出
     *        なし:false)
     *
     * @param completion(default:false)
     *    writeAll()にて、どれかの一つのOutPortのwrite()が失敗しても全
     *    てのOutPortのwrite()を呼び出しを行う:true, writeAll()にて、ど
     *    れかの一つのOutPortのwrite()が失敗した場合、すぐにfalseで抜け
     *    る:false
     *
     * @else
     *
     * @brief Set whether to execute the writeAll() method.
     *
     * Set whether to execute the writeAll() method.
     *
     * @param write(default:true)
     *        (writeAll() is called:true, writeAll() isn't called:false)
     *
     * @param completion(default:false)
     *     All OutPort::write() calls are completed.:true,
     *     If one OutPort::write() is False, return false.:false
     *
     * @endif
     */
    void setWriteAll(bool write = true, bool completion = false);


    /*!
     * @if jp
     *
     * @brief 全 Port の登録を削除する
     *
     * RTC が保持する全ての Port を削除する。
     *
     * @else
     *
     * @brief Unregister All Ports
     *
     * This operation deactivates the all Ports and deletes the all Port's
     * registrations in the RTC
     *
     * @endif
     */
    void finalizePorts();


    /*!
     * @if jp
     *
     * @brief ExecutionContextBaseリストの登録を削除する
     *
     * @else
     *
     * @brief The ExecutionContextBase list is deleted.
     *
     * @endif
     */
    void finalizeContexts();

	/*!
	* @if jp
	*
	* @brief omniINSPOAから取得したオブジェクトを登録
	*
	* @else
	*
	* @brief 
	*
	* @endif
	*/
	void setINSObjRef(RTC::LightweightRTObject_ptr obj);


    /*!
     * @if jp
     * @brief PreComponentActionListener リスナを追加する
     *
     * ComponentAction 実装関数の呼び出し直前のイベントに関連する各種リ
     * スナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - PRE_ON_INITIALIZE:    onInitialize 直前
     * - PRE_ON_FINALIZE:      onFinalize 直前
     * - PRE_ON_STARTUP:       onStartup 直前
     * - PRE_ON_SHUTDOWN:      onShutdown 直前
     * - PRE_ON_ACTIVATED:     onActivated 直前
     * - PRE_ON_DEACTIVATED:   onDeactivated 直前
     * - PRE_ON_ABORTED:       onAborted 直前
     * - PRE_ON_ERROR:         onError 直前
     * - PRE_ON_RESET:         onReset 直前
     * - PRE_ON_EXECUTE:       onExecute 直前
     * - PRE_ON_STATE_UPDATE:  onStateUpdate 直前
     *
     * リスナは PreComponentActionListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PreComponentActionListener::operator()(UniqueId ec_id)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePreComponentActionListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PreComponentAction type listener
     *
     * This operation adds certain listeners related to ComponentActions
     * pre events.
     * The following listener types are available.
     *
     * - PRE_ON_INITIALIZE:    before onInitialize
     * - PRE_ON_FINALIZE:      before onFinalize
     * - PRE_ON_STARTUP:       before onStartup
     * - PRE_ON_SHUTDOWN:      before onShutdown
     * - PRE_ON_ACTIVATED:     before onActivated
     * - PRE_ON_DEACTIVATED:   before onDeactivated
     * - PRE_ON_ABORTED:       before onAborted
     * - PRE_ON_ERROR:         before onError
     * - PRE_ON_RESET:         before onReset
     * - PRE_ON_EXECUTE:       before onExecute
     * - PRE_ON_STATE_UPDATE:  before onStateUpdate
     *
     * Listeners should have the following function operator().
     *
     * PreComponentActionListener::operator()(UniqueId ec_id)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePreComponentActionListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    typedef PreComponentActionListener PreCompActionListener;
    typedef PreComponentActionListenerType PreCompActionListenerType;
    void
    addPreComponentActionListener(PreCompActionListenerType listener_type,
                                  PreCompActionListener* listener,
                                  bool autoclean = true);


    template <class Listener>
    PreComponentActionListener*
    addPreComponentActionListener(PreCompActionListenerType listener_type,
                                   Listener& obj,
                                   void (Listener::*memfunc)(UniqueId ec_id))
    {
      class Noname
        : public PreComponentActionListener
      {
      public:
        Noname(Listener& obj, void (Listener::*memfunc)(UniqueId))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(UniqueId ec_id) override
        {
          (m_obj.*m_memfunc)(ec_id);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(UniqueId ec_id);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPreComponentActionListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief PreComponentActionListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PreComponentAction type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePreComponentActionListener(
                                 PreComponentActionListenerType listener_type,
                                 PreComponentActionListener* listener);


    /*!
     * @if jp
     * @brief PostComponentActionListener リスナを追加する
     *
     * ComponentAction 実装関数の呼び出し直後のイベントに関連する各種リ
     * スナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - POST_ON_INITIALIZE:    onInitialize 直後
     * - POST_ON_FINALIZE:      onFinalize 直後
     * - POST_ON_STARTUP:       onStartup 直後
     * - POST_ON_SHUTDOWN:      onShutdown 直後
     * - POST_ON_ACTIVATED:     onActivated 直後
     * - POST_ON_DEACTIVATED:   onDeactivated 直後
     * - POST_ON_ABORTED:       onAborted 直後
     * - POST_ON_ERROR:         onError 直後
     * - POST_ON_RESET:         onReset 直後
     * - POST_ON_EXECUTE:       onExecute 直後
     * - POST_ON_STATE_UPDATE:  onStateUpdate 直後
     *
     * リスナは PostComponentActionListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PostComponentActionListener::operator()(UniqueId ec_id, ReturnCode_t ret)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePostComponentActionListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PostComponentAction type listener
     *
     * This operation adds certain listeners related to ComponentActions
     * post events.
     * The following listener types are available.
     *
     * - POST_ON_INITIALIZE:    after onInitialize
     * - POST_ON_FINALIZE:      after onFinalize
     * - POST_ON_STARTUP:       after onStartup
     * - POST_ON_SHUTDOWN:      after onShutdown
     * - POST_ON_ACTIVATED:     after onActivated
     * - POST_ON_DEACTIVATED:   after onDeactivated
     * - POST_ON_ABORTED:       after onAborted
     * - POST_ON_ERROR:         after onError
     * - POST_ON_RESET:         after onReset
     * - POST_ON_EXECUTE:       after onExecute
     * - POST_ON_STATE_UPDATE:  after onStateUpdate
     *
     * Listeners should have the following function operator().
     *
     * PostComponentActionListener::operator()(UniqueId ec_id, ReturnCode_t ret)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePostComponentActionListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    typedef PostComponentActionListener PostCompActionListener;
    typedef PostComponentActionListenerType PostCompActionListenerType;
    void
    addPostComponentActionListener(
                               PostComponentActionListenerType listener_type,
                               PostComponentActionListener* listener,
                               bool autoclean = true);

    template <class Listener>
    PostComponentActionListener*
    addPostComponentActionListener(PostCompActionListenerType listener_type,
                                   Listener& obj,
                                   void (Listener::*memfunc)(UniqueId ec_id,
                                                             ReturnCode_t ret))
    {
      class Noname
        : public PostComponentActionListener
      {
      public:
        Noname(Listener& obj, void (Listener::*memfunc)(UniqueId, ReturnCode_t))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(UniqueId ec_id, ReturnCode_t ret) override
        {
          (m_obj.*m_memfunc)(ec_id, ret);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(UniqueId ec_id, ReturnCode_t ret);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPostComponentActionListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief PostComponentActionListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PostComponentAction type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePostComponentActionListener(
                                  PostComponentActionListenerType listener_type,
                                  PostComponentActionListener* listener);



    /*!
     * @if jp
     * @brief PortActionListener リスナを追加する
     *
     * Portの追加、削除時にコールバックされる各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - ADD_PORT:    Port追加時
     * - REMOVE_PORT: Port削除時
     *
     * リスナは PortActionListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PortActionListener::operator()(PortProfile& pprof)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePortActionListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PortAction type listener
     *
     * This operation adds certain listeners related to ComponentActions
     * post events.
     * The following listener types are available.
     *
     * - ADD_PORT:    At adding Port
     * - REMOVE_PORT: At removing Port
     *
     * Listeners should have the following function operator().
     *
     * PortActionListener::operator()(RTC::PortProfile pprof)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePortActionListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void
    addPortActionListener(PortActionListenerType listener_type,
                          PortActionListener* listener,
                          bool autoclean = true);

    template <class Listener>
    PortActionListener*
    addPortActionListener(PortActionListenerType listener_type,
                          Listener& obj,
                          void (Listener::*memfunc)(const RTC::PortProfile&))
    {
      class Noname
        : public PortActionListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const RTC::PortProfile&))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const RTC::PortProfile& pprofile) override
        {
          (m_obj.*m_memfunc)(pprofile);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const RTC::PortProfile&);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPortActionListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief PortActionListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PortAction type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePortActionListener(PortActionListenerType listener_type,
                             PortActionListener* listener);



    /*!
     * @if jp
     * @brief ExecutionContextActionListener リスナを追加する
     *
     * ExecutionContextの追加、削除時にコールバックされる各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - ATTACH_EC:    ExecutionContext アタッチ時
     * - DETACH_EC:    ExecutionContext デタッチ時
     *
     * リスナは ExecutionContextActionListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * ExecutionContextActionListener::operator()(UniqueId　ec_id)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removeExecutionContextActionListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding ExecutionContextAction type listener
     *
     * This operation adds certain listeners related to ComponentActions
     * post events.
     * The following listener types are available.
     *
     * - ADD_PORT:    At adding ExecutionContext
     * - REMOVE_PORT: At removing ExecutionContext
     *
     * Listeners should have the following function operator().
     *
     * ExecutionContextActionListener::operator()(UniqueId ec_id)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removeExecutionContextActionListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    typedef ExecutionContextActionListenerType ECActionListenerType;
    typedef ExecutionContextActionListener ECActionListener;
    void addExecutionContextActionListener(ECActionListenerType listener_type,
                                           ECActionListener* listener,
                                           bool autoclean = true);

    template <class Listener>
    ECActionListener*
    addExecutionContextActionListener(ECActionListenerType listener_type,
                                      Listener& obj,
                                      void (Listener::*memfunc)(UniqueId))
    {
      class Noname
        : public ECActionListener
      {
      public:
        Noname(Listener& obj, void (Listener::*memfunc)(UniqueId))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(UniqueId ec_id) override
        {
          (m_obj.*m_memfunc)(ec_id);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(UniqueId);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addExecutionContextActionListener(listener_type, listener, true);
      return listener;
    }


    /*!
     * @if jp
     * @brief ExecutionContextActionListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing ExecutionContextAction type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removeExecutionContextActionListener(ECActionListenerType listener_type,
                                         ECActionListener* listener);


    /*!
     * @if jp
     * @brief PortConnectListener リスナを追加する
     *
     * Portの接続時や接続解除時に呼び出される各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - ON_NOTIFY_CONNECT: notify_connect() 関数内呼び出し直後
     * - ON_NOTIFY_DISCONNECT: notify_disconnect() 呼び出し直後
     * - ON_UNSUBSCRIBE_INTERFACES: notify_disconnect() 内のIF購読解除時
     *
     * リスナは PortConnectListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PortConnectListener::operator()(const char*, ConnectorProfile)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePortConnectListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PortConnect type listener
     *
     * This operation adds certain listeners related to Port's connect actions.
     * The following listener types are available.
     *
     * - ON_NOTIFY_CONNECT: right after entering into notify_connect()
     * - ON_NOTIFY_DISCONNECT: right after entering into notify_disconnect()
     * - ON_UNSUBSCRIBE_INTERFACES: unsubscribing IF in notify_disconnect()
     *
     * Listeners should have the following function operator().
     *
     * PortConnectListener::operator()(const char*, ConnectorProfile)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePortConnectListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void addPortConnectListener(PortConnectListenerType listener_type,
                                           PortConnectListener* listener,
                                           bool autoclean = true);

    template <class Listener>
    PortConnectListener*
    addPortConnectListener(PortConnectListenerType listener_type,
                           Listener& obj,
                           void (Listener::*memfunc)(const char*,
                                                     ConnectorProfile&))
    {
      class Noname
        : public PortConnectListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const char*, ConnectorProfile&))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const char* portname, ConnectorProfile& cprofile) override
        {
          (m_obj.*m_memfunc)(portname, cprofile);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const char*, ConnectorProfile&);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPortConnectListener(listener_type, listener, true);
      return listener;
    }


    /*!
     * @if jp
     * @brief PortConnectListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PortConnect type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePortConnectListener(PortConnectListenerType listener_type,
                              PortConnectListener* listener);

    /*!
     * @if jp
     * @brief PortConnectRetListener リスナを追加する
     *
     * Portの接続時や接続解除時に呼び出される各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - ON_CONNECT_NEXTPORT: notify_connect() 中のカスケード呼び出し直後
     * - ON_SUBSCRIBE_INTERFACES: notify_connect() 中のインターフェース購読直後
     * - ON_CONNECTED: nofity_connect() 接続処理完了時に呼び出される
     * - ON_DISCONNECT_NEXT: notify_disconnect() 中にカスケード呼び出し直後
     * - ON_DISCONNECTED: notify_disconnect() リターン時
     *
     * リスナは PortConnectRetListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PortConnectRetListener::operator()(const char*, ConnectorProfile)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePortConnectRetListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PortConnectRet type listener
     *
     * This operation adds certain listeners related to Port's connect actions.
     * The following listener types are available.
     *
     * - ON_CONNECT_NEXTPORT: after cascade-call in notify_connect()
     * - ON_SUBSCRIBE_INTERFACES: after IF subscribing in notify_connect()
     * - ON_CONNECTED: completed nofity_connect() connection process
     * - ON_DISCONNECT_NEXT: after cascade-call in notify_disconnect()
     * - ON_DISCONNECTED: completed notify_disconnect() disconnection process
     *
     * Listeners should have the following function operator().
     *
     * PortConnectRetListener::operator()(const char*, ConnectorProfile)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePortConnectRetListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void addPortConnectRetListener(PortConnectRetListenerType listener_type,
                                           PortConnectRetListener* listener,
                                           bool autoclean = true);

    template <class Listener>
    PortConnectRetListener*
    addPortConnectRetListener(PortConnectRetListenerType listener_type,
                              Listener& obj,
                              void (Listener::*memfunc)(const char*,
                                                        ConnectorProfile&,
                                                        ReturnCode_t))
    {
      class Noname
        : public PortConnectRetListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const char*,
                                         ConnectorProfile&,
                                         ReturnCode_t))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const char* portname,
                        ConnectorProfile& cprofile,
                        ReturnCode_t ret) override
        {
          (m_obj.*m_memfunc)(portname, cprofile, ret);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const char* portname,
                                          ConnectorProfile& cprofile,
                                          ReturnCode_t ret);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPortConnectRetListener(listener_type, listener, true);
      return listener;
    }


    /*!
     * @if jp
     * @brief PortConnectRetListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PortConnectRet type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePortConnectRetListener(PortConnectRetListenerType listener_type,
                                 PortConnectRetListener* listener);


    /*!
     * @if jp
     *
     * @brief ConfigurationParamListener を追加する
     *
     * update(const char* config_set, const char* config_param) が呼ばれた際に
     * コールされるリスナ ConfigurationParamListener を追加する。
     * type には現在のところ ON_UPDATE_CONFIG_PARAM のみが入る。
     *
     * @param type ConfigurationParamListenerType型の値。
     *             ON_UPDATE_CONFIG_PARAM がある。
     *
     * @param listener ConfigurationParamListener 型のリスナオブジェクト。
     * @param autoclean リスナオブジェクトを自動で削除するかどうかのフラグ
     *
     * @else
     *
     * @brief Adding ConfigurationParamListener
     *
     * This function adds a listener object which is called when
     * update(const char* config_set, const char* config_param) is
     * called. In the type argument, currently only
     * ON_UPDATE_CONFIG_PARAM is allowed.
     *
     * @param type ConfigurationParamListenerType value
     *             ON_UPDATE_CONFIG_PARAM is only allowed.
     *
     * @param listener ConfigurationParamListener listener object.
     * @param autoclean a flag whether if the listener object autocleaned.
     *
     * @endif
     */
    void addConfigurationParamListener(ConfigurationParamListenerType type,
                                       ConfigurationParamListener* listener,
                                       bool autoclean = true);

    template <class Listener>
    ConfigurationParamListener*
    addConfigurationParamListener(ConfigurationParamListenerType listener_type,
                                  Listener& obj,
                                  void (Listener::*memfunc)(const char*,
                                                            const char*))
    {
      class Noname
        : public ConfigurationParamListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const char*, const char*))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const char* config_set_name,
                        const char* config_param_name) override
        {
          (m_obj.*m_memfunc)(config_set_name, config_param_name);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const char*, const char*);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addConfigurationParamListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     *
     * @brief ConfigurationParamListener を削除する
     *
     * addConfigurationParamListener で追加されたリスナオブジェクトを削除する。
     *
     * @param type ConfigurationParamListenerType型の値。
     *             ON_UPDATE_CONFIG_PARAM がある。
     * @param listener 与えたリスナオブジェクトへのポインタ
     *
     * @else
     *
     * @brief Removing ConfigurationParamListener
     *
     * This function removes a listener object which is added by
     * addConfigurationParamListener() function.
     *
     * @param type ConfigurationParamListenerType value
     *             ON_UPDATE_CONFIG_PARAM is only allowed.
     * @param listener a pointer to ConfigurationParamListener listener object.
     *
     * @endif
     */
    void removeConfigurationParamListener(ConfigurationParamListenerType type,
                                          ConfigurationParamListener* listener);

    /*!
     * @if jp
     *
     * @brief ConfigurationSetListener を追加する
     *
     * ConfigurationSet が更新されたときなどに呼ばれるリスナ
     * ConfigurationSetListener を追加する。設定可能なイベントは以下の
     * 2種類がある。
     *
     * - ON_SET_CONFIG_SET: setConfigurationSetValues() で
     *                      ConfigurationSet に値が設定された場合。
     * - ON_ADD_CONFIG_SET: addConfigurationSet() で新しい
     *                      ConfigurationSet が追加された場合。
     *
     * @param type ConfigurationSetListenerType型の値。
     * @param listener ConfigurationSetListener 型のリスナオブジェクト。
     * @param autoclean リスナオブジェクトを自動で削除するかどうかのフラグ
     *
     * @else
     *
     * @brief Adding ConfigurationSetListener
     *
     * This function add a listener object which is called when
     * ConfigurationSet is updated. Available events are the followings.
     *
     * @param type ConfigurationSetListenerType value
     * @param listener ConfigurationSetListener listener object.
     * @param autoclean a flag whether if the listener object autocleaned.
     *
     * @endif
     */
    void addConfigurationSetListener(ConfigurationSetListenerType type,
                                     ConfigurationSetListener* listener,
                                     bool autoclean = true);

    template <class Listener>
    ConfigurationSetListener*
    addConfigurationSetListener(ConfigurationSetListenerType listener_type,
                                Listener& obj,
                                void (Listener::*memfunc)
                                (const coil::Properties& config_set))
    {
      class Noname
        : public ConfigurationSetListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const coil::Properties& config_set))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const coil::Properties& config_set) override
        {
          (m_obj.*m_memfunc)(config_set);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const coil::Properties& config_set);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addConfigurationSetListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     *
     * @brief ConfigurationSetListener を削除する
     *
     * addConfigurationSetListener で追加されたリスナオブジェクトを削除する。
     *
     * @param type ConfigurationSetListenerType型の値。
     * @param listener 与えたリスナオブジェクトへのポインタ
     *
     * @else
     *
     * @brief Removing ConfigurationSetListener
     *
     * This function removes a listener object which is added by
     * addConfigurationSetListener() function.
     *
     * @param type ConfigurationSetListenerType value
     * @param listener a pointer to ConfigurationSetListener listener object.
     *
     * @endif
     */
    void removeConfigurationSetListener(ConfigurationSetListenerType type,
                                        ConfigurationSetListener* listener);

    /*!
     * @if jp
     *
     * @brief ConfigurationSetNameListener を追加する
     *
     * ConfigurationSetName が更新されたときなどに呼ばれるリスナ
     * ConfigurationSetNameListener を追加する。設定可能なイベントは以下の
     * 3種類がある。
     *
     * - ON_UPDATE_CONFIG_SET: ある ConfigurationSet がアップデートされた
     * - ON_REMOVE_CONFIG_SET: ある ConfigurationSet が削除された
     * - ON_ACTIVATE_CONFIG_SET: ある ConfigurationSet がアクティブ化された
     *
     * @param type ConfigurationSetNameListenerType型の値。
     * @param listener ConfigurationSetNameListener 型のリスナオブジェクト。
     * @param autoclean リスナオブジェクトを自動で削除するかどうかのフラグ
     *
     * @else
     *
     * @brief Adding ConfigurationSetNameListener
     *
     * This function add a listener object which is called when
     * ConfigurationSetName is updated. Available events are the followings.
     *
     * - ON_UPDATE_CONFIG_SET: A ConfigurationSet has been updated.
     * - ON_REMOVE_CONFIG_SET: A ConfigurationSet has been deleted.
     * - ON_ACTIVATE_CONFIG_SET: A ConfigurationSet has been activated.
     *
     * @param type ConfigurationSetNameListenerType value
     * @param listener ConfigurationSetNameListener listener object.
     * @param autoclean a flag whether if the listener object autocleaned.
     *
     * @endif
     */
    void
    addConfigurationSetNameListener(ConfigurationSetNameListenerType type,
                                    ConfigurationSetNameListener* listener,
                                    bool autoclean = true);

    template <class Listener>
    ConfigurationSetNameListener*
    addConfigurationSetNameListener(ConfigurationSetNameListenerType type,
                                    Listener& obj,
                                    void (Listener::*memfunc)(const char*))
    {
      class Noname
        : public ConfigurationSetNameListener
      {
      public:
        Noname(Listener& obj, void (Listener::*memfunc)(const char*))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const char* config_set_name) override
        {
          (m_obj.*m_memfunc)(config_set_name);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const char*);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addConfigurationSetNameListener(type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     *
     * @brief ConfigurationSetNameListener を削除する
     *
     * addConfigurationSetNameListener で追加されたリスナオブジェクトを
     * 削除する。
     *
     * @param type ConfigurationSetNameListenerType型の値。
     *             ON_UPDATE_CONFIG_PARAM がある。
     * @param listener 与えたリスナオブジェクトへのポインタ
     *
     * @else
     *
     * @brief Removing ConfigurationSetNameListener
     *
     * This function removes a listener object which is added by
     * addConfigurationSetNameListener() function.
     *
     * @param type ConfigurationSetNameListenerType value
     *             ON_UPDATE_CONFIG_PARAM is only allowed.
     * @param listener a pointer to ConfigurationSetNameListener
     *             listener object.
     *
     * @endif
     */
    void
    removeConfigurationSetNameListener(ConfigurationSetNameListenerType type,
                                       ConfigurationSetNameListener* listener);

    //============================================================
    // FSM Listener
    /*!
     * @if jp
     * @brief PreFsmActionListener リスナを追加する
     *
     * FsmAction 実装関数の呼び出し直前のイベントに関連する各種リ
     * スナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - PRE_ON_INITIALIZE:    onInitialize 直前
     * - PRE_ON_FINALIZE:      onFinalize 直前
     * - PRE_ON_STARTUP:       onStartup 直前
     * - PRE_ON_SHUTDOWN:      onShutdown 直前
     * - PRE_ON_ACTIVATED:     onActivated 直前
     * - PRE_ON_DEACTIVATED:   onDeactivated 直前
     * - PRE_ON_ABORTED:       onAborted 直前
     * - PRE_ON_ERROR:         onError 直前
     * - PRE_ON_RESET:         onReset 直前
     * - PRE_ON_EXECUTE:       onExecute 直前
     * - PRE_ON_STATE_UPDATE:  onStateUpdate 直前
     *
     * リスナは PreFsmActionListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PreFsmActionListener::operator()(UniqueId ec_id)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePreFsmActionListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PreFsmAction type listener
     *
     * This operation adds certain listeners related to FsmActions
     * pre events.
     * The following listener types are available.
     *
     * - PRE_ON_INITIALIZE:    before onInitialize
     * - PRE_ON_FINALIZE:      before onFinalize
     * - PRE_ON_STARTUP:       before onStartup
     * - PRE_ON_SHUTDOWN:      before onShutdown
     * - PRE_ON_ACTIVATED:     before onActivated
     * - PRE_ON_DEACTIVATED:   before onDeactivated
     * - PRE_ON_ABORTED:       before onAborted
     * - PRE_ON_ERROR:         before onError
     * - PRE_ON_RESET:         before onReset
     * - PRE_ON_EXECUTE:       before onExecute
     * - PRE_ON_STATE_UPDATE:  before onStateUpdate
     *
     * Listeners should have the following function operator().
     *
     * PreFsmActionListener::operator()(UniqueId ec_id)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePreFsmActionListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void 
    addPreFsmActionListener(PreFsmActionListenerType listener_type,
                                  PreFsmActionListener* listener,
                                  bool autoclean = true);


    template <class Listener>
    PreFsmActionListener*
    addPreFsmActionListener(PreFsmActionListenerType listener_type,
                                   Listener& obj,
                                   void (Listener::*memfunc)(const char* state))
    {
      class Noname
        : public PreFsmActionListener
      {
      public:
        Noname(Listener& obj, void (Listener::*memfunc)(const char*))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const char* state) override
        {
          (m_obj.*m_memfunc)(state);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const char* state);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPreFsmActionListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief PreFsmActionListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     * 
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PreFsmAction type listener
     *
     * This operation removes a specified listener.
     *     
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePreFsmActionListener(
                               PreFsmActionListenerType listener_type,
                               PreFsmActionListener* listener);


    /*!
     * @if jp
     * @brief PostFsmActionListener リスナを追加する
     *
     * FsmAction 実装関数の呼び出し直後のイベントに関連する各種リ
     * スナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - POST_ON_INITIALIZE:    onInitialize 直後
     * - POST_ON_FINALIZE:      onFinalize 直後
     * - POST_ON_STARTUP:       onStartup 直後
     * - POST_ON_SHUTDOWN:      onShutdown 直後
     * - POST_ON_ACTIVATED:     onActivated 直後
     * - POST_ON_DEACTIVATED:   onDeactivated 直後
     * - POST_ON_ABORTED:       onAborted 直後
     * - POST_ON_ERROR:         onError 直後
     * - POST_ON_RESET:         onReset 直後
     * - POST_ON_EXECUTE:       onExecute 直後
     * - POST_ON_STATE_UPDATE:  onStateUpdate 直後
     *
     * リスナは PostFsmActionListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * PostFsmActionListener::operator()(const char* state, ReturnCode_t ret)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removePostFsmActionListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding PostFsmAction type listener
     *
     * This operation adds certain listeners related to FsmActions
     * post events.
     * The following listener types are available.
     *
     * - POST_ON_INITIALIZE:    after onInitialize
     * - POST_ON_FINALIZE:      after onFinalize
     * - POST_ON_STARTUP:       after onStartup
     * - POST_ON_SHUTDOWN:      after onShutdown
     * - POST_ON_ACTIVATED:     after onActivated
     * - POST_ON_DEACTIVATED:   after onDeactivated
     * - POST_ON_ABORTED:       after onAborted
     * - POST_ON_ERROR:         after onError
     * - POST_ON_RESET:         after onReset
     * - POST_ON_EXECUTE:       after onExecute
     * - POST_ON_STATE_UPDATE:  after onStateUpdate
     *
     * Listeners should have the following function operator().
     *
     * PostFsmActionListener::operator()(const char* state, ReturnCode_t ret)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removePostFsmActionListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void
    addPostFsmActionListener(PostFsmActionListenerType listener_type,
                             PostFsmActionListener* listener,
                               bool autoclean = true);

    template <class Listener>
    PostFsmActionListener*
    addPostFsmActionListener(PostFsmActionListenerType listener_type,
                             Listener& obj,
                             void (Listener::*memfunc)(const char* state,
                                                       ReturnCode_t ret))
    {
      class Noname
        : public PostFsmActionListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const char*, ReturnCode_t))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const char* state, ReturnCode_t ret) override
        {
          (m_obj.*m_memfunc)(state, ret);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const char* state, ReturnCode_t ret);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addPostFsmActionListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief PostFsmActionListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing PostFsmAction type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removePostFsmActionListener(PostFsmActionListenerType listener_type,
                                PostFsmActionListener* listener);

    /*!
     * @if jp
     * @brief FsmProfileListener リスナを追加する
     *
     * FSMへのプロファイルの設定、取得時、またFSM自体への状態や遷移、イ
     * ベントの追加削除時にコールバックされる各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - SET_FSM_PROFILE       : FSM Profile設定時
     * - GET_FSM_PROFILE       : FSM Profile取得時
     * - ADD_FSM_STATE         : FSMにStateが追加された
     * - REMOVE_FSM_STATE      : FSMからStateが削除された
     * - ADD_FSM_TRANSITION    : FSMに遷移が追加された
     * - REMOVE_FSM_TRANSITION : FSMから遷移が削除された
     * - BIND_FSM_EVENT        : FSMにイベントがバインドされた
     * - UNBIND_FSM_EVENT      : FSMにイベントがアンバインドされた
     *
     * リスナは FsmProfileListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * FsmProfileListener::operator()(RTC::FsmProfile& pprof)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removeFsmProfileListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding FsmProfile type listener
     *
     * This operation adds certain listeners that is called when
     * setting/getting FsmProfile and stae/transition/event add/remove
     * to/from the FSM itself.
     *
     * The following listener types are available.
     *
     * - SET_FSM_PROFILE       : Setting FSM Profile
     * - GET_FSM_PROFILE       : Getting FSM Profile
     * - ADD_FSM_STATE         : A State added to the FSM
     * - REMOVE_FSM_STATE      : A State removed from FSM
     * - ADD_FSM_TRANSITION    : A transition added to the FSM
     * - REMOVE_FSM_TRANSITION : A transition removed from FSM
     * - BIND_FSM_EVENT        : An event bounded to the FSM
     * - UNBIND_FSM_EVENT      : An event unbounded to the FSM
     *
     * Listeners should have the following function operator().
     *
     * FsmProfileListener::operator()(RTC::PortProfile pprof)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removeFsmProfileListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void
    addFsmProfileListener(FsmProfileListenerType listener_type,
                          FsmProfileListener* listener,
                          bool autoclean = true);

    template <class Listener>
    FsmProfileListener*
    addFsmProfileListener(FsmProfileListenerType listener_type,
                          Listener& obj,
                          void (Listener::*memfunc)(const RTC::PortProfile&))
    {
      class Noname
        : public FsmProfileListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const RTC::FsmProfile&))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const RTC::PortProfile& pprofile)
        {
          (m_obj.*m_memfunc)(pprofile);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const RTC::FsmProfile&);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addFsmProfileListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief FsmProfileListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing FsmProfile type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removeFsmProfileListener(FsmProfileListenerType listener_type,
                             FsmProfileListener* listener);


    /*!
     * @if jp
     * @brief FsmStructureListener リスナを追加する
     *
     * ExtendedFsmService に関連する FSM structure の設定・取得時にコー
     * ルバックされる各種リスナを設定する。
     *
     * 設定できるリスナのタイプとコールバックイベントは以下の通り
     *
     * - SET_FSM_STRUCTURE: FSM構造の設定
     * - GET_FSM_STRUCTURE: FSM構造の取得
     *
     * リスナは FsmStructureListener を継承し、以下のシグニチャを持つ
     * operator() を実装している必要がある。
     *
     * FsmStructureListener::operator()(FsmStructure& structure)
     *
     * デフォルトでは、この関数に与えたリスナオブジェクトの所有権は
     * RTObjectに移り、RTObject解体時もしくは、
     * removeFsmStructureListener() により削除時に自動的に解体される。
     * リスナオブジェクトの所有権を呼び出し側で維持したい場合は、第3引
     * 数に false を指定し、自動的な解体を抑制することができる。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     * @param autoclean リスナオブジェクトの自動的解体を行うかどうかのフラグ
     *
     * @else
     * @brief Adding FsmStructure type listener
     *
     * This operation adds certain listeners related to FSM structure
     * data which are handled by ExtendedFsmService.
     *
     * The following listener types are available.
     *
     * - SET_FSM_STRUCTURE: Setting FSM structure
     * - GET_FSM_STRUCTURE: Getting FSM structure
     *
     * Listeners should have the following function operator().
     *
     * FsmStructureListener::operator()(RTC::FsmStructure structure)
     *
     * The ownership of the given listener object is transferred to
     * this RTObject object in default.  The given listener object will
     * be destroied automatically in the RTObject's dtor or if the
     * listener is deleted by removeFsmStructureListener() function.
     * If you want to keep ownership of the listener object, give
     * "false" value to 3rd argument to inhibit automatic destruction.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     * @param autoclean A flag for automatic listener destruction
     *
     * @endif
     */
    void
    addFsmStructureListener(FsmStructureListenerType listener_type,
                            FsmStructureListener* listener,
                            bool autoclean = true);

    template <class Listener>
    FsmStructureListener*
    addFsmStructureListener(FsmStructureListenerType listener_type,
                            Listener& obj,
                            void (Listener::*memfunc)(const RTC::PortProfile&))
    {
      class Noname
        : public FsmStructureListener
      {
      public:
        Noname(Listener& obj,
               void (Listener::*memfunc)(const RTC::FsmStructure&))
          : m_obj(obj), m_memfunc(memfunc)
        {
        }
        void operator()(const RTC::PortProfile& pprofile)
        {
          (m_obj.*m_memfunc)(pprofile);
        }
      private:
        Listener& m_obj;
        typedef void (Listener::*Memfunc)(const RTC::FsmStructure&);
        Memfunc m_memfunc;
      };
      Noname* listener(new Noname(obj, memfunc));
      addFsmStructureListener(listener_type, listener, true);
      return listener;
    }

    /*!
     * @if jp
     * @brief FsmStructureListener リスナを削除する
     *
     * 設定した各種リスナを削除する。
     *
     * @param listener_type リスナタイプ
     * @param listener リスナオブジェクトへのポインタ
     *
     * @else
     * @brief Removing FsmStructure type listener
     *
     * This operation removes a specified listener.
     *
     * @param listener_type A listener type
     * @param listener A pointer to a listener object
     *
     * @endif
     */
    void
    removeFsmStructureListener(FsmStructureListenerType listener_type,
                               FsmStructureListener* listener);



    //  protected:
    /*!
     * @if jp
     *
     * @brief RTC を終了する
     *
     * RTC の終了処理を実行する。
     * 保持している全 Port の登録を解除するとともに、該当する CORBA オブジェクト
     * を非活性化し、RTC を終了する。
     *
     * @else
     *
     * @brief Shutdown RTC
     *
     * This operation ececutes RTC's termination.
     * This unregisters all Ports, deactivates corresponding CORBA objects and
     * shuts down RTC.
     *
     * @endif
     */
    void shutdown();

    inline void preOnInitialize(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_INITIALIZE].notify(ec_id);
    }

    inline void preOnFinalize(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_FINALIZE].notify(ec_id);
    }

    inline void preOnStartup(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_STARTUP].notify(ec_id);
    }

    inline void preOnShutdown(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_SHUTDOWN].notify(ec_id);
    }

    inline void preOnActivated(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_ACTIVATED].notify(ec_id);
    }

    inline void preOnDeactivated(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_DEACTIVATED].notify(ec_id);
    }

    inline void preOnAborting(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_ABORTING].notify(ec_id);
    }

    inline void preOnError(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_ERROR].notify(ec_id);
    }

    inline void preOnReset(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_RESET].notify(ec_id);
    }

    inline void preOnExecute(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_EXECUTE].notify(ec_id);
    }

    inline void preOnStateUpdate(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_STATE_UPDATE].notify(ec_id);
    }

    inline void preOnRateChanged(UniqueId ec_id)
    {
      m_actionListeners.preaction_[PRE_ON_RATE_CHANGED].notify(ec_id);
    }

    inline void postOnInitialize(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_INITIALIZE].notify(ec_id, ret);
    }

    inline void postOnFinalize(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_FINALIZE].notify(ec_id, ret);
    }

    inline void postOnStartup(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_STARTUP].notify(ec_id, ret);
    }

    inline void postOnShutdown(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_SHUTDOWN].notify(ec_id, ret);
    }

    inline void postOnActivated(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_ACTIVATED].notify(ec_id, ret);
    }

    inline void postOnDeactivated(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_DEACTIVATED].notify(ec_id, ret);
    }

    inline void postOnAborting(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_ABORTING].notify(ec_id, ret);
    }

    inline void postOnError(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_ERROR].notify(ec_id, ret);
    }

    inline void postOnReset(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_RESET].notify(ec_id, ret);
    }

    inline void postOnExecute(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_EXECUTE].notify(ec_id, ret);
    }

    inline void postOnStateUpdate(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_STATE_UPDATE].notify(ec_id, ret);
    }

    inline void postOnRateChanged(UniqueId ec_id, ReturnCode_t ret)
    {
      m_actionListeners.postaction_[POST_ON_RATE_CHANGED].notify(ec_id, ret);
    }

    inline void onAddPort(const PortProfile& pprof)
    {
      m_actionListeners.portaction_[ADD_PORT].notify(pprof);
    }

    inline void onRemovePort(const PortProfile& pprof)
    {
      m_actionListeners.portaction_[REMOVE_PORT].notify(pprof);
    }

    inline void onAttachExecutionContext(UniqueId ec_id)
    {
      m_actionListeners.ecaction_[EC_ATTACHED].notify(ec_id);
    }

    inline void onDetachExecutionContext(UniqueId ec_id)
    {
      m_actionListeners.ecaction_[EC_DETACHED].notify(ec_id);
    }

    /*!
     * FSM related listeners
     */
    inline void preOnFsmInit(const char* state)
    {
      m_fsmActionListeners.preaction_[PRE_ON_INIT].notify(state);
    }
    inline void preOnFsmEntry(const char* state)
    {
      m_fsmActionListeners.preaction_[PRE_ON_ENTRY].notify(state);
    }
    inline void preOnFsmDo(const char* state)
    {
      m_fsmActionListeners.preaction_[PRE_ON_DO].notify(state);
    }
    inline void preOnFsmExit(const char* state)
    {
      m_fsmActionListeners.preaction_[PRE_ON_EXIT].notify(state);
    }
    inline void preOnFsmStateChange(const char* state)
    {
      m_fsmActionListeners.preaction_[PRE_ON_STATE_CHANGE].notify(state);
    }
    /*!
     * FSM related listeners
     */
    inline void postOnFsmInit(const char* state, ReturnCode_t ret)
    {
      m_fsmActionListeners.postaction_[POST_ON_INIT].notify(state, ret);
    }
    inline void postOnFsmEntry(const char* state, ReturnCode_t ret)
    {
      m_fsmActionListeners.postaction_[POST_ON_ENTRY].notify(state, ret);
    }
    inline void postOnFsmDo(const char* state, ReturnCode_t ret)
    {
      m_fsmActionListeners.postaction_[POST_ON_DO].notify(state, ret);
    }
    inline void postOnFsmExit(const char* state, ReturnCode_t ret)
    {
      m_fsmActionListeners.postaction_[POST_ON_EXIT].notify(state, ret);
    }
    inline void postOnFsmStateChange(const char* state, ReturnCode_t ret)
    {
      m_fsmActionListeners.postaction_[POST_ON_STATE_CHANGE].notify(state, ret);
    }

    
    ReturnCode_t getInheritedECOptions(coil::Properties& default_opts);

    /*!
     * @brief getting individual EC options from RTC's configuration file
     */
    ReturnCode_t
    getPrivateContextOptions(std::vector<coil::Properties>& ec_args);

    /*!
     * @brief getting global EC options from rtc.conf
     */
    ReturnCode_t
    getGlobalContextOptions(coil::Properties& global_ec_props);

    /*!
     * @brief getting EC options
     */
    ReturnCode_t
    getContextOptions(std::vector<coil::Properties>& ec_args);

    /*!
     * @brief fiding existing EC from the factory
     */
    ReturnCode_t findExistingEC(coil::Properties& ec_arg,
                                RTC::ExecutionContextBase*& ec);
    /*!
     * @brief creating, initializing and binding context
     */
    ReturnCode_t createContexts(std::vector<coil::Properties>& ec_args);


  protected:
    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    mutable Logger rtclog;
    /*!
     * @if jp
     * @brief マネージャオブジェクト
     * @else
     * @brief Manager object
     * @endif
     */
    Manager* m_pManager;

    /*!
     * @if jp
     * @brief ORB へのポインタ
     * @else
     * @brief The pointer to the ORB
     * @endif
     */
    CORBA::ORB_var m_pORB;

    /*!
     * @if jp
     * @brief POA へのポインタ
     * @else
     * @brief The pointer to the POA
     * @endif
     */
    PortableServer::POA_var m_pPOA;

    //============================================================
    // SDO 関係の変数
    //============================================================
    /*!
     * @if jp
     * @brief SDO が保持する organization のリスト
     * @else
     * @brief SDO owned organization list
     * @endif
     */
    SDOPackage::OrganizationList m_sdoOwnedOrganizations;

    /*!
     * @if jp
     * @brief SDOService のプロファイルリストからidでサーチするためのファンクタ
     * @else
     * @brief Functor to find from SDOService Profile List by id
     * @endif
     */
    struct svc_name
    {
      explicit svc_name(const char* id) : m_id(id) {}
      bool operator()(const SDOPackage::ServiceProfile& prof)
      {
        return m_id == std::string(prof.id);
      }
      std::string m_id;
    };  // struct svc_name

    /*!
     * @if jp
     * @brief SDO Configuration オブジェクトへのポインタ
     * @else
     * @brief The pointer to the SDO Configuration Object
     * @endif
     */
    SDOPackage::Configuration_impl* m_pSdoConfigImpl;

    /*!
     * @if jp
     * @brief SDO Configuration Interface へのポインタ
     * @else
     * @brief The pointer to the SDO Configuration Interface
     * @endif
     */
    SDOPackage::Configuration_var  m_pSdoConfig;

    /*!
     * @if jp
     * @brief SDO organization
     * @else
     * @brief SDO organization
     * @endif
     */
    SDOPackage::OrganizationList m_sdoOrganizations;

    /*!
     * @if jp
     * @brief SDO Status
     * @else
     * @brief SDO Status
     * @endif
     */
    SDOPackage::NVList m_sdoStatus;

    //============================================================
    // RTC 関係の変数
    //============================================================
    /*!
     * @if jp
     * @brief コンポーネントプロファイル
     * @else
     * @brief ComponentProfile
     * @endif
     */
    ComponentProfile m_profile;

    /*!
     * @if jp
     * @brief オブジェクトリファレンス
     * @else
     * @brief Object reference
     * @endif
     */
    RTObject_var m_objref;

    /*!
     * @if jp
     * @brief Port のオブジェクトリファレンスのリスト
     * @else
     * @brief List of Port Object reference
     * @endif
     */
    PortAdmin m_portAdmin;

    /*!
     * @if jp
     * @brief InPortBase* のリスト
     * @else
     * @brief List of InPortBase*
     * @endif
     */
    std::vector<InPortBase*> m_inports;

    /*!
     * @if jp
     * @brief OutPortBase* のリスト
     * @else
     * @brief List of OutPortBase*
     * @endif
     */
    std::vector<OutPortBase*> m_outports;

    /*!
     * @if jp
     * @brief 自分がownerのExecutionContextService のリスト
     * @else
     * @brief List of owned ExecutionContextService
     * @endif
     */
    ExecutionContextServiceList m_ecMine;

    /*!
     * @if jp
     * @brief ExecutionContextBase のリスト
     * @else
     * @brief List of ExecutionContextBase
     * @endif
     */
    std::vector<ExecutionContextBase*> m_eclist;

    /*!
     * @if jp
     * @brief 参加しているExecutionContextService のリスト
     * @else
     * @brief List of participating ExecutionContextService
     * @endif
     */
    ExecutionContextServiceList m_ecOther;

    /*!
     * @if jp
     * @brief Created 状態フラグ
     * @else
     * @brief Created Status Flag
     * @endif
     */
    bool m_created;

    /*!
     * @if jp
     * @brief RTCの終了状態フラグ
     * @else
     * @brief RTC Finalize Status Flag
     * @endif
     */
    bool m_exiting;

    /*!
     * @if jp
     * @brief Alive 状態フラグ
     * @else
     * @brief Alive Status Flag
     * @endif
     */
    //    bool m_alive;

    /*!
     * @if jp
     * @brief RTC のプロパティ
     * @else
     * @brief RTC's Property
     * @endif
     */
    coil::Properties m_properties;

    /*!
     * @if jp
     * @brief コンフィギュレーション情報管理オブジェクト
     * @else
     * @brief Configuration Administrator Object
     * @endif
     */
    ConfigAdmin m_configsets;

    /*!
     * @if jp
     * @brief SDO Service 管理オブジェクト
     * @else
     * @brief SDO Service Administrator Object
     * @endif
     */
    SdoServiceAdmin m_sdoservice;

    /*!
     * @if jp
     * @brief readAll()呼出用のフラグ
     * @else
     * @brief flag for readAll()
     * @endif
     */
    bool m_readAll;

    /*!
     * @if jp
     * @brief writeAll()呼出用のフラグ
     * @else
     * @brief flag for writeAll()
     * @endif
     */
    bool m_writeAll;

    /*!
     * @if jp
     * @brief readAll()用のフラグ
     *
     * true:readAll()の途中ででエラーが発生しても最後まで実施する。
     * false:readAll()の途中ででエラーが発生した場合終了。
     *
     * @else
     * @brief flag for readAll()
     *
     * true:Even if the error occurs during readAll(), it executes it to the
     *      last minute.
     * false:End when error occurs during readAll().
     *
     * @endif
     */
    bool m_readAllCompletion;

    /*!
     * @if jp
     * @brief writeAll()用のフラグ
     *
     * true:writeAll()の途中ででエラーが発生しても最後まで実施する。
     * false:writeAll()の途中ででエラーが発生した場合終了。
     *
     * @else
     * @brief flag for writeAll()
     *
     * true:Even if the error occurs during writeAll(), it executes it to the
     *      last minute.
     * false:End when error occurs during writeAll().
     *
     * @endif
     */
    bool m_writeAllCompletion;

    /*!
     * @if jp
     * @brief ComponentActionListenerホルダ
     *
     * ComponentActionListenrを保持するホルダ
     *
     * @else
     * @brief ComponentActionListener holder
     *
     * Holders of ComponentActionListeners
     *
     * @endif
     */
    ComponentActionListeners m_actionListeners;

    /*!
     * @if jp
     * @brief PortConnectListenerホルダ
     *
     * PortConnectListenrを保持するホルダ
     *
     * @else
     * @brief PortConnectListener holder
     *
     * Holders of PortConnectListeners
     *
     * @endif
     */
    PortConnectListeners m_portconnListeners;

    /*!
     * @if jp
     * @brief ComponentActionListenerホルダ
     *
     * ComponentActionListenrを保持するホルダ
     *
     * @else
     * @brief ComponentActionListener holder
     *
     * Holders of ComponentActionListeners
     *
     * @endif
     */
    FsmActionListeners m_fsmActionListeners;

    RTC::LightweightRTObject_var m_insref;


    class SdoServiceConsumerTerminator
		: public coil::Task
    {
    public:
        SdoServiceConsumerTerminator()
          {

          }
        void setSdoServiceConsumer(SdoServiceAdmin* sdoservice, const char* id)
          {
            m_sdoservice = sdoservice;
            m_id = id;
          }
        int svc() override
          {
            m_sdoservice->removeSdoServiceConsumer(m_id.c_str());
            return 0;
          }
    private:
        SdoServiceAdmin *m_sdoservice;
        std::string m_id;
    };
    SdoServiceConsumerTerminator *m_sdoconterm;


    //------------------------------------------------------------
    // Functor
    //------------------------------------------------------------
    /*!
     * @if jp
     * @brief NVList 検索用ファンクタ
     * @else
     * @brief Functor to find NVList
     * @endif
     */
    struct nv_name
    {
      explicit nv_name(const char* name) : m_name(name) {}
      bool operator()(const SDOPackage::NameValue& nv)
      {
        return m_name == std::string(nv.name);
      }
      std::string m_name;
    };  // struct nv_name

    /*!
     * @if jp
     * @brief ExecutionContext コピーファンクタ
     * @else
     * @brief Functor to copy ExecutionContext
     * @endif
     */
    struct ec_copy
    {
      explicit ec_copy(ExecutionContextList& eclist)
        : m_eclist(eclist)
      {
      }
      void operator()(ExecutionContextService_ptr ecs)
      {
        if (!::CORBA::is_nil(ecs))
          {
            CORBA_SeqUtil::push_back(m_eclist,
                                     ExecutionContext::_duplicate(ecs));
          }
      }
      ExecutionContextList& m_eclist;
    };  // struct ec_copy
    /*!
     * @if jp
     * @brief ExecutionContext 検索用ファンクタ
     * @else
     * @brief Functor to find ExecutionContext
     * @endif
     */
    struct ec_find
    {
      explicit ec_find(ExecutionContext_ptr& ec)
        : m_ec(ExecutionContext::_duplicate(ec))
      {
      }
#ifdef ORB_IS_ORBEXPRESS
      bool operator()(ExecutionContextService_var ecs)
      {
	try
	  {
            if (!::CORBA::is_nil(ecs.in()))
              {
  	        ExecutionContext_var ec;
	        ec = ExecutionContext::_narrow(ecs.in());
	        return m_ec->_is_equivalent(ec);
              }
	  }
#else
      bool operator()(ExecutionContextService_ptr ecs)
      {
        try
          {
            if (!::CORBA::is_nil(ecs))
              {
                ExecutionContext_var ec;
                ec = ExecutionContext::_narrow(ecs);
                return m_ec->_is_equivalent(ec);
              }
	  }
#endif
	catch (...)
	  {
	    return false;
	  }
	return false;
      }
      ExecutionContext_var m_ec;

    };  // struct ec_find
    //    ExecutionContextAdminList m_execContextList;

    /*!
     * @if jp
     * @brief RTC 非活性化用ファンクタ
     * @else
     * @brief Functor to deactivate RTC
     * @endif
     */
    struct deactivate_comps
    {
      explicit deactivate_comps(LightweightRTObject_ptr comp)
        : m_comp(RTC::LightweightRTObject::_duplicate(comp))
      {
      }
      void operator()(ExecutionContextService_ptr ec)
      {
        if (!::CORBA::is_nil(ec) && !ec->_non_existent())
          {
            if (ec->get_component_state(m_comp) == RTC::ACTIVE_STATE)
              {
                ec->deactivate_component(
                    RTC::LightweightRTObject::_duplicate(m_comp));
              }
            else if (ec->get_component_state(m_comp) == RTC::ERROR_STATE)
              {
                ec->reset_component(
                    RTC::LightweightRTObject::_duplicate(m_comp));
              }
            
          }
      }
      LightweightRTObject_var m_comp;
    };  // struct deactivate_comps
  };  // class RTObject_impl
} // namespace RTC

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif  // RTC_RTOBJECT
