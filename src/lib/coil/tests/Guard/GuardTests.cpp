// -*- C++ -*-
/*!
 * @file   GuardTests.cpp
 * @brief  Guard test class
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

#ifndef Guard_cpp
#define Guard_cpp

#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
//#include <unistd.h>
#include <time.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//#include <../../include/coil/Guard.h>
#include <coil/Guard.h>
#include <coil/Task.h>

/*!
 * @class GuardTests class
 * @brief Guard test
 */
namespace Guard
{
  class GuardTests
   : public CppUnit::TestFixture ,
     public coil::Task 
  {
    CPPUNIT_TEST_SUITE(GuardTests);
    CPPUNIT_TEST(test_case0);
    CPPUNIT_TEST_SUITE_END();
  
  private:

  public:
    coil::Mutex mtx;
//    long sharecount;
    class TestGuardTask
       : public coil::Task 
    {
    public:
      static long ShareCount;
      static coil::Mutex mtx;
      int svc(void)
      {
        clock_t start,end;
	    long lc;
        long lbuf;
        coil::Guard<coil::Mutex> guard(mtx);
        for(lc=0; lc<100; lc++)
        {
          lbuf = ShareCount;
          start = clock();
          for(;;)
          {
            end = clock();
            if((end-start)>1)
			{
              break;
            }
          }
		  lbuf++;
          ShareCount = lbuf;
        }
        return 0;
      }

      long getShareCount()
      {
          return ShareCount;
      }
      void setShareCount(long lc)
      {
          coil::Guard<coil::Mutex> guard(mtx);
          ShareCount = lc;
      }
    };

    /*!
     * @brief Constructor
     */
    GuardTests()
    {
//        sharecount = 0;
    }
    
    /*!
     * @brief Destructor
     */
    ~GuardTests()
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
    int svc(void)
    {
      return 0;
    }
    /*
    ---------------------------------------------------------------------------
    This function tests coil::Guard.
    Check exclusive control,when two threads access static variable.
    ---------------------------------------------------------------------------
    */
    void test_case0()
    {
      GuardTests::TestGuardTask tka;
      GuardTests::TestGuardTask tkb;
      tka.setShareCount(0);
      long lc;
      char cstr[256];

      tka.activate();
      tkb.activate();        
      tka.wait();
      tkb.wait();
      lc = tka.getShareCount(); 
      sprintf(cstr, "sharecount:%ld", lc); 
      CPPUNIT_ASSERT_MESSAGE(cstr, (lc== 200) );

    }
  };
  long GuardTests::TestGuardTask::ShareCount = 0L;
  coil::Mutex GuardTests::TestGuardTask::mtx ;

}; // namespace Guard

/*
 * Register test suite
 */
CPPUNIT_TEST_SUITE_REGISTRATION(Guard::GuardTests);

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
#endif // Guard_cpp
