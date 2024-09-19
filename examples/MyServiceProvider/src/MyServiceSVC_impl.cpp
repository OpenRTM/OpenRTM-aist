// -*-C++-*-
/*!
 * @file  MyServiceSVC_impl.cpp
 * @brief Service implementation code of MyService.idl
 *
 *  * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */

#include "MyServiceSVC_impl.h"
#include <rtm/CORBA_SeqUtil.h>
#include <iostream>

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

/*
 * Example implementational code for IDL interface SimpleService::MyService
 */
SimpleService_MyServiceSVC_impl::SimpleService_MyServiceSVC_impl()
{
  // Please add extra constructor code here.
}


SimpleService_MyServiceSVC_impl::~SimpleService_MyServiceSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
char* SimpleService_MyServiceSVC_impl::echo(const char* msg)
{
  CORBA_SeqUtil::push_back(m_echoList, CORBA::string_dup(msg));
  std::cout << "MyService::echo() was called." << std::endl;

  for (int i(0); i < 10; ++i)
    {
      std::cout << "Message: " << msg << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  std::cout << "MyService::echo() was finished" << std::endl;

  return CORBA::string_dup(msg);
}

SimpleService::EchoList* SimpleService_MyServiceSVC_impl::get_echo_history()
{
  std::cout << "MyService::get_echo_history() was called." << std::endl;
  CORBA_SeqUtil::for_each(m_echoList, seq_print<const char*>());
  
  SimpleService::EchoList_var el;
  el = new SimpleService::EchoList(m_echoList);
  return el._retn();
}

void SimpleService_MyServiceSVC_impl::set_value(::CORBA::Float value)
{
  CORBA_SeqUtil::push_back(m_valueList, value);
  m_value = value;

  std::cout << "MyService::set_value() was called." << std::endl;

  for (int i(0); i < 10; ++i)
    {
      std::cout << "Input value: " << value;
      std::cout << ", Current value: " << m_value << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  std::cout << "MyService::set_value() was finished" << std::endl;

  return;
}

::CORBA::Float SimpleService_MyServiceSVC_impl::get_value()
{
  std::cout << "MyService::get_value() was called." << std::endl;
  std::cout << "Current value: " << m_value << std::endl;

  return m_value;
}

SimpleService::ValueList* SimpleService_MyServiceSVC_impl::get_value_history()
{
  std::cout << "MyService::get_value_history() was called." << std::endl;
  CORBA_SeqUtil::for_each(m_valueList, seq_print<CORBA::Float>());

  SimpleService::ValueList_var vl;
  vl = new SimpleService::ValueList(m_valueList);
  return vl._retn();
}



// End of example implementational code



