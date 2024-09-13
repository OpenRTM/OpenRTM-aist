// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file ConsoleInTestComp.cpp
 * @brief Standalone component (test code)
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <rtm/CORBA_RTCUtil.h>
#include "ConsoleInTest.h"
#include "ConsoleIn.h"


void MyModuleInit(RTC::Manager* manager)
{
  ConsoleInTestInit(manager);
  ConsoleInInit(manager);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("ConsoleInTest");

  if (comp==nullptr)
  {
    std::cerr << "Component create failed." << std::endl;
    abort();
  }

  // Example
  // The following procedure is examples how handle RT-Components.
  // These should not be in this function.

  // Get the component's object reference
//  RTC::RTObject_var rtobj;
//  rtobj = RTC::RTObject::_narrow(manager->getPOA()->servant_to_reference(comp));

  // Get the port list of the component
//  PortServiceList* portlist;
//  portlist = rtobj->get_ports();

  // getting port profiles
//  std::cout << "Number of Ports: ";
//  std::cout << portlist->length() << std::endl << std::endl; 
//  for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i)
//  {
//    PortService_ptr port;
//    port = (*portlist)[i];
//    std::cout << "Port" << i << " (name): ";
//    std::cout << port->get_port_profile()->name << std::endl;
//    
//    RTC::PortInterfaceProfileList iflist;
//    iflist = port->get_port_profile()->interfaces;
//    std::cout << "---interfaces---" << std::endl;
//    for (CORBA::ULong i(0), n(iflist.length()); i < n; ++i)
//    {
//      std::cout << "I/F name: ";
//      std::cout << iflist[i].instance_name << std::endl;
//      std::cout << "I/F type: ";
//      std::cout << iflist[i].type_name << std::endl;
//      const char* pol;
//      pol = iflist[i].polarity == 0 ? "PROVIDED" : "REQUIRED";
//      std::cout << "Polarity: " << pol << std::endl;
//    }
//    std::cout << "---properties---" << std::endl;
//    NVUtil::dump(port->get_port_profile()->properties);
//    std::cout << "----------------" << std::endl << std::endl;
//  }

  return;
}

bool RunTest()
{
  RTC::RtcBase* comp;
  RTC::Manager &mamager = RTC::Manager::instance();
  comp = mamager.getComponent("ConsoleInTest0");
  if (comp == nullptr)
  {
    std::cerr << "Component get failed." << std::endl;
    return false;
  }

  ConsoleInTest* testcomp = dynamic_cast<ConsoleInTest*>(comp);
  if (testcomp == nullptr)
  {
    std::cerr << "Component get failed." << std::endl;
    return false;
  }

  return testcomp->runTest();
}

int main (int argc, char** argv)
{
  RTC::Manager* manager;
  manager = RTC::Manager::init(argc, argv);

  // Set module initialization proceduer
  // This procedure will be invoked in activateManager() function.
  manager->setModuleInitProc(MyModuleInit);

  // Activate manager and register to naming service
  manager->activateManager();

  // run the manager in blocking mode
  // runManager(false) is the default.
  // manager->runManager();

  // If you want to run the manager in non-blocking mode, do like this
  manager->runManager(true);

  bool ret = RunTest();

#if RTM_MAJOR_VERSION >= 2
  manager->terminate();
  manager->join();
#else
  manager->shutdown();
#endif

  
  if (ret)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
