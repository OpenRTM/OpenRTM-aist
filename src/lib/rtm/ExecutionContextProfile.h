// -*- C++ -*-
/*!
 * @file ExecutionContextProfile.h
 * @brief ExecutionContextProfile class
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

#ifndef RTC_EXECUTIONCONTEXTPROFILE_H
#define RTC_EXECUTIONCONTEXTPROFILE_H

#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <coil/Properties.h>
#include <coil/TimeValue.h>

#include <rtm/idl/RTCStub.h>
#include <rtm/SystemLogger.h>

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC_impl
{
  /*!
   * @if jp
   * @class ExecutionContextProfile
   * @brief ExecutionContextProfile クラス
   *
   * Periodic Sampled Data Processing(周期実行用)ExecutionContextクラス。
   *
   * @since 0.4.0
   *
   * @else
   * @class ExecutionContextProfile
   * @brief ExecutionContextProfile class
   *
   * Periodic Sampled Data Processing (for the execution cycles)
   * ExecutionContext class
   *
   * @since 0.4.0
   *
   * @endif
   */
  class ExecutionContextProfile
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
    explicit ExecutionContextProfile(RTC::ExecutionKind kind = RTC::PERIODIC);

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
    virtual ~ExecutionContextProfile();

    /*!
     * @if jp
     * @brief CORBA オブジェクト参照のセット
     *
     * ExecutioncontextService の CORBA オブジェクト参照をセットする。
     * セットされると、それまでセットされていたオブジェクト参照は
     * releaseされる。セットするオブジェクト参照は有効な参照でなければ
     * ならない。
     *
     * @param ec_ptr ExecutionContextServiceのCORBAオブジェクト参照
     *
     * @else
     * @brief Setting a CORBA object reference
     *
     * This operation sets a object reference to
     * ExecutionContextService.  After setting a new object reference,
     * old reference is released.  The object reference have to be
     * valid reference.
     *
     * @param ec_ptr A CORBA object reference of ExecutionContextService
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
     * @brief Get the reference to the CORBA object
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
    RTC::ReturnCode_t setPeriod(double period);
    RTC::ReturnCode_t setPeriod(coil::TimeValue period);

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
    const char* getKindString() const
    {
      return getKindString(m_profile.kind);
    }

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
     * @brief RTコンポーネントの参加者リストを取得する
     *
     * 現在登録されている参加者RTCのリストを取得する。この関数はコンポー
     * ネントリストのメンバ変数への参照を返すので、リスト使用前に
     * ExecutionContextProfile::lock() でロックし、リスト使用後は
     * ExecutionContextProfile::unlock() でロックを開放しなければならな
     * い。
     *
     * @return 参加者RTCのリスト
     *
     * @else
     *
     * @brief Getting participant RTC list
     *
     * This function returns a list of participant RTC of the
     * execution context.  Since the function returns a reference to
     * the member variable of component list, user have to lock by
     * ExecutionContextProfile::lock() before using the list, and user
     * also have to release the unlock by
     * ExecutionContextProfile::unlock().
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

    /*!
     * @if jp
     * @brief ExecutionContextProfileをロックする
     *
     * このオブジェクトが管理する RTC::ExecutionContextProfile をロックする。
     * ロックが不要になった際にはunlock()でロックを解除しなければならない。
     *
     * @else
     * @brief Getting a lock of RTC::ExecutionContextProfile
     *
     * This function locks  RTC::ExecutionContextProfile in the object.
     * The lock should be released when the lock is unneccessary.
     *
     * @endif
     */
    void lock() const;

    /*!
     * @if jp
     * @brief ExecutionContextProfileをアンロックする
     *
     * このオブジェクトが管理する RTC::ExecutionContextProfile をアンロッ
     * クする。
     *
     * @else
     * @brief Release a lock of the RTC::ExecutionContextProfile
     *
     * This function release the lock of RTC::ExecutionContextProfile
     * in the object.
     *
     * @endif
     */
    void unlock() const;

  private:
    /*!
     * @if jp
     * @brief Logger
     * @else
     * @brief Logger
     * @endif
     */
    mutable RTC::Logger rtclog;
    /*!
     * @if jp
     * @brief ECProfile
     * @else
     * @brief ECProfile
     * @endif
     */
    RTC::ExecutionContextProfile m_profile;

    /*!
     * @if jp
     * @brief mutex of ExecutionContextProfile
     * @else
     * @brief mutex ExecutionContextProfile
     * @endif
     */
    mutable coil::Mutex m_profileMutex;

    /*!
     * @if jp
     * @brief ExecutionContext の実行周期
     * @else
     * @brief Execution cycle of ExecutionContext
     * @endif
     */
    coil::TimeValue m_period;

    /*!
     * @if jp
     * @brief ExecutionContextService オブジェクトへの参照
     * @else
     * @brief Reference to ExecutionContextService object
     * @endif
     */
    RTC::ExecutionContextService_var m_ref;

    class find_participant
    {
      RTC::RTObject_var m_comp;
    public:
      explicit find_participant(RTC::RTObject_ptr comp)
        : m_comp(RTC::RTObject::_duplicate(comp)) {}
#ifdef ORB_IS_ORBEXPRESS
      bool operator()(RTC::RTObject_var comp)
      {
        return m_comp->_is_equivalent(comp.in());
      }
#else
      bool operator()(RTC::RTObject_ptr comp)
      {
        return m_comp->_is_equivalent(comp);
      }
#endif
    };
  };  // class ExecutionContextProfile
} // namespace RTC_impl

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif  // RTC_EXECUTIONCONTEXTPROFILE_H
