// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  MyServiceConsumer.cpp
 * @brief MyService Consumer Sample component
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include "MyServiceConsumer.h"
#include <rtm/CORBA_SeqUtil.h>
#include <vector>
#include <cstdlib>
#include <thread>
#include <functional>

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const myserviceconsumer_spec[] =
#else
static const char* myserviceconsumer_spec[] =
#endif
  {
    "implementation_id", "MyServiceConsumer",
    "type_name",         "MyServiceConsumer",
    "description",       "MyService Consumer Sample component",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "example",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
MyServiceConsumer::MyServiceConsumer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_MyServicePort("MyService"),
    // </rtc-template>
    async_echo(nullptr)
{
}

/*!
 * @brief destructor
 */
MyServiceConsumer::~MyServiceConsumer()
{
    if (async_echo != nullptr)
    {
        async_echo->join();
        delete async_echo;
    }

    for (auto async : async_set_value)
    {
        async->join();
        delete async;
    }
}



RTC::ReturnCode_t MyServiceConsumer::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  m_MyServicePort.registerConsumer("myservice", "SimpleService::MyService", m_myservice);
  
  // Set CORBA Service Ports
  addPort(m_MyServicePort);
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t MyServiceConsumer::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t MyServiceConsumer::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumer::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t MyServiceConsumer::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceConsumer::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceConsumer::onExecute(RTC::UniqueId /*ec_id*/)
{
  try
    {
      std::cout << std::endl;
      std::cout << "Command list: " << std::endl;
      std::cout << " echo [msg]       : echo message." << std::endl;
      std::cout << " set_value [value]: set value." << std::endl;
      std::cout << " get_value        : get current value." << std::endl;
      std::cout << " get_echo_history : get input messsage history." << std::endl;
      std::cout << " get_value_history: get input value history." << std::endl;
      std::cout << "> ";

      std::string args;
      std::string::size_type pos;
      std::vector<std::string> argv;
#if defined(RTM_OS_VXWORKS)
      args = "echo test";
#else
      std::getline(std::cin, args);
#endif

      pos = args.find_first_of(' ');
      if (pos != std::string::npos)
        {
          argv.emplace_back(args.substr(0, pos));
          argv.emplace_back(args.substr(++pos));
        }
      else
        {
          argv.emplace_back(std::move(args));
        }

      if (async_echo != nullptr && async_finished)
        {
          std::cout << "echo() finished: " <<  m_result << std::endl;
          async_echo->join();
          delete async_echo;
          async_echo = nullptr;
        }
      
      if (argv[0] == "echo" && argv.size() > 1)
	    {
          if (async_echo == nullptr)
            {
              async_finished = false;
              std::string msg(argv[1]);
              async_echo = new std::thread(
                  [&, msg] {
                      echo(msg, m_result, m_myservice);
                      async_finished = true;
                  });
            }
          else
            {
              std::cout << "echo() still invoking" << std::endl;
            }
	      return RTC::RTC_OK;
	    }
      
      if (argv[0] == "set_value" && argv.size() > 1)
	    {
          CORBA::Float val(static_cast<CORBA::Float>(atof(argv[1].c_str())));

          async_set_value.push_back(new std::thread(
              [&, val] {
                  set_value(val, m_myservice);
              }));
          std::cout << "Set remote value: " << val << std::endl;

          return RTC::RTC_OK;
	    }
      
      if (argv[0] == "get_value")
	    {
	  std::cout << "Current remote value: "
		    << m_myservice->get_value() << std::endl;
	  return RTC::RTC_OK;
	    }
      
      if (argv[0] == "get_echo_history")
	    {
	  CORBA_SeqUtil::for_each(*(m_myservice->get_echo_history()),
				  seq_print<const char*>());
	  return RTC::RTC_OK;
	    }
      
      if (argv[0] == "get_value_history")
	    {
	  CORBA_SeqUtil::for_each(*(m_myservice->get_value_history()),
				  seq_print<CORBA::Float>());
	  return RTC::RTC_OK;
	    }
      
      std::cout << "Invalid command or argument(s)." << std::endl;
    }
  catch (...)
    {
      std::cout << "No service connected." << std::endl;
    }
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t MyServiceConsumer::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumer::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumer::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumer::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumer::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void MyServiceConsumerInit(RTC::Manager* manager)
  {
    coil::Properties profile(myserviceconsumer_spec);
    manager->registerFactory(profile,
                             RTC::Create<MyServiceConsumer>,
                             RTC::Delete<MyServiceConsumer>);
  }
  
}
