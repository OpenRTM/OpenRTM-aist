// -*-C++-*-
/*!
 * @file  MyServiceSVC_impl.cpp
 * @brief Service implementation code of MyService.idl (test code)
 *
 */

#include "MyServiceSVC_impl.h"

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
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <char* SimpleService_MyServiceSVC_impl::echo(const char* msg)>"
#endif
  return 0;
}

SimpleService::EchoList* SimpleService_MyServiceSVC_impl::get_echo_history()
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <SimpleService::EchoList* SimpleService_MyServiceSVC_impl::get_echo_history()>"
#endif
  return 0;
}

void SimpleService_MyServiceSVC_impl::set_value(::CORBA::Float value)
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <void SimpleService_MyServiceSVC_impl::set_value(::CORBA::Float value)>"
#endif
}

::CORBA::Float SimpleService_MyServiceSVC_impl::get_value()
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <::CORBA::Float SimpleService_MyServiceSVC_impl::get_value()>"
#endif
  return 0;
}

SimpleService::ValueList* SimpleService_MyServiceSVC_impl::get_value_history()
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <SimpleService::ValueList* SimpleService_MyServiceSVC_impl::get_value_history()>"
#endif
  return 0;
}



// End of example implementational code



