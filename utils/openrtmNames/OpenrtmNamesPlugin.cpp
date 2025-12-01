// -*- C++ -*-
/*!
 * @file  OpenrtmNamesPlugin.cpp
 * @brief OpenrtmNamesPlugin class
 * @date  $Date: 2025-10-17 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2025
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */

#include "OpenrtmNamesPlugin.h"
#include <rtm/ManagerConfig.h>
#include <coil/stringutil.h>
#include <iostream>

namespace OpenRTMNames
{
  ManagerActionListener::ManagerActionListener(RTC::Manager* manager)
  {
    m_manager = manager;
    PortableServer::POA_var root_poa = manager->getPOA();
    CORBA::ORB_var orb = manager->getORB();
#ifndef ORB_IS_TAO
    PortableServer::POA_var ins_poa;
#else
    IORTable::Table_var adapter;
#endif
    try {
      m_nameservice = new RTM::NamingContext(root_poa);
      CosNaming::NamingContextExt_var nameserviceref = m_nameservice->_this();
      CORBA::String_var sior(orb->object_to_string(nameserviceref));
      std::cout << sior << std::endl;

#ifndef ORB_IS_TAO
#ifndef ORB_IS_RTORB
      CORBA::Object_var ins_obj = orb->resolve_initial_references("omniINSPOA");

      ins_poa = PortableServer::POA::_narrow(ins_obj);
      ins_poa->the_POAManager()->activate();

      PortableServer::ObjectId_var id = PortableServer::string_to_ObjectId("NameService");
      ins_poa->activate_object_with_id(id.in(), m_nameservice);
#else
      root_poa->reinstall_object(m_nameservice, "NameService");
#endif
#else
      CORBA::Object_var obj = orb->resolve_initial_references("IORTable");
      adapter = IORTable::Table::_narrow(obj.in());

      CORBA::String_var ior = orb->object_to_string(nameservice->_this());
      adapter->bind("NameService", ior.in());
#endif
    }
#ifndef ORB_IS_RTORB
    catch (const CORBA::INITIALIZE& ex) {
#ifdef ORB_IS_OMNIORB
      std::cerr << "Failed to initialise: " << ex.NP_minorString() << std::endl;
#elif defined(ORB_IS_TAO)
      std::cerr << "Failed to initialise: " << ex.minor() << std::endl;
#endif
      return;
    }
#endif
    catch (CORBA::SystemException& ex) {
#ifndef ORB_IS_RTORB
      std::cerr << "Caught CORBA::" << ex._name() << std::endl;
#else
      std::cerr << "Caught CORBA::" << ex.msg() << std::endl;
#endif
      return;
    }
    catch (CORBA::Exception& ex) {
#ifndef ORB_IS_RTORB
      std::cerr << "Caught CORBA::Exception: " << ex._name() << std::endl;
#else
      std::cerr << "Caught CORBA::Exception: " << ex.msg() << std::endl;
#endif
    }

  }
  ManagerActionListener::~ManagerActionListener()
  {

  }
  void ManagerActionListener::preShutdown()
  {
    m_nameservice->destroy();
    PortableServer::POA_var root_poa = m_manager->getPOA();


    PortableServer::ObjectId_var oid = root_poa->servant_to_id(m_nameservice);
    root_poa->deactivate_object(oid);


#ifndef ORB_IS_TAO
#ifndef ORB_IS_RTORB
    CORBA::Object_var obj = m_manager->theORB()->resolve_initial_references("omniINSPOA");

    PortableServer::POA_var ins_poa = PortableServer::POA::_narrow(obj);
    PortableServer::ObjectId_var id;
    id = ins_poa->servant_to_id(m_nameservice);

    ins_poa->deactivate_object(id.in());
#endif
#else
    CORBA::Object_var obj = m_manager.theORB()->resolve_initial_references("IORTable");
    IORTable::Table_var adapter = IORTable::Table::_narrow(obj.in());


    coil::Properties config(m_manager.getConfig());

    adapter->unbind("NameService");
#endif

  }
  void ManagerActionListener::postShutdown()
  {
  }
  void ManagerActionListener::postReinit()
  {

  }

  void ManagerActionListener::preReinit()
  {

  }

}

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void OpenrtmNamesPluginInit(RTC::Manager* manager)
  {
    OpenRTMNames::ManagerActionListener* listener = new OpenRTMNames::ManagerActionListener(manager);
    manager->addManagerActionListener(listener);
  }

}


