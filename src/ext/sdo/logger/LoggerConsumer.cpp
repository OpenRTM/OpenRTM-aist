// -*- C++ -*-
/*!
 * @file LoggerConsumer.h
 * @brief Component observer SDO service consumer implementation
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
 * $Id: SdoConfiguration.cpp 1971 2010-06-03 08:46:40Z n-ando $
 *
 */

#include <coil/stringutil.h>
#include <rtm/Typename.h>
#include "LoggerSkel.h"
#include "LoggerConsumer.h"
#include <iostream>

namespace RTC
{
  /*!
   * @if jp
   * @brief ctor of LoggerConsumer
   * @else
   * @brief ctor of LoggerConsumer
   * @endif
   */
  LoggerConsumer::LoggerConsumer()
  {
  }

  /*!
   * @if jp
   * @brief dtor
   * @else
   * @brief dtor
   * @endif
   */
  LoggerConsumer::~LoggerConsumer() = default;

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief Initialization
   * @endif
   */
  bool
  LoggerConsumer::init(RTObject_impl& rtobj,
                       const SDOPackage::ServiceProfile& profile)
  {
    if (!m_logger.setObject(profile.service))
      {
        // narrowing failed
        return false;
      }

    m_rtobj = &rtobj;
    m_profile = profile;
    coil::Properties prop;
    NVUtil::copyToProperties(prop, profile.properties);
    return true;
  }

  /*!
   * @if jp
   * @brief 再初期化
   * @else
   * @brief Re-initialization
   * @endif
   */
  bool
  LoggerConsumer::reinit(const SDOPackage::ServiceProfile& profile)
  {
    if (!m_logger._ptr()->_is_equivalent(profile.service))
      {
        CorbaConsumer<OpenRTM::Logger> tmp;
        if (!tmp.setObject(profile.service))
          {
            return false;
          }
        m_logger.releaseObject();
        m_logger.setObject(profile.service);
      }
    m_profile= profile;
    coil::Properties prop;
    NVUtil::copyToProperties(prop, profile.properties);
    return true;
  }

  /*!
   * @if jp
   * @brief ServiceProfile を取得する
   * @else
   * @brief getting ServiceProfile
   * @endif
   */
  const SDOPackage::ServiceProfile&
  LoggerConsumer::getProfile() const
  {
    return m_profile;
  }  

  /*!
   * @if jp
   * @brief 終了処理
   * @else
   * @brief Finalization
   * @endif
   */
  void LoggerConsumer::finalize()
  {
  }

  //============================================================
  // protected functions
 
} // namespace RTC

extern "C"
{
  void LoggerConsumerInit()
  {
    RTC::SdoServiceConsumerFactory& factory
      = RTC::SdoServiceConsumerFactory::instance();
    factory.addFactory(CORBA_Util::toRepositoryId<OpenRTM::Logger>(),
                       ::coil::Creator< ::RTC::SdoServiceConsumerBase,
                       ::RTC::LoggerConsumer>);
  }
}
