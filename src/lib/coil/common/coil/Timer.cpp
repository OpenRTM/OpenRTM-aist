// -*- C++ -*-
/*!
 * @file Timer.cpp
 * @brief Timer class
 * @date $Date: 2007-07-20 16:12:58 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2007-2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: Timer.cpp 826 2008-08-26 08:13:39Z n-ando $
 *
 */

#include <coil/Listener.h>
#include <coil/Timer.h>
#include <coil/Time.h>

#include <vector>

namespace coil
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  Timer::Timer(TimeValue& interval)
    : m_interval(interval), m_running(false)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  Timer::~Timer()
  {
    stop();
    wait();
  }

  /*!
   * @if jp
   * @brief Timer 用スレッド生成
   * @else
   * @brief Generate thread for Timer
   * @endif
   */
  int Timer::open(void * /*args*/)
  {
    activate();
    return 0;
  }

  /*!
   * @if jp
   * @brief Timer 用スレッド実行関数
   * @else
   * @brief Thread execution function for Timer
   * @endif
   */
  int Timer::svc()
  {
    TimeValue t_curr, t_pre, tm;
    while (m_running)
      {
        invoke();
        coil::sleep(m_interval);
      }
    return 0;
  }

  //============================================================
  // public functions
  //============================================================
  /*!
   * @if jp
   * @brief Timer タスク開始
   * @else
   * @brief Start Timer task
   * @endif
   */
  void Timer::start()
  {
    Guard guard(m_runningMutex);
    if (!m_running)
      {
        m_running = true;
        open(nullptr);
      }
  }

  /*!
   * @if jp
   * @brief Timer タスク停止
   * @else
   * @brief Stop Timer tast
   * @endif
   */
  void Timer::stop()
  {
    Guard guard(m_runningMutex);
    m_running = false;
  }

  /*!
   * @if jp
   * @brief Timer タスク実行
   * @else
   * @brief Invoke Timer task
   * @endif
   */
  void Timer::invoke()
  {
    Guard guard(m_taskMutex);
    for (auto & task : m_tasks)
      {
        task.remains = task.remains - m_interval;
        if (task.remains.sign() <= 0)
          {
            task.listener->invoke();
            task.remains = task.period;
          }
      }
  }

  /*!
   * @if jp
   * @brief リスナー登録
   * @else
   * @brief Register listener
   * @endif
   */
  ListenerId Timer::registerListener(ListenerBase* listener, TimeValue tm)
  {
    Guard guard(m_taskMutex);

    for (auto & task : m_tasks)
      {
        if (task.listener == listener)
          {
            task.period = tm;
            task.remains = tm;
            return listener;
          }
      }
    m_tasks.emplace_back(listener, tm);
    return listener;
  }

  /*!
   * @if jp
   * @brief リスナー登録解除
   * @else
   * @brief Unregister listener
   * @endif
   */
  bool Timer::unregisterListener(ListenerId id)
  {
    Guard guard(m_taskMutex);
    std::vector<Task>::iterator it;
    it = m_tasks.begin();

    for (size_t i(0), len(m_tasks.size()); i < len; ++i, ++it)
      {
        if (m_tasks[i].listener == id)
          {
            m_tasks.erase(it);
            return true;
          }
      }
    return false;
  }
} // namespace coil
