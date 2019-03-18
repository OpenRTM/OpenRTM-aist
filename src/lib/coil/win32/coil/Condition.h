// -*- C++ -*-
/*!
 * @file  Condition_win32.h
 * @brief Condition variable class for Win32
 * @date  $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
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

#ifndef COIL_CONDITION_H
#define COIL_CONDITION_H

#include <coil/Mutex.h>

#include <windows.h>
#include <algorithm>
#include <iostream>

namespace coil
{
  typedef struct
  {
    // Number of waiting threads.
    int waiters_count_;

    // Serialize access to <waiters_count_>.
    coil::Mutex waiters_count_lock_;

    // Semaphore used to queue up threads waiting for the condition to
    // become signaled.
    HANDLE sema_;

    // An auto-reset event used by the broadcast/signal thread to wait
    // for all the waiting thread(s) to wake up and be released from the
    // semaphore.
    HANDLE waiters_done_;

    // Keeps track of whether we were broadcasting or signaling.  This
    // allows us to optimize the code if we're just signaling.
    size_t was_broadcast_;

  } pthread_cond_t;


  inline int pthread_cond_init(pthread_cond_t *cv)
  {
    cv->waiters_count_ = 0;
    cv->was_broadcast_ = 0;
    cv->sema_ = ::CreateSemaphore(NULL,       // no security
                                  0,           // initially 0
                                  0x7fffffff,  // max count
                                  NULL);      // unnamed
    cv->waiters_done_ = ::CreateEvent(NULL,  // no security
                                      FALSE,  // auto-reset
                                      FALSE,  // non-signaled initially
                                      NULL);  // unnamed
    return 0;
  }

  /*!
   * @if jp
   *
   * @class Condition
   * @brief Condition テンプレートクラス
   *
   * @else
   *
   * @class Condition
   * @brief Condition template class
   *
   * @endif
   */
  template <class M>
  class Condition
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    explicit Condition(M& mutex)
      : m_mutex(mutex)
    {
      pthread_cond_init(&m_cond);
    }

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * デストラクタ。
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~Condition()
    {
    }

    /*!
     * @if jp
     *
     * @brief スレッド実行の再開
     *
     * 待機しているスレッド実行を再開させる。
     *
     * @else
     *
     * @brief Resume of the thread practice
     *
     * Let the practice of a waiting thread resume.
     *
     * @endif
     */
    inline void signal()
    {
      pthread_cond_signal(&m_cond);
    }

    /*!
     * @if jp
     *
     * @brief 全スレッド実行の再開
     *
     * 待機している全てのスレッド実行を再開させる。
     *
     * @else
     *
     * @brief Resume of all the thread practice
     *
     * Let all waiting thread practice resume.
     *
     * @endif
     */
    inline void broadcast()
    {
      pthread_cond_broadcast(&m_cond);
    }

    /*!
     * @if jp
     *
     * @brief スレッド実行の待機
     *
     * 条件変数が送信されるまでスレッドの実行を停止する。
     *
     * @return true: 成功, false: 失敗
     *
     * @else
     *
     * @brief Wait of the thread practice
     *
     * Stop the practice of the thread till a condition variable is transmitted.
     *
     * @return true: successful, false: failed
     *
     * @endif
     */
    bool wait()
    {
      return 0 == pthread_cond_wait(&m_cond, &m_mutex, INFINITE);
    }

    /*!
     * @if jp
     *
     * @brief 設定時間のスレッド実行待機
     *
     * 設定された時間、スレッドの実行を停止する。
     *
     * @param second 秒単位の時間
     * @param nano_second ナノ秒単位の時間
     *
     * @return true: 成功, false: 失敗
     *
     * @else
     *
     * @brief Thread practice wait of set time
     *
     * In set time, stop the practice of the thread.
     *
     * @param second Time of the seconds.
     * @param nano_second time of the nanoseconds.
     *
     * @return true: successful, false: failed
     *
     * @endif
     */
    bool wait(long second, long nano_second = 0)
    {
      DWORD milli_second = second * 1000 + nano_second / 1000000;
      return 0 == pthread_cond_wait(&m_cond, &m_mutex, milli_second);
    }

  private:
    /*!
     * @if jp
     *
     * @brief スレッド実行の待機
     *
     * シグナル状態になるまでスレッドの実行を停止する。
     *
     * @return 実行結果(成功:0、失敗:0以外)
     *
     * @else
     *
     * @brief Wait of the thread practice
     *
     * Stop the practice of the thread till become the signal state.
     *
     * @return If it is 0 succeed, other is fail.
     *
     * @endif
     */
  DWORD pthread_cond_wait(coil::pthread_cond_t *cv,
                        coil::Mutex *external_mutex, DWORD aMilliSecond)
  {
    DWORD result;

    // Avoid race conditions.
    cv->waiters_count_lock_.lock();
    cv->waiters_count_++;
    cv->waiters_count_lock_.unlock();

    // This call atomically releases the mutex and waits on the
    // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
    // are called by another thread.
//    std::cout << "Before SignalObjectAndWait [wait with time(" << milliSecond
//              << ")]" << std::endl << std::flush ;
    result = SignalObjectAndWait(external_mutex->mutex_,
                                 cv->sema_, aMilliSecond, FALSE);

//    char * p;
//    switch (result) {
//    case WAIT_ABANDONED :
//        p = "Abandoned";
//        break;
//    case WAIT_OBJECT_0 :
//        p = "Signaled";
//        break;
//    case WAIT_TIMEOUT :
//        p = "Timeout";
//        break;
//    default :
//        p = "Other !?";
//        break;
//    }
//      std::cout << "After SignalObjectAndWait [wait with time("
//                << milliSecond << ")]"
//        << " result(" << result << ":" << p << ")"
//        << std::endl << std::flush ;

    // Reacquire lock to avoid race conditions.
    cv->waiters_count_lock_.lock();

    // We're no longer waiting...
    cv->waiters_count_--;

    // Check to see if we're the last waiter after <pthread_cond_broadcast>.
    int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

    cv->waiters_count_lock_.unlock();

    // If we're the last waiter thread during this particular broadcast
    // then let all the other threads proceed.
    if (last_waiter)
      {
        // This call atomically signals the <waiters_done_> event and
        // waits until it can acquire the <external_mutex>.  This is
        // required to ensure fairness.
        result = SignalObjectAndWait(cv->waiters_done_,
                                           external_mutex->mutex_,
                                           INFINITE, FALSE);
//        std::cout << "result " << result << std::endl;
      }
    else
      {
        // Always regain the external mutex since that's the guarantee we
        // give to our callers.
        ::WaitForSingleObject(external_mutex->mutex_, 0);
      }
    return result;
  }

    /*!
     * @if jp
     *
     * @brief スレッド実行の再開
     *
     * 待機しているスレッド実行を再開させる。
     *
     * @return 実行結果(成功:0)
     *
     * @else
     *
     * @brief Resume of the thread practice
     *
     * Let the practice of a waiting thread resume.
     *
     * @return If it is 0 succeed.
     *
     * @endif
     */
  int pthread_cond_signal(pthread_cond_t *cv)
  {
    cv->waiters_count_lock_.lock();
    int have_waiters = cv->waiters_count_ > 0;
    cv->waiters_count_lock_.unlock();

    // If there aren't any waiters, then this is a no-op.
    if (have_waiters)
//    std::cout << "Before ReleaseSemaphore(1)" << std::endl << std::flush ;
      ReleaseSemaphore(cv->sema_, 1, 0);
//    std::cout << "After ReleaseSemaphore(1)" << std::endl << std::flush ;
    return 0;
  }

    /*!
     * @if jp
     *
     * @brief 全スレッド実行の再開
     *
     * 待機している全てのスレッド実行を再開させる。
     *
     * @return 実行結果(成功:0)
     *
     * @else
     *
     * @brief Resume of all the thread practice
     *
     * Let all waiting thread practice resume.
     *
     * @return If it is 0 succeed.
     *
     * @endif
     */
  int pthread_cond_broadcast(pthread_cond_t *cv)
  {
    // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
    // consistent relative to each other.
    cv->waiters_count_lock_.lock();
    int have_waiters = 0;

    if (cv->waiters_count_ > 0)
      {
        // We are broadcasting, even if there is just one waiter...
        // Record that we are broadcasting, which helps optimize
        // <pthread_cond_wait> for the non-broadcast case.
        cv->was_broadcast_ = 1;
        have_waiters = 1;
      }

    if (have_waiters)
      {
        // Wake up all the waiters atomically.
//      std::cout << "Before ReleaseSemaphore(" << cv->waiters_count_ << ")"
//                << std::endl << std::flush ;
        ReleaseSemaphore(cv->sema_, cv->waiters_count_, 0);
//      std::cout << "After ReleaseSemaphore(" << cv->waiters_count_ << ")"
//                << std::endl << std::flush ;

        cv->waiters_count_lock_.unlock();

        // Wait for all the awakened threads to acquire the counting
        // semaphore.
        WaitForSingleObject(cv->waiters_done_, INFINITE);
        // This assignment is okay, even without the <waiters_count_lock_> held
        // because no other waiter threads can wake up to access it.
        cv->was_broadcast_ = 0;
      }
    else
      {
        cv->waiters_count_lock_.unlock();
      }
    return 0;
  }

    explicit Condition(const Mutex&);
    Condition& operator=(const Mutex &);
    coil::pthread_cond_t m_cond;
    M& m_mutex;
  };  // class Condition

} // namespace coil
#endif  // COIL_CONDITION_H
