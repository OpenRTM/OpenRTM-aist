// -*- C++ -*-
/*!
 * @file Timer.h
 * @brief Timer class
 * @date $Date: 2007-04-26 15:34:05 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2007-2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: Timer.h 826 2008-08-26 08:13:39Z n-ando $
 *
 */

#ifndef Timer_h
#define Timer_h

#include <coil/TimeValue.h>
#include <coil/Listener.h>
#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <coil/Task.h>
#include <vector>

typedef ListenerBase* ListenerId;

namespace coil
{
  /*!
   * @if jp
   * @class Timer
   * @brief Timerクラス
   *
   * 登録されたリスナーのコールバック関数を、設定された周期で定期的に呼び出す。
   *
   * @since 0.4.0
   *
   * @else
   * @class Timer
   * @brief Timer class
   *
   * Invoke the callback function of registered listener periodically
   * at the set cycle.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class Timer
    : public coil::Task
  {
    typedef coil::Mutex Mutex;
    typedef coil::Guard<Mutex> Guard;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param interval タイマ起動周期
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @param interval The interval of timer
     *
     * @endif
     */
    explicit Timer(TimeValue& interval);

    /*!
     * @if jp
     * @brief デストラクタ
     *
     * デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~Timer() override;

    //============================================================
    // ACE_Task
    //============================================================
    /*!
     * @if jp
     * @brief Timer 用スレッド生成
     *
     * Timer 用の内部スレッドを生成し起動する。
     * これは ACE_Task サービスクラスメソッドのオーバーライド。
     *
     * @param args 通常は0
     *
     * @return 生成処理実行結果
     *
     * @else
     * @brief Create thread for Timer
     *
     * Create an internal thread for Timer and launch it.
     * This is an override of ACE_Task service class method.
     *
     * @param args Usually 0
     *
     * @return Creation processing result
     *
     * @endif
     */
    int open(void *args) override;

    /*!
     * @if jp
     * @brief Timer 用のスレッド実行関数
     *
     * Timer 用のスレッド実行関数。
     * 登録されたリスナーのコールバック関数を呼び出す。
     *
     * @return 実行結果
     *
     * @else
     * @brief Thread execution function for Timer
     *
     * Thread execution function for Timer.
     * Invoke the callback function of registered listener.
     *
     * @return Execution result
     *
     * @endif
     */
    int svc() override;

    //============================================================
    // public functions
    //============================================================
    /*!
     * @if jp
     * @brief Timer タスク開始
     *
     * Timer 用新規スレッドを生成し、処理を開始する。
     *
     * @else
     * @brief Start Timer task
     *
     * Create a new theread for Timer and start processing.
     *
     * @endif
     */
    void start();

    /*!
     * @if jp
     * @brief Timer タスク停止
     *
     * Timer タスクを停止する。
     *
     * @else
     * @brief Stop Timer task
     *
     * Stop Timer task.
     *
     * @endif
     */
    void stop();

    /*!
     * @if jp
     * @brief Timer タスク実行
     *
     * 登録された各リスナの起動待ち時間からタイマ起動周期を減算する。
     * 起動待ち時間がゼロとなったリスナが存在する場合は、
     * コールバック関数を呼び出す。
     *
     * @else
     * @brief Invoke Timer task
     *
     * Subtract the interval of timer from the waiting time for invocation
     * of each registered listener.
     * If the listener whose waiting time reached 0 exists, invoke the
     * callback function.
     *
     * @endif
     */
    void invoke();

    /*!
     * @if jp
     * @brief リスナー登録
     *
     * 本 Timer から起動するコールバック関数用のリスナーを起動周期を指定して
     * 登録する。
     * 同一リスナーが既に登録済みの場合は、リスナーの起動周期を指定した値に
     * 更新する。
     *
     * @param listener 登録対象リスナー
     * @param tm リスナー起動周期
     *
     * @return 登録リスナーID
     *
     * @else
     * @brief Register listener
     *
     * Register the listener of callback function invoked from this Timer by
     * specifying the interval.
     * If the same listener has already been regiseterd, the value specified
     * the invocation interval of listener will be updated.
     *
     *
     * @param listener Listener for the registration
     * @param tm The invocation interval of listener
     *
     * @return ID of the registerd listener
     *
     * @endif
     */
    ListenerId registerListener(ListenerBase* listener, TimeValue tm);

    /*!
     * @if jp
     * @brief リスナー登録
     *
     * コールバック対象オブジェクト、コールバック対象メソッドおよび起動周期を
     * 指定してリスナーを登録する。
     *
     * @param obj コールバック対象オブジェクト
     * @param cbf コールバック対象メソッド
     * @param tm リスナー起動周期
     *
     * @return 登録リスナーID
     *
     * @else
     * @brief Register listener
     *
     * Register listener by specifying the object for callback, the method
     * for callback and the invocation interval.
     *
     * @param obj Target object for callback
     * @param cbf Target method for callback
     * @param tm The invocation interval of listener
     *
     * @return ID of the registerd listener
     *
     * @endif
     */
    template <class ListenerClass>
    ListenerId registerListenerObj(ListenerClass* obj,
                   void (ListenerClass::*cbf)(),
                   TimeValue tm)
    {
      return registerListener(new ListenerObject<ListenerClass>(obj, cbf), tm);
    }

    /*!
     * @if jp
     * @brief リスナー登録
     *
     * コールバック対象メソッドと起動周期を指定してリスナーを登録する。
     *
     * @param cbf コールバック対象メソッド
     * @param tm リスナー起動周期
     *
     * @return 登録リスナーID
     *
     * @else
     * @brief Register listener
     *
     * Register listener by specifying the method for callback and the
     * invocation interval.
     *
     * @param cbf Target method for callback
     * @param tm The invocation interval of listener
     *
     * @return ID of the registerd listener
     *
     * @endif
     */
    ListenerId registerListenerFunc(void (*cbf)(), TimeValue tm)
    {
      return registerListener(new ListenerFunc(cbf), tm);
    }

    /*!
     * @if jp
     * @brief リスナー登録解除
     *
     * 指定したIDのリスナーの登録を解除する。
     * 指定したIDのリスナーが未登録の場合、false を返す。
     *
     * @param id 登録解除対象リスナーID
     *
     * @return 登録解除結果
     *
     * @else
     * @brief Unregister listener
     *
     * Unregister the listener specified by ID.
     * If the listener specified by ID is not registerd, false will be returned.
     *
     * @param id ID of the unregisterd listener
     *
     * @return Unregistration result
     *
     * @endif
     */
    bool unregisterListener(ListenerId id);

  private:
    TimeValue m_interval;

    Mutex m_runningMutex;
    bool m_running;

    struct Task
    {
      Task(ListenerBase* l, TimeValue p)
        : listener(l), period(p), remains(p)
      {
      }
      ListenerBase* listener;
      TimeValue period;
      TimeValue remains;
    };

    std::vector<Task> m_tasks;
    Mutex  m_taskMutex;
  };
} // namespace coil
#endif  // Timer_h

