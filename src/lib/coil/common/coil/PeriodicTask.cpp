// -*- C++ -*-
/*!
 * @file PeriodicTask.cpp
 * @brief PeriodicTask class
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

#include <coil/PeriodicTask.h>

namespace coil
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  PeriodicTask::PeriodicTask()
  {
    m_execStat.stat = {};
    m_periodStat.stat = {};
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  PeriodicTask::~PeriodicTask()
  {
    finalize();
    wait();
  }

  /*!
   * @if jp
   * @brief タスク実行を開始する
   * @else
   * @brief Starting the task
   * @endif
   */
  void PeriodicTask::activate()
  {
    std::lock_guard<std::mutex> guard(m_alive.mutex);
    if (m_func == nullptr)   { return; }
    if (m_alive.value) { return; }

    m_alive.value = true;
    Task::activate();
  }

  /*!
   * @if jp
   * @brief タスク実行を終了する
   * @else
   * @brief Finalizing the task
   * @endif
   */
  void PeriodicTask::finalize()
  {
    std::lock_guard<std::mutex> guard(m_alive.mutex);
    m_alive.value = false;

    std::lock_guard<std::mutex> suspend_guard(m_suspend.mutex);
    m_suspend.suspend = false;
    m_suspend.cond.notify_one();
  }

  /*!
   * @if jp
   * @brief タスク実行を中断する
   * @else
   * @brief Suspending the task
   * @endif
   */
  int PeriodicTask::suspend()
  {
    std::lock_guard<std::mutex> guard(m_suspend.mutex);
    m_suspend.suspend = true;
    return 0;
  }

  /*!
   * @if jp
   * @brief 中断されているタスクを再開する
   * @else
   * @brief Resuming the suspended task
   * @endif
   */
  int PeriodicTask::resume()
  {
    std::lock_guard<std::mutex> guard(m_suspend.mutex);
    m_suspend.resetMeasurements = true;
    m_suspend.suspend = false;
    m_suspend.cond.notify_one();
    return 0;
  }

  /*!
   * @if jp
   * @brief 中断されているタスクを1周期だけ実行する
   * @else
   * @brief Executing the suspended task one tick
   * @endif
   */
  void PeriodicTask::signal()
  {
    std::lock_guard<std::mutex> guard(m_suspend.mutex);
    m_suspend.cond.notify_one();
  }

  /*!
   * @if jp
   * @brief タスク実行関数をセットする
   * @else
   * @brief Setting task execution function
   * @endif
   */
  void PeriodicTask::setTask(std::function<void(void)> func)
  {
    m_func = std::move(func);
  }

  /*!
   * @if jp
   * @brief タスク実行周期をセットする
   * @else
   * @brief Setting task execution period
   * @endif
   */
  void PeriodicTask::setPeriod(std::chrono::nanoseconds period)
  {
    m_period = period;
  }

  /*!
   * @if jp
   * @brief タスク関数実行時間計測を有効にするか
   * @else
   * @brief Validate a Task execute time measurement
   * @endif
   */
  void PeriodicTask::executionMeasure(bool value)
  {
    m_execMeasure = value;
  }

  /*!
   * @if jp
   * @brief タスク関数実行時間計測周期
   * @else
   * @brief Task execute time measurement period
   * @endif
   */
  void PeriodicTask::executionMeasureCount(unsigned int n)
  {
    m_execCountMax = n;
  }

  /*!
   * @if jp
   * @brief タスク周期時間計測を有効にするか
   * @else
   * @brief Validate a Task period time measurement
   * @endif
   */
  void PeriodicTask::periodicMeasure(bool value)
  {
    m_periodMeasure = value;
  }

  /*!
   * @if jp
   * @brief タスク周期時間計測周期
   * @else
   * @brief Task period time measurement count
   * @endif
   */
  void PeriodicTask::periodicMeasureCount(unsigned int n)
  {
    m_periodCountMax = n;
  }

  /*!
   * @if jp
   * @brief タスク関数実行時間計測結果を取得
   * @else
   * @brief Get a result in task execute time measurement
   * @endif
   */
  TimeMeasure::Statistics PeriodicTask::getExecStat()
  {
    std::lock_guard<std::mutex> guard(m_execStat.mutex);
    return m_execStat.stat;
  }

  /*!
   * @if jp
   * @brief タスク周期時間計測結果を取得
   * @else
   * @brief Get a result in task period time measurement
   * @endif
   */
  TimeMeasure::Statistics PeriodicTask::getPeriodStat()
  {
    std::lock_guard<std::mutex> guard(m_periodStat.mutex);
    return m_periodStat.stat;
  }

  //----------------------------------------------------------------------
  // protected functions
  //----------------------------------------------------------------------
  /*!
   * @if jp
   * @brief PeriodicTask 用のスレッド実行
   * @else
   * @brief Thread execution for PeriodicTask
   * @endif
   */
  int PeriodicTask::svc()
  {
    bool periodStarted = false;
    while (m_alive.value)
      {
        {
          std::unique_lock<std::mutex> suspend_guard(m_suspend.mutex);
          if (m_suspend.suspend)
            {
              periodStarted = false;
              m_suspend.cond.wait(suspend_guard);
            }
          if (!m_alive.value)
            {
              return 0;
            }
          if (m_suspend.resetMeasurements)
            {
              m_periodTime.reset();
              m_execTime.reset();
              m_periodCount = 0;
              m_execCount = 0;
              periodStarted = false;
              m_suspend.resetMeasurements = false;
            }
        }
        const bool measurePeriod = m_periodMeasure;
        if (measurePeriod)
          {
            if (periodStarted) { m_periodTime.tack(); }
            m_periodTime.tick();
          }
        periodStarted = measurePeriod;

        const bool measureExecution = m_execMeasure;
        if (measureExecution) { m_execTime.tick(); }
        m_func();
        if (measureExecution) { m_execTime.tack(); }

        updateExecStat();
        sleep();
        updatePeriodStat();
      }
    return 0;
  }

  //----------------------------------------------------------------------
  // protected member function
  //----------------------------------------------------------------------

  /*!
   * @if jp
   * @brief スレッド休止
   * @else
   * @brief Thread sleep
   * @endif
   */
  void PeriodicTask::sleep()
  {
    const auto period = m_period.load();
    if (period > std::chrono::seconds::zero())
      {
        std::this_thread::sleep_for(period - (m_execMeasure ? m_execTime.interval() : std::chrono::nanoseconds::zero()));
      }
  }

  /*!
   * @if jp
   * @brief 実行状態更新
   * @else
   * @brief Update for execute state
   * @endif
   */
  void PeriodicTask::updateExecStat()
  {
    if (m_execCount > m_execCountMax)
      {
        std::lock_guard<std::mutex> guard(m_execStat.mutex);
        m_execStat.stat = m_execTime.getStatistics();
        m_execCount = 0;
      }
    ++m_execCount;
  }

  /*!
   * @if jp
   * @brief 周期状態更新
   * @else
   * @brief Update for period state
   * @endif
   */
  void PeriodicTask::updatePeriodStat()
  {
    if (m_periodCount > m_periodCountMax)
      {
        std::lock_guard<std::mutex> guard(m_periodStat.mutex);
        m_periodStat.stat = m_periodTime.getStatistics();
        m_periodCount = 0;
      }
    ++m_periodCount;
  }

} // namespace coil

