// -*- C++ -*-
/*!
 * @file  Inputbutton.cpp
 * @brief Console input component
 * $Date: 2007-10-09 07:33:04 $
 *
 * $Id: Inputbutton.cpp 1778 2010-01-25 10:29:27Z hakuta $
 */

#include "Inputbutton.h"
#include <iostream>

// Module specification
// <rtc-template block="module_spec">
static const char* consolein_spec[] =
  {
    "implementation_id", "Inputbutton",
    "type_name",         "Inputbutton",
    "description",       "Console input component",
    "version",           "1.0",
    "vendor",            "Noriaki Ando, AIST",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

Inputbutton::Inputbutton(RTC::Manager* manager)
  : RTC::DataFlowComponentBase(manager),
    // <rtc-template block="initializer">
    m_openOut("open", m_open),
    m_closeOut("close", m_close),
    m_minuteOut("minute", m_minute),
    m_startOut("start", m_start),
    m_stopOut("stop", m_stop),
    m_tickOut("tick", m_tick)
    // </rtc-template>
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

}

Inputbutton::~Inputbutton()
{
}


RTC::ReturnCode_t Inputbutton::onInitialize()
{
  addOutPort("open",   m_openOut);
  addOutPort("close",  m_closeOut);
  addOutPort("minute", m_minuteOut);
  addOutPort("start",  m_startOut);
  addOutPort("stop",   m_stopOut);
  addOutPort("tick",   m_tickOut);

  return RTC::RTC_OK;
}
RTC::ReturnCode_t Inputbutton::onExecute(RTC::UniqueId  /*ec_id*/)
{
  std::string cmd;
  std::cout << std::endl;
  std::cout << "Please select action!!"                       << std::endl;
  std::cout << "Commands: "                                   << std::endl;
  std::cout << "  open         : Open the microwave's door."  << std::endl;
  std::cout << "  close        : Close the microwave's door." << std::endl;
  std::cout << "  minute <int> : Increment timer. "           << std::endl;
  std::cout << "  start        : Start the microwave."        << std::endl;
  std::cout << "  stop         : Stop the microwave."         << std::endl;
  std::cout << "  tick         : Proceed time."               << std::endl;
  std::cout << "  -> others are interpreted as tick commnad." << std::endl;
  std::cout << ">> ";
  std::getline(std::cin, cmd);

  coil::vstring cmds = coil::split(cmd, " ");
  coil::eraseBlank(cmds[0]);

  std::cout << "[command]: " << cmds[0];
  if (cmds.size() > 1)
    {
      std::cout << "  [args]: ";
      for(auto & c : cmds)
        {
          std::cout << c << " ";
        }
   }
  std::cout << std::endl;

  if      (cmds[0] == "open")  { m_open.data = 0;  m_openOut.write(); }
  else if (cmds[0] == "close") { m_close.data = 0; m_closeOut.write(); }
  else if (cmds[0] == "minute")
    {
      CORBA::Long min;
      if (cmds.size() < 2 || !coil::stringTo(min, cmds[1].c_str()))
        {
          std::cout << "minute command needs an integer arg." << std::endl;
          return RTC::RTC_OK;
        }
      m_minute.data = min; m_minuteOut.write();
    }
  else if (cmds[0] == "start") { m_start.data = 0; m_startOut.write(); }
  else if (cmds[0] == "stop")  { m_stop.data = 0;  m_stopOut.write(); }
  else                         { std::cout << "tick" << std::endl; m_tick.data = 0;  m_tickOut.write(); }

  return RTC::RTC_OK;
}


extern "C"
{
 
  void InputbuttonInit(RTC::Manager* manager)
  {
    RTC::Properties profile(consolein_spec);
    manager->registerFactory(profile,
                             RTC::Create<Inputbutton>,
                             RTC::Delete<Inputbutton>);
  }
  
}


