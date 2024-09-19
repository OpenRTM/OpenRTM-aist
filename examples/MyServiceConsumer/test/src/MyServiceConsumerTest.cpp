// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  MyServiceConsumerTest.cpp
 * @brief MyService Consumer Sample component (test code)
 *
 */
// </rtc-template>

#include "MyServiceConsumerTest.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const myserviceconsumer_spec[] =
#else
static const char* myserviceconsumer_spec[] =
#endif
  {
    "implementation_id", "MyServiceConsumerTest",
    "type_name",         "MyServiceConsumerTest",
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
MyServiceConsumerTest::MyServiceConsumerTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_MyServicePort("MyService")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
MyServiceConsumerTest::~MyServiceConsumerTest()
{
}



RTC::ReturnCode_t MyServiceConsumerTest::onInitialize()
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
RTC::ReturnCode_t MyServiceConsumerTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t MyServiceConsumerTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumerTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t MyServiceConsumerTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceConsumerTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MyServiceConsumerTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t MyServiceConsumerTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumerTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumerTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumerTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t MyServiceConsumerTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool MyServiceConsumerTest::runTest()
{
    return true;
}


extern "C"
{
 
  void MyServiceConsumerTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(myserviceconsumer_spec);
    manager->registerFactory(profile,
                             RTC::Create<MyServiceConsumerTest>,
                             RTC::Delete<MyServiceConsumerTest>);
  }
  
}
