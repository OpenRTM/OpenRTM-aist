// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file SeqOutComp.cpp
 * @brief Standalone component
 *
 */
// </rtc-template>

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "SeqOut.h"

class OverwriteInstanceName
  : public RTM::RtcLifecycleActionListener
{
public:
  OverwriteInstanceName(int argc, char** argv)
    : m_name(""), m_count(0)
  {
    for (size_t i = 0; i < (size_t)argc; ++i)
      {
        std::string opt = argv[i];
        if (opt.find("--instance_name=") == std::string::npos) { continue; }

        coil::replaceString(opt, "--instance_name=", "");
        if (opt.empty()) { continue; }

        m_name = opt;
      }
  }
  virtual ~OverwriteInstanceName() override {}
  virtual void preCreate(std::string& args) override
  {
    if (m_count != 0 || m_name.empty()) { return; }
    args = args + "?instance_name=" + m_name;
    ++m_count;
  }
  virtual void postCreate(RTC::RTObject_impl*) override {}
  virtual void preConfigure(coil::Properties&) override {}
  virtual void postConfigure(coil::Properties&) override {}
  virtual void preInitialize() override {}
  virtual void postInitialize() override {}
private:
  std::string m_name;
  int32_t m_count;
};

#if defined(RTM_OS_VXWORKS) && !defined(__RTP__)
int seqout_main()
{
  RTC::Manager* manager = &RTC::Manager::instance();
  RTC::RtcBase* comp;
  SeqOutInit(manager);
  comp = manager->createComponent("SeqOut");
  if(comp)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
#else

void MyModuleInit(RTC::Manager* manager)
{
  SeqOutInit(manager);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("SeqOut");

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

int main (int argc, char** argv)
{
  // check parameter
  if(argc > 1)
    {
      for (int i = 1; i < argc; ++i)
        {
#if RTM_MAJOR_VERSION >= 2
          if (coil::normalize(argv[i]) == "-listener")
#else
          std::string arg(argv[i]);
	  coil::normalize(arg);
	  if (arg == "-listener")
#endif
            {
              // Connector Listener Dump Flag ON
              g_Listener_dump_enabled = true;
              std::cout << "Connector Listener Callback Dump start (Port: TimedLong)" << std::endl;
            }
        }
    }

  RTC::Manager* manager;
  manager = RTC::Manager::init(argc, argv);
  manager->addRtcLifecycleActionListener(new OverwriteInstanceName(argc, argv), true);

  // Set module initialization proceduer
  // This procedure will be invoked in activateManager() function.
  manager->setModuleInitProc(MyModuleInit);

  // Activate manager and register to naming service
  manager->activateManager();

  // run the manager in blocking mode
  // runManager(false) is the default.
  manager->runManager();

  // If you want to run the manager in non-blocking mode, do like this
  // manager->runManager(true);

  return 0;
}

#endif
