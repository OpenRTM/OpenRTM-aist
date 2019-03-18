// A simple simulation of an microwave oven:
// Timer can be programmed in minutes, last mode will be resumed if opened.
//
// Compile like this:
// g++ Macho.cpp Microwave.cpp


#include <iostream>
#include "MicrowaveFsm.h"

namespace MicrowaveFsm
{
  //============================================================
  // Top state
  RTC::ReturnCode_t Top::onInit()
  {
    std::cout << "[Microwave] TOP::onInit()" << std::endl;
    setState<Operational>();
    return RTC::RTC_OK;
  }
  RTC::ReturnCode_t Top::onEntry()
  {
    std::cout << "[Microwave] TOP::onEntry()" << std::endl;
    return RTC::RTC_OK;
  }
  RTC::ReturnCode_t Top::onExit()
  {
    std::cout << "[Microwave] TOP::onExit()" << std::endl;
    return RTC::RTC_OK;
  }

  //============================================================
  // State Disabled
  void Disabled::close()
  {
    std::cout << "[Microwave] >>> Door closed <<<" << std::endl;
    setState<Operational>();
  }
  RTC::ReturnCode_t Disabled::onEntry()
  {
    std::cout << "[Microwave] Disabled::onEntry()" << std::endl;
    return RTC::RTC_OK;
  }
  RTC::ReturnCode_t Disabled::onExit()
  {
    std::cout << "[Microwave] Disabled::onExit()" << std::endl;
    return RTC::RTC_OK;
  }

  //============================================================
  // State Operational
  void Operational::open()
  {
    std::cout << "[Microwave] >>> Door opened <<<" << std::endl;
    setState<Disabled>();
  }
  void Operational::stop()
  {
    std::cout << "[Microwave] >>> Stopped <<<" << std::endl;
    setState<Idle>();
  }
  RTC::ReturnCode_t Operational::onInit()
  {
    std::cout << "[Microwave] Operational::onInit()" << std::endl;
    setState<Idle>();
    return RTC::RTC_OK;
  }


  //============================================================
  // State Idle
  RTC::ReturnCode_t Idle::onEntry()
  {
    TOP::box().resetTimer();
    std::cout << "[Microwave] Idle::onEntry()" << std::endl;
    std::cout << "[Microwave] >>> Microwave ready <<<" << std::endl;
    return RTC::RTC_OK;
  }
  void Idle::minute(RTC::TimedLong time)
  {
    std::cout << "[Microwave] >>> Timer incremented <<<" << std::endl;
    setState<Programmed>();
    dispatch(Event(&TOP::minute, time));
  }

  //============================================================
  // State Programmed
  void Programmed::minute(RTC::TimedLong time)
  {
    std::cout << "[Microwave] >>> Timer incremented <<<" << std::endl;
    for (CORBA::Long i(0); i < time.data; ++i)
      {
        TOP::box().incrementTimer();
      }
    TOP::box().printTimer();
  }
  void Programmed::start()
  {
    std::cout << "[Microwave] >>> Microwave started <<<" << std::endl;
    setState<Cooking>();
  }
  RTC::ReturnCode_t Programmed::onInit()
  {
    std::cout << "[Microwave] Programmed::onInit()" << std::endl;
    return RTC::RTC_OK;
  }
  RTC::ReturnCode_t Programmed::onEntry()
  {
    std::cout << "[Microwave] Programmed::onEntry()" << std::endl;
    return RTC::RTC_OK;
  }
  RTC::ReturnCode_t Programmed::onExit()
  {
    std::cout << "[Microwave] Programmed::onExit()" << std::endl;
    return RTC::RTC_OK;
  }


  //============================================================
  // State Cooking
  void Cooking::tick()
  {
    std::cout << "[Microwave] >>> Clock tick <<<" << std::endl;
    
    TOP::Box & tb = TOP::box();
      tb.decrementTimer();
      if(tb.getRemainingTime() == 0)
        {
          std::cout << "[Microwave] >>> Finished <<<" << std::endl;
          setState<Idle>();
        }
      else
        {
          tb.printTimer();
        }
  }
  RTC::ReturnCode_t Cooking::onEntry()
  {
    std::cout << "[Microwave] Cooking::onEntry()" << std::endl;
    std::cout << "[Microwave] >>> Heating on <<<" << std::endl;
    return RTC::RTC_OK;
  }
  RTC::ReturnCode_t Cooking::onExit()
  {
    std::cout << "[Microwave] Cooking::onExit()" << std::endl;
    std::cout << "[Microwave] >>> Heating off <<<" << std::endl;
    return RTC::RTC_OK;
  }

} // namespace MicrowaveFsm
