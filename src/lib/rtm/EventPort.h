// -*- C++ -*-
/*!
 * @file EventInPort.h
 * @brief EventInPort template class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_EVENTINPORT_H
#define RTC_EVENTINPORT_H

#include <string>
#include <vector>
#include <iostream>

#include <coil/TimeValue.h>
#include <coil/Time.h>
#include <coil/OS.h>
#include <coil/Mutex.h>
#include <coil/Guard.h>

#include <rtm/RTC.h>
#include <rtm/Typename.h>
#include <rtm/InPortBase.h>
#include <rtm/CdrBufferBase.h>
#include <rtm/PortCallback.h>
#include <rtm/InPortConnector.h>
#include <rtm/Timestamp.h>
#include <rtm/StaticFSM.h>
#include <rtm/RingBuffer.h>
#include <rtm/EventBase.h>

namespace RTC
{


  template <class FSM, class TOP, class R>
  class EventBinder0
    : public ConnectorDataListener, EventBinderBase0
  {
    USE_CONNLISTENER_STATUS;
  public:
    EventBinder0(FSM& fsm,
                 const char* event_name,
                 R (TOP::*handler)(),
                 RingBuffer<EventBase*> &buffer)
      : m_fsm(fsm), m_eventName(event_name), m_handler(handler), m_buffer(buffer) {}

    virtual ~EventBinder0() {}

    virtual ReturnCode operator()(ConnectorInfo& info,
                                  cdrMemoryStream& data)
    {
      if (info.properties["fsm_event_name"] == m_eventName ||
          info.name == m_eventName)
        {
            m_buffer.write(new Event0(this));
          //m_fsm.dispatch(Macho::Event(m_handler));
          //std::cout << "Event dispatched: " << m_eventName << std::endl;
          return NO_CHANGE;
        }
      return NO_CHANGE;
    }

    virtual void run()
    {
        
        m_fsm.dispatch(Macho::Event(m_handler));
    }

    FSM& m_fsm;
    std::string m_eventName;
    R (TOP::*m_handler)();
    RingBuffer<EventBase*> &m_buffer;

  };

  template <class FSM, class TOP, class R, class P0>
  class EventBinder1
    : public ConnectorDataListenerT<P0>, EventBinderBase1<P0>
  {
    USE_CONNLISTENER_STATUS;
  public:
    EventBinder1(FSM& fsm,
                 const char* event_name,
                 R (TOP::*handler)(P0),
                 RingBuffer<EventBase*> &buffer)
      : m_fsm(fsm), m_eventName(event_name), m_handler(handler), m_buffer(buffer) {}

    virtual ~EventBinder1() {}

    virtual ReturnCode operator()(ConnectorInfo& info, P0& data)
    {
      if (info.properties["fsm_event_name"] == m_eventName ||
          info.name == m_eventName)
        {
            m_buffer.write(new Event1<P0>(this, data));
          //m_fsm.dispatch(Macho::Event(m_handler, data));
          //std::cout << "Event dispatched: " << m_eventName << std::endl;
          return NO_CHANGE;
        }
      return NO_CHANGE;
    }

    virtual void run(P0& data)
    {
        m_fsm.dispatch(Macho::Event(m_handler, data));
    }

    FSM& m_fsm;
    std::string m_eventName;
    R (TOP::*m_handler)(P0);
    RingBuffer<EventBase*> &m_buffer;
  };

  class EventConnListener
      : public ConnectorListener
  {
      USE_CONNLISTENER_STATUS;
  public:
      EventConnListener(RingBuffer<EventBase*>&buffer, CdrBufferBase* m_thebuffer) :
          m_buffer(buffer), m_thebuffer(m_thebuffer) {}
      virtual ~EventConnListener()
      {
      }

      virtual ReturnCode operator()(ConnectorInfo& info)
      {
          coil::Properties prop;
          prop["write.full_policy"] = "do_nothing";
          prop["read.empty_policy"] = "do_nothing";
          m_thebuffer->init(prop);

          coil::Properties prop_(info.properties.getNode("dataport.buffer"));
          prop_ << info.properties.getNode("inport.buffer");

          m_buffer.init(prop_);


          return NO_CHANGE;
      };
      RingBuffer<EventBase*>&m_buffer;
      CdrBufferBase *m_thebuffer;
  };


  /*!
   * @if jp
   *
   * @class EventInPort
   *
   * @brief EventInPort テンプレートクラス
   * 
   * EventInPort の実装である EventInPort<T> のテンプレートクラス。
   * <T> はBasicDataType.idl にて定義されている型で、メンバとして
   * Time 型の tm , および T型の data を持つ構造体でなくてはならない。
   * EventInPort は内部にリングバッファを持ち、外部から送信されたデータを順次
   * このリングバッファに格納する。リングバッファのサイズはデフォルトで64と
   * なっているが、コンストラクタ引数によりサイズを指定することができる。
   * データはフラグによって未読、既読状態が管理され、isNew(), write(), read(),
   * isFull(), isEmpty() 等のメソッドによりハンドリングすることができる。
   *   
   * OnRead系コールバック (読み出しに起因するイベントによりコールされる)
   *
   * - void OnRead::operator(): 
   *     EventInPort::read() を呼び出し読み出しを行う際にコールされる。
   *
   * - DataType OnReadConvert::operator(DataType): 
   *     EventInPort::read() を呼び出し、データをバッファから読みだす際に呼ばれ
   *     データの変換を行う。引数にはバッファから読み出された値が与えられ、
   *     変換後のデータを戻り値として返す。この値がread()の返す値となる。
   *
   * @since 0.2.0
   *
   * @else
   *
   * @class EventInPort
   *
   * @brief EventInPort template class
   *
   * This is a template class that implements EventInPort.  <T> is the type
   * defined in BasicDataType.idl and must be the structure which has
   * both Time type tm and type-T data as a member. EventInPort has a ring
   * buffer internally, and stores the received data externally in
   * this buffer one by one. The size of ring buffer can be specified
   * according to the argument of constructor, though the default size
   * is 64. Unread data and data which is already read are managed
   * with the flag, and the data can be handled by the isNew(),
   * write(), read(), isFull() and isEmpty() method etc.
   *
   * @since 0.2.0
   *
   * @endif
   */
  template <class FsmType>
  class EventInPort
    : public InPortBase
  {
  public:
    DATAPORTSTATUS_ENUM
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     * パラメータとして与えられる T 型の変数にバインドされる。
     *
     * @param name EventInPort 名。EventInPortBase:name() により参照される。
     * @param value この EventInPort にバインドされる T 型の変数
     * @param bufsize EventInPort 内部のリングバッファのバッファ長(デフォルト値:64)
     * @param read_block 読込ブロックフラグ。
     *        データ読込時に未読データがない場合、次のデータ受信までブロックする
     *        かどうかを設定(デフォルト値:false)
     * @param write_block 書込ブロックフラグ。
     *        データ書込時にバッファがフルであった場合、バッファに空きができる
     *        までブロックするかどうかを設定(デフォルト値:false)
     * @param read_timeout 読込ブロックを指定していない場合の、データ読取タイム
     *        アウト時間(ミリ秒)(デフォルト値:0)
     * @param write_timeout 書込ブロックを指定していない場合の、データ書込タイム
     *        アウト時間(ミリ秒)(デフォルト値:0)
     *
     * @else
     *
     * @brief A constructor.
     *
     * constructor.
     * This is bound to type-T variable given as a parameter.
     *
     * @param name A name of the EventInPort. This name is referred by
     *             EventInPortBase::name().
     * @param value type-T variable that is bound to this EventInPort.
     * @param bufsize Buffer length of internal ring buffer of EventInPort
     *                (The default value:64)
     * @param read_block Flag of reading block.
     *                   When there are not unread data at reading data,
     *                   set whether to block data until receiving the next 
     *                   data. (The default value:false)
     * @param write_block Flag of writing block.
     *                    If the buffer was full at writing data, set whether 
     *                    to block data until the buffer has space. 
     *                    (The default value:false)
     * @param read_timeout Data reading timeout time (millisecond) 
     *                     when not specifying read blocking.
     *                     (The default value:0)
     * @param write_timeout Data writing timeout time (millisecond)
     *                      when not specifying writing block.
     *                      (The default value:0)
     *
     * @endif
     */
    EventInPort(const char* name, FsmType& fsm,
                int bufsize=64,
                bool read_block = false, bool write_block = false,
                int read_timeout = 0, int write_timeout = 0)
      : InPortBase(name, "any"),
      m_name(name), m_fsm(fsm), m_buffer(fsm.getBuffer())
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
    virtual ~EventInPort(void){};

    /*!
     * @if jp
     *
     * @brief ポート名称を取得する。
     *
     * ポート名称を取得する。
     *
     * @return ポート名称
     *
     * @else
     *
     * @brief Get port name
     *
     * Get port name.
     *
     * @return The port name
     *
     * @endif
     */
    virtual const char* name()
    {
      return m_name.c_str();
    }

    virtual void init(coil::Properties& prop)
    {
        InPortBase::init(prop);
        this->addConnectorListener
            (ON_CONNECT,
            new EventConnListener(m_buffer, m_thebuffer));
    }

    template <class TOP, class R, class P0>
    void bindEvent(const char* name,
                   R (TOP::*handler)(P0))
    {
      this->addConnectorDataListener
        (ON_RECEIVED,
         new EventBinder1<FsmType, TOP, R, P0>(m_fsm, name, handler, m_buffer));
    }
    template <typename TOP, class R>
    void bindEvent(const char* name,
                   R (TOP::*handler)())
    {
      this->addConnectorDataListener
        (ON_RECEIVED,
         new EventBinder0<FsmType, TOP, R>(m_fsm, name, handler, m_buffer));
    }
    virtual bool read(std::string name="") { return true; }
  private:
    /*!
     * @if jp
     * @brief ポート名
     * @else
     * @berif Port's name
     * @endif
     */
    std::string m_name;
    FsmType& m_fsm;
    RingBuffer<EventBase*> &m_buffer;
  };
}; // End of namesepace RTM

#endif // RTC_EVENTINPORT_H
