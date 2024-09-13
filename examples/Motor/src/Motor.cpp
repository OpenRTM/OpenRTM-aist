// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  Motor.cpp
 * @brief Motor component
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include "Motor.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const motor_spec[] =
#else
static const char* motor_spec[] =
#endif
  {
    "implementation_id", "Motor",
    "type_name",         "Motor",
    "description",       "Motor component",
    "version",           "2.0.0",
    "vendor",            "AIST",
    "category",          "example",
    "activity_type",     "SPORADIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.motor_id", "0",

    // Widget
    "conf.__widget__.motor_id", "text",
    // Constraints

    "conf.__type__.motor_id", "int",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Motor::Motor(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inIn("in", m_in),
    m_outOut("out", m_out)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Motor::~Motor()
{
}



RTC::ReturnCode_t Motor::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("in", m_inIn);
  
  // Set OutPort buffer
  addOutPort("out", m_outOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("motor_id", m_motor_id, "0");

  m_configsets.update("default");

  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Motor::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t Motor::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onActivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onDeactivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t Motor::onExecute(RTC::UniqueId /*ec_id*/)
{
  if (m_inIn.isNew()) {
    m_inIn.read();
    std::cout << "Motor Received data: " << m_in.data << std::endl << std::endl;
    m_out.data = static_cast<CORBA::Long>(m_in.data) * 2;
    m_outOut.write();
  }
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t Motor::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Motor::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void MotorInit(RTC::Manager* manager)
  {
    coil::Properties profile(motor_spec);
    manager->registerFactory(profile,
                             RTC::Create<Motor>,
                             RTC::Delete<Motor>);
  }
  
}
