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

#ifndef COIL_UUID_H
#define COIL_UUID_H

#include <coil/config_coil.h>
#include <stdlib.h>
#include <sstream>
#include <vxWorks.h>





namespace coil
{
  class uuid_t {
  public:
    uuid_t();
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_version;
    uint8_t clock_seq_low;
    uint8_t clock_seq_hi_variant;
    uint32_t node_low;
    uint16_t node_high;
  };


  template <typename DataType>
  std::string StringToUUID(DataType v, unsigned int size)
  {
    std::stringstream stream;
    stream << std::hex << v;
    std::string ret(stream.str());

    if(ret.size() < size)
    {
      for(unsigned int i=ret.size();i < size;i++)
      {
        ret = "0" + ret;
      }
    }
    return ret;
  }

  class UUID
  {
    uuid_t _uuid;
    char buf[37];
  public:
    UUID();
    UUID(uuid_t*);
    const char* to_string();
  };

  class UUID_Generator
  {
  public:
    UUID_Generator();
    
    void init();
    UUID* generateUUID(int n, int h);
  };
};


#endif // COIL_UUID_H
