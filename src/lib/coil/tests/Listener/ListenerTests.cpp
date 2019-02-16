// -*- C++ -*-
/*!
 * @file   ListenerTests.cpp
 * @brief  Listener test class
 * @date   $Date$ 
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * $Id$ 
 *
 */

/*
 * $Log$
 *
 */

#ifndef Listener_cpp
#define Listener_cpp

#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
//#include <unistd.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#include <coil/Listener.h>

/*!
 * @class ListenerTests class
 * @brief Listener test
 */
namespace Listener
{
  short g_iCallBackFlag;
    /*
    ---------------------------------------------------------------------------
    ---------------------------------------------------------------------------
    */
  void CallBackFunc()
  {
    g_iCallBackFlag = 1;
  }
  class CallBackTestClass
  {
  private:
    short m_callbackflag;
  public:
    CallBackTestClass()
    {
        m_callbackflag = 0;
    }
    void callbackfunc()
    {
        m_callbackflag = 1;
    }
    short getCallBackFuncStat()
    {
        return m_callbackflag;
    }
  };
  class ListenerTests
   : public CppUnit::TestFixture 
  {
    CPPUNIT_TEST_SUITE(ListenerTests);
    CPPUNIT_TEST(test_ListenerObject);
    CPPUNIT_TEST(test_ListenerObject2);
    CPPUNIT_TEST(test_ListenerObject3);
    CPPUNIT_TEST(test_ListenerFunc);
    CPPUNIT_TEST_SUITE_END();
  
  private:
    short m_callbackflag;
  public:
    class TestClassMember
    {
    private:
      short m_callbackflag;
    public:
      TestClassMember()
      {
          m_callbackflag = 0;
      }
      void callbackfunc()
      {
          m_callbackflag = 1;
      }
      short getCallBackFuncStat()
      {
          return m_callbackflag;
      }
      
    };
  
    /*!
     * @brief Constructor
     */
    ListenerTests()
    {
    }
    
    /*!
     * @brief Destructor
     */
    ~ListenerTests()
    {
    }
  
    /*!
     * @brief Test initialization
     */
    virtual void setUp()
    {
    }
    
    /*!
     * @brief Test finalization
     */
    virtual void tearDown()
    { 
    }
  
    /* test case */
    /*
    ---------------------------------------------------------------------------
    ---------------------------------------------------------------------------
    */
    void callbackThis()
    {
        m_callbackflag = 1;
    }
    
    /*
    ---------------------------------------------------------------------------
    This function tests coil::ListenerObject.
    Check that the registered callback function is called.
    ---------------------------------------------------------------------------
    */
    void test_ListenerObject()
    {
        short istat;
        TestClassMember * tc;
        tc = new TestClassMember;
        ListenerObject<TestClassMember> lo(tc,&TestClassMember::callbackfunc) ;
        lo.invoke();
        istat = tc->getCallBackFuncStat();
        CPPUNIT_ASSERT_MESSAGE("ListenerObject", (istat == 1) );
        delete tc;
    }
    /*
    ---------------------------------------------------------------------------
    This function tests coil::ListenerObject.
    Check that the registered callback function is called.    
    ---------------------------------------------------------------------------
    */
    void test_ListenerObject2()
    {
        short istat;
        CallBackTestClass * tc;
        tc = new CallBackTestClass;
        ListenerObject<CallBackTestClass> lo(tc,&CallBackTestClass::callbackfunc) ;
        lo.invoke();
        istat = tc->getCallBackFuncStat();
        CPPUNIT_ASSERT_MESSAGE("ListenerObject", (istat == 1) );
        delete tc;
    }
    /*
    ---------------------------------------------------------------------------
    This function tests coil::ListenerObject.
    Check that the registered callback function is called.
    ---------------------------------------------------------------------------
    */
    void test_ListenerObject3()
    {
        m_callbackflag = 0;
        ListenerObject<ListenerTests> lo(this,&ListenerTests::callbackThis) ;
        lo.invoke();
        CPPUNIT_ASSERT_MESSAGE("ListenerObject", (m_callbackflag == 1) );
    }
    /*
    ---------------------------------------------------------------------------
    This function tests coil::ListenerFunc.
    Check that the registered callback function is called.
    ---------------------------------------------------------------------------
    */
    void test_ListenerFunc()
    {
        g_iCallBackFlag = 0;
        ListenerFunc lf(&CallBackFunc) ;
        lf.invoke();
        CPPUNIT_ASSERT_MESSAGE("ListenerFunc", (g_iCallBackFlag == 1) );
    }
  };
}; // namespace Listener

/*
 * Register test suite
 */
CPPUNIT_TEST_SUITE_REGISTRATION(Listener::ListenerTests);

#ifdef LOCAL_MAIN
int main(int argc, char* argv[])
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    CppUnit::Outputter* outputter = 
      new CppUnit::TextOutputter(&runner.result(), std::cout);
    runner.setOutputter(outputter);
    bool retcode = runner.run();
    return !retcode;
}
#endif // MAIN
#endif // Listener_cpp
