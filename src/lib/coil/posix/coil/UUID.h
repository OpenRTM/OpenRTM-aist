// -*- C++ -*-
/*!
 * @file  MutexPosix.h
 * @brief RT-Middleware Service interface
 * @date  $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
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

#ifndef COIL_UUID_H
#define COIL_UUID_H

#include <coil/config_coil.h>
#include <uuid/uuid.h>
namespace coil
{
  class UUID
  {
    uuid_t _uuid;
    char buf[37];
  public:
    UUID();
    explicit UUID(uuid_t* /*uuid*/);
    const char* to_string();
  };

  class UUID_Generator
  {
  public:
    UUID_Generator();

    void init();
    UUID* generateUUID(int n, int h);
  };
} // namespace coil

#endif  // COIL_UUID_H
