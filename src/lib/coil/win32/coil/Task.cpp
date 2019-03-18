// -*- C++ -*-
/*!
 * @file Task_win32.cpp
 * @brief Task class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/Task.h>

namespace coil
{

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  Task::Task()
    : m_count(0)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  Task::~Task()
  {
    m_count = 0;
  }

  /*!
   * @if jp
   * @brief タスクオープン
   * @else
   * @brief Task open
   * @endif
   */
  int Task::open(void* /*args*/)
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief タスククローズ
   * @else
   * @brief Task close
   * @endif
   */
  int Task::close(unsigned long /*flags*/)
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief スレッドを実行する
   * @else
   * @brief Execute thread
   * @endif
   */
  int Task::svc()
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief スレッドを生成する
   * @else
   * @brief Create a thread
   * @endif
   */
  void Task::activate()
  {
    if (m_count == 0)
      {
        m_thread =
           (HANDLE)::_beginthreadex(NULL,  // security
           0,  // stuck size
           Task::svc_run,  // func
           reinterpret_cast<void*>(this),  // argument
           0,  // flag (0 or CREATE_SUSPEND)
           NULL);  // thread descripter
        ++m_count;
      };
  }

  /*!
   * @if jp
   * @brief スレッド終了を待つ
   * @else
   * @brief Waiting for the thread terminate
   * @endif
   */
  int Task::wait(void)
  {
    if (m_count > 0)
      {
        DWORD retval = ::WaitForSingleObject(m_thread, INFINITE);
        if (retval == WAIT_OBJECT_0)
          {
            return 0;
          }
        else
          {
            return -1;
          }
      }
    return 0;
  }

  /*!
   * @if jp
   * @brief タスク実行を中断する
   * @else
   * @brief Suspending the task
   * @endif
   */
  int Task::suspend(void)
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief 中断されているタスクを再開する
   * @else
   * @brief Resuming the suspended task
   * @endif
   */
  int Task::resume(void)
  {
    return 0;
  }

  /*!
   * @if jp
   * @brief タスク数リセット
   * @else
   * @brief Reset of task count
   * @endif
   */
  void Task::reset()
  {
    m_count = 0;
  }

  /*!
   * @if jp
   * @brief タスク実行を終了する
   * @else
   * @brief Finalizing the task
   * @endif
   */
  void Task::finalize()
  {
    reset();
  }

  /*!
   * @if jp
   * @brief スレッド実行を開始する
   * @else
   * @brief Start thread Execution
   * @endif
   */
  unsigned int WINAPI Task::svc_run(void* args)
  {
    Task* t = (coil::Task*)args;
    t->svc();
    t->finalize();
    return 0;
  }
} // namespace coil


