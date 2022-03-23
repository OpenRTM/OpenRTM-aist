// -*- C++ -*-
/*!
 * @file RTObjectBinding.h
 * @brief Defined a function that binds Manager to Naming server
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

#ifndef RTC_MANAGERBINDING_H
#define RTC_MANAGERBINDING_H

#include <rtm/idl/ManagerSkel.h>
#include "ObjectBinding.h"

namespace RTM
{
  /*!
   * @if jp
   * @class ManagerBinding クラス
   * @brief ManagerBinding クラス
   * ネームサービスに登録するManager、パス、ネーミングコンテキストを保持し、
   * 情報の取得関数や終了関数を提供する
   * ネームサービスに"rtc"というkindで登録された場合にRTObjectとして扱う
   *
   * @else
   *
   * @class ManagerBinding class
   * @brief ManagerBinding class
   *
   * @endif
   */
  class ManagerBinding : ObjectBinding {


  private:
    RTM::Manager_var m_mgr;

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
    ManagerBinding();
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
    ManagerBinding(const CosNaming::Name& n, CosNaming::BindingType t,
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
    ManagerBinding(const ObjectBinding &obj);
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
    ~ManagerBinding() override = default;
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
    void update() override;
  };
} // namespace RTM

#endif
