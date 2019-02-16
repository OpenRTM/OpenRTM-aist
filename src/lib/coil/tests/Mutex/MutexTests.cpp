// -*- C++ -*-
/*!
 * @file   MutexTests.cpp
 * @brief  Mutex test class
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

#ifndef Mutex_cpp
#define Mutex_cpp

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

#include <coil/Mutex.h>
#include <coil/Task.h>
#include <coil/Time.h>


/*!
 * @class MutexTests class
 * @brief Mutex test
 */
namespace Mutex
{
  class MutexTests
   : public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE(MutexTests);
//    CPPUNIT_TEST(test_case0);
    CPPUNIT_TEST(test_lock);
    CPPUNIT_TEST(test_trylock);
    CPPUNIT_TEST_SUITE_END();
  
  private:
  public:
    class TestTaskLock
       : public coil::Task 
    {
    public:
      static long ShareCount;
      static coil::Mutex mtx;
      int svc(void)
      {
        clock_t start, end;
		long lc;
        long lbuf;
        for(lc=0; lc<100; lc++)
        {
            mtx.lock();
            lbuf = ShareCount;
//			usleep(1);
			start = clock();
			for(;;)
			{
			  end = clock();
              if((double)(end-start)/CLOCKS_PER_SEC>0.000001)
			  {
			    break;
			  }
			}
            lbuf++;
            ShareCount = lbuf;
            mtx.unlock();
        }
        return 0;
      }

      long getShareCount()
      {
          return ShareCount;
      }
      void setShareCount(long lc)
      {
          mtx.lock();
          ShareCount = lc;
          mtx.unlock();
      }
    };
    class TestTaskTrylock
       : public coil::Task 
    {
    public:
      static long ShareCount;
      static coil::Mutex mtx;
      int svc(void)
      {
        clock_t start, end;
        long lc;
        long lbuf;
        int iret;
        for(lc=0; lc<100; lc++)
        {
            for (;;)
            {
                iret = mtx.trylock();
                if(iret == false)
                {
                    lbuf = ShareCount;
//                    usleep(1);
			        start = clock();
                    for(;;)
                    {
                      end = clock();
                      if((double)(end-start)/CLOCKS_PER_SEC>0.000001)
					  {
                        break;
                      }
                    }
					lbuf++;
                    ShareCount = lbuf;
                    break;
                }
            }
            mtx.unlock();
        }
        return 0;
      }

      long getShareCount()
      {
          return ShareCount;
      }
      void setShareCount(long lc)
      {
          mtx.lock();
          ShareCount = lc;
          mtx.unlock();
      }

    };
    /*!
     * @brief Constructor
     */
    MutexTests()
    {
    }
    
    /*!
     * @brief Destructor
     */
    ~MutexTests()
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
    This function tests the Mutex::lock function.
    Check exclusive control,when two threads access static variable.
    ---------------------------------------------------------------------------
    */
    void test_lock()
    {
        long lc;
        char cstr[256];
        MutexTests::TestTaskLock tka;
        MutexTests::TestTaskLock tkb;
        tka.setShareCount(0); 

        tka.activate();
        tkb.activate();        
        tka.wait();
        tkb.wait();
        lc = tka.getShareCount(); 
        sprintf( cstr, "count %ld" , lc );
//		std::cout<<cstr<<std::endl;
        CPPUNIT_ASSERT_MESSAGE(cstr , (lc == 200) );

    }
    /*
    ---------------------------------------------------------------------------
    This function tests the Mutex::trylock function.
    Check exclusive control,when two threads access static variable.
    ---------------------------------------------------------------------------
    */
    void test_trylock()
    {
        long lc;
        char cstr[256];
        MutexTests::TestTaskTrylock tka;
        MutexTests::TestTaskTrylock tkb;
        tka.setShareCount(0); 

        tka.activate();
        tkb.activate();        
        tka.wait();
        tkb.wait();
        lc = tka.getShareCount(); 
        sprintf( cstr, "count %ld" , lc );
//		std::cout<<cstr<<std::endl;
        CPPUNIT_ASSERT_MESSAGE(cstr , (lc == 200) );

    }
  };
  long MutexTests::TestTaskLock::ShareCount = 0L;
  coil::Mutex MutexTests::TestTaskLock::mtx ;
  long MutexTests::TestTaskTrylock::ShareCount = 0L;
  coil::Mutex MutexTests::TestTaskTrylock::mtx ;
}; // namespace Mutex

/*
 * Register test suite
 */
CPPUNIT_TEST_SUITE_REGISTRATION(Mutex::MutexTests);

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
#endif // Mutex_cpp
