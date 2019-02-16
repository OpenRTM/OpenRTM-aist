// -*- C++ -*-
/*!
 * @file   TimeValueTests.cpp
 * @brief  TimeValue test class
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

#ifndef TimeValue_cpp
#define TimeValue_cpp

#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//#include <../../include/coil/TimeValue.h>
#include <coil/TimeValue.h>

/*!
 * @class TimeValueTests class
 * @brief TimeValue test
 */
namespace TimeValue
{
  class TimeValueTests
   : public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE(TimeValueTests);
//    CPPUNIT_TEST(test_case0);
    CPPUNIT_TEST(test_init);
    CPPUNIT_TEST(test_operatorEQ);
    CPPUNIT_TEST(test_operatorPLUS);
    CPPUNIT_TEST(test_operatorMINUS);
    CPPUNIT_TEST(test_sign);
    CPPUNIT_TEST(test_operatordouble);
    CPPUNIT_TEST_SUITE_END();
  
  private:
  
  public:
  
    /*!
     * @brief Constructor
     */
    TimeValueTests()
    {
    }
    
    /*!
     * @brief Destructor
     */
    ~TimeValueTests()
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
    void test_case0()
    {
    }
    /* test case */
    /*
    ---------------------------------------------------------------------------
    This function tests the constructor of TimeValue.
     coil::TimeValue(double)
    Check that the value of the following tables is correctly set.
    ---------------------------------------------------------------------------
    */
    void test_init()
    {
        typedef struct data_struct {
            double indouble;
            long   outlong1;
            long   outlong2;
        } DATA_STRUCT;

        DATA_STRUCT datatable[] = {
            { 0.0 ,       0 , 0 },
            { 3.5 ,       3 , 500000 },
            { 3.555555 ,  3 , 555555 },
            { 3.5999994 , 3 , 599999 },
            { 3.5999995 , 3 , 600000 },
            { 0.999999 ,  0 , 999999 },
            { 1.000000 ,  1 , 0 },
            { 1.000001 ,  1 , 1 },
            { -1.0 ,      -1 , 0 },
            { -0.3 ,      0 , -300000 },
        };

        char cstr[256];
        short ic;
        long lsec,lusec;
        long ltvsec,ltvusec;
        double dbsec;
        coil::TimeValue *tv;
        for (ic=0; ic < (sizeof datatable/sizeof datatable[0]); ic++)
        {
            dbsec = datatable[ic].indouble;
            tv = new coil::TimeValue(dbsec);
            lsec = datatable[ic].outlong1;
            lusec = datatable[ic].outlong2;
            ltvsec = tv->sec();
            ltvusec = tv->usec();
            sprintf(cstr,"loop counter:%d  in:%f  sec():%ld  usec():%ld", ic, dbsec, ltvsec, ltvusec);
            CPPUNIT_ASSERT_MESSAGE(cstr, (ltvsec == lsec) & (ltvusec ==lusec) );
            delete tv;
        }
    }
    /*
    ---------------------------------------------------------------------------
    This function tests operator = of TimeValue.
    Check that the value of the following tables is correctly set
    ---------------------------------------------------------------------------
    */
    void test_operatorEQ()
    {
        typedef struct data_struct {
            double indouble;
            long   outlong1;
            long   outlong2;
        } DATA_STRUCT;

        DATA_STRUCT datatable[] = {
            { 0.0 ,       0 ,  0},
            { 3.5 ,       3 , 500000},
            { 3.555555 ,  3 , 555555},
            { 3.5999994 , 3 , 599999},
            { 3.5999995 , 3 , 600000},
            { -3.555555 ,  -3 , -555555},
            { -3.5999994 , -3 , -599999},
            { -3.5999995 , -3 , -600000},
        };

        char cstr[256];
        short ic;
        long lsec,lusec;
        long ltvsec,ltvusec;
        double dbsec;
        coil::TimeValue tv;
        for (ic=0; ic < (sizeof datatable/sizeof datatable[0]); ic++)
        {
            dbsec = datatable[ic].indouble;
            tv = dbsec; 
            lsec = datatable[ic].outlong1;
            lusec = datatable[ic].outlong2;
            ltvsec = tv.sec();
            ltvusec = tv.usec();
            sprintf(cstr,"loop counter:%d  in:%f  sec():%ld  usec():%ld ", ic, dbsec, ltvsec, ltvusec);
            CPPUNIT_ASSERT_MESSAGE(cstr, (ltvsec == lsec) & (ltvusec ==lusec));
        }
    }
    /*
    ---------------------------------------------------------------------------
    This function tests operator + of TimeValue.
    Check that the following table values are correctly added.
    ---------------------------------------------------------------------------
    */
    void test_operatorPLUS()
    {
        typedef struct data_struct {
            double indouble1;
            double indouble2;
            long   outlong1;
            long   outlong2;
        } DATA_STRUCT;

        DATA_STRUCT datatable[] = {
            { 0.0 ,       0.0 ,       0 , 0},
            { 3.5 ,       0.5 ,       4 , 0},
            { 3.599999 ,  0.000001 ,  3 , 600000},
            { 3.5999994 , 0.0000001 , 3 , 599999},
            { 3.5999995 , 0.0000001 , 3 , 600000},
            { -3.5 ,      -0.5 ,      -4 , 0},
            { -3.599999 , -0.000001 , -3 , -600000},
            { -3.5999994 ,-0.0000001 ,-3 , -599999},
            { -3.5999995 ,-0.0000001 ,-3 , -600000},
            { -3.5 ,      3.5 , 0 , 0},
            { -3.5 ,      3.499999 , 0 , -1},
            { -3.5 ,      3.500001 , 0 , 1},
            { 3.5 ,      -3.5 , 0 , 0},
            { 3.5 ,      -3.499999 , 0 , 1},
            { 3.5 ,      -3.500001 , 0 , -1},
        };

        char cstr[256];
        short ic;
        long lsec,lusec;
        long ltvsec,ltvusec;
        double dbsec1, dbsec2;
        coil::TimeValue tv1;
        coil::TimeValue tv2;
        for (ic=0; ic < (sizeof datatable/sizeof datatable[0]); ic++)
        {
            dbsec1 = datatable[ic].indouble1;
            tv1 = dbsec1; 
            dbsec2 = datatable[ic].indouble2;
            tv2 = dbsec2; 
            lsec = datatable[ic].outlong1;
            lusec = datatable[ic].outlong2;
            tv1 = tv1 + tv2;
            ltvsec = tv1.sec();
            ltvusec = tv1.usec();
            sprintf(cstr,"loop counter:%d  in1:%f  in2:%f  sec():%ld  usec():%ld ", ic, dbsec1, dbsec2, ltvsec, ltvusec);
            CPPUNIT_ASSERT_MESSAGE(cstr, (ltvsec == lsec) & (ltvusec ==lusec));
        }
    }
    /*
    ---------------------------------------------------------------------------
    This function tests operator - of TimeValue.
    Check that the following table values are correctly subtracted.
    ---------------------------------------------------------------------------
    */
    void test_operatorMINUS()
    {
        typedef struct data_struct {
            double indouble1;
            double indouble2;
            long   outlong1;
            long   outlong2;
        } DATA_STRUCT;

        DATA_STRUCT datatable[] = {
            { 0.0 ,       0.0 ,       0 , 0},
            { 3.5 ,       0.5 ,       3 , 0},
            { 3.6 ,       0.000001 ,  3 , 599999},
            { 3.6 ,       0.0000001 , 3 , 600000},
            { 3.5999995 , 0.0000001 , 3 , 600000},
            { 0.5 ,       3.5 ,       -3 , 0},
            { -3.6 ,       -0.000001 ,  -3 , -599999},
            { -3.6 ,       -0.0000001 , -3 , -600000},
            { -3.5999995 , -0.0000001 , -3 , -600000},
            { -3.5 ,      3.5 , -7 , 0},
            { -3.5 ,      3.499999 , -6 , -999999},
            { -3.5 ,      3.500001 , -7 , -1},
            { 3.5 ,      -3.5 , 7 , 0},
            { 3.5 ,      -3.499999 , 6 , 999999},
            { 3.5 ,      -3.500001 , 7 , 1},
        };

        char cstr[256];
        short ic;
        long lsec,lusec;
        long ltvsec,ltvusec;
        double dbsec1, dbsec2;
        coil::TimeValue tv1;
        coil::TimeValue tv2;
        for (ic=0; ic < (sizeof datatable/sizeof datatable[0]); ic++)
        {
            dbsec1 = datatable[ic].indouble1;
            tv1 = dbsec1; 
            dbsec2 = datatable[ic].indouble2;
            tv2 = dbsec2; 
            lsec = datatable[ic].outlong1;
            lusec = datatable[ic].outlong2;
            tv1 = tv1 - tv2;
            ltvsec = tv1.sec();
            ltvusec = tv1.usec();
            sprintf(cstr,"loop counter:%d  in1:%f  in2:%f  sec():%ld  usec():%ld ", ic, dbsec1, dbsec2, ltvsec, ltvusec);
            CPPUNIT_ASSERT_MESSAGE(cstr, (ltvsec == lsec) & (ltvusec ==lusec));
        }
    }
    /*
    ---------------------------------------------------------------------------
    This function tests TimeValue::sign.
    Check that the sign is correctly judged to the following table values.
    ---------------------------------------------------------------------------
    */
    void test_sign()
    {
        typedef struct data_struct {
            double indouble;
            int    outint;
        } DATA_STRUCT;

        DATA_STRUCT datatable[] = {
            { 0.0 ,       0 },
            { 3.5 ,       1 },
            { -3.5 ,      -1 },
            { 0.5 ,       1 },
            { -0.5 ,      -1 },
        };

        char cstr[256];
        short ic;
        int isign;
        int itvsign;
        double dbsec;
        coil::TimeValue tv;
        for (ic=0; ic < (sizeof datatable/sizeof datatable[0]); ic++)
        {
            dbsec = datatable[ic].indouble;
            tv = dbsec; 
            isign= datatable[ic].outint;
            itvsign = tv.sign();
            sprintf(cstr,"loop counter:%d  in:%f  sgin():%d", ic, dbsec, itvsign);
            CPPUNIT_ASSERT_MESSAGE(cstr, (isign == itvsign) );
        }
    }
    /*
    ---------------------------------------------------------------------------
    This function tests operator() of TimeValue.
    Check that the value of the following tables is correctly set.
    ---------------------------------------------------------------------------
    */
    void test_operatordouble()
    {
        typedef struct data_struct {
            double indouble;
            double outdouble;
        } DATA_STRUCT;

        DATA_STRUCT datatable[] = {
            { 0.0 ,       0.0},
            { 3.5 ,       3.5},
            { 3.555555 ,  3.555555},
            { 3.5999994 , 3.599999},
            { 3.5999995 , 3.6},
            { -3.555555 ,  -3.555555},
            { -3.5999994 , -3.599999},
            { -3.5999995 , -3.600000},
        };

        char cstr[256];
        short ic;
        double dbinsec;
        double dboutsec;
        double dbtvsec;
        coil::TimeValue tv;
        for (ic=0; ic < (sizeof datatable/sizeof datatable[0]); ic++)
        {
            dbinsec = datatable[ic].indouble;
            tv = dbinsec; 
            dboutsec = datatable[ic].outdouble;
            dbtvsec = (double)tv;
            sprintf(cstr,"loop counter:%d  in:%f  sec:%f ", ic, dbinsec, dbtvsec);
            CPPUNIT_ASSERT_MESSAGE(cstr, (dbtvsec == dboutsec) );
        }
    }
  };
}; // namespace TimeValue

/*
 * Register test suite
 */
CPPUNIT_TEST_SUITE_REGISTRATION(TimeValue::TimeValueTests);

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
#endif // TimeValue_cpp
