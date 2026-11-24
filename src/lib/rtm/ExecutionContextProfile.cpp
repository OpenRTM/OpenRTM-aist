// -*- C++ -*-
/*!
 * @file ExecutionContextProfile.cpp
 * @brief ExecutionContextProfile class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <array>
#include <cassert>
#include <rtm/ExecutionContextProfile.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/NVUtil.h>

namespace RTC_impl
{

  /*!
   * @if jp
   * @brief デフォルトコンストラクタ
   * @else
   * @brief Default constructor
   * @endif
   */
  ExecutionContextProfile::
  ExecutionContextProfile(RTC::ExecutionKind  /*kind*/)
    : rtclog("periodic_ecprofile"),
      m_period(std::chrono::microseconds(1)),
      m_ref(RTC::ExecutionContextService::_nil())
  {
    RTC_TRACE(("ExecutionContextProfile()"));
    RTC_DEBUG(("Actual rate: %lld [nsec]", m_period.count()));
    // profile initialization
    m_profile.kind = RTC::PERIODIC;
#if defined(_MSC_VER) && (_MSC_VER < 1900)
    // Visual Studio 2013: std::chrono is broken.
    m_profile.rate = 1.0
                     / m_period.count()
                     * decltype(m_period)::period::den;
#else
    m_profile.rate = std::chrono::duration<double>(1) / m_period;
#endif
    m_profile.owner = RTC::RTObject::_nil();
    m_profile.participants.length(0);
    m_profile.properties.length(0);
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ExecutionContextProfile::~ExecutionContextProfile()
  {
    RTC_TRACE(("~ExecutionContextProfile()"));

    // cleanup EC's profile
    m_profile.owner = RTC::RTObject::_nil();
    m_profile.participants.length(0);
    m_profile.properties.length(0);
    m_ref = RTC::ExecutionContextService::_nil();
  }

  /*!
   * @if jp
   * @brief CORBA オブジェクト参照の取得
   * @else
   * @brief Get the reference to the CORBA object
   * @endif
   */
  void ExecutionContextProfile::
  setObjRef(RTC::ExecutionContextService_ptr ec_ptr)
  {
    RTC_TRACE(("setObjRef()"));
    assert(!CORBA::is_nil(ec_ptr));
    std::lock_guard<std::mutex> guard(m_profileMutex);
    m_ref = RTC::ExecutionContextService::_duplicate(ec_ptr);
  }

  /*!
   * @if jp
   * @brief CORBA オブジェクト参照の取得
   * @else
   * @brief Get the reference to the CORBA object
   * @endif
   */
  RTC::ExecutionContextService_ptr
  ExecutionContextProfile::getObjRef() const
  {
    RTC_TRACE(("getObjRef()"));
    std::lock_guard<std::mutex> guard(m_profileMutex);
#ifdef ORB_IS_ORBEXPRESS
    return RTC::ExecutionContextService::_duplicate(m_ref.in());
#else
    return RTC::ExecutionContextService::_duplicate(m_ref);
#endif
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行周期(Hz)を設定する
   * @else
   * @brief Set execution rate(Hz) of ExecutionContext
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextProfile::setRate(double rate)
  {
    RTC_TRACE(("setRate(%f)", rate));
    if (rate <= 0.0) { return RTC::BAD_PARAMETER; }

    std::chrono::duration<double> period(1.0 / rate);
    return setPeriod(std::chrono::duration_cast<std::chrono::nanoseconds>(period));
  }

  RTC::ReturnCode_t ExecutionContextProfile::setPeriod(std::chrono::nanoseconds period)
  {
    RTC_TRACE(("setPeriod(%lld [nsec])", period.count()));
    if (period <= std::chrono::seconds::zero())
      {
        return RTC::BAD_PARAMETER;
      }

    std::lock_guard<std::mutex> guard(m_profileMutex);
#if defined(_MSC_VER) && (_MSC_VER < 1900)
    // Visual Studio 2013: std::chrono is broken.
    m_profile.rate = 1.0
                     / period.count()
                     * decltype(period)::period::den;
#else
    m_profile.rate = std::chrono::duration<double>(1) / period;
#endif
    m_period = period;
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief ExecutionContext の実行周期(Hz)を取得する
   * @else
   * @brief Get execution rate(Hz) of ExecutionContext
   * @endif
   */
  CORBA::Double ExecutionContextProfile::getRate() const
  {
    std::lock_guard<std::mutex> guard(m_profileMutex);
    return m_profile.rate;
  }

  std::chrono::nanoseconds ExecutionContextProfile::getPeriod() const
  {
    std::lock_guard<std::mutex> guard(m_profileMutex);
    return m_period;
  }

  /*!
   * @if jp
   * @brief ExecutionKind を文字列化する
   * @else
   * @brief Converting ExecutionKind enum to string
   * @endif
   */
  const char*
  ExecutionContextProfile::getKindString(RTC::ExecutionKind kind) 
  {
    static const std::array<const char*, 3> kinds = {{"PERIODIC", "EVENT_DRIVEN", "OTHER"}};
    if (kind < RTC::PERIODIC || kind > RTC::OTHER)
      {
        return "";
      }
    return kinds[kind];
  }

  /*!
   * @if jp
   * @brief ExecutionKind を取得する
   * @else
   * @brief Get the ExecutionKind
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextProfile::setKind(RTC::ExecutionKind kind)
  {
    if (kind < RTC::PERIODIC || kind > RTC::OTHER)
      {
        RTC_ERROR(("Invalid kind is given. %d", kind));
        return RTC::BAD_PARAMETER;
      }
    RTC_TRACE(("setKind(%s)", getKindString(kind)));
    std::lock_guard<std::mutex> guard(m_profileMutex);
    m_profile.kind = kind;
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief ExecutionKind を取得する
   * @else
   * @brief Get the ExecutionKind
   * @endif
   */
  RTC::ExecutionKind ExecutionContextProfile::getKind() const
  {
    std::lock_guard<std::mutex> guard(m_profileMutex);
    RTC_TRACE(("%s = getKind()", getKindString(m_profile.kind)));
    return m_profile.kind;
  }

  /*!
   * @if jp
   * @brief Ownerコンポーネントをセットする。
   * @else
   * @brief Setting owner component of the execution context
   * @endif
   */
  RTC::ReturnCode_t ExecutionContextProfile::
  setOwner(RTC::LightweightRTObject_ptr comp)
  {
    RTC_TRACE(("setOwner()"));
    assert(!CORBA::is_nil(comp));
    RTC::RTObject_var rtobj = RTC::RTObject::_narrow(comp);
    if (CORBA::is_nil(rtobj))
      {
        RTC_ERROR(("Narrowing failed."));
        return RTC::BAD_PARAMETER;
      }
    std::lock_guard<std::mutex> guard(m_profileMutex);
    m_profile.owner = RTC::RTObject::_duplicate(rtobj);
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief Ownerコンポーネントの参照を取得する
   * @else
   * @brief Getting a reference of the owner component
   * @endif
   */
  RTC::RTObject_ptr ExecutionContextProfile::getOwner() const
  {
    RTC_TRACE(("getOwner()"));
    std::lock_guard<std::mutex> guard(m_profileMutex);
#ifdef ORB_IS_ORBEXPRESS
    return RTC::RTObject::_duplicate(m_profile.owner.in());
#else
    return RTC::RTObject::_duplicate(m_profile.owner);
#endif
  }

  /*!
   * @if jp
   * @brief RTコンポーネントを追加する
   * @else
   * @brief Add an RT-component
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextProfile::addComponent(RTC::LightweightRTObject_ptr comp)
  {
    RTC_TRACE(("addComponent()"));
    if (CORBA::is_nil(comp))
      {
        RTC_ERROR(("A nil reference was given."));
        return RTC::BAD_PARAMETER;
      }
    RTC::RTObject_var rtobj = RTC::RTObject::_narrow(comp);
    if (CORBA::is_nil(rtobj))
      {
        RTC_ERROR(("Narrowing was failed."));
        return RTC::RTC_ERROR;
      }
    std::lock_guard<std::mutex> guard(m_profileMutex);
#ifndef ORB_IS_RTORB
    CORBA_SeqUtil::push_back(m_profile.participants, rtobj._retn());
#else
    CORBA_SeqUtil::push_back(m_profile.participants,
                             RTC::RTObject::_duplicate(rtobj.in()));
#endif
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief RTコンポーネントを参加者リストから削除する
   * @else
   * @brief Remove the RT-Component from participant list
   * @endif
   */
  RTC::ReturnCode_t
  ExecutionContextProfile::removeComponent(RTC::LightweightRTObject_ptr comp)
  {
    RTC_TRACE(("removeComponent()"));
    if (CORBA::is_nil(comp))
      {
        RTC_ERROR(("A nil reference was given."));
        return RTC::BAD_PARAMETER;
      }
    RTC::RTObject_var rtobj = RTC::RTObject::_narrow(comp);
    if (CORBA::is_nil(rtobj))
      {
        RTC_ERROR(("Narrowing was failed."));
        return RTC::RTC_ERROR;
      }
    {
      std::lock_guard<std::mutex> guard(m_profileMutex);
      CORBA::Long index;
      index = CORBA_SeqUtil::find(m_profile.participants,
                                  find_participant(rtobj));
      if (index < 0)
        {
          RTC_ERROR(("The given RTObject does not exist in the EC."));
          return RTC::BAD_PARAMETER;
        }
      CORBA_SeqUtil::erase(m_profile.participants, index);
    }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief ExecutionKind を取得する
   * @else
   * @brief Get the ExecutionKind
   * @endif
   */
#ifndef ORB_IS_RTORB
  const RTC::RTCList& ExecutionContextProfile::getComponentList() const
#else
  const RTC_RTCList& ExecutionContextProfile::getComponentList() const
#endif
  {
    RTC_TRACE(("getComponentList(%d)", m_profile.participants.length()));
    return m_profile.participants;
  }

  /*!
   * @if jp
   * @brief Propertiesをセットする
   * @else
   * @brief Setting Properties
   * @endif
   */
  void ExecutionContextProfile::setProperties(coil::Properties& props)
  {
    RTC_TRACE(("setProperties()"));
    RTC_DEBUG_STR((props));
    std::lock_guard<std::mutex> guard(m_profileMutex);
    NVUtil::copyFromProperties(m_profile.properties, props);
  }

  /*!
   * @if jp
   * @brief Propertiesを取得する
   * @else
   * @brief Setting Properties
   * @endif
   */
  coil::Properties ExecutionContextProfile::getProperties() const
  {
    RTC_TRACE(("getProperties()"));
    std::lock_guard<std::mutex> guard(m_profileMutex);
    coil::Properties props = coil::Properties();
    NVUtil::copyToProperties(props, m_profile.properties);
    RTC_DEBUG_STR((props));
    return props;
  }

  /*!
   * @if jp
   * @brief ExecutionContextProfile を取得する
   * @else
   * @brief Get the ExecutionContextProfile
   * @endif
   */
  RTC::ExecutionContextProfile* ExecutionContextProfile::getProfile()
  {
    RTC_TRACE(("getProfile()"));
    RTC::ExecutionContextProfile_var p;
    {
      std::lock_guard<std::mutex> guard(m_profileMutex);
      p = new RTC::ExecutionContextProfile(m_profile);
    }
    return p._retn();
  }

  /*!
   * @if jp
   * @brief ExecutionContextProfile を取得する
   * @else
   * @brief Get the ExecutionContextProfile
   * @endif
   */
  const RTC::ExecutionContextProfile&
  ExecutionContextProfile::getProfile() const
  {
    RTC_TRACE(("getProfile()"));
    std::lock_guard<std::mutex> guard(m_profileMutex);
    return m_profile;
  }

  /*!
   * @if jp
   * @brief ExecutionContextProfileをロックする
   * @else
   * @brief Getting a lock of RTC::ExecutionContextProfile
   * @endif
   */
  void ExecutionContextProfile::lock() const
  {
    m_profileMutex.lock();
  }

  /*!
   * @if jp
   * @brief ExecutionContextProfileをアンロックする
   * @else
   * @brief Release a lock of the RTC::ExecutionContextProfile
   * @endif
   */
  void ExecutionContextProfile::unlock() const
  {
    m_profileMutex.unlock();
  }

} // namespace RTC_impl
