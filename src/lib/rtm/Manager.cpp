// -*- C++ -*-
/*!
 * @file Manager.h
 * @brief RTComponent manager class
 * @date $Date: 2008-03-06 06:58:40 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2003-2005
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */


#include <rtm/Manager.h>
#include <rtm/ManagerConfig.h>
#include <rtm/ModuleManager.h>
#include <rtm/CorbaNaming.h>
#include <rtm/NamingManager.h>
#include <rtm/RTC.h>
#include <rtm/PeriodicExecutionContext.h>
#include <rtm/SimulatorExecutionContext.h>
#include <rtm/ExtTrigExecutionContext.h>
#include <rtm/OpenHRPExecutionContext.h>
#include <rtm/PeriodicECSharedComposite.h>
#include <rtm/MultilayerCompositeEC.h>
#include <rtm/RTCUtil.h>
#include <rtm/ManagerServant.h>
#include <coil/Properties.h>
#include <coil/stringutil.h>
#include <coil/Signal.h>
#include <coil/TimeValue.h>
#include <coil/Timer.h>
#include <coil/OS.h>
#include <rtm/FactoryInit.h>
#include <rtm/CORBA_IORUtil.h>
#include <rtm/CORBA_RTCUtil.h>
#include <rtm/SdoServiceConsumerBase.h>
#include <rtm/LocalServiceAdmin.h>
#include <rtm/SystemLogger.h>
#include <rtm/LogstreamBase.h>
#include <rtm/NumberingPolicyBase.h>

#ifdef RTM_OS_VXWORKS
#include <rtm/VxWorksRTExecutionContext.h>
#ifndef __RTP__
#include <rtm/VxWorksInterruptExecutionContext.h>
#endif
#endif


#ifdef RTM_OS_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  // _GNU_SOURCE
#include <pthread.h>
#endif  // RTM_OS_LINUX

#include <fstream>
#include <iostream>

#if defined(minor)
#undef minor
#endif

//static sig_atomic_t g_mgrActive = true;
extern "C" void handler(int)
{
  ::RTC::Manager::instance().terminate();
}

namespace RTC
{
  Manager* Manager::manager = nullptr;
  coil::Mutex Manager::mutex;

  Manager::InstanceName::InstanceName(RTObject_impl* comp)
    : m_name(comp->getInstanceName())
  {}
  Manager::InstanceName::InstanceName(const char* name)
    : m_name(name)
  {}
  Manager::InstanceName::InstanceName(const std::string& name)
    : m_name(name)
  {}
  bool Manager::InstanceName::operator()(RTObject_impl* comp)
  {
    return m_name == comp->getInstanceName();
  }


  /*!
   * @if jp
   * @brief Protected コンストラクタ
   * @else
   * @brief Protected Constructor
   * @endif
   */
  Manager::Manager()
    : m_initProc(nullptr), m_namingManager(nullptr), m_timer(nullptr),
      m_logStreamBuf(), rtclog(&m_logStreamBuf),
      m_runner(nullptr), m_terminator(nullptr)
  {
    new coil::SignalAction((coil::SignalHandler) handler, SIGINT);
  }

  /*!
   * @if jp
   * @brief Protected コピーコンストラクタ
   * @else
   * @brief Protected Copy Constructor
   * @endif
   */
  Manager::Manager(const Manager& manager)
    : m_initProc(nullptr), m_namingManager(nullptr), m_timer(nullptr),
      m_logStreamBuf(), rtclog(&m_logStreamBuf),
      m_runner(nullptr), m_terminator(nullptr)
  {
    new coil::SignalAction((coil::SignalHandler) handler, SIGINT);
  }

  /*!
   * @if jp
   * @brief マネージャの初期化
   * @else
   * @brief Initialize manager
   * @endif
   */
  Manager* Manager::init(int argc, char** argv)
  {
    // DCL for singleton
    if (!manager)
      {
        Guard guard(mutex);
        if (!manager)
          {
            manager = new Manager();
            manager->initManager(argc, argv);
            manager->initFactories();
            manager->initLogger();
            manager->initORB();
            manager->initNaming();
            manager->initExecContext();
            manager->initComposite();
            manager->initTimer();
            manager->initManagerServant();
          }
      }
    return manager;
  }

  /*!
   * @if jp
   * @brief マネージャのインスタンスの取得
   * @else
   * @brief Get instance of the manager
   * @endif
   */
  Manager& Manager::instance()
  {
    // DCL for singleton
    if (!manager)
      {
        Guard guard(mutex);
        if (!manager)
          {
            manager = new Manager();
            manager->initManager(0, nullptr);
            manager->initFactories();
            manager->initLogger();
            manager->initORB();
            manager->initNaming();
            manager->initExecContext();
            manager->initComposite();
            manager->initTimer();
            manager->initManagerServant();
          }
      }
    return *manager;
  }

  /*!
   * @if jp
   * @brief マネージャ終了処理
   * @else
   * @brief Terminate Manager
   * @endif
   */
  void Manager::terminate()
  {
    if (m_terminator != nullptr)
      {
	m_terminator->terminate();
      }
  }

  /*!
   * @if jp
   * @brief マネージャ・シャットダウン
   * @else
   * @brief Shutdown Manager
   * @endif
   */
  void Manager::shutdown()
  {
    RTC_TRACE(("Manager::shutdown()"));
    m_listeners.manager_.preShutdown();
    if (m_timer != nullptr)
      {
        m_timer->stop();
      }
    shutdownComponents();
    shutdownNaming();
    shutdownORB();
    shutdownManager();
    // 終了待ち合わせ
    if (m_runner != nullptr)
      {
        m_runner->wait();
      }
    else
      {
        join();
      }
    m_listeners.manager_.postShutdown();
    shutdownLogger();
  }

  /*!
   * @if jp
   * @brief マネージャ終了処理の待ち合わせ
   * @else
   * @brief Wait for Manager's termination
   * @endif
   */
  void Manager::join()
  {
    RTC_TRACE(("Manager::wait()"));
    {
      Guard guard(m_terminate.mutex);
      ++m_terminate.waiting;
    }
    while (true)
      {
        {
          Guard guard(m_terminate.mutex);
          if (m_terminate.waiting > 1) break;
        }
        coil::usleep(100000);
      }
  }

  /*!
   * @if jp
   * @brief 初期化プロシージャのセット
   * @else
   * @brief Set initial procedure
   * @endif
   */
  void Manager::setModuleInitProc(ModuleInitProc proc)
  {
    m_initProc = proc;
  }

  /*!
   * @if jp
   * @brief Managerのアクティブ化
   * @else
   * @brief Activate the Manager
   * @endif
   */
  bool Manager::activateManager()
  {
    RTC_TRACE(("Manager::activateManager()"));

    try
      {
        if (CORBA::is_nil(this->thePOAManager()))
        {
          RTC_ERROR(("Could not get POA manager."));
          return false;
        }
        this->thePOAManager()->activate();
        RTC_TRACE(("POA Manager activated."));
      }
    catch (...)
      {
        RTC_ERROR(("POA Manager activatatin failed."));
        return false;
      }

    std::vector<std::string> lsvc;
    lsvc = coil::split(m_config["manager.local_service.modules"], ",");

    for (int i(0), len(lsvc.size()); i < len; ++i)
      {
        size_t begin_pos(lsvc[i].find_first_of('('));
        size_t end_pos(lsvc[i].find_first_of(')'));
        std::string filename, initfunc;
        if (begin_pos != std::string::npos && end_pos != std::string::npos &&
            begin_pos < end_pos)
          {
            initfunc = lsvc[i].substr(begin_pos + 1, end_pos - (begin_pos + 1));
            filename = lsvc[i].substr(0, begin_pos);
            coil::eraseBothEndsBlank(initfunc);
            coil::eraseBothEndsBlank(filename);
          }
        else
          {
            initfunc = coil::split(lsvc[i], ".").operator[](0) + "Init";
            filename = lsvc[i];
          }
        if (filename.find_first_of('.') == std::string::npos)
          {
            if (m_config.findNode("manager.modules.C++.suffixes") != nullptr)
              {
                filename += "." + m_config["manager.modules.C++.suffixes"];
              }
          }
        try
          {
            m_module->load(filename, initfunc);
          }
        catch (ModuleManager::Error& e)
          {
            RTC_ERROR(("Module load error: %s", e.reason.c_str()));
          }
        catch (ModuleManager::SymbolNotFound& e)
          {
            RTC_ERROR(("Symbol not found: %s", e.name.c_str()));
          }
        catch (ModuleManager::ModuleNotFound& e)
          {
            RTC_ERROR(("Module not found: %s", e.name.c_str()));
          }
        catch (...)
          {
            RTC_ERROR(("Unknown Exception"));
          }
      }

    initLocalService();

    std::vector<std::string> mods;
    mods = coil::split(m_config["manager.modules.preload"], ",");

    for (int i(0), len(mods.size()); i < len; ++i)
      {
        size_t begin_pos(mods[i].find_first_of('('));
        size_t end_pos(mods[i].find_first_of(')'));
        std::string filename, initfunc;
        if (begin_pos != std::string::npos && end_pos != std::string::npos &&
            begin_pos < end_pos)
          {
            initfunc = mods[i].substr(begin_pos + 1, end_pos - (begin_pos + 1));
            filename = mods[i].substr(0, begin_pos);
            coil::eraseBothEndsBlank(initfunc);
            coil::eraseBothEndsBlank(filename);
          }
        else
          {
            initfunc = coil::split(mods[i], ".").operator[](0) + "Init";
            filename = mods[i];
          }
        if (filename.find_first_of('.') == std::string::npos)
          {
            if (m_config.findNode("manager.modules.C++.suffixes") != nullptr)
              {
                filename += "." + m_config["manager.modules.C++.suffixes"];
              }
          }
        try
          {
            m_module->load(filename, initfunc);
          }
        catch (ModuleManager::Error& e)
          {
            RTC_ERROR(("Module load error: %s", e.reason.c_str()));
          }
        catch (ModuleManager::SymbolNotFound& e)
          {
            RTC_ERROR(("Symbol not found: %s", e.name.c_str()));
          }
        catch (ModuleManager::ModuleNotFound& e)
          {
            RTC_ERROR(("Module not found: %s", e.name.c_str()));
          }
        catch (...)
          {
            RTC_ERROR(("Unknown Exception"));
          }
      }

    m_config["sdo.service.consumer.available_services"]
      = coil::flatten(SdoServiceConsumerFactory::instance().getIdentifiers());





	invokeInitProc();
	initPreCreation();
	initPreConnection();
	initPreActivation();

    return true;
  }

  /*!
   * @if jp
   * @brief Managerの実行
   * @else
   * @brief Run the Manager
   * @endif
   */
  void Manager::runManager(bool no_block)
  {
    if (no_block)
      {
        RTC_TRACE(("Manager::runManager(): non-blocking mode"));
        m_runner = new OrbRunner(m_pORB);
        m_runner->open(nullptr);
      }
    else
      {
        RTC_TRACE(("Manager::runManager(): blocking mode"));
        m_pORB->run();
        RTC_TRACE(("Manager::runManager(): ORB was terminated"));
        join();
      }
    return;
  }

  //============================================================
  // Module management
  //============================================================
  /*!
   * @if jp
   * @brief モジュールのロード
   * @else
   * @brief Load module
   * @endif
   */
  ReturnCode_t Manager::load(const std::string& fname,
                             const std::string& initfunc)
  {
    RTC_TRACE(("Manager::load(fname = %s, initfunc = %s)",
               fname.c_str(), initfunc.c_str()));

    std::string file_name(fname);
    std::string init_func(initfunc);
    m_listeners.module_.preLoad(file_name, init_func);
    try
      {
        if (init_func.empty())
          {
            coil::vstring mod(coil::split(file_name, "."));
            init_func = mod[0] + "Init";
          }
        std::string path(m_module->load(file_name, init_func));
        RTC_DEBUG(("module path: %s", path.c_str()));
        m_listeners.module_.postLoad(path, init_func);
      }
    catch(RTC::ModuleManager::NotAllowedOperation& e)
      {
        RTC_ERROR(("Operation not allowed: %s",
                   e.reason.c_str()));
        return RTC::PRECONDITION_NOT_MET;
      }
    catch(RTC::ModuleManager::NotFound& e)
      {
        RTC_ERROR(("Not found: %s",
                   e.name.c_str()));
        return RTC::RTC_ERROR;
      }
    catch(RTC::ModuleManager::InvalidArguments& e)
      {
        RTC_ERROR(("Invalid argument: %s",
                   e.reason.c_str()));
        return RTC::BAD_PARAMETER;
      }
    catch(RTC::ModuleManager::Error& e)
      {
        RTC_ERROR(("Error: %s", e.reason.c_str()));
        return RTC::RTC_ERROR;
      }
    catch (...)
      {
        RTC_ERROR(("Unknown error."));
        return RTC::RTC_ERROR;
      }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief モジュールのアンロード
   * @else
   * @brief Unload module
   * @endif
   */
  void Manager::unload(const char* fname)
  {
    RTC_TRACE(("Manager::unload()"));
    std::string fnamestr(fname);
    m_listeners.module_.preUnload(fnamestr);
    m_module->unload(fname);
    m_listeners.module_.postUnload(fnamestr);
    return;
  }

  /*!
   * @if jp
   * @brief 全モジュールのアンロード
   * @else
   * @brief Unload all modules
   * @endif
   */
  void Manager::unloadAll()
  {
    RTC_TRACE(("Manager::unloadAll()"));
    m_module->unloadAll();
    return;
  }

  /*!
   * @if jp
   * @brief ロード済みのモジュールリストを取得する
   * @else
   * @brief Get a list of loaded modules
   * @endif
   */
  std::vector<coil::Properties> Manager::getLoadedModules()
  {
    RTC_TRACE(("Manager::getLoadedModules()"));
    return m_module->getLoadedModules();
  }

  /*!
   * @if jp
   * @brief ロード可能なモジュールリストを取得する
   * @else
   * @brief Get a list of loadable modules
   * @endif
   */
std::vector<coil::Properties> Manager::getLoadableModules()
  {
    RTC_TRACE(("Manager::getLoadableModules()"));
    return m_module->getLoadableModules();
  }

  //============================================================
  // Component factory management
  //============================================================
  /*!
   * @if jp
   * @brief RTコンポーネント用ファクトリを登録する
   * @else
   * @brief Register RT-Component Factory
   * @endif
   */
  bool Manager::registerFactory(coil::Properties& profile,
                                RtcNewFunc new_func,
                                RtcDeleteFunc delete_func)
  {
    RTC_TRACE(("Manager::registerFactory(%s)", profile["type_name"].c_str()));

    std::string policy_name =
      m_config.getProperty("manager.components.naming_policy", "default");
    RTM::NumberingPolicyBase* policy =
      RTM::NumberingPolicyFactory::instance().createObject(policy_name);
    FactoryBase* factory;
    if (policy == nullptr)
      {
        factory = new FactoryCXX(profile, new_func, delete_func);
      }
    else
      {
        factory = new FactoryCXX(profile, new_func, delete_func, policy);
      }
    try
      {
        bool ret = m_factory.registerObject(factory);
        if (!ret) {
          delete factory;
          return false;
        }
        return true;
      }
    catch (...)
      {
        delete factory;
        return false;
      }
  }

  std::vector<coil::Properties> Manager::getFactoryProfiles()
  {
    std::vector<FactoryBase*> factories(m_factory.getObjects());
    std::vector<coil::Properties> props;
    for (int i(0), len(factories.size()); i < len; ++i)
      {
        props.push_back(factories[i]->profile());
      }
    return props;
  }

  /*!
   * @if jp
   * @brief ExecutionContext用ファクトリを登録する
   * @else
   * @brief Register ExecutionContext Factory
   * @endif
   */
  bool Manager::registerECFactory(const char* name,
                                  ECNewFunc new_func,
                                  ECDeleteFunc delete_func)
  {
    RTC_TRACE(("Manager::registerECFactory(%s)", name));
    try
      {
        ECFactoryBase* factory;
        factory = new ECFactoryCXX(name, new_func, delete_func);
        if (m_ecfactory.registerObject(factory))
          {
            return true;
          }
      }
    catch (...)
      {
        return false;
      }
    return false;
  }

  /*!
   * @if jp
   * @brief ファクトリ全リストを取得する
   * @else
   * @brief Get the list of all Factories
   * @endif
   */
  std::vector<std::string> Manager::getModulesFactories()
  {
    RTC_TRACE(("Manager::getModulesFactories()"));

    ModuleFactories m;
    return m_factory.for_each(m).modlist;
  }

  //============================================================
  // Component management
  //============================================================
  /*!
   * @if jp
   * @brief RTコンポーネントを生成する
   * @else
   * @brief Create RT-Components
   * @endif
   */
  RTObject_impl* Manager::createComponent(const char* comp_args)
  {
    RTC_TRACE(("Manager::createComponent(%s)", comp_args));
    std::string argstr(comp_args);
    m_listeners.rtclifecycle_.preCreate(argstr);
    //------------------------------------------------------------
    // extract "comp_type" and "comp_prop" from comp_arg
    coil::Properties comp_prop = coil::Properties();
    coil::Properties comp_id   = coil::Properties();
    if (!procComponentArgs(argstr.c_str(), comp_id, comp_prop)) return nullptr;

    //------------------------------------------------------------
    // Because the format of port-name had been changed from <port_name>
    // to <instance_name>.<port_name>, the following processing was added.
    // (since r1648)

    if (comp_prop.findNode("exported_ports") != nullptr)
      {
        coil::vstring exported_ports;
        exported_ports = coil::split(comp_prop["exported_ports"], ",");

        std::string exported_ports_str;
        for (size_t i(0), len(exported_ports.size()); i < len; ++i)
          {
            coil::vstring keyval(coil::split(exported_ports[i], "."));
            if (keyval.size() > 2)
              {
                exported_ports_str += (keyval[0] + "." + keyval.back());
              }
            else
              {
                exported_ports_str += exported_ports[i];
              }

            if (i != exported_ports.size() - 1)
              {
                exported_ports_str += ",";
              }
          }

         comp_prop["exported_ports"] = exported_ports_str;
         comp_prop["conf.default.exported_ports"] = exported_ports_str;

      }
    //------------------------------------------------------------

    //------------------------------------------------------------
    // Create Component
    FactoryBase* factory(m_factory.find(comp_id));
    if (factory == nullptr)
      {
        RTC_ERROR(("Factory not found: %s",
                   comp_id["implementation_id"].c_str()));

        if (!coil::toBool(m_config["manager.modules.search_auto"], "YES", "NO", true))
          {
            return nullptr;
          }
        // automatic module loading
        std::vector<coil::Properties> mp(m_module->getLoadableModules());
        RTC_INFO(("%d loadable modules found", mp.size()));

        std::vector<coil::Properties>::iterator it;
        it = std::find_if(mp.begin(), mp.end(), ModulePredicate(comp_id));
        if (it == mp.end())
          {
            RTC_ERROR(("No module for %s in loadable modules list",
                       comp_id["implementation_id"].c_str()));
            return nullptr;
          }
        if (it->findNode("module_file_name") == nullptr)
          {
            RTC_ERROR(("Hmm...module_file_name key not found."));
            return nullptr;
          }
        // module loading
        RTC_INFO(("Loading module: %s", (*it)["module_file_name"].c_str()))
          load((*it)["module_file_name"].c_str(), "");
        factory = m_factory.find(comp_id);
        if (factory == nullptr)
          {
            RTC_ERROR(("Factory not found for loaded module: %s",
                       comp_id["implementation_id"].c_str()));
            return nullptr;
          }
      }

    coil::Properties prop;
    prop = factory->profile();

    const char* inherit_prop[] = {
      "config.version",
      "openrtm.name",
      "openrtm.version",
      "os.name",
      "os.release",
      "os.version",
      "os.arch",
      "os.hostname",
      "corba.endpoints",
      "corba.endpoints_ipv4",
      "corba.endpoints_ipv6",
      "corba.id",
      "exec_cxt.periodic.type",
      "exec_cxt.periodic.rate",
      "exec_cxt.event_driven.type",
      "exec_cxt.sync_transition",
      "exec_cxt.sync_activation",
      "exec_cxt.sync_deactivation",
      "exec_cxt.sync_reset",
      "exec_cxt.transition_timeout",
      "exec_cxt.activation_timeout",
      "exec_cxt.deactivation_timeout",
      "exec_cxt.reset_timeout",
      "exec_cxt.cpu_affinity",
      "exec_cxt.priority",
      "exec_cxt.stack_size",
      "exec_cxt.interrupt",
      "logger.enable",
      "logger.log_level",
      "naming.enable",
      "naming.type",
      "naming.formats",
      "sdo.service.provider.available_services",
      "sdo.service.consumer.available_services",
      "sdo.service.provider.enabled_services",
      "sdo.service.consumer.enabled_services",
      "manager.instance_name",
      ""
    };

    coil::Properties &_prop = prop.getNode("port");
    _prop << m_config.getNode("port");

    RTObject_impl* comp;
    comp = factory->create(this);
    if (comp == nullptr)
      {
        RTC_ERROR(("RTC creation failed: %s",
                   comp_id["implementation_id"].c_str()));
        return nullptr;
      }

    if (m_config.getProperty("corba.endpoints_ipv4").empty())
      {
        setEndpointProperty(comp->getObjRef());
      }

    for (int i(0); inherit_prop[i][0] != '\0'; ++i)
      {
        const char* key(inherit_prop[i]);
        if (m_config.findNode(key) != nullptr)
          {
            prop[key] = m_config[key];
          }
      }

    RTC_TRACE(("RTC created: %s", comp_id["implementation_id"].c_str()));
    m_listeners.rtclifecycle_.postCreate(comp);
    prop << comp_prop;

    //------------------------------------------------------------
    // Load configuration file specified in "rtc.conf"
    //
    // rtc.conf:
    //   [category].[type_name].config_file = file_name
    //   [category].[instance_name].config_file = file_name
    m_listeners.rtclifecycle_.preConfigure(prop);
    configureComponent(comp, prop);
    m_listeners.rtclifecycle_.postConfigure(prop);
    // comp->setProperties(prop);

    //------------------------------------------------------------
    // Component initialization
    m_listeners.rtclifecycle_.preInitialize();
    if (comp->initialize() != RTC::RTC_OK)
      {
        RTC_TRACE(("RTC initialization failed: %s",
                   comp_id["implementation_id"].c_str()));
        RTC_TRACE(("%s was finalized", comp_id["implementation_id"].c_str()));
        if (comp->exit() != RTC::RTC_OK)
          {
            RTC_DEBUG(("%s finalization was failed.",
                       comp_id["implementation_id"].c_str()));
          }
        return nullptr;
      }
    RTC_TRACE(("RTC initialization succeeded: %s",
               comp_id["implementation_id"].c_str()));
    m_listeners.rtclifecycle_.postInitialize();
    //------------------------------------------------------------
    // Bind component to naming service
    registerComponent(comp);
    return comp;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントを直接 Manager に登録する
   * @else
   * @brief Register RT-Component directly without Factory
   * @endif
   */
  bool Manager::registerComponent(RTObject_impl* comp)
  {
    RTC_TRACE(("Manager::registerComponent(%s)", comp->getInstanceName()));
    // ### NamingManager のみで代用可能
    m_compManager.registerObject(comp);

    coil::vstring names(comp->getNamingNames());

    m_listeners.naming_.preBind(comp, names);
    for (int i(0), len(names.size()); i < len; ++i)
      {
        RTC_TRACE(("Bind name: %s", names[i].c_str()));
        m_namingManager->bindObject(names[i].c_str(), comp);
      }
    m_listeners.naming_.postBind(comp, names);

	publishPorts(comp);
	subscribePorts(comp);


	try
	{
		std::string id_str = comp->getCategory();
		id_str = id_str + "." + comp->getInstanceName();

#ifndef ORB_IS_TAO
		CORBA::Object_ptr obj = theORB()->resolve_initial_references("omniINSPOA");
		PortableServer::POA_ptr poa = PortableServer::POA::_narrow(obj);
		poa->the_POAManager()->activate();

		

		PortableServer::ObjectId_var id;

#ifndef ORB_IS_RTORB
		id = PortableServer::string_to_ObjectId(id_str.c_str());
#else // ORB_IS_RTORB
		id = PortableServer::
			string_to_ObjectId((char *)id_str.c_str());
#endif // ORB_IS_RTORB

		poa->activate_object_with_id(id.in(), comp);

		CORBA::Object_var rtcobj = poa->id_to_reference(id);
		comp->setINSObjRef(::RTC::LightweightRTObject::_narrow(rtcobj));
#else
		CORBA::Object_var obj = theORB()->resolve_initial_references("IORTable");
		IORTable::Table_var adapter = IORTable::Table::_narrow(obj.in());

		CORBA::String_var ior = theORB()->object_to_string(comp->getObjRef());
		adapter->bind(id_str.c_str(), ior.in());

		comp->setINSObjRef(comp->getObjRef());
#endif
	}
	catch (...)
	{
	}

    return true;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントの登録を解除する
   * @else
   * @brief Unregister RT-Components
   * @endif
   */
  bool Manager::unregisterComponent(RTObject_impl* comp)
  {
    RTC_TRACE(("Manager::unregisterComponent(%s)", comp->getInstanceName()));
    // ### NamingManager のみで代用可能
    m_compManager.unregisterObject(comp->getInstanceName());

    coil::vstring names(comp->getNamingNames());

    m_listeners.naming_.preUnbind(comp, names);
    for (int i(0), len(names.size()); i < len; ++i)
      {
        RTC_TRACE(("Unbind name: %s", names[i].c_str()));
        m_namingManager->unbindObject(names[i].c_str());
      }
    m_listeners.naming_.postUnbind(comp, names);

    return true;
  }


  ExecutionContextBase* Manager::createContext(const char* ec_args)
  {
    RTC_TRACE(("Manager::createContext()"));
    RTC_TRACE(("ExecutionContext type: %s",
               m_config.getProperty("exec_cxt.periodic.type").c_str()));

    std::string ec_id = std::string();
    coil::Properties ec_prop = coil::Properties();
    if (!procContextArgs(ec_args, ec_id, ec_prop)) { return nullptr; }

    ECFactoryBase* factory(m_ecfactory.find(ec_id.c_str()));
    if (factory == nullptr)
      {
        RTC_ERROR(("Factory not found: %s", ec_id.c_str()));
        return nullptr;
      }

    ExecutionContextBase* ec;
    ec = factory->create();
    return ec;
  }

  /*!
   * @if jp
   * @brief Manager に登録されているRTコンポーネントを削除する
   * @else
   * @brief Unregister RT-Components that have been registered to Manager
   * @endif
   */
  void Manager::deleteComponent(RTObject_impl* comp)
  {
    RTC_TRACE(("deleteComponent(RTObject*)"));
    // cleanup from manager's table, and naming serivce
    unregisterComponent(comp);

    // find factory
    coil::Properties& comp_id(comp->getProperties());
    FactoryBase* factory(m_factory.find(comp_id));
    if (factory == nullptr)
      {
        RTC_DEBUG(("Factory not found: %s",
                   comp_id["implementation_id"].c_str()));
        return;
      }
    else
      {
        RTC_DEBUG(("Factory found: %s",
                   comp_id["implementation_id"].c_str()));
        factory->destroy(comp);
      }

    if (coil::toBool(m_config["manager.shutdown_on_nortcs"],
                     "YES", "NO", true) &&
        !coil::toBool(m_config["manager.is_master"], "YES", "NO", false))
      {
        std::vector<RTObject_impl*> comps;
        comps = getComponents();
        if (comps.empty())
          {
            shutdown();
          }
      }
  }

  void Manager::deleteComponent(const char* instance_name)
  {
    RTC_TRACE(("deleteComponent(%s)", instance_name));
    RTObject_impl* comp;
    comp = m_compManager.find(instance_name);
    if (comp == nullptr)
      {
        RTC_WARN(("RTC %s was not found in manager.", instance_name));
        return;
      }
    deleteComponent(comp);
  }

  /*!
   * @if jp
   * @brief Manager に登録されているRTコンポーネントを検索する
   * @else
   * @brief Get RT-Component's pointer
   * @endif
   */
  RTObject_impl* Manager::getComponent(const char* instance_name)
  {
    RTC_TRACE(("Manager::getComponent(%s)", instance_name));
    return m_compManager.find(instance_name);
  }

  /*!
   * @if jp
   * @brief Manager に登録されている全RTコンポーネントを取得する
   * @else
   * @brief Get all RT-Components registered in the Manager
   * @endif
   */
  std::vector<RTObject_impl*> Manager::getComponents()
  {
    RTC_TRACE(("Manager::getComponents()"));
    return m_compManager.getObjects();
  }

  void Manager::
  addManagerActionListener(RTM::ManagerActionListener* listener,
                           bool autoclean)
  {
    m_listeners.manager_.addListener(listener, autoclean);
  }
  void Manager::
  removeManagerActionListener(RTM::ManagerActionListener* listener)
  {
    m_listeners.manager_.removeListener(listener);
  }

  void Manager::
  addModuleActionListener(RTM::ModuleActionListener* listener,
                           bool autoclean)
  {
    m_listeners.module_.addListener(listener, autoclean);
  }
  void Manager::
  removeModuleActionListener(RTM::ModuleActionListener* listener)
  {
    m_listeners.module_.removeListener(listener);
  }

  void Manager::
  addRtcLifecycleActionListener(RTM::RtcLifecycleActionListener* listener,
                                bool autoclean)
  {
    m_listeners.rtclifecycle_.addListener(listener, autoclean);
  }
  void Manager::
  removeRtcLifecycleActionListener(RTM::RtcLifecycleActionListener* listener)
  {
    m_listeners.rtclifecycle_.removeListener(listener);
  }

  void Manager::
  addNamingActionListener(RTM::NamingActionListener* listener,
                          bool autoclean)
  {
    m_listeners.naming_.addListener(listener, autoclean);
  }

  void Manager::
  removeNamingActionListener(RTM::NamingActionListener* listener)
  {
    m_listeners.naming_.removeListener(listener);
  }

  void Manager::
  addLocalServiceActionListener(RTM::LocalServiceActionListener* listener,
                                bool autoclean)
  {
    m_listeners.localservice_.addListener(listener, autoclean);
  }

  void Manager::
  removeLocalServiceActionListener(RTM::LocalServiceActionListener* listener)
  {
    m_listeners.localservice_.removeListener(listener);
  }


  //============================================================
  // CORBA 関連
  //============================================================
  /*!
   * @if jp
   * @brief ORB のポインタを取得する (所有権保持)
   * @else
   * @brief Get the pointer to the ORB (ownership retained)
   * @endif
   */
  CORBA::ORB_ptr Manager::theORB()
  {
    RTC_TRACE(("Manager::theORB()"));
    return m_pORB.in();
  }
  /*!
   * @if jp
   * @brief ORB のポインタを取得する (所有権複製)
   * @else
   * @brief Get the pointer to the ORB (ownership duplicated)
   * @endif
   */
  CORBA::ORB_ptr Manager::getORB()
  {
    RTC_TRACE(("Manager::getORB()"));
    return CORBA::ORB::_duplicate(m_pORB);
  }

  /*!
   * @if jp
   * @brief Manager が持つ RootPOA のポインタを取得する (所有権保持)
   * @else
   * @brief Get the pointer to the RootPOA (ownership retained)
   * @endif
   */
  PortableServer::POA_ptr Manager::thePOA()
  {
    RTC_TRACE(("Manager::thePOA()"));
    return m_pPOA.in();
  }
#ifdef ORB_IS_OMNIORB
  PortableServer::POA_ptr Manager::theShortCutPOA()
  {
    RTC_TRACE(("Manager::theShortCutPOA()"));
    return m_pShortCutPOA.in();
  }
#endif
  /*!
   * @if jp
   * @brief Manager が持つ RootPOA のポインタを取得する (所有権複製)
   * @else
   * @brief Get the pointer to the RootPOA (ownership duplicated)
   * @endif
   */
  PortableServer::POA_ptr Manager::getPOA()
  {
    RTC_TRACE(("Manager::getPOA()"));
    return PortableServer::POA::_duplicate(m_pPOA);
  }
#ifdef ORB_IS_OMNIORB
  PortableServer::POA_ptr Manager::getShortCutPOA()
  {
    RTC_TRACE(("Manager::getPOA()"));
    return PortableServer::POA::_duplicate(m_pShortCutPOA);
  }
#endif
  
  /*!
   * @if jp
   * @brief Manager が持つ POAManager を取得する (所有権保持)
   * @else
   * @brief Get the pointer to the POAManager (ownership retained)
   * @endif
   */
  PortableServer::POAManager_ptr Manager::thePOAManager()
  {
    RTC_TRACE(("Manager::thePOAManager()"));
    return m_pPOAManager.in();
  }

  /*!
   * @if jp
   * @brief Manager が持つ POAManager を取得する (所有権複製)
   * @else
   * @brief Get the pointer to the POAManager (ownership duplicated)
   * @endif
   */
  PortableServer::POAManager_ptr Manager::getPOAManager()
  {
    RTC_TRACE(("Manager::getPOAManager()"));
    return PortableServer::POAManager::_duplicate(m_pPOAManager);
  }

  //============================================================
  // Protected functions
  //============================================================

  //============================================================
  // Manager initialization and finalization
  //============================================================
  /*!
   * @if jp
   * @brief Manager の内部初期化処理
   * @else
   * @brief Manager internal initialization
   * @endif
   */
  void Manager::initManager(int argc, char** argv)
  {
    // load configurations
    ManagerConfig config(argc, argv);
    config.configure(m_config);

    // initialize ModuleManager
    m_module = new ModuleManager(m_config);

    // initialize Terminator
    double waittime(0.5);
    if (m_config.findNode("manager.termination_waittime") != nullptr)
      {
	const char* s = m_config["manager.termination_waittime"].c_str();
	if (!coil::stringTo(waittime, s))
	  {
	    waittime = 0.5;
	  }
      }
    m_terminator = new Terminator(this, waittime);
    {
      Guard guard(m_terminate.mutex);
      m_terminate.waiting = 0;
    }

    // initialize Timer
    if (coil::toBool(m_config["timer.enable"], "YES", "NO", true))
      {
        coil::TimeValue tm(0, 100000);
        std::string tick(m_config["timer.tick"]);
        if (!tick.empty())
          {
            tm = atof(tick.c_str());
            m_timer = new coil::Timer(tm);
            m_timer->start();
          }
      }

    if (coil::toBool(m_config["manager.shutdown_auto"], "YES", "NO", true) &&
        !coil::toBool(m_config["manager.is_master"], "YES", "NO", false))
      {
        coil::TimeValue tm(10, 0);
        if (m_config.findNode("manager.auto_shutdown_duration") != nullptr)
          {
            double duration(10.0);
            const char* s = m_config["manager.auto_shutdown_duration"].c_str();
            if (coil::stringTo(duration, s))
              {
                tm = duration;
              }
          }
        if (m_timer != nullptr)
          {
            m_timer->registerListenerObj(this,
                                         &Manager::shutdownOnNoRtcs, tm);
          }
      }

    {
      coil::TimeValue tm(1, 0);
      if (m_timer != nullptr)
        {
          m_timer->registerListenerObj(this,
                                       &Manager::cleanupComponents, tm);
        }
    }


	{
		coil::vstring lmpm_ = coil::split(m_config["manager.preload.modules"], ",");
		for (coil::vstring::iterator itr = lmpm_.begin(); itr != lmpm_.end(); ++itr)
		{
			std::string mpm_ = (*itr);
			coil::eraseBothEndsBlank(mpm_);
			if (mpm_.empty())
			{
				continue;
			}
			std::string basename_ = coil::split(mpm_, ".").front() + "Init";
			try
			{
				m_module->load(mpm_, basename_);
			}
			catch (...)
			{
				//RTC_ERROR((""));
			}
		}

	}
	m_config["manager.instance_name"] = formatString(m_config["manager.instance_name"].c_str(), m_config);


  }

  /*!
   * @if jp
   * @brief Manager の終了処理
   * @else
   * @brief Manager internal finalization
   * @endif
   */
  void Manager::shutdownManager()
  {
    RTC_TRACE(("Manager::shutdownManager()"));
  }

  void  Manager::shutdownOnNoRtcs()
  {
    RTC_TRACE(("Manager::shutdownOnNoRtcs()"));
    if (coil::toBool(m_config["manager.shutdown_on_nortcs"], "YES", "NO", true))
      {
        std::vector<RTObject_impl*> comps(getComponents());
        if (comps.empty())
          {
            terminate();
          }
      }

  }

  //============================================================
  // Logger initialization and finalization
  //============================================================
  /*!
   * @if jp
   * @brief File logger 初期化
   * initLoggerから呼ばれる
   * @else
   * @brief File logger initialization
   * This function is called from initLogger.
   * @endif
   */
  void Manager::initLogstreamFile()
  {
    // format logger file name
    m_config["logger.file_name"] = 
      formatString(m_config["logger.file_name"].c_str(), m_config);

    std::vector<std::string> logouts =
      coil::split(m_config["logger.file_name"], ",");
    coil::Properties& logprop = m_config.getNode("logger");

    for (int i(0), len(logouts.size()); i < len; ++i)
      {
        if (logouts[i].empty()) { continue; }

        LogstreamBase* logstream =
          LogstreamFactory::instance().createObject("file");
        if (logstream == nullptr)
          {
            std::cerr << "\"file\" logger creation failed" << std::endl;
            continue;
          }
        if (!logstream->init(logprop))
          {
            std::cerr << "\"file\" logger initialization failed" << std::endl;
            LogstreamFactory::instance().deleteObject("file", logstream);
            continue;
          }
        m_logStreamBuf.addStream(logstream->getStreamBuffer());
      }
  }

  void Manager::initLogstreamPlugins()
  {
    // loading logstream module
    // create logstream object and attach to the logger
    coil::vstring mods = coil::split(m_config["logger.plugins"], ",");
    for (size_t i(0); i < mods.size(); ++i)
      {
        std::string basename = mods[i].substr(0, mods[i].find('.'));
        basename += "Init";
        try
          {
            m_module->load(mods[i], basename);
          }
        catch (...)
          {
            RTC_WARN(("Logstream plugin module load failed: %s",
                      mods[i].c_str()));
            continue;
          }
      }
  }

  void Manager::initLogstreamOthers()
  {
    LogstreamFactory& factory(LogstreamFactory::instance());

    coil::Properties pp(m_config.getNode("logger.logstream"));

    const std::vector<Properties*>& leaf0 = pp.getLeaf();
    for (size_t i(0); i < leaf0.size(); ++i)
      {
        std::string lstype = leaf0[i]->getName();
        LogstreamBase* logstream = factory.createObject(lstype);
        if (logstream == nullptr)
          {
            RTC_WARN(("Logstream %s creation failed.", lstype.c_str()));
            continue;
          }
        RTC_INFO(("Logstream %s created.", lstype.c_str()));
        if (!logstream->init(*leaf0[i]))
          {
            RTC_WARN(("Logstream %s init failed.", lstype.c_str()));
            factory.deleteObject(lstype.c_str(), logstream);
            RTC_WARN(("Logstream %s deleted.", lstype.c_str()));
          }
        RTC_INFO(("Logstream %s added.", lstype.c_str()));
        m_logStreamBuf.addStream(logstream->getStreamBuffer());
      }
  }

  /*!
   * @if jp
   * @brief System logger の初期化
   * @else
   * @brief System logger initialization
   * @endif
   */
  bool Manager::initLogger()
  {
    // Enable logger or not
    rtclog.setLevel("SILENT");
    rtclog.setName("manager");
    if (!coil::toBool(m_config["logger.enable"], "YES", "NO", true))
      {
        return true;
      }

    // Set date format for log entry header
    rtclog.setDateFormat(m_config["logger.date_format"].c_str());
    rtclog.setClockType(m_config["logger.clock_type"]);
    // Loglevel was set from configuration file.
    rtclog.setLevel(m_config["logger.log_level"].c_str());
    // Log stream mutex locking mode
    coil::toBool(m_config["logger.stream_lock"], "enable", "disable", false) ?
      rtclog.enableLock() : rtclog.disableLock();

    // File Logstream init
    initLogstreamFile();
    // Load logstream plugin
    initLogstreamPlugins();
    // Initialize other logstreams
    initLogstreamOthers();

    RTC_INFO(("%s", m_config["openrtm.version"].c_str()));
    RTC_INFO(("Copyright (C) 2003-2017"));
    RTC_INFO(("  Noriaki Ando"));
    RTC_INFO(("  Intelligent Systems Research Institute, AIST"));
    RTC_INFO(("Manager starting."));
    RTC_INFO(("Starting local logging."));

    return true;
  }

  /*!
   * @if jp
   * @brief System Logger の終了処理
   * @else
   * @brief System Logger finalization
   * @endif
   */
  void Manager::shutdownLogger()
  {
    RTC_TRACE(("Manager::shutdownLogger()"));
    rtclog.flush();

    for (int i(0), len(m_logfiles.size()); i < len; ++i)
      {
        m_logfiles[i]->close();
        //        m_logStreamBuf.removeStream(m_logfiles[i]->rdbuf());
        delete m_logfiles[i];
      }
    if (!m_logfiles.empty())
      {
        m_logfiles.clear();
      }
  }

  //============================================================
  // ORB initialization and finalization
  //============================================================
  /*!
   * @if jp
   * @brief CORBA ORB の初期化処理
   * @else
   * @brief CORBA ORB initialization
   * @endif
   */
  bool Manager::initORB()
  {
    RTC_TRACE(("Manager::initORB()"));
    // Initialize ORB
    try
      {
	std::vector<std::string> args(coil::split(createORBOptions(), " "));
	// TAO's ORB_init needs argv[0] as command name.
	args.insert(args.begin(), "manager");
	char** argv = coil::toArgv(args);
	int argc(args.size());
	
#ifdef ORB_IS_ORBEXPRESS
	CORBA::ORB::spawn_flags(VX_SPE_TASK | VX_STDIO);
	CORBA::ORB::stack_size(20000);
	m_pORB = CORBA::ORB_init(argc, argv, "");
	CORBA::Object_var obj =
          m_pORB->resolve_initial_references((char*)"RootPOA");
	m_pPOA = PortableServer::POA::_narrow(obj);
#else
	// ORB initialization
	m_pORB = CORBA::ORB_init(argc, argv);
	// Get the RootPOA
	CORBA::Object_var obj =
          m_pORB->resolve_initial_references((char*)"RootPOA");
	m_pPOA = PortableServer::POA::_narrow(obj);
#endif
	if (CORBA::is_nil(m_pPOA))
	  {
	    RTC_ERROR(("Could not resolve RootPOA."));
	    return false;
	  }
	// Get the POAManager
	m_pPOAManager = m_pPOA->the_POAManager();
#ifdef ORB_IS_OMNIORB
	CORBA::PolicyList pl;
	pl.length(1);
#ifdef RTM_OMNIORB_42
	pl[0] = omniPolicy::create_local_shortcut_policy(omniPolicy::LOCAL_CALLS_SHORTCUT);
#else
	CORBA::Any v;
	v <<= omniPolicy::LOCAL_CALLS_SHORTCUT;
	pl[0] = m_pORB->create_policy(omniPolicy::LOCAL_SHORTCUT_POLICY_TYPE, v);
#endif
	m_pShortCutPOA = m_pPOA->create_POA("shortcut", m_pPOAManager, pl);
#endif

#ifdef ORB_IS_OMNIORB
        const char* conf = "corba.alternate_iiop_addresses";
        if (m_config.findNode(conf) != nullptr)
          {
            coil::vstring addr_list;
            addr_list = coil::split(m_config[conf], ",", true);

            for (size_t i(0); i < addr_list.size(); ++i)
              {
                coil::vstring addr_port = coil::split(addr_list[i], ":");
                if (addr_port.size() == 2)
                  {
                    IIOP::Address iiop_addr;
                    iiop_addr.host = addr_port[0].c_str();
                    CORBA::UShort port;
                    coil::stringTo(port, addr_port[1].c_str());
                    iiop_addr.port = port;
#if defined(RTM_OMNIORB_40) || defined(RTM_OMNIORB_41)
                    omniIOR::add_IIOP_ADDRESS(iiop_addr);
#else
                    omniIOR::add_IIOP_ADDRESS(iiop_addr, nullptr);
#endif  // defined(RTC_OMNIORB_40) and defined(RTC_OMNIORB_41)
                  }
              }
          }
#endif  // ORB_IS_OMNIORB
      }
    catch (...)
      {
        RTC_ERROR(("Exception: Caught unknown exception in initORB()."));
        return false;
      }
    return true;
  }

  /*!
   * @if jp
   * @brief ORB のコマンドラインオプション作成
   * @else
   * @brief ORB command option creation
   * @endif
   */
  std::string Manager::createORBOptions()
  {
    std::string opt(m_config["corba.args"]);
    RTC_DEBUG(("corba.args: %s", opt.c_str()));

    RTC_DEBUG_STR((m_config));

    coil::vstring endpoints(0);
    createORBEndpoints(endpoints);
    createORBEndpointOption(opt, endpoints);

    RTC_PARANOID(("ORB options: %s", opt.c_str()));
    return opt;
  }

  void Manager::createORBEndpoints(coil::vstring& endpoints)
  {
    // corba.endpoint is obsolete
    // corba.endpoints with comma separated values are acceptable
    if (m_config.findNode("corba.endpoints") != nullptr)
      {
        endpoints = coil::split(m_config["corba.endpoints"], ",");
        RTC_DEBUG(("corba.endpoints: %s", m_config["corba.endpoints"].c_str()));
      }

    if (m_config.findNode("corba.endpoint") != nullptr)
      {
        coil::vstring tmp(coil::split(m_config["corba.endpoint"], ","));
        endpoints.insert(endpoints.end(), tmp.begin(), tmp.end());
        RTC_DEBUG(("corba.endpoint: %s", m_config["corba.endpoint"].c_str()));
      }
    // If this process has master manager,
    // master manager's endpoint inserted at the top of endpoints
    RTC_DEBUG(("manager.is_master: %s",
               m_config["manager.is_master"].c_str()));
    if (coil::toBool(m_config["manager.is_master"], "YES", "NO", false))
      {
        std::string mm(m_config.getProperty("corba.master_manager", ":2810"));
        coil::vstring mmm(coil::split(mm, ":"));
        if (mmm.size() == 2)
          {
            endpoints.insert(endpoints.begin(), std::string(":") + mmm[1]);
          }
        else
          {
            endpoints.insert(endpoints.begin(), ":2810");
          }
      }
    coil::vstring tmp(endpoints);
    endpoints = coil::unique_sv(tmp);
  }

  void Manager::createORBEndpointOption(std::string& opt,
                                        coil::vstring& endpoints)
  {
    std::string corba(m_config["corba.id"]);
    RTC_DEBUG(("corba.id: %s", corba.c_str()));

    for (size_t i(0); i < endpoints.size(); ++i)
      {
        std::string& endpoint(endpoints[i]);
        RTC_DEBUG(("Endpoint is : %s", endpoint.c_str()));
        if (endpoint.find(':') == std::string::npos) { endpoint += ":"; }

        if (corba == "omniORB")
          {
            coil::normalize(endpoint);
            if (coil::normalize(endpoint) == "all:")
              {
#ifdef ORB_IS_OMNIORB
#ifdef RTC_CORBA_CXXMAPPING11
                // omniORB 4.1 or later
                opt += " -ORBendPointPublish all(addr)";
#else
                // omniORB 4.0
                opt += " -ORBendPointPublishAllIFs 1";
#endif  // RTC_CORBA_CXXMAPPING1
#endif  // ORB_IS_OMNIORB
              }
            else
              {
                opt += " -ORBendPoint giop:tcp:" + endpoint;
              }
          }
        else if (corba == "TAO")
          {
            opt += " -ORBEndPoint iiop://" + endpoint;
          }
        else if (corba == "MICO")
          {
            opt += "-ORBIIOPAddr inet:" + endpoint;
          }
        else
          {
          }
      }
  }


  /*!
   * @if jp
   * @brief ORB の終了処理
   * @else
   * @brief ORB finalization
   * @endif
   */
  void Manager::shutdownORB()
  {
    RTC_TRACE(("Manager::shutdownORB()"));
    if (CORBA::is_nil(m_pORB))
      {
        return;
      }
    try
      {
      while (m_pORB->work_pending())
        {
          RTC_PARANOID(("Pending work still exists."));
          if (m_pORB->work_pending())
            m_pORB->perform_work();
        }
        RTC_DEBUG(("No pending works of ORB. Shutting down POA and ORB."));
      }
    catch(...)
      {
        RTC_ERROR(("Caught SystemException during perform_work."));
      }

    if (!CORBA::is_nil(m_pPOA))
      {
        try
          {
            if (!CORBA::is_nil(m_pPOAManager))
              m_pPOAManager->deactivate(false, true);
            RTC_DEBUG(("POA Manager was deactivated."));
            m_pPOA->destroy(false, true);
            m_pPOA = PortableServer::POA::_nil();
            RTC_DEBUG(("POA was destroid."));
          }
        catch (CORBA::SystemException& ex)
          {
            RTC_ERROR(("Exception cought during root POA destruction"));
#ifndef ORB_IS_RTORB
            RTC_ERROR(("CORBA::SystemException(minor=%d)", ex.minor()));
#endif  // ORB_IS_RTORB
          }
        catch (...)
          {
            RTC_ERROR(("Caught unknown exception during POA destruction."));
          }
      }

    if (!CORBA::is_nil(m_pORB))
      {
        try
          {
            m_pORB->shutdown(true);
            RTC_DEBUG(("ORB was shutdown."));
            //m_pORB->destroy();
            RTC_DEBUG(("ORB was destroied."));
            m_pORB = CORBA::ORB::_nil();
          }
        catch (CORBA::SystemException& ex)
          {
            RTC_ERROR(("Exception caught during ORB shutdown"));
#ifndef ORB_IS_RTORB
            RTC_ERROR(("CORBA::SystemException(minodor=%d)", ex.minor()));
#endif  // ORB_IS_RTORB
          }
        catch (...)
          {
            RTC_ERROR(("Caught unknown exception during ORB shutdown."));
          }
      }
  }

  //============================================================
  // Naming initialization and finalization
  //============================================================
  /*!
   * @if jp
   * @brief NamingManager の初期化
   * @else
   * @brief NamingManager initialization
   * @endif
   */
  bool Manager::initNaming()
  {
    RTC_TRACE(("Manager::initNaming()"));

    m_namingManager = new NamingManager(this);

    // If NameService is disabled, return immediately
    if (!coil::toBool(m_config["naming.enable"], "YES", "NO", true))
      {
        return true;
      }

    // NameServer registration for each method and servers
    std::vector<std::string> meth(coil::split(m_config["naming.type"], ","));

    for (int i(0), len_i(meth.size()); i < len_i; ++i)
      {
        std::vector<std::string> names;
        names = coil::split(m_config[meth[i] + ".nameservers"], ",");


        for (int j(0), len_j(names.size()); j < len_j; ++j)
          {
            RTC_TRACE(("Register Naming Server: %s/%s",                \
                       meth[i].c_str(), names[j].c_str()));
            m_namingManager->registerNameServer(meth[i].c_str(),
                                                names[j].c_str());
          }
      }

    // NamingManager Timer update initialization
    if (coil::toBool(m_config["naming.update.enable"], "YES", "NO", true))
      {
        coil::TimeValue tm(10, 0);  // default interval = 10sec for safty
        std::string intr(m_config["naming.update.interval"]);
        if (!intr.empty())
          {
            tm = atof(intr.c_str());
          }
        if (m_timer != nullptr)
          {
            m_timer->registerListenerObj(m_namingManager,
                                         &NamingManager::update, tm);
          }
      }
    return true;
  }

  /*!
   * @if jp
   * @brief NamingManager の終了処理
   * @else
   * @brief NamingManager finalization
   * @endif
   */
  void Manager::shutdownNaming()
  {
    RTC_TRACE(("Manager::shutdownNaming()"));
    std::vector<RTObject_impl*> comps = getComponents();

    for (size_t i(0); i < comps.size(); ++i)
      {
        coil::vstring names = comps[i]->getNamingNames();
        m_listeners.naming_.preUnbind(comps[i], names);
        for (size_t j(0); j < names.size(); ++j)
          {
            m_namingManager->unbindObject(names[j].c_str());
          }
        m_listeners.naming_.postUnbind(comps[i], names);
      }

    m_namingManager->unbindAll();
    delete m_namingManager;
  }

  /*!
   * @if jp
   * @brief NamingManagerを取得する
   * @else
   * @brief Getting NamingManager
   * @endif
   */
  NamingManager& Manager::getNamingManager()
  {
    return *m_namingManager;
  }

  //============================================================
  // Naming initialization and finalization
  //============================================================
  /*!
   * @if jp
   * @brief ExecutionContextManager の初期化
   * @else
   * @brief ExecutionContextManager initialization
   * @endif
   */
  bool Manager::initExecContext()
  {
    RTC_TRACE(("Manager::initExecContext()"));
    PeriodicExecutionContextInit(this);
    ExtTrigExecutionContextInit(this);
    OpenHRPExecutionContextInit(this);
    SimulatorExecutionContextInit(this);
    MultilayerCompositeECInit(this);
#ifdef RTM_OS_VXWORKS
    VxWorksRTExecutionContextInit(this);
#ifndef __RTP__
    VxWorksInterruptExecutionContextInit(this);
#endif
#endif
    // initialize CPU affinity
#ifdef RTM_OS_LINUX
    initCpuAffinity();
#endif  // RTM_OS_LINUX

    return true;
  }

  bool Manager::initComposite()
  {
    RTC_TRACE(("Manager::initComposite()"));
    PeriodicECSharedCompositeInit(this);

    return true;
  }

  bool Manager::initFactories()
  {
    RTC_TRACE(("Manager::initFactories()"));
    RTM::FactoryInit();
    return true;
  }

  void Manager::initCpuAffinity()
  {
    RTC_TRACE(("initCpuAffinity()"));
#ifdef RTM_OS_LINUX
    if (m_config.findNode("manager.cpu_affinity") == nullptr) { return; }

    std::string& affinity(m_config["manager.cpu_affinity"]);
    RTC_DEBUG(("CPU affinity property: %s", affinity.c_str()));

    coil::vstring tmp = coil::split(affinity, ",", true);

    pid_t pid = getpid();
    cpu_set_t cpu_set; CPU_ZERO(&cpu_set);

    for (size_t i(0); i < tmp.size(); ++i)
      {
        int num;
        if (coil::stringTo(num, tmp[i].c_str()))
          {
            CPU_SET(num, &cpu_set);
            RTC_DEBUG(("CPU affinity mask set to %d", num));
          }
      }

    int result = sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0)
      {
        RTC_ERROR(("pthread_getaffinity_np():"
                   "CPU affinity mask setting failed"));
      }
    CPU_ZERO(&cpu_set);
    result = sched_getaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0)
      {
        RTC_ERROR(("pthread_getaffinity_np(): returned error."));
      }
    for (size_t j(0); j < CPU_SETSIZE; ++j)
      {
        if (CPU_ISSET(j, &cpu_set))
          {
            RTC_DEBUG(("Current CPU affinity mask is %d.", j));
          }
      }
#endif  // RTM_OS_LINUX
  }

  /*!
   * @if jp
   * @brief Timer の初期化
   * @else
   * @brief Timer initialization
   * @endif
   */
  bool Manager::initTimer()
  {
    return true;
  }

  bool Manager::initManagerServant()
  {
    RTC_TRACE(("Manager::initManagerServant()"));
    if (!coil::toBool(m_config["manager.corba_servant"], "YES", "NO", true))
      {
        return true;
      }
    m_mgrservant = new ::RTM::ManagerServant();
    if (m_config.getProperty("corba.endpoints_ipv4").empty())
      {
        setEndpointProperty(m_mgrservant->getObjRef());
      }
    coil::Properties& prop(m_config.getNode("manager"));
    std::vector<std::string> names(coil::split(prop["naming_formats"], ","));

    if (coil::toBool(prop["is_master"], "YES", "NO", true))
      {
        for (int i(0), len(names.size()); i < len; ++i)
          {
            std::string mgr_name(formatString(names[i].c_str(), prop));
            m_namingManager->bindObject(mgr_name.c_str(), m_mgrservant);
          }
      }

	if (coil::toBool(m_config["corba.update_master_manager.enable"], "YES", "NO", true)
		&& !coil::toBool(m_config["manager.is_master"], "YES", "NO", false))
	{
		coil::TimeValue tm(10, 0);
		if (m_config.findNode("corba.update_master_manager.interval") != nullptr)
		{
			float duration = 10;
			
			coil::stringTo<float>(duration, m_config["corba.update_master_manager.interval"].c_str());
			if (duration > 0)
			{
				tm = coil::TimeValue(duration);
			}
			if (m_timer)
			{
				m_timer->registerListenerObj(m_mgrservant, &RTM::ManagerServant::updateMasterManager, tm);
			}

			
		}

	}

    return true;
  }

  /*!
   * @if jp
   * @brief ManagerServantを取得する
   * @else
   * @brief Getting ManagerServant
   * @endif
   */
  RTM::ManagerServant& Manager::getManagerServant()
  {
	  RTC_TRACE(("Manager.getManagerServant()"));
    return *m_mgrservant;
  }

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
  ** @return NamingManager
  *
  * @endif
  */
  NamingManager* Manager::getNaming()
  {
	  RTC_TRACE(("Manager.getNaming()"));
	  return m_namingManager;
  }

  bool Manager::initLocalService()
  {
    RTC_TRACE(("Manager::initLocalService()"));

    RTM::LocalServiceAdmin& admin = RTM::LocalServiceAdmin::instance();
    coil::Properties& prop(m_config.getNode("manager.local_service"));
    admin.init(prop);
    RTC_DEBUG(("LocalServiceAdmin's properties:"));
    RTC_DEBUG_STR((prop));

    RTM::LocalServiceProfileList svclist = admin.getServiceProfiles();
    for (size_t i(0); i < svclist.size(); ++i)
      {
        RTC_INFO(("Available local service: %s (%s)",
        svclist[i].name.c_str(), svclist[i].uuid.c_str()));
      }
    return true;
  }

  /*!
   * @if jp
   * @brief NamingManager の終了処理
   * @else
   * @brief NamingManager finalization
   * @endif
   */
  void Manager::shutdownComponents()
  {
    RTC_TRACE(("Manager::shutdownComponents()"));
    std::vector<RTObject_impl*> comps;
    comps = m_namingManager->getObjects();
    for (int i(0), len(comps.size()); i < len; ++i)
      {
        try
          {
            comps[i]->exit();
            coil::Properties p(comps[i]->getInstanceName());
            p << comps[i]->getProperties();
            rtclog.lock();
            rtclog.write(::RTC::Logger::RTL_PARANOID,  p);
            rtclog.unlock();
          }
        catch (...)
          {
          }
      }
    for (CORBA::ULong i(0), len(m_ecs.size()); i < len; ++i)
      {
        try {
          PortableServer::RefCountServantBase* servant;
          servant =
                  dynamic_cast<PortableServer::RefCountServantBase*>(m_ecs[i]);
          if (servant == nullptr)
            {
              RTC_ERROR(
                  ("Invalid dynamic cast. EC->RefCountServantBase failed."));
              return;
            }
          PortableServer::ObjectId_var oid = m_pPOA->servant_to_id(servant);
          m_pPOA->deactivate_object(oid);
        }
        catch (...)
          {
          }
      }
    return;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントの登録解除
   * @else
   * @brief Unregister RT-Components
   * @endif
   */
  void Manager::cleanupComponent(RTObject_impl* comp)
  {
    RTC_TRACE(("Manager::cleanupComponent()"));
    unregisterComponent(comp);
  }

  void Manager::cleanupComponents()
  {
    RTC_VERBOSE(("Manager::cleanupComponents()"));
    Guard guard(m_finalized.mutex);
    RTC_VERBOSE(("%d components are marked as finalized.",
               m_finalized.comps.size()));
    for (size_t i(0); i < m_finalized.comps.size(); ++i)
      {
        deleteComponent(m_finalized.comps[i]);
      }
    m_finalized.comps.clear();
  }

  void Manager::notifyFinalized(RTObject_impl* comp)
  {
    RTC_TRACE(("Manager::notifyFinalized()"));
    Guard guard(m_finalized.mutex);
    m_finalized.comps.push_back(comp);
  }

  /*!
   * @if jp
   * @brief createComponentの引数を処理する
   * @else
   *
   * @endif
   */
  bool Manager::procComponentArgs(const char* comp_arg,
                                  coil::Properties& comp_id,
                                  coil::Properties& comp_conf)
  {
    std::vector<std::string> id_and_conf(coil::split(comp_arg, "?"));
    // arg should be "id?key0=value0&key1=value1...".
    // id is mandatory, conf is optional
    if (id_and_conf.size() != 1 && id_and_conf.size() != 2)
      {
        RTC_ERROR(("Invalid arguments. Two or more '?' in arg : %s", comp_arg));
        return false;
      }

	//RTM::CompParam::prof_list

    if (id_and_conf[0].find(':') == std::string::npos)
      {
		  std::string id = RTM::CompParam::prof_list[0];
		  id = id + ":::";

		  id_and_conf[0].insert(0, id);
        id_and_conf[0] += "::";
      }
    std::vector<std::string> id(coil::split(id_and_conf[0], ":"));

    // id should be devided into 1 or 5 elements
    // RTC:[vendor]:[category]:impl_id:[version] => 5
	if (id.size() != RTM::CompParam::prof_list_size)
      {
        RTC_ERROR(("Invalid RTC id format.: %s", id_and_conf[0].c_str()));
        return false;
      }

	if (id[0] != RTM::CompParam::prof_list[0])
      {
        RTC_ERROR(("Invalid id type: %s", id[0].c_str()));
        return false;
      }
	for (size_t i(1); i < RTM::CompParam::prof_list_size; ++i)
      {
		  comp_id[RTM::CompParam::prof_list[i]] = id[i];
		  RTC_TRACE(("RTC basic propfile %s: %s", RTM::CompParam::prof_list[i], id[i].c_str()));
      }

    if (id_and_conf.size() == 2)
      {
        std::vector<std::string> conf(coil::split(id_and_conf[1], "&"));
        for (int i(0), len(conf.size()); i < len; ++i)
          {
            if (conf[i].empty()) { continue; }
            std::vector<std::string> keyval(coil::split(conf[i], "="));
            if (keyval.size() != 2) { continue; }
            comp_conf[keyval[0]] = keyval[1];
            RTC_TRACE(("RTC property %s: %s",
                       keyval[0].c_str(), keyval[1].c_str()));
          }
      }
    return true;
  }

  bool Manager::procContextArgs(const char* ec_args,
                                std::string& ec_id,
                                coil::Properties& ec_conf)
  {
    std::vector<std::string> id_and_conf(coil::split(ec_args, "?"));
    if (id_and_conf.size() != 1 && id_and_conf.size() != 2)
      {
        RTC_ERROR(("Invalid arguments. Two or more '?' in arg : %s", ec_args));
        return false;
      }
    if (id_and_conf[0].empty())
      {
        RTC_ERROR(("Empty ExecutionContext's name"));
        return false;
      }
    ec_id = id_and_conf[0];

    if (id_and_conf.size() == 2)
      {
        std::vector<std::string> conf(coil::split(id_and_conf[1], "&"));
        for (int i(0), len(conf.size()); i < len; ++i)
          {
            std::vector<std::string> k(coil::split(conf[i], "="));
            ec_conf[k[0]] = k[1];
            RTC_TRACE(("EC property %s: %s", k[0].c_str(), k[1].c_str()));
          }
      }
    return true;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントのコンフィギュレーション処理
   * @else
   *
   * @endif
   */
  void Manager::configureComponent(RTObject_impl* comp,
                                   const coil::Properties& prop)
  {
    std::string category(comp->getCategory());
    std::string type_name(comp->getTypeName());
    std::string inst_name(comp->getInstanceName());

    std::string type_conf(category + "." + type_name + ".config_file");
    std::string name_conf(category + "." + inst_name + ".config_file");
    coil::vstring config_fname;
    coil::Properties type_prop, name_prop;

    // Load "category.instance_name.config_file"
    if (!m_config[name_conf].empty())
      {
        std::ifstream conff(m_config[name_conf].c_str());
        if (!conff.fail())
          {
            name_prop.load(conff);
            RTC_INFO(("Component instance conf file: %s loaded.",
                      m_config[name_conf].c_str()));
            RTC_DEBUG_STR((name_prop))
            config_fname.push_back(m_config[name_conf].c_str());
          }
      }
    if (m_config.findNode(category + "." + inst_name) != nullptr)
      {
        coil::Properties& temp(m_config.getNode(category + "." + inst_name));
        coil::vstring keys(temp.propertyNames());
        if (!(keys.size() == 1 && keys.back() == "config_file"))
          {
            name_prop << m_config.getNode(category + "." + inst_name);
            RTC_INFO(("Component type conf exists in rtc.conf. Merged."));
            RTC_DEBUG_STR((name_prop));
            if (m_config.findNode("config_file") != nullptr)
              {
                config_fname.push_back(m_config["config_file"]);
              }
          }
      }
    if (!m_config[type_conf].empty())
      {
        std::ifstream conff(m_config[type_conf].c_str());
        if (!conff.fail())
          {
            type_prop.load(conff);
            RTC_INFO(("Component type conf file: %s loaded.",
                      m_config[type_conf].c_str()));
            RTC_DEBUG_STR((type_prop));
            config_fname.push_back(m_config[type_conf].c_str());
          }
      }
    if (m_config.findNode(category + "." + type_name) != nullptr)
      {
        coil::Properties& temp(m_config.getNode(category + "." + type_name));
        coil::vstring keys(temp.propertyNames());
        if (!(keys.size() == 1 && keys.back() == "config_file"))
          {
            type_prop << m_config.getNode(category + "." + type_name);
            RTC_INFO(("Component type conf exists in rtc.conf. Merged."));
            RTC_DEBUG_STR((type_prop));
            if (m_config.findNode("config_file") != nullptr)
              {
                config_fname.push_back(m_config["config_file"]);
              }
          }
      }
    // Merge Properties. type_prop is merged properties
    comp->setProperties(prop);
    type_prop << name_prop;
    type_prop["config_file"] = coil::flatten(coil::unique_sv(config_fname));
    comp->setProperties(type_prop);

    //------------------------------------------------------------
    // Format component's name for NameService
    std::string naming_formats;
    coil::Properties& comp_prop(comp->getProperties());

    naming_formats += m_config["naming.formats"];
    if (comp_prop.findNode("naming.formats") != nullptr)
      {
        naming_formats = comp_prop["naming.formats"];
      }
    naming_formats = coil::flatten(coil::unique_sv(coil::split(naming_formats,
                                                               ",")));

    std::string naming_names;
    naming_names = formatString(naming_formats.c_str(), comp->getProperties());
    comp->getProperties()["naming.formats"] = naming_formats;
    comp->getProperties()["naming.names"] = naming_names;
  }

  /*!
   * @if jp
   * @brief プロパティ情報のマージ
   * @else
   * @brief Merge property information
   * @endif
   */
  bool Manager::mergeProperty(coil::Properties& prop, const char* file_name)
  {
    if (file_name == nullptr)
      {
        RTC_ERROR(("Invalid configuration file name."));
        return false;
      }
    if (file_name[0] != '\0')
      {
        std::ifstream conff(file_name);
        if (!conff.fail())
          {
            prop.load(conff);
            conff.close();
            return true;
          }
      }
    return false;
  }

  /*!
   * @if jp
   * @brief NamingServer に登録する際の登録情報を組み立てる
   * @else
   * @brief Construct registration information when registering to Naming server
   * @endif
   */
  std::string Manager::formatString(const char* naming_format,
                                    coil::Properties& prop)
  {
    std::string name(naming_format), str;
    std::string::iterator it, it_end;
    int count(0);

    it = name.begin();
    it_end = name.end();
    for ( ; it != it_end; ++it)
      {
        char c(*it);
        if (c == '%')
          {
            ++count;
            if (!(count % 2)) str.push_back((*it));
          }
        else if (c == '$')
          {
            count = 0;
            ++it;
            if (*it == '{' || *it == '(')
              {
                ++it;
                std::string env = std::string();
                for ( ; it != it_end && (*it) != '}' && (*it) != ')'; ++it)
                  {
                    env += *it;
                  }
                char* envval = coil::getenv(env.c_str());
                if (envval != nullptr) str += envval;
              }
            else
              {
                str.push_back(c);
              }
          }
        else
          {
            if (count > 0 && (count % 2))
              {
                count = 0;
                if      (c == 'n')  str += prop["instance_name"];
                else if (c == 't')  str += prop["type_name"];
                else if (c == 'm')  str += prop["type_name"];
                else if (c == 'v')  str += prop["version"];
                else if (c == 'V')  str += prop["vendor"];
                else if (c == 'c')  str += prop["category"];
                else if (c == 'i')  str += prop["implementation_id"];
                else if (c == 'N')
                  {
                    size_t n = prop["implementation_id"].size();
                    str += prop["instance_name"].substr(n);
                  }
                else if (c == 'h')  str += m_config["os.hostname"];
                else if (c == 'M')  str += m_config["manager.name"];
                else if (c == 'p')  str += m_config["manager.pid"];
                else
                  {
                    str.push_back(c);
                  }
              }
            else
              {
                count = 0;
                str.push_back(c);
              }
          }
      }
    return str;
  }

  /*!
   * @if jp
   * @brief corba.endpoints にエンドポイント情報を設定する
   * @else
   * @brief Setting endpoint info from corba.endpoints
   * @endif
   */
  void Manager::setEndpointProperty(CORBA::Object_ptr objref)
  {
#if !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
    RTC_TRACE(("sedEndpointProperty()"));
    if (CORBA::is_nil(objref))
      {
        RTC_WARN(("Object reference is nil."));
        return;
      }

    bool ipv4, ipv6;
    std::vector<int> ipv4_list, ipv6_list;
    endpointPropertySwitch("ipv4", ipv4, ipv4_list);
    endpointPropertySwitch("ipv6", ipv6, ipv6_list);

    CORBA::String_var iorstr = theORB()->object_to_string(objref);
    IOP::IOR ior;
    CORBA_IORUtil::toIOR(iorstr, ior);
    std::vector<IIOP::Address> endpoints;
    endpoints = CORBA_IORUtil::getEndpoints(ior);

    coil::vstring epstr, epstr_ipv4, epstr_ipv6;
    size_t ipv4_count(0), ipv6_count(0);

    coil::vstring addrs;
    for (size_t i(0); i < endpoints.size(); ++i)
      {
        std::string addr(endpoints[i].host);
        if (ipv4 && coil::isIPv4(addr))
          {
            std::string tmp(addr + ":" + coil::otos(endpoints[i].port));
            if (ipv4_list.empty() ||
                std::find(ipv4_list.begin(), ipv4_list.end(), ipv4_count)
                != ipv4_list.end())
              {
                epstr.push_back(tmp);
                epstr_ipv4.push_back(tmp);
              }
            ipv4_count += 1;
          }
        if (ipv6 && coil::isIPv6(addr))
          {
            std::string tmp("[" + addr + "]:" + coil::otos(endpoints[i].port));
            if (ipv6_list.empty() ||
                std::find(ipv6_list.begin(), ipv6_list.end(), ipv6_count)
                != ipv6_list.end())
              {
                epstr.push_back(tmp);
                epstr_ipv6.push_back(tmp);
              }
            ipv6_count += 1;
          }
      }
    m_config.setProperty("corba.endpoints", coil::flatten(epstr));
    m_config.setProperty("corba.endpoints_ipv4", coil::flatten(epstr_ipv4));
    m_config.setProperty("corba.endpoints_ipv6", coil::flatten(epstr_ipv6));
#endif
  }

  /*!
   * @if jp
   * @brief corba.endpoint_property からオプション情報を取得する
   * @else
   * @brief Getting option info from corba.endpoint_property
   * @endif
   */
  void Manager::endpointPropertySwitch(const std::string& ipver,
                                       bool& ip, std::vector<int>& ip_list)
  {
    ip = false; ip_list.resize(0);

    std::string ep_prop;
    ep_prop = m_config.getProperty("corba.endpoint_property", "ipv4");
    coil::toLower(ep_prop);

    std::string::size_type pos = ep_prop.find(ipver);
    if (pos == std::string::npos) { return; }

    ip = true;
    pos += ipver.size();
    if (pos >= ep_prop.size() || ep_prop[pos] != '(') { return; }
    std::string::size_type par_begin, par_end;
    par_begin = pos;
    ++pos;
    while (pos < ep_prop.size())
      {
        if (ep_prop[pos] == ')') { break; }
        ++pos;
      }
    par_end = pos;

    std::string list_num(ep_prop.substr(par_begin + 1, par_end - 1));
    coil::vstring nums = coil::split(list_num, ",");
    for (size_t i(0); i < nums.size(); ++i)
      {
        int n;
        if (coil::stringTo(n, nums[i].c_str()))
          {
            ip_list.push_back(n);
          }
      }
    return;
  }




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
  void Manager::initPreConnection()
  {
	  RTC_TRACE(("Connection pre-connection: %s",
		  m_config["manager.components.preconnect"].c_str()));
	  std::vector<std::string> connectors;
	  connectors = coil::split(m_config["manager.components.preconnect"], ",");
	  for (int i(0), len(connectors.size()); i < len; ++i)
	  {
		  
		  coil::eraseBothEndsBlank(connectors[i]);
		  if (connectors[i].empty())
		  {
			  continue;
		  }

		  std::string port0_str = coil::split(connectors[i], "?")[0];
		  coil::mapstring param = coil::urlparam2map(connectors[i]);

		  coil::vstring ports;
		  coil::mapstring configs;

		  for (coil::mapstring::iterator param_itr = param.begin(); param_itr != param.end(); ++param_itr) {
			  if (param_itr->first == "port")
			  {
				  ports.push_back(param_itr->second);
				  continue;
			  }
			  std::string tmp = param_itr->first;
			  coil::replaceString(tmp, "port", "");
              std::string::size_type pos = param_itr->first.find("port");

			  int val = 0;
              if (coil::stringTo<int>(val, tmp.c_str()) && pos != std::string::npos)
			  {
				  ports.push_back(param_itr->second);
				  continue;
			  }
			  configs[param_itr->first] = param_itr->second;
		  }


          /*
		  if (ports.size() == 0)
		  {
			  RTC_ERROR(("Invalid format for pre-connection."));
			  RTC_ERROR(("Format must be Comp0.port0?port=Comp1.port1"));
			  continue;
		  }
          */

		  if (configs.count("dataflow_type") == 0)
		  {
			  configs["dataflow_type"] = "push";
		  }
		  if (configs.count("interface_type") == 0)
		  {
			  configs["interface_type"] = "corba_cdr";
		  }


		  coil::vstring tmp = coil::split(port0_str, ".");
		  tmp.pop_back();
		  std::string comp0_name = coil::flatten(tmp, ".");
		  
		  std::string port0_name = port0_str;
		  RTObject_impl* comp0 = nullptr;
		  RTC::RTObject_ptr comp0_ref = nullptr;

		  if (comp0_name.find("://") == std::string::npos)
		  {
			  comp0 = getComponent(comp0_name.c_str());
			  if (comp0 == nullptr)
			  {
				  RTC_ERROR(("%s not found.", comp0_name.c_str()));
				  continue;
			  }
			  comp0_ref = comp0->getObjRef();
		  }
		  else
		  {
			  RTC::RTCList rtcs = m_namingManager->string_to_component(comp0_name);
			  if (rtcs.length() == 0)
			  {
				  RTC_ERROR(("%s not found.", comp0_name.c_str()));
				  continue;
			  }
			  comp0_ref = rtcs[0];
			  coil::vstring tmp_port0_name = coil::split(port0_str, "/");
			  port0_name = tmp_port0_name.back();


		  }

		  RTC::PortService_var port0_var = CORBA_RTCUtil::get_port_by_name(comp0_ref, port0_name);

		  if (CORBA::is_nil(port0_var))
		  {
			  RTC_DEBUG(("port %s found: ", port0_str.c_str()));
			  continue;
		  }

          if (ports.empty())
          {
              coil::Properties prop;

              for (coil::mapstring::iterator config_itr = configs.begin(); config_itr != configs.end(); ++config_itr) {
                  std::string key = config_itr->first;
                  std::string value = config_itr->second;
                  coil::eraseBothEndsBlank(key);
                  coil::eraseBothEndsBlank(value);

                  prop["dataport." + key] = value;
              }

              if (RTC::RTC_OK != CORBA_RTCUtil::connect(connectors[i], prop, port0_var, RTC::PortService::_nil()))
              {
                  RTC_ERROR(("Connection error: %s", connectors[i].c_str()));
              }
          }

		  for (coil::vstring::iterator port_itr = ports.begin(); port_itr != ports.end(); ++port_itr)
		  {

			  std::string port_str = (*port_itr);

			  tmp = coil::split(port_str, ".");
			  tmp.pop_back();
			  std::string comp_name = coil::flatten(tmp, ".");
			  std::string port_name = port_str;
			  RTObject_impl* comp = nullptr;
			  RTC::RTObject_ptr comp_ref = nullptr;


			  if (comp_name.find("://") == std::string::npos)
			  {
				  comp = getComponent(comp_name.c_str());
				  if (comp == nullptr)
				  {
					  RTC_ERROR(("%s not found.", comp_name.c_str()));
					  continue;
				  }
				  comp_ref = comp->getObjRef();
			  }
			  else
			  {
				  RTC::RTCList rtcs = m_namingManager->string_to_component(comp_name);
				  if (rtcs.length() == 0)
				  {
					  RTC_ERROR(("%s not found.", comp_name.c_str()));
					  continue;
				  }
				  comp_ref = rtcs[0];
				  coil::vstring tmp_port_name = coil::split(port_str, "/");
				  port_name = tmp_port_name.back();


			  }

			  RTC::PortService_var port_var = CORBA_RTCUtil::get_port_by_name(comp_ref, port_name);

			  if (CORBA::is_nil(port_var))
			  {
				  RTC_DEBUG(("port %s found: ", port_str.c_str()));
				  continue;
			  }

			  coil::Properties prop;

			  for (coil::mapstring::iterator config_itr = configs.begin(); config_itr != configs.end(); ++config_itr) {
				  std::string key = config_itr->first;
				  std::string value = config_itr->second;
				  coil::eraseBothEndsBlank(key);
				  coil::eraseBothEndsBlank(value);

				  prop["dataport." + key] = value;
			  }



			  if (RTC::RTC_OK != CORBA_RTCUtil::connect(connectors[i], prop, port0_var, port_var))
			  {
				  RTC_ERROR(("Connection error: %s", connectors[i].c_str()));
			  }
		  }
	  }
  }

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
  void Manager::initPreActivation()
  {
	  RTC_TRACE(("Components pre-activation: %s",
		  m_config["manager.components.preactivation"].c_str()));
	  std::vector<std::string> comps;
	  comps = coil::split(m_config["manager.components.preactivation"],
		  ",");
	  for (int i(0), len(comps.size()); i < len; ++i)
	  {
		  coil::eraseBothEndsBlank(comps[i]);
		  if (!comps[i].empty())
		  {
			  RTC::RTObject_ptr comp_ref;
			  if (comps[i].find("://") == std::string::npos)
			  {
				  RTObject_impl* comp = getComponent(comps[i].c_str());
				  if (comp == nullptr)
				  {
					  RTC_ERROR(("%s not found.", comps[i].c_str())); continue;
				  }
				  comp_ref = comp->getObjRef();
			  }
			  else
			  {
				  RTC::RTCList rtcs = m_namingManager->string_to_component(comps[i]);
				  if (rtcs.length() == 0)
				  {
					  RTC_ERROR(("%s not found.", comps[i].c_str()));
					  continue;
				  }
				  comp_ref = rtcs[0];
				  
			  }
			  
			  RTC::ReturnCode_t ret = CORBA_RTCUtil::activate(comp_ref);
			  if (ret != RTC::RTC_OK)
			  {
				  RTC_ERROR(("%s activation filed.", comps[i].c_str()));
			  }
			  else
			  {
				  RTC_INFO(("%s activated.", comps[i].c_str()));
			  }
		  }
		  

	  }
  }

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
  void Manager::initPreCreation()
  {
	  RTC_TRACE(("Components pre-creation: %s",
		  m_config["manager.components.precreate"].c_str()));
	  std::vector<std::string> comp;
	  comp = coil::split(m_config["manager.components.precreate"], ",");
	  for (int i(0), len(comp.size()); i < len; ++i)
	  {
		  this->createComponent(comp[i].c_str());
	  }
  }





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
  void Manager::invokeInitProc()
  {
	  if (m_initProc != nullptr)
	  {
		  m_initProc(this);
	  }
  }

  void Manager::publishPorts(RTObject_impl* comp)
  {
	  PortServiceList_var ports = comp->get_ports();
	  for (size_t i(0); i < ports->length(); ++i)
	  {
		  PortProfile_var prof = ports[i]->get_port_profile();
		  coil::Properties prop;
		  NVUtil::copyToProperties(prop, prof->properties);
		  if ((prop.hasKey("publish_topic") == nullptr ||
			  prop["publish_topic"] == "") &&
			  (prop.hasKey("subscribe_topic") == nullptr ||
			  prop["subscribe_topic"] == "") &&
			  (prop.hasKey("rendezvous_point") == nullptr ||
			  prop["rendezvous_point"] == "")) {
			  continue;
		  }

		  std::string name;
		  if (prop["port.port_type"] == "DataOutPort")
		  {
			  name = "dataports.port_cxt/";
			  name += prop["publish_topic"] + ".topic_cxt/";
			  name += prof->name; name += ".outport";
		  }
		  else if (prop["port.port_type"] == "DataInPort")
		  {
			  name = "dataports.port_cxt/";
			  name += prop["subscribe_topic"] + ".topic_cxt/";
			  name += prof->name; name += ".inport";
		  }
		  else if (prop["port.port_type"] == "CorbaPort")
		  {
			  name = "svcports.port_cxt/";
			  name += prop["rendezvous_point"] + ".svc_cxt/";
			  name += prof->name; name += ".svc";
		  }
		  else
		  {
			  RTC_WARN(("Unknown port type: %s", prop["port.port_type"].c_str()));
			  continue;
		  }
		  PortBase* port;
		  port = dynamic_cast<PortBase*>(m_pPOA->reference_to_servant(ports[i]));
		  m_namingManager->bindObject(name.c_str(), port);
	  }
  }
  void Manager::subscribePorts(RTObject_impl* comp)
  {
	  PortServiceList_var ports = comp->get_ports();
	  for (size_t i(0); i < ports->length(); ++i)
	  {
		  PortProfile_var prof = ports[i]->get_port_profile();
		  coil::Properties prop;
		  NVUtil::copyToProperties(prop, prof->properties);
		  if ((prop.hasKey("publish_topic") == nullptr ||
			  prop["publish_topic"] == "") &&
			  (prop.hasKey("subscribe_topic") == nullptr ||
			  prop["subscribe_topic"] == "") &&
			  (prop.hasKey("rendezvous_point") == nullptr ||
			  prop["rendezvous_point"] == "")) {
			  continue;
		  }

		  std::string name;
		  PortServiceList_var nsports;
		  if (prop["port.port_type"] == "DataOutPort")
		  {
			  name = "dataports.port_cxt/";
			  name += prop["publish_topic"] + ".topic_cxt";
			  nsports = getPortsOnNameServers(name, "inport");
			  connectDataPorts(ports[i], nsports);
		  }
		  else if (prop["port.port_type"] == "DataInPort")
		  {
			  name = "dataports.port_cxt/";
			  name += prop["subscribe_topic"] + ".topic_cxt";
			  nsports = getPortsOnNameServers(name, "outport");
			  connectDataPorts(ports[i], nsports);
		  }
		  else if (prop["port.port_type"] == "CorbaPort")
		  {
			  name = "svcports.port_cxt/";
			  name += prop["rendezvous_point"] + ".svc_cxt";
			  nsports = getPortsOnNameServers(name, "svc");
			  connectServicePorts(ports[i], nsports);
		  }
	  }
  }

  PortServiceList_var Manager::getPortsOnNameServers(const std::string& nsname,
	  const std::string& kind)
  {
	  PortServiceList_var ports = new PortServiceList();
	  std::vector<RTC::NamingService*>& ns(m_namingManager->getNameServices());
	  for (size_t i(0); i < ns.size(); ++i)
	  {
		  NamingOnCorba* noc = dynamic_cast<NamingOnCorba*>(ns[i]->ns);
		  if (noc == nullptr) { continue; }
		  CorbaNaming& cns(noc->getCorbaNaming());
		  CosNaming::BindingList_var bl = new CosNaming::BindingList();
		  cns.listByKind(nsname.c_str(), kind.c_str(), bl);

		  for (CORBA::ULong j(0); j < bl->length(); ++j)
		  {
			  if (bl[j].binding_type != CosNaming::nobject) { continue; }
			  std::string tmp(cns.toString(bl[j].binding_name));
			  std::string nspath;
			  nspath = "/" + nsname + "/" + tmp;
			  // ### TODO: All escape characteres should be removed. ###
			  coil::replaceString(nspath, "\\", "");
			  CORBA::Object_var obj = cns.resolveStr(nspath.c_str());
			  RTC::PortService_var portsvc = RTC::PortService::_narrow(obj);
			  if (CORBA::is_nil(portsvc)) { continue; }
			  try { PortProfile_var p = portsvc->get_port_profile(); }
			  catch (...) { continue; } // the port must be zombie
			  CORBA::ULong len(ports->length());
			  ports->length(len + 1);
			  ports[len] = portsvc;
		  }
	  }
	  return ports._retn();
  }

  void Manager::connectDataPorts(PortService_ptr port,
	  PortServiceList_var& target_ports)
  {
	  for (CORBA::ULong i(0); i < target_ports->length(); ++i)
	  {
		  if (port->_is_equivalent(target_ports[i])) { continue; }
		  if (CORBA_RTCUtil::already_connected(port, target_ports[i]))
		  {
			  continue;
		  }
		  std::string con_name;
		  PortProfile_var p0 = port->get_port_profile();
		  PortProfile_var p1 = target_ports[i]->get_port_profile();
		  con_name += p0->name;
		  con_name += ":";
		  con_name += p1->name;
		  coil::Properties prop;
		  if (RTC::RTC_OK !=
			  CORBA_RTCUtil::connect(con_name, prop, port, target_ports[i]))
		  {
			  RTC_ERROR(("Connection error in topic connection."));
		  }
	  }
  }

    void Manager::connectServicePorts(PortService_ptr port,
                                    PortServiceList_var& target_ports)
  {
    for (CORBA::ULong i(0); i < target_ports->length(); ++i)
      {
        if (port->_is_equivalent(target_ports[i])) { continue; }
        if (CORBA_RTCUtil::already_connected(port, target_ports[i]))
          { continue; }
        std::string con_name;
        PortProfile_var p0 = port->get_port_profile();
        PortProfile_var p1 = target_ports[i]->get_port_profile();
        con_name += p0->name;
        con_name += ":";
        con_name += p1->name;
        coil::Properties prop;
        if (RTC::RTC_OK !=
            CORBA_RTCUtil::connect(con_name, prop, port, target_ports[i]))
          {
            RTC_ERROR(("Connection error in topic connection."));
          }
      }
  }
  

}  // namespace RTC;
