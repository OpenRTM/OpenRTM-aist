// -*- C++ -*-
/*!
 * @file  ConfigSample.cpp
 * @brief Configuration example component
 * $Date: 2007-04-23 07:26:22 $
 *
 * $Id$
 */

#include "ConfigSample.h"
#include "VectorConvert.h"

// Module specification
// <rtc-template block="module_spec">
static const char* configsample_spec[] =
  {
    "implementation_id", "ConfigSample",
    "type_name",         "ConfigSample",
    "description",       "Configuration example component",
    "version",           "1.0",
    "vendor",            "Noriaki Ando, AIST",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.int_param0", "0",
    "conf.default.int_param1", "1",
    "conf.default.double_param0", "0.11",
    "conf.default.double_param1", "9.9",
    "conf.default.str_param0", "hoge",
    "conf.default.str_param1", "dara",
    "conf.default.vector_param0", "0.0,1.0,2.0,3.0,4.0",

    ""
  };
// </rtc-template>

char ticktack()
{
  static int i(0);
  const char* c = "/-\\|";
  i = (i + 1) % 4;
  return c[i];
}


ConfigSample::ConfigSample(RTC::Manager* manager)
  : RTC::DataFlowComponentBase(manager)
    // <rtc-template block="initializer">
    
    // </rtc-template>
{
}

ConfigSample::~ConfigSample()
{
}


RTC::ReturnCode_t ConfigSample::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("int_param0", m_int_param0, "0");
  bindParameter("int_param1", m_int_param1, "1");
  bindParameter("double_param0", m_double_param0, "0.11");
  bindParameter("double_param1", m_double_param1, "9.9");
  bindParameter("str_param0", m_str_param0, "hoge");
  bindParameter("str_param1", m_str_param1, "dara");
  bindParameter("vector_param0", m_vector_param0, "0.0,1.0,2.0,3.0,4.0");
  
  // </rtc-template>

  std::cout << std::endl;
  std::cout << "Please change configuration values from RtcLink" << std::endl; 
  std::cout << std::endl;

  return RTC::RTC_OK;
}



/*
RTC::ReturnCode_t ConfigSample::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ConfigSample::onExecute(RTC::UniqueId  /*ec_id*/)
{
  static size_t maxlen(0);
  size_t curlen(0);
  const char* c = "                    ";
  {
      std::cout << "---------------------------------------" << std::endl;
      std::cout << " Active Configuration Set: ";
      std::cout << m_configsets.getActiveId() << c << std::endl;
      std::cout << "---------------------------------------" << std::endl;
      
      std::cout << "int_param0:       " << m_int_param0 << c << std::endl;
      std::cout << "int_param1:       " << m_int_param1 << c << std::endl;
      std::cout << "double_param0:    " << m_double_param0 << c << std::endl;
      std::cout << "double_param1:    " << m_double_param1 << c << std::endl;
      std::cout << "str_param0:       " << m_str_param0 << c << std::endl;
      std::cout << "str_param1:       " << m_str_param1 << c << std::endl;
      for (size_t i(0), len(m_vector_param0.size()); i < len; ++i)
	  {
	      std::cout << "vector_param0[" << i << "]: ";
	      std::cout << m_vector_param0[i] << c << std::endl;
	  }
      std::cout << "---------------------------------------" << std::endl;

      curlen = m_vector_param0.size();
      maxlen = maxlen > curlen ? maxlen : curlen;
      for (size_t i(0), len(maxlen - curlen); i < len; ++i)
	  {
	      std::cout << c << c << std::endl;
	  }

      std::cout << "Updating.... " << ticktack() << c << std::endl;

      for (size_t i(0), len(11 + maxlen); i < len; ++i)
	  {
	    std::cout << "[A\r";
	  }
    }
  if (m_int_param0 > 1000 && m_int_param0 < 1000000)
    {
      coil::usleep(m_int_param0);
    }
  else
    {
      coil::usleep(100000);
    }
  return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t ConfigSample::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ConfigSample::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ConfigSampleInit(RTC::Manager* manager)
  {
    coil::Properties profile(configsample_spec);
    manager->registerFactory(profile,
                             RTC::Create<ConfigSample>,
                             RTC::Delete<ConfigSample>);
  }
  
};


