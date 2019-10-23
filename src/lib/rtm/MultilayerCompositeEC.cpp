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
    int count(0);

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
        
        
        auto t1 = std::chrono::high_resolution_clock::now();

        auto period = getPeriod();
        auto rest = period - (t1 - t0);
        if (count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", std::chrono::duration<double>(period).count()));
            RTC_PARANOID(("Execution: %f [s]", std::chrono::duration<double>(t1 - t0).count()));
            RTC_PARANOID(("Sleep:     %f [s]", std::chrono::duration<double>(rest).count()));
            int task_num = 0;
            for (auto & task : m_tasklist)
            {
                coil::TimeMeasure::Statistics st = task->getExecStat();
                RTC_PARANOID(("MAX(%d):  %f [s]", task_num, st.max_interval));
                RTC_PARANOID(("MIN(%d):  %f [s]", task_num, st.min_interval));
                RTC_PARANOID(("MEAN(%d): %f [s]", task_num, st.mean_interval));
                RTC_PARANOID(("SD(%d):   %f [s]", task_num, st.std_deviation));
                task_num += 1;
            }
          }
        auto t2 = std::chrono::high_resolution_clock::now();
        if (!m_nowait && (rest > std::chrono::seconds::zero()))
          {
            if (count > 1000) { RTC_PARANOID(("sleeping...")); }
            std::this_thread::sleep_until(t0 + period);
          }
        if (count > 1000)
          {
            auto t3 = std::chrono::high_resolution_clock::now();
            RTC_PARANOID(("Slept:     %f [s]", std::chrono::duration<double>(t3 - t2).count()));
            count = 0;
          }
        ++count;
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
                  rtcs.emplace_back(RTC::RTObject::_duplicate(rtobj));
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

      ChildTask *ct = new ChildTask(task, this);

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

  MultilayerCompositeEC::ChildTask::~ChildTask() = default;

  void MultilayerCompositeEC::ChildTask::addComponent(RTC::LightweightRTObject_ptr rtc)
  {
      m_rtcs.emplace_back(rtc);
  }

  void MultilayerCompositeEC::ChildTask::updateCompList()
  {
      std::vector<RTC::LightweightRTObject_ptr>::iterator rtc = m_rtcs.begin();
      while (rtc != m_rtcs.end())
      {
          RTC_impl::RTObjectStateMachine* comp = m_ec->findComponent(*rtc);
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

          while (!m_signal_worker.running_)
          {
              m_signal_worker.cond_.wait(guard);
          }
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
          while (!m_signal_worker.running_){
             m_signal_worker.cond_.wait(guard);
          }
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
      m_task->resume();
      m_task->finalize();

      delete m_task;
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
                            ::RTC_exp::MultilayerCompositeEC>);

    coil::vstring ecs;
    ecs = RTC::ExecutionContextFactory::instance().getIdentifiers();
  }
}

