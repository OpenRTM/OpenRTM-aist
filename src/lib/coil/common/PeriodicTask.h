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

#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <coil/Condition.h>
#include <coil/TimeValue.h>
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
   * task.setTask(TaskFuncBase(obj, mem_func)); // 実行する関数を与える
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
    typedef coil::Guard<coil::Mutex> Guard;

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
    virtual ~PeriodicTask();

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
    virtual void activate();

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
    virtual void finalize();

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
    virtual int suspend(void);

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
    virtual int resume(void);

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
    virtual void signal();

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
    virtual bool setTask(TaskFuncBase* func, bool delete_in_dtor = true);

    /*!
     * @if jp
     * @brief タスク実行関数をセットする
     *
     * @param func int (*)() 型の関数ポインタ
     *
     * @return true: 成功, false: 失敗
     *
     * @else
     * @brief Setting task execution function
     *
     * @param func Set int (*)() type function pointer
     *
     * @return true: successful, false: failed
     *
     * @endif
     */
    template <class O, class F>
    bool setTask(O* obj, F fun)
    {
      return this->setTask(new TaskFunc<O, F>(obj, fun));
    }

    /*!
     * @if jp
     * @brief タスク実行周期をセットする
     *
     * @param period 実行周期 [sec]
     *
     * @else
     * @brief Setting task execution period
     *
     * @param period Execution period [sec]
     *
     * @endif
     */
    virtual void setPeriod(double period);

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
    virtual void setPeriod(TimeValue& period);

    /*!
     * @if jp
     * @brief タスク関数実行時間計測を有効にするか
     * @else
     * @brief Validate a Task execute time measurement
     * @endif
     */
    virtual void executionMeasure(bool value);

    /*!
     * @if jp
     * @brief タスク関数実行時間計測周期
     * @else
     * @brief Task execute time measurement period
     * @endif
     */
    virtual void executionMeasureCount(int n);

    /*!
     * @if jp
     * @brief タスク周期時間計測を有効にするか
     * @else
     * @brief Validate a Task period time measurement
     * @endif
     */
    virtual void periodicMeasure(bool value);

    /*!
     * @if jp
     * @brief タスク周期時間計測周期
     * @else
     * @brief Task period time measurement count
     * @endif
     */
    virtual void periodicMeasureCount(int n);

    /*!
     * @if jp
     * @brief タスク関数実行時間計測結果を取得
     * @else
     * @brief Get a result in task execute time measurement
     * @endif
     */
    virtual TimeMeasure::Statistics getExecStat();

    /*!
     * @if jp
     * @brief タスク周期時間計測結果を取得
     * @else
     * @brief Get a result in task period time measurement
     * @endif
     */
    virtual TimeMeasure::Statistics getPeriodStat();

  protected:
    /*!
     * @if jp
     * @brief PeriodicTask 用のスレッド実行
     * @else
     * @brief Thread execution for PeriodicTask
     * @endif
     */
    virtual int svc();

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
    coil::TimeValue m_period;

    /*!
     * @if jp
     * @brief スレッド休止フラグ
     * @else
     * @brief Thread sleep flag
     * @endif
     */
    bool m_nowait;

    /*!
     * @if jp
     * @brief タスク実行関数
     * @else
     * @brief Task execution function
     * @endif
     */
    TaskFuncBase* m_func;

    /*!
     * @if jp
     * @brief タスク実行関数削除フラグ
     * @else
     * @brief Task execution function delete flag
     * @endif
     */
    bool m_deleteInDtor;

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
      bool value;
      coil::Mutex mutex;
    };

    /*!
     * @if jp
     * @brief タスク生存フラグ
     * @else
     * @brief Task alive flag
     * @endif
     */
    alive_t m_alive;

    /*!
     * @if jp
     * @brief タスク中断管理用構造体
     * @else
     * @brief Structure for task suspend management
     * @endif
     */
    struct suspend_t
    {
      explicit suspend_t(bool sus) : suspend(sus),  cond(mutex) {}
      bool suspend;
      coil::Mutex mutex;
      coil::Condition<coil::Mutex> cond;
    };

    /*!
     * @if jp
     * @brief タスク中断情報
     * @else
     * @brief Task suspend infomation
     * @endif
     */
    suspend_t m_suspend;

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
      coil::Mutex mutex;
    };

    /*!
     * @if jp
     * @brief タスク実行時間計測フラグ
     * @else
     * @brief Task execution time measurement flag
     * @endif
     */
    bool              m_execMeasure;

    /*!
     * @if jp
     * @brief タスク実行時間計測回数
     * @else
     * @brief Task execution time measurement count
     * @endif
     */
    unsigned int      m_execCount;

    /*!
     * @if jp
     * @brief タスク実行時間計測周期
     * @else
     * @brief Task execution time measurement max count
     * @endif
     */
    unsigned int      m_execCountMax;

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
    bool              m_periodMeasure;

    /*!
     * @if jp
     * @brief タスク周期時間計測回数
     * @else
     * @brief Task periodic time measurement count
     * @endif
     */
    unsigned int      m_periodCount;

    /*!
     * @if jp
     * @brief タスク周期時間計測最大数
     * @else
     * @brief Task periodic time measurement max count
     * @endif
     */
    unsigned int      m_periodCountMax;

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

};  // namespace coil

#endif  // COIL_PERIODICTASK_H
