// -*- C++ -*-
/*!
 * @file NamingManager.h
 * @brief naming Service helper class
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

#include <coil/Routing.h>
#include <coil/stringutil.h>

#include <rtm/NamingManager.h>
#include <rtm/Manager.h>
#include <rtm/CORBA_IORUtil.h>
#include <rtm/CORBA_RTCUtil.h>

#include <algorithm>
#include <functional>
#include <iostream>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  NamingOnCorba::NamingOnCorba(CORBA::ORB_ptr orb, const char *names)
      : m_cosnaming(orb, names), m_endpoint(""),
        m_replaceEndpoint(false)
  {
    rtclog.setName("NamingOnCorba");
    if (CORBA_RTCUtil::CorbaURI(names).isAddressOnly())
    {
      coil::Properties &prop(Manager::instance().getConfig());
      m_replaceEndpoint =
          coil::toBool(prop["corba.nameservice.replace_endpoint"],
                       "YES", "NO", true);

      coil::vstring host_port(coil::split(names, ":"));
      if (coil::dest_to_endpoint(host_port[0], m_endpoint))
      {

        RTC_INFO(("Endpoint for the CORBA naming service (%s) is %s.",
                  host_port[0].c_str(),
                  m_endpoint.c_str()));
      }
      else
      {
        RTC_WARN(("No endpoint for the CORBA naming service (%s) was found.",
                  host_port[0].c_str()));
      }
    }
  }
  /*!
   * @if jp
   * @brief 指定した CORBA オブジェクトのNamingServiceへバインド
   * @else
   * @brief Bind the specified CORBA objects to NamingService
   * @endif
   */
  void NamingOnCorba::bindObject(const char *name,
                                 const RTObject_impl *rtobj)
  {
    RTC_TRACE(("bindObject(name = %s, rtobj)", name));
#ifdef ORB_IS_OMNIORB
    if (!m_endpoint.empty() && m_replaceEndpoint)
    {
      CORBA::Object_var obj(rtobj->getObjRef());
      CORBA::String_var ior;
      CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
      ior = orb->object_to_string(obj.in());
      std::string iorstr((const char *)ior);

      RTC_DEBUG(("Original IOR information:\n %s",
                 CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
      CORBA_IORUtil::replaceEndpoint(iorstr, m_endpoint);
      CORBA::Object_var newobj = orb->string_to_object(iorstr.c_str());

      RTC_DEBUG(("Modified IOR information:\n %s",
                 CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
      m_cosnaming.rebindByString(name, newobj.in(), true);
    }
    else
    {
#endif // ORB_IS_OMNIORB
      RTC::RTObject_var objref = rtobj->getObjRef();
      m_cosnaming.rebindByString(name, objref.in(), true);
#ifdef ORB_IS_OMNIORB
    }
#endif // ORB_IS_OMNIORB
  }

  void NamingOnCorba::bindObject(const char *name,
                                 const PortBase *port)
  {
    RTC_TRACE(("bindObject(name = %s, port)", name));
#ifdef ORB_IS_OMNIORB
    if (!m_endpoint.empty() && m_replaceEndpoint)
    {
      CORBA::Object_var obj(port->getPortRef());
      CORBA::String_var ior;
      CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
      ior = orb->object_to_string(obj.in());
      std::string iorstr((const char *)ior);

      RTC_DEBUG(("Original IOR information:\n %s",
                 CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
      CORBA_IORUtil::replaceEndpoint(iorstr, m_endpoint);
      CORBA::Object_var newobj = orb->string_to_object(iorstr.c_str());

      RTC_DEBUG(("Modified IOR information:\n %s",
                 CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
      m_cosnaming.rebindByString(name, newobj.in(), true);
    }
    else
    {
#endif // ORB_IS_OMNIORB
      PortService_var portref = port->getPortRef();
      m_cosnaming.rebindByString(name, portref, true);
#ifdef ORB_IS_OMNIORB
    }
#endif // ORB_IS_OMNIORB
  }

  void NamingOnCorba::bindObject(const char *name,
                                 const RTM::ManagerServant *mgr)
  {
    RTC_TRACE(("bindObject(name = %s, mgr)", name));
#ifdef ORB_IS_OMNIORB
    if (!m_endpoint.empty() && m_replaceEndpoint)
    {
      CORBA::Object_var obj(mgr->getObjRef());
      CORBA::String_var ior;
      CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
      ior = orb->object_to_string(obj.in());
      std::string iorstr((const char *)ior);

      RTC_DEBUG(("Original IOR information:\n %s",
                 CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
      CORBA_IORUtil::replaceEndpoint(iorstr, m_endpoint);
      CORBA::Object_var newobj = orb->string_to_object(iorstr.c_str());

      RTC_DEBUG(("Modified IOR information]\n %s",
                 CORBA_IORUtil::formatIORinfo(iorstr.c_str()).c_str()));
      m_cosnaming.rebindByString(name, newobj.in(), true);
    }
    else
    {
#endif // ORB_IS_OMNIORB
      RTM::Manager_var objref = mgr->getObjRef();
      m_cosnaming.rebindByString(name, objref.in(), true);
#ifdef ORB_IS_OMNIORB
    }
#endif // ORB_IS_OMNIORB
  }

  /*!
   * @if jp
   * @brief 指定した CORBA オブジェクトをNamingServiceからアンバインド
   * @else
   * @brief Unbind the specified CORBA object from NamingService
   * @endif
   */
  void NamingOnCorba::unbindObject(const char *name)
  {
    RTC_TRACE(("unbindObject(name  = %s)", name));
    m_cosnaming.unbind(name);
  }

  bool NamingOnCorba::isAlive()
  {
    RTC_TRACE(("isAlive()"));
    return m_cosnaming.isAlive();
  }

  /*!
   * @if jp
   *
   * @brief ネーミングサービスからRTCをインスタンス名から検索し、
   *        一致するRTCのリストを取得する
   *
   * @param context 現在検索中のコンテキスト
   * @param name RTCのインスタンス名
   * @param rtcs RTCのリスト
   *
   * @else
   *
   * @brief
   *
   * @param context
   * @param name
   * @param rtcs
   *
   * @endif
   */
  void NamingOnCorba::getComponentByName(CosNaming::NamingContext_ptr context, const std::string &name, RTC::RTCList &rtcs)
  {
    RTC_TRACE(("getComponentByName(name = %s)", name.c_str()));
    CORBA::ULong length = 500;
    CosNaming::BindingList_var bl;
    CosNaming::BindingIterator_var bi;
    context->list(length, bl, bi);

    CORBA::ULong len(bl->length());
    for (CORBA::ULong i = 0; i < len; ++i)
    {
      if (bl[i].binding_type == CosNaming::ncontext)
      {
        CosNaming::NamingContext_var next_context = CosNaming::NamingContext::
            _narrow(context->resolve(bl[i].binding_name));
        getComponentByName(next_context, name, rtcs);
      }
      else if (bl[i].binding_type == CosNaming::nobject)
      {
        if (std::string(bl[i].binding_name[0].id) == name && std::string(bl[i].binding_name[0].kind) == "rtc")
        {
          try
          {
            RTC::RTObject_var obj = RTC::RTObject::_narrow(context->resolve(bl[i].binding_name));
            if (!obj->_non_existent())
            {
#ifndef ORB_IS_RTORB
              CORBA_SeqUtil::push_back(rtcs, obj);
#else
              CORBA_SeqUtil::push_back(rtcs, obj.in());
#endif
            }
          }
          catch (...)
          {
          }
        }
      }
    }
  }
  /*!
   * @if jp
   *
   * @brief ネーミングサービスからRTCをインスタンス名から検索し、
   *        一致するRTCのリストを取得する
   *
   * @param context 現在検索中のコンテキスト
   * @param name RTCのインスタンス名
   * @param rtcs RTCのリスト
   *
   * @else
   *
   * @brief
   *
   * @param context
   * @param name
   * @param rtcs
   *
   * @endif
   */
  RTC::RTCList NamingOnCorba::string_to_component(std::string name)
  {
    RTC_TRACE(("string_to_component(name = %s)", name.c_str()));
    RTC::RTCList rtc_list;

    CORBA_RTCUtil::RTCURIObject rtcuri(name, true, false);

    if (!rtcuri.isRTCNameURI())
    {
      RTC_WARN(("syntax error: %s", name.c_str()));
      return rtc_list;
    }
    else
    {
      RTC_INFO(("URI: %s, Address: %s, Name: %s", name.c_str(), rtcuri.getAddress().c_str(), rtcuri.getRTCName().c_str()));
      try
      {
        RTC::CorbaNaming cns = m_cosnaming;

        if (rtcuri.getAddress() != "*")
        {
          CORBA::ORB_var orb = Manager::instance().getORB();
          cns = RTC::CorbaNaming(orb, rtcuri.getAddress().c_str());
        }

        std::string rtcname = rtcuri.getRTCName();

        std::string context;
        std::string compname;

        size_t pos = rtcname.find_first_of("/");
        if (pos != std::string::npos)
        {
          context = rtcname.substr(0, pos);
          compname = rtcname.substr(pos + 1);
        }

        if (context == "*")
        {
          CosNaming::NamingContext_var root_cxt = cns.getRootContext();
          getComponentByName(root_cxt, compname, rtc_list);
          return rtc_list;
        }
        else
        {
          rtcname += ".rtc";
          CORBA::Object_var obj = cns.resolveStr(rtcname.c_str());
          if (CORBA::is_nil(obj))
          {
            return rtc_list;
          }
          if (obj->_non_existent())
          {
            return rtc_list;
          }
          CORBA_SeqUtil::push_back(rtc_list, RTC::RTObject::_narrow(obj));
          return rtc_list;
        }
      }
      catch (...)
      {
        return rtc_list;
      }
    }
    return rtc_list;
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  NamingOnManager::NamingOnManager(CORBA::ORB_ptr orb, Manager *mgr)
      : m_orb(orb), m_mgr(mgr)
  {
  }
  /*!
   * @if jp
   * @brief 指定した CORBA オブジェクトのNamingServiceへバインド
   * @else
   * @brief Bind the specified CORBA objects to NamingService
   * @endif
   */
  void NamingOnManager::bindObject(const char *name,
                                   const RTObject_impl * /*rtobj*/)
  {
    RTC_TRACE(("bindObject(name = %s, rtobj)", name));
    return;
  }

  void NamingOnManager::bindObject(const char *name,
                                   const PortBase * /*port*/)
  {
    RTC_TRACE(("bindObject(name = %s, port)", name));
    return;
  }

  void NamingOnManager::bindObject(const char *name,
                                   const RTM::ManagerServant * /*mgr*/)
  {
    RTC_TRACE(("bindObject(name = %s, mgr)", name));
    return;
  }

  /*!
   * @if jp
   * @brief 指定した CORBA オブジェクトをNamingServiceからアンバインド
   * @else
   * @brief Unbind the specified CORBA object from NamingService
   * @endif
   */
  void NamingOnManager::unbindObject(const char *name)
  {
    RTC_TRACE(("unbindObject(name  = %s)", name));
    return;
  }

  bool NamingOnManager::isAlive()
  {
    RTC_TRACE(("isAlive()"));
    return true;
  }

  /*!
   * @if jp
   *
   * @brief rtcloc形式でRTCのオブジェクトリファレンスを取得する
   *
   * @param name rtcloc形式でのRTC名
   * rtcloc://localhost:2809/example/ConsoleIn
   * @return RTCのオブジェクトリファレンスのリスト
   *
   * @else
   *
   * @brief
   *
   * @param name
   * @return
   *
   * @endif
   */
  RTC::RTCList NamingOnManager::string_to_component(std::string name)
  {
    RTC_TRACE(("string_to_component(name = %s)", name.c_str()));
    RTC::RTCList rtc_list;

    CORBA_RTCUtil::RTCURIObject rtcuri(name, false, true);
    if (!rtcuri.isRTCLocURI())
    {
      RTC_WARN(("syntax error: %s", name.c_str()));
      return rtc_list;
    }
    else
    {
      RTC_INFO(("URI: %s, Address: %s, Name: %s", name.c_str(), rtcuri.getAddress().c_str(), rtcuri.getRTCName().c_str()));
      RTM::Manager_var mgr = getManager(rtcuri.getAddress());
      if (!CORBA::is_nil(mgr))
      {
        rtc_list = (*mgr->get_components_by_name(rtcuri.getRTCName().c_str()));
        RTM::ManagerList *slaves = mgr->get_slave_managers();
        for (unsigned int i = 0; i < slaves->length(); i++)
        {
          try
          {
            RTC::RTCList slave_rtcs = (*(*slaves)[i]->get_components_by_name(rtcuri.getRTCName().c_str()));
            CORBA_SeqUtil::push_back_list(rtc_list, slave_rtcs);
          }
          catch (...)
          {
            mgr->remove_slave_manager((*slaves)[i]);
          }
        }
      }
      return rtc_list;
    }
    return rtc_list;
  }

  /*!
   * @if jp
   *
   * @brief 指定ホスト名、ポート名でManagerのオブジェクトリファレンスを取得
   *
   * @param name ホスト名、ポート名
   *
   * @return Managerのオブジェクトリファレンス
   *
   * @else
   *
   * @brief
   *
   * @param name
   * @return
   *
   * @endif
   */
  RTM::Manager_ptr NamingOnManager::getManager(const std::string &name)
  {
    RTC_TRACE(("getManager(name = %s)", name.c_str()));
    if (name == "*")
    {
      RTM::Manager_var mgr = RTM::Manager::_nil();
      RTM::ManagerServant &mgr_sev = m_mgr->getManagerServant();
      if (mgr_sev.is_master())
      {
        mgr = mgr_sev.getObjRef();
      }
      else
      {
        RTM::ManagerList_var masters = mgr_sev.get_master_managers();

        if (masters->length() > 0)
        {
          mgr = RTM::Manager::_duplicate(masters[0]);
        }
        else
        {
          mgr = mgr_sev.getObjRef();
        }
      }
      return mgr;
    }

    try
    {
      coil::Properties prop = m_mgr->getConfig();
      std::string manager_name = prop.getProperty("manager.name");

      std::string mgrloc(CORBA_RTCUtil::CorbaURI(name, manager_name).toString());

      CORBA::Object_var mobj = m_orb->string_to_object(mgrloc.c_str());
      RTM::Manager_var mgr = RTM::Manager::_narrow(mobj);

      RTC_DEBUG(("corbaloc: %s", mgrloc.c_str()));

      return RTM::Manager::_duplicate(mgr);
    }
    catch (CORBA::SystemException &)
    {
    }
    catch (...)
    {
      RTC_ERROR(("Unknown exception cought."));
    }
    return RTM::Manager::_nil();
  }

  //============================================================
  // NamingManager
  //============================================================
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  NamingManager::NamingManager(Manager *manager)
      : m_manager(manager), rtclog("NamingManager")
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  NamingManager::~NamingManager() = default;

  /*!
   * @if jp
   * @brief NameServer の登録
   * @else
   * @brief Register the NameServer
   * @endif
   */
  void NamingManager::registerNameServer(const char *method,
                                         const char *name_server)
  {
    RTC_TRACE(("NamingManager::registerNameServer(%s, %s)",
               method, name_server));
    NamingBase *name;
    name = createNamingObj(method, name_server);

    std::lock_guard<std::mutex> guard(m_namesMutex);
    m_names.emplace_back(new NamingService(method, name_server, name));
  }

  /*!
   * @if jp
   * @brief 指定したオブジェクトのNamingServiceへバインド
   * @else
   * @brief Bind the specified objects to NamingService
   * @endif
   */
  void NamingManager::bindObject(const char *name,
                                 const RTObject_impl *rtobj)
  {
    RTC_TRACE(("NamingManager::bindObject(%s)", name));

    std::lock_guard<std::mutex> guard(m_namesMutex);
    for (auto &n : m_names)
    {
      if (n->ns != nullptr)
      {
        try
        {
          n->ns->bindObject(name, rtobj);
        }
        catch (...)
        {
          delete n->ns;
          n->ns = nullptr;
        }
      }
    }
    registerCompName(name, rtobj);
  }
  void NamingManager::bindObject(const char *name,
                                 const PortBase *port)
  {
    RTC_TRACE(("NamingManager::bindObject(%s)", name));

    std::lock_guard<std::mutex> guard(m_namesMutex);
    for (auto &n : m_names)
    {
      if (n->ns != nullptr)
      {
        try
        {
          n->ns->bindObject(name, port);
        }
        catch (...)
        {
          delete n->ns;
          n->ns = nullptr;
        }
      }
    }
    registerPortName(name, port);
  }
  void NamingManager::bindObject(const char *name,
                                 const RTM::ManagerServant *mgr)
  {
    RTC_TRACE(("NamingManager::bindObject(%s)", name));

    std::lock_guard<std::mutex> guard(m_namesMutex);
    for (auto &n : m_names)
    {
      if (n->ns != nullptr)
      {
        try
        {
          n->ns->bindObject(name, mgr);
        }
        catch (...)
        {
          delete n->ns;
          n->ns = nullptr;
        }
      }
    }
    registerMgrName(name, mgr);
  }

  /*!
   * @if jp
   * @brief NamingServer の情報の更新
   * @else
   * @brief Update information of NamingServer
   * @endif
   */
  void NamingManager::update()
  {
    RTC_TRACE(("NamingManager::update()"));

    std::lock_guard<std::mutex> guard(m_namesMutex);
    bool rebind(coil::toBool(m_manager->getConfig()["naming.update.rebind"],
                             "YES", "NO", false));
    for (auto &name : m_names)
    {
      if (name->ns == nullptr) // if ns==NULL
      {
        RTC_DEBUG(("Retrying connection to %s/%s",
                   name->method.c_str(),
                   name->nsname.c_str()));
        retryConnection(name);
      }
      else
      {
        try
        {
          if (rebind)
          {
            bindCompsTo(name->ns);
          }
          if (!name->ns->isAlive())
          {
            RTC_INFO(("Name server: %s (%s) disappeared.",
                      name->nsname.c_str(),
                      name->method.c_str()));
            delete name->ns;
            name->ns = nullptr;
          }
        }
        catch (...)
        {
          RTC_INFO(("Name server: %s (%s) disappeared.",
                    name->nsname.c_str(),
                    name->method.c_str()));
          delete name->ns;
          name->ns = nullptr;
        }
      }
    }
  }

  /*!
   * @if jp
   * @brief 指定したオブジェクトをNamingServiceからアンバインド
   * @else
   * @brief Unbind the specified object from NamingService
   * @endif
   */
  void NamingManager::unbindObject(const char *name)
  {
    RTC_TRACE(("NamingManager::unbindObject(%s)", name));

    std::lock_guard<std::mutex> guard(m_namesMutex);
    for (auto &n : m_names)
    {
      if (n->ns != nullptr)
      {
        try
        {
          n->ns->unbindObject(name);
        }
        catch (...)
        {
          delete n->ns;
          n->ns = nullptr;
        }
      }
    }
    unregisterCompName(name);
    unregisterMgrName(name);
  }

  /*!
   * @if jp
   * @brief 全てのオブジェクトをNamingServiceからアンバインド
   * @else
   * @brief Unbind all objects from NamingService
   * @endif
   */
  void NamingManager::unbindAll()
  {
    RTC_TRACE(("NamingManager::unbindAll(): %d names.", m_compNames.size()));
    {
      std::lock_guard<std::mutex> guard(m_compNamesMutex);
      coil::vstring names;
      // unbindObject modifiy m_compNames
      for (auto &compName : m_compNames)
      {
        names.emplace_back(compName->name);
      }
      for (auto &name : names)
      {
        unbindObject(name.c_str());
      }
    }
    {
      std::lock_guard<std::mutex> guard(m_mgrNamesMutex);
      coil::vstring names;
      // unbindObject modifiy m_mgrNames
      for (auto &mgrName : m_mgrNames)
      {
        names.emplace_back(mgrName->name);
      }
      for (auto &name : names)
      {
        unbindObject(name.c_str());
      }
    }
    {
      std::lock_guard<std::mutex> guard(m_portNamesMutex);
      coil::vstring names;
      // unbindObject modifiy m_portNames
      for (auto &portName : m_portNames)
      {
        names.emplace_back(portName->name);
      }
      for (auto &name : names)
      {
        unbindObject(name.c_str());
      }
    }
  }

  /*!
   * @if jp
   * @brief バインドされている全てのオブジェクトを取得
   * @else
   * @brief Get all bound objects
   * @endif
   */
  std::vector<RTObject_impl *> NamingManager::getObjects()
  {
    std::vector<RTObject_impl *> comps;
    std::lock_guard<std::mutex> guard(m_compNamesMutex);

    for (auto &compName : m_compNames)
    {
      comps.emplace_back(const_cast<RTObject_impl *>(compName->rtobj));
    }
    return comps;
  }

  //============================================================
  // Protected
  //============================================================
  /*!
   * @if jp
   * @brief NameServer 管理用オブジェクトの生成
   * @else
   * @brief Create objects for NameServer management
   * @endif
   */
  NamingBase *NamingManager::createNamingObj(const char *method,
                                             const char *name_server)
  {
    RTC_TRACE(("createNamingObj(method = %s, nameserver = %s",
               method, name_server));
    std::string m(method);

    if (m == "corba")
    {
      try
      {
        NamingBase *name;
        CORBA::ORB_var orb = m_manager->getORB();
        name = new NamingOnCorba(orb.in(), name_server);
        if (name == nullptr)
          return nullptr;
        RTC_INFO(("NameServer connection succeeded: %s/%s",
                  method, name_server));
        return name;
      }
      catch (...)
      {
        RTC_INFO(("NameServer connection failed: %s/%s",
                  method, name_server));
        return nullptr;
      }
    }
    else if (m == "manager")
    {
      NamingBase *name;
      CORBA::ORB_var orb = m_manager->getORB();
      name = new NamingOnManager(orb.in(), m_manager);
      return name;
    }
    return nullptr;
  }

  /*!
   * @if jp
   * @brief 設定済みコンポーネントを NameServer に登録
   * @else
   * @brief Register the configured component to NameServer
   * @endif
   */
  void NamingManager::bindCompsTo(NamingBase *ns)
  {
    for (auto &compName : m_compNames)
    {
      try
      {
        ns->bindObject(compName->name.c_str(), compName->rtobj);
      }
      catch (...)
      {
      }
    }
  }

  /*!
   * @if jp
   * @brief NameServer に登録するコンポーネントの設定
   * @else
   * @brief Configure the components that will be registered to NameServer
   * @endif
   */
  void NamingManager::registerCompName(const char *name,
                                       const RTObject_impl *rtobj)
  {
    for (auto &compName : m_compNames)
    {
      if (compName->name == name)
      {
        compName->rtobj = rtobj;
        return;
      }
    }
    m_compNames.emplace_back(new Comps(name, rtobj));
    return;
  }
  /*!
   * @if jp
   * @brief NameServer に登録するコンポーネントの設定
   * @else
   * @brief Configure the components that will be registered to NameServer
   * @endif
   */
  void NamingManager::registerPortName(const char *name,
                                       const PortBase *port)
  {
    for (auto &portName : m_portNames)
    {
      if (portName->name == name)
      {
        portName->port = port;
        return;
      }
    }
    m_portNames.emplace_back(new Port(name, port));
    return;
  }
  void NamingManager::registerMgrName(const char *name,
                                      const RTM::ManagerServant *mgr)
  {
    for (auto &mgrName : m_mgrNames)
    {
      if (mgrName->name == name)
      {
        mgrName->mgr = mgr;
        return;
      }
    }
    m_mgrNames.emplace_back(new Mgr(name, mgr));
    return;
  }

  /*!
   * @if jp
   * @brief NameServer に登録するコンポーネントの設定解除
   * @else
   * @brief Unregister the components that will be registered to NameServer
   * @endif
   */
  void NamingManager::unregisterCompName(const char *name)
  {
    for (std::vector<Comps *>::iterator comp = m_compNames.begin(); comp != m_compNames.end(); ++comp)
    {
      if ((*comp)->name == name)
      {
        delete (*comp);
        m_compNames.erase(comp);
        return;
      }
    }
    return;
  }
  void NamingManager::unregisterMgrName(const char *name)
  {
    for (std::vector<Mgr *>::iterator mgr = m_mgrNames.begin(); mgr != m_mgrNames.end(); ++mgr)
    {
      if ((*mgr)->name == name)
      {
        delete (*mgr);
        m_mgrNames.erase(mgr);
        return;
      }
    }
  }

  void NamingManager::retryConnection(NamingService *ns)
  {
    // recreate NamingObj
    NamingBase *nsobj(nullptr);
    try
    {
      nsobj = createNamingObj(ns->method.c_str(),
                              ns->nsname.c_str());
      if (nsobj != nullptr) // if succeed
      {
        RTC_INFO(("Connected to a name server: %s/%s",
                  ns->method.c_str(), ns->nsname.c_str()));
        ns->ns = nsobj;
        bindCompsTo(nsobj); // rebind all comps to new NS
        return;
      }
      else
      {
        RTC_DEBUG(("Name service: %s/%s still not available.",
                   ns->method.c_str(),
                   ns->nsname.c_str()));
      }
    }
    catch (...)
    {
      RTC_DEBUG(("Name server: %s/%s disappeared again.",
                 ns->method.c_str(),
                 ns->nsname.c_str()));
      if (nsobj != nullptr)
      {
        delete ns->ns;
        ns->ns = nullptr;
      }
    }
  }
  /*!
   * @if jp
   *
   * @brief rtcloc形式でRTCのオブジェクトリファレンスを取得
   *
   *
   *
   * @param name rtcloc形式でのRTC名
   * rtcloc://localhost:2809/example/ConsoleIn
   * @return RTCのオブジェクトリファレンスのリスト
   *
   * @else
   *
   * @brief
   * registerMgrName
   * @param name
   *
   * @return
   *
   *
   * @endif
   */
  RTCList NamingManager::string_to_component(const std::string &name)
  {

    for (auto &n : m_names)
    {
      if (n->ns != nullptr)
      {
        RTCList comps = n->ns->string_to_component(name);
        if (comps.length() > 0)
        {
          return comps;
        }
      }
    }
    return RTCList();
  }
} // namespace RTC
