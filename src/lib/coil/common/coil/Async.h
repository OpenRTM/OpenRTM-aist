// -*- C++ -*-
/*!
 * @file Async.h
 * @brief Asynchronous function invocation helper class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_ASYNC_H
#define COIL_ASYNC_H

#include <coil/Task.h>
#include <coil/Guard.h>
#include <iostream>
#include <utility>

namespace coil
{
  /*!
   * @if jp
   *
   * @class Async
   * @brief Async クラス
   *
   * @else
   *
   * @class Async
   * @brief Async class
   *
   * @endif
   */
  class Async
    : public coil::Task
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
    Async() {}

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
    ~Async() override {}

    /*!
     * @if jp
     *
     * @brief 非同期実行用純粋仮想関数
     *
     * 非同期実行用純粋仮想関数。
     *
     * @else
     *
     * @brief Asynchronous invocation
     *
     * Pure virtual function for Asynchronous invocation.
     *
     * @endif
     */
    virtual void invoke() = 0;

    /*!
     * @if jp
     *
     * @brief 完了状態チェック用純粋仮想関数
     *
     * 完了状態チェック用純粋仮想関数。
     *
     * @return true: 完了, false: 未完了
     *
     * @else
     *
     * @brief Check on completion state
     *
     * Pure virtual function for check on completion state.
     *
     * @return true: finished, false: unfinished
     *
     * @endif
     */
    virtual bool finished() = 0;
  };

  /*!
   * @if jp
   *
   * @class Async_t
   * @brief Async_t テンプレートクラス
   *
   * @else
   *
   * @class Async_t
   * @brief Async_t template class
   *
   * @endif
   */
  template <typename Object, typename Func>
  class Async_t
    : public Async
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param obj 登録対象オブジェクト
     * @param func 非同期実行用関数
     * @param auto_delete 非同期実行終了時に自動的にインスタンス削除を行うかどうかのフラグ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param obj The target object for the asynchronous function.
     * @param func Asynchronous function.
     * @param auto_delete flag for automatic instance destruction.
     *
     * @endif
     */
    Async_t(Object* obj, Func func, bool auto_delete = false)
      : m_obj(obj), m_func(std::move(func)), m_finished(false), m_autodelete(auto_delete)
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
    ~Async_t() override
    {
    }

    /*!
     * @if jp
     *
     * @brief 非同期処理用のスレッド実行関数
     *
     * 登録されたオブジェクトの非同期処理を呼び出す。
     *
     * @return 実行結果
     *
     * @else
     *
     * @brief Thread execution function for asynchronous invoke.
     *
     * Invoke the registered objects operation.
     *
     * @return The execution result.
     *
     * @endif
     */
    int svc() override
    {
      m_func(m_obj);
      {
        coil::Guard<Mutex> guard(m_mutex);
        m_finished = true;
      }

      return 0;
    }

    /*!
     * @if jp
     *
     * @brief 非同期処理終了
     *
     * 非同期処理を終了し、インスタンスを削除する。
     *
     * @else
     *
     * @brief Finalize the asynchronous function
     *
     * Finalize the asynchronous function for preparing it for destruction.
     *
     * @endif
     */
    void finalize() override
    {
      Task::finalize();
      if (m_autodelete) delete this;
    }

    /*!
     * @if jp
     *
     * @brief 非同期処理活性化
     *
     * 非同期処理を活性化する。
     *
     * @else
     *
     * @brief Asynchronous function Activation
     *
     * Activate of Asynchronous function.
     *
     * @endif
     */
    void invoke() override
    {
      activate();
    }

    /*!
     * @if jp
     *
     * @brief 完了状態チェック
     *
     * 完了状態を返す。
     *
     * @return true: 完了, false: 未完了
     *
     * @else
     *
     * @brief Check on completion state
     *
     * Return a completion state.
     *
     * @return true: finished, false: unfinished
     *
     * @endif
     */
    bool finished() override
    {
      coil::Guard<Mutex> guard(m_mutex);
      return m_finished;
    }
  private:
    Object* m_obj;
    Func m_func;
    bool m_finished;
    const bool m_autodelete;
    Mutex m_mutex;
  };

  /*!
   * @if jp
   *
   * @class Async_ref_t
   * @brief Async_ref_t テンプレートクラス
   *
   * @else
   *
   * @class Async_ref_t
   * @brief Async_ref_t template class
   *
   * @endif
   */
  template <typename Object, typename Func>
  class Async_ref_t
    : public Async
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param obj 登録対象オブジェクト
     * @param func 非同期実行用関数
     * @param auto_delete 非同期実行終了時に自動的にインスタンス削除を行うかどうかのフラグ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param obj The target object for the asynchronous function.
     * @param func Asynchronous function.
     * @param auto_delete flag for automatic instance destruction.
     *
     * @endif
     */
    Async_ref_t(Object* obj, Func& func, bool auto_delete = false)
      : m_obj(obj), m_func(func), m_finished(false), m_autodelete(auto_delete)
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
    ~Async_ref_t() override
    {
    }

    /*!
     * @if jp
     *
     * @brief 非同期処理用のスレッド実行関数
     *
     * 登録されたオブジェクトの非同期処理を呼び出す。
     *
     * @return 実行結果
     *
     * @else
     *
     * @brief Thread execution function for asynchronous invoke.
     *
     * Invoke the registered objects operation.
     *
     * @return The execution result.
     *
     * @endif
     */
    int svc() override
    {
      m_func(m_obj);
      m_finished = true;
      return 0;
    }

    /*!
     * @if jp
     *
     * @brief 非同期処理活性化
     *
     * 非同期処理を活性化する。
     *
     * @else
     *
     * @brief Asynchronous function Activation
     *
     * Activate of Asynchronous function.
     *
     * @endif
     */
    void invoke() override
    {
      activate();
    }

    /*!
     * @if jp
     *
     * @brief 完了状態チェック
     *
     * 完了状態を返す。
     *
     * @return true: 完了, false: 未完了
     *
     * @else
     *
     * @brief Check on completion state
     *
     * Return a completion state.
     *
     * @return true: finished, false: unfinished
     *
     * @endif
     */
    bool finished() override
    {
      return m_finished;
    }

    /*!
     * @if jp
     *
     * @brief 非同期処理終了
     *
     * 非同期処理を終了し、インスタンスを削除する。
     *
     * @else
     *
     * @brief Finalize the asynchronous function
     *
     * Finalize the asynchronous function for preparing it for destruction.
     *
     * @endif
     */
    void finalize() override
    {
      Task::finalize();
      if (m_autodelete) delete this;
    }
  private:
    Object* m_obj;
    Func& m_func;
    bool m_finished;
    bool m_autodelete;

  };

  /*!
   * @if jp
   * @brief 非同期メンバー関数呼び出しヘルパー関数
   *
   * メンバー関数を非同期に呼ぶためのヘルパー関数
   * 例
   *
   *  class A
   *  {
   *  public:
   *    // 時間のかかる関数
   *    void hoge() {
   *      for (int i(0); i < 5; ++i) {
   *        std::cout << "hoge" << std::endl;
   *        sleep(1);
   *      }
   *    }
   *    // 時間のかかる関数
   *    void munya(const char* msg) {
   *      for (int i(0); i < 10; ++i) {
   *        std::cout << "message is: " << msg << std::endl;
   *        sleep(1);
   *      }
   *    }
   *    int add_one(int val) {
   *      return val + 1;
   *    }
   *  };
   * この様なクラスのオブジェクトに対して、
   *
   *  A a;
   *  Async* invoker0(AsyncInvoker(&a,
   *                               std::mem_fun(&A::hoge)));
   *  Async* invoker1(AsyncInvoker(&a,
   *                               std::bind2nd(std::mem_fun(&A::munya),
   *                                            "ほげ")));
   *  invoker0->invoke(); // すぐに戻る
   *  invoker1->invoke(); // すぐに戻る
   *
   *  delete invoker0; // 必ず削除すること
   *  delete invoker1; // 必ず削除すること
   *
   * のように非同期の呼び出しができる。
   * 呼び出しの戻り値を取得したい場合は、自前の関数オブジェクトを用意する。
   *
   *  class add_one_functor
   *  {
   *    int m_val, m_ret;
   *  public:
   *    add_one_functor(int val) : m_val(val), m_ret(0) {}
   *    void operaotr(A* obj) {
   *      m_ret = obj->add_one(m_val);
   *    }
   *    int get_ret() {
   *      return m_ret;
   *    }
   *  };
   *
   * 上記の関数オブジェクトのインスタンスを作成し、そのポインタを渡す。
   *
   *  add_one_functor aof(100);
   *  Async* invoker2(AsyncInvoker(&a, &aof));
   *  invoker2->invoke();
   *  invoker2->wait();
   *  std::cout << "result: " << aof.get_ret() << std::endl;
   *  delete invoker2;
   *
   * 通常、AsyncInvoker が返すオブジェクトは明示的に削除しなければ
   * ならないが、第三引数に true を渡すことで、非同期実行が終了すると同時に
   * 自動的にインスタンスが削除される。
   *
   * // invoker3 は削除 (delete invoker3) してはいけない
   * Async* invoker3(AsyncInvoker(&a, std::mem_fun(&A::hoge), true));
   *
   * // インスタンス生成と同時に実行することもできる。
   * AsyncInvoker(&a, std::mem_fun(&A::hoge))->invoke();
   *
   * @param obj 登録対象オブジェクト
   * @param func 非同期実行用関数
   * @param auto_delete 非同期実行終了時に自動的にインスタンス削除を行うかどうかのフラグ
   *
   * @return Async_t インスタンス
   *
   * @else
   *
   * @brief Helper function for async member function summons
   *
   * Helper function for async member function summons.
   *
   * @param obj The target object for the asynchronous function.
   * @param func Asynchronous function.
   * @param auto_delete flag for automatic instance destruction.
   *
   * @return Async_t Instance
   *
   * @endif
   */
  template <typename Object, typename Func>
  inline Async_t<Object, Func>*
  AsyncInvoker(Object* obj, Func func, bool auto_delete = false)
  {
    return new Async_t<Object, Func>(obj, func, auto_delete);
  }

  /*!
   * @if jp
   *
   * @brief 非同期メンバー関数呼び出しヘルパー関数
   *
   * メンバー関数を非同期に呼ぶためのヘルパー関数
   *
   * @param obj 登録対象オブジェクト
   * @param func 非同期実行用関数
   * @param auto_delete 非同期実行終了時に自動的にインスタンス削除を行うかどうかのフラグ
   *
   * @return Async_ref_t インスタンス
   *
   * @else
   *
   * @brief Helper function for async member function summons
   *
   * Helper function for async member function summons.
   *
   * @param obj The target object for the asynchronous function.
   * @param func Asynchronous function.
   * @param auto_delete flag for automatic instance destruction.
   *
   * @return Async_ref_t Instance
   *
   * @endif
   */
  template <typename Object, typename Func>
  inline Async_ref_t<Object, Func>*
  AsyncInvoker(Object* obj, Func* func, bool auto_delete = false)
  {
    return new Async_ref_t<Object, Func>(obj, *func, auto_delete);
  }


} // namespace coil

#endif  // COIL_ASYNC_H
