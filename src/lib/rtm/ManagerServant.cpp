// -*- C++ -*-
/*!
 * @file ManagerServant.cpp
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
#include <coil/Process.h>
#include <coil/Properties.h>
#include <coil/stringutil.h>
#include <rtm/Manager.h>
#include <rtm/ManagerServant.h>
#include <rtm/NVUtil.h>
#include <rtm/RTObject.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/CORBA_IORUtil.h>

#include <vector>
#include <string>

namespace RTM
{
  //
  // Example implementational code for IDL interface RTM::Manager
  //
  ManagerServant::ManagerServant()
    : rtclog("ManagerServant"),
      m_mgr(::RTC::Manager::instance()),
      m_objref(RTM::Manager::_nil()),
      m_masters(0), m_slaves(0),
      m_isMaster(false)
  {
    rtclog.setName("ManagerServant");
    coil::Properties config(m_mgr.getConfig());    

    if (!createINSManager())
      {
        RTC_WARN(("Manager CORBA servant creation failed."));
        return;
      }
    RTC_INFO(("Named manager reference (INS) was successfully created."));

    if (coil::toBool(config["manager.is_master"], "YES", "NO", true))
      { // this is master manager
        RTC_TRACE(("This manager is master."));
        m_isMaster = true;
        RTC_INFO(("Master manager servant was successfully created."));
        return;
      }
    else
      { // manager is slave
        RTC_TRACE(("This manager is slave."));
        try
          {
            RTM::Manager_var owner;
            owner = findManager(config["corba.master_manager"]);
            if (CORBA::is_nil(owner))
              {
                RTC_WARN(("Master manager not found"));
                return;
              }
            RTC_INFO(("Master manager found: %s",
                      config["corba.master_manager"].c_str()));
            RTC_INFO(("Adding this manager to this manager."))
            add_master_manager(owner);
            RTC_INFO(("Register this manager to master manager."))
            owner->add_slave_manager(m_objref.in());
            RTC_INFO(("Slave manager servant was successfully created."));
            return;
          }
        catch (...)
          {
            RTC_ERROR(("Unknown exception cought."));
          }
      }
  }

  ManagerServant::~ManagerServant()
  {
    Guard guardm(m_masterMutex);
    for (CORBA::ULong i(0); i < m_masters.length(); ++i)
      {
        try
          {
            if (CORBA::is_nil(m_masters[i])) { continue; }
            m_masters[i]
              ->remove_slave_manager(RTM::Manager::_duplicate(m_objref));
          }
        catch (...)
          {
            m_masters[i] = RTM::Manager::_nil();
          }
      }
    m_masters.length(0);

    Guard guards(m_slaveMutex);
    for (CORBA::ULong i(0); i < m_slaves.length(); ++i)
      {
        try
          {
            if (CORBA::is_nil(m_slaves[i])) { continue; }
            m_slaves[i]
              ->remove_master_manager(RTM::Manager::_duplicate(m_objref));
          }
        catch (...)
          {
            m_slaves[i] = RTM::Manager::_nil();
          }
      }
    m_slaves.length(0);


    if (!CORBA::is_nil(m_objref))
      {
#ifndef ORB_IS_TAO
#ifndef ORB_IS_RTORB
        CORBA::Object_ptr obj = m_mgr.theORB()->resolve_initial_references("omniINSPOA");
#else // ROB_IS_RTORB
        CORBA::Object_ptr obj = m_mgr.theORB()->resolve_initial_references((char*)"omniINSPOA");
#endif // ORB_IS_RTORB


        PortableServer::POA_ptr poa = PortableServer::POA::_narrow(obj);
        PortableServer::ObjectId_var id;
        id = poa->servant_to_id(this);

        poa->deactivate_object(id.in());
#else
        CORBA::Object_var obj = m_mgr.theORB()->resolve_initial_references("IORTable");
        IORTable::Table_var adapter = IORTable::Table::_narrow(obj.in());


        coil::Properties config(m_mgr.getConfig());

        adapter->unbind(config["manager.name"].c_str());
#endif
      }

  }

  /*!
   * @if jp
   * @brief モジュールをロードする
   * @else
   * @brief Loading a module
   * @endig
   */
  RTC::ReturnCode_t
  ManagerServant::load_module(const char* pathname, const char* initfunc)
  {
    RTC_TRACE(("ManagerServant::load_module(%s, %s)", pathname, initfunc));

    m_mgr.load(pathname, initfunc);

    return ::RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief モジュールをアンロードする
   * @else
   * @brief Unloading a module
   * @endig
   */
  RTC::ReturnCode_t ManagerServant::unload_module(const char* pathname)
  {
    RTC_TRACE(("ManagerServant::unload_module(%s)", pathname));

    m_mgr.unload(pathname);

    return ::RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief ロード可能なモジュールのプロファイルを取得する
   * @else
   * @brief Getting loadable module profiles
   * @endig
   */
  RTM::ModuleProfileList* ManagerServant::get_loadable_modules()
  {
    RTC_TRACE(("get_loadable_modules()"));

    // copy local module profiles
    ::RTM::ModuleProfileList_var cprof = new ::RTM::ModuleProfileList();
    std::vector<coil::Properties> prof(m_mgr.getLoadableModules());

    cprof->length(static_cast<CORBA::Long>(prof.size()));
    for (size_t i(0), len(prof.size()); i < len; ++i)
      {
        RTC_VERBOSE_STR((prof[i]));
#ifndef ORB_IS_TAO
        NVUtil::copyFromProperties(cprof[static_cast<CORBA::Long>(i)].properties, prof[i]);
#else
        NVUtil::copyFromProperties(cprof.inout()[(CORBA::Long)i].properties, prof[i]);
#endif
      }

    if (false)
      {
        // copy slaves' module profiles
        Guard gurad(m_slaveMutex);
        RTC_DEBUG(("%d slaves exists.", m_slaves.length()));
        for (int i(0), len(m_slaves.length()); i < len; ++i)
          {
            try
              {
                if (!CORBA::is_nil(m_slaves[i]))
                  {
                    ::RTM::ModuleProfileList_var sprof;
                    sprof = m_slaves[i]->get_loadable_modules();
#ifndef ORB_IS_RTORB
                    CORBA_SeqUtil::push_back_list(cprof.inout(), sprof.in());
#else  // ORB_IS_RTORB
                    CORBA_SeqUtil::push_back_list(cprof, sprof);
#endif  // ORB_IS_RTORB
                    continue;
                  }
              }
            catch (...)
              {
                RTC_INFO(("slave (%d) has disappeared.", i));
                m_slaves[i] = RTM::Manager::_nil();
              }
            CORBA_SeqUtil::erase(m_slaves, i); --i;
          }
      }
    return cprof._retn();
  }

  /*!
   * @if jp
   * @brief ロード済みのモジュールのプロファイルを取得する
   * @else
   * @brief Getting loaded module profiles
   * @endig
   */
  RTM::ModuleProfileList* ManagerServant::get_loaded_modules()
  {
    RTC_TRACE(("get_loaded_modules()"));

    // copy local module profiles
    ::RTM::ModuleProfileList_var cprof = new RTM::ModuleProfileList();
    std::vector<coil::Properties> prof(m_mgr.getLoadedModules());

    cprof->length(static_cast<CORBA::Long>(prof.size()));
    for (size_t i(0), len(prof.size()); i < len; ++i)
      {
        RTC_VERBOSE_STR((prof[i]));
#ifndef ORB_IS_TAO
        NVUtil::copyFromProperties(cprof[static_cast<CORBA::Long>(i)].properties, prof[i]);
#else
        NVUtil::copyFromProperties(cprof.inout()[(CORBA::Long)i].properties, prof[i]);
#endif
      }

    if (false)
      {
        // copy slaves' module profile
        Guard guard(m_slaveMutex);
        RTC_DEBUG(("%d slave managers exists.", m_slaves.length()));
        for (int i(0), len(m_slaves.length()); i < len; ++i)
          {
            try
              {
                if (!CORBA::is_nil(m_slaves[i]))
                  {
                    ::RTM::ModuleProfileList_var sprof;
                    sprof = m_slaves[i]->get_loaded_modules();
#ifndef ORB_IS_RTORB
                    CORBA_SeqUtil::push_back_list(cprof.inout(), sprof.in());
#else  // ORB_IS_RTORB
                    CORBA_SeqUtil::push_back_list(cprof, sprof);
#endif  // ORB_IS_RTORB
                    continue;
                  }
              }
            catch (...)
              {
                RTC_INFO(("slave (%d) has disappeared.", i));
                m_slaves[i] = RTM::Manager::_nil();
              }
            CORBA_SeqUtil::erase(m_slaves, i); --i;
          }
      }
    return cprof._retn();
  }

  /*!
   * @if jp
   * @brief コンポーネントファクトリのプロファイルを取得する
   * @else
   * @brief Getting component factory profiles
   * @endig
   */
  RTM::ModuleProfileList* ManagerServant::get_factory_profiles()
  {
    RTC_TRACE(("get_factory_profiles()"));

    // copy local factory profiles
    ::RTM::ModuleProfileList_var cprof = new RTM::ModuleProfileList();
    std::vector<coil::Properties> prof(m_mgr.getFactoryProfiles());

    cprof->length(static_cast<CORBA::Long>(prof.size()));
    for (size_t i(0), len(prof.size()); i < len; ++i)
      {
        RTC_VERBOSE_STR((prof[i]));
#ifndef ORB_IS_TAO
        NVUtil::copyFromProperties(cprof[static_cast<CORBA::Long>(i)].properties, prof[i]);
#else
        NVUtil::copyFromProperties(cprof.inout()[(CORBA::Long)i].properties, prof[i]);
#endif
      }

    if (false)
      {
        // copy slaves' factory profile
        Guard guard(m_slaveMutex);
        RTC_DEBUG(("%d slave managers exists.", m_slaves.length()));
        for (int i(0), len(m_slaves.length()); i < len; ++i)
          {
            try
              {
                if (!CORBA::is_nil(m_slaves[i]))
                  {
                    ::RTM::ModuleProfileList_var sprof;
                    sprof = m_slaves[i]->get_factory_profiles();
#ifndef ORB_IS_RTORB
                    CORBA_SeqUtil::push_back_list(cprof.inout(), sprof.in());
#else  // ORB_IS_RTORB
                    CORBA_SeqUtil::push_back_list(cprof, sprof);
#endif  // ORB_IS_RTORB
                    continue;
                  }
              }
            catch (...)
              {
                RTC_INFO(("slave (%d) has disappeared.", i));
                m_slaves[i] = RTM::Manager::_nil();
              }
            CORBA_SeqUtil::erase(m_slaves, i); --i;
          }
      }
    return cprof._retn();
  }

  std::string ManagerServant::getParameterByModulename(const std::string param_name, std::string &module_name)
  {
        size_t pos0 = module_name.find("&" + param_name + "=");
        size_t pos1 = module_name.find("?" + param_name + "=");

        if (pos0 == std::string::npos && pos1 == std::string::npos)
          {
            return "";
          }

        size_t pos = 0;
        if (pos0 == std::string::npos)
          {
            pos = pos1;
          }
        else
          {
            pos = pos0;
        }

        std::string paramstr;
        size_t endpos = module_name.find('&', pos + 1);


        if (endpos == std::string::npos)
          {
            endpos = module_name.find('?', pos + 1);

            if (endpos == std::string::npos)
              {
                paramstr = module_name.substr((pos + 1));
                
              }
            else
              {
                paramstr = module_name.substr((pos + 1), endpos);
              }
          }
        else
          {
            paramstr = module_name.substr((pos + 1), endpos);
          }
        RTC_VERBOSE(("%s arg: %s", param_name.c_str(), paramstr.c_str()));
        
        size_t eqpos = paramstr.find('=');

        paramstr = paramstr.substr(eqpos + 1);


        RTC_DEBUG(("%s is %s", param_name.c_str(), paramstr.c_str()));

        if (endpos == std::string::npos)
          {
            module_name = module_name.substr(0, pos);
          }
        else
          {
            module_name = module_name.substr(0, pos) + module_name.substr(endpos);
          }

        return paramstr;
  }
  
  /*!
   * @if jp
   * @brief コンポーネントを生成する
   * @else
   * @brief Creating an RT-Component
   * @endig
   */
  RTC::RTObject_ptr ManagerServant::create_component(const char* module_name)
  {
    RTC_TRACE(("create_component(%s)", module_name));
    RTC_TRACE(("This manager is master: %s", m_isMaster ? "YES" : "NO"));
    std::string create_arg(module_name);
    if (create_arg.empty()) // invalid arg
      {
        return RTC::RTObject::_nil();
      }
    coil::vstring tmp = coil::split(create_arg, "&");
    if (tmp.back().empty())
      {
        create_arg = create_arg.substr(0, create_arg.size() - 1);
      }
    tmp = coil::split(create_arg, "?");
    if (tmp.back().empty())
      {
        create_arg = create_arg.substr(0,create_arg.size() - 1);
      }

    RTC::RTObject_var rtobj;
    // create component by address
    rtobj = createComponentByAddress(create_arg);
    if (!CORBA::is_nil(rtobj)) { return rtobj._retn(); }

    // create component by manager's name
    rtobj = createComponentByManagerName(create_arg);
    if (!CORBA::is_nil(rtobj)) { return rtobj._retn(); }


    getParameterByModulename("manager_address", create_arg);
    std::string manager_name = getParameterByModulename("manager_name", create_arg);


    CompParam comp_param(create_arg);

    // === Master ===
    // The aster manager never hosts RT-Components and it request to
    // create to slave components.
    if (m_isMaster)
      {
          {
            Guard guard(m_slaveMutex);
            for (CORBA::ULong i(0); i < m_slaves.length(); ++i)
              {
                try
                  {
                    RTM::NVList* prof = m_slaves[i]->get_configuration();
                    coil::Properties prop;
                    NVUtil::copyToProperties(prop, (*prof));
                    std::string slave_lang = prop["manager.language"];

                    if (slave_lang == comp_param.language())
                      {
                        RTC::RTObject_var comp = m_slaves[i]->create_component(create_arg.c_str());
                        RTC_DEBUG(("Component created %s", create_arg.c_str()));
                        if (!CORBA::is_nil(comp)) { return comp._retn(); }
                      }


                  }
                catch (CORBA::SystemException&)
                  {
                    RTC_DEBUG(("Exception was caught while creating component."));
                  }
              }
          }

        if (manager_name.empty())
          {
            create_arg = create_arg + "&manager_name=manager_%p";
            rtobj = createComponentByManagerName(create_arg);
            if (!CORBA::is_nil(rtobj)) { return rtobj._retn(); };
          }
        return RTC::RTObject::_nil();
      }
    else
      {
        // === Slave ===
        // create component on the manager    
        RTC_DEBUG(("Creating component on this manager"));
        RTC_DEBUG(("arg: %s", create_arg.c_str()));
        RTC::RTObject_impl* rtc = m_mgr.createComponent(create_arg.c_str());
        if (rtc != nullptr)
          {
            return RTC::RTObject::_duplicate(rtc->getObjRef());
          }
      }

    // no RTObject created.
    return RTC::RTObject::_nil();
  }

  /*!
   * @if jp
   * @brief コンポーネントを削除する
   * @else
   * @brief Deleting an RT-Component
   * @endig
   */
  RTC::ReturnCode_t ManagerServant::delete_component(const char* instance_name)
  {
    RTC_TRACE(("delete_component(%s)", instance_name));

    RTC::RTObject_impl* comp = m_mgr.getComponent(instance_name);
    if (comp == nullptr)
      {
        RTC_WARN(("No such component exists: %s", instance_name));
        return RTC::BAD_PARAMETER;
      }
    try
      {
        comp->exit();
      }
    catch (...)
      { // never come here
        RTC_ERROR(("Unknown exception was raised, when RTC was finalized."));
        return RTC::RTC_ERROR;
      }
    return ::RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief 起動中のコンポーネントのリストを取得する
   * @else
   * @brief Getting RT-Component list running on this manager
   * @endig
   */
  RTC::RTCList* ManagerServant::get_components()
  {
    RTC_TRACE(("get_components()"));

    // get local component references
    std::vector<RTC::RTObject_impl*> rtcs = m_mgr.getComponents();
    ::RTC::RTCList_var crtcs = new ::RTC::RTCList();

    crtcs->length(static_cast<CORBA::Long>(rtcs.size()));
    for (size_t i(0), len(rtcs.size()); i < len; ++i)
      {
#ifndef ORB_IS_TAO
        crtcs[static_cast<CORBA::Long>(i)] = RTC::RTObject::_duplicate(rtcs[i]->getObjRef());
#else
        crtcs.inout()[(CORBA::Long)i] = RTC::RTObject::_duplicate(rtcs[i]->getObjRef());
#endif
      }

    // get slaves' component references
    Guard guard(m_slaveMutex);
    RTC_DEBUG(("%d slave managers exists.", m_slaves.length()));
    for (int i(0), len(m_slaves.length()); i < len; ++i)
      {
        try
          {
            if (!CORBA::is_nil(m_slaves[i]))
              {
                ::RTC::RTCList_var srtcs;
                srtcs = m_slaves[i]->get_components();
#ifndef ORB_IS_RTORB
                CORBA_SeqUtil::push_back_list(crtcs.inout(), srtcs.in());
#else  // ORB_IS_RTORB
                CORBA_SeqUtil::push_back_list(srtcs, crtcs);
#endif  // ORB_IS_RTORB
                continue;
              }
          }
        catch (...)
          {
            RTC_INFO(("slave (%d) has disappeared.", i));
            m_slaves[i] = RTM::Manager::_nil();
          }
        CORBA_SeqUtil::erase(m_slaves, i); --i;
      }
    return crtcs._retn();
  }

  /*!
   * @if jp
   * @brief 起動中のコンポーネントプロファイルのリストを取得する
   * @else
   * @brief Getting RT-Component's profile list running on this manager
   * @endig
   */ 
  RTC::ComponentProfileList* ManagerServant::get_component_profiles()
  {
    RTC_TRACE(("get_component_profiles()"));

    // copy local component profiles
    ::RTC::ComponentProfileList_var cprofs = new ::RTC::ComponentProfileList();
    std::vector<RTC::RTObject_impl*> rtcs = m_mgr.getComponents();
    cprofs->length(static_cast<CORBA::Long>(rtcs.size()));
    for (size_t i(0), len(rtcs.size()); i < len; ++i)
      {
        ::RTC::ComponentProfile_var prof = rtcs[i]->get_component_profile();
#ifndef ORB_IS_TAO
        cprofs[static_cast<CORBA::Long>(i)] = prof;
#else
        cprofs.inout()[(CORBA::Long)i] = prof;
#endif
      }

    // copy slaves' component profiles
    Guard guard(m_slaveMutex);
    RTC_DEBUG(("%d slave managers exists.", m_slaves.length()));
    for (int i(0), len(m_slaves.length()); i < len; ++i)
      {
        try
          {
            if (!CORBA::is_nil(m_slaves[i]))
              {
                ::RTC::ComponentProfileList_var sprofs;
                sprofs = m_slaves[i]->get_component_profiles();
#ifndef ORB_IS_RTORB
                CORBA_SeqUtil::push_back_list(cprofs.inout(), sprofs.in());
#else  // ORB_IS_RTORB
                CORBA_SeqUtil::push_back_list(cprofs, sprofs);
#endif  // ORB_IS_RTORB
                continue;
              }
          }
        catch (...)
          {
            RTC_INFO(("slave (%d) has disappeared.", i));
            m_slaves[i] = RTM::Manager::_nil();
          }
        CORBA_SeqUtil::erase(m_slaves, i); --i;
      }
    return cprofs._retn();
  }

  // manager 基本
  /*!
   * @if jp
   * @brief マネージャのプロファイルを取得する
   * @else
   * @brief Getting this manager's profile.
   * @endig
   */
  RTM::ManagerProfile* ManagerServant::get_profile()
  {
    RTC_TRACE(("get_profile()"));
    RTM::ManagerProfile* prof = new RTM::ManagerProfile();
    NVUtil::copyFromProperties(prof->properties,
                               m_mgr.getConfig().getNode("manager"));
    return prof;
  }

  /*!
   * @if jp
   * @brief マネージャのコンフィギュレーションを取得する
   * @else
   * @brief Getting this manager's configuration.
   * @endig
   */
  RTM::NVList* ManagerServant::get_configuration()
  {
    RTC_TRACE(("get_configuration()"));
#ifndef ORB_IS_RTORB
    ::RTC::NVList* nvlist = new ::RTC::NVList();
    NVUtil::copyFromProperties(*nvlist, m_mgr.getConfig());
#else
    ::RTC::NVList* nvlist;
    RTC_NVList nvlist_in;
    NVUtil::copyFromProperties(nvlist_in, m_mgr.getConfig());
    nvlist = new ::RTC::NVList(nvlist_in);
#endif
    return nvlist;
  }

  /*!
   * @if jp
   * @brief マネージャのコンフィギュレーションを設定する
   * @else
   * @brief Setting manager's configuration
   * @endig
   */
  RTC::ReturnCode_t
  ManagerServant::set_configuration(const char* name, const char* value)
  {
    RTC_TRACE(("set_configuration(name = %s, value = %s)", name, value));
    m_mgr.getConfig().setProperty(name, value);
    return ::RTC::RTC_OK;
  }
  /*!
   * @if jp
   * @brief マネージャがマスターかどうか
   * @else
   * @brief Whether this manager is master or not
   * @endig
   */
  CORBA::Boolean ManagerServant::is_master()
  {
    RTC_TRACE(("is_master(): %s", m_isMaster ? "YES" : "NO"));
    return m_isMaster;
  }

  /*!
   * @if jp
   * @brief マスターマネージャの取得
   * @else
   * @brief Getting master managers
   * @endig
   */
  RTM::ManagerList* ManagerServant::get_master_managers()
  {
    RTC_TRACE(("get_master_managers()"));
    Guard guard(m_masterMutex);
    return new ManagerList(m_masters);
  }

  /*!
   * @if jp
   * @brief マスターマネージャの追加
   * @else
   * @brief Getting a master manager
   * @endig
   */
  RTC::ReturnCode_t ManagerServant::add_master_manager(RTM::Manager_ptr mgr)
  {
    Guard guard(m_masterMutex);
    CORBA::Long index;
    RTC_TRACE(("add_master_manager(), %d masters", m_masters.length()));
    index = CORBA_SeqUtil::find(m_masters, is_equiv(mgr));

    if (!(index < 0))  // found in my list
      {
        RTC_ERROR(("Already exists."));
        return RTC::BAD_PARAMETER;
      }

    CORBA_SeqUtil::push_back(m_masters, RTM::Manager::_duplicate(mgr));
    RTC_TRACE(("add_master_manager() done, %d masters", m_masters.length()));
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief マスターマネージャの削除
   * @else
   * @brief Removing a master manager
   * @endig
   */
  RTC::ReturnCode_t
  ManagerServant::remove_master_manager(RTM::Manager_ptr mgr)
  {
    Guard guard(m_masterMutex);
    RTC_TRACE(("remove_master_manager(), %d masters", m_masters.length()));

    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_masters, is_equiv(mgr));

    if (index < 0)  // not found in my list
      {
        RTC_ERROR(("Not found."));
        return RTC::BAD_PARAMETER;
      }

    CORBA_SeqUtil::erase(m_masters, index);
    RTC_TRACE(("remove_master_manager() done, %d masters", m_masters.length()));
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief スレーブマネージャの取得
   * @else
   * @brief Getting slave managers
   * @endig
   */
  ManagerList* ManagerServant::get_slave_managers()
  {
    Guard guard(m_slaveMutex);
    RTC_TRACE(("get_slave_managers(), %d slaves", m_slaves.length()));

    return new ManagerList(m_slaves);
  }

  /*!
   * @if jp
   * @brief スレーブマネージャの追加
   * @else
   * @brief Getting a slave manager
   * @endig
   */
  RTC::ReturnCode_t ManagerServant::add_slave_manager(RTM::Manager_ptr mgr)
  {
    Guard guard(m_slaveMutex);
    RTC_TRACE(("add_slave_manager(), %d slaves", m_slaves.length()));

    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_slaves, is_equiv(mgr));

    if (!(index < 0))  // found in my list
      {
        RTC_ERROR(("Already exists."));
        return RTC::BAD_PARAMETER;
      }

    CORBA_SeqUtil::push_back(m_slaves, RTM::Manager::_duplicate(mgr));
    RTC_TRACE(("add_slave_manager() done, %d slaves", m_slaves.length()));
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief スレーブマネージャの削除
   * @else
   * @brief Removing a slave manager
   * @endig
   */
  RTC::ReturnCode_t ManagerServant::remove_slave_manager(RTM::Manager_ptr mgr)
  {
    Guard guard(m_slaveMutex);
    RTC_TRACE(("remove_slave_manager(), %d slaves", m_slaves.length()));
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_slaves, is_equiv(mgr));

    if (index < 0)  // not found in my list
      {
        RTC_ERROR(("Not found."));
        return RTC::BAD_PARAMETER;
      }

    CORBA_SeqUtil::erase(m_slaves, index);
    RTC_TRACE(("remove_slave_manager() done, %d slaves", m_slaves.length()));
    return RTC::RTC_OK;
  }
  
  /*!
   * @if jp
   * @brief プロセスのコピーを生成する
   * @else
   * @brief The copy of the process is generated.
   * @endif
   */
  RTC::ReturnCode_t ManagerServant::fork()
  {
    //    m_mgr.fork();
    
    return ::RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief shutdownする
   * @else
   * @brief This method shutdowns RTC.
   * @endif
   */
  RTC::ReturnCode_t ManagerServant::shutdown()
  {
    if (!m_isMaster)
      {
        Guard guardm(m_masterMutex);
        for (CORBA::ULong i(0); i < m_masters.length(); ++i)
          {
            try
              {
                if (CORBA::is_nil(m_masters[i])) { continue; }
                m_masters[i]
                  ->remove_slave_manager(RTM::Manager::_duplicate(m_objref));
              }
            catch (...)
              {
                m_masters[i] = RTM::Manager::_nil();
              }
          }
        m_masters.length(0);
      }
    else
      {
        Guard guards(m_slaveMutex);
        for (CORBA::ULong i(0); i < m_slaves.length(); ++i)
          {
            try
              {
                if (CORBA::is_nil(m_slaves[i])) { continue; }
                m_slaves[i]
                 ->remove_master_manager(RTM::Manager::_duplicate(m_objref));
              }
            catch (...)
              {
                m_slaves[i] = RTM::Manager::_nil();
              }
          }
        m_slaves.length(0);
      }
    m_mgr.terminate();
    return ::RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief 再起動する。
   * @else
   * @brief This method restarts RTC.
   * @endif
   */
  RTC::ReturnCode_t ManagerServant::restart()
  {
    //    m_mgr.restart();
    return ::RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief 指定名のRTCオブジェクトリファレンスを取得
   * @else
   * @brief Obtain specific RTObject reference by name
   * @endig
   */
  RTC::RTCList* ManagerServant::get_components_by_name(const char* name)
  {
    RTC_TRACE(("get_components_by_name()"));
    ::RTC::RTCList_var crtcs = new ::RTC::RTCList();

    // check argument
    std::string tmp(name);
    coil::eraseHeadBlank(tmp);
    if (tmp.empty()) { return crtcs._retn(); }

    std::vector<RTC::RTObject_impl*> rtcs = m_mgr.getComponents();
    coil::vstring rtc_name = coil::split(tmp, "/");
    for (auto & rtc : rtcs)
      {
        // name = ConsoleIn0, instancename = ConsoleIn0
        if (rtc_name.size() == 1 &&
            rtc_name[0] == rtc->getInstanceName())
          {
            RTC::RTObject_var rtcref =
              RTC::RTObject::_duplicate(rtc->getObjRef());
#ifndef ORB_IS_RTORB
            CORBA_SeqUtil::push_back(crtcs.inout(), rtcref.in());
#else // ORB_IS_RTORB
            CORBA_SeqUtil::push_back(crtcs, rtcref);
#endif // ORB_IS_RTORB
            continue;
          }
        if (rtc_name.size() < 2) { continue; } // size != 0

        // name = */ConsoleIn0 instancename = ConsoleIn0   OR
        // naem = sample/ConsoleIn0 category = sample && instance == ConsoleIn0
        if ((rtc_name[0] == "*" &&
             rtc_name[1] == rtc->getInstanceName()) ||
            (rtc_name[0] == rtc->getCategory() &&
             rtc_name[1] == rtc->getInstanceName()))
          {
            RTC::RTObject_var rtcref =
              RTC::RTObject::_duplicate(rtc->getObjRef());
#ifndef ORB_IS_RTORB
            CORBA_SeqUtil::push_back(crtcs.inout(), rtcref.in());
#else // ORB_IS_RTORB
            CORBA_SeqUtil::push_back(crtcs, rtcref);
#endif // ORB_IS_RTORB
          }
      }
    return crtcs._retn();
  }

  /*!
   * @if jp
   * @brief RTCのリファレンスを取得する。
   * @else
   * @brief Get the reference of RTC.
   * @endif
   */
  CORBA::Object_ptr ManagerServant::get_service(const char*  /*name*/)
  {
    return ::CORBA::Object::_nil();
  }

  //======================================================================
  // Local functions
  /*!
   * @if jp
   * @brief Managerのリファレンスを取得する。
   * @else
   * @brief Get the reference of Manager. 
   * @endif
   */ 
  RTM::Manager_ptr ManagerServant::getObjRef() const
  {
#ifdef ORB_IS_ORBEXPRESS
    return m_objref.in();
#else
    return m_objref;
#endif
  }

  /*!
   * @if jp
   * @brief INSManagerの生成
   * @else
   * @brief Generate INSManager. 
   * @endif
   */
  bool ManagerServant::createINSManager()
  {
    try
      {
        // Ppreparing INS POA
        CORBA::Object_var obj;
#ifndef ORB_IS_TAO
#ifndef ORB_IS_RTORB
        obj = m_mgr.theORB()->resolve_initial_references("omniINSPOA");
#else  // ROB_IS_RTORB
        obj = m_mgr.theORB()->resolve_initial_references(
                                      const_cast<char*>("omniINSPOA"));
#endif  // ORB_IS_RTORB
        PortableServer::POA_ptr poa = PortableServer::POA::_narrow(obj);
        poa->the_POAManager()->activate();

        // Create readable object ID
        coil::Properties config(m_mgr.getConfig());
        PortableServer::ObjectId_var id;
        RTC_DEBUG(("Creating named manager: %s",
                   config["manager.name"].c_str()));
#ifndef ORB_IS_RTORB
        id = PortableServer::string_to_ObjectId(config["manager.name"].c_str());
#else  // ORB_IS_RTORB
        id = PortableServer::string_to_ObjectId(
                     reinterpret_cast<char*>(config["manager.name"].c_str()));
#endif  // ORB_IS_RTORB

        // Object activation
        RTC_DEBUG(("Activating manager with id(%s)", config["manager.name"].c_str()));
        poa->activate_object_with_id(id.in(), this);
        CORBA::Object_var mgrobj = poa->id_to_reference(id);

        // Set m_objref
        m_objref = ::RTM::Manager::_narrow(mgrobj);

        CORBA::String_var ior;
        ior = m_mgr.theORB()->
          object_to_string(RTM::Manager::_duplicate(m_objref));
        std::string iorstr((const char*)ior);
        RTC_DEBUG(("Manager's IOR information:\n %s",
                   CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
#else
        obj = m_mgr.theORB()->resolve_initial_references("IORTable");
        IORTable::Table_var adapter = IORTable::Table::_narrow(obj.in());

        // Create readable object ID
        coil::Properties config(m_mgr.getConfig());

        RTC_DEBUG(("Creating named manager: %s",
                    config["manager.name"].c_str()));


        // Object activation
        RTC_DEBUG(("Activating manager with id(%s)", config["manager.name"].c_str()));
        CORBA::String_var ior = m_mgr.theORB()->object_to_string(_this());
        adapter->bind(config["manager.name"].c_str(), ior.in());

        // Set m_objref 
        m_objref = _this();


        std::string iorstr((const char*)ior);
        RTC_DEBUG(("Manager's IOR information:\n %s",
                    CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
#endif
      }
    catch (...)
      {
        return false;
      }
    return true;
  }

  /*!
   * @if jp
   * @brief Managerのリファレンスを検索する。
   * @else
   * @brief Find the reference of Manager. 
   * @endif
   */
  RTM::Manager_ptr ManagerServant::findManager(const std::string& host_port)
  {
    RTC_TRACE(("findManager(host_port = %s)", host_port.c_str()));
    try
      {
        coil::Properties config(m_mgr.getConfig());
        // Why RtORB does not allow corbaloc:iiop: ?
        //        std::string mgrloc("corbaloc:iiop:");
        std::string mgrloc("corbaloc::");
        mgrloc += host_port;
        mgrloc += "/" + config["manager.name"];

        RTC_DEBUG(("corbaloc: %s", mgrloc.c_str()));

        CORBA::Object_var mobj;
        mobj = m_mgr.theORB()->string_to_object(mgrloc.c_str());
#ifndef ORB_IS_RTORB
        RTM::Manager_var mgr = ::RTM::Manager::_narrow(mobj);
#else  // ORB_IS_RTORB
        RTM::Manager_var mgr;
        if (!make_client_connection(mobj->impl()->connection))
          {
            return RTM::Manager_ptr();
          }
        else
          {
            mgr = ::RTM::Manager::_narrow(mobj);
          }
#endif  // ORB_IS_RTORB

        CORBA::String_var ior;
        ior = m_mgr.theORB()->object_to_string(RTM::Manager::_duplicate(mgr));
        std::string iorstr((const char*)ior);
        RTC_DEBUG(("Manager's IOR information:\n %s",
                   CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));

        return mgr._retn();
      }
    catch(CORBA::SystemException& e)
      {
#ifndef ORB_IS_RTORB
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_DEBUG(("CORBA SystemException cought (CORBA::%s)", e._name()));
#endif
#endif // ORB_IS_RTORB
      }
    catch (...)
      {
        RTC_ERROR(("Unknown exception cought."));
      }
    return RTM::Manager::_nil();
  }

  /*!
   * @if jp
   * @brief 指定名のマネージャを取得
   * @else
   * @brief Obtain Manager's reference by name
   * @endif
   */
  RTM::Manager_ptr ManagerServant::findManagerByName(const std::string mgr_name)
  {
    RTC_TRACE(("findManagerByName(%s)", mgr_name.c_str()));

    coil::Properties prop = m_mgr.getConfig();
    if (mgr_name == prop["manager.instance_name"])
      {
        return RTM::Manager::_duplicate(getObjRef());
      }
    if (m_isMaster)
      {
        Guard guard(m_slaveMutex);
        return findManagerFromList(mgr_name, m_slaves);
      }
    Guard guard(m_masterMutex);
    return findManagerFromList(mgr_name, m_masters);
  }
  
  /*!
   * @if jp
   * @brief マネージャのリストから指定名を持つマネージャを取得
   * @else
   * @brief Obtain Manager's reference by name
   * @endif
   */
  RTM::Manager_ptr ManagerServant::findManagerFromList(const std::string name,
                                                       RTM::ManagerList& mlist)
  {
    for (CORBA::ULong i(0); i < mlist.length(); ++i)
      {
        try
          {
            if (CORBA::is_nil(mlist[i])) { continue; }
            RTM::NVList_var nvlist = mlist[i]->get_configuration();
            if (NVUtil::isStringValue(nvlist.in(),
                                      "manager.instance_name",
                                      name.c_str()))
              {
                return RTM::Manager::_duplicate(mlist[i]); // end condition
              }
            RTM::ManagerList_var slist = mlist[i]->get_slave_managers();
            if (slist->length() == 0) { continue; }
            RTM::Manager_var mgr = findManagerFromList(name, slist.inout());
            if (CORBA::is_nil(mgr)) { continue; }
            return mgr._retn(); // backtrack
          }
        catch (...)
          {
            RTC_ERROR(("A slave manager thrown exception."));
            mlist[i] = RTM::Manager::_nil();
            CORBA_SeqUtil::erase(mlist, i);
            --i; // list length is shortened
            RTC_ERROR(("This slave manager is removed from slave list."));
          }
      }
    return RTM::Manager::_nil();
  }

  /*!
   * @if jp
   * @brief 指定のマネージャでRTCを起動する
   * @else
   * @brief Launch RTC on specified manager
   * @endif
   */
  RTC::RTObject_ptr
  ManagerServant::createComponentByManagerName(const std::string create_arg)
  {
    RTC_TRACE(("createComponentByManagerName(%s)",create_arg.c_str()));
    coil::mapstring param = coil::urlparam2map(create_arg);
    for(auto & p : param)
      {
        RTC_DEBUG(("create_arg[%s] = %s",
                   p.first.c_str(), p.second.c_str()));
      }

    std::string mgrstr = param["manager_name"];
    if (mgrstr.empty())
      {
        RTC_WARN(("No manager_name found: %s", mgrstr.c_str()));
        return RTC::RTObject::_nil();
      }


    CompParam comp_param(create_arg);

    // detect languange
    std::string lang = comp_param.language();
    if (lang.empty()) { lang = "C++"; }
    RTC_INFO(("Specified manager's language: %s", lang.c_str()));

    RTM::Manager_var mgrobj = findManagerByName(mgrstr);
    if (CORBA::is_nil(mgrobj))
      {
        RTC_INFO(("Manager: %s not found.", mgrstr.c_str()));
        RTC_INFO(("Creating new manager named %s", mgrstr.c_str()));

        std::string rtcd_cmd_key("manager.modules.");
        rtcd_cmd_key += lang + ".manager_cmd";
        coil::Properties& prop = m_mgr.getConfig();
        std::string rtcd_cmd = prop[rtcd_cmd_key];

        if (rtcd_cmd.empty())
          {
            RTC_WARN(("rtcd command name not found. Default rtcd is used"));
            rtcd_cmd = "rtcd";
          }
        rtcd_cmd += " -o manager.is_master:NO";
        rtcd_cmd += " -o manager.corba_servant:YES";
        rtcd_cmd += " -o corba.master_manager:" + prop["corba.master_manager"];
        rtcd_cmd += " -o manager.name:" + prop["manger.name"];
        rtcd_cmd += " -o manager.instance_name:" + mgrstr;
        rtcd_cmd += " -o shutdown_auto:NO";
        rtcd_cmd += " -o manager.auto_shutdown_duration:50";


        coil::vstring slaves_names;
        if (mgrstr == "manager_%p")
          {
            Guard gurad(m_slaveMutex);
                        
            for (CORBA::ULong i(0); i < m_slaves.length(); ++i)
              {
                try
                  {
                    RTM::NVList_var nvlist = m_slaves[i]->get_configuration();
                    coil::Properties sleave_prop;
                    NVUtil::copyToProperties(sleave_prop, nvlist);
                    std::string name = sleave_prop["manager.instance_name"];
                    if (isProcessIDManager(name))
                      {
                        slaves_names.push_back(name);
                      }
                  }
                catch (...)
                  {
                     RTC_ERROR(("A slave manager thrown exception."));
                     CORBA_SeqUtil::erase(m_slaves, i);
                     RTC_ERROR(("This slave manager is removed from slave list."));
                   }
               }
          }

        RTC_DEBUG(("Invoking command: %s.", rtcd_cmd.c_str()));
        int ret(coil::launch_shell(rtcd_cmd));
        if (ret == -1)
          {
            RTC_DEBUG(("%s: failed", rtcd_cmd.c_str()));
            return RTC::RTObject::_nil();
          }
        
        for (size_t i(0); i < 1000; ++i)
          {
            coil::sleep(coil::TimeValue(0.01));
            RTC_DEBUG(("Detecting new slave manager (%s).", mgrstr.c_str()));
            if (mgrstr == "manager_%p")
              {
                Guard gurad(m_slaveMutex);
                for (CORBA::ULong j(0); j < m_slaves.length(); ++j)
                  {
                    RTM::NVList_var nvlist = m_slaves[j]->get_configuration();
                    coil::Properties sleave_prop;
                    NVUtil::copyToProperties(sleave_prop, nvlist);
                    std::string name = sleave_prop["manager.instance_name"];

                    if (isProcessIDManager(name))
                      {
                        if (std::count(slaves_names.begin(), slaves_names.end(), name) == 0)
                          {
                            mgrobj = m_slaves[j];
                          }
                      }
                  }
              }
            else
              {
                mgrobj = findManagerByName(mgrstr);
              }
            if (!CORBA::is_nil(mgrobj))
              {
                RTC_INFO(("New slave manager (%s) launched.", mgrstr.c_str()));
                break;
              }
            RTC_DEBUG(("Waiting for slave manager started."));
            coil::sleep(coil::TimeValue(0.01));
          }
      }

    if (CORBA::is_nil(mgrobj))
      {
        RTC_ERROR(("Launch failed: manager (%s)", mgrstr.c_str()));
        return RTC::RTObject::_nil();
      }
    std::string create_arg_str(create_arg);
    getParameterByModulename("manager_name", create_arg_str);
    RTC_DEBUG(("Creating component on %s",  mgrstr.c_str()));
    RTC_DEBUG(("arg: %s", create_arg_str.c_str()));
    try
      {
        return mgrobj->create_component(create_arg_str.c_str());
      }
    catch (CORBA::SystemException&)
      {
        RTC_ERROR(("Exception was caught while creating component."));
        return RTC::RTObject::_nil();
      }
    catch (...)
      {
        RTC_ERROR(("Unknown non-CORBA exception cought."));
        return RTC::RTObject::_nil();
      }
      
    return RTC::RTObject::_nil();
  }
  
  /*
   * @if jp
   * @brief 指定アドレスのマネージャでRTCを起動する
   * @else
   * @brief Launch a RTC on the specified IP/Port address manager
   * @endif
   */
  RTC::RTObject_ptr
  ManagerServant::createComponentByAddress(const std::string create_arg)
  {
    RTC_TRACE(("createComponentByAddress(%s)",create_arg.c_str()));
    coil::mapstring param = coil::urlparam2map(create_arg);
    for(auto & p : param)
      {
        RTC_DEBUG(("create_arg[%s] = %s",
                   p.first.c_str(), p.second.c_str()));
      }

    std::string mgrstr = param["manager_address"];
    if (mgrstr.empty())
      {
        RTC_WARN(("No manager_address found: %s", mgrstr.c_str()));
        return RTC::RTObject::_nil();
      }
    
    coil::vstring mgrvstr = coil::split(mgrstr, ":", true);
    if (mgrvstr.size() != 2)
      {
        RTC_WARN(("Invalid manager address: %s", mgrstr.c_str()));
        return RTC::RTObject::_nil();
      }


    CompParam comp_param(create_arg);
    // detect languange
    std::string lang = comp_param.language();
    if (lang.empty()) { lang = "C++"; }
    RTC_INFO(("Specified manager's language: %s", lang.c_str()));

    // find manager
    RTM::Manager_var mgrobj = findManager(mgrstr);

    if (CORBA::is_nil(mgrobj))
      {
        RTC_INFO(("Manager: %s not found.", mgrstr.c_str()));
        RTC_INFO(("Creating new manager named %s", mgrstr.c_str()));

        std::string rtcd_cmd_key("manager.modules.");
        rtcd_cmd_key += lang + "manager_cmd";
        coil::Properties& prop = m_mgr.getConfig();
        std::string rtcd_cmd = prop[rtcd_cmd_key];

        if (rtcd_cmd.empty())
          {
            RTC_WARN(("rtcd command name not found. Default rtcd is used."));
            rtcd_cmd = "rtcd";
          }
        rtcd_cmd += " -o corba.master_manager:" + mgrstr;
        rtcd_cmd += " -d ";

        RTC_DEBUG(("Invoking command: %s.", rtcd_cmd.c_str()));
        int ret(coil::launch_shell(rtcd_cmd));
        if (ret == -1)
          {
            RTC_DEBUG(("%s: failed", rtcd_cmd.c_str()));
            return RTC::RTObject::_nil();
          }

        coil::sleep(coil::TimeValue(0.01));
        for (size_t i(0); i < 1000; ++i)
          {
            RTC_DEBUG(("Detecting new slave manager (%s).", mgrstr.c_str()))
            mgrobj = findManagerByName(mgrstr);
            if (!CORBA::is_nil(mgrobj))
              {
                RTC_INFO(("New slave manager (%s) launched.", mgrstr.c_str()));
                break;
              }
            RTC_DEBUG(("Waiting for slave manager started."));
            coil::sleep(coil::TimeValue(0.01));
          }

        if (CORBA::is_nil(mgrobj))
          {
            RTC_ERROR(("Launch failed: manager (%s)", mgrstr.c_str()));
            return RTC::RTObject::_nil();
          }

        std::string create_arg_str(create_arg);
        getParameterByModulename("manager_address", create_arg_str);
        RTC_DEBUG(("Creating component on %s",  mgrstr.c_str()));
        RTC_DEBUG(("arg: %s", create_arg.c_str()));
        try
          {
            return mgrobj->create_component(create_arg_str.c_str());
          }
        catch (CORBA::SystemException&)
          {
            RTC_ERROR(("Exception was caught while creating component."));
            return RTC::RTObject::_nil();
          }
        catch (...)
          {
            RTC_ERROR(("Unknown non-CORBA exception cought."));
            return RTC::RTObject::_nil();
          }
      }
    return RTC::RTObject::_nil();
  }


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
  void ManagerServant::updateMasterManager()
    {
      if (!m_isMaster && !CORBA::is_nil(m_objref))
        {
            {
              Guard guardm(m_masterMutex);
              if (m_masters.length() > 0)
                {
                  for (CORBA::ULong i = 0; i < m_masters.length(); i++)
                    {
                      try
                        {
                          if (m_masters[i]->_non_existent())
                            {
                              CORBA_SeqUtil::erase(m_masters, i);
                            }
                        }
                      catch (...)
                        {
                          RTC_ERROR(("Unknown exception cought."));
                          CORBA_SeqUtil::erase(m_masters, i);
                        }
                    }
                }
            }
            
          if (m_masters.length() == 0)
            {
              coil::Properties config = m_mgr.getConfig();
              try
                {
                  RTM::Manager_ptr owner = findManager(config.getProperty("corba.master_manager"));
                  if (CORBA::is_nil(owner))
                    {
                      RTC_INFO(("Master manager not found"));
                      return;
                    }
                  add_master_manager(owner);
                  owner->add_slave_manager(m_objref);
                }
              catch (...)
                {
                  RTC_ERROR(("Unknown exception cought."))
                }
            }

        }
    }

  bool ManagerServant::isProcessIDManager(std::string mgrname)
    {
      size_t pos = mgrname.find("manager_");
      if (pos != std::string::npos)
        {
          std::string id = mgrname;
          coil::replaceString(id, "manager_", "");
          
          int val = 0;
          if (coil::stringTo<int>(val, id.c_str()))
            {
              return true;
            }
        }
      return false;
    }

  const char* CompParam::prof_list[prof_list_size] = { "RTC", "vendor", "category", "implementation_id", "language", "version" };

  CompParam::CompParam(std::string module_name)
    {
      module_name = coil::split(module_name, "?")[0];
      coil::vstring param_list = coil::split(module_name,":");

      if (param_list.size() < prof_list_size)
        {
          m_type = "RTC";
          m_vendor = "";
          m_category = "";
          m_impl_id = module_name;
          m_language = "C++";
          m_version = "";
        }
      else
        {
          m_type = param_list[0];
          m_vendor = param_list[1];
          m_category = param_list[2];
          m_impl_id = param_list[3];

          if (!param_list[4].empty())
            {
              m_language = param_list[4];
            }
          else
            {
              m_language = "C++";
            }
          m_version = param_list[5];
        }
  }


  std::string CompParam::vendor()
    {
      return m_vendor;
    }
  std::string CompParam::category()
    {
      return m_category;
    }
  std::string CompParam::impl_id()
    {
      return m_impl_id;
    }
  std::string CompParam::language()
    {
      return m_language;
    }
  std::string CompParam::version()
    {
      return m_version;
    }
} // namespace RTM
