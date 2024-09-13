// -*- C++ -*-
/*!
 * @file ModuleManager.cpp
 * @brief Loadable modules manager class
 * @date $Date: 2007-12-31 03:08:04 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
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

#include <rtm/RTC.h>

#include <coil/File.h>
#include <coil/Process.h>

// RTC includes
#include <rtm/Manager.h>
#include <rtm/ModuleManager.h>
#include <coil/stringutil.h>

#ifdef __QNX__
using std::FILE;
using std::fgets;
using std::feof;
#endif

namespace RTC
{

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  ModuleManager::ModuleManager(coil::Properties& prop)
    : rtclog("ModuleManager"), m_properties(prop)
  {
    for (auto path : coil::split(prop[CONFIG_PATH], ","))
    {
      m_configPath.emplace_back(coil::eraseHeadBlank(std::move(path)));
    }
    for (auto path : coil::split(prop[MOD_LOADPTH], ","))
    {
      m_loadPath.emplace_back(coil::eraseHeadBlank(std::move(path)));
    }
    m_absoluteAllowed = coil::toBool(prop[ALLOW_ABSPATH], "yes", "no", false);
    m_downloadAllowed = coil::toBool(prop[ALLOW_URL], "yes", "no", false);
    m_initFuncSuffix  = prop[INITFUNC_SFX];
    m_initFuncPrefix  = prop[INITFUNC_PFX];

    if (coil::toBool(prop["manager.is_master"], "YES", "NO", false))
    {
      m_supported_languages = coil::split(prop["manager.supported_languages"], ",");
    }
    else
    {
      m_supported_languages.push_back("C++");
    }
    
    for (auto& lang : m_supported_languages)
    {
        m_loadfailmods[lang] = coil::vstring();
    }
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ModuleManager::~ModuleManager()
  {
    unloadAll();
  }

  /*!
   * @if jp
   * @brief モジュールのロード
   * @else
   * @brief Load the module
   * @endif
   */
  std::string ModuleManager::load(const std::string& file_name)
  {
    RTC_TRACE(("load(fname = %s)", file_name.c_str()));
    coil::Properties prop;
    prop["module_file_name"] = file_name;
    return load(prop);
  }

  /*!
   * @if jp
   * @brief モジュールのロード
   * @else
   * @brief Load the module
   * @endif
   */
  std::string ModuleManager::load(coil::Properties& prop)
  {
    RTC_TRACE(("load(filename = %s, filepath = %s, language = %s)", 
      prop["module_file_name"].c_str(), prop["module_file_path"].c_str(), prop["language"].c_str()));
    std::string file_name(prop["module_file_name"]);
    std::string file_path(prop["module_file_path"]);
    if (file_name.empty())
      throw InvalidArguments("Invalid file name.");

    if (coil::isURL(file_name))
    {
      if (!m_downloadAllowed)
      {
        RTC_ERROR(("Downloading module is not allowed."));
        throw NotAllowedOperation("Downloading module is not allowed.");
      }
      else
      {
        throw NotFound("Not implemented.");
      }
    }

    // Find local file from load path or absolute directory
    if (coil::isAbsolutePath(file_name))
    {
      if (!m_absoluteAllowed)
      {
        RTC_ERROR(("Absolute path is not allowed"));
        throw NotAllowedOperation("Absolute path is not allowed");
      }
      else
      {
        file_path = file_name;
      }
    }
    else if (file_path.empty())
    {
      StringVector paths;
      for (size_t i = 0; i < m_loadPath.size(); i++)
      {
        paths.emplace_back(coil::replaceEnv(m_loadPath[i]));
      }
      file_path = findFile(file_name, paths);
    }

    // Now file_name is valid full path to module
    if (file_path.empty() || !fileExist(file_path))
    {
      RTC_ERROR(("Module file not found: %s", file_name.c_str()));
      throw FileNotFound(file_name);
    }

    DLLEntity *dll(new DLLEntity());

    int retval = dll->dll.open(file_path.c_str());
    if (retval != 0)
    {
      RTC_ERROR(("Module file %s load failed: %s",
        file_path.c_str(), dll->dll.error()));
      delete dll;
      throw Error("DLL open failed.");
    }
    dll->properties["file_path"] = file_path;
    bool ret = m_modules.registerObject(dll);
    if (!ret)
    {
      RTC_ERROR(("Module registration failed: %s", file_path.c_str()));
      delete dll;
    }

    return file_path;
  }



  /*!
   * @if jp
   * @brief モジュールのロード、初期化
   * @else
   * @brief Load and initialize the module
   * @endif
   */
  std::string ModuleManager::load(const std::string& file_name,
                                  const std::string& init_func)
  {
    RTC_TRACE(("load(fname = %s, init_func = %s)",
               file_name.c_str(), init_func.c_str()));
    std::string name;
    name = load(file_name);

    if (name.empty())
      {
        throw InvalidOperation("Invalid file name");
      }

    void (*initfptr)(Manager *);
    *reinterpret_cast<void **>(&initfptr) = this->symbol(name, init_func);
    (*initfptr)(&(Manager::instance()));

    return name;
  }
  /*!
   * @if jp
   * @brief モジュールのロード、初期化
   * @else
   * @brief Load and initialize the module
   * @endif
   */
  std::string ModuleManager::load(coil::Properties& prop,
    const std::string& init_func)
  {
    RTC_TRACE(("load(filename = %s, filepath = %s, language = %s, init_func = %s)", 
      prop["module_file_name"].c_str(), prop["module_file_path"].c_str(), prop["language"].c_str(), init_func.c_str()));
    std::string name;
    name = load(prop);

    if (name.empty())
    {
      throw InvalidOperation("Invalid file name");
    }

    void(*initfptr)(Manager *);
    *reinterpret_cast<void **>(&initfptr) = this->symbol(name, init_func);
    (*initfptr)(&(Manager::instance()));

    return name;
  }

  /*!
   * @if jp
   * @brief モジュールのアンロード
   * @else
   * @brief Unload the module
   * @endif
   */
  void ModuleManager::unload(const std::string& file_name)
  {
    DLLEntity* dll(m_modules.find(file_name.c_str()));
    if (dll == nullptr)
      throw NotFound(file_name);

    dll->dll.close();
    m_modules.unregisterObject(file_name.c_str());

    delete dll;

    return;
  }

  /*!
   * @if jp
   * @brief 全モジュールのアンロード
   * @else
   * @brief Unload all modules
   * @endif
   */
  void ModuleManager::unloadAll()
  {
    RTC_TRACE(("unloadAll()"));
    std::vector<DLLEntity*> dlls(m_modules.getObjects());

    for (auto & d : dlls)
      {
        std::string ident(d->properties["file_path"]);
        DLLEntity* dll(m_modules.unregisterObject(ident.c_str()));
        dll->dll.close();
      }
    return;
  }

  /*!
   * @if jp
   * @brief モジュールのシンボルの参照
   * @else
   * @brief Refer to the symbol of the module
   * @endif
   */
  void* ModuleManager::symbol(const std::string& file_name,
                              const std::string& func_name)
  {
    RTC_TRACE(("symbol(%s, %s)",
               file_name.c_str(), func_name.c_str()));
    // "file_name" should be in modules map.
    DLLEntity* dll(m_modules.find(file_name.c_str()));
    if (dll == nullptr)
      {
        RTC_ERROR(("Module %s not found in module table.", file_name.c_str()));
        throw ModuleNotFound(file_name);
      }

    RTC_DEBUG(("Finding function symbol: %s in %s",
               func_name.c_str(), file_name.c_str()));
    void* func;
    func = dll->dll.symbol(func_name.c_str());

    if (func == nullptr)
      {
        RTC_ERROR(("Specified symbol %s not found.", func_name.c_str()));
        throw SymbolNotFound(func_name);
      }

    return func;
  }

  /*!
   * @if jp
   * @brief モジュールロードパスを指定する
   * @else
   * @brief Set the module load path
   * @endif
   */
  void ModuleManager::setLoadpath(const std::vector<std::string>& load_path)
  {
    RTC_TRACE(("setLoadpath(%s)", coil::flatten(load_path, ", ").c_str()));
    m_loadPath = load_path;
    return;
  }

  /*!
   * @if jp
   * @brief モジュールロードパスを追加する
   * @else
   * @brief Add the module load path
   * @endif
   */
  void ModuleManager::addLoadpath(const std::vector<std::string>& load_path)
  {
    RTC_TRACE(("addLoadpath(%s)", coil::flatten(load_path, ", ").c_str()));
    StringVectorConstItr it(load_path.begin());
    StringVectorConstItr it_end(load_path.end());

    while (it != it_end)
      {
        m_loadPath.emplace_back(*it);
        ++it;
      }

    return;
  }

  /*!
   * @if jp
   * @brief ロード済みのモジュールリストを取得する
   * @else
   * @brief Get the module list that has been loaded
   * @endif
   */
  std::vector<coil::Properties> ModuleManager::getLoadedModules()
  {
    RTC_TRACE(("getLoadedModules()"));
    std::vector< DLLEntity* > dlls(m_modules.getObjects());
    std::vector<coil::Properties> modules(0);
    for (auto & dll : dlls)
      {
        modules.emplace_back(dll->properties);
      }
    return modules;
  }

  /*!
   * @if jp
   * @brief ロード可能なモジュールリストを取得する(未実装)
   * @else
   * @brief Get the loadable module list(not implemented)
   * @endif
   */
  std::vector<coil::Properties> ModuleManager::getLoadableModules()
  {
    RTC_TRACE(("getLoadableModules()"));

    // getting loadable module file path list.
    coil::Properties& gprop(Manager::instance().getConfig());

    RTC_DEBUG(("langs: %s", gprop["manager.supported_languages"].c_str()));

    // for each languages
    for (auto & lang : m_supported_languages)
      {
        // 1. getting loadable files list
        coil::vstring modules(0);
        getModuleList(lang, modules);
        RTC_DEBUG(("%s: %s", lang.c_str(), coil::flatten(modules).c_str()));

        // 2. getting module properties from loadable modules
        vProperties tmpprops(0);
        getModuleProfiles(lang, modules, tmpprops);
        RTC_DEBUG(("Modile profile size: %d (newly founded modules)",
                   tmpprops.size()));
        m_modprofs.insert(m_modprofs.end(),
                          std::make_move_iterator(tmpprops.begin()),
                          std::make_move_iterator(tmpprops.end()));
      }
    RTC_DEBUG(("Modile profile size: %d", m_modprofs.size()));
    // 3. removing module profiles for which module file does not exist
    removeInvalidModules();
    RTC_DEBUG(("Modile profile size: %d (invalid mod-profiles deleted)",
               m_modprofs.size()));
    return m_modprofs;
  }

  /*!
   * @if jp
   * @brief LoadPath からのファイルの検索
   * @else
   * @brief Search the file from the LoadPath
   * @endif
   */
  std::string ModuleManager::findFile(const std::string& fname,
                                      const std::vector<std::string>& load_path)
  {
    RTC_TRACE(("findFile(%s, %s)", fname.c_str(),
               coil::flatten(load_path, ", ").c_str()));
    StringVectorConstItr it, it_end;

    it     = load_path.begin();
    it_end = load_path.end();

    while (it != it_end)
      {
        std::string f{coil::replaceString((*it) + "/" + fname, "//", "/")};
        if (fileExist(f))
          {
            return f;
          }
        coil::vstring ret;
        coil::findFile((*it), fname, ret);
        if (!ret.empty())
          {
            return ret.front();
          }

        ++it;
      }

    return std::string("");
  }

  /*!
   * @if jp
   * @brief ファイルが存在するかどうかのチェック
   * @else
   * @brief Check whether the file exists
   * @endif
   */
  bool ModuleManager::fileExist(const std::string& filename)
  {
    RTC_TRACE(("fileExist(%s)", filename.c_str()));
    std::ifstream infile;
    infile.open(filename.c_str(), std::ios::in);
    // fial() 0: ok, !0: fail
    if (infile.fail())
      {
        infile.close();
        return false;
      }
    else
      {
        infile.close();
        return true;
      }

    return false;
  }

  /*!
   * @if jp
   * @brief 初期化関数シンボルを生成する
   * @else
   * @brief Create initialization function symbol
   * @endif
   */
  std::string ModuleManager::getInitFuncName(const std::string& file_path)
  {
    RTC_TRACE(("getInitFuncName(%s)", file_path.c_str()));
    std::string base_name(coil::basename(file_path.c_str()));

    return m_initFuncPrefix + base_name + m_initFuncSuffix;
  }


  /*!
   * @if jp
   * @brief 無効なモジュールプロファイルを削除する
   * @else
   * @brief Removing incalid module profiles
   * @endif
   */
  void ModuleManager::removeInvalidModules()
  {
    std::vector<coil::Properties>::iterator it(m_modprofs.begin());

    while (it < m_modprofs.end())
      {
        if (!fileExist((*it)["module_file_path"]))
          {
            it = m_modprofs.erase(it);
          }
        else
          {
            ++it;
          }
      }
  }

  /*!
   * @if jp
   * @brief 指定言語におけるロードパス上のローダブルなファイルリストを返す
   * @else
   * @brief Getting loadable file list on the loadpath for given language
   * @endif
   */
  void ModuleManager::getModuleList(const std::string& lang,
                                    coil::vstring& modules)
  {
    std::string l = "manager.modules." + lang;
    coil::Properties& lprop(Manager::instance().getConfig().getNode(l));

    // load path: manager.modules.<lang>.load_path + manager.modules.load_path
    coil::vstring paths(coil::split(lprop["load_paths"], ","));
    paths.insert(paths.end(), m_loadPath.begin(), m_loadPath.end());

    coil::vstring suffixes(coil::split(lprop["suffixes"], ","));
    RTC_DEBUG(("suffixes: %s", coil::flatten(suffixes).c_str()));

    // for each load path list
    for (auto & path : paths)
      {
        if (path.empty())
          {
            RTC_WARN(("Given load path is empty"));
            continue;
          }
        
        RTC_DEBUG(("Module load path: %s", path.c_str()));

        // get file list for each suffixes
        coil::vstring flist(0);
        for (auto & suffixe : suffixes)
          {
            coil::vstring tmp;
            coil::getFileList(path, suffixe, tmp);
            RTC_DEBUG(("File list (path:%s, ext:%s): %s", path.c_str(),
                       suffixe.c_str(), coil::flatten(tmp).c_str()));
            flist.insert(flist.end(),
                         std::make_move_iterator(tmp.begin()),
                         std::make_move_iterator(tmp.end()));
          }

        // reformat file path and remove cached files
        for (auto & f : flist)
          {
            addNewFile(coil::replaceString(coil::replaceString(
                         std::move(f), "\\", "/"), "//", "/"), modules, lang);
          }
      }
      std::sort(modules.begin(), modules.end());
      modules.erase(std::unique(modules.begin(), modules.end()), modules.end());
  }

  /*!
   * @if jp
   * @brief キャッシュに無いパスだけmodulesに追加する
   * @else
   * @brief Adding file path not existing cache
   * @endif
   */
  void ModuleManager::addNewFile(const std::string& fpath,
                                 coil::vstring& modules, const std::string& lang)
  {
    for (auto & modprof : m_modprofs)
      {
                  
        if (modprof["module_file_path"] == fpath)
          {

            RTC_DEBUG(("Module %s already exists in cache.",
                       fpath.c_str()));
            return;
          }
      }
    for (auto & loadfailmod : m_loadfailmods[lang])
      {
        if (loadfailmod == fpath)
          {
            return;
          }
      }
      RTC_DEBUG(("New module: %s", fpath.c_str()));
      modules.emplace_back(fpath);
  }

  /*!
   * @if jp
   * @brief 指定言語、ファイルリストからモジュールのプロパティを返す
   * @else
   * @brief Getting module properties from given language and file list
   * @endif
   */
  void ModuleManager::getModuleProfiles(const std::string& lang,
                                        const coil::vstring& modules,
                                        vProperties& modprops)
  {
#if !defined(VXWORKS_69) && !defined(VXWORKS_66)
    std::string l = "manager.modules." + lang;
    coil::Properties& lprop(Manager::instance().getConfig().getNode(l));

    for (const auto & module : modules)
      {
        std::string cmd(lprop["profile_cmd"]);
        cmd += " \"" + module + "\"";

        coil::vstring outlist;
        if (coil::create_process(cmd, outlist) == -1)
          {
              std::cerr << "create_process faild" << std::endl;
              continue;
          }

        coil::Properties props;
        for (auto & out : outlist)
          {
            std::string::size_type pos(out.find(':'));
            if (pos != std::string::npos)
              {
                  std::string key{coil::eraseBothEndsBlank(out.substr(0, pos))};
                  props[key] = coil::eraseBothEndsBlank(out.substr(pos + 1));
              }
          }

        RTC_DEBUG(("rtcprof cmd sub process done."));
        if (props["implementation_id"].empty())
          {
            m_loadfailmods[lang].emplace_back(module);
            continue;
          }
        props["module_file_name"] = coil::basename(module.c_str());
        props["module_file_path"] = module;
        props["language"] = lang;
        modprops.emplace_back(std::move(props));
      }
#endif
  }

} // namespace RTC
