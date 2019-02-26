// -*- C++ -*-
/*!
 * @file PeriodicECSharedComposite.h
 * @brief Periodic Execution Context Shared Composite Component class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_PERIODICECSHAREDCOMPOSITE_H
#define RTC_PERIODICECSHAREDCOMPOSITE_H

#include <rtm/idl/RTCSkel.h>
#include <rtm/idl/OpenRTMSkel.h>
#include <rtm/RTObject.h>
#include <rtm/PeriodicExecutionContext.h>
#include <rtm/SdoOrganization.h>
#include <coil/stringutil.h>

#include <string>
#include <utility>
#include <vector>

/*!
 * @if jp
 * @namespace SDOPackage
 *
 * @brief SDO パッケージ
 *
 * @else
 *
 * @namespace SDOPackage
 *
 * @brief SDO Package
 *
 * @endif
 */

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace SDOPackage
{
  /*!
   * @if jp
   * @class PeriodicECOrganization
   * @brief PeriodicECOrganization クラス
   *
   * Organization_imp の実装
   *
   * @else
   * @class PeriodicECOrganization
   * @brief PeriodicECOrganization class
   *
   * Implement of Organization_imp
   *
   * @endif
   */
  class PeriodicECOrganization
    : public Organization_impl
  {
    typedef std::vector<std::string> PortList;

  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param rtobj オブジェクト
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @param rtobj Object
     *
     * @endif
     */
    explicit PeriodicECOrganization(::RTC::RTObject_impl* rtobj);
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
    virtual ~PeriodicECOrganization(void);

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organizationメンバーを追加する
     *
     * Organization が保持するメンバーリストに与えられたSDOListを追加する。
     *
     * @param sdo_list 追加される SDO メンバーのリスト
     * @return 追加が成功したかどうかがboolで返される
     *
     * @else
     *
     * @brief [CORBA interface] Add Organization member
     *
     * This operation adds the given SDOList to the existing organization's
     * member list
     *
     * @param sdo_list SDO member list to be added
     * @return boolean will returned if the operation succeed
     *
     * @endif
     */
    virtual ::CORBA::Boolean add_members(const SDOList& sdo_list)
      throw (::CORBA::SystemException,
             InvalidParameter, NotAvailable, InternalError);

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organizationメンバーをセットする
     *
     * Organization が保持するメンバーリストを削除し、与えられた
     * SDOListを新規にセットする。
     *
     * @param sdo_list 新規にセットされる SDO メンバーのリスト
     * @return 追加が成功したかどうかがboolで返される
     *
     * @else
     *
     * @brief [CORBA interface] Set Organization member
     *
     * This operation removes existing member list and sets the given
     * SDOList to the existing organization's member list
     *
     * @param sdo_list SDO member list to be set
     * @return boolean will returned if the operation succeed
     *
     * @endif
     */
    virtual ::CORBA::Boolean set_members(const SDOList& sdos)
      throw (::CORBA::SystemException,
             InvalidParameter, NotAvailable, InternalError);

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organizationメンバーを削除する
     *
     * Organization が保持するメンバーリスト内の特定のSDOを削除する。
     *
     * @param id 削除される SDO の ID
     * @return 追加が成功したかどうかがboolで返される
     *
     * @else
     *
     * @brief [CORBA interface] Remove a member of Organization
     *
     * This operation removes a SDO from existing member list by specified ID.
     *
     * @param id The ID of the SDO to be removed
     * @return boolean will returned if the operation succeed
     *
     * @endif
     */
    virtual ::CORBA::Boolean remove_member(const char* id)
      throw (::CORBA::SystemException,
             InvalidParameter, NotAvailable, InternalError);

    /*!
     * @if jp
     * @brief Organizationメンバーを削除する
     * @else
     * @brief Remove a member of Organization
     * @endif
     */
    void removeAllMembers(void);
    /*!
     * @if jp
     * @brief Organizationメンバーを更新/削除する
     * @else
     * @brief Update/Remove a member of Organization
     * @endif
     */
    void updateDelegatedPorts(void);

  protected:
    class Member;
    /*!
     * @if jp
     * @brief SDOからDFCへの変換
     * @else
     * @brief Conversion from SDO to DFC
     * @endif
     */
    bool sdoToDFC(const SDO_ptr sdo, ::OpenRTM::DataFlowComponent_ptr& dfc);

    /*!
     * @if jp
     * @brief Owned ExecutionContext を停止させる
     * @else
     * @brief Stop Owned ExecutionContexts
     * @endif
     */
    void stopOwnedEC(Member& member);

    /*!
     * @if jp
     * @brief Owned ExecutionContext を起動する
     * @else
     * @brief Start Owned ExecutionContexts
     * @endif
     */
    void startOwnedEC(Member& member);

    /*!
     * @if jp
     * @brief DFC に Organization オブジェクトを与える
     * @else
     * @brief Set Organization object to target DFC
     * @endif
     */
    void addOrganizationToTarget(Member& member);

    /*!
     * @if jp
     * @brief Organization オブジェクトを DFCから削除する
     * @else
     * @brief Remove Organization object from a target DFC
     * @endif
     */
    void removeOrganizationFromTarget(Member& member);
    void addRTCToEC(const RTC::RTObject_var& rtobj);

    /*!
     * @if jp
     * @brief Composite の ExecutionContext を DFC にセットする
     * @else
     * @brief Set CompositeRTC's ExecutionContext to the given DFC
     * @endif
     */
    void addParticipantToEC(Member& member);

    /*!
     * @if jp
     * @brief Composite の ExecutionContext から DFC を削除する
     * @else
     * @brief Remove participant DFC from CompositeRTC's ExecutionContext
     * @endif
     */
    void removeParticipantFromEC(Member& member);

    /*!
     * @if jp
     * @brief ポートを委譲する
     * @else
     * @brief Delegate given RTC's ports to the Composite
     * @endif
     */
    void addPort(Member& member, PortList& portlist);

    /*!
     * @if jp
     * @brief 委譲していたポートを削除する
     * @else
     * @brief Remove delegated participatns's ports from the composite
     * @endif
     */
    void removePort(Member& member, PortList& portlist);

    /*!
     * @if jp
     * @brief PortsListを更新する
     * @else
     * @brief PortsList is updated.
     * @endif
     */
    void updateExportedPortsList(void);

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
     * @brief RT オブジェクト
     * @else
     * @brief RT Object
     * @endif
     */
    ::RTC::RTObject_impl* m_rtobj;

    /*!
     * @if jp
     * @brief ExecutionContext オブジェクトリファレンス
     * @else
     * @brief ExecutionContext Object reference
     * @endif
     */
    ::RTC::ExecutionContext_var m_ec;

    class Member
    {
    public:
      explicit Member(RTC::RTObject_ptr rtobj)
      //        : rtobj_(rtobj),
      //          profile_(rtobj->get_component_profile()),
      //          eclist_(rtobj->get_owned_contexts()),
      //          config_(rtobj->get_configuration())
      {
        rtobj_   = RTC::RTObject::_duplicate(rtobj);
        profile_ = rtobj->get_component_profile();
        eclist_  = rtobj->get_owned_contexts();
        config_  = rtobj->get_configuration();
      }

      virtual ~Member(void)
      {
        /*
        rtobj_.out();
        profile_.out();
        eclist_.out();
        config_.out();
        */
      }

      Member(const Member& x)
      //        : rtobj_(x.rtobj_),
      //          profile_(x.profile_),
      //          eclist_(x.eclist_),
      //          config_(x.config_)
      {
        rtobj_   = x.rtobj_;
        profile_ = x.profile_;
        eclist_  = x.eclist_;
        config_  = x.config_;
      }

      Member& operator=(const Member& x)
      {
//        std::cout << "####################op=" << std::endl;
        Member tmp(x);
        tmp.swap(*this);
        return *this;
      }
//
      void swap(Member& x)
      {
        RTC::RTObject_var rtobj(x.rtobj_);
        RTC::ComponentProfile_var profile(x.profile_);
        RTC::ExecutionContextList_var eclist(x.eclist_);
        SDOPackage::Configuration_var config(x.config_);

        x.rtobj_ = this->rtobj_;
        x.profile_ = this->profile_;
        x.eclist_ = this->eclist_;
        x.config_ = this->config_;

        this->rtobj_ = rtobj;
        this->profile_ = profile;
        this->eclist_ = eclist;
        this->config_ = config;
      }

      RTC::RTObject_var rtobj_;
      RTC::ComponentProfile_var profile_;
      RTC::ExecutionContextList_var eclist_;
      SDOPackage::Configuration_var config_;
    };

    /*!
     * @if jp
     * @brief RTCメンバーリスト
     * @else
     * @brief Member list
     * @endif
     */
    std::vector<Member> m_rtcMembers;
    typedef std::vector<Member>::iterator MemIt;

    /*!
     * @if jp
     * @brief Port List
     * @else
     * @brief Port List
     * @endif
     */
    PortList m_expPorts;

    /*!
     * @if jp
     * @brief PortListを標準出力する。
     * @else
     * @brief Output PortList to StandardOutput.
     * @endif
     */
    void print(PortList p)
    {
      for (int i(0), len(p.size()); i < len; ++i)
        {
          std::cout << p[i] << std::endl;
        }
    }
  };
};  // namespace SDOPackage


/*!
 * @if jp
 * @namespace RTC
 *
 * @brief RTコンポーネント
 *
 * @else
 *
 * @namespace RTC
 *
 * @brief RT-Component
 *
 * @endif
 */
namespace RTC
{
  class Manager;

  /*!
   * @if jp
   * @class PeriodicECSharedComposite
   * @brief PeriodicECSharedComposite クラス
   *
   * データフロー型RTComponentの基底クラス。
   * 各種データフロー型RTComponentを実装する場合は、本クラスを継承する形で実装
   * する。
   *
   * @since 0.4.0
   *
   * @else
   * @class PeriodicECSharedComposite
   * @brief PeriodicECSharedComposite class
   *
   * This is a base class of the data flow type RT-Component.
   * Inherit this class when implementing various data flow type RT-Components.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class PeriodicECSharedComposite
    : public RTObject_impl
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
     * @brief Constructor
     *
     * Constructor
     *
     * @param manager Manager object
     *
     * @endif
     */
    explicit PeriodicECSharedComposite(Manager* manager);

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
    virtual ~PeriodicECSharedComposite(void);

    /*!
     * @if jp
     * @brief 初期化
     *
     * データフロー型 RTComponent の初期化を実行する。
     * 実際の初期化処理は、各具象クラス内に記述する。
     *
     * @else
     * @brief Initialization
     *
     * Initialization the data flow type RT-Component.
     * Write the actual initialization code in each concrete class.
     *
     * @endif
     */
    virtual ReturnCode_t onInitialize(void);
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
    virtual ReturnCode_t onActivated(RTC::UniqueId exec_handle);
    void activateChildComp(const RTC::RTObject_var& rtobj);
    /*!
     * @if jp
     *
     * @brief 非活性化処理用コールバック関数
     *
     * ComponentAction::on_deactivated が呼ばれた際に実行されるコールバック
     * 関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際の非活性化処理は、本関数をオーバーライドして実装する
     * 必要がある。
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
    virtual ReturnCode_t onDeactivated(RTC::UniqueId exec_handle);

    /*!
     * @if jp
     *
     * @brief リセット処理用コールバック関数
     *
     * ComponentAction::on_reset が呼ばれた際に実行されるコールバック関数。<BR>
     * 本関数は無条件に RTC::RTC_OK を返すようにダミー実装されているので、
     * 各コンポーネントの実際のリセット処理は、本関数をオーバーライドして実装する
     * 必要がある。
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
    virtual ReturnCode_t onReset(RTC::UniqueId exec_handle);
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
    virtual ReturnCode_t onFinalize(void);

    virtual ReturnCode_t exit()
      throw (CORBA::SystemException);

  protected:
    /*!
     * @if jp
     * @brief コンポーネント
     * @else
     * @brief Components
     * @endif
     */
    std::vector<std::string> m_members;

    /*!
     * @if jp
     * @brief オブジェクトのリファレンス
     * @else
     * @brief Reference of object
     * @endif
     */
    OpenRTM::DataFlowComponent_var m_ref;
//    PeriodicExecutionContext* m_pec;
//    ExecutionContextService_var m_ecref;
    /*!
     * @if jp
     * @brief Organizationのリファレンス
     * @else
     * @brief Reference of Organization
     * @endif
     */
    SDOPackage::PeriodicECOrganization* m_org;
  };  // class PeriodicECOrganization
};  // namespace RTC

#ifdef WIN32
#pragma warning( default : 4290 )
#endif


extern "C"
{
  DLL_EXPORT void PeriodicECSharedCompositeInit(RTC::Manager* manager);
};

#endif  // RTC_PERIODICECSHAREDCOMPOSITE_H
