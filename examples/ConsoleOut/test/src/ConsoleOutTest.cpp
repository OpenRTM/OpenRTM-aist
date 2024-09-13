// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  ConsoleOutTest.cpp
 * @brief Console output component (test code)
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include "ConsoleOutTest.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const consoleout_spec[] =
#else
static const char* consoleout_spec[] =
#endif
  {
    "implementation_id", "ConsoleOutTest",
    "type_name",         "ConsoleOutTest",
    "description",       "Console output component",
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
ConsoleOutTest::ConsoleOutTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inOut("in", m_in)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ConsoleOutTest::~ConsoleOutTest()
{
}



RTC::ReturnCode_t ConsoleOutTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("in", m_inOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ConsoleOutTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t ConsoleOutTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onActivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onDeactivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t ConsoleOutTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t ConsoleOutTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOutTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool ConsoleOutTest::runTest()
{
    return true;
}


extern "C"
{
 
  void ConsoleOutTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(consoleout_spec);
    manager->registerFactory(profile,
                             RTC::Create<ConsoleOutTest>,
                             RTC::Delete<ConsoleOutTest>);
  }
  
}
