// -*- C++ -*-
/*!
 * @file openrtmNames.cpp
 * @brief Naming Server main function
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2022
 *     Nobuhiko Miyamoto
 *     Software Platform Research Team,
 *     Industrial Cyber-Physical Systems Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/RTC.h>
#include "NamingContext.h"
#ifdef ORB_IS_OMNIORB
#ifdef SSL_ENABLE
#include <omniORB4/sslContext.h>
#ifdef HTTP_ENABLE
#include <omniORB4/httpContext.h>
#endif
#endif
#elif defined(ORB_IS_TAO)
#include <tao/IORTable/IORTable.h>
#ifdef SSL_ENABLE
#include <orbsvcs/SecurityLevel2C.h>
#endif
#endif
#include <rtm/ManagerConfig.h>
#include <coil/stringutil.h>
#include <iostream>


int main(int argc, char **argv)
{
  coil::Properties prop;
  RTC::ManagerConfig config(argc, argv);
  config.configure(prop);

  std::string opt(prop["corba.args"]);
  coil::vstring args{ coil::parseArgs(opt) };
  args.insert(args.begin(), "openrtmNames");
  coil::Argv cargv = coil::Argv(args);
  int cargvsize = static_cast<int>(cargv.size());

  CORBA::ORB_var orb;
  PortableServer::POA_var root_poa;
#ifndef ORB_IS_TAO
  PortableServer::POA_var ins_poa;
#else
  IORTable::Table_var adapter;
#endif
  PortableServer::Servant_var<RTM::NamingContext> nameservice = nullptr;

  try {
#ifdef ORB_IS_OMNIORB
#ifdef SSL_ENABLE
    std::string ssl_certificate_authority_file = prop["corba.ssl.certificate_authority_file"];
    std::string ssl_key_file = prop["corba.ssl.key_file"];
    std::string ssl_key_file_password = prop["corba.ssl.key_file_password"];
    if(!ssl_certificate_authority_file.empty())
#ifdef RTM_OMNIORB_43
      omni::sslContext::certificate_authority_file
#else
      sslContext::certificate_authority_file
#endif
        = CORBA::string_dup(ssl_certificate_authority_file.c_str());
    if(!ssl_key_file.empty())
#ifdef RTM_OMNIORB_43
      omni::sslContext::key_file
#else
      sslContext::key_file
#endif
        = CORBA::string_dup(ssl_key_file.c_str());
    if(!ssl_key_file_password.empty())
#ifdef RTM_OMNIORB_43
      omni::sslContext::key_file_password
#else
      sslContext::key_file_password
#endif
        = CORBA::string_dup(ssl_key_file_password.c_str());

#ifdef HTTP_ENABLE
    std::string http_certificate_authority_file = prop["corba.http.certificate_authority_file"];
    std::string http_key_file = prop["corba.http.key_file"];
    std::string http_key_file_password = prop["corba.http.key_file_password"];
    if(!http_certificate_authority_file.empty())
      omni::httpContext::certificate_authority_file = CORBA::string_dup(http_certificate_authority_file.c_str());
    if(!http_key_file.empty())
      omni::httpContext::key_file = CORBA::string_dup(http_key_file.c_str());
    if(!http_key_file_password.empty())
      omni::httpContext::key_file_password = CORBA::string_dup(http_key_file_password.c_str());
#endif
#endif
#endif
    orb = CORBA::ORB_init(cargvsize, cargv.get());

#ifdef ORB_IS_TAO
#ifdef SSL_ENABLE
    for(size_t i=0;i < args.size();i++)
    {
      if(args[i].find("-ORB") != std::string::npos && args[i].find("Endpoint") != std::string::npos)
      {
        if(i < args.size()-1)
        {
          if(args[i+1].find("ssliop://") != std::string::npos)
          {
            CORBA::Object_var sec_man =
              orb->resolve_initial_references ("SecurityLevel2:SecurityManager");
            SecurityLevel2::SecurityManager_var sec2manager =
              SecurityLevel2::SecurityManager::_narrow (sec_man.in ());
            SecurityLevel2::AccessDecision_var ad_tmp = sec2manager->access_decision ();
            TAO::SL2::AccessDecision_var ad =
              TAO::SL2::AccessDecision::_narrow (ad_tmp.in ());
            ad->default_collocated_decision (true);
            break;
          }
        }
      }
    }
#endif
#endif

    CORBA::Object_var root_obj = orb->resolve_initial_references("RootPOA");
    root_poa = PortableServer::POA::_narrow(root_obj);

    
    CORBA::PolicyList pl;
    pl.length(1);
    pl[0] = static_cast<CORBA::Policy_ptr>(root_poa->create_lifespan_policy(PortableServer::PERSISTENT));

    nameservice = new RTM::NamingContext(root_poa);

    CosNaming::NamingContextExt_var nameserviceref = nameservice->_this();
    CORBA::String_var sior(orb->object_to_string(nameserviceref));
    std::cout << sior << std::endl;


    
#ifndef ORB_IS_TAO
    PortableServer::POAManager_var pman = root_poa->the_POAManager();
    pman->activate();
    CORBA::Object_var ins_obj = orb->resolve_initial_references("omniINSPOA");

    ins_poa = PortableServer::POA::_narrow(ins_obj);
    ins_poa->the_POAManager()->activate();

    PortableServer::ObjectId_var id = PortableServer::string_to_ObjectId("NameService");
    ins_poa->activate_object_with_id(id.in(), nameservice);
#else
    CORBA::Object_var obj = orb->resolve_initial_references("IORTable");
    adapter = IORTable::Table::_narrow(obj.in());

    CORBA::String_var ior = orb->object_to_string(nameservice->_this());
    adapter->bind("NameService", ior.in());
#endif

    orb->run();

  }
  catch (const CORBA::INITIALIZE& ex) {
#ifdef ORB_IS_OMNIORB
    std::cerr << "Failed to initialise: " << ex.NP_minorString() << std::endl;
#elif defined(ORB_IS_TAO)
    std::cerr << "Failed to initialise: " << ex.minor() << std::endl;
#endif
    return -1;
  }
   catch (CORBA::SystemException& ex) {
    std::cerr << "Caught CORBA::" << ex._name() << std::endl;
    return -1;
  }
  catch (CORBA::Exception& ex) {
    std::cerr << "Caught CORBA::Exception: " << ex._name() << std::endl;
    return -1;
  }

  /*
  PortableServer::ObjectId_var id = ins_poa->servant_to_id(nameservice);
  ins_poa->deactivate_object(id);

  orb->destroy();
  */
  return 0;
}
