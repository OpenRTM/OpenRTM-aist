// -*- C++ -*-
/*!
 * @file RTObjectBinding.cpp
 * @brief Defined a function that binds RTObject to Naming server
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2022
 *     Nobuhiko Miyamoto
 *     Software Platform Research Team,
 *     Industrial Cyber-Physical Systems Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */
#include "RTObjectBinding.h"

namespace RTM
{
  /*!
   * @if jp
   *
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   * @else
   *
   * @brief Constructor
   *
   * Constructor
   *
   * @endif
   */
  RTObjectBinding::RTObjectBinding() : ObjectBinding()
  {}
  /*!
   * @if jp
   *
   * @brief コンストラクタ
   *
   * @param n バインドするパス
   * @param t バインディング型
   * @param o CORBAオブジェクト
   * @param nct ネーミングコンテキスト
   *
   * @else
   *
   * @brief Constructor
   *
   * @param n 
   * @param t 
   * @param o 
   * @param nct 
   *
   * @endif
   */
  RTObjectBinding::RTObjectBinding(const CosNaming::Name& n, CosNaming::BindingType t,
      CORBA::Object_ptr o, NamingContext* nct)
      : ObjectBinding(n, t, o, nct)
  {
    m_rtobj = RTC::RTObject::_narrow(o);
  }
  /*!
   * @if jp
   *
   * @brief コピーコンストラクタ
   *
   * @param obj コピー元のオブジェクト
   *
   * @else
   *
   * @brief Copy Constructor
   *
   * @param obj 
   *
   * @endif
   */
  RTObjectBinding::RTObjectBinding(const ObjectBinding &obj) : ObjectBinding(obj)
  {
  }
  /*!
   * @if jp
   *
   * @brief 更新処理
   * 未実装
   * 
   *
   * @else
   *
   * @brief 
   *
   *
   * @endif
   */
  void RTObjectBinding::update()
  {
  }

} // namespace RTM

