// -*- C++ -*-
/*!
 * @file TimeMeasure.cpp
 * @brief Periodic time measurement class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/Time.h>
#include <coil/TimeMeasure.h>
#include <cmath>

//#define RDTSC(X) __asm__ __volatile__ ("rdtsc" : "=A" (X))
//#define NSEC_PER_SEC 1000000000

#ifndef ULLONG_MAX
#define ULLONG_MAX 0xffffffffffffffffULL
#endif

namespace coil
{

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  TimeMeasure::TimeMeasure(unsigned long buflen)
    : m_begin(0.0), m_interval(0.0),
      m_count(0), m_countMax(buflen + 1),
      m_recurred(false)
  {
    m_record.reserve(m_countMax);
    for (unsigned long int i(0); i < m_countMax; ++i)
      {
        m_record.emplace_back(0, 0);
      }
  }

  /*!
   * @if jp
   * @brief 時間統計の計測を開始する
   * @else
   * @brief Begin time measurement for time statistics
   * @endif
   */
  void TimeMeasure::tick()
  {
    m_begin = clock();  // [TimeValue]
  }

  /*!
   * @if jp
   * @brief 時間統計の計測を終了する
   * @else
   * @brief Finish time measurement for time statistics
   * @endif
   */
  void TimeMeasure::tack()
  {
    if (m_begin.sec() == 0) { return; }

    m_interval = clock() - m_begin;
    m_record.at(m_count) = m_interval;
    ++m_count;
    if (m_count == m_countMax)
      {
        m_count = 0;
        m_recurred = true;
      }
  }

  /*!
   * @if jp
   * @brief 経過時間を取得する
   * @else
   * @brief Get a interval time
   * @endif
   */
  coil::TimeValue& TimeMeasure::interval()
  {
    return m_interval;
  }

  /*!
   * @if jp
   * @brief 統計関連データの初期化
   * @else
   * @brief Initialize for statistics related data
   * @endif
   */
  void TimeMeasure::reset()
  {
    m_count = 0;
    m_recurred = false;
    m_begin = 0.0;
  }

  /*!
   * @if jp
   * @brief 時間統計バッファサイズを取得する
   * @else
   * @brief Get number of time measurement buffer
   * @endif
   */
  unsigned long int TimeMeasure::count() const
  {
    return m_recurred ?
      static_cast<unsigned long int>(m_record.size()) : m_count;
  }

  /*!
   * @if jp
   * @brief 統計データの総計を取得する
   * @else
   * @brief Get total statistics
   * @endif
   */
  bool TimeMeasure::getStatistics(double &max_interval,
                                  double &min_interval,
                                  double &mean_interval,
                                  double &stddev)
  {
    max_interval = static_cast<double>(0);
    min_interval = static_cast<double>(ULLONG_MAX);

    double sum = 0;
    double sq_sum = 0;
    unsigned long int len(count());

    if (len == 0UL) { return false; }

    for (unsigned long int i(0); i < len; ++i)
      {
        double trecord(m_record[i]);
        sum += trecord;
        sq_sum += trecord * trecord;

        if (trecord > max_interval) max_interval = trecord;
        if (trecord < min_interval) min_interval = trecord;
      }

    mean_interval = sum / len;
    stddev = sqrt(sq_sum / len - (mean_interval * mean_interval));

    return true;
  }

  /*!
   * @if jp
   * @brief 統計結果を取得する
   * @else
   * @brief Get statistics result
   * @endif
   */
  TimeMeasure::Statistics TimeMeasure::getStatistics()
  {
    Statistics s;
    getStatistics(s.max_interval, s.min_interval,
                  s.mean_interval, s.std_deviation);
    return s;
  }

} // namespace coil
