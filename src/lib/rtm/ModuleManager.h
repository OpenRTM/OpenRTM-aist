// -*- C++ -*-
/*!
 * @file ModuleManager.h
 * @brief Loadable modules manager class
 * @date $Date: 2007-12-31 03:08:04 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2017
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

#ifndef RTC_MODULEMANAGER_H
#define RTC_MODULEMANAGER_H


// ACE includes
#include <coil/DynamicLib.h>

// RTC includes
#include <rtm/Manager.h>
#include <coil/Properties.h>
#include <rtm/ObjectManager.h>

// STL includes
#include <string>
#include <utility>
#include <vector>
#include <map>

#define CONFIG_EXT    "manager.modules.config_ext"
#define CONFIG_PATH   "manager.modules.config_path"
#define DETECT_MOD    "manager.modules.detect_loadable"
#define MOD_LOADPTH   "manager.modules.load_path"
#define INITFUNC_SFX  "manager.modules.init_func_suffix"
#define INITFUNC_PFX  "manager.modules.init_func_prefix"
#define ALLOW_ABSPATH "manager.modules.abs_path_allowed"
#define ALLOW_URL     "manager.modules.download_allowed"
#define MOD_DWNDIR    "manager.modules.download_dir"
#define MOD_DELMOD    "manager.modules.download_cleanup"
#define MOD_PRELOAD   "manager.modules.preload"

namespace RTC
{
  /*!
   * @if jp
   * @class ModuleManager
   * @brief モジュールマネージャクラス
   *
   * モジュールのロード、アンロードなどを管理するクラス
   *
   * @since 0.4.0
   *
   * @else
   * @class ModuleManager
   * @brief ModuleManager class
   *
   * This is a class to manage for loading and unloading modules.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class ModuleManager
  {
    using vProperties = std::vector<coil::Properties>;
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     * 設定された Property オブジェクト内の情報を基に初期化を実行する。
     *
     * @param prop 初期化用プロパティ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor.
     * Initialize based on information in the set Property object.
     *
     * @param prop Properties for initialization
     *
     * @endif
     */
    explicit ModuleManager(coil::Properties& prop);

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * @else
     *
     * @brief Destructor
     *
     * @endif
     */
    ~ModuleManager();

    /*!
     * @if jp
     * @brief ファイル・オープン失敗例外処理用構造体
     * @else
     * @brief Structure for exception handling when file open is failed
     * @endif
     */
    struct Error
    {
      explicit Error(std::string  _reason)
        : reason(std::move(_reason)) {}
      std::string reason;
    };

    /*!
     * @if jp
     * @brief 未実装部，指定モジュール不明例外処理用構造体
     * @else
     * @brief Structure for exception handling of unimplemented part
     *        and specified module missing
     * @endif
     */
    struct NotFound
    {
      explicit NotFound(std::string  _name)
        : name(std::move(_name)) {}
      std::string name;
    };

    /*!
     * @if jp
     * @brief 指定ファイル不明例外処理用構造体
     * @else
     * @brief Structure for exception handling when specified file
     *        cannot be found
     * @endif
     */
    struct FileNotFound
      : public NotFound
    {
      explicit FileNotFound(const std::string& _name)
        : NotFound(_name) {}
    };

    /*!
     * @if jp
     * @brief 指定モジュール不明例外処理用構造体
     * @else
     * @brief Structure for exception handling when specified module
     *        cannot be found
     * @endif
     */
    struct ModuleNotFound
      : public NotFound
    {
      explicit ModuleNotFound(const std::string& _name)
        : NotFound(_name) {}
    };

    /*!
     * @if jp
     * @brief 指定シンボル不明例外処理用構造体
     * @else
     * @brief Structure for exception handling when specified symbol
     *        cannot be found
     * @endif
     */
    struct SymbolNotFound
      : public NotFound
    {
      explicit SymbolNotFound(const std::string& _name)
        : NotFound(_name) {}
    };

    /*!
     * @if jp
     * @brief 指定操作禁止時例外処理用構造体
     * @else
     * @brief Structure for exception handling when specified
     *        operation cannot be allowed.
     * @endif
     */
    struct NotAllowedOperation
      : public Error
    {
      explicit NotAllowedOperation(const std::string& _reason)
        : Error(_reason) {}
    };

    /*!
     * @if jp
     * @brief 指定引数不正時例外処理用構造体
     * @else
     * @brief Structure for exception handling when specified
     *        argument is invalid.
     * @endif
     */
    struct InvalidArguments
      : public Error
    {
      explicit InvalidArguments(const std::string& _reason)
        : Error(_reason) {}
    };

    /*!
     * @if jp
     * @brief 指定操作不正時例外処理用構造体
     * @else
     * @brief Structure for exception handling when specified
     *        operation is invalid.
     * @endif
     */
    struct InvalidOperation
      : public Error
    {
      explicit InvalidOperation(const std::string& _reason)
        : Error(_reason) {}
    };
    using ModuleInitFunc = void (*)(Manager*);

    /*!
     * @if jp
     *
     * @brief モジュールのロード
     *
     * file_name をDLL もしくは共有ライブラリとしてロードする。
     * file_name は既定のロードパス (manager.modules.load_path) に対する
     * 相対パスで指定する。
     *
     * Property manager.modules.abs_path_allowed が yes の場合、
     * ロードするモジュールを絶対パスで指定することができる。<br>
     * Property manager.modules.download_allowed が yes の場合、
     * ロードするモジュールをURLで指定することができる。
     *
     * file_name は絶対パスで指定することができる。
     * manager.modules.abs_path_allowd が no の場合、
     * 既定のモジュールロードパスから、file_name のモジュールを探しロードする。
     *
     * @param file_name ロード対象モジュール名
     *
     * @return 指定したロード対象モジュール名
     *
     * @else
     *
     * @brief Load the module
     *
     * Load file_name as DLL or a shared liblary.
     * The file_name is specified by the relative path to default load
     * path (manager.modules.load_path).
     *
     * If Property manager.modules.abs_path_allowed is yes,
     * the load module can be specified by the absolute path.<br>
     * If Property manager.modules.download_allowed is yes,
     * the load module can be specified with URL.
     *
     * The file_name can be specified by the absolute path.
     * If manager.modules.abs_path_allowed is no, module of file_name
     * will be searched from the default module load path and loaded.
     *
     * @param file_name The target module name for the loading
     *
     * @return Name of module for the specified load
     *
     * @endif
     */
    std::string load(const std::string& file_name);
    /*!
     * @if jp
     *
     * @brief モジュールのロード
     *
     * file_name をDLL もしくは共有ライブラリとしてロードする。
     * file_name は既定のロードパス (manager.modules.load_path) に対する
     * 相対パスで指定する。
     * ただし、module_file_pathが設定されている場合には module_file_path の
     * パスのライブラリをロードする。
     *
     * Property manager.modules.abs_path_allowed が yes の場合、
     * ロードするモジュールを絶対パスで指定することができる。<br>
     * Property manager.modules.download_allowed が yes の場合、
     * ロードするモジュールをURLで指定することができる。
     *
     * module_file_name は絶対パスで指定することができる。
     * manager.modules.abs_path_allowd が no の場合、
     * 既定のモジュールロードパスから、file_name のモジュールを探しロードする。
     *
     * @param prop   module_file_name: モジュールファイル名
     *               module_file_path: モジュールいファイルのパス
     *               language: プログラミング言語
     *
     * @return 指定したロード対象モジュール名
     *
     * @else
     *
     * @brief Load the module
     *
     * Load file_name as DLL or a shared liblary.
     * The file_name is specified by the relative path to default load
     * path (manager.modules.load_path).
     *
     * If Property manager.modules.abs_path_allowed is yes,
     * the load module can be specified by the absolute path.<br>
     * If Property manager.modules.download_allowed is yes,
     * the load module can be specified with URL.
     *
     * The module_file_name can be specified by the absolute path.
     * If manager.modules.abs_path_allowed is no, module of file_name
     * will be searched from the default module load path and loaded.
     *
     * @param prop   module_file_name: module file name
     *               module_file_path: module file path
     *               language: programming language
     *
     * @return Name of module for the specified load
     *
     * @endif
     */
    std::string load(coil::Properties& prop);


    /*!
     * @if jp
     *
     * @brief モジュールのロード、初期化
     *
     * 指定したファイルをDLL もしくは共有ライブラリとしてロードするとともに、
     * 指定した初期化用オペレーションを実行する。
     *
     * @param file_name ロード対象モジュール名
     * @param init_func 初期化処理用オペレーション
     *
     * @return 指定したロード対象モジュール名
     *
     * @else
     *
     * @brief Load and intialize the module
     *
     * Load the specified file as DLL or a shared library, and execute operation
     * for specified initialization.
     *
     * @param file_name The target module name for the loading
     * @param init_func Operation for initialization
     *
     * @return Name of module for the specified load
     *
     * @endif
     */
    std::string load(const std::string& file_name,
                     const std::string& init_func);
    /*!
     * @if jp
     *
     * @brief モジュールのロード、初期化
     *
     * 指定したファイルをDLL もしくは共有ライブラリとしてロードするとともに、
     * 指定した初期化用オペレーションを実行する。
     *
     * @param prop   module_file_name: モジュールファイル名
     *               module_file_path: モジュールいファイルのパス
     *               language: プログラミング言語
     * @param init_func 初期化処理用オペレーション
     *
     * @return 指定したロード対象モジュール名
     *
     * @else
     *
     * @brief Load and intialize the module
     *
     * Load the specified file as DLL or a shared library, and execute operation
     * for specified initialization.
     *
     * @param prop   module_file_name: module file name
     *               module_file_path: module file path
     *               language: programming language
     * @param init_func Operation for initialization
     *
     * @return Name of module for the specified load
     *
     * @endif
     */
    std::string load(coil::Properties& prop,
        const std::string& init_func);


    /*!
     * @if jp
     * @brief モジュールのアンロード
     *
     * 指定したロード済みモジュールをクローズし、アンロードする。
     *
     * @param file_name アンロード対象モジュール名
     *
     * @else
     * @brief Unload the module
     *
     * Close and unload the specified module that has been loaded.
     *
     * @param file_name Name of module for the unloading
     *
     * @endif
     */
    void unload(const std::string& file_name);

    /*!
     * @if jp
     * @brief 全モジュールのアンロード
     *
     * 全てのロード済みモジュールをアンロードする。
     *
     * @else
     * @brief Unload all modules
     *
     * Unload all modules that have been loaded.
     *
     * @endif
     */
    void unloadAll();

    /*!
     * @if jp
     * @brief モジュールのシンボルの参照
     * @else
     * @brief Refer to the symbol of the module
     * @endif
     */
    void* symbol(const std::string& file_name, const std::string& func_name);

    /*!
     * @if jp
     * @brief モジュールロードパスを指定する
     *
     * モジュールロード時に対象モジュールを検索するパスを指定する。
     *
     * @param load_path モジュール検索対象パスリスト
     *
     * @else
     * @brief Set the module load path
     *
     * Specify searching path to find the target module when loading module.
     *
     * @param load_path List of module search path
     *
     * @endif
     */
    void setLoadpath(const std::vector<std::string>& load_path);

    /*!
     * @if jp
     * @brief モジュールロードパスを取得する
     *
     * 設定されているモジュールを検索対象パスリストを取得する。
     *
     * @return load_path モジュール検索対象パスリスト
     *
     * @else
     * @brief Get the module load path
     *
     * Get the search path of the set module.
     *
     * @return load_path List of module search path
     *
     * @endif
     */
    inline std::vector<std::string> getLoadPath()
    {
      return m_loadPath;
    }

    /*!
     * @if jp
     * @brief モジュールロードパスを追加する
     *
     * 指定されたパスリストを検索対象パスリストに追加する。
     *
     * @return load_path 追加モジュール検索対象パスリスト
     *
     * @else
     * @brief Add the module load path
     *
     * Add specified path list to search path list.
     *
     * @return load_path List of additional module search path
     *
     * @endif
     */
    void addLoadpath(const std::vector<std::string>& load_path);

    /*!
     * @if jp
     * @brief ロード済みのモジュールリストを取得する
     *
     * 既にロード済みのモジュールリストを取得する。
     *
     * @return ロード済みモジュールリスト
     *
     * @else
     * @brief Get the module list that has been loaded
     *
     * Get the module list that has been loaded.
     *
     * @return List of module that has been loaded
     *
     * @endif
     */
    std::vector<coil::Properties> getLoadedModules();

    /*!
     * @if jp
     * @brief ロード可能モジュールリストを取得する
     *
     * ロード可能なモジュールのリストを取得する。
     * (未実装)
     *
     * @return ロード可能モジュールリスト
     *
     * @else
     * @brief Get the loadable module list
     *
     * Get the loadable module list (not implemented).
     *
     * @return Loadable module list
     *
     * @endif
     */
    std::vector<coil::Properties> getLoadableModules();

    /*!
     * @if jp
     * @brief モジュールの絶対パス指定許可
     *
     * ロード対象モジュールの絶対パス指定を許可するように設定する。
     *
     * @else
     * @brief Allow absolute path when specify module path
     *
     * Set to allow the absolute path when specify the module for the load.
     *
     * @endif
     */
    inline void allowAbsolutePath()
    {
      m_absoluteAllowed = true;
    }

    /*!
     * @if jp
     * @brief モジュールの絶対パス指定禁止
     *
     * ロード対象モジュールの絶対パス指定を禁止するように設定する。
     *
     * @else
     * @brief Disallow absolute path when specify module path
     *
     * Set to disallow the absolute path when specify the module for the load.
     *
     * @endif
     */
    inline void disallowAbsolutePath()
    {
      m_absoluteAllowed = false;
    }

    /*!
     * @if jp
     * @brief モジュールのURL指定許可
     *
     * ロード対象モジュールのURL指定を許可する。
     * 本設定が許可されている場合、モジュールをダウンロードしてロードすることが
     * 許可される。
     *
     * @else
     * @brief Allow URL when specify module path
     *
     * Allow URL when specify module for the load.
     * When this setup is allowed, downloading and loading the module will
     * be allowed.
     *
     * @endif
     */
    inline void allowModuleDownload()
    {
      m_downloadAllowed = true;
    }

    /*!
     * @if jp
     * @brief モジュールのURL指定禁止
     *
     * ロード対象モジュールのURL指定を禁止する。
     *
     * @else
     * @brief Disallow URL when specify module path
     *
     * Disallow URL when specify module for the load.
     *
     * @endif
     */
    inline void disallowModuleDownload()
    {
      m_downloadAllowed = false;
    }

    /*!
     * @if jp
     * @brief LoadPath からのファイルの検索
     *
     * 指定されたパス内に、指定されたファイルが存在するか確認する。
     *
     * @param fname 検索対象ファイル名
     * @param load_path 検索先パスリスト
     *
     * @return 検索されたファイル名
     *
     * @else
     * @brief Search the file from the LoadPath
     *
     * Check whether the specified file exists in the specified path.
     *
     * @param fname Target file name of the search
     * @param load_path Path list for the search
     *
     * @return File name that was found
     *
     * @endif
     */
    std::string findFile(const std::string& fname,
                         const std::vector<std::string>& load_path);

    /*!
     * @if jp
     * @brief ファイルが存在するかどうかのチェック
     *
     * 指定されたファイルが存在するか確認する。
     *
     * @param filename 存在確認対象ファイル名
     *
     * @return ファイル存在確認結果(ファイルあり:true，なし:false)
     *
     * @else
     * @brief Check whether the file exists
     *
     * Check whether the specified file exists.
     *
     * @param filename Name of file existence for checking
     *
     * @return File existence result(File existence:true, Else:false)
     *
     * @endif
     */
  bool fileExist(const std::string& filename);

    /*!
     * @if jp
     * @brief 初期化関数シンボルを生成する
     *
     * 初期化関数の名称を組み立てる。
     *
     * @param file_path 初期化対象モジュール名称
     *
     * @return 初期化関数名称組み立て結果
     *
     * @else
     * @brief Create initialization function symbol
     *
     * Assemble names of the initialization functions.
     *
     * @param file_path Name of module for initialization
     *
     * @return Assembly result of initialization function name
     *
     * @endif
     */
    std::string getInitFuncName(const std::string& file_path);

  protected:
    /*!
     * @if jp
     * @brief 無効なモジュールプロファイルを削除する
     * @else
     * @brief Removing incalid module profiles
     * @endif
     */
    void removeInvalidModules();

    /*!
     * @if jp
     * @brief 指定言語におけるロードパス上のローダブルなファイルリストを返す
     * @else
     * @brief Getting loadable file list on the loadpath for given language
     * @endif
     */
    void getModuleList(const std::string& lang, coil::vstring& modules);

    /*!
     * @if jp
     * @brief キャッシュに無いパスだけmodulesに追加する
     * @else
     * @brief Adding file path not existing cache
     * @endif
     */
    void addNewFile(const std::string& fpath, coil::vstring& modules, const std::string& lang);

    /*!
     * @if jp
     * @brief 指定言語、ファイルリストからモジュールのプロパティを返す
     * @else
     * @brief Getting module properties from given language and file list
     * @endif
     */
    void getModuleProfiles(const std::string& lang,
                           const coil::vstring& modules, vProperties& modprops);

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
     * @brief DLL管理用構造体
     * @else
     * @brief Structure for DLL management
     * @endif
     */
    struct DLLEntity
    {
      coil::Properties properties;
      coil::DynamicLib dll;
    };

    using StringVector = std::vector<std::string>;
    using StringVectorItr = StringVector::iterator;
    using StringVectorConstItr = StringVector::const_iterator;

    using DllMap = std::vector<DLLEntity>;
    using DllMapItr = DllMap::iterator;
    using DllMapConstItr = DllMap::const_iterator;


    /*!
     * @if jp
     * @brief Module Manager プロパティ
     * @else
     * @brief Module Manager properties
     * @endif
     */
    coil::Properties& m_properties;

    /*!
     * @if jp
     * @brief ロード済みモジュールリスト
     * @else
     * @brief Module list that has already loaded
     * @endif
     */
    class DllPred
    {
      std::string m_filepath;
    public:
      explicit DllPred(const char* filepath) 
      {
        m_filepath = coil::replaceString(filepath, "\\", "/");
        m_filepath = coil::replaceString(m_filepath, "//", "/");
      }
      explicit DllPred(const DLLEntity* dll)
      {
        m_filepath = coil::replaceString(dll->properties["file_path"], "\\", "/");
        m_filepath = coil::replaceString(m_filepath, "//", "/");
      }
      bool operator()(DLLEntity* dllentity)
      {
        std::string file_path = coil::replaceString(
          dllentity->properties.getProperty("file_path"), "\\", "/");
        file_path = coil::replaceString(file_path, "//", "/");
        return m_filepath == dllentity->properties.getProperty("file_path");
      }
    };
    /*!
     * @if jp
     * @brief ロード済みモジュールリスト
     * @else
     * @brief Module list that has already loaded
     * @endif
     */
    ObjectManager<const char*, DLLEntity, DllPred> m_modules;

    /*!
     * @if jp
     * @brief モジュール・ロード・パス・リスト
     * @else
     * @brief Module load path list
     * @endif
     */
    StringVector m_loadPath;
    /*!
     * @if jp
     * @brief コンフィギュレーション・パス・リスト
     * @else
     * @brief Configuration path list
     * @endif
     */
    StringVector m_configPath;
    /*!
     * @if jp
     * @brief モジュールURL指定許可フラグ
     * @else
     * @brief Flag of URL when specify module for the load.
     * @endif
     */
    bool m_downloadAllowed;
    /*!
     * @if jp
     * @brief モジュール絶対パス指定許可フラグ
     * @else
     * @brief Flag of absolute path when specify module for the load.
     * @endif
     */
    bool m_absoluteAllowed;

    /*!
     * @if jp
     * @brief 初期実行関数サフィックス
     * @else
     * @brief Initial execution function suffix
     * @endif
     */
    std::string m_initFuncSuffix;

    /*!
     * @if jp
     * @brief 初期実行関数プリフィックス
     * @else
     * @brief Initial execution function prefix
     * @endif
     */
    std::string m_initFuncPrefix;

    /*!
     * @if jp
     * @brief サポートするRTCの実装言語
     * @else
     * @brief 
     * @endif
     */
    coil::vstring m_supported_languages;

    /*!
     * @if jp
     * @brief モジュールアンロードファンクタ
     * @else
     * @brief Module unloading functor
     * @endif
     */
    class UnloadPred
    {
    public:
      UnloadPred() = default;
      void operator()(DLLEntity* dll)
      {
        dll->dll.close();
        delete dll;
      }
    };

    vProperties m_modprofs;
    std::map<std::string, coil::vstring> m_loadfailmods;

  };   // class ModuleManager
} // namespace RTC

#endif  // RTC_MODULEMANAGER_H
