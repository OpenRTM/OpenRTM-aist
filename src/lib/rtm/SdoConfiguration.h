// -*- C++ -*-
/*!
 * @file SdoConfiguration.h
 * @brief RT component base class
 * @date $Date: 2008-01-14 07:49:34 $
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

#ifndef RTC_SDOCONFIGURATION_H
#define RTC_SDOCONFIGURATION_H

// CORBA header include
#include <rtm/RTC.h>
#include <mutex>

// local includes
#include <rtm/idl/SDOPackageSkel.h>
#include <rtm/ConfigAdmin.h>
#include <rtm/SystemLogger.h>
#include <rtm/SdoServiceAdmin.h>

#include <string>

// SdoConfiguration with SeqEx 159120
// SdoConfiguration with SeqUtil 114504 114224

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
   * @class Configuration_impl
   * @brief SDO Configuration 実装クラス
   *
   * Configuration interface は Resource Data Model で定義されたデータの
   * 追加、削除等の操作を行うためのインターフェースである。
   * DeviceProfile, ServiceProfile, ConfigurationProfile および Organization
   * の変更を行うためのオペレーションを備えている。SDO の仕様ではアクセス制御
   * およびセキュリティに関する詳細については規定していない。
   *
   * 複数の設定 (Configuration) を保持することにより、容易かつ素早くある設定
   * を反映させることができる。事前に定義された複数の設定を ConfigurationSets
   * および configuration profile として保持することができる。ひとつの
   * ConfigurationSet は特定の設定に関連付けられた全プロパティ値のリストを、
   * ユニークID、詳細とともに持っている。これにより、各設定項目の詳細を記述し
   * 区別することができる。Configuration interface のオペレーションはこれら
   * ConfiguratioinSets の管理を支援する。
   *
   *
   * - ConfigurationSet: id, description, NVList から構成される1セットの設定
   * - ConfigurationSetList: ConfigurationSet のリスト
   * - Parameter: name, type, allowed_values から構成されるパラメータ定義。
   * - ActiveConfigurationSet: 現在有効なコンフィギュレーションの1セット。
   *
   * 以下、SDO仕様に明記されていないもしくは解釈がわからないため独自解釈
   *
   * 以下の関数は ParameterList に対して処理を行う。
   * - get_configuration_parameters()
   *
   * 以下の関数はアクティブなConfigurationSetに対する処理を行う
   * - get_configuration_parameter_values()
   * - get_configuration_parameter_value()
   * - set_configuration_parameter()
   *
   * 以下の関数はConfigurationSetListに対して処理を行う
   * - get_configuration_sets()
   * - get_configuration_set()
   * - set_configuration_set_values()
   * - get_active_configuration_set()
   * - add_configuration_set()
   * - remove_configuration_set()
   * - activate_configuration_set()
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class Configuration_impl
   * @brief Configuration implementation class
   *
   * Configuration interface provides operations to add or remove data
   * specified in resource data model. These operations provide functions to
   * change DeviceProfile, ServiceProfile, ConfigurationProfile, and
   * Organization. This SDO specification does not address access control or
   * security aspects.
   *
   * Different configurations can be stored for simple and quick activation.
   * Different predefined configurations are stored as different
   * ConfigurationSets or configuration profile. A ConfigurationSet stores the
   * value of all properties assigned for the particular configuration along
   * with its unique id and description to identify and describe the
   * configuration respectively. Operations in the configuration interface
   * help manage these ConfigurationSets.
   *
   *
   * - ConfigurationSet: id, description, one configuration set to consist
   *                     of NVList
   * - ConfigurationSetList: List of ConfigurationSet
   * - Parameter: Parameter definition consist of name, type and allowed_values
   * - ActiveConfigurationSet: One set of configuration set that is valid.
   *
   * The following functions do processing to ParameterList.
   * - get_configuration_parameters()
   *
   * The following functions do processing to active ConfigurationSet
   * - get_configuration_parameter_values()
   * - get_configuration_parameter_value()
   * - set_configuration_parameter()
   *
   * The following functions do processing to ConfigurationSetList
   * - get_configuration_sets()
   * - get_configuration_set()
   * - set_configuration_set_values()
   * - get_active_configuration_set()
   * - add_configuration_set()
   * - remove_configuration_set()
   * - activate_configuration_set()
   *
   * @since 0.4.0
   *
   * @endif
   */
  class Configuration_impl
    : public virtual POA_SDOPackage::Configuration,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     * @brief 既定の POA (Manager の POA) を返す
     *
     * _this() で暗黙に活性化するときに使う。corba.bidirectional_giop が YES
     * なら、Manager の POA は BiDirectional ポリシーを持つ (Bi-Directional
     * GIOP で呼び返されるオブジェクトは、その POA にある必要がある)。
     * @else
     * @brief Returns the default POA (the POA of the Manager)
     *
     * Used for implicit activation by _this(). If corba.bidirectional_giop is
     * YES, the POA of the Manager has the BiDirectional policy (objects
     * called back through Bi-Directional GIOP must be in such a POA).
     * @endif
     */
    PortableServer::POA_ptr _default_POA() override;

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param configAdmin ConfigurationSetList
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param configAdmin ConfigurationSetList
     *
     * @endif
     */
    Configuration_impl(RTC::ConfigAdmin& configsets,
                       RTC::SdoServiceAdmin& sdoServiceAdmin);

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * 仮想デストラクタ。
     *
     * @else
     *
     * @brief Virtual destractor
     *
     * Virtual destractor.

     * @endif
     */
    ~Configuration_impl() override;

    //============================================================
    //
    // <<< CORBA interfaces >>>
    //
    //============================================================
    /*!
     * @if jp
     *
     * @brief [CORBA interface] SDO の DeviceProfile のセット
     *
     * このオペレーションは SDO の DeviceProfile をセットする。SDO が
     * DeviceProfile を保持していない場合は新たな DeviceProfile を生成し、
     * DeviceProfile をすでに保持している場合は既存のものと置き換える。
     *
     * @param dProfile SDO に関連付けられる DeviceProfile。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InvalidParameter 引数 "dProfile" が null である。
     * @exception InternalError 内部的エラーが発生した。
     *
     * @else
     *
     * @brief [CORBA interface] Set DeviceProfile of SDO
     *
     * This operation sets the DeviceProfile of an SDO. If the SDO does not
     * have DeviceProfile, the operation will create a new DeviceProfile,
     * otherwise it will replace the existing DeviceProfile.
     *
     * @param dProfile The device profile that is to be assigned to this SDO.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "dProfile" is null.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean set_device_profile(const DeviceProfile& dProfile) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] SDO の ServiceProfile のセット
     *
     * このオペレーションはこの Configuration interface を所有する対象 SDO の
     * ServiceProfile を追加する。もし引数の ServiceProfile の id が空であれば
     * 新しい ID が生成されその ServiceProfile を格納する。もし id が空で
     * なければ、SDO は同じ id を持つ ServiceProfile を検索する。
     * 同じ id が存在しなければこの ServiceProfile を追加し、id が存在すれば
     * 上書きをする。<br>
     * (注意：最新バージョンではオペレーション名がadd_service_profile変更)
     *
     * @param sProfile 追加する ServiceProfile
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
     * @brief [CORBA interface] Set SDO's ServiceProfile
     *
     * This operation adds ServiceProfile to the target SDO that navigates this
     * Configuration interface. If the id in argument ServiceProfile is null,
     * new id is created and the ServiceProfile is stored. If the id is not
     * null, the target SDO searches for ServiceProfile in it with the same id.
     * It adds the ServiceProfile if not exist, or overwrites if exist.
     *
     * @param sProfile ServiceProfile to be added.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument "sProfile" is null.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean add_service_profile(const ServiceProfile& sProfile) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization の追加
     *
     * このオペレーションは Organization object のリファレンスを追加する。
     *
     * @param org 追加する Organization
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InvalidParameter 引数 "organization" が null である。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Add Organization
     *
     * This operation adds reference of an Organization object.
     *
     * @param org Organization to be added.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InvalidParameter The argument “organization” is null.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean add_organization(Organization_ptr org) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ServiceProfile の削除
     *
     * このオペレーションはこの Configuration interface を持つ SDO の
     * Service の ServiceProfile を削除する。削除する ServiceProfile
     * は引数により指定される。
     *
     * @param id 削除する ServiceProfile の serviceID。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "id" が null である。もしくは "id" に
     *            関連付けられた ServiceProfile が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Remove ServiceProfile
     *
     * This operation removes ServiceProfile object to the SDO that has this
     * Configuration interface. The ServiceProfile object to be removed is
     * specified by argument.
     *
     * @param id serviceID of a ServiceProfile to be removed.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The argument "sProfile" is null, or if the
     *                             object that is specified by argument
     *                             "sProfile" does not exist.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean remove_service_profile(const char* id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Organization の参照の削除
     *
     * このオペレーションは Organization の参照を削除する。
     *
     * @param organization_id 削除する Organization の一意な id。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "organization_id" が null である。
     *                             もしくは "organization_id" に関連付けられた
     *                             OrganizationProfile が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Remove the reference of Organization
     *
     * This operation removes the reference of an Organization object.
     *
     * @param organization_id Unique id of the organization to be removed.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The argument "organizationID" is null,
     *                             or the object which is specified by argument
     *                             "organizationID" does not exist.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean remove_organization(const char* organization_id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] 設定パラメータのリストの取得
     *
     * このオペレーションは configuration parameter のリストを返す。
     * SDO が設定可能なパラメータを持たなければ空のリストを返す。
     *
     * @return 設定を特徴付けるパラメータ定義のリスト。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get a list of configuration parameters
     *
     * This operation returns a list of Parameters. An empty list is returned
     * if the SDO does not have any configurable parameter.
     *
     * @return The list with definitions of parameters characterizing the
     *          configuration.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    ParameterList* get_configuration_parameters() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Configuration parameter の値のリストの取得
     *
     * このオペレーションは全ての configuration パラメータおよび値を返す。
     *
     * @return 全ての configuration パラメータと値のリスト。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get a list of the value of configuration
     *                          parameters
     *
     * This operation returns all configuration parameters and their values.
     *
     * @return List of all configuration parameters and their values.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    NVList* get_configuration_parameter_values() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Configuration parameter の値の取得
     *
     * このオペレーションは引数 "name" で指定されたパラメータ値を返す。
     *
     * @param name 値を要求するパラメータの名前。
     *
     * @return 指定されたパラメータの値。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "name" が null である。
     *            もしくは "name" に関連付けられたパラメータが存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get the value of configuration parameter
     *
     * This operation returns a value of parameter that is specified by
     * argument "name."
     *
     * @param name Name of the parameter whose value is requested.
     *
     * @return The value of the specified parameter.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The value of the argument "name" is
     *                             empty String, or null, or the parameter
     *                             that is specified by argument "name"
     *                             does not exist.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Any* get_configuration_parameter_value(const char* name) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] Configuration パラメータの変更
     *
     * このオペレーションは "name" で指定したパラメータの値を "value" に
     * 変更する。
     *
     * @param name 変更対象パラメータの名前。
     * @param value 変更対象パラメータの新しい値。
     *
     * @return オペレーションが成功したかどうかを返す。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数( "name"もしくは"value") が null である。
     *            もしくは "name" に関連付けられたパラメータが存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Modify the configuration parameter value
     *
     * This operation sets a parameter to a value that is specified by argument
     * "value." The parameter to be modified is specified by argument " name."
     *
     * @param name The name of parameter to be modified.
     * @param value New value of the specified parameter.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The arguments ("name" and/or "value") is
     *                             null, or the parameter that is specified by
     *                             the argument "name" does not exist.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean set_configuration_parameter(const char* name,
                                                       const CORBA::Any& value) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConfigurationSet リストの取得
     *
     * このオペレーションは ConfigurationProfile が持つ ConfigurationSet の
     * リストを返す。 SDO が ConfigurationSet を持たなければ空のリストを返す。
     *
     * @return 保持している ConfigurationSet のリストの現在値。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get a list of ConfigurationSet
     *
     * This operation returns a list of ConfigurationSets that the
     * ConfigurationProfile has. An empty list is returned if the SDO does not
     * have any ConfigurationSets.
     * This operation returns a list of all ConfigurationSets of the SDO.
     * If no predefined ConfigurationSets exist, then empty list is returned.
     *
     * @return The list of stored configuration with their current values.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    ConfigurationSetList* get_configuration_sets() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConfigurationSet の取得
     *
     * このオペレーションは引数で指定された ConfigurationSet の ID に関連
     * 付けられた ConfigurationSet を返す。
     *
     * @param config_id ConfigurationSet の識別子。
     *
     * @return 引数により指定された ConfigurationSet。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter "config_id" が null か、指定された
     *            ConfigurationSet が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get a ConfigurationSet
     *
     * This operation returns the ConfigurationSet specified by the parameter
     * configurationSetID.
     *
     * @param config_id Identifier of ConfigurationSet requested.
     *
     * @return The configuration set specified by the parameter config_id.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The parameter 'config_id' is null or
     *                             there are no ConfigurationSets stored with
     *                             such id.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    ConfigurationSet* get_configuration_set(const char* id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] アクティブな ConfigurationSet を取得する
     *
     * このオペレーションは当該SDOの現在アクティブな ConfigurationSet を返す。
     * (もしSDOの現在の設定が予め定義された ConfigurationSet により設定されて
     * いるならば。)
     * ConfigurationSet は以下の場合にはアクティブではないものとみなされる。
     *
     * - 現在の設定が予め定義された ConfigurationSet によりセットされていない、
     * - SDO の設定がアクティブになった後に変更された、
     * - SDO を設定する ConfigurationSet が変更された、
     *
     * これらの場合には、空の ConfigurationSet が返される。
     *
     * @return 現在アクティブな ConfigurationSet。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Get active ConfigurationSet
     *
     * This operation returns the current active ConfigurationSet of an
     * SDO (i.e., if the current configuration of the SDO was set using
     * predefined configuration set).
     * ConfigurationSet cannot be considered active if the:
     *
     * - Current configuration of the SDO was not set using any predefined
     *   ConfigurationSet, or
     * - Configuration of the SDO was changed after it has been active, or
     * - ConfigurationSet that was used to configure the SDO was modified.
     *
     * Empty ConfigurationSet is returned in these cases.
     *
     * @return The active ConfigurationSet.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    ConfigurationSet* get_active_configuration_set() override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConfigurationSet を追加する
     *
     * ConfigurationProfile に ConfigurationSet を追加するオペレーション。
     *
     * @param configuration_set 追加する ConfigurationSet。
     *
     * @return オペレーションが成功したかどうか。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter "configurationSet" が null か、
     *            "configurationSet"で定義された属性の１つが不正か、
     *            指定された configurationSet もIDが既に存在する。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Add ConfigurationSet
     *
     * This operation adds a ConfigurationSet to the ConfigurationProfile.
     *
     * @param configuration_set The ConfigurationSet that is added.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The argument "configurationSet" is null,
     *                             or one of the attributes defining
     *                             "configurationSet" is invalid, or the
     *                             specified identifier of the configuration
     *                             set already exists.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean
    add_configuration_set(const ConfigurationSet& configuration_set) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConfigurationSet をセットする
     *
     * このオペレーションは指定された id の ConfigurationSet を更新する。
     *
     * @param config_id 変更する ConfigurationSet の ID。
     * @param configuration_set 変更する ConfigurationSet そのもの。
     *
     * @return ConfigurationSet が正常に更新できた場合は true。
     *         そうでなければ false を返す。
     *
     * @exception InvalidParameter config_id が null か、
     *                             指定された id で格納された ConfigurationSetが
     *                             存在しないか、指定された configuration_set内
     *                             の属性の１つが不正。
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Set ConfigurationSet
     *
     * This operation modifies the specified ConfigurationSet of an SDO.
     *
     * Note: The number of parameters differs between spec and IDL!!
     *
     * @param config_id The ID of ConfigurationSet to be modified.
     * @param configuration_set ConfigurationSet to be replaced.
     *
     * @return A flag indicating if the ConfigurationSet was modified
     *         successfully. "true" - The ConfigurationSet was modified
     *         successfully. "false" - The ConfigurationSet could not be
     *         modified successfully.
     *
     * @exception InvalidParameter The parameter 'configurationSetID' is null
     *                             or there is no ConfigurationSet stored with
     *                             such id.
     *                             This exception is also raised if one of the
     *                             attributes defining ConfigurationSet is not
     *                             valid.
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean
    set_configuration_set_values(const ConfigurationSet& configuration_set) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConfigurationSet を削除する
     *
     * ConfigurationProfile から ConfigurationSet を削除する。
     *
     * @param config_id 削除する ConfigurationSet の id。
     *
     * @return オペレーションが成功したかどうか。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "configurationSetID" が null である、
     *            もしくは、引数で指定された ConfigurationSet が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Remove ConfigurationSet
     *
     * This operation removes a ConfigurationSet from the ConfigurationProfile.
     *
     * @param config_id The id of ConfigurationSet which is removed.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The arguments "configurationSetID" is null,
     *                             or the object specified by the argument
     *                             "configurationSetID" does not exist.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean remove_configuration_set(const char* id) override;

    /*!
     * @if jp
     *
     * @brief [CORBA interface] ConfigurationSet のアクティブ化
     *
     * ConfigurationProfile に格納された ConfigurationSet のうち一つを
     * アクティブにする。
     * このオペレーションは特定の ConfigurationSet をアクティブにする。
     * すなわち、SDO のコンフィギュレーション・プロパティがその格納されている
     * ConfigurationSet により設定されるプロパティの値に変更される。
     * 指定された ConfigurationSet の値がアクティブ・コンフィギュレーション
     * にコピーされるということを意味する。
     *
     * @param config_id アクティブ化する ConfigurationSet の id。
     *
     * @return オペレーションが成功したかどうか。
     *
     * @exception SDONotExists ターゲットのSDOが存在しない。(本例外は、CORBA標準
     *                         システム例外のOBJECT_NOT_EXISTにマッピングされる)
     * @exception InvalidParameter 引数 "config_id" が null である、もしくは
     *            引数で指定された ConfigurationSet が存在しない。
     * @exception NotAvailable SDOは存在するが応答がない。
     * @exception InternalError 内部的エラーが発生した。
     * @else
     *
     * @brief [CORBA interface] Activate ConfigurationSet
     *
     * This operation activates one of the stored ConfigurationSets in the
     * ConfigurationProfile.
     * This operation activates the specified stored ConfigurationSets.
     * This means that the configuration properties of the SDO are changed as
     * the values of these properties specified in the stored ConfigurationSet.
     * In other words, values of the specified ConfigurationSet are now copied
     * to the active configuration.
     *
     * @param config_id Identifier of ConfigurationSet to be activated.
     *
     * @return If the operation was successfully completed.
     *
     * @exception SDONotExists The target SDO does not exist.(This exception
     *                         is mapped to CORBA standard system exception
     *                         OBJECT_NOT_EXIST.)
     * @exception InvalidParameter The argument ("configID") is null or
     *                             there is no configuration set with identifier
     *                             specified by the argument.
     * @exception NotAvailable The target SDO is reachable but cannot respond.
     * @exception InternalError The target SDO cannot execute the operation
     *                          completely due to some internal error.
     * @endif
     */
    CORBA::Boolean activate_configuration_set(const char* id) override;

    // end of CORBA interface definition
    //============================================================

    /*!
     * @if jp
     *
     * @brief オブジェクト　リファレンスを取得する
     *
     * 対象のオブジェクトリファレンスを取得する
     *
     * @return オブジェクトリファレンス
     *
     * @else
     *
     * @brief Get object reference
     *
     * Get the target object reference.
     *
     * @return Object reference
     *
     * @endif
     */
    Configuration_ptr getObjRef();

    /*!
     * @if jp
     *
     * @brief SDO の DeviceProfile を取得する
     *
     * SDO の DeviceProfile を取得する
     *
     * @return SDO の DeviceProfile
     *
     * @else
     *
     * @brief Get the DeviceProfile of SDO
     *
     * Get the DeviceProfile of SDO.
     *
     * @return DeviceProfile of SDO
     *
     * @endif
     */
    DeviceProfile getDeviceProfile();

    /*!
     * @if jp
     *
     * @brief SDO の Organization リストを取得する
     *
     * SDO の Organization リストを取得する
     *
     * @return SDO の Organization リスト
     *
     * @else
     *
     * @brief Get a list of Organization of SDO
     *
     * Get a list of Organization of SDO.
     *
     * @return List of Organization of SDO
     *
     * @endif
     */
    OrganizationList getOrganizations();

  protected:
    ::RTC::Logger rtclog;
    /*!
     * @if jp
     *
     * @brief UUIDを生成する
     *
     * UUIDを生成する
     *
     * @return 生成したUUID
     *
     * @else
     *
     * @brief Generate UUID
     *
     * Generate UUID.
     *
     * @return UUID that has been generated
     *
     * @endif
     */
    static std::string getUUID() ;

    /*!
     * @if jp
     * @brief CORBA オブジェクトへの参照
     * @else
     * @brief The reference to CORBA object
     * @endif
     */
    Configuration_var m_objref;

    /*!
     * @if jp
     * @brief Lock 付き SDO DeviceProfile
     * @else
     * @brief SDO DeviceProfile with mutex lock
     * @endif
     */
    DeviceProfile m_deviceProfile;
    std::mutex m_dprofile_mutex;

    /*!
     * @if jp
     * @brief SDO Parameter
     *
     * 実装技術に非依存な変数(パラメータ)を定義するデータ構造。
     * パラメータ構造は、変数の名前と型を定義する。
     * 定義されている属性は以下のとおり。
     *  - name : パラメータの名前。
     *  - type : パラメータの型名。パラメータ・データ型のオリジナルの値範囲は
     *           属性 allowedValues の定義で限定することができる。
     * - allowedValues : パラメータが取ることのできる値。
     *                   パラメータ型に固有の定義を限定する必要がある場合のみ
     *                   この属性は使用される。例えば、文字列パラメータに許さ
     *                   れる値を列挙によって限定したり、数値型パラメータに許
     *                   される値を範囲によって限定したりする。パラメータに許
     *                   される値は、列挙、範囲またはインターバル構造で定義す
     *                   ることができる。
     *                   もしもパラメータに対する制約がない場合は、allowedValues
     *                   属性はnullとなる。すなわち、パラメータ型に固有の範囲
     *                   であればどのような値も取ることができる。
     *
     * @else
     * @brief SDO Parameter
     *
     * Data structure to define a variable (parameter) independently
     * of implementation technologies. The Parameter structure defines
     * the name and type of a variable.
     * Attributes defined in Parameter.
     *  - name : Parameter’s name.
     *  - type : Name of parameter's type. The original value scope of
     *           parameter data type can be constrained by definitions
     *           allocated in the attribute allowedValues.
     * - allowedValues : Values that the parameter can accept.
     *           This attribute is used only when the value scope
     *           inherent to the parameter type must be constrained. For
     *           example, the values allowed for a string parameter may
     *           be constrained by an enumeration, or the values
     *           allowed for a numeric parameter may be constrained by
     *           a range. The values allowed for a parameter can be
     *           defined in enumeration, range, or interval structures.
     *           The value of attribute allowedValues is null if there is
     *           no constraint on a parameter value, that is, any value
     *           can be assigned to the parameter as far as it follows the
     *           value scope inherent to the parameter’s type.
     * @endif
     *
     *    struct Parameter
     *    {
     *      string         name;
     *      TypeCode  type;
     *      AllowedValues allowed_values;
     *    };
     */

    /*!
     * @if jp
     * @brief Lock 付き SDO ParameterList
     * @else
     * @brief SDO ParameterList with mutex lock
     * @endif
     */
    ParameterList m_parameters;
    std::mutex m_params_mutex;

    /*!
     * @if jp
     * @brief Lock 付き SDO ConfigurationSetList
     * @else
     * @brief SDO ConfigurationSetList with mutex lock
     * @endif
     */
    /*
      struct ConfigurationSet
      {
      string id;
      string description;
      NVList configuration_data;
      };
    */
    RTC::ConfigAdmin& m_configsets;
    std::mutex m_config_mutex;

    /*!
     * @if jp
     * @brief Lock 付き SDO Service 管理オブジェクト
     * @else
     * @brief SDO Service admin object with mutex lock
     * @endif
     */
    RTC::SdoServiceAdmin& m_sdoservice;
    std::mutex m_sdoservice_mutex;

    /*!
     * @if jp
     * @brief  Lock 付き SDO OrganizationList
     * @else
     * @brief SDO OrganizationList with mutex lock
     * @endif
     */
    OrganizationList m_organizations;
    std::mutex m_org_mutex;

    /*!
     * @if jp
     * @brief  NVList用functor
     * @else
     * @brief  Functor for NVList
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
    };

    /*!
     * @if jp
     * @brief  Organization用functor
     * @else
     * @brief  Functor for Organization
     * @endif
     */
    struct org_id
    {
      explicit org_id(const char* id) : m_id(id) {}
#ifdef ORB_IS_ORBEXPRESS
      bool operator()(const Organization_var& o)
#else
      bool operator()(const Organization_ptr& o)
#endif
      {
        CORBA::String_var id(o->get_organization_id());
        return m_id == (const char *)id;
      }
      const std::string m_id;
    };

    /*!
     * @if jp
     * @brief  ConfigurationSet用functor
     * @else
     * @brief  Functor for ConfigurationSet
     * @endif
     */
    struct config_id
    {
      explicit config_id(const char* id) :  m_id(id) {}
      bool operator()(const ConfigurationSet& c)
      {
        std::string id(c.id);
        return m_id == id;
      }
      const std::string m_id;
    };
  };  // class Configuration_impl
} // namespace SDOPackage

#endif  // RTC_SDOCONFIGURATION_H
