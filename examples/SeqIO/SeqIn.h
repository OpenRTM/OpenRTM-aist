// -*- C++ -*-
/*!
 * @file  SeqIn.h
 * @brief Sequence InPort component
 * @date  $Date: 2008-02-29 04:54:53 $
 *
 * $Id$
 */

#ifndef SEQIN_H
#define SEQIN_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

// Connector Listener Dump Flag : default OFF
extern bool g_Listener_dump_enabled;

class DataListener
  : public ConnectorDataListenerT<RTC::TimedLong>
{
  USE_CONNLISTENER_STATUS;
public:
  DataListener(const char* name) : m_name(name) {}
  ~DataListener() override
  {
    // Connector Listener Dump check
    if(g_Listener_dump_enabled)
      {
        std::cout << "dtor of " << m_name << std::endl;
      }
  }

  ReturnCode operator()(ConnectorInfo& info,
                          TimedLong& data) override
  {
    // Connector Listener Dump check
    if(g_Listener_dump_enabled)
      {
        std::cout << "------------------------------"   << std::endl;
        std::cout << "Data Listener: " << m_name       << std::endl;
        std::cout << "Profile::name: " << info.name    << std::endl;
        std::cout << "Profile::id:   " << info.id      << std::endl;
//        std::cout << "Profile::properties: "            << std::endl;
//        std::cout << info.properties;
//        std::cout                                       << std::endl;
        std::cout << "Data:          " << data.data    << std::endl;
        std::cout << "------------------------------"   << std::endl;
      }
    return NO_CHANGE;
  }
  std::string m_name;
};


class ConnListener
  : public ConnectorListener
{
  USE_CONNLISTENER_STATUS;
public:
  ConnListener(const char* name) : m_name(name) {}
  ~ConnListener() override
  {
    // Connector Listener Dump check
    if(g_Listener_dump_enabled)
      {
        std::cout << "dtor of " << m_name << std::endl;
      }
  }

  ReturnCode operator()(ConnectorInfo& info) override
  {
    // Connector Listener Dump check
    if(g_Listener_dump_enabled)
      {
        std::cout << "------------------------------"   << std::endl;
        std::cout << "Connector Listener: " << m_name       << std::endl;
        std::cout << "Profile::name:      " << info.name    << std::endl;
        std::cout << "Profile::id:        " << info.id      << std::endl;
        std::cout << "Profile::properties: "            << std::endl;
        std::cout << info.properties;
        std::cout                                       << std::endl;
        std::cout << "------------------------------"   << std::endl;
      }
    return NO_CHANGE;
  }
  std::string m_name;
};


class SeqIn
  : public RTC::DataFlowComponentBase
{
 public:
  SeqIn(RTC::Manager* manager);
  ~SeqIn() override;

  // The initialize action (on CREATED->ALIVE transition)
  // formaer rtc_init_entry() 
  RTC::ReturnCode_t onInitialize() override;

  // The finalize action (on ALIVE->END transition)
  // formaer rtc_exiting_entry()
  // virtual RTC::ReturnCode_t onFinalize();

  // The startup action when ExecutionContext startup
  // former rtc_starting_entry()
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  // The shutdown action when ExecutionContext stop
  // former rtc_stopping_entry()
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  // The activated action (Active state entry action)
  // former rtc_active_entry()
  // virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  // The deactivated action (Active state exit action)
  // former rtc_active_exit()
  // virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  // The execution action that is invoked periodically
  // former rtc_active_do()
  RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id) override;

  // The aborting action when main logic error occurred.
  // former rtc_aborting_entry()
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  // The error action in ERROR state
  // former rtc_error_do()
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  // The reset action that is invoked resetting
  // This is same but different the former rtc_init_entry()
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  // The state update action that is invoked after onExecute() action
  // no corresponding operation exists in OpenRTm-aist-0.2.0
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  // The action that is invoked when execution context's rate is changed
  // no corresponding operation exists in OpenRTm-aist-0.2.0
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // DataInPort declaration
  // <rtc-template block="inport_declare">
  TimedOctet m_Octet;
  InPort<TimedOctet> m_OctetIn;
  TimedShort m_Short;
  InPort<TimedShort> m_ShortIn;
  TimedLong m_Long;
  InPort<TimedLong> m_LongIn;
  TimedFloat m_Float;
  InPort<TimedFloat> m_FloatIn;
  TimedDouble m_Double;
  InPort<TimedDouble> m_DoubleIn;
  TimedOctetSeq m_OctetSeq;
  InPort<TimedOctetSeq> m_OctetSeqIn;
  TimedShortSeq m_ShortSeq;
  InPort<TimedShortSeq> m_ShortSeqIn;
  TimedLongSeq m_LongSeq;
  InPort<TimedLongSeq> m_LongSeqIn;
  TimedFloatSeq m_FloatSeq;
  InPort<TimedFloatSeq> m_FloatSeqIn;
  TimedDoubleSeq m_DoubleSeq;
  InPort<TimedDoubleSeq> m_DoubleSeqIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

 private:

};


extern "C"
{
  DLL_EXPORT void SeqInInit(RTC::Manager* manager);
};

#endif // SEQIN_H
