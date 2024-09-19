// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  MyServiceProvider.cpp
 * @brief MyService Provider Sample component
 *
 */
// </rtc-template>

#include "MyServiceProvider.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const myserviceprovider_spec[] =
#else
static const char* myserviceprovider_spec[] =
#endif
  {
    "implementation_id", "MyServiceProvider",
    "type_name",         "MyServiceProvider",
    "description",       "MyService Provider Sample component",
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
MyServiceProvider::MyServiceProvider(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_MyServicePort("MyService")
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
MyServiceProvider::~MyServiceProvider()
{
}



RTC::ReturnCode_t MyServiceProvider::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer

  
  // Set service provider to Ports
  m_MyServicePort.registerProvider("myservice", "SimpleService::MyService", m_myservice);
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  addPort(m_MyServicePort);
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t MyServiceProvider::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t MyServiceProvider::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProvider::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t MyServiceProvider::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceProvider::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceProvider::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t MyServiceProvider::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProvider::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProvider::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProvider::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProvider::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void MyServiceProviderInit(RTC::Manager* manager)
  {
    coil::Properties profile(myserviceprovider_spec);
    manager->registerFactory(profile,
                             RTC::Create<MyServiceProvider>,
                             RTC::Delete<MyServiceProvider>);
  }
  
}
