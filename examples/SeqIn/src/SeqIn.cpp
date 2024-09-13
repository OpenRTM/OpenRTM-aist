// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  SeqIn.cpp
 * @brief Sequence InPort component
 *
 */
// </rtc-template>

#include "SeqIn.h"
#include <iostream>

// Connector Listener Dump Flag
bool g_Listener_dump_enabled = false;

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const seqin_spec[] =
#else
static const char* seqin_spec[] =
#endif
  {
    "implementation_id", "SeqIn",
    "type_name",         "SeqIn",
    "description",       "Sequence InPort component",
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
SeqIn::SeqIn(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_OctetIn("Octet", m_Octet),
    m_ShortIn("Short", m_Short),
    m_LongIn("Long", m_Long),
    m_FloatIn("Float", m_Float),
    m_DoubleIn("Double", m_Double),
    m_OctetSeqIn("OctetSeq", m_OctetSeq),
    m_ShortSeqIn("ShortSeq", m_ShortSeq),
    m_LongSeqIn("LongSeq", m_LongSeq),
    m_FloatSeqIn("FloatSeq", m_FloatSeq),
    m_DoubleSeqIn("DoubleSeq", m_DoubleSeq)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
SeqIn::~SeqIn()
{
}



RTC::ReturnCode_t SeqIn::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("Octet", m_OctetIn);
  addInPort("Short", m_ShortIn);
  addInPort("Long", m_LongIn);
  addInPort("Float", m_FloatIn);
  addInPort("Double", m_DoubleIn);
  addInPort("OctetSeq", m_OctetSeqIn);
  addInPort("ShortSeq", m_ShortSeqIn);
  addInPort("LongSeq", m_LongSeqIn);
  addInPort("FloatSeq", m_FloatSeqIn);
  addInPort("DoubleSeq", m_DoubleSeqIn);
  
  // check m_LongIn port only
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE,
                                    new DataListener("ON_BUFFER_WRITE"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_FULL, 
                                    new DataListener("ON_BUFFER_FULL"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_WRITE_TIMEOUT, 
                                    new DataListener("ON_BUFFER_WRITE_TIMEOUT"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_OVERWRITE, 
                                    new DataListener("ON_BUFFER_OVERWRITE"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_BUFFER_READ, 
                                    new DataListener("ON_BUFFER_READ"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_SEND, 
                                    new DataListener("ON_SEND"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVED,
                                    new DataListener("ON_RECEIVED"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_FULL, 
                                    new DataListener("ON_RECEIVER_FULL"));
  m_LongIn.addConnectorDataListener(ConnectorDataListenerType::ON_RECEIVER_TIMEOUT, 
                                    new DataListener("ON_RECEIVER_TIMEOUT"));

  m_LongIn.addConnectorListener(ConnectorListenerType::ON_BUFFER_EMPTY,
                                    new ConnListener("ON_BUFFER_EMPTY"));
  m_LongIn.addConnectorListener(ConnectorListenerType::ON_BUFFER_READ_TIMEOUT,
                                    new ConnListener("ON_BUFFER_READ_TIMEOUT"));
  m_LongIn.addConnectorListener(ConnectorListenerType::ON_SENDER_EMPTY,
                                    new ConnListener("ON_SENDER_EMPTY"));
  m_LongIn.addConnectorListener(ConnectorListenerType::ON_SENDER_TIMEOUT,
                                    new ConnListener("ON_SENDER_TIMEOUT"));
  m_LongIn.addConnectorListener(ConnectorListenerType::ON_SENDER_ERROR,
                                    new ConnListener("ON_SENDER_ERROR"));
  m_LongIn.addConnectorListener(ConnectorListenerType::ON_CONNECT,
                                    new ConnListener("ON_CONNECT"));
  m_LongIn.addConnectorListener(ConnectorListenerType::ON_DISCONNECT,
                                    new ConnListener("ON_DISCONNECT"));

  // Set OutPort buffer

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t SeqIn::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t SeqIn::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onActivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onDeactivated(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t SeqIn::onExecute(RTC::UniqueId /*ec_id*/)
{
  m_DoubleIn.read();
  m_FloatIn.read();
  m_LongIn.read();
  m_ShortIn.read();
  m_OctetIn.read();
  m_DoubleSeqIn.read();
  m_FloatSeqIn.read();
  m_LongSeqIn.read();
  m_ShortSeqIn.read();
  m_OctetSeqIn.read();

  int h_col(4);
  int col(13);
  int all_col = h_col + (col * 5);
  int all_row(0);

  int d_size(1);
  int f_size(1);
  int l_size(1);
  int s_size(1);
  int o_size(1);

  int ds_size(m_DoubleSeq.data.length());
  int fs_size(m_FloatSeq.data.length());
  int ls_size(m_LongSeq.data.length());
  int ss_size(m_ShortSeq.data.length());
  int os_size(m_OctetSeq.data.length());

  int max_size;

  std::vector<int> in_size;
  std::vector<int>::iterator it;
  in_size.reserve(10);
  in_size.emplace_back(d_size);
  in_size.emplace_back(f_size);
  in_size.emplace_back(l_size);
  in_size.emplace_back(s_size);
  in_size.emplace_back(o_size);
  in_size.emplace_back(ds_size);
  in_size.emplace_back(fs_size);
  in_size.emplace_back(ls_size);
  in_size.emplace_back(ss_size);
  in_size.emplace_back(os_size);

  it = std::max_element(in_size.begin(), in_size.end());

  max_size = *it;

  std::cout.width(h_col + (col * 5));
  std::cout.fill('-');
  std::cout << "-" << std::endl; all_row++;

  std::cout.width(h_col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << " ";
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << "Double";
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << "Float";
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << "Long";
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << "Short";
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << "Octet";
  std::cout << std::endl; all_row++;

  std::cout.width(all_col);
  std::cout.fill('-');
  std::cout << "-" << std::endl;  all_row++;

  for (int i = 0; i < (all_col - 10)/ 2; i++)
    std::cout << " ";
  std::cout << "Basic type";
  for (int i = 0; i < (all_col - 10)/ 2; i++)
    std::cout << " ";
  std::cout << std::endl; all_row++;

  std::cout.width(h_col + (col * 5));
  std::cout.fill('-');
  std::cout << "-" << std::endl; all_row++;

  std::cout.width(h_col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << " ";   
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << m_Double.data;
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << m_Float.data;
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << m_Long.data;
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << m_Short.data;
  std::cout.width(col);
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout.fill(' ');
  std::cout << int(m_Octet.data) << "[";
  if (int(m_Octet.data) < 0x20 || int(m_Octet.data) > 0x7e)
    {
      std::cout << " " << "]";
    }
  else
    {
      std::cout << m_Octet.data << "]";
    }
  std::cout << std::endl; all_row++;
  
  std::cout.width(all_col);
  std::cout.fill('-');
  std::cout << "-" << std::endl; all_row++;
  
  for (int i = 0; i < (all_col - 13)/ 2; i++)
    std::cout << " ";
  std::cout << "Sequence type";
  for (int i = 0; i < (all_col - 13)/ 2; i++)
    std::cout << " ";
  std::cout << std::endl; all_row++;
  
  std::cout.width(all_col);
  std::cout.fill('-');
  std::cout << "-" << std::endl; all_row++;

  for (int i = 0; i < max_size; i++)
    {
      std::cout.width(h_col - 2);
      std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::cout.fill(' ');
      std::cout << i;
      std::cout.width(2);
      std::cout << ": ";
      
      std::cout.width(col);
      std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::cout.fill(' ');
      if (i < ds_size)
	{
	  std::cout << m_DoubleSeq.data[i];
	}
      else
	{
	  std::cout << "-";
	}
      
      std::cout.width(col);
      std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::cout.fill(' ');
      if (i < fs_size)
	{
	  std::cout << m_FloatSeq.data[i];
	}
      else
	{
	  std::cout << "-";
	}
      
      std::cout.width(col);
      std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::cout.fill(' ');
      if (i < ls_size)
	{
	  std::cout << m_LongSeq.data[i];
	}
      else
	{
	  std::cout << "-";
	}
      
      std::cout.width(col);
      std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::cout.fill(' ');
      if (i < ss_size)
	{
	  std::cout << m_ShortSeq.data[i];
	}
      else
	{
	  std::cout << "-";
	}

      std::cout.width(col);
      std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::cout.fill(' ');
      if (i < os_size)
       {
         std::cout << int(m_OctetSeq.data[i]) << "[";
         if (int(m_OctetSeq.data[i]) < 0x20 || int(m_OctetSeq.data[i]) > 0x7e)
           {
             std::cout << " " << "]";
           }
         else
           {
             std::cout << m_OctetSeq.data[i] << "]";
           }
       }
      else
       {
         std::cout << "-";
       }
         
      std::cout << std::endl; all_row++;
    }
  
  // Connector Listener Dump check
  if(!g_Listener_dump_enabled)
    {
      for (int i = 0 ; i < all_row; i++)
        {
          std::cout << "[A\r";
        }
    }

  return RTC::RTC_OK;
}


//RTC::ReturnCode_t SeqIn::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t SeqIn::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void SeqInInit(RTC::Manager* manager)
  {
    coil::Properties profile(seqin_spec);
    manager->registerFactory(profile,
                             RTC::Create<SeqIn>,
                             RTC::Delete<SeqIn>);
  }
  
}
