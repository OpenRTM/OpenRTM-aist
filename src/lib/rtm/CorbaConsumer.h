// -*- C++ -*-
/*!
 * @file CorbaConsumer.h
 * @brief CORBA Consumer class
 * @date $Date: 2007-12-31 03:08:02 $
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
 * $Id$
 *
 */

#ifndef RTC_CORBACONSUMER_H
#define RTC_CORBACONSUMER_H

#include <utility>
#include <iostream>
#ifdef ORB_IS_MICO
#include <CORBA.h>
#endif
#ifdef ORB_IS_OMNIORB
#ifdef WIN32
#pragma warning( disable : 4267 )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
#endif  // WIN32
#include <omniORB4/CORBA.h>
#ifdef WIN32
#pragma warning( default : 4267 )
#pragma warning( default : 4290 )
#pragma warning( default : 4311 )
#pragma warning( default : 4312 )
#endif  // WIN32
#endif
#ifdef ORB_IS_ORBACUS
#include <OB/CORBA.h>
#endif
#ifdef ORB_IS_ORBIT2
#include <orbitcpp/orb-cpp/orbitcpp.h>
#endif
#ifdef ORB_IS_ORBIX
#include <CORBA.h>
#endif
#ifdef ORB_IS_TAO
#include <tao/corba.h>
#endif

/*!
 * @if jp
 * @namespace RTC
 *
 * @brief RTコンポーネント
 *
 * @else
 *
 * @namespace RTC
 *
 * @brief RT-Component
 *
 * @endif
 */
namespace RTC
{
  /*!
   * @if jp
   * @class CorbaConsumerBase
   *
   * @brief オブジェクトリファレンスを保持するプレースホルダ基底クラス
   *
   * 通信手段として CORBA を選択した場合のコンシューマ実装のための基底クラス
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class ConsumerBase
   *
   * @brief Placeholder base class to hold remote object reference.
   *
   * A base class for consumer implementation when chose CORBA
   * as a communication tool.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class CorbaConsumerBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Consructor
     *
     * @endif
     */
    CorbaConsumerBase() {}

    /*!
     * @if jp
     *
     * @brief コピーコンストラクタ
     *
     * @param x コピー元のCorbaConsumerBaseオブジェクト
     *
     * @else
     *
     * @brief Copy Consructor
     *
     * @param x A CorbaConsumerBase object of copy source
     *
     * @endif
     */
    CorbaConsumerBase(const CorbaConsumerBase& x)
#ifdef ORB_IS_ORBEXPRESS
      : m_objref(CORBA::Object::_duplicate(x.m_objref.in()))
#else
      : m_objref(CORBA::Object::_duplicate(x.m_objref))
#endif
    {
    }

    /*!
     * @if jp
     *
     * @brief 代入演算子
     *
     * @param x 代入元
     *
     * @return 代入結果
     *
     * @else
     *
     * @brief Assignment operator
     *
     * @param x Copy source.
     *
     * @return An assignment result
     *
     * @endif
     */
    CorbaConsumerBase& operator=(const CorbaConsumerBase& x)
    {
      CorbaConsumerBase tmp(x);
      tmp.swap(*this);
      return *this;
    }

    /*!
     * @if jp
     *
     * @brief swap関数
     *
     * @param x 代入元
     *
     * @else
     *
     * @brief swap function
     *
     * @param x Copy source.
     *
     * @endif
     */
    void swap(CorbaConsumerBase& x)
    {
      CORBA::Object_var tmpref = x.m_objref;
      x.m_objref = this->m_objref;
      this->m_objref = tmpref;
    }

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * @else
     *
     * @brief Virtual destructor
     *
     * @endif
     */
    virtual ~CorbaConsumerBase()
    {
      releaseObject();
    }

    /*!
     * @if jp
     *
     * @brief CORBAオブジェクトをセットする
     *
     * 与えられたオブジェクトリファレンスは、ConsumerBase オブジェクト内に
     * CORBA::Object_var 型として保持される。
     * _var 型変数を引数に渡す場合は var.in() を渡すこと。
     *
     * @param obj CORBA オブジェクトのリファレンス
     *
     * @return obj が nil リファレンスの場合 false を返す。
     *
     * @else
     *
     * @brief Set CORBA Object
     *
     * The given CORBA Object is held as CORBA::Object_var type in ConsumerBase
     * object.
     *
     * @param obj Object reference of CORBA object
     *
     * @return If obj is nil reference, it returns false.
     *
     * @endif
     */

    virtual bool setObject(CORBA::Object_ptr obj)
    {
      if (CORBA::is_nil(obj))
        {
          return false;
        }
      m_objref = CORBA::Object::_duplicate(obj);
      return true;
    }

    /*!
     * @if jp
     *
     * @brief CORBAオブジェクトを取得する
     *
     * ConsumerBase オブジェクト内に CORBA::Object_var 型として保持されている
     * オブジェクトリファレンスを取得する。
     * 呼び出し側はvar型変数で受けるか、使用後CORBA::release()を呼び出して
     * 参照カウントをデクリメントすること。
     *
     * @return obj CORBA オブジェクトのリファレンス
     *
     * @else
     *
     * @brief Get CORBA Object
     *
     * Get the object reference held as CORBA::Object_var type in ConsumerBase
     * object.
     *
     * @return Object reference of CORBA object
     *
     * @endif
     */
    virtual CORBA::Object_ptr getObject()
    {
      return m_objref;
    }

    /*!
     * @if jp
     *
     * @brief CORBAオブジェクトの設定をクリアする
     *
     * 設定されている CORBA オブジェクトをクリアする。
     * CORBAオブジェクトそのものに対しては何も操作しない。
     *
     * @else
     *
     * @brief Clear CORBA object setting
     *
     * Clear CORBA object which is set.
     * Operate nothing for CORBA object itself.
     *
     * @endif
     */
    virtual void releaseObject()
    {
      m_objref = CORBA::Object::_nil();
    }

  protected:
    /*!
     * @if jp
     * @brief 設定された CORBA オブジェクト
     * @else
     * @brief CORBA object which is set.
     * @endif
     */
    CORBA::Object_var m_objref;
  };

  /*!
   * @if jp
   *
   * @class CorbaConsumer
   * @brief オブジェクトリファレンスを保持するプレースホルダテンプレートクラス
   *
   * テンプレート引数で与えられた型のCORBAオブジェクトを保持する。
   * オブジェクトがセットされたときに、与えられた型で narrow されるので、
   * _ptr() で取得するリファレンスは、narrow 済みのリファレンスである。
   * 内部的な使用のために、_ptr 型, _var型も同時にテンプレート引数として
   * 与える必要がある。(下記注意事項参照)
   * <br>
   * 注意: ObjectTypePtr = ObjectType::_ptr_type としているか、
   *       _ptr_type は標準では規定されていない。
   *       ただし、omniORB, TAO, MICO では、プロキシクラス内部で、
   *       Type_ptr 型を _ptr_type に typedef しているので、
   *       テンプレートの第2引数を指定しなくてもコンパイルは通る。
   *
   * @param ObjectType このホルダが保持するオブジェクトの型
   * @param ObjectTypePtr このホルダが保持する _ptr 型
   * @param ObjectTypeVar このホルダが保持する _var 型
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class Consumer
   * @brief Placeholder template class to hold remote object reference.
   *
   * This class holds a type of object that given by template parameter.
   * For internal use, _ptr type and _var type should be given as template
   * parameter. (Please refere the following notation.)
   *
   * Note: ObjectTypePtr's default value is defined as ObjectType::_ptr_type,
   *       although _ptr_type is not defined as normative type.
   *       However, omniORB, TAO, MICO, ORBit, ORBacus define _ptr_type and
   *       _var_type as _ptr type and _var type in stub code.
   *       Usually, you don't need to specify 2nd and 3rd template arguments.
   *
   * @since 0.4.0
   *
   * @endif
   */
  template <class ObjectType,
            typename ObjectTypePtr = typename ObjectType::_ptr_type,
            typename ObjectTypeVar = typename ObjectType::_var_type>
  class CorbaConsumer
    : public CorbaConsumerBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * @else
     *
     * @brief Consructor
     *
     * @endif
     */
    CorbaConsumer() {}

    /*!
     * @if jp
     *
     * @brief コピーコンストラクタ
     *
     * @param x コピー元
     *
     * @else
     *
     * @brief Copy constructor
     *
     * @param x Copy source.
     *
     * @endif
     */
    CorbaConsumer(const CorbaConsumer& x)
#ifdef ORB_IS_ORBEXPRESS
      : m_var(ObjectType::_duplicate(x.m_var.in()))
#else
      : m_var(ObjectType::_duplicate(x.m_var))
#endif
    {
    }

    /*!
     * @if jp
     *
     * @brief 代入演算子
     *
     * @param x 代入元
     *
     * @return 代入結果
     *
     * @else
     *
     * @brief Assignment operator
     *
     * @param x Copy source.
     *
     * @return An assignment result
     *
     * @endif
     */
    CorbaConsumer& operator=(const CorbaConsumer& x)
    {
      CorbaConsumer tmp(x);
      tmp.swap(*this);
      return *this;
    }

    void swap(CorbaConsumer& x)
    {
      CorbaConsumerBase::swap(x);
      ObjectTypeVar tmpref = x.m_var;
      x.m_var = this->m_var;
      this->m_var = tmpref;

    }

    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     *
     * @else
     *
     * @brief Virtual destructor
     *
     * @endif
     */
    ~CorbaConsumer() override
    {
      releaseObject();
    }

    /*!
     * @if jp
     * @brief オブジェクトをセットする
     *
     * ConsumerBase のオーバーライド。CORBA::Object_var にオブジェクトをセット
     * するとともに、templateパラメータの型で narrow したオブジェクトを
     * メンバ変数に保持する。
     * _var 型変数を引数に渡す場合は var.in() を渡すこと。
     *
     * @param [in] obj CORBA Objecct
     *
     * @return オブジェクト設定結果
     *         設定対象オブジェクトが null の場合は false が返ってくる
     *
     * @else
     * @brief Set Object
     *
     * Override function of ConsumerBase. This operation set an Object to
     * CORBA:Object_var in the class, and this object is narrowed to
     * given template parameter and stored in the member variable.
     *
     * @param obj CORBA Objecct
     *
     * @return An object setting result.
     *         If target object is null, it returns false.
     *
     * @endif
     */
    bool setObject(CORBA::Object_ptr obj) override
    {
      if (!CorbaConsumerBase::setObject(obj))
        {
          releaseObject();
          return false;  // object is nil
        }

      ObjectTypeVar var = ObjectType::_narrow(m_objref);

      if (CORBA::is_nil(var))
        {
          releaseObject();
          return false;
        }

      m_var = var;
      return true;
    }

    /*!
     * @if jp
     * @brief ObjectType 型のオブジェクトのリファレンスを取得
     *
     * ObjectType に narrow済みのオブジェクトのリファレンスを取得する。
     * オブジェクトリファレンスを使用するには、setObject() でセット済みで
     * なければならない。
     * オブジェクトがセットされていなければ　nil オブジェクトリファレンスが
     * 返される。
     *
     * @return ObjectType に narrow 済みのオブジェクトのリファレンス
     *
     * @else
     * @brief Get Object reference narrowed as ObjectType
     *
     * This operation returns object reference narrowed as ObjectType.
     * To use the returned object reference, reference have to be set by
     * setObject().
     * If object is not set, this operation returns nil object reference.
     *
     * @return The object reference narrowed as ObjectType
     *
     * @endif
     */
    inline ObjectTypePtr _ptr()
    {
      return m_var.inout();
    }

    /*!
     * @if jp
     * @brief ObjectType 型のオブジェクトのリファレンスを取得
     *
     * ObjectType に narrow済みのオブジェクトのリファレンスを取得する。
     * オブジェクトリファレンスを使用するには、setObject() でセット済みで
     * なければならない。
     * オブジェクトがセットされていなければ　nil オブジェクトリファレンスが
     * 返される。
     *
     * @return ObjectType に narrow 済みのオブジェクトのリファレンス
     *
     * @else
     * @brief Get Object reference narrowed as ObjectType
     *
     * This operation returns object reference narrowed as ObjectType.
     * To use the returned object reference, reference have to be set by
     * setObject().
     * If object is not set, this operation returns nil object reference.
     *
     * @return The object reference narrowed as ObjectType
     *
     * @endif
     */
    inline ObjectTypePtr operator->()
    {
      return m_var.inout();
    }

    /*!
     * @if jp
     *
     * @brief CORBAオブジェクトの設定をクリアする
     *
     * 設定されている CORBA オブジェクトをクリアする。
     * CORBAオブジェクトそのものに対しては何も操作しない。
     *
     * @else
     *
     * @brief Clear CORBA object setting
     *
     * Clear CORBA object which is set.
     * Operate nothing for CORBA object itself.
     *
     * @endif
     */
    void releaseObject() override
    {
      CorbaConsumerBase::releaseObject();
      m_var = ObjectType::_nil();
    }

  protected:
    /*!
     * @if jp
     * @brief 設定された CORBA オブジェクト
     * @else
     * @brief CORBA object which has been set.
     * @endif
     */
    ObjectTypeVar m_var;
  };
} // namespace RTC
#endif  // RTC_CORBACONSUMER_H
