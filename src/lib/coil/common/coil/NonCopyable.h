// -*- C++ -*-
/*!
 * @file  NonCopyable.h
 * @brief Non Copyable mixin class
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


#ifndef COIL_NONCOPYABLE_H
#define COIL_NONCOPYABLE_H

namespace coil
{
  /*!
   * @if jp
   * @class NonCopyable
   * @brief コピー禁止ミックスイン
   *
   * 対象クラスのオブジェクトのコピーを禁止する。コピーを禁止したいクラ
   * スでは、NonCopyableをprivate継承することでオブジェクトのコピーを禁
   * 止することができる。
   *
   * -例:
   * class CopyProhibitedClass : private NonCopyable {};
   *
   * @else
   *
   * @class NonCopyable
   * @brief Non-copyable Mixin
   *
   * This mix-in class prevents objects of a class from being
   * copy-constructed or assigned to each other. User can prohibit the
   * class copying by inheriting from NonCopyable class as a private
   * base class.
   *
   * -example:
   * class CopyProhibitedClass : private NonCopyable {};
   *
   * @endif
   */
  class NonCopyable
  {
  protected:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    NonCopyable() {}

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * デストラクタ。
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~NonCopyable() {}
  private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
  };

  /*!
   * @if jp
   * @class NonCopyableCRTP
   * @brief コピー禁止ミックスイン(CRTP版)
   *
   * 対象クラスのオブジェクトのコピーを禁止する。コピーを禁止したいクラ
   * スでは、NonCopyableをprivate継承することでオブジェクトのコピーを禁
   * 止することができる。このCRTP (Curiously Recursive Template
   * Pattern) 版は、空の基底クラスに対する最適化 (Empty Base
   * Optimization) を行わせたい場合に利用する。
   *
   * -例:
   * struct A : NonCopyableCRTP<A> {};
   * struct B : NonCopyableCRTP<B> {};
   * struct C: A, B {};
   *
   * @else
   *
   * @class NonCopyable
   * @brief Non-copyable Mixin
   *
   * This mix-in class prevents objects of a class from being
   * copy-constructed or assigned to each other. User can prohibit the
   * class copying by inheriting from NonCopyable class as a private
   * base class.　The CRTP (Curiously Recursive Template Pattern)
   * version would be used for empty base optimization for
   * multipe-inherited.
   *
   * -example:
   * class CopyProhibitedClass : private NonCopyable {};
   *
   * @endif
   */
  template <class T>
  class NonCopyableCRTP
  {
  protected:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    NonCopyableCRTP() {}

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * デストラクタ。
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~NonCopyableCRTP() {}
  private:
    NonCopyableCRTP(const NonCopyableCRTP &);
    T & operator=(const T &);
  };
} // namespace coil

#endif  // COIL_NONCOPYABLE_H
