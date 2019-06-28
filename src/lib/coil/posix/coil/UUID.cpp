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

#include <coil/UUID.h>

#include <cstring>
#include <iostream>


namespace coil
{
  UUID_Generator::UUID_Generator() {}

  void UUID_Generator::init() {}
  UUID* UUID_Generator::generateUUID(int  /*varsion*/, int  /*variant*/)
  {
    uuid_t uuid;

    uuid_generate(uuid);
    return new UUID(&uuid);
  }

  UUID::UUID()
  {
    uuid_clear(this->_uuid);
  }

  UUID::UUID(uuid_t *uuid)
  {
    strncpy(reinterpret_cast<char *>(this->_uuid),
            reinterpret_cast<char *>(*uuid), sizeof(this->_uuid));
  }

  const char* UUID::to_string()
  {
    uuid_unparse(this->_uuid, buf);
    return buf;
  }
} // namespace coil
