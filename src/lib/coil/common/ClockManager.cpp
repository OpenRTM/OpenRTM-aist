// -*- C++ -*-
/*!
 * @file ClockManager.cpp
 * @brief Global clock management class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2012
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/Time.h>
#include <coil/ClockManager.h>
#include <coil/Guard.h>

#include <string>

namespace coil
{
  //============================================================
  // System Clock
  //============================================================
  SystemClock::~SystemClock()
  {
  }
  coil::TimeValue SystemClock::gettime() const
  {
    return coil::gettimeofday();
  }
  bool SystemClock::settime(coil::TimeValue clocktime)
  {
#if defined(__powerpc__) && !defined(__RTP__)
    if (coil::settimeofday(clocktime.sec(), clocktime.usec()*1000) == 0)
      {
        return true;
      }
    return false;
#elif defined(VXWORKS_66)
    timespec tv;
    tv.tv_sec = clocktime.sec();
    tv.tv_nsec = clocktime.usec()*1000;
    if (coil::settimeofday(&tv) == 0)
      {
        return true;
      }
    return false;
#else
    timeval tv;
    tv.tv_sec = clocktime.sec();
    tv.tv_usec = clocktime.usec();
    if (coil::settimeofday(&tv, nullptr) == 0)
      {
        return true;
      }
    return false;
#endif
  }
  //
  //============================================================

  //============================================================
  // Logical Time Clock
  //============================================================
  LogicalClock::LogicalClock()
    : m_currentTime(0.0)
  {
  }

  LogicalClock::~LogicalClock()
  {
  }

  coil::TimeValue LogicalClock::gettime() const
  {
    coil::Guard<coil::Mutex> guard(m_currentTimeMutex);
    return m_currentTime;
  }

  bool LogicalClock::settime(coil::TimeValue clocktime)
  {
    Guard<Mutex> guard(m_currentTimeMutex);
    m_currentTime = clocktime;
    return true;
  }
  //
  //============================================================

  //============================================================
  // Adjusted Clock
  //============================================================
  AdjustedClock::AdjustedClock()
    : m_offset(0.0)
  {
  }

  AdjustedClock::~AdjustedClock()
  {
  }

  coil::TimeValue AdjustedClock::gettime() const
  {
    Guard<Mutex> guard(m_offsetMutex);
    return coil::gettimeofday() - m_offset;
  }

  bool AdjustedClock::settime(coil::TimeValue clocktime)
  {
    Guard<Mutex> guard(m_offsetMutex);
    m_offset = coil::gettimeofday() - clocktime;
    return true;
  }

  IClock& ClockManager::getClock(const std::string& clocktype)
  {
    if (clocktype == "logical")
      {
        return m_logicalClock;
      }
    else if (clocktype == "adjusted")
      {
        return m_adjustedClock;
      }
    else if (clocktype == "system")
      {
        return m_systemClock;
      }
    else
      {
      }
    return m_systemClock;
  }

#ifdef WIN32
  ClockManager* ClockManager::clockmgr = NULL;
  coil::Mutex ClockManager::clockmgr_mutex;
  ClockManager& ClockManager::instance()
  {
    if (!clockmgr)
      {
        coil::Guard<coil::Mutex> guard(clockmgr_mutex);
        if (!clockmgr)
          {
            clockmgr = new ClockManager();
          }
      }
    return *clockmgr;
  }
#endif
};  // namespace coil
