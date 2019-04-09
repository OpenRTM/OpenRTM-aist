// -*- C++ -*-
/*!
 * @file PeriodicTaskBase.h
 * @brief TaskFuncBase TaskFunc PeriodicTaskBase class
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

#ifndef COIL_PERIODICTASKBASE_H
#define COIL_PERIODICTASKBASE_H

#include <coil/TimeValue.h>
#include <coil/TimeMeasure.h>
#include <coil/Task.h>

namespace coil
{
  /*!
   * @if jp
   *
   * @class TaskFuncBase
   * @brief TaskFuncBase クラス
   *
   * @else
   *
   * @class TaskFuncBase
   * @brief TaskFuncBase class
   *
   * @endif
   */
  class TaskFuncBase
  {
  public:
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
    virtual ~TaskFuncBase() {}

    /*!
     * @if jp
     *
     * @brief オブジェクトの関数実行用純粋仮想関数
     *
     * オブジェクトの関数実行用純粋仮想関数。
     *
     * @else
     *
     * @brief Functor
     *
     * Pure virtual function for Functor.
     *
     * @endif
     */
    virtual int operator()() = 0;
  };

  /*!
   * @if jp
   *
   * @class TaskFunc
   * @brief TaskFunc テンプレートクラス
   *
   * @else
   *
   * @class TaskFunc
   * @brief TaskFunc template class
   *
   * @endif
   */
  template <typename T, typename F = int (*)()>
  class TaskFunc
    : public TaskFuncBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param obj オブジェクト
     * @param func 関数
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param obj Object.
     * @param func Function.
     *
     * @endif
     */
    TaskFunc(T* obj, F func)
      : m_obj(obj), m_func(func)
    {
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
    ~TaskFunc() override {}

    /*!
     * @if jp
     *
     * @brief オブジェクトの関数実行
     *
     * オブジェクトの関数を実行する。
     *
     * @else
     *
     * @brief Functor
     *
     * Execute a function of the object.
     *
     * @endif
     */
    int operator()() override
    {
      return (m_obj->*m_func)();
    }

    /*!
     * @if jp
     * @brief オブジェクト
     * @else
     * @brief object
     * @endif
     */
    T* m_obj;

    /*!
     * @if jp
     * @brief 関数
     * @else
     * @brief function
     * @endif
     */
    F m_func;
  };

  /*!
   * @if jp
   *
   * @class PeriodicTaskBase
   * @brief PeriodicTaskBase クラス
   *
   * @else
   *
   * @class PeriodicTaskBase
   * @brief PeriodicTaskBase class
   *
   * @endif
   */
  class PeriodicTaskBase
    : public coil::Task
  {
  public:
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
    ~PeriodicTaskBase() override {}

    /*!
     * @if jp
     *
     * @brief タスク実行を開始する純粋仮想関数
     *
     * タスク実行を開始する純粋仮想関数。
     *
     * @else
     *
     * @brief Starting the task
     *
     * Pure virtual function for starting the task.
     *
     * @endif
     */
    void activate() override = 0;

    /*!
     * @if jp
     *
     * @brief タスク実行を終了する純粋仮想関数
     *
     * タスク実行を終了する純粋仮想関数。
     *
     * @else
     *
     * @brief Finalizing the task
     *
     * Pure virtual function for finalizing the task.
     *
     * @endif
     */
    void finalize() override = 0;

    /*!
     * @if jp
     *
     * @brief タスク実行を中断する純粋仮想関数
     *
     * タスク実行を中断する純粋仮想関数。
     *
     * @else
     *
     * @brief Suspending the task
     *
     * Pure virtual function for suspending the task.
     *
     * @endif
     */
    int suspend() override = 0;

    /*!
     * @if jp
     *
     * @brief 中断されているタスクを再開する純粋仮想関数
     *
     * 中断されているタスクを再開する純粋仮想関数。
     *
     * @else
     *
     * @brief Resuming the suspended task
     *
     * Pure virtual function for resuming the suspended task.
     *
     * @endif
     */
    int resume() override = 0;

    /*!
     * @if jp
     *
     * @brief 中断されているタスクを1周期だけ実行する純粋仮想関数
     *
     * 中断されているタスクを1周期だけ実行する純粋仮想関数。
     *
     * @else
     *
     * @brief Executing the suspended task one tick
     *
     * Pure virtual function for executing the suspended task one tick.
     *
     * @endif
     */
    virtual void signal() = 0;

    /*!
     * @if jp
     *
     * @brief タスク実行関数をセットする純粋仮想関数
     *
     * タスク実行関数をセットする純粋仮想関数。
     *
     * @param func 関数
     * @param delete_in_dtor 削除フラグ
     *
     * @else
     *
     * @brief Setting task execution function
     *
     * Pure virtual function for setting task execution function.
     *
     * @param func Function.
     * @param delete_in_dtor Delete flag.
     *
     * @endif
     */
    virtual bool setTask(TaskFuncBase* func, bool delete_in_dtor = true) = 0;

    /*!
     * @if jp
     *
     * @brief タスク実行関数をセットする
     *
     * タスク実行関数をセットする
     *
     * @param obj オブジェクト
     * @param fun 関数
     *
     * @return true: 成功, false: 失敗
     *
     * @else
     *
     * @brief Setting task execution function
     *
     * Pure virtual function for setting task execution function.
     *
     * @param obj Object.
     * @param fun Function.
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
     *
     * @brief タスク実行周期をセットする純粋仮想関数
     *
     * タスク実行周期をセットする純粋仮想関数。
     *
     * @param period 実行周期
     *
     * @else
     *
     * @brief Setting task execution period
     *
     * Pure virtual function for setting task execution period.
     *
     * @param period Execution period.
     *
     * @endif
     */
    virtual void setPeriod(double period) = 0;

    /*!
     * @if jp
     *
     * @brief タスク実行周期をセットする純粋仮想関数
     *
     * タスク実行周期をセットする純粋仮想関数。
     *
     * @param period 実行周期
     *
     * @else
     *
     * @brief Setting task execution period
     *
     * Pure virtual function for setting task execution period.
     *
     * @param period Execution period.
     *
     * @endif
     */
    virtual void setPeriod(coil::TimeValue& period) = 0;

    /*!
     * @if jp
     *
     * @brief タスク関数実行時間計測を有効にする純粋仮想関数
     *
     * タスク関数実行時間計測を有効にする純粋仮想関数。
     *
     * @param value フラグ(true: 有効, false: 無効)
     *
     * @else
     *
     * @brief Validate a Task execute time measurement
     *
     * Pure virtual function for validate a Task execute time measurement.
     *
     * @param value flag(true: Valid, false: Invalid).
     *
     * @endif
     */
    virtual void executionMeasure(bool value) = 0;

    /*!
     * @if jp
     *
     * @brief タスク関数実行時間計測周期用純粋仮想関数
     *
     * タスク関数実行時間計測周期用純粋仮想関数。
     *
     * @param n 計測周期
     *
     * @else
     *
     * @brief Task execute time measurement period
     *
     * Pure virtual function for task execute time measurement period.
     *
     * @param n Measurement period.
     *
     * @endif
     */
    virtual void executionMeasureCount(unsigned int n) = 0;

    /*!
     * @if jp
     *
     * @brief タスク周期時間計測を有効にする純粋仮想関数
     *
     * タスク周期時間計測を有効にする純粋仮想関数。
     *
     * @param value フラグ(true: 有効, false: 無効)
     *
     * @else
     *
     * @brief Validate a Task period time measurement
     *
     * Pure virtual function for validate a Task period time measurement.
     *
     * @param value flag(true: Valid, false: Invalid).
     *
     * @endif
     */
    virtual void periodicMeasure(bool value) = 0;

    /*!
     * @if jp
     *
     * @brief タスク周期時間計測周期用純粋仮想関数
     *
     * タスク周期時間計測周期用純粋仮想関数。
     *
     * @param n 計測周期
     *
     * @else
     *
     * @brief Task period time measurement count
     *
     * Pure virtual function for task period time measurement count.
     *
     * @param n Measurement period.
     *
     * @endif
     */
    virtual void periodicMeasureCount(unsigned int n) = 0;

    /*!
     * @if jp
     *
     * @brief タスク関数実行時間計測結果を取得する純粋仮想関数
     *
     * タスク関数実行時間計測結果を取得する純粋仮想関数。
     *
     * @else
     *
     * @brief Get a result in task execute time measurement
     *
     * Pure virtual function for get a result in task execute time measurement.
     *
     * @endif
     */
    virtual coil::TimeMeasure::Statistics getExecStat() = 0;

    /*!
     * @if jp
     *
     * @brief タスク周期時間計測結果を取得する純粋仮想関数
     *
     * タスク周期時間計測結果を取得する純粋仮想関数。
     *
     * @else
     *
     * @brief Get a result in task period time measurement
     *
     * Pure virtual function for get a result in task period time measurement.
     *
     * @endif
     */
    virtual coil::TimeMeasure::Statistics getPeriodStat() = 0;

  };
} // namespace coil

#endif  // COIL_PERIODICTASKBASE_H
