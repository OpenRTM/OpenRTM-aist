// -*- C++ -*-
/*!
 * @file  PublisherPeriodic.cpp
 * @brief PublisherPeriodic class
 * @date  $Date: 2007-12-31 03:08:06 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
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
#include <rtm/RTC.h>
#include <coil/Properties.h>
#include <coil/Time.h>
#include <coil/stringutil.h>
#include <rtm/PublisherPeriodic.h>
#include <rtm/InPortConsumer.h>
#include <rtm/idl/DataPortSkel.h>
#include <rtm/PeriodicTaskFactory.h>
#include <rtm/SystemLogger.h>

#include <cstdlib>
#include <string>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  PublisherPeriodic::PublisherPeriodic()
    : rtclog("PublisherPeriodic"),
      m_consumer(nullptr), m_buffer(nullptr), m_task(nullptr), m_listeners(nullptr),
      m_retcode(PORT_OK), m_pushPolicy(PUBLISHER_POLICY_NEW),
      m_skipn(0), m_active(false), m_readback(false), m_leftskip(0)
  {
  }


  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  PublisherPeriodic::~PublisherPeriodic()
  {
    RTC_TRACE(("~PublisherPeriodic()"));
    if (m_task != nullptr)
      {
        m_task->resume();
        m_task->finalize();
        RTC_PARANOID(("task finalized."));

        RTC::PeriodicTaskFactory::instance().deleteObject(m_task);
        RTC_PARANOID(("task deleted."));
      }

    // "consumer" should be deleted in the Connector
    m_consumer = nullptr;
    // "buffer"   should be deleted in the Connector
    m_buffer = nullptr;
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief initialization
   * @endif
   */
  PublisherBase::ReturnCode PublisherPeriodic::init(coil::Properties& prop)
  {
    RTC_TRACE(("init()"));
    RTC_DEBUG_STR((prop));

    setPushPolicy(prop);
    if (!createTask(prop))
      {
        return INVALID_ARGS;
      }
    return PORT_OK;





    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief InPortコンシューマのセット
   * @else
   * @brief Store InPort consumer
   * @endif
   */
  PublisherBase::ReturnCode
  PublisherPeriodic::setConsumer(InPortConsumer* consumer)
  {
    RTC_TRACE(("setConsumer()"));

    if (consumer == nullptr)
      {
        RTC_ERROR(("setConsumer(consumer = 0): invalid argument."));
        return INVALID_ARGS;
      }
    m_consumer = consumer;
    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief バッファのセット
   * @else
   * @brief Setting buffer pointer
   * @endif
   */
  PublisherBase::ReturnCode PublisherPeriodic::setBuffer(CdrBufferBase* buffer)
  {
    RTC_TRACE(("setBuffer()"));

    if (buffer == nullptr)
      {
        RTC_ERROR(("setBuffer(buffer == 0): invalid argument"));
        return INVALID_ARGS;
      }
    m_buffer = buffer;
    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief リスナのセット
   * @else
   * @brief Setting buffer pointer
   * @endif
   */
  PublisherBase::ReturnCode
  PublisherPeriodic::setListener(ConnectorInfo& info,
                                 ConnectorListeners* listeners)
  {
    RTC_TRACE(("setListeners()"));

    if (listeners == nullptr)
      {
        RTC_ERROR(("setListeners(listeners == 0): invalid argument"));
        return INVALID_ARGS;
      }
    m_profile = info;
    m_listeners = listeners;
    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief データを書き込む
   * @else
   * @brief Write data
   * @endif
   */
  PublisherBase::ReturnCode
  PublisherPeriodic::write(ByteDataStreamBase* data,
                           unsigned long sec,
                           unsigned long usec)
  {
    RTC_PARANOID(("write()"));

    if (m_consumer == nullptr) { return PRECONDITION_NOT_MET; }
    if (m_buffer == nullptr) { return PRECONDITION_NOT_MET; }
    if (m_listeners == nullptr) { return PRECONDITION_NOT_MET; }

    if (m_retcode == CONNECTION_LOST)
      {
        RTC_DEBUG(("write(): connection lost."));
        return m_retcode;
      }

    ByteData data_ = *data;

    if (m_retcode == SEND_FULL)
      {
        RTC_DEBUG(("write(): InPort buffer is full."));
        m_buffer->write(data_, sec, usec);
        return BUFFER_FULL;
      }

    onBufferWrite(data_);
    CdrBufferBase::ReturnCode ret(m_buffer->write(data_, sec, usec));
    RTC_DEBUG(("%s = write()", CdrBufferBase::toString(ret)));
    m_task->resume();
    return convertReturn(ret, data_);
  }

  /*!
   * @if jp
   * @brief アクティブ化確認
   * @else
   * @brief Confirm to activate
   * @endif
   */
  bool PublisherPeriodic::isActive()
  {
    return m_active;
  }

  /*!
   * @if jp
   * @brief アクティブ化
   * @else
   * @brief activation
   * @endif
   */
  PublisherBase::ReturnCode PublisherPeriodic::activate()
  {
    if (m_task == nullptr) { return PRECONDITION_NOT_MET; }
    if (m_buffer == nullptr) { return PRECONDITION_NOT_MET; }
    m_active = true;
    m_task->resume();
    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief 非アクティブ化
   * @else
   * @brief deactivation
   * @endif
   */
  PublisherBase::ReturnCode PublisherPeriodic::deactivate()
  {
    if (m_task == nullptr) { return PRECONDITION_NOT_MET; }
    m_active = false;
    m_task->suspend();
    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief スレッド実行関数
   * @else
   * @brief Thread execution function
   * @endif
   */
  int PublisherPeriodic::svc()
  {
    Guard guard(m_retmutex);
    switch (m_pushPolicy)
      {
      case PUBLISHER_POLICY_ALL:
        m_retcode = pushAll();
        break;
      case PUBLISHER_POLICY_FIFO:
        m_retcode = pushFifo();
        break;
      case PUBLISHER_POLICY_SKIP:
        m_retcode = pushSkip();
        break;
      case PUBLISHER_POLICY_NEW:
        m_retcode = pushNew();
        break;
      default:
        m_retcode = pushNew();
        break;
      }
    return 0;
  }

  /*!
   * @brief push all policy
   */
  PublisherBase::ReturnCode PublisherPeriodic::pushAll()
  {
    RTC_TRACE(("pushAll()"));
    if (bufferIsEmpty()) { return BUFFER_EMPTY; }

    while (m_buffer->readable() > 0)
      {
        ByteData& cdr(m_buffer->get());
        onBufferRead(cdr);

        onSend(cdr);
        ReturnCode ret(m_consumer->put(cdr));
        if (ret != PORT_OK)
          {
            RTC_DEBUG(("%s = consumer.put()", DataPortStatus::toString(ret)));
            return invokeListener(ret, cdr);
          }
        onReceived(cdr);

        m_buffer->advanceRptr();

      }
    return PORT_OK;
  }

  /*!
   * @brief push "fifo" policy
   */
  PublisherBase::ReturnCode PublisherPeriodic::pushFifo()
  {
    RTC_TRACE(("pushFifo()"));
    if (bufferIsEmpty()) { return BUFFER_EMPTY; }

    ByteData& cdr(m_buffer->get());
    onBufferRead(cdr);

    onSend(cdr);
    ReturnCode ret(m_consumer->put(cdr));

    if (ret != PORT_OK)
      {
        RTC_DEBUG(("%s = consumer.put()", DataPortStatus::toString(ret)));
        return invokeListener(ret, cdr);
      }
    onReceived(cdr);

    m_buffer->advanceRptr();

    return PORT_OK;
  }

  /*!
   * @brief push "skip" policy
   */
  PublisherBase::ReturnCode PublisherPeriodic::pushSkip()
  {
    RTC_TRACE(("pushSkip()"));
    if (bufferIsEmpty()) { return BUFFER_EMPTY; }

    ReturnCode ret(PORT_OK);
    size_t readable = m_buffer->readable();
    size_t preskip(readable + m_leftskip);
    assert(m_skipn >= 0);
    size_t loopcnt(preskip / (m_skipn + 1));
    assert(m_skipn >= m_leftskip);
    size_t postskip(m_skipn - m_leftskip);
    for (size_t i(0); i < loopcnt; ++i)
      {
        m_buffer->advanceRptr(static_cast<long>(postskip));
        readable -= postskip;
        ByteData& cdr(m_buffer->get());
        onBufferRead(cdr);

        onSend(cdr);
        ret = m_consumer->put(cdr);
        if (ret != PORT_OK)
          {
            m_buffer->advanceRptr(-static_cast<long>(postskip));
            RTC_DEBUG(("%s = consumer.put()", DataPortStatus::toString(ret)));
            return invokeListener(ret, cdr);
          }
        onReceived(cdr);
        postskip = m_skipn + 1;
      }

    m_buffer->advanceRptr(static_cast<long>(readable));
    assert(m_skipn >= 0);
    m_leftskip = preskip % (m_skipn +1);

    return ret;
  }

   /*!
    * @brief push "new" policy
    */
  PublisherBase::ReturnCode PublisherPeriodic::pushNew()
  {
    RTC_TRACE(("pushNew()"));
    if (bufferIsEmpty()) { return BUFFER_EMPTY; }

    // In case of the periodic/push_new policy, the buffer should
    // allow readback. But, readback flag should be set as "true"
    // after written at least one datum into the buffer.
    m_readback = true;
    m_buffer->advanceRptr(static_cast<long>(m_buffer->readable()) - 1);

    ByteData& cdr(m_buffer->get());

    onBufferRead(cdr);

    onSend(cdr);
    ReturnCode ret(m_consumer->put(cdr));
    if (ret != PORT_OK)
      {
        RTC_DEBUG(("%s = consumer.put()", DataPortStatus::toString(ret)));
        return invokeListener(ret, cdr);
      }
    onReceived(cdr);

    m_buffer->advanceRptr();

    return PORT_OK;
  }

  /*!
   * @if jp
   * @brief PushPolicy の設定
   * @else
   * @brief Setting PushPolicy
   * @endif
   */
  void PublisherPeriodic::setPushPolicy(const coil::Properties& prop)
  {
    // push_policy default: NEW
    std::string push_policy = prop.getProperty("publisher.push_policy", "new");
    RTC_DEBUG(("push_policy: %s", push_policy.c_str()));

    coil::normalize(push_policy);
    if      (push_policy == "all")  { m_pushPolicy = PUBLISHER_POLICY_ALL;  }
    else if (push_policy == "fifo") { m_pushPolicy = PUBLISHER_POLICY_FIFO; }
    else if (push_policy == "skip") { m_pushPolicy = PUBLISHER_POLICY_SKIP; }
    else if (push_policy == "new")  { m_pushPolicy = PUBLISHER_POLICY_NEW;  }
    else
      {
        RTC_ERROR(("invalid push_policy value: %s", push_policy.c_str()));
        m_pushPolicy = PUBLISHER_POLICY_NEW;     // default push policy
      }

    // skip_count default: 0
    std::string skip_count = prop.getProperty("publisher.skip_count", "0");
    RTC_DEBUG(("skip_count: %s", skip_count.c_str()));

    if (!coil::stringTo(m_skipn, skip_count.c_str()))
      {
        RTC_ERROR(("invalid skip_count value: %s", skip_count.c_str()));
        m_skipn = 0;           // desfault skip count
      }
    if (m_skipn < 0)
      {
        RTC_ERROR(("invalid skip_count value: %d", m_skipn));
        m_skipn = 0;           // default skip count
      }
  }

  /*!
   * @if jp
   * @brief Task の設定
   * @else
   * @brief Setting Task
   * @endif
   */
  bool PublisherPeriodic::createTask(const coil::Properties& prop)
  {
    RTC::PeriodicTaskFactory& factory(RTC::PeriodicTaskFactory::instance());

    // Creating and setting task object
    coil::vstring th = factory.getIdentifiers();
    RTC_DEBUG(("available task types: %s", coil::flatten(th).c_str()));

    m_task = factory.createObject(prop.getProperty("thread_type", "default"));
    if (m_task == nullptr)
      {
        RTC_ERROR(("Task creation failed: %s",
                   prop.getProperty("thread_type", "default").c_str()));
        return false;
      }
    m_task->setTask(this, &PublisherPeriodic::svc);
    RTC_PARANOID(("Task creation succeeded."));

    // Extracting publisher's period time
    double hz(100);
    if (!coil::stringTo(hz, prop["publisher.push_rate"].c_str()) &&
        // for 0.4 compatibility
        !coil::stringTo(hz, prop["push_rate"].c_str()))
      {
        RTC_ERROR(("publisher.push_rate/push_rate were not found."));
        return false;
      }

    if (hz <= 0)
      {
        RTC_ERROR(("invalid period: %f [s]", hz));
        return false;
      }
    m_task->setPeriod(1.0/hz);

    // Setting task measurement function
    m_task->executionMeasure(coil::toBool(prop["measurement.exec_time"],
                                          "enable", "disable", true));

    int ecount(1000);
    if (coil::stringTo(ecount, prop["measurement.exec_count"].c_str()))
      {
        m_task->executionMeasureCount(ecount);
      }

    m_task->periodicMeasure(coil::toBool(prop["measurement.period_time"],
                                   "enable", "disable", true));
    int pcount(1000);
    if (coil::stringTo(pcount, prop["measurement.period_count"].c_str()))
      {
        m_task->periodicMeasureCount(pcount);
      }

    // Start task in suspended mode
    m_task->suspend();
    m_task->activate();
    m_task->suspend();

    return true;
  }

  /*!
   * @if jp
   * @brief BufferStatus から DataPortStatus への変換
   * @else
   * @brief Convertion from BufferStatus to DataPortStatus
   * @endif
   */
  PublisherBase::ReturnCode
  PublisherPeriodic::convertReturn(BufferStatus::Enum status,
                                   ByteData& data)
  {
    /*
     * BufferStatus -> DataPortStatus
     *
     * BUFFER_OK     -> PORT_OK
     * BUFFER_ERROR  -> BUFFER_ERROR
     * BUFFER_FULL   -> BUFFER_FULL
     * NOT_SUPPORTED -> PORT_ERROR
     * TIMEOUT       -> BUFFER_TIMEOUT
     * PRECONDITION_NOT_MET -> PRECONDITION_NOT_MET
     */
    switch (status)
      {
      case BufferStatus::BUFFER_OK:
        // no callback
        return DataPortStatus::PORT_OK;
      case BufferStatus::BUFFER_ERROR:
        // no callback
        return DataPortStatus::BUFFER_ERROR;
      case BufferStatus::BUFFER_FULL:
        onBufferFull(data);
        return DataPortStatus::BUFFER_FULL;
      case BufferStatus::NOT_SUPPORTED:
        // no callback
        return DataPortStatus::PORT_ERROR;
      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        return DataPortStatus::BUFFER_TIMEOUT;
      case BufferStatus::PRECONDITION_NOT_MET:
        // no callback
        return DataPortStatus::PRECONDITION_NOT_MET;
      default:
        // no callback
        return DataPortStatus::PORT_ERROR;
      }
  }

  /*!
   * @if jp
   * @brief DataPortStatusに従ってリスナへ通知する関数を呼び出す。
   * @else
   * @brief Call listeners according to the DataPortStatus
   * @endif
   */
  PublisherPeriodic::ReturnCode
  PublisherPeriodic::invokeListener(DataPortStatus::Enum status,
                                    ByteData& data)
  {
    // ret:
    // PORT_OK, PORT_ERROR, SEND_FULL, SEND_TIMEOUT, CONNECTION_LOST,
    // UNKNOWN_ERROR
    switch (status)
      {
      case PORT_ERROR:
        onReceiverError(data);
        return PORT_ERROR;

      case SEND_FULL:
        onReceiverFull(data);
        return SEND_FULL;

      case SEND_TIMEOUT:
        onReceiverTimeout(data);
        return SEND_TIMEOUT;

      case CONNECTION_LOST:
        onReceiverError(data);
        return CONNECTION_LOST;

      case UNKNOWN_ERROR:
        onReceiverError(data);
        return UNKNOWN_ERROR;

      default:
        onReceiverError(data);
        return PORT_ERROR;
      }
  }

} // namespace RTC

extern "C"
{
  void PublisherPeriodicInit()
  {
    ::RTC::PublisherFactory::
      instance().addFactory("periodic",
                            ::coil::Creator< ::RTC::PublisherBase,
                                             ::RTC::PublisherPeriodic>,
                            ::coil::Destructor< ::RTC::PublisherBase,
                                                ::RTC::PublisherPeriodic>);
  }
};
