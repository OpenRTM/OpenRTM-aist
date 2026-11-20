// -*- C++ -*-
/*!
 * @file FactoryInit.cpp
 * @if jp
 * @brief OpenRTMの通信・バッファなどのFactoryを初期化する。 実装。
 * @else
 * @brief Initialize OpenRTM communication, buffer and related factories. Implementation.
 * @endif
 * @date $Date: 2026-11-13 15:32:20 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 */

// -*- C++ -*-
/*
 * @file FactoryInit.cpp
 * @brief factory initialization function
 * @date $Date: 2008-03-06 06:58:40 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009-2017
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: Manager.cpp 1296 2009-04-27 08:43:24Z n-ando $
 *
 */

// Logstream
#include <rtm/LogstreamFile.h>

// Buffers
#include <rtm/CdrRingBuffer.h>

// Threads
#include <rtm/DefaultPeriodicTask.h>

// default Publishers
#include <rtm/PublisherFlush.h>
#include <rtm/PublisherNew.h>
#include <rtm/PublisherPeriodic.h>

// default Provider/Consumer
#include <rtm/InPortCorbaCdrProvider.h>
#include <rtm/InPortCorbaCdrConsumer.h>
#include <rtm/OutPortCorbaCdrConsumer.h>
#include <rtm/CSPTransport.h>
#include <rtm/OutPortCorbaCdrProvider.h>
#include <rtm/InPortDirectProvider.h>
#include <rtm/InPortDirectConsumer.h>
#include <rtm/OutPortDirectProvider.h>
#include <rtm/OutPortDirectConsumer.h>
#ifndef ORB_IS_RTORB
#include <rtm/InPortSHMProvider.h>
#include <rtm/InPortSHMConsumer.h>
#include <rtm/OutPortSHMProvider.h>
#include <rtm/OutPortSHMConsumer.h>
#endif
#include <rtm/InPortDSProvider.h>
#include <rtm/InPortDSConsumer.h>
#include <rtm/OutPortDSProvider.h>
#include <rtm/OutPortDSConsumer.h>
#ifdef ORB_IS_TAO
#include <rtm/InPortCorbaCdrUDPProvider.h>
#include <rtm/InPortCorbaCdrUDPConsumer.h>
#endif

// RTC name numbering policy
#include <rtm/NumberingPolicy.h>
#include <rtm/NamingServiceNumberingPolicy.h>
#include <rtm/NodeNumberingPolicy.h>

#ifdef ENABLE_OBSERVER
#include <ext/sdo/observer/ComponentObserverConsumer.h>
#endif

namespace RTM
{
  void FactoryInit()
  {
    // Logstream
    LogstreamFileInit();

    // Buffers
    CdrRingBufferInit();

    // Threads
    DefaultPeriodicTaskInit();

    // Publishers
    PublisherFlushInit();
    PublisherNewInit();
    PublisherPeriodicInit();

    // Providers/Consumer
    InPortCSPProviderInit();
    OutPortCSPProviderInit();
    InPortCSPConsumerInit();
    OutPortCSPConsumerInit();
    InPortCorbaCdrProviderInit();
    InPortCorbaCdrConsumerInit();
    OutPortCorbaCdrConsumerInit();
    OutPortCorbaCdrProviderInit();
    InPortDirectProviderInit();
    InPortDirectConsumerInit();
	OutPortDirectProviderInit();
	OutPortDirectConsumerInit();
#ifndef ORB_IS_RTORB
    InPortSHMProviderInit();
    InPortSHMConsumerInit();
    OutPortSHMProviderInit();
    OutPortSHMConsumerInit();
#endif
    InPortDSProviderInit();
    InPortDSConsumerInit();
    OutPortDSProviderInit();
    OutPortDSConsumerInit();
#ifdef ORB_IS_TAO
    InPortCorbaCdrUDPProviderInit();
    InPortCorbaCdrUDPConsumerInit();
#endif

    // Naming Policy
    ProcessUniquePolicyInit();
	NamingServiceNumberingPolicyInit();
	NodeNumberingPolicyInit();

    // SDO services
#ifdef ENABLE_OBSERVER
    ComponentObserverConsumerInit();
#endif
  }
} // namespace RTM
