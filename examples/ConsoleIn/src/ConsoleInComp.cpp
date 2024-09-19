// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file ConsoleInComp.cpp
 * @brief Standalone component
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "ConsoleIn.h"
#include <rtm/NVUtil.h>

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
int consolein_main()
{
  RTC::Manager* manager = &RTC::Manager::instance();
  RTC::RtcBase* comp;
  ConsoleInInit(manager);
  comp = manager->createComponent("ConsoleIn");
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
  ConsoleInInit(manager);
  RTC::RtcBase* comp;

  // Create a component
  std::cout << "Creating a component: \"ConsoleIn\"....";
  comp = manager->createComponent("ConsoleIn");
  std::cout << "succeed." << std::endl;

  if (comp==nullptr)
  {
    std::cerr << "Component create failed." << std::endl;
    abort();
  }

  RTC::ComponentProfile_var prof;
  prof = comp->get_component_profile();
  std::cout << "=================================================" << std::endl;
  std::cout << " Component Profile" << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "InstanceID:     " << prof->instance_name << std::endl;
  std::cout << "Implementation: " << prof->type_name << std::endl;
  std::cout << "Description:    " << prof->description << std::endl;
  std::cout << "Version:        " << prof->version << std::endl;
  std::cout << "Maker:          " << prof->vendor << std::endl;
  std::cout << "Category:       " << prof->category << std::endl;
  std::cout << "  Other properties   " << std::endl;
  NVUtil::dump(prof->properties);
  std::cout << "=================================================" << std::endl;

  PortServiceList_var portlist;
  portlist = comp->get_ports();

  for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i)
    {
      PortService_var port;
      port = PortService::_duplicate(portlist[i]);
      RTC::PortProfile_var portprof = port->get_port_profile();
      std::cout << "================================================="
		<< std::endl;
      std::cout << "Port" << i << " (name): ";
      std::cout << portprof->name << std::endl;
      std::cout << "-------------------------------------------------"
		<< std::endl;    
      RTC::PortInterfaceProfileList iflist;
      iflist = portprof->interfaces;

      for (CORBA::ULong j(0), m(iflist.length()); j < m; ++j)
	    {
	      std::cout << "I/F name: ";
	      std::cout << iflist[j].instance_name << std::endl;
	      std::cout << "I/F type: ";
	      std::cout << iflist[j].type_name << std::endl;
	      const char* pol;
	      pol = iflist[j].polarity == 0 ? "PROVIDED" : "REQUIRED";
	      std::cout << "Polarity: " << pol << std::endl;
	    }
      std::cout << "- properties -" << std::endl;
      NVUtil::dump(portprof->properties);
      std::cout << "-------------------------------------------------" << std::endl;
    }

  return;
}

int main (int argc, char** argv)
{
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
