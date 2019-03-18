// -*-C++-*-
/*!
 * @file  MyServiceSVC_impl.cpp
 * @brief Service implementation code of MyService.idl
 *
 */

#include "MyServiceSVC_impl.h"
#include <rtm/CORBA_SeqUtil.h>
#include <coil/Time.h>
#include <iostream>

template <class T>
struct seq_print
{
  seq_print() : m_cnt(0) {}
  void operator()(T val)
  {
    std::cout << m_cnt << ": " << val << std::endl;
    ++m_cnt;
  }
  int m_cnt;
};

/*
 * Example implementational code for IDL interface MyService
 */
MyServiceSVC_impl::MyServiceSVC_impl()
{
  // Please add extra constructor code here.
}


MyServiceSVC_impl::~MyServiceSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
char* MyServiceSVC_impl::echo(const char* msg)
  throw (CORBA::SystemException)
{
  CORBA_SeqUtil::push_back(m_echoList, CORBA::string_dup(msg));
  std::cout << "MyService::echo() was called." << std::endl;

  for (int i(0); i < 10; ++i)
    {
      std::cout << "Message: " << msg << std::endl;
      coil::sleep(1);
    }
  std::cout << "MyService::echo() was finished" << std::endl;

  return CORBA::string_dup(msg);
}

SimpleService::EchoList* MyServiceSVC_impl::get_echo_history()
  throw (CORBA::SystemException)
{
  std::cout << "MyService::get_echo_history() was called." << std::endl;
  CORBA_SeqUtil::for_each(m_echoList, seq_print<const char*>());
  
  SimpleService::EchoList_var el;
  el = new SimpleService::EchoList(m_echoList);
  return el._retn();
}

void MyServiceSVC_impl::set_value(CORBA::Float value)
  throw (CORBA::SystemException)
{
  CORBA_SeqUtil::push_back(m_valueList, value);
  m_value = value;

  std::cout << "MyService::set_value() was called." << std::endl;

  for (int i(0); i < 10; ++i)
    {
      std::cout << "Input value: " << value;
      std::cout << ", Current value: " << m_value << std::endl;
      coil::sleep(1);
    }
  std::cout << "MyService::set_value() was finished" << std::endl;

  return;
}

CORBA::Float MyServiceSVC_impl::get_value()
  throw (CORBA::SystemException)
{
  std::cout << "MyService::get_value() was called." << std::endl;
  std::cout << "Current value: " << m_value << std::endl;

  return m_value;
}

SimpleService::ValueList* MyServiceSVC_impl::get_value_history()
  throw (CORBA::SystemException)
{
  std::cout << "MyService::get_value_history() was called." << std::endl;
  CORBA_SeqUtil::for_each(m_valueList, seq_print<CORBA::Float>());

  SimpleService::ValueList_var vl;
  vl = new SimpleService::ValueList(m_valueList);
  return vl._retn();
}



// End of example implementational code



