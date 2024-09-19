// -*-C++-*-
/*!
 * @file  MyServiceSVC_impl.h
 * @brief Service implementation header of MyService.idl
 *
 *  * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 */

#include "MyServiceSkel.h"

#ifndef MYSERVICESVC_IMPL_H
#define MYSERVICESVC_IMPL_H
 
/*!
 * @class SimpleService_MyServiceSVC_impl
 * Example class implementing IDL interface SimpleService::MyService
 */
class SimpleService_MyServiceSVC_impl
 : public virtual POA_SimpleService::MyService,
   public virtual PortableServer::RefCountServantBase
{
 private:
   // Make sure all instances are built on the heap by making the
   // destructor non-public
   //virtual ~SimpleService_MyServiceSVC_impl();

 public:
  /*!
   * @brief standard constructor
   */
   SimpleService_MyServiceSVC_impl();
  /*!
   * @brief destructor
   */
   virtual ~SimpleService_MyServiceSVC_impl();

   // attributes and operations
   char* echo(const char* msg);
   SimpleService::EchoList* get_echo_history();
   void set_value(::CORBA::Float value);
   ::CORBA::Float get_value();
   SimpleService::ValueList* get_value_history();

private:
  CORBA::Float m_value;
  SimpleService::EchoList m_echoList;
  SimpleService::ValueList m_valueList;
};



#endif // MYSERVICESVC_IMPL_H


