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


#ifdef COIL_OS_FREEBSD
void error_code(uint32_t status)
{
  if (status == uuid_s_ok)
    std::cout << "uuid_s_ok" << std::endl;
  else if (status == uuid_s_bad_version)
    std::cout << "uuid_s_bad_version" << std::endl;
  else if (status == uuid_s_invalid_string_uuid)
    std::cout << "uuid_s_invalid_string_uuid" << std::endl;
  else if (status == uuid_s_no_memory)
    std::cout << "uuid_s_no_memory" << std::endl;
  else
    std::cout << "other error" << std::endl;
}

void uuid_clear(uuid_t& uu)
{
  uint32_t status;
  uuid_create_nil(&uu, &status);
}
void uuid_unparse(uuid_t& uu, char*& uuidstr)
{
  uint32_t status;
  uuid_to_string(&uu, &uuidstr, &status);
}
void uuid_generate(uuid_t& out)
{
  uint32_t status;
  uuid_create(&out, &status);
}
#endif

namespace coil
{

#ifdef COIL_OS_FREEBSD
  UUID::UUID()
    : m_uuidstr(0)
  {
    ::uuid_clear(m_uuid);
  }
  UUID::UUID(const uuid_t& uuid)
    : m_uuid(uuid), m_uuidstr(0)
  {
  }

  UUID::~UUID()
  {
    free(m_uuidstr);
  }

  const char* UUID::to_string()
  {
    uuid_unparse(m_uuid, m_uuidstr);
    return m_uuidstr;
  }


  UUID_Generator::UUID_Generator()
  {
  }

  UUID_Generator::~UUID_Generator()
  {
  }

  void UUID_Generator::init()
  {
  }

  UUID* UUID_Generator::generateUUID(int n, int h)
  {
    uuid_t uuid;
    uuid_generate(uuid);
    return new UUID(uuid);
  }
#endif

#if defined(COIL_OS_LINUX) || defined(COIL_OS_DARWIN) || defined(COIL_OS_QNX)

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

#endif
} // namespace coil
