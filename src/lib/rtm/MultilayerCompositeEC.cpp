// -*- C++ -*-
/*!
 * @file MultilayerCompositeEC.cpp
 * @brief 
 * @date $Date: 2018-10-03 15:44:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#include <rtm/Manager.h>
#include <rtm/RTObject.h>
#include <rtm/MultilayerCompositeEC.h>
#include <rtm/RTObjectStateMachine.h>
#include <rtm/PeriodicTaskFactory.h>

#include <cstring>
#include <algorithm>
#include <iostream>
#include <string>

#define DEEFAULT_PERIOD 0.000001
namespace RTC_exp
{
  /*!
   * @if jp
   * @brief デフォルトコンストラクタ
   * @else
   * @brief Default constructor
   * @endif
   */
  MultilayerCompositeEC::
  MultilayerCompositeEC()
  : PeriodicExecutionContext()
  {
    RTC_TRACE(("MultilayerCompositeEC()"));
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  MultilayerCompositeEC::~MultilayerCompositeEC()
  {
    RTC_TRACE(("~MultilayerCompositeEC()"));
    {
      std::lock_guard<std::mutex> guard(m_svcmutex);
      m_svc = false;
    }
    {
      std::lock_guard<std::mutex> guard(m_workerthread.mutex_);
      m_workerthread.running_ = true;
      m_workerthread.cond_.notify_all();
    }
    wait();
    for (auto* task : m_tasklist) { task->finalize(); delete task; }
    m_tasklist.clear();
  }

  void MultilayerCompositeEC::init(coil::Properties& props)
  {
    PeriodicExecutionContext::init(props);

  }


  /*------------------------------------------------------------
   * Run by a daemon thread to handle deferred processing
   * ACE_Task class method over ride.
   *------------------------------------------------------------*/
  /*!
   * @if jp
   * @brief ExecutionContext 用のスレッド実行関数
   * @else
   * @brief Thread execution function for ExecutionContext
   * @endif
   */
  int MultilayerCompositeEC::svc()
  {
    RTC_TRACE(("svc()"));

    const RTC::RTObject_ptr owner = getOwner();
    m_ownersm = m_worker.findComponent(owner);

    do
      {
        m_ownersm->workerPreDo();
        // Thread will stopped when all RTCs are INACTIVE.
        // Therefore WorkerPreDo(updating state) have to be invoked
        // before stopping thread.
        {
          std::unique_lock<std::mutex> guard(m_workerthread.mutex_);
          while (!m_workerthread.running_)
            {
              m_workerthread.cond_.wait(guard);
            }
        }
        auto t0 = std::chrono::high_resolution_clock::now();
        m_ownersm->workerDo();
        m_ownersm->workerPostDo();

        for (auto & task : m_tasklist)
        {
            task->signal();
        }

        for (auto & task : m_tasklist)
        {
            task->join();
        }

        if (!m_nowait)
        {
            auto t1 = std::chrono::high_resolution_clock::now();
            auto exectime = t1 - t0;
            if (exectime.count() >= 0)
              {
                auto diff = getPeriod() - exectime;
                if (diff.count() > 0)
                  {
#ifdef _WIN32
                    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
                    Sleep(static_cast<DWORD>(ms.count()));
#else
                    std::this_thread::sleep_for(diff);

#endif
                  }
              }
        }
      } while (threadRunning());
    RTC_DEBUG(("Thread terminated."));
    return 0;
  }


  RTC::ReturnCode_t MultilayerCompositeEC::bindComponent(RTC::RTObject_impl* rtc)
  {
      RTC::ReturnCode_t ret = ExecutionContextBase::bindComponent(rtc);
      ::RTC::Manager &mgr = ::RTC::Manager::instance();
      std::string threads_str = rtc->getProperties()["conf.default.members"];
      for (auto const & thread : coil::split(threads_str, "|"))
      {
          std::vector<RTC::LightweightRTObject_var> owned;
          std::vector<RTC::LightweightRTObject_ptr> rtcs;
          for (auto&& member : coil::split(thread, ","))
          {
              member =  coil::eraseBothEndsBlank(std::move(member));
              if (member.empty())
              {
                  continue;
              }
              else
              {
                  RTC::RTObject_impl* comp = mgr.getComponent(member.c_str());
                  if (comp == nullptr)
                  {
                      RTC_ERROR(("no RTC found: %s", member.c_str()));
                      continue;
                  }
                  RTC::RTObject_var rtobj = comp->getObjRef();
                  if (CORBA::is_nil(rtobj))
                  {
                      continue;
                  }
                  owned.emplace_back(RTC::LightweightRTObject::_duplicate(rtobj.in()));
                  rtcs.emplace_back(owned.back().in());
              }
          }
          addTask(rtcs);
      }
      return ret;

  }

  RTC_impl::RTObjectStateMachine* MultilayerCompositeEC::findComponent(RTC::LightweightRTObject_ptr comp)
  {
      return m_worker.findComponent(comp);
  }

  void MultilayerCompositeEC::addTask(std::vector<RTC::LightweightRTObject_ptr> rtcs)
  {
      std::string param = "ec";
      param += coil::otos(m_tasklist.size());
      coil::Properties tmp(m_profile.getProperties());
      
      coil::Properties prop = tmp.getNode(param);

      RTC::PeriodicTaskFactory& factory(RTC::PeriodicTaskFactory::instance());

      coil::PeriodicTaskBase* task = factory.createObject(prop.getProperty("thread_type", "default"));
      if (task == nullptr)
      {
          RTC_ERROR(("Task creation failed: %s",
              prop.getProperty("thread_type", "default").c_str()));
          return;
      }

      std::unique_ptr<ChildTask> ownedTask(new ChildTask(task, this));
      ChildTask* ct = ownedTask.get();

      task->setTask([ct]{ ct->svc(); });
      task->setPeriod(std::chrono::seconds(0));
      task->executionMeasure(coil::toBool(prop["measurement.exec_time"],
          "enable", "disable", true));


      int ecount(1000);
      if (coil::stringTo(ecount, prop["measurement.exec_count"].c_str()))
      {
          task->executionMeasureCount(ecount);
      }

      task->periodicMeasure(coil::toBool(prop["measurement.period_time"],
          "enable", "disable", true));
      int pcount(1000);
      if (coil::stringTo(pcount, prop["measurement.period_count"].c_str()))
      {
          task->periodicMeasureCount(pcount);
      }


      for (auto & rtc : rtcs)
      {
          addRTCToTask(ct, rtc);
      }

      m_tasklist.emplace_back(ct);
      ownedTask.release();

      // Start task in suspended mode
      task->suspend();
      task->activate();
      task->suspend();

  }

  void MultilayerCompositeEC::addRTCToTask(ChildTask* task, RTC::LightweightRTObject_ptr rtobj)
  {
      ::OpenRTM::DataFlowComponent_var comp = ::OpenRTM::DataFlowComponent::_narrow(rtobj);
      SDOPackage::OrganizationList_var orglist = comp->get_owned_organizations();

      if (orglist->length() == 0)
      {
          task->addComponent(rtobj);
      }
      for (CORBA::ULong i(0); i < orglist->length(); ++i)
      {
          SDOPackage::SDOList_var sdos = orglist[i]->get_members();
          for (CORBA::ULong j(0); j < sdos->length(); ++j)
          {
              ::OpenRTM::DataFlowComponent_var dfc = ::OpenRTM::DataFlowComponent::_narrow(sdos[j].in());
              addRTCToTask(task, dfc);
          }
      }

  }

  MultilayerCompositeEC::ChildTask::ChildTask(coil::PeriodicTaskBase* task, MultilayerCompositeEC* ec) :
      m_task(task), m_ec(ec)
  {

  }

  MultilayerCompositeEC::ChildTask::~ChildTask()
  {
      finalize();
  }

  void MultilayerCompositeEC::ChildTask::addComponent(RTC::LightweightRTObject_ptr rtc)
  {
      m_rtcs.emplace_back(RTC::LightweightRTObject::_duplicate(rtc));
  }

  void MultilayerCompositeEC::ChildTask::updateCompList()
  {
      auto rtc = m_rtcs.begin();
      while (rtc != m_rtcs.end())
      {
          RTC_impl::RTObjectStateMachine* comp = m_ec->findComponent(rtc->in());
          if (comp != nullptr)
          {
              rtc = m_rtcs.erase(rtc);
              m_comps.emplace_back(comp);
          }
          else
          {
              ++rtc;
          }
      }
  }

  int MultilayerCompositeEC::ChildTask::svc()
  {
      {
          std::lock_guard<std::mutex> guard(m_worker.mutex_);
          m_worker.running_ = true;
      }

      {
          std::unique_lock<std::mutex> guard(m_signal_worker.mutex_);

          while (!m_signal_worker.running_ && !m_stopping)
          {
              m_signal_worker.cond_.wait(guard);
          }
          if (m_stopping) { return 0; }
          m_signal_worker.running_ = false;
      }

      updateCompList();
      for (auto & comp : m_comps)
      {
          comp->workerPreDo();
          comp->workerDo();
          comp->workerPostDo();
      }

      {
          std::lock_guard<std::mutex> guard(m_worker.mutex_);
          m_worker.running_ = false;

          m_worker.cond_.notify_one();
      }

      {
          std::unique_lock<std::mutex> guard(m_signal_worker.mutex_);
          while (!m_signal_worker.running_ && !m_stopping){
             m_signal_worker.cond_.wait(guard);
          }
          if (m_stopping) { return 0; }
          m_signal_worker.running_ = false;
      }

      return 0;
  }

  void MultilayerCompositeEC::ChildTask::signal()
  {
      bool ret = false;
      while (!ret)
      {
          m_task->signal();
          {
              std::lock_guard<std::mutex> guard(m_worker.mutex_);
              ret = m_worker.running_;
          }
      }
      {
          std::lock_guard<std::mutex> guard(m_signal_worker.mutex_);
          m_signal_worker.running_ = true;
          m_signal_worker.cond_.notify_one();
      }
  }

  void MultilayerCompositeEC::ChildTask::join()
  {
      {
          std::unique_lock<std::mutex> guard(m_worker.mutex_);
          while (m_worker.running_)
          {
              m_worker.cond_.wait(guard);
          }
      }

      {
          std::lock_guard<std::mutex> guard(m_signal_worker.mutex_);
          m_signal_worker.running_ = true;
          m_signal_worker.cond_.notify_one();
      }
  }

  coil::TimeMeasure::Statistics MultilayerCompositeEC::ChildTask::getPeriodStat()
  {
      return m_task->getPeriodStat();
  }

  coil::TimeMeasure::Statistics MultilayerCompositeEC::ChildTask::getExecStat()
  {
      return m_task->getExecStat();
  }

  void MultilayerCompositeEC::ChildTask::finalize()
  {
      if (m_task == nullptr) { return; }
      {
        std::lock_guard<std::mutex> guard(m_signal_worker.mutex_);
        m_stopping = true;
        m_signal_worker.cond_.notify_all();
      }
      m_task->resume();
      m_task->finalize();

      RTC::PeriodicTaskFactory::instance().deleteObject(m_task);
      m_task = nullptr;
  }

} // namespace RTC_exp

extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */

  void MultilayerCompositeECInit(RTC::Manager*  /*manager*/)
  {
    RTC::ExecutionContextFactory::
      instance().addFactory("MultilayerCompositeEC",
                            ::coil::Creator< ::RTC::ExecutionContextBase,
                            ::RTC_exp::MultilayerCompositeEC>,
                            ::coil::Destructor< ::RTC::ExecutionContextBase,
                            ::RTC_exp::MultilayerCompositeEC>);

    coil::vstring ecs;
    ecs = RTC::ExecutionContextFactory::instance().getIdentifiers();
  }
}

