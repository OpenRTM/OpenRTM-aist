// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  ConsoleOut.cpp
 * @brief Console output component
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#include "ConsoleOut.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const consoleout_spec[] =
#else
static const char* consoleout_spec[] =
#endif
  {
    "implementation_id", "ConsoleOut",
    "type_name",         "ConsoleOut",
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
ConsoleOut::ConsoleOut(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inIn("in", m_in)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ConsoleOut::~ConsoleOut()
{
}



RTC::ReturnCode_t ConsoleOut::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("in", m_inIn);
  
  // Set OutPort buffer

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE,
                                    new DataListener("ON_BUFFER_WRITE"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_FULL, 
                                    new DataListener("ON_BUFFER_FULL"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE_TIMEOUT, 
                                    new DataListener("ON_BUFFER_WRITE_TIMEOUT"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_OVERWRITE, 
                                    new DataListener("ON_BUFFER_OVERWRITE"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_READ, 
                                    new DataListener("ON_BUFFER_READ"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_SEND, 
                                    new DataListener("ON_SEND"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVED,
                                    new DataListener("ON_RECEIVED"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_FULL, 
                                    new DataListener("ON_RECEIVER_FULL"));
  m_inIn.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_TIMEOUT, 
                                    new DataListener("ON_RECEIVER_TIMEOUT"));

  m_inIn.addConnectorListener(ConnectorListenerType::ON_BUFFER_EMPTY,
                                    new ConnListener("ON_BUFFER_EMPTY"));
  m_inIn.addConnectorListener(ConnectorListenerType::ON_BUFFER_READ_TIMEOUT,
                                    new ConnListener("ON_BUFFER_READ_TIMEOUT"));
  m_inIn.addConnectorListener(ConnectorListenerType::ON_SENDER_EMPTY,
                                    new ConnListener("ON_SENDER_EMPTY"));
  m_inIn.addConnectorListener(ConnectorListenerType::ON_SENDER_TIMEOUT,
                                    new ConnListener("ON_SENDER_TIMEOUT"));
  m_inIn.addConnectorListener(ConnectorListenerType::ON_SENDER_ERROR,
                                    new ConnListener("ON_SENDER_ERROR"));
  m_inIn.addConnectorListener(ConnectorListenerType::ON_CONNECT,
                                    new ConnListener("ON_CONNECT"));
  m_inIn.addConnectorListener(ConnectorListenerType::ON_DISCONNECT,
                                    new ConnListener("ON_DISCONNECT"));

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ConsoleOut::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t ConsoleOut::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onActivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onDeactivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t ConsoleOut::onExecute(RTC::UniqueId /*ec_id*/)
{
  if (m_inIn.isNew())
    {
      m_inIn.read();
      std::cout << "Received: " << m_in.data << std::endl;
      std::cout << "TimeStamp: " << m_in.tm.sec << "[s] ";
      std::cout << m_in.tm.nsec << "[ns]" << std::endl;
    }
#if RTM_MAJOR_VERSION >= 2
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
#else
  coil::usleep(1000);
#endif

  return RTC::RTC_OK;
}


//RTC::ReturnCode_t ConsoleOut::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t ConsoleOut::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void ConsoleOutInit(RTC::Manager* manager)
  {
    coil::Properties profile(consoleout_spec);
    manager->registerFactory(profile,
                             RTC::Create<ConsoleOut>,
                             RTC::Delete<ConsoleOut>);
  }
  
}
