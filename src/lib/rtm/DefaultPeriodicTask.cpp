// -*- C++ -*-
/*!
 * @file DefaultPeriodicTask.h
 * @brief PeiodicTaskFactory class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009
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

#include <coil/PeriodicTask.h>
#include <rtm/DefaultPeriodicTask.h>
#include <rtm/PeriodicTaskFactory.h>

extern "C"
{
  void DefaultPeriodicTaskInit()
  {
    ::RTC::PeriodicTaskFactory::
      instance().addFactory("default",
                            ::coil::Creator< ::coil::PeriodicTaskBase,
                                             ::RTC::DefaultPeriodicTask >);
  }
}

