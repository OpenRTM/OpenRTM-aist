// -*- C++ -*-
/*!
 * @file VxWorksRTExecutionContext.cpp
 * @brief VxWorksRTExecutionContext class
 * @date $Date: 2017-07-11 10:53:05 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */


#include <coil/Time.h>

#include <taskLib.h>
#include <sysLib.h>
#include <rtm/VxWorksRTExecutionContext.h>
#include <rtm/RTObjectStateMachine.h>

#define TIMEVALUE_ONE_SECOND_IN_NSECS 1000000000l // 1 [sec] = 1000000000 [nsec]


namespace RTC_exp
{
  /*!
   * @if jp
   * @brief デフォルトコンストラクタ
   * @else
   * @brief Default constructor
   * @endif
   */
  VxWorksRTExecutionContext::
  VxWorksRTExecutionContext()
  {
    RTC_TRACE(("VxWorksRTExecutionContext()"));

    RTC_DEBUG(("Actual period: %d [sec], %d [usec]",
               m_profile.getPeriod().sec(), m_profile.getPeriod().usec()));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  VxWorksRTExecutionContext::~VxWorksRTExecutionContext()
  {
    RTC_TRACE(("~VxWorksRTExecutionContext()"));
    
  }



  void VxWorksRTExecutionContext::init(coil::Properties& props)
  {
    RTC_TRACE(("init()"));
    ExecutionContextBase::init(props);

    setCpuAffinity(props);
    setPriority(props);
    setStackSize(props);

    RTC_DEBUG(("init() done"));
  }


  /*------------------------------------------------------------
   * Run by a daemon thread to handle deferred processing
   * ACE_Task class method over ride.
   *------------------------------------------------------------*/
  /*!
   * @if jp
   * @brief ExecutionContext 用のスレッド実行関数
   * @else
   * @brief Thread execution function for ExecutionContext
   * @endif
   */
  int VxWorksRTExecutionContext::svc(void)
  {
    RTC_TRACE(("svc()"));
    int count(0);



    do
      {
        ExecutionContextBase::invokeWorkerPreDo();
        // Thread will stopped when all RTCs are INACTIVE.
        // Therefore WorkerPreDo(updating state) have to be invoked
        // before stopping thread.
        {
          std::lock_guard<coil::Mutex> guard(m_workerthread.mutex_);
          while (!m_workerthread.running_)
            {
              m_workerthread.cond_.wait();
            }
        }
        timespec ts;
        ::clock_gettime(CLOCK_MONOTONIC, &ts);
        TimeValue_nano t0(ts.tv_sec, ts.tv_nsec);

        ExecutionContextBase::invokeWorkerDo();
        ExecutionContextBase::invokeWorkerPostDo();

        ::clock_gettime(CLOCK_MONOTONIC, &ts);
        TimeValue_nano t1(ts.tv_sec, ts.tv_nsec);

        TimeValue_nano period(1.0/getRate());
        if (count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", (double)period));
            RTC_PARANOID(("Execution: %f [s]", (double)(t1 - t0)));
            RTC_PARANOID(("Sleep:     %f [s]", (double)(period - (t1 - t0))));
          }
        ::clock_gettime(CLOCK_MONOTONIC, &ts);
        TimeValue_nano t2(ts.tv_sec, ts.tv_nsec);
        if (!m_nowait && period > (t1 - t0))
          {
            if (count > 1000) { RTC_PARANOID(("sleeping...")); }
            sleep_nano((TimeValue_nano)(period - (t1 - t0)));
          }
        if (count > 1000)
          {
            ::clock_gettime(CLOCK_MONOTONIC, &ts);
            TimeValue_nano t3(ts.tv_sec, ts.tv_nsec);
            RTC_PARANOID(("Slept:     %f [s]", (double)(t3 - t2)));
            count = 0;
          }
        ++count;
      } while (threadRunning());
    RTC_DEBUG(("Thread terminated."));
    return 0;
  }


  void VxWorksRTExecutionContext::setPriority(coil::Properties& props)
  {
    RTC_TRACE(("setPriority()"));
    std::string priority;
    getProperty(props, "priority", priority);
    int num;
    if (coil::stringTo(num, priority.c_str()))
    {
      set_priority(num);
    }
  }


  void VxWorksRTExecutionContext::setStackSize(coil::Properties& props)
  {
    RTC_TRACE(("setStackSize()"));
    std::string stack_size;
    getProperty(props, "stack_size", stack_size);
    int num;
    if (coil::stringTo(num, stack_size.c_str()))
    {
      set_stacksize(num);
    }
  }

  int VxWorksRTExecutionContext::sleep_nano(TimeValue_nano interval)
  {
    int tps = sysClkRateGet();
    if(taskDelay((double)interval.sec()*tps + (double)(interval.nsec()*tps)/TIMEVALUE_ONE_SECOND_IN_NSECS) == OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
  }


  TimeValue_nano::TimeValue_nano(long sec, long nsec)
  {
    m_sec = sec;
    m_nsec = nsec;
    normalize();
  }
  TimeValue_nano::TimeValue_nano(double timeval)
  {
    double dbHalfAdj;
    if ( timeval >= 0 ) 
    {
        dbHalfAdj = +0.5;
    }
    else
    {
        dbHalfAdj = -0.5;
    }
    m_sec = (long int)timeval;
    m_nsec = (long)((timeval - (double)m_sec)
                    * TIMEVALUE_ONE_SECOND_IN_NSECS + dbHalfAdj );
    normalize();
  }
  TimeValue_nano::~TimeValue_nano()
  {
  }
  TimeValue_nano TimeValue_nano::operator-(TimeValue_nano& tm)
  {
    TimeValue_nano res;
    if (m_sec >= tm.m_sec) // +
      {
        if (m_nsec >= tm.m_nsec) /* 繰り下がり無し */
          {
            res.m_sec  = m_sec  - tm.m_sec;  // -
            res.m_nsec = m_nsec - tm.m_nsec; // +
          }
        else /* m_usec < tm.m_usec 繰り下がり有り */
          {
            res.m_sec  = m_sec  - tm.m_sec - 1;
            res.m_nsec = (m_nsec + TIMEVALUE_ONE_SECOND_IN_NSECS) - tm.m_nsec;
          }
      }
    else // m_sec < tm.m_sec // -
      {
          if (tm.m_nsec >= m_nsec) /* 繰り下がり無し */
            {
              res.m_sec  = - (tm.m_sec  - m_sec); // +
              res.m_nsec = - (tm.m_nsec - m_nsec);  // +
            }
          else /* tm.m_usec < m_usec 繰り下がり有り */
            {
              res.m_sec  = - (tm.m_sec - m_sec  - 1);
              res.m_nsec = - (tm.m_nsec + TIMEVALUE_ONE_SECOND_IN_NSECS) + m_nsec;
            }
      }
    res.normalize();
    return res;
  }
  TimeValue_nano TimeValue_nano::operator+(TimeValue_nano& tm)
  {
    TimeValue_nano res;
    res.m_sec  = m_sec  + tm.m_sec;
    res.m_nsec = m_nsec + tm.m_nsec;
    if (res.m_nsec >= TIMEVALUE_ONE_SECOND_IN_NSECS)
      {
        ++res.m_sec;
        res.m_nsec -= TIMEVALUE_ONE_SECOND_IN_NSECS;
      }
    res.normalize();
    return res;
  }
  TimeValue_nano TimeValue_nano::operator=(double time)
  {
    double dbHalfAdj;
    if ( time >= 0 ) 
    {
       dbHalfAdj = +0.5;
    }
    else
    {
       dbHalfAdj = -0.5;
    }

    m_sec = (long)time;
    m_nsec = (long)((time - (double)m_sec)*TIMEVALUE_ONE_SECOND_IN_NSECS + dbHalfAdj);
    normalize();
    return *this;
  }
  TimeValue_nano::operator double() const
  {
    return (double)m_sec + ((double)m_nsec/TIMEVALUE_ONE_SECOND_IN_NSECS);
  }
  int TimeValue_nano::sign() const
  {
    if (m_sec > 0) return 1;
    if (m_sec < 0) return -1;
    if (m_nsec > 0) return 1;
    if (m_nsec < 0) return -1;
    return 0;
  }
  void TimeValue_nano::normalize()
  {
    if (m_nsec >= TIMEVALUE_ONE_SECOND_IN_NSECS)
      {
        do
          {
            ++m_sec;
            m_nsec -= TIMEVALUE_ONE_SECOND_IN_NSECS;
          }
        while (m_nsec >= TIMEVALUE_ONE_SECOND_IN_NSECS);
      }
    else if (m_nsec <= -TIMEVALUE_ONE_SECOND_IN_NSECS)
      {
        do
          {
            --m_sec;
            m_nsec += TIMEVALUE_ONE_SECOND_IN_NSECS;
          }
        while (m_nsec <= -TIMEVALUE_ONE_SECOND_IN_NSECS);
      }
    
    if (m_sec >= 1 && m_nsec < 0)
      {
        --m_sec;
        m_nsec += TIMEVALUE_ONE_SECOND_IN_NSECS;
      }
    else if (m_sec < 0 && m_nsec > 0)
      {
        ++m_sec;
        m_nsec -= TIMEVALUE_ONE_SECOND_IN_NSECS;
      }
  }

} // namespace RTC 

extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */

 void VxWorksRTExecutionContextInit(RTC::Manager* manager)
  {
    RTC::ExecutionContextFactory::
      instance().addFactory("VxWorksRTExecutionContext",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC_exp::VxWorksRTExecutionContext>);

    coil::vstring ecs;
    ecs = RTC::ExecutionContextFactory::instance().getIdentifiers();
  }
};


