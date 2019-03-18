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

namespace coil
{
  //------------------------------------------------------------
  // UUID class
  //------------------------------------------------------------
  /*!
   * @if jp
   * @brief UUIDクラス コンストラクタ
   * @else
   * @brief UUID class constructor
   * @endif
   */
  UUID::UUID()
    : m_uuidstr(0)
  {
  }

  /*!
   * @if jp
   * @brief UUIDクラス コンストラクタ
   * @else
   * @brief UUID class constructor
   * @endif
   */
  UUID::UUID(const uuid_t& uuid)
    : m_uuid(uuid), m_uuidstr(0)
  {
  }

  /*!
   * @if jp
   * @brief UUIDクラス デストラクタ
   * @else
   * @brief UUID class destructor
   * @endif
   */
  UUID::~UUID()
  {
    ::RpcStringFreeA(reinterpret_cast<RPC_CSTR*>(&m_uuidstr));
  }

  /*!
   * @if jp
   * @brief UUID値を文字列に変換する
   * @else
   * @brief Converting from UUID value to string
   * @endif
   */
  const char* UUID::to_string()
  {
    if (::UuidToStringA(&m_uuid, reinterpret_cast<RPC_CSTR*>(&m_uuidstr))
       != RPC_S_OK)
      {
        return 0;
      }
    else
      {
        return m_uuidstr;
      }
  }

  //------------------------------------------------------------
  // UUID_Generator class
  //------------------------------------------------------------
  /*!
   * @if jp
   * @brief UUIDクラス コンストラクタ
   *
   * @else
   * @brief UUID class constructor
   *
   * @endif
   */
  UUID_Generator::UUID_Generator()
  {
  }
  /*!
   * @if jp
   * @brief UUIDクラス デストラクタ
   * @else
   * @brief UUID class destructor
   * @endif
   */
  UUID_Generator::~UUID_Generator()
  {
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief Initialization
   * @endif
   */
  void UUID_Generator::init()
  {
  }

  /*!
   * @if jp
   * @brief UUIDを生成する
   * @else
   * @brief Generate UUID value
   * @endif
   */
  UUID* UUID_Generator::generateUUID(int /*n*/, int /*h*/)
  {
    uuid_t uuid;
    if (::UuidCreate(&uuid) != RPC_S_OK)
      {
        return 0;
      }
    else
      {
        return new UUID(uuid);
      }
  }
} // namespace coil
