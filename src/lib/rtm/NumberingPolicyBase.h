// -*- C++ -*-
/*!
 * @file NumberingPolicyBase.h
 * @brief Object numbering policy base class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_NUMBERINGPOLICYBASE_H
#define RTC_NUMBERINGPOLICYBASE_H

#include <string>
#include <vector>
#include <coil/Factory.h>

namespace RTM
{
  /*!
   * @if jp
   *
   * @class NumberingPolicyBase
   * @brief オブジェクト生成時ネーミング・ポリシー(命名規則)管理用抽象クラス
   *
   * オブジェクトを生成する際のネーミング・ポリシー(命名規則)を管理するための
   * 抽象インターフェースクラス。
   * 具象クラスは、以下の純粋仮想関数の実装を提供しなければならない。
   * - onCreate() : オブジェクト生成時の名称作成
   * - onDelete() : オブジェクト削除時の名称解放
   *
   * @since 1.2
   *
   * @else
   * @class NumberingPolicy
   * @brief Abstruct class for naming policy management when creating objects
   *
   * This is the abstract interface class to manage the naming policy when
   * creating objects.
   * Concrete classes must implement the following pure virtual functions.
   * - onCreate() : Create the name when creating objects.
   * - onDelete() : Delete the name when deleting objects.
   *
   * @since 0.4.0
   *
   * @endif
   */
  class NumberingPolicyBase
  {
  public:
    /*!
     * @if jp
     * @brief オブジェクト未発見例外処理用構造体
     * @else
     * @brief The structures for exception handling when object was not
     *        found.
     * @endif
     */
    struct ObjectNotFound {};
    
    /*!
     * @if jp
     *
     * @brief 仮想デストラクタ
     * 
     * @else
     *
     * @brief Virtual destractor
     *
     * @endif
     */
    virtual ~NumberingPolicyBase() {}
    
    /*!
     * @if jp
     *
     * @brief オブジェクト生成時の名称作成
     *
     * オブジェクト生成時の名称を生成するための純粋仮想関数
     * 
     * @param obj 名称生成対象オブジェクト
     *
     * @return 生成したオブジェクト名称
     *
     * @else
     *
     * @brief Create the name when creating objects
     *
     * Pure virtual function to create the name when creating objects.
     * 
     * @param obj The target object for the creation
     *
     * @return Name of the created object
     *
     * @endif
     */
    virtual std::string onCreate(void* obj) = 0;
    
    /*!
     * @if jp
     *
     * @brief オブジェクト削除時の名称解放
     *
     * オブジェクト削除時に名称を解放するための純粋仮想関数
     * 
     * @param obj 名称解放対象オブジェクト
     *
     * @else
     *
     * @brief Delete the name when deleting objects
     *
     * Pure virtual function to delete the name when deleting object.
     * 
     * @param obj The target object of the delete
     *
     * @endif
     */
    virtual void onDelete(void* obj) = 0;
  };
  
  typedef coil::GlobalFactory<NumberingPolicyBase> NumberingPolicyFactory;
  
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  EXTERN template class DLL_PLUGIN coil::GlobalFactory<NumberingPolicyBase>;
#endif
  
} // namespace RTM
#endif // RTC_NUMBERINGPOLICYBASE_H
