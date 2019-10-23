// -*- C++ -*-
/*!
 * @file  InPortConsumer.h
 * @brief InPortConsumer class
 * @date  $Date: 2007-12-31 03:08:03 $
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
 * $Id: InPortConsumer.h 1225 2009-02-28 02:30:25Z n-ando $
 *
 */

#include <rtm/CdrRingBuffer.h>

namespace RTC
{
} // namespace RTC

extern "C"
{
  void CdrRingBufferInit()
  {
    RTC::CdrBufferFactory::instance().
      addFactory("ring_buffer",
                 coil::Creator<RTC::CdrBufferBase, RTC::CdrRingBuffer>);
  }
}
