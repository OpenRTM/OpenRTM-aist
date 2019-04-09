// -*- C++ -*-
/*!
 * @file Timevalue.cpp
 * @brief Timevalue class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/TimeValue.h>

namespace coil
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  TimeValue::TimeValue(long sec, long usec)
  {
    m_sec = sec;
    m_usec = usec;
    normalize();
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  TimeValue::TimeValue(double timeval)
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
    m_sec = static_cast<long int>(timeval);
    m_usec = static_cast<long>((timeval - static_cast<double>(m_sec))
                    * TIMEVALUE_ONE_SECOND_IN_USECS + dbHalfAdj);
    normalize();
  }

  /*!
   * @if jp
   * @brief 時間減算
   * @else
   * @brief Time subtraction
   * @endif
   */
  TimeValue TimeValue::operator-(TimeValue& tm)
  {
    TimeValue res;
    if (m_sec >= tm.m_sec)  // +
      {
        if (m_usec >= tm.m_usec) /* 繰り下がり無し */
          {
            res.m_sec  = m_sec  - tm.m_sec;   // -
            res.m_usec = m_usec - tm.m_usec;  // +
          }
        else /* m_usec < tm.m_usec 繰り下がり有り */
          {
            res.m_sec  = m_sec  - tm.m_sec - 1;
            res.m_usec = (m_usec + 1000000) - tm.m_usec;
          }
      }
    else  // m_sec < tm.m_sec // -
      {
          if (tm.m_usec >= m_usec) /* 繰り下がり無し */
            {
              res.m_sec  = - (tm.m_sec  - m_sec);   // +
              res.m_usec = - (tm.m_usec - m_usec);  // +
            }
          else /* tm.m_usec < m_usec 繰り下がり有り */
            {
              res.m_sec  = - (tm.m_sec - m_sec  - 1);
              res.m_usec = - (tm.m_usec + 1000000) + m_usec;
            }
      }
    res.normalize();
    return res;
  }

  /*!
   * @if jp
   * @brief 時間加算
   * @else
   * @brief Time addition
   * @endif
   */
  TimeValue TimeValue::operator+(TimeValue& tm)
  {
    TimeValue res;
    res.m_sec  = m_sec  + tm.m_sec;
    res.m_usec = m_usec + tm.m_usec;
    if (res.m_usec >= 1000000)
      {
        ++res.m_sec;
        res.m_usec -= 1000000;
      }
    res.normalize();
    return res;
  }

  /*!
   * @if jp
   * @brief double型→時間型変換
   * @else
   * @brief Convert double type into time type
   * @endif
   */
  TimeValue TimeValue::operator=(double time)
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

    m_sec = static_cast<long>(time);
    m_usec = static_cast<long>((time - static_cast<double>(m_sec)) *
                     TIMEVALUE_ONE_SECOND_IN_USECS + dbHalfAdj);
    normalize();
    return *this;
  }

  /*!
   * @if jp
   * @brief 時間型→double型変換
   * @else
   * @brief Convert time type into double type
   * @endif
   */
  TimeValue::operator double() const
  {
    return static_cast<double>(m_sec) +
          (static_cast<double>(m_usec) / TIMEVALUE_ONE_SECOND_IN_USECS);
  }

  /*!
   * @if jp
   * @brief 符号判定
   * @else
   * @brief Sign judgment
   * @endif
   */
  int TimeValue::sign() const
  {
    if (m_sec > 0) return 1;
    if (m_sec < 0) return -1;
    if (m_usec > 0) return 1;
    if (m_usec < 0) return -1;
    return 0;
  }

  /*!
   * @if jp
   * @brief 正規化
   * @else
   * @brief Normalize
   * @endif
   */
  void TimeValue::normalize()
  {
    if (m_usec >= TIMEVALUE_ONE_SECOND_IN_USECS)
      {
        do
          {
            ++m_sec;
            m_usec -= TIMEVALUE_ONE_SECOND_IN_USECS;
          }
        while (m_usec >= TIMEVALUE_ONE_SECOND_IN_USECS);
      }
    else if (m_usec <= -TIMEVALUE_ONE_SECOND_IN_USECS)
      {
        do
          {
            --m_sec;
            m_usec += TIMEVALUE_ONE_SECOND_IN_USECS;
          }
        while (m_usec <= -TIMEVALUE_ONE_SECOND_IN_USECS);
      }

    if (m_sec >= 1 && m_usec < 0)
      {
        --m_sec;
        m_usec += TIMEVALUE_ONE_SECOND_IN_USECS;
      }
    else if (m_sec < 0 && m_usec > 0)
      {
        ++m_sec;
        m_usec -= TIMEVALUE_ONE_SECOND_IN_USECS;
      }
    else
      {
      }
  }

} // namespace coil
