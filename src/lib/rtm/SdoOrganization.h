// -*- C++ -*-
/*!
 * @file SdoOrganization.h
 * @brief SDO Organization implementation class
 * @date $Date: 2008-01-14 07:49:28 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
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

#ifndef RTC_SDOORGANIZATION_H
#define RTC_SDOORGANIZATION_H

#include <rtm/RTC.h>
#include <rtm/idl/SDOPackageSkel.h>
#include <rtm/SystemLogger.h>

#include <mutex>

#include <string>

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

namespace SDOPackage
{
  /*!
   * @if jp
   *
   * @class Organization_impl
   * @brief SDO Organization 実装クラス
   *
   * Organization interface は Resource Data Model で定義されたデータの
   * 追加、削除等の操作を行うためのインターフェースである。
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class Organization_impl
   * @brief Organization implementation class
   *
   * Organization interface is an interface to add and delete etc data
   * defined by Resource Data Model.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class Organization_impl
    : public virtual POA_SDOPackage::Organization,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
#ifdef ORB_IS_RTORB
    explicit Organization_impl(RTC::RTObject_ptr sdo);
#endif  // ORB_IS_RTROB
    explicit Organization_impl(SDOSystemElement_ptr sdo);

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * 仮想デストラクタ。
     *
     * @else
     *
     * @brief Virtual destructor
     *
     * Virtual Virtual destructor
     *
     * @endif
     */
    ~Organization_impl() override;

    //============================================================
    //
    // <<< CORBA interfaces >>>
    //
    //============================================================
    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization ID を取得する
     *
     * Organization の ID を返すオペレーション。
     *
     * @return Resource Data Model で定義された Organization ID。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get Organization ID
     *
     * This operation returns the 'ID' of the Organization.
     *
     * @return The id of the Organization defined in the resource data model.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    char* get_organization_id() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] OrganizationProperty のセット
     *
     * ※ SDO Specification の PIM 記述とオペレーション名が異なる。
     * ※ addOrganizationProperty に対応か？<BR>
     * OrganizationProperty を Organization に追加するオペレーション。
     * OrganizationProperty は Organization のプロパティ記述である。
     *
     * @param org_property セットする OrganizationProperty
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter "org_property" が null。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Set OrganizationProperty
     *
     * Note: The PIM description of SDO Specification differs from the operation
     *       name.
     * Note: Does this operation correspond to addOrganizationProperty?
     * This operation adds the OrganizationProperty to an Organization. The
     * OrganizationProperty is the property description of an Organization.
     *
     * @param org_property The type of organization to be added.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The argument "organizationProperty" is null.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean
    add_organization_property(const OrganizationProperty& organization_property) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] OrganizationProperty の取得
     *
     * Organization が所有する OrganizationProperty を返すオペレーション。
     * Organization がプロパティを持たなければ空のリストを返す。
     *
     * @return Organization のプロパティのリスト。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get OrganizationProperty
     *
     * This operation returns the OrganizationProperty that an Organization
     * has. An empty OrganizationProperty is returned if the Organization does
     * not have any properties.
     *
     * @return The list with properties of the organization.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    OrganizationProperty* get_organization_property() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] OrganizationProperty の特定の値の取得
     *
     * OrganizationProperty の指定された値を返すオペレーション。
     * 引数 "name" で指定されたプロパティの値を返す。
     *
     * @param name 値を返すプロパティの名前。
     *
     * @return 引数 "name" で指定されたプロパティの値。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "name" で指定されたプロパティが
     *            存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get specified value of OrganizationProperty
     *
     * This operation returns a value in the OrganizationProperty.
     * The value to be returned is specified by argument "name."
     *
     * @param name The name of the value to be returned.
     *
     * @return The value of property which is specified by argument "name".
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter There are no Property stored with argument
     *                             "name".
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Any* get_organization_property_value(const char* name) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] OrganizationProperty の値のセット
     *
     * OrganizationProperty の NVList に name と value のセットを追加もしくは
     * 更新するオペレーション。name と value は引数 "name" と "value" により
     * 指定する。
     *
     * @param name 追加・更新されるプロパティの名前。
     * @param value 追加・更新されるプロパティの値。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "name" で指定されたプロパティは
     *            存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Set specified value of OrganizationProperty
     *
     * This operation adds or updates a pair of name and value as a property
     * of Organization to/in NVList of the OrganizationProperty. The name and
     * the value to be added/updated are specified by argument "name" and
     * "value."
     *
     * @param name The name of the property to be added/updated.
     * @param value The value of the property to be added/updated.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The property that is specified by argument
     *                             "name" does not exist.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean
    set_organization_property_value(const char* name, const CORBA::Any& value) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] OrganizationProperty の削除
     *
     * OrganizationProperty の NVList から特定のプロパティを削除する。
     * 削除されるプロパティの名前は引数 "name" により指定される。
     *
     * @param name 削除するプロパティの名前。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "name" で指定されたプロパティは
     *            存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Remove specified OrganizationProperty
     *
     * This operation removes a property of Organization from NVList of the
     * OrganizationProperty. The property to be removed is specified by
     * argument "name."
     *
     * @param name The name of the property to be removed.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The property that is specified by argument
     *                             "name" does not exist.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean remove_organization_property(const char* name) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] SDO メンバーの追加
     *
     * Organization にメンバーとして SDO を追加する。
     * 引数 "sdo" に追加するメンバー SDO を指定する。
     *
     * @param sdo_list Organization に追加される SDO のリスト。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "sdo" が nullである。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Add the member SDOs
     *
     * This operation adds a member that is an SDO to the organization.
     * The member to be added is specified by argument "sdo."
     *
     * @param sdo_list The member to be added to the organization.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "sdo" is null.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean add_members(const SDOList& sdo_list) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization のメンバーを取得する
     *
     * Organization のメンバーの SDO のリストを返す。
     * メンバーが存在しなければ空のリストを返す。
     *
     * @return Organization に含まれるメンバー SDO のリスト。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get the member list of the Organization
     *
     * This operation returns a list of SDOs that are members of an
     * Organization. An empty list is returned if the Organization does not
     * have any members.
     *
     * @return Member SDOs that are contained in the Organization object.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    SDOList* get_members() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] SDO の セット
     *
     * SDO のリストを Organization のメンバーとしてセットする。
     * Organization がすでにメンバーの SDO を管理している場合は、
     * 与えられた SDO のリストに置き換える。
     *
     * @param sdos メンバーの SDO。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "SDOList" が nullである、もしくは
     *            引数に指定された "SDOList" が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Set SDO
     *
     * This operation assigns a list of SDOs to an Organization as its members.
     * If the Organization has already maintained a member SDO(s) when it is
     * called, the operation replaces the member(s) with specified list of
     * SDOs.
     *
     * @param sdos Member SDOs to be assigned.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "SDOList" is null, or the object
     *                             that is specified by the argument "sdos"
     *                             does not exist.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean set_members(const SDOList& sdos) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] SDO メンバーの削除
     *
     * Organization から引数で指定された "id" の SDO を削除する。
     *
     * @param id 削除する SDO の id。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "id" が null もしくは存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Remove member SDO from Organization
     *
     * This operation removes a member from the organization. The member to be
     * removed is specified by argument "id."
     *
     * @param id Id of the SDO to be removed from the organization.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "id" is null or does not exist.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean remove_member(const char* id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization のオーナーを取得する
     *
     * この Organization のオーナーへの参照を返す。
     *
     * @return オーナーオブジェクトへの参照。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get the owner of Organization
     *
     * This operation returns the SDOSystemElement that is owner of
     * the Organization.
     *
     * @return Reference of owner object.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    SDOSystemElement_ptr get_owner() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization にオーナーをセットする
     *
     * Organization に対して SDOSystemElement をオーナーとしてセットする。
     * 引数 "sdo" にセットする SDOSystemElement を指定する。
     *
     * @param sdo オーナーオブジェクトの参照。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "sdo" が nullである、もしくは、
     *                             "sdo" が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Set the owner to the Organization
     *
     * This operation sets an SDOSystemElement to the owner of the
     * Organization. The SDOSystemElement to be set is specified by argument
     * "sdo."
     *
     * @param sdo Reference of owner object.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "sdo" is null, or the object
     *                             that is specified by "sdo" in argument "sdo"
     *                             does not exist.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean set_owner(SDOSystemElement_ptr sdo) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization の DependencyType を取得
     *
     * Organization の関係を表す "DependencyType" を返す。
     *
     * @return Organization の依存関係 DependencyType を返す。
     *         DependencyType は OMG SDO 仕様の Section 2.2.2 2-3 ページの
     *         "Data Structures Used by Resource Data Model" を参照。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get the DependencyType of the Organization
     *
     * This operation gets the relationship "DependencyType" of the
     * Organization.
     *
     * @return The relationship of the Organization as DependencyType.
     *         DependencyType is defined in Section 2.2.2, "Data Structures
     *         Used by Resource Data Model," on page 2-3
     *         of OMG SDO Specification.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    DependencyType get_dependency() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization の DependencyType をセットする
     *
     * Organization の依存関係 "DependencyType" をセットする。
     * 引数 "dependency" により依存関係を与える。
     *
     * @param dependency Organization の依存関係を表す DependencyType。
     *        DependencyType は OMG SDO 仕様の Section 2.2.2、2-3 ページの
     *        "Data Structures Used by Resource Data Model" を参照。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "sProfile" が nullである。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Set the DependencyType of the Organization
     *
     * This operation sets the relationship "DependencyType" of the
     * Organization. The value to be set is specified by argument "dependency."
     *
     * @param dependency The relationship of the Organization as
     *                   DependencyType. DependencyType is defined in Section
     *                   2.2.2, "Data Structures Used by Resource Data Model,"
     *                   on page 2-3.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "dependency" is null.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean set_dependency(DependencyType dependency) override;

    // end of CORBA interface definition
    //============================================================
    Organization_ptr getObjRef();

  protected:
    ::RTC::Logger rtclog;
    Organization_var m_objref;
    /*!
     * @if jp
     * @brief Organization の識別子
     * @else
     * @brief The identifier of the Organization.
     * @endif
     */
    std::string m_pId;

    /*!
     * @if jp
     * @brief Organization に関連付けられた SDO メンバのリスト
     * @else
     * @brief A list of SDO members associated with the Organization
     * @endif
     */
    SDOPackage::SDOList m_memberList;

    /*!
     * @if jp
     * @brief Organization の owner
     * @else
     * @brief The owner of the Organization
     * @endif
     */
    SDOPackage::SDOSystemElement_var m_varOwner;

    /*!
     * @if jp
     *
     * @brief 依存関係のタイプ
     *
     * Owner と member の依存関係を指定する属性。
     * Organization は以下のトポロジパターンを表現することができる。
     *
     * -# owner が member を管理する階層的構造。この場合 DependencyType は OWN
     *    という値を持つ。
     * -# members が owner を管理する逆向きの階層的構造。この場合は
     *    DependencyType は OWNER という値を持つ。
     * -# owner と member に依存関係がないフラットな構造。この場合は
     *    DependencyType は NO_DEPENDENCY という値を持つ。
     *
     * SDO および SDOSystemElement のサブクラスは Organization の owner として
     * 振舞うことが出来る。SDO が owner の場合にはOrganization は上記の
     * いずれかのトポロジーパターンをとる。
     *
     * - Organization が 1. のトポロジーパターンを持つ場合、唯一つの owner SDO
     *   は member SDO を制御する。たとえば、エアコン(owner)は、
     *   温度センサ(member)、湿度センサ(member)、風量制御器(member)を制御する。
     * - Organization が 2. のトポロジを持つ場合は、複数の SDO member が唯一の
     *   SDO owner を共有する。たとえば、アンプ(owner)はいくつかのAV
     *   コンポーネント(member)から共有される。
     * - SDO ではない SDOSystemElement のサブクラスが owner の場合、
     *   以下のようなトポロジー例が考えられる。
     * -- User(owner)-SDO(member): ユーザ(owner) は一つ以上の SDO(member)を
     *   管理する。これは上記トポロジパタン1.にあたる。
     * -- Location(owner)-SDO(members): 一つ以上の SDO(member) が特定の場所
     *   = location(owner) で動作している場合、Organization のトポロジパターン
     *   は 3. の場合になる。たとえば、複数の PDA がある部屋にあり、互いに同等
     *   な関係であり相互に通信可能な場合はこれにあたる。
     *
     * @else
     *
     * @brief Dependency type
     *
     * This attribute specifies the dependency relation between the owner and
     * members of the organization.
     * Organization is used to form the following three patterns of topology.
     *
     * -# Hierarchical organization, which indicates owner supervises members.
     *    In this case, DependencyType should hold OWN value (see description
     *    of DependencyType on previous pages).
     * -# Reversely hierarchical organization, which indicates members
     *    supervise owner. In this case, DependencyType should hold OWNED value
     *    (see description of DependencyType on previous pages).
     * -# Flat organization, which indicates no dependency exists. In this
     *    case, DependencyType should hold NO_DEPENDENCY value (see description
     *    of DependencyType on previous pages).
     *
     * Both an SDO and another subclass of SDOSystemElement can act as owner
     * of an Organization. When an SDO is an owner, Organization can represent
     * any of the above three topology patterns.
     *
     * - When an Organization represents topology pattern (1), an SDO (owner)
     *   controls one or more SDOs (members). For example, air conditioner
     *   (owner) controls a temperature sensor (member), humidity sensor
     *   (member), and wind flow controller (member).
     * - When an Organization represents topology pattern (2), multiple
     *   SDOs(members) share an SDO (owner). For example, an amplifier (owner)
     *   is shared by several AV components (members) in an AV stereo.
     * - When a subclass of SDOSystemElement, which is not an SDO is an owner
     *   examples of the topology are as follows.
     * -- User (owner)-SDO (members): When a user (owner) supervises one or
     *    more SDOs (members), the organization represents topology pattern 1.
     * -- Location (owner)-SDO (members): When one or more SDOs (members) are
     *    operating in a specific location (owner), the organization represents
     *    topology pattern 3. For example, multiple PDAs in the same place
     *    (e.g., a room) have equal relationships among them to communicate
     *    with each other.
     * @endif
     */
    SDOPackage::DependencyType m_dependency;

    /*!
     * @if jp
     *
     * @brief Organization プロパティ
     *
     * OrganizationProperty は Organization のプロパティ情報を保持する。
     * 一つの Organization は0個もしくは1個の OrganizationProperty をもつ。
     *
     * @else
     *
     * @brief Organization property
     *
     * OrganizationProperty contains the properties of an Organization.
     * An Organization has zero or one (at most one) instance
     * of OrganizationProperty.
     *
     * @endif
     */
    SDOPackage::OrganizationProperty m_orgProperty;
    std::mutex m_org_mutex;

    /*!
     * @if jp
     * @brief  NameValue用functor
     * @else
     * @brief  Functor for NameValue
     * @endif
     */
    struct nv_name
    {
      explicit nv_name(const char* name) : m_name(name) {}
      bool operator()(const NameValue& nv)
      {
        return m_name == std::string(nv.name);
      }
      std::string m_name;
    };  // struct nv_name

    /*!
     * @if jp
     * @brief  SDO用functor
     * @else
     * @brief  Functor for SDO
     * @endif
     */
    struct sdo_id
    {
      explicit sdo_id(const char* id) : m_id(id) {}
#ifdef ORB_IS_ORBEXPRESS
      bool operator()(const SDO_var sdo)
#else
      bool operator()(const SDO_ptr sdo)
#endif
      {
        CORBA::String_var id(sdo->get_sdo_id());

        return m_id == (const char*)id;
      }
      std::string m_id;
    };  // struct sdo_id
  };  // class Organization_impl
} // namespace SDOPackage

#endif  // RTC_NAMESPACE SDOPACKAGE
