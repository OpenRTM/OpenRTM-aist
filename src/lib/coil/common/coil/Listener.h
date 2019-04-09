// -*- C++ -*-
/*!
 * @file Listener.h
 * @brief Listener class
 * @date $Date: 2007-12-31 03:08:04 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2007
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: Listener.h 826 2008-08-26 08:13:39Z n-ando $
 *
 */

#ifndef Listener_h
#define Listener_h


/*!
 * @if jp
 * @class ListenerBase
 * @brief ListenerBase クラス
 *
 * タイマーに登録するリスナー用抽象インターフェースクラス。
 *
 * @since 0.4.0
 *
 * @else
 * @class ListenerBase
 * @brief ListenerBase class
 *
 * This is the abstract interface for Listener that registers
 * to the timer
 *
 * @since 0.4.0
 *
 * @endif
 */
class ListenerBase
{
public:
  /*!
   * @if jp
   * @brief デストラクタ
   *
   * デストラクタ
   *
   * @else
   * @brief Destructor
   *
   * Destructor
   *
   * @endif
   */
  virtual ~ListenerBase() {}

  /*!
   * @if jp
   * @brief コールバック処理
   *
   * コールバック処理用純粋仮想関数
   *
   * @else
   * @brief Callback
   *
   * Pure virtual function for callback-handling
   *
   * @endif
   */
  virtual void invoke() = 0;
};

/*!
 * @if jp
 * @class ListenerObject
 * @brief ListenerObject クラス
 *
 * タイマーに登録するリスナー用基底クラス。
 *
 * @since 0.4.0
 *
 * @else
 * @class ListenerObject
 * @brief ListenerObject class
 *
 * This is a base class for Listener that registers to the timer
 *
 * @since 0.4.0
 *
 * @endif
 */
template <class ListenerClass,
          class CallbackFunc = void (ListenerClass::*)()>
class ListenerObject
  : public ListenerBase
{
public:
  /*!
   * @if jp
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   * @param obj リスナーオブジェクト
   * @param cbf コールバック用関数
   *
   * @else
   * @brief Constructor
   *
   * Constructor
   *
   * @param obj Listener Object
   * @param cbf Callback Function
   *
   * @endif
   */
  ListenerObject(ListenerClass* obj, CallbackFunc cbf)
    : m_obj(obj), m_cbf(cbf)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   *
   * デストラクタ
   *
   * @else
   * @brief Destructor
   *
   * Destructor
   *
   * @endif
   */
  ~ListenerObject() override
  {
  }

  /*!
   * @if jp
   * @brief コールバック用処理
   *
   * コールバック処理用関数
   *
   * @else
   * @brief Callback
   *
   * Callback function
   *
   * @endif
   */
  void invoke() override
  {
    (m_obj->*m_cbf)();
  }

private:
  ListenerClass* m_obj;
  CallbackFunc m_cbf;
};

/*!
 * @if jp
 * @class ListenerFunc
 * @brief ListenerFunc クラス
 *
 * コールバック用オブジェクト。
 *
 * @since 0.4.0
 *
 * @else
 * @class ListenerFunc
 * @brief ListenerFunc class
 *
 * Callback object
 *
 * @since 0.4.0
 *
 * @endif
 */
typedef void (*CallbackFunc)();
class ListenerFunc
  : public ListenerBase
{
public:
  /*!
   * @if jp
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   * @param cbf コールバック用関数
   *
   * @else
   * @brief Constructor
   *
   * Constructor
   *
   * @param cbf Callback Function
   *
   * @endif
   */
  explicit ListenerFunc(CallbackFunc cbf)
    : m_cbf(cbf)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   *
   * デストラクタ
   *
   * @else
   * @brief Destructor
   *
   * Destructor
   *
   * @endif
   */
  ~ListenerFunc() override {}

  /*!
   * @if jp
   * @brief コールバック処理
   *
   * コールバック処理用関数
   *
   * @else
   * @brief Callback
   *
   * Callback function
   *
   * @endif
   */
  void invoke() override
  {
    (*m_cbf)();
  }

private:
  CallbackFunc m_cbf;
};

#endif  // Listener_h
