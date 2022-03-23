// -*- C++ -*-
/*!
 * @file RTObjectBinding.h
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

#ifndef RTC_RTOBJECTBINDING_H
#define RTC_RTOBJECTBINDING_H

#include <rtm/RTC.h>
#include "ObjectBinding.h"

namespace RTM
{
  /*!
   * @if jp
   * @class RTObjectBinding クラス
   * @brief RTObjectBinding クラス
   * ネームサービスに登録するRTObject、パス、ネーミングコンテキストを保持し、
   * 情報の取得関数や終了関数を提供する
   * ネームサービスに"rtc"というkindで登録された場合にRTObjectとして扱う
   *
   * @else
   *
   * @class RTObjectBinding class
   * @brief RTObjectBinding class
   *
   * @endif
   */
  class RTObjectBinding : ObjectBinding {


  private:
    RTC::RTObject_var m_rtobj;

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
    RTObjectBinding();
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
    RTObjectBinding(const CosNaming::Name& n, CosNaming::BindingType t,
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
    RTObjectBinding(const ObjectBinding &obj);
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
    ~RTObjectBinding() override = default;
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
