// -*- C++ -*-
/*!
 * @file ManagerServant.h
 * @brief RTComponent manager servant implementation class
 * @date $Date: 2007-12-31 03:08:04 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008-2010
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

#ifndef RTM_MANAGERSERVANT_H
#define RTM_MANAGERSERVANT_H

#include <mutex>
#include <rtm/idl/ManagerSkel.h>
#include <rtm/Manager.h>
#include <rtm/SystemLogger.h>

#include <iostream>

/*!
 * @if jp
 * @namespace RTM
 *
 * @brief RT-Middleware
 *
 *
 * @else
 * @namespace RTM
 *
 * @brief RT-Middleware
 *
 *
 * @endif
 */
namespace RTM
{
  /*!
   * @if jp
   * @brief ManagerのCORBA化クラス
   *
   * ManagerをCORBAサーバント化し、外部からコンポーネントの生成・削除、
   * システム状態の取得などが行える。
   *
   * @else
   * @brief Manager CORBA class
   *
   * This class changes Manager to CORBA Servant.
   * Generation/deletion of the component, to get the state of the system,
   * etc. can be done from the outside.
   *
   * @endif
   */
  class ManagerServant
    : public virtual POA_RTM::Manager,
      public virtual PortableServer::RefCountServantBase
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
    // standard constructor
    ManagerServant();

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
    ~ManagerServant() override;

    /*!
     * @if jp
     * @brief モジュールをロードする
     *
     * 当該マネージャに指定されたモジュールをロードし、指定された初期化
     * 関数で初期化を行う。
     *
     * @param pathname モジュールへのパス
     * @param initfunc モジュールの初期化関数
     * @return リターンコード
     *
    // standard constructor
     * @else
     * @brief Loading a module
     *
     * This operation loads a specified loadable module、and perform
     * initialization with the specified function.
     *
     * @param pathname A path to a loading module.
     * @param initfunc Module initialization function.
     * @return The return code.
     *
     * @endif
     */
    RTC::ReturnCode_t load_module(const char* pathname, const char* initfunc) override;

    /*!
     * @if jp
     * @brief モジュールをアンロードする
     *
     * 当該マネージャに指定されたモジュールをアンロードする。
     *
     * @param pathname モジュールへのパス
     * @return リターンコード
     *
     * @else
     * @brief Unloading a module
     *
     * This operation unloads a specified loadable module.
     *
     * @param pathname A path to a loading module.
     * @return The return code.
     *
     * @endif
     */
    RTC::ReturnCode_t unload_module(const char* pathname) override;

    /*!
     * @if jp
     * @brief ロード可能なモジュールのプロファイルを取得する
     *
     * ロード可能なモジュールのプロファイルを取得する。
     *
     * @return モジュールプロファイル
     *
     * @else
     * @brief Getting loadable module profiles
     *
     * This operation returns loadable module profiles.
     *
     * @return A module profile list.
     *
     * @endif
     */
    RTM::ModuleProfileList* get_loadable_modules() override;

    /*!
     * @if jp
     * @brief ロード済みのモジュールのプロファイルを取得する
     *
     * ロード済みのモジュールのプロファイルを取得する。
     *
     * @return モジュールプロファイル
     *
     * @else
     * @brief Getting loaded module profiles
     *
     * This operation returns loaded module profiles.
     *
     * @return A module profile list.
     *
     * @endif
     */
    RTM::ModuleProfileList* get_loaded_modules() override;

    // component 関連
    /*!
     * @if jp
     * @brief コンポーネントファクトリのプロファイルを取得する
     *
     * ロード済みのモジュールのうち、RTコンポーネントのモジュールが持つ
     * ファクトリのプロファイルのリストを取得する。
     *
     * @return コンポーネントファクトリのプロファイルリスト
     *
     * @else
     * @brief Getting component factory profiles
     *
     * This operation returns component factory profiles from loaded
     * RT-Component module factory profiles.
     *
     * @return An RT-Component factory profile list.
     *
     * @endif
     */
    RTM::ModuleProfileList* get_factory_profiles() override;

    /*!
     * @if jp
     * @brief コンポーネントを生成する
     *
     * 引数に指定されたコンポーネントを生成する。
     *
     * @return 生成されたRTコンポーネント
     *
     * @else
     * @brief Creating an RT-Component
     *
     * This operation creates RT-Component according to the string
     * argument.
     *
     * @return A created RT-Component
     *
     * @endif
     */
    RTC::RTObject_ptr create_component(const char* module_name) override;

    /*!
     * @if jp
     * @brief コンポーネントを削除する
     *
     * 引数に指定されたコンポーネントを削除する。
     *
     * @return リターンコード
     *
     * @else
     * @brief Deleting an RT-Component
     *
     * This operation delete an RT-Component according to the string
     * argument.
     *
     * @return Return code
     *
     * @endif
     */
    RTC::ReturnCode_t delete_component(const char* instance_name) override;

    /*!
     * @if jp
     * @brief 起動中のコンポーネントのリストを取得する
     *
     * 現在当該マネージャ上で起動中のコンポーネントのリストを返す。
     *
     * @return RTコンポーネントのリスト
     *
     * @else
     * @brief Getting RT-Component list running on this manager
     *
     * This operation returns RT-Component list running on this manager.
     *
     * @return A list of RT-Components
     *
     * @endif
     */
    RTC::RTCList* get_components() override;

    /*!
     * @if jp
     * @brief 起動中のコンポーネントプロファイルのリストを取得する
     *
     * 現在当該マネージャ上で起動中のコンポーネントのプロファイルのリス
     * トを返す。
     *
     * @return RTコンポーネントプロファイルのリスト
     *
     * @else
     * @brief Getting RT-Component's profile list running on this manager
     *
     * This operation returns RT-Component's profile list running on
     * this manager.
     *
     * @return A list of RT-Components' profiles
     *
     * @endif
     */
    RTC::ComponentProfileList* get_component_profiles() override;

    // manager 基本
    /*!
     * @if jp
     * @brief マネージャのプロファイルを取得する
     *
     * 現在当該マネージャのプロファイルを取得する。
     *
     * @return マネージャプロファイル
     *
     * @else
     * @brief Getting this manager's profile.
     *
     * This operation returns this manager's profile.
     *
     * @return Manager's profile
     *
     * @endif
     */
    RTM::ManagerProfile* get_profile() override;

    /*!
     * @if jp
     * @brief マネージャのコンフィギュレーションを取得する
     *
     * 現在当該マネージャのコンフィギュレーションを取得する。
     *
     * @return マネージャコンフィギュレーション
     *
     * @else
     * @brief Getting this manager's configuration.
     *
     * This operation returns this manager's configuration.
     *
     * @return Manager's configuration
     *
     * @endif
     */
    RTM::NVList* get_configuration() override;

    /*!
     * @if jp
     * @brief マネージャのコンフィギュレーションを設定する
     *
     * 現在当該マネージャのコンフィギュレーションを設定する。
     *
     * @param name セットするコンフィギュレーションのキー名
     * @param value セットするコンフィギュレーションの値
     * @return リターンコード
     *
     * @else
     * @brief Setting manager's configuration
     *
     * This operation sets managers configuration.
     *
     * @param name A configuration key name to be set
     * @param value A configuration value to be set
     * @return Return code
     *
     * @endif
     */
    RTC::ReturnCode_t set_configuration(const char* name, const char* value) override;

    /*!
     * @if jp
     * @brief マネージャがマスターかどうか
     *
     * この関数はマネージャがマスターかどうかを返す。Trueならば、当該マ
     * ネージャはマスターであり、それ以外は False を返す。
     *
     * @return マスターマネージャかどうかのbool値
     *
     * @else
     * @brief Whether this manager is master or not
     *
     * It returns "True" if this manager is a master, and it returns
     * "False" in other cases.
     *
     * @return A boolean value that means it is master or not.
     *
     * @endif
     */
    ::CORBA::Boolean is_master() override;

    /*!
     * @if jp
     * @brief マスターマネージャの取得
     *
     * このマネージャがスレーブマネージャの場合、マスターとなっているマ
     * ネージャのリストを返す。このマネージャがマスターの場合、空のリス
     * トが返る。
     *
     * @return マスターマネージャのリスト
     *
     * @else
     * @brief Getting master managers
     *
     * This operation returns master manager list if this manager is
     * slave. If this manager is master, an empty sequence would be
     * returned.
     *
     * @return Master manager list
     *
     * @endif
     */
    RTM::ManagerList* get_master_managers() override;

    /*!
     * @if jp
     * @brief マスターマネージャの追加
     *
     * このマネージャのマスタとしてマネージャを一つ追加する。戻り値には、
     * 当該マネージャ上で追加されたマスターマネージャを識別するユニーク
     * なIDが返される。このマネージャがマスタの場合、当該IDで指定された
     * マスターマネージャを返す。IDで指定されたマスターマネージャがない
     * 場合、nilオブジェクトが返る。
     *
     * @return マスターマネージャ
     *
     * @else
     * @brief Getting a master manager
     *
     * This operation returns a master manager with specified id. If
     * the manager with the specified id does not exist, nil object
     * reference would be returned.
     *
     * @return A master manager
     *
     * @endif
     */
    RTC::ReturnCode_t add_master_manager(RTM::Manager_ptr mgr) override;

    /*!
     * @if jp
     * @brief マスターマネージャの削除
     *
     * このマネージャが保持するマスタのうち、指定されたものを削除する。
     *
     * @param mgr マスターマネージャ
     * @return ReturnCode_t
     *
     * @else
     * @brief Removing a master manager
     *
     * This operation removes a master manager from this manager.
     *
     * @param mgr A master manager
     * @return ReturnCode_t
     *
     * @endif
     */
    RTC::ReturnCode_t remove_master_manager(RTM::Manager_ptr mgr) override;

    /*!
     * @if jp
     * @brief スレーブマネージャの取得
     *
     * このマネージャがスレーブマネージャの場合、スレーブとなっているマ
     * ネージャのリストを返す。このマネージャがスレーブの場合、空のリス
     * トが返る。
     *
     * @return スレーブマネージャのリスト
     *
     * @else
     * @brief Getting slave managers
     *
     * This operation returns slave manager list if this manager is
     * slave. If this manager is slave, an empty sequence would be
     * returned.
     *
     * @return Slave manager list
     *
     * @endif
     */
    RTM::ManagerList* get_slave_managers() override;

    /*!
     * @if jp
     * @brief スレーブマネージャの追加
     *
     * このマネージャのマスタとしてマネージャを一つ追加する。
     *
     * @param mgr スレーブマネージャ
     * @return ReturnCode_t
     *
     * @else
     * @brief Getting a slave manager
     *
     * This operation add a slave manager to this manager.
     *
     * @param mgr A slave manager
     * @return ReturnCode_t
     *
     * @endif
     */
    RTC::ReturnCode_t add_slave_manager(RTM::Manager_ptr mgr) override;

    /*!
     * @if jp
     * @brief スレーブマネージャの削除
     *
     * このマネージャが保持するマスタのうち、指定されたものを削除する。
     *
     * @param mgr スレーブマネージャ
     * @return ReturnCode_t
     *
     * @else
     * @brief Removing a slave manager
     *
     * This operation removes a slave manager from this manager.
     *
     * @param mgr A slave manager
     * @return ReturnCode_t
     *
     * @endif
     */
    RTC::ReturnCode_t remove_slave_manager(RTM::Manager_ptr mgr) override;

    /*!
     * @if jp
     * @brief プロセスのコピーを生成する
     * @return ReturnCode_t
     * @else
     * @brief The copy of the process is generated.
     * @return ReturnCode_t
     * @endif
     */
    RTC::ReturnCode_t fork() override;
    /*!
     * @if jp
     * @brief shutdownする
     * @return ReturnCode_t
     * @else
     * @brief This method shutdowns RTC.
     * @return ReturnCode_t
     * @endif
     */
    RTC::ReturnCode_t shutdown() override;
    /*!
     * @if jp
     * @brief 再起動する。
     * @return ReturnCode_t
     * @else
     * @brief This method restarts RTC.
     * @return ReturnCode_t
     * @endif
     */
    RTC::ReturnCode_t restart() override;

     /*!
     * @if jp
     * @brief 指定名のRTCオブジェクトリファレンスを取得
     *
     * @param name RTC名
     * @return RTCリスト
     *
     * @else
     * @brief 
     *
     * @param name name
     * @return list of RTC
     *
     * @endig
     */
    RTC::RTCList* get_components_by_name(const char* name) override;

    /*!
     * @if jp
     * @brief RTCのリファレンスを取得する。
     * @return RTCのリファレンス
     * @else
     * @brief Get the reference of RTC.
     * @return RTC reference
     * @endif
     */
    CORBA::Object_ptr get_service(const char* name) override;

    /*!
     * @if jp
     * @brief Managerのリファレンスを取得する。
     * @return Managerのリファレンス
     * @else
     * @brief Get the reference of Manager.
     * @return Manager reference
     * @endif
     */
    RTM::Manager_ptr getObjRef() const;

    /*!
     * @if jp
     * @brief INSManagerの生成
     * @return
     * @else 成功:true, 失敗:false
     * @brief Generate INSManager.
     * @return Successful:true, Failed:false
     * @endif
     */
    bool createINSManager();

    /*!
     * @if jp
     * @brief Managerのリファレンスを検索する。
     * @return Managerのリファレンス
     * @else
     * @brief Find the reference of Manager.
     * @return Manager reference
     * @endif
     */
    RTM::Manager_ptr findManager(const std::string& host_port);

    /*!
      * @if jp
      * @brief 指定名のマネージャを取得
      *
      * マネージャがマスターの場合は登録されているスレーブマネージャか
      * ら検索する。マネージャがスレーブの場合は登録されているマスター
      * マネージャからスレーブマネージャを検索する
      *
      * @param manager_name マネージャ名
      * @return マネージャの参照
      *
      * @else
      * @brief Obtain Manager's reference by name
      *
      * If this is master manager, find it from the registered slave managers.
      * If this is slave manager, request it to its master manager.
      *
      * @param manager_name Manager's name
      * @return Manager's object reference 
      * @endif
      */
    RTM::Manager_ptr findManagerByName(const std::string& mgr_name);

    /*!
      * @if jp
      * @brief マネージャのリストから指定名を持つマネージャを取得
      *
      * 引数に与えられたマネージャに対して、指定されたマネージャ名を持
      * つマネージャの参照を返すとともに、もしなければ、再帰的にスレー
      * ブマネージャを探索して、指定された名称を持つマネージャの参照を
      * 返す。この関数はクラス内から呼ぶための内部関数である。引数に与
      * えるマネージャリストは必要に応じてMutexで保護すること。
      *
      * @param manager_name マネージャ名
      * @param mlist マネージャのリスト
      * @return マネージャの参照
      *
      * @else
      * @brief Obtain Manager's reference by name
      *
      * This function search a specified name manager from the given
      * manager list. And if it is not found, this function also
      * search from its slave managers recursively. This function is
      * internal function. The given manager list must be guarded by
      * mutex.
      *
      * @param manager_name Manager's name
      * @param mlist Manager list
      * @return Manager's object reference 
      * @endif
      */
    RTM::Manager_ptr findManagerFromList(const std::string& name,
                                         RTM::ManagerList& mlist);

    /*!
     * @if jp
     * @brief 指定のマネージャでRTCを起動する
     *
     * comp&manager_name=mgr のようにRTC名&manager_name=マネージャ名と
     * 指定する
     *
     * @param module_name 起動するRTC、マネージャ名
     * @return 生成されたRTCのオブジェクト参照
     *
     * @else
     *
     * @brief Launch RTC on specified manager
     *
     * Manager is specified with "manager_name" key such as
     * "comp&manager_name=mgr" in the RTC launch parameter.
     *
     * @param module_name Given parameter to create RTC
     * @return created RTObject object reference
     *
     * @endif
     */
    RTC::RTObject_ptr
    createComponentByManagerName(std::string& create_arg, std::string& mgrstr);

    /*
     * @if jp
     * @brief 指定アドレスのマネージャでRTCを起動する
     *
     * comp&manager_address=localhost:2810 のようにRTC名
     * &manager_address=マネージャのホスト名、ポート番号を指定する
     *
     * @param module_name 起動するRTC、マネージャのホストアドレス
     * @return 生成されたRTCのオブジェクト参照
     *
     * @else
     * @brief Launch a RTC on the specified IP/Port address manager
     *
     * @param module_name Given parameter to create RTC
     * @return created RTObject object reference
     *
     * @endif
     */
    RTC::RTObject_ptr
    createComponentByAddress(std::string& create_arg, std::string& mgrstr);

    /*
     * @if jp
     * @brief マスターマネージャの有無を確認してリストを更新する
     *
     *
     *
     * @else
     * @brief 
     *
     *
     * @endif
     */
    void updateMasterManager();
    /*
     * @if jp
     * @brief create引数から指定されたキーの値を返す
     *
     * create引数から指定されたキーを値を返すとともに、そのキーと値を削除した引
     * 数を返す。
     * module_name: module_name?param1=value1&param2=value2&param3...
     * param_name:  param2
     * の場合
     * 返り値: value2
     * module_name: module_name?param1=value1&param3...
     * となる。
     *
     * @else @brief returns value of specified param_name from create arg
     *
     * This function returns the value of specified param_name from
     * create argument, and delete param_name=value from the create
     * arg string. If the arguments are
     * module_name: module_name?param1=value1&param2=value2&param3...
     * param_name:  param2
     * this function returns
     * ret value: value2
     * module_name: module_name?param1=value1&param3...
     *
     * @endif
     */
    std::string getParameterByModulename(const std::string& param_name, std::string &module_name);
    static bool isProcessIDManager(const std::string& mgrname);

  private:
    /*!
     * @if jp
     * @brief ロガーオブジェクト
     * @else
     * @brief Logger object
     * @endif
     */
    ::RTC::Logger rtclog{"ManagerServant"};

    /*!
     * @if jp
     * @brief RTC::Managerへの参照
     * @else
     * @brief Reference to the RTC::Manager
     * @endif
     */
    ::RTC::Manager& m_mgr{::RTC::Manager::instance()};

    /*!
     * @if jp
     * @brief ManagerServantのオブジェクトリファレンス
     * @else
     * @brief An object reference of ManagerServant
     * @endif
     */
    ::RTM::Manager_var m_objref{RTM::Manager::_nil()};

    /*!
     * @if jp
     * @brief マスタマネージャのリスト
     * @else
     * @brief List of master managers
     * @endif
     */
    ::RTM::ManagerList m_masters;

    /*!
     * @if jp
     * @brief m_masters の Mutex
     * @else
     * @brief Mutex of m_masters
     * @endif
     */
    ::std::mutex m_masterMutex;

    /*!
     * @if jp
     * @brief スレーブマネージャのリスト
     * @else
     * @brief List of slave managers
     * @endif
     */
    ::RTM::ManagerList m_slaves;

    /*!
     * @if jp
     * @brief m_slaves の Mutex
     * @else
     * @brief Mutex of m_slaves
     * @endif
     */
    ::std::mutex m_slaveMutex;

    /*!
     * @if jp
     * @brief マスタかどうかのフラグ
     * @else
     * @brief Flag if this is master
     * @endif
     */
    CORBA::Boolean m_isMaster{false};

    /*!
     * @if jp
     * @brief Manager_var が等価かどうかのファンクタ
     * @else
     * @brief Functor to inspect if Manster_var is equivalent
     * @endif
     */
    class is_equiv
    {
      RTM::Manager_var m_mgr;
    public:

      is_equiv(RTM::Manager_ptr mgr)
        : m_mgr(RTM::Manager::_duplicate(mgr)) {}
#ifdef ORB_IS_ORBEXPRESS
      bool operator()(RTM::Manager_var mgr)
      {
        return m_mgr->_is_equivalent(mgr.in());
      }
#else
      bool operator()(RTM::Manager_ptr mgr)
      {
        return m_mgr->_is_equivalent(mgr);
      }
#endif
    };


  };


  class CompParam
  {
  public:
    CompParam(std::string module_name);
    ~CompParam();
    static const unsigned int prof_list_size = 6;
    static const char* prof_list[prof_list_size];
    std::string vendor();
    std::string category();
    std::string impl_id();
    std::string language();
    std::string version();
  private:
    std::string m_type;
    std::string m_vendor;
    std::string m_category;
    std::string m_impl_id;
    std::string m_language;
    std::string m_version;
  
  };

} // namespace RTM
#endif // RTM_MANAGERSERVANT_H
