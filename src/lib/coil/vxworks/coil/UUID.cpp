// -*- C++ -*-
/*!
 * @file  UUID.h
 * @brief UUID Generator for VxWorks
 * @date  $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/UUID.h>
#include <iostream>
#include <string.h>

#if defined(VXWORKS_66) && !defined(__RTP__)
#include <timers.h>
#else
#include <sys/time.h>
#endif

#include <coil/TimeValue.h>
#include <coil/Time.h>

#ifndef __RTP__
#include <muxLib.h>
#endif


namespace coil
{






  uuid_t::uuid_t()
  {
    time_low = 0;
    time_mid = 0;
    time_hi_version = 0;
    clock_seq_low = 0;
    clock_seq_hi_variant = 0;
    node_low = 0;
    node_high = 0;
  }
  UUID_Generator::UUID_Generator(){}
  
  void UUID_Generator::init(){}
  UUID* UUID_Generator::generateUUID(int version, int variant){
    uuid_t uuid;
    coil::TimeValue tv = coil::gettimeofday();
    uint64_t timestamp = ((uint64_t)tv.sec()*10000000LL + (uint64_t)tv.usec()/100LL) + 0x01b21dd213814000LL;
    srand(tv.sec() + tv.usec());
    uint64_t clock_seq = rand() * (1E14+ 1.0) / (1.0 + RAND_MAX);
    
    uuid.time_low = timestamp & 0xffffffffL;
    uuid.time_mid = (timestamp >> 32L) & 0xffffL;
    uuid.time_hi_version = ((timestamp >> 48L) & 0x0fffL) | version << 12L;
    uuid.clock_seq_low = clock_seq & 0xffL;
    uuid.clock_seq_hi_variant = (clock_seq >> 8L) & 0x3fL;
    
#if defined(__RTP__) || defined(VXWORKS_66)
    uint64_t node = rand() * (281474976710655LL + 1.0) / (1.0 + RAND_MAX);
#else
    char mData[6];
    PROTO_COOKIE muxCookie = muxBind ("eth0",1,NULL,NULL,NULL,NULL,MUX_PROTO_SNARF,"IEEE802.2 LLC",NULL); 
    muxIoctl(muxCookie, EIOCGADDR, mData);
    uint64_t node = (uint64_t)mData[0] + ((uint64_t)mData[1] >> 8) + ((uint64_t)mData[2] >> 16) + ((uint64_t)mData[3] >> 24) + ((uint64_t)mData[4] >> 32) + ((uint64_t)mData[5] >> 40);
#endif
    uuid.node_low = node & 0xffffffffL;
    uuid.node_high = (node >> 32L) & 0xffffL;
    
    

    return new UUID(&uuid);
  }
  

  
  UUID::UUID(){
    
  }
  
  UUID::UUID(uuid_t *uuid){
    _uuid.time_low = uuid->time_low;
    _uuid.time_mid = uuid->time_mid;
    _uuid.time_hi_version = uuid->time_hi_version;
    _uuid.clock_seq_low = uuid->clock_seq_low;
    _uuid.clock_seq_hi_variant = uuid->clock_seq_hi_variant;
  
    _uuid.node_low = uuid->node_low;
    _uuid.node_high = uuid->node_high;
  }
  
  const char* UUID::to_string()
  {
    std::string result;

    result = StringToUUID<uint32_t>(_uuid.time_low,8);
    result += "-";
    result += StringToUUID<uint16_t>(_uuid.time_mid,4);
    result += "-";
    result += StringToUUID<uint16_t>(_uuid.time_hi_version,4);
    result += "-";
    result += StringToUUID<uint16_t>((uint16_t)_uuid.clock_seq_low,2);
    result += StringToUUID<uint16_t>((uint16_t)_uuid.clock_seq_hi_variant,2);
    result += "-";
    result += StringToUUID<uint32_t>(_uuid.node_low,8);
    result += StringToUUID<uint16_t>(_uuid.node_high,4);

    return strdup(result.c_str());
  }



};
