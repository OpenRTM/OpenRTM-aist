/// -*- C++ -*-
/*!
 * @file Manager.h
 * @brief RTComponent manager class
 * @date $Date: 2007-12-31 03:08:04 $
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

#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <rtm/RTC.h>

#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <coil/Task.h>

#ifdef ORB_IS_TAO
#include <tao/Strategies/advanced_resource.h>
#include <tao/IORTable/IORTable.h>
#endif

#include <rtm/Factory.h>
#include <rtm/ECFactory.h>
#include <rtm/ObjectManager.h>
#include <rtm/SystemLogger.h>
#include <rtm/ManagerActionListener.h>

#ifdef ORB_IS_ORBEXPRESS
#include <RTPortableServer.h>
#endif

namespace RTM
{
  class ManagerServant;
}

namespace coil
{
  class Timer;
};

namespace RTC
{
//  class Properties;
  class CorbaNaming;
  class ModuleManager;
  class NamingManager;
  class Manager;
  class RTObject_impl;
  typedef RTObject_impl RtcBase;

  typedef void (*ModuleInitProc)(Manager* manager);

  /*!
   * @if jp
   * @class Manager
   * @brief Manager クラス
   *
   * コンポーネントなど各種の情報管理を行うマネージャクラス。
   *
   * @since 0.2.0
   *
   * @else
   * @class Manager
   * @brief Manager class
   *
   * This is a manager class that manages various information
   * such as components.
   *
   * @since 0.2.0
   *
   * @endif
   */
  class Manager
  {
    typedef coil::Mutex Mutex;
    typedef coil::Guard<Mutex> Guard;
  protected:
    /*!
     * @if jp
     * @brief Protected コンストラクタ
     *
     * Protected コンストラクタ
     *
     * @else
     * @brief Protected Constructor
     *
     * Protected Constructor
     *
     * @endif
     */
    Manager();

    /*!
     * @if jp
     * @brief Protected コピーコンストラクタ
     *
     * Protected コピーコンストラクタ
     *
     * @param manager コピー元マネージャオブジェクト
     *
     * @else
     * @brief Protected Copy Constructor
     *
     * Protected Copy Constructor
     *
     * @param manager Manager object of copy source
     *
     * @endif
     */
    Manager(const Manager& manager);
    //      Manager& operator=(const Manager& manager){return manager;};

  public:
    /*!
     * @if jp
     * @brief マネージャの初期化
     *
     * マネージャを初期化する static メンバ関数。
     * マネージャをコマンドライン引数を与えて初期化する。
     * マネージャを使用する場合は、必ずこの初期化メンバ関数 init() を
     * 呼ばなければならない。
     * マネージャのインスタンスを取得する方法として、init(), instance() の
     * 2つの static メンバ関数が用意されているが、初期化はinit()でしか
     * 行われないため、Manager の生存期間の一番最初にはinit()を呼ぶ必要がある。
     *
     * ※マネージャの初期化処理
     * - initManager: 引数処理、configファイルの読み込み、サブシステム初期化
     * - initLogger: Logger初期化
     * - initORB: ORB 初期化
     * - initNaming: NamingService 初期化
     * - initExecutionContext: ExecutionContext factory 初期化
     * - initTimer: Timer 初期化
     *
     * @param argc コマンドライン引数の数
     * @param argv コマンドライン引数
     *
     * @return Manager の唯一のインスタンスの参照
     *
     * @else
     * @brief Initialize manager
     *
     * This is the static member function to initialize the Manager.
     * The Manager is initialized by given commandline arguments.
     * To use the manager, this initialization member function init() must be
     * called. The manager has two static functions to get the instance such as
     * init() and instance(). Since initializing process is only performed by
     * the init() function, the init() has to be called at the beginning of
     * the lifecycle of the Manager.
     *
     * *Initialization of manager
     * - initManager: Argument processing, reading config file,
     *                initialization of subsystem
     * - initLogger: Initialization of Logger
     * - initORB: Initialization of ORB
     * - initNaming: Initialization of NamingService
     * - initExecutionContext: Initialization of ExecutionContext factory
     * - initTimer: Initialization of Timer
     *
     * @param argc The number of command line arguments.
     * @param argv The array of the command line arguments.
     *
     * @return Reference of the unique instance of Manager
     *
     * @endif
     */
    static Manager* init(int argc, char** argv);

    /*!
     * @if jp
     * @brief マネージャのインスタンスの取得
     *
     * マネージャのインスタンスを取得する static メンバ関数。
     * この関数を呼ぶ前に、必ずこの初期化メンバ関数 init() が呼ばれている
     * 必要がある。
     *
     * @return Manager の唯一のインスタンスの参照
     *
     * @else
     *
     * @brief Get instance of the manager
     *
     * This is the static member function to get the instance of the Manager.
     * Before calling this function, ensure that the initialization function
     * "init()" is called.
     *
     * @return The only instance reference of the manager
     *
     * @endif
     */
    static Manager& instance();

    //============================================================
    // Manager
    //============================================================

    /*!
     * @if jp
     * @brief マネージャ終了処理
     *
     * マネージャの終了処理を実行する。
     *
     * @else
     * @brief Terminate manager
     *
     * Terminate manager's processing
     *
     * @endif
     */
    void terminate();

    /*!
     * @if jp
     * @brief マネージャ・シャットダウン
     *
     * マネージャの終了処理を実行する。
     * ORB終了後、同期を取って終了する。
     *
     * @else
     * @brief Shutdown Manager
     *
     * Terminate manager's processing.
     * After terminating ORB, shutdown manager in sync.
     *
     * @endif
     */
    void shutdown();

    /*!
     * @if jp
     * @brief マネージャ終了処理の待ち合わせ
     *
     * 同期を取るため、マネージャ終了処理の待ち合わせを行う。
     *
     * @else
     * @brief Wait for Manager's termination
     *
     * Wait for Manager's termination to synchronize.
     *
     * @endif
     */
    void join();

    /*!
     * @if jp
     * @brief ログバッファの取得
     *
     * マネージャに設定したログバッファを取得する。
     *
     * @return マネージャに設定したログバッファ
     *
     * @else
     * @brief Get the log buffer
     *
     * Get the log buffer that has been set to manager.
     *
     * @return Log buffer to set to manager
     *
     * @endif
     */
    LogStreamBuf& getLogStreamBuf() {return m_logStreamBuf;}

    /*!
     * @if jp
     * @brief コンフィグレーションのログレベルの取得
     *
     * コンフィグレーションのログレベルを取得する。
     *
     * @return コンフィギュレーションのログレベル
     *
     * @else
     * @brief Get the log level of the configuration.
     *
     * Get the log level of the configuration.
     *
     * @return Log level of Manager's configuration
     *
     * @endif
     */
    std::string& getLogLevel() {return m_config["logger.log_level"];}

    /*!
     * @if jp
     * @brief マネージャコンフィギュレーションの取得
     *
     * マネージャに設定したコンフィギュレーションを取得する。
     *
     * @return マネージャのコンフィギュレーション
     *
     * @else
     * @brief Get the manager configuration
     *
     * Get the manager configuration that has been set to manager.
     *
     * @return Manager's configuration
     *
     * @endif
     */
    coil::Properties& getConfig() { return m_config;}

    /*!
     * @if jp
     *
     * @brief 初期化プロシージャのセット
     *
     * このオペレーションはユーザが行うモジュール等の初期化プロシージャ
     * を設定する。ここで設定されたプロシージャは、マネージャが初期化され、
     * アクティブ化された後、適切なタイミングで実行される。
     *
     * @param proc 初期化プロシージャの関数ポインタ
     *
     * @else
     *
     * @brief Set initial procedure
     *
     * This operation sets the initial procedure call to process module
     * initialization, other user defined initialization and so on.
     * The given procedure will be called at the proper timing after the
     * manager initialization, activation and run.
     *
     * @param proc A function pointer to the initial procedure call
     *
     * @endif
     */
    void setModuleInitProc(ModuleInitProc proc);

    /*!
     * @if jp
     *
     * @brief Managerのアクティブ化
     *
     * このオペレーションは以下の処理を行う
     * - CORBA POAManager のアクティブ化
     * - マネージャCORBAオブジェクトのアクティブ化
     * - Manager のオブジェクト参照の登録
     *
     * このオペレーションは、マネージャの初期化後、runManager()
     * の前に呼ぶ必要がある。
     *
     *
     * @return 処理結果(アクティブ化成功:true、失敗:false)
     *
     * @else
     *
     * @brief Activate the Manager
     *
     * This operation do the following:
     * - Activate CORBA POAManager
     * - Activate Manager CORBA object
     * - Bind object reference of the Manager to the nameserver
     *
     * This operation should be invoked after Manager:init(),
     * and before runManager().
     *
     * @return Activation result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool activateManager();

    /*!
     * @if jp
     *
     * @brief Managerの実行
     *
     * このオペレーションはマネージャのメインループを実行する。
     * このメインループ内では、CORBA ORBのイベントループ等が
     * 処理される。デフォルトでは、このオペレーションはブロックし、
     * Manager::destroy() が呼ばれるまで処理を戻さない。
     * 引数 no_block が true に設定されている場合は、内部でイベントループ
     * を処理するスレッドを起動し、ブロックせずに処理を戻す。
     *
     * @param no_block false: ブロッキングモード, true: ノンブロッキングモード
     *
     * @else
     *
     * @brief Run the Manager
     *
     * This operation processes the main event loop of the Manager.
     * In this main loop, CORBA's ORB event loop or other processes
     * are performed. As the default behavior, this operation is going to
     * blocking mode and never returns until Manager::destroy() is called.
     * When the given argument "no_block" is set to "true", this operation
     * creates a thread to process the event loop internally, and it doesn't
     * block and returns.
     *
     * @param no_block false: Blocking mode, true: non-blocking mode.
     *
     * @endif
     */
    void runManager(bool no_block = false);

    //============================================================
    // Module management
    //============================================================
    /*!
     * @if jp
     * @brief [CORBA interface] モジュールのロード
     *
     * 指定したコンポーネントのモジュールをロードするとともに、
     * 指定した初期化関数を実行する。
     *
     * @param fname   モジュールファイル名
     * @param initfunc 初期化関数名
     * @return 終了コード
     *         RTC::RTC_OK 正常終了
     *         RTC::RTC_ERROR ロード失敗・不明なエラー
     *         RTC::PRECONDITION_NOT_MET 設定にり許可されない操作
     *         RTC::BAD_PARAMETER 不正なパラメータ
     * 
     * @else
     *
     * @brief [CORBA interface] Load module
     *
     * Load specified module (shared library, DLL etc..),
     * and invoke initialize function.
     *
     * @param fname    The module file name
     * @param initfunc The initialize function name
     * @return Return code
     *         RTC::RTC_OK Normal return
     *         RTC::RTC_ERROR Load failed, or unknown error
     *         RTC::PRECONDITION_NOT_MET Not allowed operation by conf
     *         RTC::BAD_PARAMETER Invalid parameter
     *
     * @endif
     */
    ReturnCode_t load(const std::string& fname, const std::string& initfunc);

    /*!
     * @if jp
     *
     * @brief モジュールのアンロード
     *
     * モジュールをアンロードする
     *
     * @param fname モジュールのファイル名
     *
     * @else
     *
     * @brief Unload module
     *
     * Unload module.
     *
     * @param fname The module file name
     *
     * @endif
     */
    void unload(const char* fname);

    /*!
     * @if jp
     *
     * @brief 全モジュールのアンロード
     *
     * モジュールをすべてアンロードする
     *
     * @else
     *
     * @brief Unload all modules
     *
     * Unload all modules.
     *
     * @endif
     */
    void unloadAll();

    /*!
     * @if jp
     * @brief ロード済みのモジュールリストを取得する
     *
     * 現在マネージャにロードされているモジュールのリストを取得する。
     *
     * @return ロード済みモジュールリスト
     *
     * @else
     * @brief Get a list of loaded modules
     *
     * Get module list that is currently loaded into manager.
     *
     * @return Module list that has been loaded.
     *
     * @endif
     */
    std::vector<coil::Properties> getLoadedModules();

    /*!
     * @if jp
     * @brief ロード可能なモジュールリストを取得する
     *
     * ロード可能モジュールのリストを取得する。
     * (現在はModuleManager側で未実装)
     *
     * @return ロード可能モジュール　リスト
     *
     * @else
     * @brief Get a list of loadable modules
     *
     * Get loadable module list.
     * (Currently, unimplemented on ModuleManager side)
     *
     * @return Loadable module list
     *
     * @endif
     */
  std::vector<coil::Properties> getLoadableModules();

    //============================================================
    // Component Factory Management
    //============================================================
    /*!
     * @if jp
     * @brief RTコンポーネント用ファクトリを登録する
     *
     * RTコンポーネントのインスタンスを生成するための
     * Factoryを登録する。
     *
     * @param profile RTコンポーネント プロファイル
     * @param new_func RTコンポーネント生成用関数
     * @param delete_func RTコンポーネント破棄用関数
     *
     * @return 登録処理結果(登録成功:true、失敗:false)
     *
     * @else
     * @brief Register RT-Component Factory
     *
     * Register Factory to create RT-Component's instances.
     *
     * @param profile RT-Component profile
     * @param new_func RT-Component creation function
     * @param delete_func RT-Component destruction function
     *
     * @return Registration result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool registerFactory(coil::Properties& profile,
                         RtcNewFunc new_func,
                         RtcDeleteFunc delete_func);

    /*!
     * @if jp
     * @brief ファクトリのプロファイルを取得
     *
     * ファクトリのプロファイルを取得する。
     *
     * @return ファクトリのプロファイル
     *
     * @else
     * @brief Get profiles of factories.
     *
     * Get profiles of factories.
     *
     * @return profiles of factories
     *
     * @endif
     */
    std::vector<coil::Properties> getFactoryProfiles();

    /*!
     * @if jp
     * @brief ExecutionContext用ファクトリを登録する
     *
     * ExecutionContextのインスタンスを生成するための
     * Factoryを登録する。
     *
     * @param name 生成対象ExecutionContext名称
     * @param new_func ExecutionContext生成用関数
     * @param delete_func ExecutionContext破棄用関数
     *
     * @return 登録処理結果(登録成功:true、失敗:false)
     *
     * @else
     * @brief Register ExecutionContext Factory
     *
     * Register Factory to create ExecutionContext's instances.
     *
     * @param name ExecutionContext name for the creation
     * @param new_func ExecutionContext creation function
     * @param delete_func ExecutionContext destruction function
     *
     * @return Registration result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool registerECFactory(const char* name,
                           ECNewFunc new_func,
                           ECDeleteFunc delete_func);

    /*!
     * @if jp
     * @brief ファクトリ全リストを取得する
     *
     * 登録されているファクトリの全リストを取得する。
     *
     * @return 登録ファクトリ リスト
     *
     * @else
     * @brief Get the list of all Factories
     *
     * Get the list of all factories that have been registered.
     *
     * @return Registered factory list
     *
     * @endif
     */
    std::vector<std::string> getModulesFactories();

    //============================================================
    // Component management
    //============================================================
    /*!
     * @if jp
     * @brief RTコンポーネントを生成する
     *
     * 指定したRTコンポーネントのインスタンスを登録されたFactory経由
     * で生成する。
     *
     * 生成されるコンポーネントの各種プロファイルは以下の優先順位で
     * 設定される。
     *
     * -# createComponent() の引数で与えられたプロファイル
     * -# rtc.confで指定された外部ファイルで与えられたプロファイル
     * --# category.instance_name.config_file
     * --# category.component_type.config_file
     * -# コードに埋め込まれたプロファイル
     *
     * インスタンス生成が成功した場合、併せて以下の処理を実行する。
     *  - 外部ファイルで設定したコンフィギュレーション情報の読み込み，設定
     *  - ExecutionContextのバインド，動作開始
     *  - ネーミングサービスへの登録
     *
     * @param comp_args 生成対象RTコンポーネントIDおよびコンフィギュレー
     * ション引数。フォーマットは大きく分けて "id" と "configuration"
     * 部分が存在する。
     *
     * comp_args:     [id]?[configuration]
     *                id は必須、configurationはオプション
     * id:            RTC:[vendor]:[category]:[implementation_id]:[version]
     *                RTC は固定かつ必須
     *                vendor, category, version はオプション
     *                implementation_id は必須
     *                オプションを省略する場合でも ":" は省略不可
     * configuration: [key0]=[value0]&[key1]=[value1]&[key2]=[value2].....
     *                RTCが持つPropertiesの値をすべて上書きすることができる。
     *                key=value の形式で記述し、"&" で区切る
     *
     * 例えば、
     * RTC:jp.go.aist:example:ConfigSample:1.0?conf.default.str_param0=munya
     * RTC::example:ConfigSample:?conf.default.int_param0=100
     *
     * @return 生成したRTコンポーネントのインスタンス
     *
     * @else
     * @brief Create RT-Components
     *
     * Create specified RT-Component's instances via registered Factory.
     * When its instances have been created successfully, the following
     * processings are also executed.
     *  - Read and set configuration information that was set by external file.
     *  - Bind ExecutionContext and start operation.
     *  - Register to naming service.
     *
     * @param module_name Target RT-Component names for the creation
     *
     * @return Created RT-Component's instances
     *
     * @endif
     */
    RTObject_impl* createComponent(const char* comp_args);
    /*!
     * @if jp
     * @brief Contextを生成する
     *
     * @return 生成したConetextのインスタンス
     *
     * @else
     * @brief Create Context
     *
     * @return Created Context's instances
     *
     * @endif
     */
    ExecutionContextBase* createContext(const char* ec_args);

    /*!
     * @if jp
     * @brief RTコンポーネントの登録解除
     *
     * 指定したRTコンポーネントのインスタンスをネーミングサービスから
     * 登録解除する。
     *
     * @param comp 登録解除対象RTコンポーネント
     *
     * @else
     * @brief Unregister RT-Components
     *
     * Unregister specified RT-Component's instances from naming service.
     *
     * @param comp Target RT-Components for the unregistration
     *
     * @endif
     */
    void cleanupComponent(RTObject_impl* comp);

    /*!
     * @if jp
     * @brief RTコンポーネントの削除する
     *
     * notifyFinalized()によって登録されたRTコンポーネントを削除する。
     *
     * @else
     * @brief This method deletes RT-Components.
     *
     * This method deletes RT-Components registered by notifyFinalized().
     *
     * @endif
     */
    void cleanupComponents();

    /*!
     * @if jp
     * @brief RTコンポーネントの削除する
     *
     * 削除するRTコンポーネントを登録する。
     * 登録されたRTコンポーネントは cleanupComponents() で削除される。
     *
     * @param 削除するRTコンポーネント
     *
     * @else
     * @brief This method deletes RT-Components.
     *
     * The deleted RT-Component is registered. The registered RT-Components
     * are deleted by cleanupComponents().
     *
     * @param Deleted RT component
     * @endif
     */
    void notifyFinalized(RTObject_impl* comp);

    /*!
     * @if jp
     * @brief RTコンポーネントを直接 Manager に登録する
     *
     * 指定したRTコンポーネントのインスタンスを
     * ファクトリ経由ではなく直接マネージャに登録する。
     *
     * @param comp 登録対象RTコンポーネントのインスタンス
     *
     * @return 登録処理結果(登録成功:true、失敗:false)
     *
     * @else
     * @brief Register RT-Component directly without Factory
     *
     * Register specified RT-Component's instances not via Factory
     * to Manager directly.
     *
     * @param comp Target RT-Component's instances for the registration
     *
     * @return Registration result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool registerComponent(RTObject_impl* comp);

    /*!
     * @if jp
     * @brief RTコンポーネントの登録を解除する
     *
     * 指定したRTコンポーネントの登録を解除する。
     *
     * @param comp 登録解除対象RTコンポーネントのインスタンス
     *
     * @return 登録解除処理結果(解除成功:true、解除失敗:false)
     *
     * @else
     * @brief Unregister RT-Components
     *
     * Unregister specified RT-Components
     *
     * @param comp Target RT-Component's instances for the unregistration
     *
     * @return Unregistration result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool unregisterComponent(RTObject_impl* comp);


    /*!
     * @if jp
     * @brief Manager に登録されているRTコンポーネントを削除する
     *
     * マネージャに登録されているRTコンポーネントを削除する。
     * 指定されたRTコンポーネントをネーミングサービスから削除し、
     * RTコンポーネント自体を終了させるとともに、インスタンスを解放する。
     *
     * @param comp 削除対象RTコンポーネントのインスタンス
     *
     * @else
     * @brief Unregister RT-Components that have been registered to Manager
     *
     * Unregister RT-Components that have been registered to manager
     * Remove specified RT-Component from naming service, terminate itself
     * and release its instances.
     *
     * @param comp Target RT-Component's instances for the unregistration
     *
     * @endif
     */
    void deleteComponent(RTObject_impl* comp);

    /*!
     * @if jp
     * @brief Manager に登録されているRTコンポーネントを削除する
     *
     * マネージャに登録されているRTコンポーネントを削除する。
     * 指定されたRTコンポーネントをネーミングサービスから削除し、
     * RTコンポーネント自体を終了させるとともに、インスタンスを解放する。
     *
     * @param instance_name 削除対象RTコンポーネントのインスタンス名
     *
     * @else
     * @brief Unregister RT-Components that have been registered to Manager
     *
     * Unregister RT-Components that have been registered to manager
     * Remove specified RT-Component from naming service, terminate itself
     * and release its instances.
     *
     * @param instance_name Target RT-Component's instances for the
     *                      unregistration
     *
     * @endif
     */
    void deleteComponent(const char* instance_name);


    /*!
     * @if jp
     * @brief Manager に登録されているRTコンポーネントを検索する
     *
     * Manager に登録されているRTコンポーネントを指定した名称で検索し、
     * 合致するコンポーネントを取得する。
     *
     * @param instance_name 検索対象RTコンポーネントの名称
     *
     * @return 名称が一致するRTコンポーネントのインスタンス
     *
     * @else
     * @brief Get RT-Component's pointer
     *
     * Search RT-Component that has been registered to Manager by its specified
     * name, and get it that matches.
     *
     * @param instance_name Target RT-Component's name for searching
     *
     * @return Target RT-Component's instances that matches
     *
     * @endif
     */
    RTObject_impl* getComponent(const char* instance_name);

    /*!
     * @if jp
     * @brief Manager に登録されている全RTコンポーネントを取得する
     *
     * Manager に登録されているRTコンポーネントの全インスタンスを取得する。
     *
     * @return 全RTコンポーネントのインスタンスリスト
     *
     * @else
     * @brief Get all RT-Components registered in the Manager
     *
     * Get all RT-Component's instances that have been registered to Manager.
     *
     * @return List of all RT-Component's instances
     *
     * @endif
     */
    std::vector<RTObject_impl*> getComponents();


    void
    addManagerActionListener(RTM::ManagerActionListener* listener,
                             bool autoclean = true);
    void
    removeManagerActionListener(RTM::ManagerActionListener* listener);

    void
    addModuleActionListener(RTM::ModuleActionListener* listener,
                             bool autoclean = true);
    void
    removeModuleActionListener(RTM::ModuleActionListener* listener);

    void
    addRtcLifecycleActionListener(RTM::RtcLifecycleActionListener* listener,
                                  bool autoclean = true);
    void
    removeRtcLifecycleActionListener(RTM::RtcLifecycleActionListener* listener);

    void
    addNamingActionListener(RTM::NamingActionListener* listener,
                            bool autoclean = true);
    void
    removeNamingActionListener(RTM::NamingActionListener* listener);

    void
    addLocalServiceActionListener(RTM::LocalServiceActionListener* listener,
                                       bool autoclean = true);
    void
    removeLocalServiceActionListener(RTM::LocalServiceActionListener* listener);


    //============================================================
    // CORBA 関連
    //============================================================
    /*!
     * @if jp
     * @brief ORB のポインタを取得する (所有権保持)
     *
     * Manager に設定された ORB のポインタを取得する。所有権は保持され
     * たままである。_var型で受ける場合は getORB() を用いること。この関
     * 数は以下のようにORBのオペレーションを直接呼ぶ場合に利用する。
     *
     * Manager::instance().theORB()->string_to_object(<IOR string>);
     *
     * @return ORB オブジェクト
     *
     * @else
     * @brief Get the pointer to ORB (ownership retained)
     *
     * Get the pointer to ORB that has been set to Manager. The
     * ownership of ORB is retained. If the returned value is assigned
     * to _var variable, getORB() operation must be used. This
     * operation is provided for the following use.
     *
     * Manager::instance().theORB()->string_to_object(<IOR string>);
     *
     * @return ORB object
     *
     * @endif
     */
    CORBA::ORB_ptr theORB();

    /*!
     * @if jp
     * @brief ORB のポインタを取得する (所有権複製)
     *
     * Manager に設定された ORB のポインタを取得する。所有権はコピーさ
     * れ（リファレンスカウントが1増加する。）、受け取った側でも所有権
     * を獲得する。したがって、使用後にはリファレンスを解放する必要があ
     * る。通常は以下のように戻り値を _var 型変数で受け取り、自動的に解
     * 放させる方法が推奨される。
     *
     * {
     *   CORBA::ORB_var orb = Manager::instance().getORB();
     *   orb->string_to_object(<IOR string>);
     * } // ORB's reference in orb is released here.
     *
     * @return ORB オブジェクト
     *
     * @else
     * @brief Get the pointer to ORB (ownership duplicated)
     *
     * Get the pointer to ORB that has been set to Manager. The
     * ownership is copied (the reference count is incremented.),
     * calling side also obtains reference ownership. The obtained
     * reference must be released after use. Usually it is recommended
     * that return value are assigned to _var type variables are
     * wonership would be released automatically.
     *
     * {
     *   CORBA::ORB_var orb = Manager::instance().getORB();
     *   orb->string_to_object(<IOR string>);
     * } // ORB's reference in orb is released here.
     *
     * @return ORB object
     *
     * @endif
     */
    CORBA::ORB_ptr getORB();

    /*!
     * @if jp
     * @brief Manager が持つ RootPOA のポインタを取得する (所有権保持)
     *
     * Manager に設定された RootPOA へのポインタを取得する。所有権は保持され
     * たままである。_var型で受ける場合は getPOA() を用いること。この関
     * 数は以下のようにPOAのオペレーションを直接呼ぶ場合に利用する。
     *
     * Manager::instance().getPOA()->servant_to_id(m_servant);
     *
     * @return RootPOAオブジェクト
     *
     * @else
     * @brief Get a pointer to RootPOA held by Manager
     *
     * Get the pointer to RootPOA that has been set to Manager. The
     * ownership of POA is retained. If the returned value is assigned
     * to _var variable, getPOA() operation must be used. This
     * operation is provided for the following use.
     *
     * Manager::instance().getPOA()->servant_to_id(m_servant);
     *
     * @return RootPOA object
     *
     * @endif
     */

    PortableServer::POA_ptr thePOA();
#ifdef ORB_IS_OMNIORB
    PortableServer::POA_ptr theShortCutPOA();
#endif
    /*!
     * @if jp
     * @brief Manager が持つ RootPOA のポインタを取得する (所有権複製)
     *
     * Manager に設定された RootPOA へのポインタを取得する。所有権はコ
     * ピーされ（リファレンスカウントが1増加する。）、受け取った側でも
     * 所有権を獲得する。したがって、使用後にはリファレンスを解放する必
     * 要がある。通常は以下のように戻り値を _var 型変数で受け取り、自動
     * 的に解放させる方法が推奨される。
     *
     * {
     *   PortableServer::POA_var poa = Manager::instance().getPOA();;
     *   poa->servant_to_id(m_servant);
     * } // POA's reference in orb is released here.
     *
     * @return RootPOAオブジェクト
     *
     * @else
     * @brief Get a pointer to RootPOA held by Manager
     *
     * Get the pointer to RootPOA that has been set to Manager. The
     * ownership is copied (the reference count is incremented.),
     * calling side also obtains reference ownership. The obtained
     * reference must be released after use. Usually it is recommended
     * that return value are assigned to _var type variables are
     * wonership would be released automatically.
     *
     * {
     *   PortableServer::POA_var poa = Manager::instance().getPOA();;
     *   poa->servant_to_id(m_servant);
     * } // POA's reference in orb is released here.
     *
     * @return RootPOA object
     *
     * @endif
     */
    PortableServer::POA_ptr getPOA();
#ifdef ORB_IS_OMNIORB
    PortableServer::POA_ptr getShortCutPOA();
#endif

    /*!
     * @if jp
     * @brief Manager が持つ POAManager を取得する (所有権保持)
     *
     * Manager に設定された POAMAnager を取得する。所有権は保持され
     * たままである。_var型で受ける場合は getORB() を用いること。この関
     * 数は以下のようにORBのオペレーションを直接呼ぶ場合に利用する。
     *
     * Manager::instance().thePOAManager()->activate();
     *
     * @return POAマネージャ
     *
     * @else
     * @brief Get POAManager that Manager has (ownership retained)
     *
     * Get POAMAnager that has been set to Manager. The ownership of
     * POAManager is retained. If the returned value is assigned to
     * _var variable, getPOAManager() operation must be used. This
     * operation is provided for the following use.
     *
     * Manager::instance().thePOAManager()->activate();
     *
     * @return POA manager
     *
     * @endif
     */

    PortableServer::POAManager_ptr thePOAManager();
    /*!
     * @if jp
     * @brief Manager が持つ POAManager を取得する (所有権複製)
     *
     * Manager に設定された POAMAnager を取得する。所有権はコピーさ
     * れ（リファレンスカウントが1増加する。）、受け取った側でも所有権
     * を獲得する。したがって、使用後にはリファレンスを解放する必要があ
     * る。通常は以下のように戻り値を _var 型変数で受け取り、自動的に解
     * 放させる方法が推奨される。
     *
     * {
     *   PortableServer::POAManager_var poam;
     *   poam = = Manager::instance().getPOAManager();
     *   poam->activate();
     * } // POAManager's reference in orb is released here.
     *
     *
     * @return POAマネージャ
     *
     * @else
     * @brief Get POAManager that Manager has (ownership duplicated)
     *
     * Get POAMAnager that has been set to Manager. The
     * ownership is copied (the reference count is incremented.),
     * calling side also obtains reference ownership. The obtained
     * reference must be released after use. Usually it is recommended
     * that return value are assigned to _var type variables are
     * wonership would be released automatically.
     *
     * {
     *   PortableServer::POAManager_var poam;
     *   poam = = Manager::instance().getPOAManager();
     *   poam->activate();
     * } // POAManager's reference in orb is released here.
     *
     * @return POA manager
     *
     * @endif
     */
    PortableServer::POAManager_ptr getPOAManager();

    /*!
     * @if jp
     * @brief ManagerServantを取得する
     *
     * @else
     *
     * @brief Getting ManagerServant
     *
     * @endif
     */
    RTM::ManagerServant& getManagerServant();

    /*!
     * @if jp
     * @brief LocalService の初期化
     *
     * @return Timer 初期化処理実行結果(初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief LocalService initialization
     *
     * @return Timer Initialization result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initLocalService();
    /*!
     * @if jp
     * @brief NamingManagerを取得する
     *
     * @return NamingManager
     *
     * @else
     *
     * @brief Getting NamingManager
     *
     * @return NamingManager
     *
     * @endif
     */
    NamingManager* getNaming();
    
    //============================================================
    // Protected functions
    //============================================================
  protected:
    //============================================================
    // Manager initialize and finalization
    //============================================================
    /*!
     * @if jp
     * @brief Manager の内部初期化処理
     *
     * Manager の内部初期化処理を実行する。
     *  - Manager コンフィギュレーションの設定
     *  - ログ出力ファイルの設定
     *  - 終了処理用スレッドの生成
     *  - タイマ用スレッドの生成(タイマ使用時)
     *
     * @param argc コマンドライン引数の数
     * @param argv コマンドライン引数
     *
     * @else
     * @brief Manager internal initialization
     *
     * Execute Manager's internal initialization processing.
     *  - Set Manager configuration
     *  - Set log output file
     *  - Create termination processing thread
     *  - Create timer thread (when using timer)
     *
     * @param argc Number of commandline arguments
     * @param argv Commandline arguments
     *
     * @endif
     */
    void initManager(int argc, char** argv);

    /*!
     * @if jp
     * @brief Manager の終了処理
     *
     * Manager を終了する
     * (ただし，現在は未実装)
     *
     * @else
     * @brief Shutdown Manager
     *
     * Shutdown Manager
     * (However, not implemented now)
     *
     * @endif
     */
    void shutdownManager();

    /*!
     * @if jp
     * @brief Manager の終了処理
     *
     * configuration の "manager.shutdown_on_nortcs" YES で、
     * コンポーネントが登録されていない場合 Manager を終了する。
     *
     * @else
     * @brief Shutdown Manager
     *
     * This method shutdowns Manager as follows.
     * - "Manager.shutdown_on_nortcs" of configuration is YES.
     * - The component is not registered.
     *
     * @endif
     */
    void shutdownOnNoRtcs();

    //============================================================
    // Logger initialize and terminator
    //============================================================
    void initLogstreamFile();
    void initLogstreamPlugins();
    void initLogstreamOthers();
    /*!
     * @if jp
     * @brief System logger の初期化
     *
     * System logger の初期化を実行する。
     * コンフィギュレーションファイルに設定された情報に基づき、
     * ロガーの初期化，設定を実行する。
     *
     * @return 初期化実行結果(初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief System logger initialization
     *
     * Initialize System logger.
     * Initialize logger and set it according to the set information in
     * configuration file,
     *
     * @return Initialization result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initLogger();

    /*!
     * @if jp
     * @brief System Logger の終了処理
     *
     * System Loggerの終了処理を実行する。
     * バッファ内に保持されているログ情報が存在する場合には、
     * 強制的にログファイルに出力し、ログファイルを閉じる。
     *
     * @else
     * @brief System Logger finalization
     *
     * Finalize System Logger.
     * If log information stored in the buffer exists, output information
     * to the log file forcibly and close it.
     *
     * @endif
     */
    void shutdownLogger();

    //============================================================
    // ORB initialization and finalization
    //============================================================
    /*!
     * @if jp
     * @brief CORBA ORB の初期化処理
     *
     * 引数により与えられた設定を元にORBを初期化する。
     *
     * @return ORB 初期化処理結果(初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief CORBA ORB initialization
     *
     * Initialize ORB based on the configuration given by arguments.
     *
     * @return ORB initialization result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initORB();

    /*!
     * @if jp
     * @brief ORB のコマンドラインオプション作成
     *
     * コンフィギュレーション情報に設定された内容から
     * ORB の起動時オプションを作成する。
     *
     * @return ORB 起動時オプション
     *
     * @else
     * @brief Create ORB command options
     *
     * Create ORB launch options from configuration information
     * that has been set.
     *
     * @return ORB launch options
     *
     * @endif
     */
    std::string createORBOptions();

    /*!
     * @if jp
     * @brief エンドポイントの生成
     *
     * コンフィグレーションからエンドポイントを生成する。
     *
     * @param endpoints エンドポイントリスト
     *
     * @else
     * @brief Create Endpoints
     *
     * Create Endpoints from the configuration.
     *
     * @param endpoints Endpoints list
     *
     * @endif
     */
    void createORBEndpoints(coil::vstring& endpoints);

    /*!
     * @if jp
     * @brief ORB の Endpoint のコマンドラインオプション作成
     * @param opt コマンドラインオプション
     * @param endpoint エンドポイントリスト
     *
     * @else
     * @brief Create a command optional line of Endpoint of ORB.
     * @param opt ORB options
     * @param endpoint Endpoints list
     *
     * @endif
     */
    void createORBEndpointOption(std::string& opt, coil::vstring& endpoint);

    /*!
     * @if jp
     * @brief ORB の終了処理
     *
     * ORB の終了処理を実行する。
     * 実行待ちの処理が存在する場合には、その処理が終了するまで待つ。
     * 実際の終了処理では、POA Managerを非活性化し、 ORB のシャットダウンを実行
     * する。
     *
     * @else
     * @brief ORB finalization
     *
     * Finalize ORB .
     * When the waiting process exists, wait until it completes.
     * In actual finalization, deactivate POA Manager and then shutdown of ORB.
     *
     * @endif
     */
    void shutdownORB();

    //============================================================
    // NamingService initialization and finalization
    //============================================================
    /*!
     * @if jp
     * @brief NamingManager の初期化
     *
     * NamingManager の初期化処理を実行する。
     * ただし、NamingManager を使用しないようにプロパティ情報に設定されている
     * 場合には何もしない。
     * NamingManager を使用する場合、プロパティ情報に設定されている
     * デフォルト NamingServer を登録する。
     * また、定期的に情報を更新するように設定されている場合には、指定された周期
     * で自動更新を行うためのタイマを起動するとともに、更新用メソッドをタイマに
     * 登録する。
     *
     * @return 初期化処理結果(初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief NamingManager initialization
     *
     * Initialize NamingManager .
     * However, operate nothing, if it is set to property that NamingManager
     * is not used.
     * Register default NamingServer that is set to property information,
     * when NamingManager is used.
     * Also, launch a timer that updates information automatically at specified
     * cycle and register the method for the update to the timer, when it is set
     * to update it reguraly.
     *
     * @return Initialization result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initNaming();

    /*!
     * @if jp
     * @brief NamingManager の終了処理
     *
     * NamingManager を終了する。
     * 登録されている全要素をアンバインドし、終了する。
     *
     * @else
     * @brief NamingManager finalization
     *
     * Finalize NamingManager.
     * Unbind all registered elements and shutdown them.
     *
     * @endif
     */
    void shutdownNaming();

    /*!
     * @if jp
     * @brief NamingManagerを取得する
     *
     * @else
     *
     * @brief Getting NamingManager
     *
     * @endif
     */
     NamingManager& getNamingManager();
    
    //============================================================
    // Component management
    //============================================================
    /*!
     * @if jp
     * @brief NamingManager に登録されている RTコンポーネントの終了処理
     *
     * NamingManager に登録されているRTコンポーネントのリストを取得し、
     * 全コンポーネントを終了する。
     *
     * @else
     * @brief NamingManager finalization
     *
     * Get a list of RT-Components that have been registered to NamingManager,
     * and shutdown all components.
     *
     * @endif
     */
    void shutdownComponents();


    /*!
     * @if jp
     * @brief 引数文字列からコンポーネント型名・プロパティを抽出する
     *
     * 文字列からコンポーネント型とコンポーネントのプロパティを抽出する。
     * 与えられる文字列のフォーマットは RTC の ID とコンフィギュレーショ
     * ンからなる
     *
     * [RTC type]?[key(0)]=[val(0)]&[key(1)]=[val(1)]&...&[key(n)]=[val(n)]
     *
     * である。なお、RTC type は implementation_id のみ、もしくは、下記
     * の RTC ID 形式
     *
     * RTC:[vendor]:[category]:[impl_id]:[version]
     *
     * を受け付ける。戻り値である、comp_id は、
     * "vendor", "category", "implementation_id", "version" のキーを持つ
     * Properties 型のオブジェクトとして返される。
     * comp_conf には "?" 以下に記述されるコンポーネントに与えるプロパティ
     * が Properties 型のオブジェクトとして返される。
     *
     * @return comp_arg にコンポーネント型が含まれていない場合false
     * @param comp_arg  処理すべき文字列
     * @param comp_id 抽出されたコンポーネントの型名
     * @param comp_conf 抽出されたコンポーネントのプロパティ
     *
     * @else
     * @brief Extracting component type/properties from the given string
     *
     * This operation extracts component type name and its properties
     * from the figen character string.
     * The given string formats is the following.
     *
     * [RTC type]?[key(0)]=[val(0)]&[key(1)]=[val(1)]...[key(n)]=[val(n)]
     *
     * Returned value "comp_id" has keys of "vendor", "category",
     * "implementation_id", "version", and returned as Properties type
     * object. "comp_conf" is returned as Properties type object
     * includeing component properties to be given to component.
     *
     * @return comp_arg false will returned if no component type in arg
     * @param comp_arg  character string to be processed
     * @param comp_type extracted component type name
     * @param comp_prop extracted component's properties
     *
     * @endif
     */
    bool procComponentArgs(const char* comp_arg,
                           coil::Properties& comp_id,
                           coil::Properties& comp_conf);
    /*!
     * @if jp
     * @brief 引数文字列からExecutionContext名・プロパティを抽出する
     *
     * 文字列からExecutionContext名とプロパティを抽出する。
     * 与えられる文字列のフォーマットは RTC の ID とコンフィギュレーショ
     * ンからなる
     *
     * [ExecutionContext名]?[key(0)]=[val(0)]&[key(1)]=[val(1)]&...&[key(n)]=[val(n)]
     *
     * である。
     *
     * ec_conf には "?" 以下に記述されるコンポーネントに与えるプロパティ
     * が Properties 型のオブジェクトとして返される。
     *
     * @return ec_args にExecutionContext名が含まれていない場合false
     * @param ec_args  処理すべき文字列
     * @param ec_id 抽出されたExecutionContext名
     * @param ec_conf 抽出されたExecutionContextのプロパティ
     *
     * @else
     * @brief Extracting ExecutionContext's name/properties from the given
     *        string
     *
     * This operation extracts ExecutionContext's name and its properties
     * from the figen character string.
     * The given string formats is the following.
     *
     * [ExecutionContext's name]?[key(0)]=[val(0)]&[key(1)]=[val(1)]...[key(n)]=[val(n)]
     *
     * "ec_conf" is returned as Properties type object
     * includeing component properties to be given to component.
     *
     * @return ec_arg false will returned if no ExecutionContext's name in arg
     * @param ec_arg  character string to be processed
     * @param ec_type extracted ExecutionContext's name
     * @param ec_prop extracted ExecutionContext's properties
     *
     * @endif
     */
    bool procContextArgs(const char* ec_args,
                         std::string& ec_id,
                         coil::Properties& ec_conf);

    /*!
     * @if jp
     * @brief RTコンポーネントのコンフィギュレーション処理
     *
     * RTコンポーネントの型およびインスタンス毎に記載されたプロパティファイルの
     * 情報を読み込み、コンポーネントに設定する。
     * また、各コンポーネントの NamingService 登録時の名称を取得し、設定する。
     *
     * @param comp コンフィギュレーション対象RTコンポーネント
     *
     * @else
     * @brief Configure RT-Component
     *
     * Read property files described each RT-Component's type and instance,
     * and configure it to the component.
     * Also, get each component's registered name when registering to
     * NamingService and configure it.
     *
     * @param comp Target RT-Component for the configuration
     *
     * @endif
     */
    void configureComponent(RTObject_impl* comp, const coil::Properties& prop);

    /*!
     * @if jp
     * @brief ExecutionContextManager の初期化
     *
     * 使用する各 ExecutionContext の初期化処理を実行し、各 ExecutionContext
     * 生成用 Factory を ExecutionContextManager に登録する。
     *
     * @return ExecutionContextManager 初期化処理実行結果
     *         (初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief ExecutionContextManager initialization
     *
     * Initialize each ExecutionContext that is used, and register each
     * ExecutionContext creation Factory to ExecutionContextManager.
     *
     * @return ExecutionContextManager initialization result
     *          (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initExecContext();

    /*!
     * @if jp
     * @brief PeriodicECSharedComposite の初期化
     *
     * @return PeriodicECSharedComposite 初期化処理実行結果
     *         (初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief PeriodicECSharedComposite initialization
     *
     * @return PeriodicECSharedComposite initialization result
     *          (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initComposite();

    /*!
     * @if jp
     * @brief ファクトリの初期化
     *
     * バッファ、スレッド、パブリッシャ、プロバイダ、コンシューマの
     * ファクトリを初期化する。
     *
     * @return ファクトリ初期化処理実行結果
     *         (初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief Factories initialization
     *
     * Initialize buffer factories, thread factories, publisher factories,
     * provider factories, and consumer factories.
     *
     * @return PeriodicECSharedComposite initialization result
     *          (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initFactories();

    void initCpuAffinity();
	/*!
	 * @if jp
	 * @brief 起動時にrtc.confで指定したポートを接続する
	 *
	 * 例:
	 * manager.components.preconnect: RTC0.port0?port=RTC0.port1&interface_type=corba_cdr&dataflow_type=pull&~,~
	 *
	 *
	 * @else
	 * @brief 
	 *
	 *
	 * @endif
	 */
	void initPreConnection();
	/*!
	 * @if jp
	 * @brief 起動時にrtc.confで指定したRTCをアクティベーションする
	 *
	 * 例:
	 * manager.components.preactivation: RTC1,RTC2~
	 *
	 *
	 * @else
	 * @brief
	 *
	 *
	 * @endif
	 */
	void initPreActivation();
	/*!
	 * @if jp
	 * @brief 起動時にrtc.confで指定したRTCを生成する
	 *
	 * 例:
	 * manager.components.precreate RTC1,RTC2~
	 *
	 *
	 * @else
	 * @brief
	 *
	 *
	 * @endif
	 */
	void initPreCreation();
	/*!
	* @if jp
	* @brief 
	*
	*
	*
	* @else
	* @brief
	*
	*
	* @endif
	*/
	void invokeInitProc();
	/*!
	* @if jp
	* @brief
	* @param comp
	*
	*
	*
	* @else
	* @brief
	* @param comp
	*
	*
	* @endif
	*/
	void publishPorts(RTObject_impl* comp);
	/*!
	* @if jp
	* @brief
	* @param comp
	*
	*
	*
	* @else
	* @brief
	* @param comp
	*
	*
	* @endif
	*/
	void subscribePorts(RTObject_impl* comp);
	/*!
	* @if jp
	* @brief
	* @param comp
	*
	*
	*
	* @else
	* @brief
	* @param comp
	*
	*
	* @endif
	*/
	PortServiceList_var getPortsOnNameServers(const std::string& nsname, const std::string& kind);
	/*!
	* @if jp
	* @brief
	* @param port
	* @param target_ports
	*
	*
	* @else
	* @brief
	* @param port
	* @param target_ports
	*
	*
	* @endif
	*/
	void connectDataPorts(PortService_ptr port, PortServiceList_var& target_ports);
	/*!
	* @if jp
	* @brief
	* @param port
	* @param target_ports
	*
	*
	* @else
	* @brief
	* @param port
	* @param target_ports
	*
	*
	* @endif
	*/
	void connectServicePorts(PortService_ptr port, PortServiceList_var& target_ports);
    
    /*!
     * @if jp
     * @brief Timer の初期化
     *
     * 使用する各 Timer の初期化処理を実行する。
     * (現状の実装では何もしない)
     *
     * @return Timer 初期化処理実行結果(初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief Timer initialization
     *
     * Initialize each Timer that is used.
     * (In current implementation, nothing is done.)
     *
     * @return Timer Initialization result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initTimer();

    /*!
     * @if jp
     * @brief ManagerServant の初期化
     *
     * @return Timer 初期化処理実行結果(初期化成功:true、初期化失敗:false)
     *
     * @else
     * @brief ManagerServant initialization
     *
     * @return Timer Initialization result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool initManagerServant();



    /*!
     * @if jp
     * @brief ManagerServant へのポインタ
     * @else
     * @brief The pointer to the ManagerServant
     * @endif
     */
    RTM::ManagerServant* m_mgrservant;

    /*!
     * @if jp
     * @brief プロパティ情報のマージ
     *
     * 指定されたファイル内に設定されているプロパティ情報をロードし、
     * 既存の設定済みプロパティとマージする。
     *
     * @param prop マージ対象プロパティ
     * @param file_name プロパティ情報が記述されているファイル名
     *
     * @return マージ処理実行結果(マージ成功:true、マージ失敗:false)
     *
     * @else
     * @brief Merge property information
     *
     * Load property information that is configured in the specified file,
     * and merge existing properties that has been configured.
     *
     * @param prop Target properties for the merge
     * @param file_name File name that property information is described
     *
     * @return Merge result (Successful:true, Failed:false)
     *
     * @endif
     */
    bool mergeProperty(coil::Properties& prop, const char* file_name);

    /*!
     * @if jp
     * @brief NamingServer に登録する際の登録情報を組み立てる
     *
     * 指定された書式とプロパティ情報を基に NameServer に登録する際の情報を
     * 組み立てる。
     * 各書式指定用文字の意味は以下のとおり
     * - % : コンテキストの区切り
     * - n : インスタンス名称
     * - t : 型名
     * - m : 型名
     * - v : バージョン
     * - V : ベンダー
     * - c : カテゴリ
     * - h : ホスト名
     * - M : マネージャ名
     * - p : プロセスID
     *
     * @param naming_format NamingService 登録情報書式指定
     * @param prop 使用するプロパティ情報
     *
     * @return 指定書式変換結果
     *
     * @else
     * @brief Construct registration information when registering to
     *        Naming server
     *
     * Construct information when registering to NameServer based on specified
     * format and property information.
     * Each format specification character means as follows:
     * - % : Break of Context
     * - n : Instance's name
     * - t : Type name
     * - m : Type name
     * - v : Version
     * - V : Vender
     * - c : Category
     * - h : Host name
     * - M : Manager name
     * - p : Process ID
     *
     * @param naming_format Format specification for NamingService registration
     * @param prop Property information that is used
     *
     * @return Specification format conversion result
     *
     * @endif
     */
    std::string formatString(const char* naming_format,
                             coil::Properties& prop);

    /*!
     * @if jp
     * @brief corba.endpoints にエンドポイント情報を設定する
     * @else
     * @brief Setting endpoint info from corba.endpoints
     * @endif
     */
    void setEndpointProperty(CORBA::Object_ptr objref);

    /*!
     * @if jp
     * @brief corba.endpoint_property からオプション情報を取得する
     * @else
     * @brief Getting option info from corba.endpoint_property
     * @endif
     */
    void endpointPropertySwitch(const std::string& ipver,
                                bool& ip, std::vector<int>& ip_list);
    
    //============================================================
    // protected 変数
    //============================================================

    //------------------------------------------------------------
    // static var
    //------------------------------------------------------------
    /*!
     * @if jp
     * @brief 唯一の Manager へのポインタ
     * @else
     * @brief The pointer to the Manager
     * @endif
     */
    static Manager* manager;

    /*!
     * @if jp
     * @brief 唯一の Manager へのポインタに対する mutex
     * @else
     * @brief The mutex of the pointer to the Manager
     * @endif
     */
    static Mutex mutex;

    //------------------------------------------------------------
    // CORBA var
    //------------------------------------------------------------
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
#ifdef ORB_IS_OMNIORB
    PortableServer::POA_var m_pShortCutPOA;
#endif
    
    /*!
     * @if jp
     * @brief POAManager へのポインタ
     * @else
     * @brief The pointer to the POAManager
     * @endif
     */
    PortableServer::POAManager_var m_pPOAManager;

    //------------------------------------------------------------
    // Manager's variable
    //------------------------------------------------------------
    /*!
     * @if jp
     * @brief ユーザ初期化関数へのポインタ
     * @else
     * @brief User's initialization function's pointer
     * @endif
     */
    ModuleInitProc m_initProc;

    /*!
     * @if jp
     * @brief Manager の configuration を格納する Properties
     * @else
     * @brief Managaer's configuration Properties
     * @endif
     */
    coil::Properties m_config;

    /*!
     * @if jp
     * @brief ModuleManager へのポインタ
     * @else
     * @brief The pointer to the ModuleManager
     * @endif
     */
    ModuleManager* m_module;

    /*!
     * @if jp
     * @brief NamingManager へのポインタ
     * @else
     * @brief The pointer to the NamingManager
     * @endif
     */
    NamingManager* m_namingManager;

    /*!
     * @if jp
     * @brief Timer Object
     * @else
     * @brief Timer Object
     * @endif
     */
    coil::Timer* m_timer;

    //------------------------------------------------------------
    // Logger
    //------------------------------------------------------------
    /*!
     * @if jp
     * @brief ロガーバッファ
     * @else
     * @brief Logger buffer
     * @endif
     */
    LogStreamBuf m_logStreamBuf;

    /*!
     * @if jp
     * @brief ロガーストリーム
     * @else
     * @brief Logger stream
     * @endif
     */
    Logger rtclog;

    /*!
     * @if jp
     * @brief ログ出力ファイル
     * @else
     * @brief Files for log output
     * @endif
     */
    std::vector<std::filebuf*> m_logfiles;

    //============================================================
    // コンポーネントマネージャ
    //============================================================
    // ObjectManager へ渡す述語クラス
    struct InstanceName
    {
      explicit InstanceName(RTObject_impl* comp);
      explicit InstanceName(const char* name);
      explicit InstanceName(const std::string& name);
      bool operator()(RTObject_impl* comp);
      std::string m_name;
    };

    typedef ObjectManager<std::string, RTObject_impl,
                          InstanceName> ComponentManager;

    /*!
     * @if jp
     * @brief コンポーネントマネージャ
     * @else
     * @brief ComponentManager
     * @endif
     */
    ComponentManager m_compManager;

    //============================================================
    // コンポーネントファクトリ
    //============================================================
    // コンポーネントファクトリへ渡す述語クラス
    class FactoryPredicate
    {
    public:
      explicit FactoryPredicate(const char* imple_id)
        : m_vendor(""), m_category(""), m_impleid(imple_id), m_version("")
      {
      }
      explicit FactoryPredicate(const coil::Properties& prop)
        : m_vendor(prop["vendor"]),
          m_category(prop["category"]),
          m_impleid(prop["implementation_id"]),
          m_version(prop["version"])
      {
      }
      explicit FactoryPredicate(FactoryBase* factory)
        : m_vendor(factory->profile()["vendor"]),
          m_category(factory->profile()["category"]),
          m_impleid(factory->profile()["implementation_id"]),
          m_version(factory->profile()["version"])
      {
      }
      bool operator()(FactoryBase* factory)
      {
        // implementation_id must not be empty
        if (m_impleid.empty()) return false;

        const coil::Properties& prop(factory->profile());

        if (m_impleid != prop["implementation_id"])
          return false;
        if (!m_vendor.empty()   && m_vendor != prop["vendor"])
          return false;
        if (!m_category.empty() && m_category != prop["category"])
          return false;
        if (!m_version.empty()  && m_version != prop["version"])
          return false;

        return true;
      }
    private:
      std::string m_vendor;
      std::string m_category;
      std::string m_impleid;
      std::string m_version;
    };

    class ModulePredicate
    {
      coil::Properties& m_prop;
    public:
      explicit ModulePredicate(coil::Properties& prop)
      : m_prop(prop)
      {
      }
      bool operator()(coil::Properties& prop)
      {
        if (m_prop["implementation_id"] != prop["implementation_id"])
          {
            return false;
          }
        if (!m_prop["vendor"].empty() &&
            m_prop["vendor"] != prop["vendor"])     { return false; }
        if (!m_prop["category"].empty() &&
            m_prop["category"] != prop["category"]) { return false; }
        if (!m_prop["version"].empty() &&
            m_prop["version"] != prop["version"])   { return false; }
        return true;
      }
    };

    /*!
     * @if jp
     * @brief コンポーネントファクトリ
     * @else
     * @brief ComponentFactory
     * @endif
     */
    typedef ObjectManager<const coil::Properties, FactoryBase,
                          FactoryPredicate> FactoryManager;

    /*!
     * @if jp
     * @brief ComponentManager
     * @else
     * @brief ComponentManager
     * @endif
     */
    FactoryManager m_factory;

    //============================================================
    // ExecutionContextファクトリ
    //============================================================
    // ECファクトリへ渡す述語クラス
    struct ECFactoryPredicate
    {
      explicit ECFactoryPredicate(const char* name) : m_name(name) {}
      explicit ECFactoryPredicate(ECFactoryBase* factory)
        : m_name(factory->name()) {}
      bool operator()(ECFactoryBase* factory)
      {
        return m_name == factory->name();
      }
      std::string m_name;
    };
    typedef ObjectManager<const char*,
                          ECFactoryBase,
                          ECFactoryPredicate> ECFactoryManager;

    /*!
     * @if jp
     * @brief ExecutionContext マネージャ
     * @else
     * @brief ExecutionContext Manager
     * @endif
     */
    ECFactoryManager m_ecfactory;

    /*!
     * @if jp
     * @brief ExecutionContext リスト
     * @else
     * @brief ExecutionContext list
     * @endif
     */
    std::vector<ExecutionContextBase*> m_ecs;

    // ファクトリ名をリストアップするためのファンクタ
    struct ModuleFactories
    {
      void operator()(FactoryBase* f)
      {
        modlist.push_back(f->profile().getProperty("implementation_id"));
      }
      std::vector<std::string> modlist;
    };

    //------------------------------------------------------------
    // ORB runner
    //------------------------------------------------------------
    /*!
     * @if jp
     * @class OrbRunner
     * @brief OrbRunner クラス
     *
     * ORB 実行用ヘルパークラス。
     *
     * @since 0.4.0
     *
     * @else
     * @class OrbRunner
     * @brief OrbRunner class
     *
     * ORB exrcution helper class
     *
     * @since 0.4.0
     *
     * @endif
     */
    class OrbRunner
      : public coil::Task
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
      explicit OrbRunner(CORBA::ORB_ptr orb) : m_pORB(orb)
      {
        open(nullptr);
      };

      /*!
       * @if jp
       * @brief ORB 活性化処理
       *
       * ORB 活性化処理
       *
       * @param args 活性化時引数
       *
       * @return 活性化結果
       *
       * @else
       * @brief ORB activation processing
       *
       * ORB activation processing.
       *
       * @param args ORB activation processing
       *
       * @return Activation result
       *
       * @endif
       */
      virtual int open(void *args)
      {
        activate();
        return 0;
      }

      /*!
       * @if jp
       * @brief ORB 開始処理
       *
       * ORB 開始処理
       *
       * @return 開始処理結果
       *
       * @else
       * @brief ORB start processing
       *
       * ORB start processing
       *
       * @return Starting result
       *
       * @endif
       */
      virtual int svc(void)
      {
        m_pORB->run();
//        Manager::instance().shutdown();
        return 0;
      }

      /*!
       * @if jp
       * @brief ORB 終了処理
       *
       * ORB 終了処理
       *
       * @param flags 終了処理フラグ
       *
       * @return 終了処理結果
       *
       * @else
       * @brief ORB close processing
       *
       * ORB close processing.
       *
       * @param flags Flag of close processing
       *
       * @return Close result
       *
       * @endif
       */
      virtual int close(unsigned long flags)
      {
        return 0;
      }
    private:
      CORBA::ORB_ptr m_pORB;
    };
    /*!
     * @if jp
     * @brief ORB ヘルパークラスへのポインタ
     * @else
     * @brief The pointer to ORB helper class
     * @endif
     */
    OrbRunner* m_runner;

    //------------------------------------------------------------
    // Manager Terminator
    //------------------------------------------------------------
    /*!
     * @if jp
     * @class Terminator
     * @brief Terminator クラス
     *
     * ORB 終了用ヘルパークラス。
     *
     * @since 0.4.0
     *
     * @else
     * @class Terminator
     * @brief Terminator class
     *
     * ORB termination helper class.
     *
     * @since 0.4.0
     *
     * @endif
     */
    class Terminator
      : public coil::Task
    {
    public:
      /*!
       * @if jp
       * @brief コンストラクタ
       *
       * コンストラクタ
       *
       * @param manager マネージャ・オブジェクト
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
      explicit Terminator(Manager* manager, double waittime = 0)
                : m_manager(manager), m_waittime(waittime) {};

      /*!
       * @if jp
       * @brief 終了処理
       *
       * ORB，マネージャ終了処理を開始する。
       *
       * @else
       * @brief Termination processing
       *
       * Start ORB and manager's termination processing.
       *
       * @endif
       */
      void terminate()
      {
        open(nullptr);
      }

      /*!
       * @if jp
       * @brief 終了処理活性化処理
       *
       * 終了処理活性化処理
       *
       * @param args 活性化時引数
       *
       * @return 活性化結果
       *
       * @else
       * @brief Termination processing activation
       *
       * Termination processing activation.
       *
       * @param args Activation argument
       *
       * @return Activation result
       *
       * @endif
       */
      virtual int open(void *args)
      {
        activate();
        return 0;
      }

      /*!
       * @if jp
       * @brief ORB，マネージャ終了処理
       *
       * ORB，マネージャ終了処理
       *
       * @return 終了処理結果
       *
       * @else
       * @brief ORB and manager's termination processing
       *
       * ORB and manager's termination processing.
       *
       * @return Termination result
       *
       * @endif
       */
      virtual int svc(void)
      {
        coil::sleep(m_waittime);
        Manager::instance().shutdown();
        return 0;
      }
      Manager* m_manager;
      coil::TimeValue m_waittime;
    };

    /*!
     * @if jp
     * @brief ORB 終了用ヘルパークラスへのポインタ
     * @else
     * @brief The pointer to ORB termination helper class.
     * @endif
     */
    Terminator* m_terminator;

    struct Term
    {
      int waiting;
      Mutex mutex;
    };
    /*!
     * @if jp
     * @brief マネージャ終了処理用同期フラグ
     *
     * マネージャ終了の待ち合せ処理で同期を取るためのフラグ。
     *
     * @else
     * @brief Synchronous flag for manager termination
     *
     * Flag used to take synchronization by join().
     *
     * @endif
     */
    Term m_terminate;

    struct Finalized
    {
      Mutex mutex;
      std::vector<RTObject_impl*> comps;
    };
    Finalized m_finalized;

    ::RTM::ManagerActionListeners m_listeners;
  };  // class Manager
};  // namespace RTC

#endif  // RTC_MANAGER_H
