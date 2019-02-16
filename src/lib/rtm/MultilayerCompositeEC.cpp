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

#include <coil/Time.h>
#include <coil/TimeValue.h>


#include <rtm/Manager.h>
#include <rtm/RTObject.h>
#include <rtm/MultilayerCompositeEC.h>
#include <rtm/RTObjectStateMachine.h>
#include <rtm/PeriodicTaskFactory.h>

#ifdef RTM_OS_LINUX
#define _GNU_SOURCE
#include <pthread.h>
#include <algorithm>
#endif // RTM_OS_LINUX

#include <string.h>
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
  : PeriodicExecutionContext(), m_ownersm(NULL)
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
  int MultilayerCompositeEC::svc(void)
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
          Guard guard(m_workerthread.mutex_);
          while (!m_workerthread.running_)
            {
              m_workerthread.cond_.wait();
            }
        }
        coil::TimeValue t0(coil::clock());
        m_ownersm->workerDo();
        m_ownersm->workerPostDo();
        
        
        for (std::vector<ChildTask*>::iterator task = m_tasklist.begin(); task != m_tasklist.end(); ++task)
        {
            (*task)->signal();
        }
        
        for (std::vector<ChildTask*>::iterator task = m_tasklist.begin(); task != m_tasklist.end(); ++task)
        {
            (*task)->join();
        }
        
        
        coil::TimeValue t1(coil::clock());

        coil::TimeValue period(getPeriod());
        if (count > 1000)
          {
            RTC_PARANOID(("Period:    %f [s]", static_cast<double>(period)));
            RTC_PARANOID(("Execution: %f [s]", static_cast<double>(t1 - t0)));
            RTC_PARANOID(("Sleep:     %f [s]",
                                    static_cast<double>(period - (t1 - t0))));
            int task_num = 0;
            for (std::vector<ChildTask*>::iterator task = m_tasklist.begin(); task != m_tasklist.end(); ++task)
            {
                coil::TimeMeasure::Statistics st = (*task)->getExecStat();
                RTC_PARANOID(("MAX(%d):  %f [s]", task_num, st.max_interval));
                RTC_PARANOID(("MIN(%d):  %f [s]", task_num, st.min_interval));
                RTC_PARANOID(("MEAN(%d): %f [s]", task_num, st.mean_interval));
                RTC_PARANOID(("SD(%d):   %f [s]", task_num, st.std_deviation));
                task_num += 1;
            }
          }
        coil::TimeValue t2(coil::clock());
        if (!m_nowait && period > (t1 - t0))
          {
            if (count > 1000) { RTC_PARANOID(("sleeping...")); }
            coil::sleep((coil::TimeValue)(period - (t1 - t0)));
          }
        if (count > 1000)
          {
            coil::TimeValue t3(coil::clock());
            RTC_PARANOID(("Slept:     %f [s]", static_cast<double>(t3 - t2)));
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
      coil::vstring threads = coil::split(threads_str, "|");

      for (coil::vstring::iterator thread = threads.begin(); thread != threads.end(); ++thread)
      {
          std::vector<RTC::LightweightRTObject_ptr> rtcs;
          coil::vstring members = coil::split(*thread, ",");

          for (coil::vstring::iterator member = members.begin(); member != members.end(); ++member)
          {
              std::string m = *member;
              coil::eraseBothEndsBlank(m);

              if (m.empty())
              {
                  continue;
              }
              else
              {
                  
                  RTC::RTObject_impl* comp = mgr.getComponent(m.c_str());
                  if (comp == NULL)
                  {
                      RTC_ERROR(("no RTC found: %s", member));
                      continue;
                  }
                  RTC::RTObject_ptr rtobj = comp->getObjRef();
                  if (CORBA::is_nil(rtobj))
                  {
                      continue;
                  }
                  rtcs.push_back(rtobj);
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
      if (task == NULL)
      {
          RTC_ERROR(("Task creation failed: %s",
              prop.getProperty("thread_type", "default").c_str()));
          return;
      }

      ChildTask *ct = new ChildTask(task, this);

      task->setTask(ct, &ChildTask::svc);
      task->setPeriod(0.0);
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


      for (std::vector<RTC::LightweightRTObject_ptr>::iterator rtc = rtcs.begin(); rtc != rtcs.end(); ++rtc)
      {
          addRTCToTask(ct, (*rtc));
      }

      m_tasklist.push_back(ct);

      // Start task in suspended mode
      task->suspend();
      task->activate();
      task->suspend();

  }

  void MultilayerCompositeEC::addRTCToTask(ChildTask* task, RTC::LightweightRTObject_ptr rtobj)
  {
      ::OpenRTM::DataFlowComponent_ptr comp = ::OpenRTM::DataFlowComponent::_narrow(rtobj);
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

  void MultilayerCompositeEC::ChildTask::addComponent(RTC::LightweightRTObject_ptr rtc)
  {
      m_rtcs.push_back(rtc);
  }

  void MultilayerCompositeEC::ChildTask::updateCompList()
  {
      std::vector<RTC::LightweightRTObject_ptr>::iterator rtc = m_rtcs.begin();
      while (rtc != m_rtcs.end())
      {
          RTC_impl::RTObjectStateMachine* comp = m_ec->findComponent(*rtc);
          if (comp != NULL)
          {
              rtc = m_rtcs.erase(rtc);
              m_comps.push_back(comp);
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
          Guard guard(m_worker.mutex_);
          m_worker.running_ = true;
      }

      {
          Guard guard(m_signal_worker.mutex_);

          while (!m_signal_worker.running_)
          {
              m_signal_worker.cond_.wait();
          }
          m_signal_worker.running_ = false;
          
      }
      

      
      
      updateCompList();
      for (std::vector<RTC_impl::RTObjectStateMachine*>::iterator comp = m_comps.begin(); comp != m_comps.end(); ++comp)
      {
          (*comp)->workerPreDo();
          (*comp)->workerDo();
          (*comp)->workerPostDo();
      }
      
      {
          Guard guard(m_worker.mutex_);
          m_worker.running_ = false;
          
          m_worker.cond_.signal();
          

      }

      {
          Guard guard(m_signal_worker.mutex_);
          while (!m_signal_worker.running_){
             m_signal_worker.cond_.wait();
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
              Guard guard(m_worker.mutex_);
              ret = m_worker.running_;
          }
      }
      {
          Guard guard(m_signal_worker.mutex_);
          m_signal_worker.running_ = true;
          m_signal_worker.cond_.signal();
      }
      

  }

  void MultilayerCompositeEC::ChildTask::join()
  {
      {
          Guard guard(m_worker.mutex_);
          while (m_worker.running_)
          {
              m_worker.cond_.wait();
          }
      }

      {
          Guard guard(m_signal_worker.mutex_);
          m_signal_worker.running_ = true;
          m_signal_worker.cond_.signal();
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

      RTC::PeriodicTaskFactory::instance().deleteObject(m_task);
  }

};  // namespace RTC_exp

extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */

  void MultilayerCompositeECInit(RTC::Manager* manager)
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
};

