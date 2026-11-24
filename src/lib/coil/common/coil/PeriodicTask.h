// -*- C++ -*-
/*!
 * @file PeriodicTask.h
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

#ifndef COIL_PERIODICTASK_H
#define COIL_PERIODICTASK_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <coil/TimeMeasure.h>
#include <coil/PeriodicTaskBase.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace coil
{
  /*!
   * @if jp
   * @class PeriodicTask
   * @brief 周期タスクスレッド実行クラス
   *
   * 特定の関数を周期実行するためのスレッドオブジェクトを実現する。
   * 使用手順は以下の通り。
   *
   * task; // インスタンス生成
   * task.setTask(function); // 実行する関数を与える
   * task.activate(); // スレッドをスタートさせる
   *
   * task.suspend(); // 周期実行を止める
   * task.signal(); // 1周期だけ実行
   * task.resume(); // 周期実行を再開
   *
   * task.finalize(); // タスクを終了させる
   *
   * @else
   * @class PeriodicTask
   * @brief PeriodicTask class
   *
   * @endif
   */
  class PeriodicTask
    : public coil::PeriodicTaskBase
  {
  public:

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    PeriodicTask();

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
    ~PeriodicTask() override;

    /*!
     * @if jp
     * @brief タスク実行を開始する
     *
     * タスクの実行を開始するためにスレッドをスタートさせる。  タスクが
     * 正常に開始された場合は true が返り、すでにタスクが開始済み、また
     * は実行するタスクが設定されていなければ false を返す。
     *
     * @return true: 正常開始、false: スレッド済みかタスクが未設定である。
     *
     * @else
     * @brief Starting the task
     *
     * Starting a thread to execute a task.  If the task/thread is
     * started properly, it will return 'TRUE'.  if the task/thread
     * are already started or task function object is not set, 'FALSE'
     * will be returned.
     *
     * @return true: normal start, false: already started  or task is not set
     *
     * @endif
     */
    void activate() override;

    /*!
     * @if jp
     * @brief タスク実行を終了する
     *
     * 実行中のタスクを終了する。
     *
     * @else
     * @brief Finalizing the task
     *
     * Finalizing the task running.
     *
     * @endif
     */
    void finalize() override;

    /*!
     * @if jp
     * @brief タスク実行を中断する
     *
     * 実行中のタスクを中断する。
     *
     * @else
     * @brief Suspending the task
     *
     * Suspending the task running.
     *
     * @endif
     */
    int suspend() override;

    /*!
     * @if jp
     * @brief 中断されているタスクを再開する
     *
     * 中断されているタスクを再開する
     *
     * @else
     * @brief Resuming the suspended task
     *
     * Resuming the suspended task
     *
     * @endif
     */
    int resume() override;

    /*!
     * @if jp
     * @brief 中断されているタスクを1周期だけ実行する
     *
     * 中断されているタスクを1周期だけ実行する
     *
     * @else
     * @brief Executing the suspended task one tick
     *
     * Executing the suspended task one tick
     *
     * @endif
     */
    void signal() override;

    /*!
     * @if jp
     * @brief タスク実行関数をセットする
     *
     * @param func int (*)() 型の関数ポインタ
     *
     * @else
     * @brief Setting task execution function
     *
     * @param func Set int (*)() type function pointer
     *
     * @endif
     */
    void setTask(std::function<void(void)> func) override;

    /*!
     * @if jp
     * @brief タスク実行周期をセットする
     *
     * @param period 実行周期
     *
     * @else
     * @brief Setting task execution period
     *
     * @param period Execution period
     *
     * @endif
     */
    void setPeriod(std::chrono::nanoseconds period) override;

    /*!
     * @if jp
     * @brief タスク関数実行時間計測を有効にするか
     * @else
     * @brief Validate a Task execute time measurement
     * @endif
     */
    void executionMeasure(bool value) override;

    /*!
     * @if jp
     * @brief タスク関数実行時間計測周期
     * @else
     * @brief Task execute time measurement period
     * @endif
     */
    void executionMeasureCount(unsigned int n) override;

    /*!
     * @if jp
     * @brief タスク周期時間計測を有効にするか
     * @else
     * @brief Validate a Task period time measurement
     * @endif
     */
    void periodicMeasure(bool value) override;

    /*!
     * @if jp
     * @brief タスク周期時間計測周期
     * @else
     * @brief Task period time measurement count
     * @endif
     */
    void periodicMeasureCount(unsigned int n) override;

    /*!
     * @if jp
     * @brief タスク関数実行時間計測結果を取得
     * @else
     * @brief Get a result in task execute time measurement
     * @endif
     */
    TimeMeasure::Statistics getExecStat() override;

    /*!
     * @if jp
     * @brief タスク周期時間計測結果を取得
     * @else
     * @brief Get a result in task period time measurement
     * @endif
     */
    TimeMeasure::Statistics getPeriodStat() override;

  protected:
    /*!
     * @if jp
     * @brief PeriodicTask 用のスレッド実行
     * @else
     * @brief Thread execution for PeriodicTask
     * @endif
     */
    int svc() override;

    /*!
     * @if jp
     * @brief スレッド休止
     * @else
     * @brief Thread sleep
     * @endif
     */
    virtual void sleep();

    /*!
     * @if jp
     * @brief 実行状態更新
     * @else
     * @brief Update for execute state
     * @endif
     */
    virtual void updateExecStat();

    /*!
     * @if jp
     * @brief 周期状態更新
     * @else
     * @brief Update for period state
     * @endif
     */
    virtual void updatePeriodStat();

  protected:
    /*!
     * @if jp
     * @brief タスク実行周期
     * @else
     * @brief Task execution period
     * @endif
     */
    std::atomic<std::chrono::nanoseconds> m_period{std::chrono::nanoseconds::zero()};

    /*!
     * @if jp
     * @brief タスク実行関数
     * @else
     * @brief Task execution function
     * @endif
     */
    std::function<void(void)> m_func;

    /*!
     * @if jp
     * @class alive_t
     * @brief alive_t クラス
     * @else
     * @class alive_t
     * @brief alive_t class
     * @endif
     */
    class alive_t
    {
    public:
      explicit alive_t(bool val) : value(val) {}
      std::atomic<bool> value;
      std::mutex mutex;
    };

    /*!
     * @if jp
     * @brief タスク生存フラグ
     * @else
     * @brief Task alive flag
     * @endif
     */
    alive_t m_alive{false};

    /*!
     * @if jp
     * @brief タスク中断管理用構造体
     * @else
     * @brief Structure for task suspend management
     * @endif
     */
    struct suspend_t
    {
      explicit suspend_t(bool sus) : suspend(sus) {}
      bool suspend;
      bool resetMeasurements{false};
      std::mutex mutex;
      std::condition_variable cond;
    };

    /*!
     * @if jp
     * @brief タスク中断情報
     * @else
     * @brief Task suspend infomation
     * @endif
     */
    suspend_t m_suspend{false};

    /*!
     * @if jp
     * @brief タスク実行時間計測管理用構造体
     * @else
     * @brief Structure for task execution time measurement management
     * @endif
     */
    struct statistics_t
    {
      coil::TimeMeasure::Statistics stat;
      std::mutex mutex;
    };

    /*!
     * @if jp
     * @brief タスク実行時間計測フラグ
     * @else
     * @brief Task execution time measurement flag
     * @endif
     */
    std::atomic<bool> m_execMeasure{false};

    /*!
     * @if jp
     * @brief タスク実行時間計測回数
     * @else
     * @brief Task execution time measurement count
     * @endif
     */
    unsigned int      m_execCount{0};

    /*!
     * @if jp
     * @brief タスク実行時間計測周期
     * @else
     * @brief Task execution time measurement max count
     * @endif
     */
    std::atomic<unsigned int> m_execCountMax{1000};

    /*!
     * @if jp
     * @brief タスク実行時間計測統計
     * @else
     * @brief Task execution time measurement statistics
     * @endif
     */
    statistics_t      m_execStat;

    /*!
     * @if jp
     * @brief タスク実行時間計測情報
     * @else
     * @brief Task execution time  measurement infomation
     * @endif
     */
    coil::TimeMeasure m_execTime;

    /*!
     * @if jp
     * @brief タスク周期時間計測フラグ
     * @else
     * @brief Task periodic time measurement flag
     * @endif
     */
    std::atomic<bool> m_periodMeasure{false};

    /*!
     * @if jp
     * @brief タスク周期時間計測回数
     * @else
     * @brief Task periodic time measurement count
     * @endif
     */
    unsigned int      m_periodCount{0};

    /*!
     * @if jp
     * @brief タスク周期時間計測最大数
     * @else
     * @brief Task periodic time measurement max count
     * @endif
     */
    std::atomic<unsigned int> m_periodCountMax{1000};

    /*!
     * @if jp
     * @brief タスク周期時間計測統計
     * @else
     * @brief Task periodic time measurement statistics
     * @endif
     */
    statistics_t      m_periodStat;

    /*!
     * @if jp
     * @brief タスク周期時間計測情報
     * @else
     * @brief Task periodic time  measurement infomation
     * @endif
     */
    coil::TimeMeasure m_periodTime;

  };

} // namespace coil

#endif  // COIL_PERIODICTASK_H
