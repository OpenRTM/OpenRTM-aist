// -*- C++ -*-
/*!
 * @file ClockManager.h
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

#ifndef COIL_CLOCKMANAGER_H
#define COIL_CLOCKMANAGER_H

#include <coil/Singleton.h>
#include <coil/TimeValue.h>
#include <coil/Mutex.h>
#include <coil/Guard.h>

#include <string>

namespace coil
{
  /*!
   * @if jp
   * @class 時刻設定・取得オブジェクトのインターフェース
   *
   * このクラスは ClockManager によって管理されるクロックオブジェクトの
   * ためのインターフェースである。ClockManager は複数のクロックオブジェ
   * クトを管理し、必要に応じて適切なクロックオブジェクトを IClock イン
   * ターフェースをもつオブジェクトとして返す。クロックオブジェクトは単
   * にシステム時刻を返すものや、独自の論理時刻を持つクロックオブジェク
   * ト等が考えられる。
   *
   * @else
   * @brief An interface to set and get time
   *
   * This class is a interface for clock objects managed by
   * ClockManager. ClockManager manages one or more clocks, and it
   * returns appropriate clock objects according to demands. The clock
   * object might be clock which just returns system time, or a clock
   * which returns individual logical time.
   *
   * @endif
   */
  class IClock
  {
  public:
    virtual ~IClock() {}
    /*!
     * @if jp
     * @brief 時刻を取得する
     * @return 現在の時刻
     * @else
     * @brief Getting time
     * @return Current time
     * @endif
     */
    virtual coil::TimeValue gettime() const = 0;
    /*!
     * @if jp
     * @brief 時刻を設定する
     * @param clocktime 現在の時刻
     * @else
     * @brief Setting time
     * @param clocktime Current time
     * @endif
     */
    virtual bool settime(coil::TimeValue clocktime) = 0;
  };

  /*!
   * @if jp
   * @class システム時刻を扱うクロックオブジェクト
   *
   * このクラスはシステムクロックを設定または取得するクラスである。
   *
   * @else
   * @brief clock object to handle system clock
   *
   * This class sets and gets system clock.
   *
   * @endif
   */
  class SystemClock
    : public IClock
  {
  public:
    ~SystemClock() override;
    coil::TimeValue gettime() const override;
    bool settime(coil::TimeValue clocktime) override;
  };

  /*!
   * @if jp
   * @class 論理時間を扱うクロックオブジェクト
   *
   * このクラスは論理時間を設定または取得するクラスである。
   * 単純に settime() によって設定された時刻を gettime() によって取得するす。
   *
   * @else
   * @brief Clock object to handle logical clock
   *
   * This class sets and gets system clock.
   * It just sets time by settime() and gets time by gettime().
   *
   * @endif
   */
  class LogicalClock
    : public IClock
  {
  public:
    LogicalClock();
    ~LogicalClock() override;
    coil::TimeValue gettime() const override;
    bool settime(coil::TimeValue clocktime) override;
  private:
    coil::TimeValue m_currentTime;
    mutable coil::Mutex m_currentTimeMutex;
  };

  /*!
   * @if jp
   * @class 調整済み時刻を扱うクロックオブジェクト
   *
   * settime() 呼び出し時に現在時刻との差をオフセットとして保持し、
   * gettime() によってオフセット調整済みの時刻を返す。
   *
   * @else
   * @brief Clock object to handle adjusted clock
   *
   * This class stores a offset time with current system clock when
   * settime(), and gettime() returns adjusted clock by the offset.
   *
   * @endif
   */
  class AdjustedClock
    : public IClock
  {
  public:
    AdjustedClock();
    ~AdjustedClock() override;
    coil::TimeValue gettime() const override;
    bool settime(coil::TimeValue clocktime) override;
  private:
    coil::TimeValue m_offset;
    mutable coil::Mutex m_offsetMutex;
  };

  /*!
   * @if jp
   * @class グローバルなクロック管理クラス。
   *
   * このクラスはグローバルにクロックオブジェクトを提供するシングルトン
   * クラスである。getClocK(クロック名) により IClock 型のクロックオブ
   * ジェクトを返す。利用可能なクロックは "system", "logical" および
   * "adjusted" の３種類である。
   *
   * @else
   * @brief A global clock management class
   *
   * This class is a singleton class that provides clock objects
   * globally. It provides a IClock object by getClock(<clock
   * type>). As clock types, "system", "logical" and "adjusted" are
   * available.
   *
   * @endif
   */
  class ClockManager
#ifndef WIN32
    : public coil::Singleton<ClockManager>
#endif
  {
  public:
    IClock& getClock(const std::string& clocktype);
#ifdef WIN32
    static ClockManager& instance();
    static ClockManager* clockmgr;
    static coil::Mutex clockmgr_mutex;
  private:
    ClockManager() {}
#endif
  private:
    SystemClock   m_systemClock;
    LogicalClock  m_logicalClock;
    AdjustedClock m_adjustedClock;
  };
} // namespace coil
#endif  // COIL_CLOCKMANAGER_H
