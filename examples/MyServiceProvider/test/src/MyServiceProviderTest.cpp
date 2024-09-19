// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  MyServiceProviderTest.cpp
 * @brief MyService Provider Sample component (test code)
 *
 */
// </rtc-template>

#include "MyServiceProviderTest.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const myserviceprovider_spec[] =
#else
static const char* myserviceprovider_spec[] =
#endif
  {
    "implementation_id", "MyServiceProviderTest",
    "type_name",         "MyServiceProviderTest",
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
MyServiceProviderTest::MyServiceProviderTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_MyServicePort("MyService")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
MyServiceProviderTest::~MyServiceProviderTest()
{
}



RTC::ReturnCode_t MyServiceProviderTest::onInitialize()
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
RTC::ReturnCode_t MyServiceProviderTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t MyServiceProviderTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProviderTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t MyServiceProviderTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceProviderTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceProviderTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t MyServiceProviderTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProviderTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProviderTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProviderTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceProviderTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool MyServiceProviderTest::runTest()
{
    return true;
}


extern "C"
{
 
  void MyServiceProviderTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(myserviceprovider_spec);
    manager->registerFactory(profile,
                             RTC::Create<MyServiceProviderTest>,
                             RTC::Delete<MyServiceProviderTest>);
  }
  
}
