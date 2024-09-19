// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  MyServiceConsumer.h
 * @brief MyService Consumer Sample component
 *
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */
// </rtc-template>

#ifndef MYSERVICECONSUMER_H
#define MYSERVICECONSUMER_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

#include <iostream>
#include <utility>
#include <thread>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">
#include "MyServiceStub.h"

// </rtc-template>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

using namespace RTC;

// <rtc-template block="component_description">
/*!
 * @class MyServiceConsumer
 * @brief MyService Consumer Sample component
 *
 */
// </rtc-template>
class MyServiceConsumer
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  MyServiceConsumer(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~MyServiceConsumer() override;

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  // <rtc-template block="activity">
  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onInitialize() override;

  /***
   *
   * The finalize action (on ALIVE->END transition)
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onFinalize() override;

  /***
   *
   * The startup action when ExecutionContext startup
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id) override;

  /***
   *
   * The shutdown action when ExecutionContext stop
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id) override;

  /***
   *
   * The activated action (Active state entry action)
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id) override;

  /***
   *
   * The deactivated action (Active state exit action)
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id) override;

  /***
   *
   * The execution action that is invoked periodically
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id) override;

  /***
   *
   * The aborting action when main logic error occurred.
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id) override;

  /***
   *
   * The error action in ERROR state
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onError(RTC::UniqueId ec_id) override;

  /***
   *
   * The reset action that is invoked resetting
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onReset(RTC::UniqueId ec_id) override;
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id) override;

  /***
   *
   * The action that is invoked when execution context's rate is changed
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id) override;
  // </rtc-template>


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  /*!
   */
  RTC::CorbaPort m_MyServicePort;
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  /*!
   */
  RTC::CorbaConsumer<SimpleService::MyService> m_myservice;
  
  // </rtc-template>

  void set_value(CORBA::Float val, RTC::CorbaConsumer<SimpleService::MyService>& obj)
  {
    try
      {
        if( CORBA::is_nil((obj).operator->()) )
          {
            std::cout << "No service connected." << std::endl;
          }
        else
          {
            (obj)->set_value(val);
          }
      }
    catch (const CORBA::INV_OBJREF &)
      {
      }
    catch (const CORBA::OBJECT_NOT_EXIST &)
      {
      }
    catch (const CORBA::OBJ_ADAPTER &)
      {
      }
    catch (...)
      {
      }
  }

  void echo(std::string msg, std::string& result, RTC::CorbaConsumer<SimpleService::MyService>& obj)
  {
    try
      {
        if( CORBA::is_nil((obj).operator->()) )
          {
            std::cout << "No service connected." << std::endl;
          }
        else
          {
            result = (obj)->echo(msg.c_str());
          }
      }
    catch (const CORBA::INV_OBJREF &)
      {
      }
    catch (const CORBA::OBJECT_NOT_EXIST &)
      {
      }
    catch (const CORBA::OBJ_ADAPTER &)
      {
      }
    catch (...)
      {
      }
  }

 private:
  std::thread* async_echo;
  bool async_finished{false};
  std::vector<std::thread*> async_set_value;
  std::string m_result;

  template <class T>
  struct seq_print
  {
    seq_print()  {}
    void operator()(T val)
    {
      std::cout << m_cnt << ": " << val << std::endl;
      ++m_cnt;
    }
    int m_cnt{0};
  };

};


extern "C"
{
  DLL_EXPORT void MyServiceConsumerInit(RTC::Manager* manager);
};

#endif // MYSERVICECONSUMER_H
