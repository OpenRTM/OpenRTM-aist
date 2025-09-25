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

#include <coil/ClockManager.h>
#include <mutex>
#include <chrono>
#include <string>

namespace coil
{
  //============================================================
  // IClock
  //============================================================
  IClock::~IClock() = default;

  //============================================================
  // System Clock
  //============================================================
  SystemClock::~SystemClock() = default;
  std::chrono::nanoseconds SystemClock::gettime() const
  {
    return std::chrono::system_clock::now().time_since_epoch();
  }
  bool SystemClock::settime(std::chrono::nanoseconds /* clocktime */)
  {
      // OpenRTM-aist do not set the System Clock.
      return true;
  }
  //
  //============================================================

  //============================================================
  // Logical Time Clock
  //============================================================
  LogicalClock::LogicalClock() = default;

  LogicalClock::~LogicalClock() = default;

  std::chrono::nanoseconds LogicalClock::gettime() const
  {
    std::lock_guard<std::mutex> guard(m_currentTimeMutex);
    return m_currentTime;
  }

  bool LogicalClock::settime(std::chrono::nanoseconds clocktime)
  {
    std::lock_guard<std::mutex> guard(m_currentTimeMutex);
    m_currentTime = clocktime;
    return true;
  }
  //
  //============================================================

  //============================================================
  // Adjusted Clock
  //============================================================
  AdjustedClock::AdjustedClock() = default;

  AdjustedClock::~AdjustedClock() = default;

  std::chrono::nanoseconds AdjustedClock::gettime() const
  {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> guard(m_offsetMutex);
    return now - m_offset;
  }

  bool AdjustedClock::settime(std::chrono::nanoseconds clocktime)
  {
    using dur_t = decltype(m_offset)::duration;
    auto offset = std::chrono::steady_clock::now()
                  - std::chrono::duration_cast<dur_t>(clocktime);
    std::lock_guard<std::mutex> guard(m_offsetMutex);
    m_offset = offset;
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
  ClockManager* ClockManager::clockmgr = nullptr;
  std::mutex ClockManager::clockmgr_mutex;
  ClockManager& ClockManager::instance()
  {
    if (clockmgr == nullptr)
      {
        std::lock_guard<std::mutex> guard(clockmgr_mutex);
        if (clockmgr == nullptr)
          {
            clockmgr = new ClockManager();
          }
      }
    return *clockmgr;
  }
#endif
} // namespace coil
