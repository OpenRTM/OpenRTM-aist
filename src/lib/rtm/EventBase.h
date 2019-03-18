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

#ifndef RTC_EVENTBASE_H
#define RTC_EVENTBASE_H



namespace RTC
{
  class EventBinderBase0
  {
  public:
      EventBinderBase0(){}
      virtual ~EventBinderBase0() {}
      virtual void run() = 0;
  };
  template <class P0>class EventBinderBase1
  {
  public:
      EventBinderBase1(){}
      virtual ~EventBinderBase1() {}
      virtual void run(P0& data) = 0;
  };

  class EventBase
  {
  public:
      EventBase(){}
      virtual ~EventBase(){}
      virtual void operator()()=0;
  };

  class Event0 : public EventBase
  {
  public:
      Event0(EventBinderBase0 *eb):
          m_eb(eb)
      {
      }
      ~Event0() override {}
      void operator()() override
      {
          m_eb->run();
      }
  private:
      EventBinderBase0 *m_eb;
  };

  template <class P0>
  class Event1 : public EventBase
  {
  public:
      Event1(EventBinderBase1<P0> *eb, P0 &data):
          m_eb(eb), m_data(data)
      {
      }
      ~Event1() override {}
      void operator()() override
      {
          m_eb->run(m_data);
      }
  private:
      EventBinderBase1<P0> *m_eb;
      P0 m_data;
  };


} // namespace RTC

#endif // RTC_EVENTBASE_H
