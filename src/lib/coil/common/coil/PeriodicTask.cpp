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
#include <coil/Time.h>

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
    : m_period(0.0), m_nowait(false),
      m_func(nullptr), m_deleteInDtor(true),
      m_alive(false), m_suspend(false),
      m_execCount(0), m_execCountMax(1000),
      m_periodCount(0), m_periodCountMax(1000)
  {
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
    if (m_func != nullptr && m_deleteInDtor)
      {
        delete m_func;
      }
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
    Guard guard(m_alive.mutex);
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
    Guard gaurd(m_alive.mutex);
    m_alive.value = false;

    Guard suspend_gaurd(m_suspend.mutex);
    m_suspend.suspend = false;
    m_suspend.cond.signal();
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
    Guard gaurd(m_suspend.mutex);
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
    m_periodTime.reset();
    m_execTime.reset();

    Guard gaurd(m_suspend.mutex);
    m_suspend.suspend = false;
    m_suspend.cond.signal();
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
    Guard gaurd(m_suspend.mutex);
    m_suspend.cond.signal();
  }

  /*!
   * @if jp
   * @brief タスク実行関数をセットする
   * @else
   * @brief Setting task execution function
   * @endif
   */
  bool PeriodicTask::setTask(TaskFuncBase* func, bool delete_in_dtor)
  {
    if (func == nullptr) { return false; }
    m_deleteInDtor = delete_in_dtor;
    m_func = func;
    return true;
  }

  /*!
   * @if jp
   * @brief タスク実行周期をセットする
   * @else
   * @brief Setting task execution period
   * @endif
   */
  void PeriodicTask::setPeriod(double period)
  {
    m_period = period;

    if (m_period.sec() == 0 && m_period.usec() == 0)
      {
        m_nowait = true;
        return;
      }
    m_nowait = false;
    return;
  }

  /*!
   * @if jp
   * @brief タスク実行周期をセットする
   * @else
   * @brief Setting task execution period
   * @endif
   */
  void PeriodicTask::setPeriod(TimeValue& period)
  {
    m_period = period;

    if (m_period.sec() == 0 && m_period.usec() == 0)
      {
        m_nowait = true;
        return;
      }
    m_nowait = false;
    return;
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
    Guard guard(m_execStat.mutex);
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
    Guard guard(m_periodStat.mutex);
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
    while (m_alive.value)  // needs lock?
      {
        if (m_periodMeasure) { m_periodTime.tack(); }
        {  // wait if suspended
          Guard suspend_gaurd(m_suspend.mutex);
          if (m_suspend.suspend)
            {
              m_suspend.cond.wait();
              // break if finalized
              if (!m_alive.value)
                {
                  return 0;
                }
            }
        }
        if (m_periodMeasure) { m_periodTime.tick(); }

        // task execution
        if (m_execMeasure) { m_execTime.tick(); }
        (*m_func)();
        if (m_execMeasure) { m_execTime.tack(); }

        // wait for next period
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
    if (m_nowait)
      {
        return;
      }
    coil::sleep(m_period - m_execTime.interval());
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
        Guard guard(m_execStat.mutex);
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
        Guard guard(m_periodStat.mutex);
        m_periodStat.stat = m_periodTime.getStatistics();
        m_periodCount = 0;
      }
    ++m_periodCount;
  }

} // namespace coil

