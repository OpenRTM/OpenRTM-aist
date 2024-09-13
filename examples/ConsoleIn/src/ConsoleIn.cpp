// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  ConsoleIn.cpp
 * @brief Console input component
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include "ConsoleIn.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const consolein_spec[] =
#else
static const char* consolein_spec[] =
#endif
  {
    "implementation_id", "ConsoleIn",
    "type_name",         "ConsoleIn",
    "description",       "Console input component",
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
ConsoleIn::ConsoleIn(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_outOut("out", m_out)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ConsoleIn::~ConsoleIn()
{
}



RTC::ReturnCode_t ConsoleIn::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("out", m_outOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE,
                                    new DataListener("ON_BUFFER_WRITE"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_FULL, 
                                    new DataListener("ON_BUFFER_FULL"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE_TIMEOUT, 
                                    new DataListener("ON_BUFFER_WRITE_TIMEOUT"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_OVERWRITE, 
                                    new DataListener("ON_BUFFER_OVERWRITE"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_READ, 
                                    new DataListener("ON_BUFFER_READ"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_SEND, 
                                    new DataListener("ON_SEND"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVED,
                                    new DataListener("ON_RECEIVED"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_FULL, 
                                    new DataListener("ON_RECEIVER_FULL"));
  m_outOut.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_TIMEOUT, 
                                    new DataListener("ON_RECEIVER_TIMEOUT"));

  m_outOut.addConnectorListener(ConnectorListenerType::ON_BUFFER_EMPTY,
                                    new ConnListener("ON_BUFFER_EMPTY"));
  m_outOut.addConnectorListener(ConnectorListenerType::ON_BUFFER_READ_TIMEOUT,
                                    new ConnListener("ON_BUFFER_READ_TIMEOUT"));
  m_outOut.addConnectorListener(ConnectorListenerType::ON_SENDER_EMPTY,
                                    new ConnListener("ON_SENDER_EMPTY"));
  m_outOut.addConnectorListener(ConnectorListenerType::ON_SENDER_TIMEOUT,
                                    new ConnListener("ON_SENDER_TIMEOUT"));
  m_outOut.addConnectorListener(ConnectorListenerType::ON_SENDER_ERROR,
                                    new ConnListener("ON_SENDER_ERROR"));
  m_outOut.addConnectorListener(ConnectorListenerType::ON_CONNECT,
                                    new ConnListener("ON_CONNECT"));
  m_outOut.addConnectorListener(ConnectorListenerType::ON_DISCONNECT,
                                    new ConnListener("ON_DISCONNECT"));

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ConsoleIn::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t ConsoleIn::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onActivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onDeactivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t ConsoleIn::onExecute(RTC::UniqueId /*ec_id*/)
{
  std::cout << "Please input number: ";
#if defined(RTM_OS_VXWORKS)
  static int count = 0;
  m_out.data = count;
  count++;
#else
  std::cin >> m_out.data;
  if(std::cin.fail())
  {
    std::cin.clear();
    std::cin.ignore(1024, '\n');
    return RTC::RTC_ERROR;
  }
#endif
  if (m_out.data == 666) return RTC::RTC_ERROR;
  m_outOut.write();

  return RTC::RTC_OK;
}


//RTC::ReturnCode_t ConsoleIn::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleIn::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void ConsoleInInit(RTC::Manager* manager)
  {
    coil::Properties profile(consolein_spec);
    manager->registerFactory(profile,
                             RTC::Create<ConsoleIn>,
                             RTC::Delete<ConsoleIn>);
  }
  
}
