// -*- C++ -*-
/*!
 * @file ObjectBinding.h
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

#ifndef RTC_OBJECTBINDING_H
#define RTC_OBJECTBINDING_H

#include "NamingContext.h"

namespace RTM
{

  /*!
   * @if jp
   * @class ObjectBinding クラス
   * @brief ObjectBinding クラス
   * ネームサービスに登録するCORBAオブジェクト、パス、ネーミングコンテキストを保持し、
   * 情報の取得関数や終了関数を提供する
   *
   * @else
   *
   * @class ObjectBinding class
   * @brief ObjectBinding class
   *
   * @endif
   */
  class ObjectBinding {


  protected:
    CosNaming::Binding m_binding;
    CORBA::Object_var m_object;

    NamingContext* m_nc;

  public:
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
    ObjectBinding();
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
    ObjectBinding(const CosNaming::Name& n, CosNaming::BindingType t,
      CORBA::Object_ptr o, NamingContext* nct);
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
    ObjectBinding(const ObjectBinding &obj);
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
    virtual ~ObjectBinding();
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
    bool operator==(const ObjectBinding& obj) const;
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
    const CosNaming::Binding& get_binding() const;
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
    CORBA::Object_ptr get_object();
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
    void destroy();
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
    virtual void update();

  };
} // namespace RTM

#endif
