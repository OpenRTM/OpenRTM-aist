// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  SeqOut.cpp
 * @brief Sequence OutPort component
 *
 */
// </rtc-template>

#include "SeqOut.h"
#include <cstdlib>

// Connector Listener Dump Flag
bool g_Listener_dump_enabled = false;

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const seqout_spec[] =
#else
static const char* seqout_spec[] =
#endif
  {
    "implementation_id", "SeqOut",
    "type_name",         "SeqOut",
    "description",       "Sequence OutPort component",
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
SeqOut::SeqOut(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_OctetOut("Octet", m_Octet),
    m_ShortOut("Short", m_Short),
    m_LongOut("Long", m_Long),
    m_FloatOut("Float", m_Float),
    m_DoubleOut("Double", m_Double),
    m_OctetSeqOut("OctetSeq", m_OctetSeq),
    m_ShortSeqOut("ShortSeq", m_ShortSeq),
    m_LongSeqOut("LongSeq", m_LongSeq),
    m_FloatSeqOut("FloatSeq", m_FloatSeq),
    m_DoubleSeqOut("DoubleSeq", m_DoubleSeq)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
SeqOut::~SeqOut()
{
}



RTC::ReturnCode_t SeqOut::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("Octet", m_OctetOut);
  addOutPort("Short", m_ShortOut);
  addOutPort("Long", m_LongOut);
  addOutPort("Float", m_FloatOut);
  addOutPort("Double", m_DoubleOut);
  addOutPort("OctetSeq", m_OctetSeqOut);
  addOutPort("ShortSeq", m_ShortSeqOut);
  addOutPort("LongSeq", m_LongSeqOut);
  addOutPort("FloatSeq", m_FloatSeqOut);
  addOutPort("DoubleSeq", m_DoubleSeqOut);

  // check m_LongOut port only
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE,
                                    new DataListener("ON_BUFFER_WRITE"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_FULL, 
                                    new DataListener("ON_BUFFER_FULL"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE_TIMEOUT, 
                                    new DataListener("ON_BUFFER_WRITE_TIMEOUT"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_OVERWRITE, 
                                    new DataListener("ON_BUFFER_OVERWRITE"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_READ, 
                                    new DataListener("ON_BUFFER_READ"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_SEND, 
                                    new DataListener("ON_SEND"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVED,
                                    new DataListener("ON_RECEIVED"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_FULL, 
                                    new DataListener("ON_RECEIVER_FULL"));
  m_LongOut.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_TIMEOUT, 
                                    new DataListener("ON_RECEIVER_TIMEOUT"));

  m_LongOut.addConnectorListener(ConnectorListenerType::ON_BUFFER_EMPTY,
                                    new ConnListener("ON_BUFFER_EMPTY"));
  m_LongOut.addConnectorListener(ConnectorListenerType::ON_BUFFER_READ_TIMEOUT,
                                    new ConnListener("ON_BUFFER_READ_TIMEOUT"));
  m_LongOut.addConnectorListener(ConnectorListenerType::ON_SENDER_EMPTY,
                                    new ConnListener("ON_SENDER_EMPTY"));
  m_LongOut.addConnectorListener(ConnectorListenerType::ON_SENDER_TIMEOUT,
                                    new ConnListener("ON_SENDER_TIMEOUT"));
  m_LongOut.addConnectorListener(ConnectorListenerType::ON_SENDER_ERROR,
                                    new ConnListener("ON_SENDER_ERROR"));
  m_LongOut.addConnectorListener(ConnectorListenerType::ON_CONNECT,
                                    new ConnListener("ON_CONNECT"));
  m_LongOut.addConnectorListener(ConnectorListenerType::ON_DISCONNECT,
                                    new ConnListener("ON_DISCONNECT"));  

  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  m_DoubleSeq.data.length(10);
  m_FloatSeq.data.length(10);
  m_LongSeq.data.length(10);
  m_ShortSeq.data.length(10);
  m_OctetSeq.data.length(10);

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("data_type", m_data_type, "serial");

  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SeqOut::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t SeqOut::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onActivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onDeactivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t SeqOut::onExecute(RTC::UniqueId /*ec_id*/)
{
  static unsigned char cnt0, cntseq0;
  static short  cnt1, cntseq1;
  static long   cnt2, cntseq2;
  static float  cnt3, cntseq3;
  static double cnt4, cntseq4;

  // Configuration check
  if (m_data_type == "serial")
    {
      ++cnt0;
      if( cnt0 > 254 )
        {
          cnt0 = 1;
        }
      m_Octet.data = cnt0;

      ++cnt1;
      if( cnt1 > 30000 )
        {
          cnt1 = 1;
        }
      m_Short.data = cnt1;

      ++cnt2;
      if( cnt2 > 30000 )
        {
          cnt2 = 1;
        }
      m_Long.data = cnt2;

      ++cnt3;
      if( cnt3 > 30000 )
        {
          cnt3 = 1.0;
        }
      m_Float.data = cnt3 + static_cast<CORBA::Float>(0.12);

      ++cnt4;
      if( cnt4 > 30000 )
        {
          cnt4 = 1.0;
        }
      m_Double.data = cnt4 + 0.345;
    }
  else				//default
    {
      m_Octet.data = static_cast<CORBA::Octet>(rand());
      m_Short.data = static_cast<CORBA::Short>(rand());
      m_Long.data = static_cast<CORBA::Long>(rand());
      m_Float.data = static_cast<CORBA::Float>(rand());
      m_Double.data = static_cast<CORBA::Double>(rand());
    }

  std::cout << "-: ";
  std::cout << m_Double.data << " ";
  std::cout << m_Float.data << " ";
  std::cout << m_Long.data << " ";
  std::cout << m_Short.data << " ";
  std::cout << int(m_Octet.data) << "[";
  if (int(m_Octet.data) < 0x20 || int(m_Octet.data) > 0x7e)
    {
      std::cout << " " << "]";
    }
  else
    {
      std::cout << m_Octet.data << "]";
    }
  std::cout << "                                " << std::endl << std::endl;

  for (CORBA::ULong i = 0; i < 10; ++i)
    {
      // Configuration check
      if (m_data_type == "serial")
        {
          ++cntseq0;
          if( cntseq0 > 254 )
            {
              cntseq0 = 1;
            }
          m_OctetSeq.data[i] = cntseq0;

          ++cntseq1;
          if( cntseq1 > 30000 )
            {
              cntseq1 = 1;
            }
          m_ShortSeq.data[i] = cntseq1;

          ++cntseq2;
          if( cntseq2 > 30000 )
            {
              cntseq2 = 1;
            }
          m_LongSeq.data[i] = cntseq2;

          ++cntseq3;
          if( cntseq3 > 30000 )
            {
              cntseq3 = 1.0;
            }
          m_FloatSeq.data[i] = cntseq3 + static_cast<CORBA::Float>(0.12);

          ++cntseq4;
          if( cntseq4 > 30000 )
            {
              cntseq4 = 1.0;
            }
          m_DoubleSeq.data[i] = cntseq4 + 0.345;
        }
      else				//default
        {
          m_DoubleSeq.data[i] = static_cast<double>(rand());
          m_FloatSeq.data[i] = static_cast<float>(rand());
          m_LongSeq.data[i] = static_cast<long>(rand());
          m_ShortSeq.data[i] = static_cast<short>(rand());
          m_OctetSeq.data[i] = static_cast<unsigned char>(rand());
        }

      std::cout << i << ": ";
      std::cout << m_DoubleSeq.data[i] << " ";
      std::cout << m_FloatSeq.data[i] << " ";
      std::cout << m_LongSeq.data[i] << " ";
      std::cout << m_ShortSeq.data[i] << " ";
      std::cout << int(m_OctetSeq.data[i]) << "[";
      if (int(m_OctetSeq.data[i]) < 0x20 || int(m_OctetSeq.data[i]) > 0x7e)
        {
          std::cout << " " << "]";
        }
      else
        {
          std::cout << m_OctetSeq.data[i] << "]";
        }
      std::cout << "                             " << std::endl;
    }
  // Configuration check
  if (m_data_type == "serial")
    {
      std::cout << std::endl << "write data type : serial" << std::endl;
    }
  else				//default
    {
      std::cout << std::endl << "write data type : random" << std::endl;
    }

  // Connector Listener Dump check
  if(!g_Listener_dump_enabled)
    {
      std::cout << "[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r[A\r";
    }

  m_OctetOut.write();
  m_ShortOut.write();
  m_LongOut.write();
  m_FloatOut.write();
  m_DoubleOut.write();

  m_OctetSeqOut.write();
  m_ShortSeqOut.write();
  m_LongSeqOut.write();
  m_FloatSeqOut.write();
  m_DoubleSeqOut.write();

  // Connector Listener Dump check
  if(g_Listener_dump_enabled)
    {
#if RTM_MAJOR_VERSION >= 2
      std::this_thread::sleep_for(std::chrono::seconds(1));
#else
      coil::usleep(1000000);
#endif
    }
  else
    {
#if RTM_MAJOR_VERSION >= 2
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
#else
      coil::usleep(200000);
#endif
    }

  return RTC::RTC_OK;
}


//RTC::ReturnCode_t SeqOut::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqOut::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void SeqOutInit(RTC::Manager* manager)
  {
    coil::Properties profile(seqout_spec);
    manager->registerFactory(profile,
                             RTC::Create<SeqOut>,
                             RTC::Delete<SeqOut>);
  }
  
}
