// -*- C++ -*-
/*!
 * @file ObjectBinding.cpp
 * @brief Defined a function that binds CORBA object to Naming server
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

#include "ObjectBinding.h"
#include <iostream>

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
  ObjectBinding::ObjectBinding() : m_nc(nullptr)
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
  ObjectBinding::ObjectBinding(const CosNaming::Name& n, CosNaming::BindingType t,
      CORBA::Object_ptr o, NamingContext* nct)
  {
    m_binding.binding_name = n;
    m_binding.binding_type = t;

    m_object = CORBA::Object::_duplicate(o);
    m_nc = nct;
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
  ObjectBinding::ObjectBinding(const ObjectBinding &obj)
  {
    m_binding = obj.m_binding;
    m_object = CORBA::Object::_duplicate(obj.m_object);
    m_nc = obj.m_nc;
  }
  /*!
   * @if jp
   *
   * @brief デストラクタ
   *
   * デストラクタ
   *
   * @else
   *
   * @brief Destructor
   *
   * Destructor
   *
   * @endif
   */
  ObjectBinding::~ObjectBinding()
  {
  }
  /*!
   * @if jp
   *
   * @brief 等価演算子
   *
   * @param obj 比較対象のオブジェクト
   * @return true：等価
   *
   * @else
   *
   * @brief 
   *
   * @param obj 
   * @return
   *
   * @endif
   */
  bool ObjectBinding::operator==(const ObjectBinding& obj) const
  {
    if(m_binding.binding_name.length()==obj.m_binding.binding_name.length() && 
        m_binding.binding_type==obj.m_binding.binding_type && 
        m_object->_is_equivalent(obj.m_object))
    {
      for(unsigned int i=0;i < m_binding.binding_name.length();i++)
      {
        if((strcmp(m_binding.binding_name[i].id, obj.m_binding.binding_name[i].id)!=0) ||
          (strcmp(m_binding.binding_name[i].kind, obj.m_binding.binding_name[i].kind)!=0))
        {
          return false;
        }
      }
      return true;
    }
    else
    {
      return false;
    }
  }
  /*!
   * @if jp
   *
   * @brief バインディングオブジェクトを取得する
   *
   * @return バインディングオブジェクト
   *
   * @else
   *
   * @brief 
   *
   * @return
   *
   * @endif
   */
  const CosNaming::Binding& ObjectBinding::get_binding() const
  {
    return m_binding;
  }
  /*!
   * @if jp
   *
   * @brief CORBAオブジェクトを取得する
   *
   * @return CORBAオブジェクト
   *
   * @else
   *
   * @brief 
   *
   * @return
   *
   * @endif
   */
  CORBA::Object_ptr ObjectBinding::get_object()
  {
    return m_object.inout();
  }
  /*!
   * @if jp
   *
   * @brief 終了処理
   * ネーミングコンテキストのリストからオブジェクトを削除する
   * 
   *
   * @else
   *
   * @brief 
   *
   *
   * @endif
   */
  void ObjectBinding::destroy()
  {
    m_nc->remove_object(this);
  }
  /*!
   * @if jp
   *
   * @brief 更新処理
   * 個別のオブジェクトで更新処理を実装したい場合、
   * 派生クラスでオーバーライドして利用する。
   * 
   *
   * @else
   *
   * @brief 
   *
   *
   * @endif
   */
  void ObjectBinding::update()
  {
  }

} // namespace RTM

