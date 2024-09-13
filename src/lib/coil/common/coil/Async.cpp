// -*- C++ -*-
/*!
 * @file Async.cpp
 * @brief Asynchronous function invocation helper class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2021
 *     Software Platform Research Team,
 *     Industrial Cyber-Physical Systems Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#if defined (WIN32)
#pragma warning(push)
#pragma warning(disable:4996)
#elif defined (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <coil/Async.h>

namespace coil
{
  DeleteAsyncThread* DeleteAsyncThread::delasync = nullptr;
  std::mutex DeleteAsyncThread::mutex;

  DeleteAsyncThread::DeleteAsyncThread()
  {
      m_task.setTask([this] { svc(); });
      m_task.setPeriod(std::chrono::seconds(1));
  }

  DeleteAsyncThread::~DeleteAsyncThread()
  {

  }

  void DeleteAsyncThread::activate()
  {
      m_task.suspend();
      m_task.activate();
      m_task.suspend();
  }

  int DeleteAsyncThread::svc()
  {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::lock_guard<std::mutex> guard(m_mutex);

      std::vector<Async*>::iterator thread = m_threads.begin();
      while (thread != m_threads.end())
      {
          (*thread)->exit();
          thread = m_threads.erase(thread);
      }

      return 0;
  }

  void DeleteAsyncThread::add(Async* thread)
  {
      std::lock_guard<std::mutex> guard(m_mutex);
      m_threads.push_back(thread);
      m_task.signal();
  }

  DeleteAsyncThread* DeleteAsyncThread::instance()
  {
      std::lock_guard<std::mutex> guard(mutex);
      if (delasync == nullptr)
      {
          delasync = new DeleteAsyncThread();
          delasync->activate();
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      return delasync;
  }
}

#if defined (WIN32)
#pragma warning(pop)
#elif defined (__GNUC__)
#pragma GCC diagnostic pop
#endif
