// -*- C++ -*-
/*!
 * @file FsmActionListener.h
 * @brief component action listener class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016-2017
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_FSMACTIONLISTENER_H
#define RTC_FSMACTIONLISTENER_H

#include <vector>
#include <utility>
#include <mutex>
#include <array>
#include <rtm/RTC.h>
#include <rtm/idl/RTCSkel.h>
#include <rtm/ConnectorBase.h>
#include <rtm/idl/ExtendedFsmServiceStub.h>

namespace RTC
{
  /*!
   * @if jp
   *
   * FSMコンポーネントに関する種々の振る舞いをフックするためのリスナ定
   * 義。リスナには大きく分けると、
   *
   * - FSMそのものの動作をフックするためのリスナ
   * - FSMに関するメタデータ変更等の動作をフックするためのリスナ
   *
   * の2種類に分けられる。さらに前者は、FSMの状態遷移等のアクションの前
   * 後それぞれをフックするための PreFsmActionListener と
   * PostFsmActionListener の二つがあり、後者は、FSMのProfileの変更をフッ
   * クする FsmProfileListener と FSMの構造 (Structure) の変更をフック
   * する FsmStructureListener の二つに分けられる。以上、以下のFSMに関
   * する以下の4種類のリスナークラス群が提供されている。
   *
   * - PreFsmActionListener
   * - PostFsmActionListener
   * - FsmProfileListner
   * - FsmStructureListener
   *
   * @else
   *
   *
   * @endif
   */

  //============================================================
  /*!
   * @if jp
   * @brief PreFsmActionListener のタイプ
   *
   * PreFsmActionListener には以下のフックポイントが定義されている。こ
   * れらが呼び出されるかどうかは、FSMの実装に依存する。
   *
   * - PRE_ON_INIT:          init 直前
   * - PRE_ON_ENTRY:         entry 直前
   * - PRE_ON_DO:            do 直前
   * - PRE_ON_EXIT:          exit 直前
   * - PRE_ON_STATE_CHANGE:  状態遷移直前
   *
   * @else
   * @brief The types of PreFsmActionListener
   *
   * PreFsmActionListener has the following hook points. If these
   * listeners are actually called or not called are depends on FSM
   * implementations.
   *
   * - PRE_ON_INIT:          just before "init" action
   * - PRE_ON_ENTRY:         just before "entry" action
   * - PRE_ON_DO:            just before "do" action
   * - PRE_ON_EXIT:          just before "exit" action
   * - PRE_ON_STATE_CHANGE:  just before state transition action
   *
   * @endif
   */
  enum class PreFsmActionListenerType : uint8_t
    {
      PRE_ON_INIT,
      PRE_ON_ENTRY,
      PRE_ON_DO,
      PRE_ON_EXIT,
      PRE_ON_STATE_CHANGE,
      PRE_FSM_ACTION_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class PreFsmActionListener クラス
   * @brief PreFsmActionListener クラス
   *
   * PreFsmActionListener クラスは、Fsmのアクションに関するコールバック
   * を実現するリスナーオブジェクトの基底クラスである。FSMのアクション
   * の直前の動作をフックしたい場合、以下の例のように、このクラスを継承
   * したコールバックオブジェクトを定義し、適切なコールバック設定関数か
   * らRTObjectに対してコールバックオブジェクトをセットする必要がある。
   *
   * <pre>
   * class MyListener
   *   : public PreFsmActionListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *
   *   virtual void operator()(const char* state_name)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *     std::cout << "Current state: " state_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * このようにして定義されたリスナクラスは、以下のようにRTObjectに対し
   * て、セットされる。
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addPreFsmActionListener(PreFsmActionListenerType::PRE_ON_STATE_CHANGE,
   *                             new MyListener("init listener"),
   *                             true);
   *    :
   * </pre>
   *
   * 第1引数の "PRE_ON_STATE_CHANGE" は、コールバックをフックするポイン
   * トであり、以下の値を取ることが可能である。なお、すべてのコールバッ
   * クポイントが実装されているとは限らず、これらが呼び出されるかどうか
   * は、FSMの実装に依存する。
   *
   * - PRE_ON_INIT:          init 直前
   * - PRE_ON_ENTRY:         entry 直前
   * - PRE_ON_DO:            do 直前
   * - PRE_ON_EXIT:          exit 直前
   * - PRE_ON_STATE_CHANGE:  状態遷移直前
   *
   * 第2引数はリスナオブジェクトのポインタである。第3引数はオブジェクト
   * 自動削除フラグであり、true の場合は、RTObject削除時に自動的にリス
   * ナオブジェクトが削除される。falseの場合は、オブジェクトの所有権は
   * 呼び出し側に残り、削除は呼び出し側の責任で行わなければならない。
   * RTObject のライフサイクル中にコールバックが必要ならば上記のような
   * 呼び出し方で第3引数を true としておくとよい。逆に、コールバックを
   * 状況等に応じてセットしたりアンセットしたりする必要がある場合は
   * falseとして置き、リスナオブジェクトのポインタをメンバ変数などに保
   * 持しておき、
   * RTObject_impl::addPreFsmActionListener()/removePreFsmActionListener()
   * により、セットとアンセットを管理するといった使い方も可能である。
   *
   * @else
   * @class PreFsmActionListener class
   * @brief PreFsmActionListener class
   *
   * PreFsmActionListener class is a base class for the listener
   * objects which realize callback to hook FSM related pre-actions.
   * To hook execution just before a FSM action, the callback object
   * should be defined as follows, and set to RTObject through
   * appropriate callback set function.
   *
   * <pre>
   * class MyListener
   *   : public PreFsmActionListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *
   *   virtual void operator()(const char* state_name)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *     std::cout << "Current state: " state_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * The listener class defined above is set to RTObject as follows.
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addPreFsmActionListener(PreFsmActionListenerType::PRE_ON_STATE_CHANGE,
   *                             new MyListener("init listener"),
   *                             true);
   *    :
   * </pre>
   *
   * The first argument "PRE_ON_STATE_CHANGE" specifies callback hook
   * point, and the following values are available. Not all the
   * callback points are implemented. It depends on the FSM
   * implementations.
   *
   * - PRE_ON_INIT:          just before "init" action
   * - PRE_ON_ENTRY:         just before "entry" action
   * - PRE_ON_DO:            just before "do" action
   * - PRE_ON_EXIT:          just before "exit" action
   * - PRE_ON_STATE_CHANGE:  just before state transition action
   *
   * The second argument is a pointers to the listener object. The
   * third argument is a flag for automatic object destruction. When
   * "true" is given to the third argument, the given object in second
   * argument is automatically destructed with RTObject. In the "false
   * " case, the ownership of the object is left in the caller side,
   * and then destruction of the object must be done by users'
   * responsibility.
   *
   * It is good for setting "true" as third argument, if the listener
   * object life span is equals to the RTObject's life cycle.  On the
   * otehr hand, if callbacks are required to set/unset depending on
   * its situation, the third argument could be "false".  In that
   * case, listener objects pointers must be stored to member
   * variables, and set/unset of the listener objects shoud be
   * paerformed throguh
   * RTObject_impl::addPreFsmActionListener()/removePreFsmActionListener()
   * functions.
   *
   * @endif
   */
  class PreFsmActionListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief PreFsmActionListenerType を文字列に変換
     *
     * PreFsmActionListenerType を文字列に変換する
     *
     * @param type 変換対象 PreFsmActionListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert PreFsmActionListenerType into the string.
     *
     * Convert PreFsmActionListenerType into the string.
     *
     * @param type The target PreFsmActionListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(PreFsmActionListenerType type)
    {
      static const char* const typeString[] =
        {
          "PRE_ON_INIT",
          "PRE_ON_ENTRY",
          "PRE_ON_DO",
          "PRE_ON_EXIT",
          "PRE_ON_STATE_CHANGE",
          "PRE_FSM_ACTION_LISTENER_NUM"
        };
      if (type < PreFsmActionListenerType::PRE_FSM_ACTION_LISTENER_NUM)
        {
          return typeString[static_cast<uint8_t>(type)];
        }
      return "";
    }

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PreFsmActionListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * PreFsmActionListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for PreFsmActionListener.
     *
     * @endif
     */
    virtual void operator()(const char*) = 0;
  };


  //============================================================
  /*!
   * @if jp
   * @brief PostFsmActionListener のタイプ
   *
   * PostFsmActionListener には以下のフックポイントが定義されている。こ
   * れらが呼び出されるかどうかは、FSMの実装に依存する。
   *
   * - POST_ON_INIT:          init 直後
   * - POST_ON_ENTRY:         entry 直後
   * - POST_ON_DO:            do 直後
   * - POST_ON_EXIT:          exit 直後
   * - POST_ON_STATE_CHANGE:  状態遷移直後
   *
   * @else
   * @brief The types of PostFsmActionListener
   *
   * PostFsmActionListener has the following hook points. If these
   * listeners are actually called or not called are depends on FSM
   * implementations.
   *
   * - POST_ON_INIT:          just after "init" action
   * - POST_ON_ENTRY:         just after "entry" action
   * - POST_ON_DO:            just after "do" action
   * - POST_ON_EXIT:          just after "exit" action
   * - POST_ON_STATE_CHANGE:  just after state transition action
   *
   * @endif
   */
  enum class PostFsmActionListenerType : uint8_t
    {
      POST_ON_INIT,
      POST_ON_ENTRY,
      POST_ON_DO,
      POST_ON_EXIT,
      POST_ON_STATE_CHANGE,
      POST_FSM_ACTION_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class PostFsmActionListener クラス
   * @brief PostFsmActionListener クラス
   *
   * PostFsmActionListener クラスは、Fsmのアクションに関するコールバック
   * を実現するリスナーオブジェクトの基底クラスである。FSMのアクション
   * の直後の動作をフックしたい場合、以下の例のように、このクラスを継承
   * したコールバックオブジェクトを定義し、適切なコールバック設定関数か
   * らRTObjectに対してコールバックオブジェクトをセットする必要がある。
   *
   * <pre>
   * class MyListener
   *   : public PostFsmActionListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *
   *   virtual void operator()(const char* state_name, ReturnCode_t ret)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *     std::cout << "Current state: " state_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * このようにして定義されたリスナクラスは、以下のようにRTObjectに対し
   * て、セットされる。
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addPostFsmActionListener(PostFsmActionListenerType::POST_ON_STATE_CHANGE,
   *                             new MyListener("init listener"),
   *                             true);
   *    :
   * </pre>
   *
   * 第1引数の "POST_ON_STATE_CHANGE" は、コールバックをフックするポイン
   * トであり、以下の値を取ることが可能である。なお、すべてのコールバッ
   * クポイントが実装されているとは限らず、これらが呼び出されるかどうか
   * は、FSMの実装に依存する。
   *
   * - POST_ON_INIT:          init 直後
   * - POST_ON_ENTRY:         entry 直後
   * - POST_ON_DO:            do 直後
   * - POST_ON_EXIT:          exit 直後
   * - POST_ON_STATE_CHANGE:  状態遷移直後
   *
   * 第2引数はリスナオブジェクトのポインタである。第3引数はオブジェクト
   * 自動削除フラグであり、true の場合は、RTObject削除時に自動的にリス
   * ナオブジェクトが削除される。falseの場合は、オブジェクトの所有権は
   * 呼び出し側に残り、削除は呼び出し側の責任で行わなければならない。
   * RTObject のライフサイクル中にコールバックが必要ならば上記のような
   * 呼び出し方で第3引数を true としておくとよい。逆に、コールバックを
   * 状況等に応じてセットしたりアンセットしたりする必要がある場合は
   * falseとして置き、リスナオブジェクトのポインタをメンバ変数などに保
   * 持しておき、
   * RTObject_impl::addPostFsmActionListener()/removePostFsmActionListener()
   * により、セットとアンセットを管理するといった使い方も可能である。
   *
   * @else
   * @class PostFsmActionListener class
   * @brief PostFsmActionListener class
   *
   * PostFsmActionListener class is a base class for the listener
   * objects which realize callback to hook FSM related post-actions.
   * To hook execution just before a FSM action, the callback object
   * should be defined as follows, and set to RTObject through
   * appropriate callback set function.
   *
   * <pre>
   * class MyListener
   *   : public PostFsmActionListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *
   *   virtual void operator()(const char* state_name, ReturnCode\t ret)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *     std::cout << "Current state: " state_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * The listener class defined above is set to RTObject as follows.
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addPostFsmActionListener(PostFsmActionListenerType::POST_ON_STATE_CHANGE,
   *                             new MyListener("init listener"),
   *                             true);
   *    :
   * </pre>
   *
   * The first argument "POST_ON_STATE_CHANGE" specifies callback hook
   * point, and the following values are available. Not all the
   * callback points are implemented. It depends on the FSM
   * implementations.
   *
   * - POST_ON_INIT:          just after "init" action
   * - POST_ON_ENTRY:         just after "entry" action
   * - POST_ON_DO:            just after "do" action
   * - POST_ON_EXIT:          just after "exit" action
   * - POST_ON_STATE_CHANGE:  just after state transition action
   *
   * The second argument is a pointers to the listener object. The
   * third argument is a flag for automatic object destruction. When
   * "true" is given to the third argument, the given object in second
   * argument is automatically destructed with RTObject. In the "false
   * " case, the ownership of the object is left in the caller side,
   * and then destruction of the object must be done by users'
   * responsibility.
   *
   * It is good for setting "true" as third argument, if the listener
   * object life span is equals to the RTObject's life cycle.  On the
   * otehr hand, if callbacks are required to set/unset depending on
   * its situation, the third argument could be "false".  In that
   * case, listener objects pointers must be stored to member
   * variables, and set/unset of the listener objects shoud be
   * paerformed throguh
   * RTObject_impl::addPostFsmActionListener()/removePostFsmActionListener()
   * functions.
   *
   * @endif
   */
  class PostFsmActionListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief PostFsmActionListenerType を文字列に変換
     *
     * PostFsmActionListenerType を文字列に変換する
     *
     * @param type 変換対象 PostFsmActionListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert PostFsmActionListenerType into the string.
     *
     * Convert PostFsmActionListenerType into the string.
     *
     * @param type The target PostFsmActionListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(PostFsmActionListenerType type)
    {
      static const char* const typeString[] =
        {
          "POST_ON_INIT",
          "POST_ON_ENTRY",
          "POST_ON_DO",
          "POST_ON_EXIT",
          "POST_ON_STATE_CHANGE",
          "POST_FSM_ACTION_LISTENER_NUM"
        };
      if (type < PostFsmActionListenerType::POST_FSM_ACTION_LISTENER_NUM)
        {
          return typeString[static_cast<uint8_t>(type)];
        }
      return "";
    }

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PostFsmActionListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * PostFsmActionListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for PostFsmActionListener.
     *
     * @endif
     */
    virtual void operator()(const char* state,
                            ReturnCode_t ret) = 0;
  };

  //============================================================
  /*!
   * @if jp
   * @brief FsmProfileListener のタイプ
   *
   * - SET_FSM_PROFILE       : FSM Profile設定時
   * - GET_FSM_PROFILE       : FSM Profile取得時
   * - ADD_FSM_STATE         : FSMにStateが追加された
   * - REMOVE_FSM_STATE      : FSMからStateが削除された
   * - ADD_FSM_TRANSITION    : FSMに遷移が追加された
   * - REMOVE_FSM_TRANSITION : FSMから遷移が削除された
   * - BIND_FSM_EVENT        : FSMにイベントがバインドされた
   * - UNBIND_FSM_EVENT      : FSMにイベントがアンバインドされた
   *
   * @else
   * @brief The types of FsmProfileListener
   *
   * - SET_FSM_PROFILE       : Setting FSM Profile
   * - GET_FSM_PROFILE       : Getting FSM Profile
   * - ADD_FSM_STATE         : A State added to the FSM
   * - REMOVE_FSM_STATE      : A State removed from FSM
   * - ADD_FSM_TRANSITION    : A transition added to the FSM
   * - REMOVE_FSM_TRANSITION : A transition removed from FSM
   * - BIND_FSM_EVENT        : An event bounded to the FSM
   * - UNBIND_FSM_EVENT      : An event unbounded to the FSM
   *
   * @endif
   */
  enum class FsmProfileListenerType : uint8_t
    {
      SET_FSM_PROFILE,
      GET_FSM_PROFILE,
      ADD_FSM_STATE,
      REMOVE_FSM_STATE,
      ADD_FSM_TRANSITION,
      REMOVE_FSM_TRANSITION,
      BIND_FSM_EVENT,
      UNBIND_FSM_EVENT,
      FSM_PROFILE_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class FsmProfileListener クラス
   * @brief FsmProfileListener クラス
   *
   * FsmProfileListener クラスは、FSMのProfileに関連したアクションのコー
   * ルバックを実現するリスナーオブジェクトの基底クラスである。FSM
   * Profileのアクションの動作をフックしたい場合、以下の例のように、こ
   * のクラスを継承したコールバックオブジェクトを定義し、適切なコールバッ
   * ク設定関数からRTObjectに対してコールバックオブジェクトをセットする
   * 必要がある。
   *
   * <pre>
   * class MyListener
   *   : public FsmProfileListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *
   *   virtual void operator()(const ::RTC::FsmProfile& fsmprof)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * このようにして定義されたリスナクラスは、以下のようにRTObjectに対し
   * て、セットされる。
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addFsmProfileListener(FsmProfileListenerType::SET_FSM_PROFILE,
   *                           new MyListener("prof listener"),
   *                           true);
   *    :
   * </pre>
   *
   * 第1引数の "SET_FSM_PROFILE" は、コールバックをフックするポイン
   * トであり、以下の値を取ることが可能である。なお、すべてのコールバッ
   * クポイントが実装されているとは限らず、これらが呼び出されるかどうか
   * は、FSMサービスの実装に依存する。
   *
   * - SET_FSM_PROFILE       : FSM Profile設定時
   * - GET_FSM_PROFILE       : FSM Profile取得時
   * - ADD_FSM_STATE         : FSMにStateが追加された
   * - REMOVE_FSM_STATE      : FSMからStateが削除された
   * - ADD_FSM_TRANSITION    : FSMに遷移が追加された
   * - REMOVE_FSM_TRANSITION : FSMから遷移が削除された
   * - BIND_FSM_EVENT        : FSMにイベントがバインドされた
   * - UNBIND_FSM_EVENT      : FSMにイベントがアンバインドされた
   *
   * 第2引数はリスナオブジェクトのポインタである。第3引数はオブジェクト
   * 自動削除フラグであり、true の場合は、RTObject削除時に自動的にリス
   * ナオブジェクトが削除される。falseの場合は、オブジェクトの所有権は
   * 呼び出し側に残り、削除は呼び出し側の責任で行わなければならない。
   * RTObject のライフサイクル中にコールバックが必要ならば上記のような
   * 呼び出し方で第3引数を true としておくとよい。逆に、コールバックを
   * 状況等に応じてセットしたりアンセットしたりする必要がある場合は
   * falseとして置き、リスナオブジェクトのポインタをメンバ変数などに保
   * 持しておき、addFsmProfileListener()/removeFsmProfileListener() に
   * より、セットとアンセットを管理するといった使い方も可能である。
   *
   * @else
   * @class FsmProfileListener class
   * @brief FsmProfileListener class
   *
   * FsmProfileListener class is a base class for the listener
   * objects which realize callback to hook FSM Profile related actions.
   * To hook execution just before a FSM profile action, the callback object
   * should be defined as follows, and set to RTObject through
   * appropriate callback set function.
   *
   * <pre>
   * class MyListener
   *   : public FsmProfileListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *
   *   virtual void operator()(const ::RTC::FsmProfile& fsmprof)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * The listener class defined above is set to RTObject as follows.
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addFsmProfileListener(FsmProfileListenerType::SET_FSM_PROFILE,
   *                           new MyListener("prof listener"),
   *                           true);
   *    :
   * </pre>
   *
   * The first argument "SET_FSM_PROFILE" specifies callback hook
   * point, and the following values are available. Not all the
   * callback points are implemented. It depends on the FSM service
   * implementations.
   *
   * - SET_FSM_PROFILE       : Setting FSM Profile
   * - GET_FSM_PROFILE       : Getting FSM Profile
   * - ADD_FSM_STATE         : A State added to the FSM
   * - REMOVE_FSM_STATE      : A State removed from FSM
   * - ADD_FSM_TRANSITION    : A transition added to the FSM
   * - REMOVE_FSM_TRANSITION : A transition removed from FSM
   * - BIND_FSM_EVENT        : An event bounded to the FSM
   * - UNBIND_FSM_EVENT      : An event unbounded to the FSM
   *
   * The second argument is a pointers to the listener object. The
   * third argument is a flag for automatic object destruction. When
   * "true" is given to the third argument, the given object in second
   * argument is automatically destructed with RTObject. In the "false
   * " case, the ownership of the object is left in the caller side,
   * and then destruction of the object must be done by users'
   * responsibility.
   *
   * It is good for setting "true" as third argument, if the listener
   * object life span is equals to the RTObject's life cycle.  On the
   * otehr hand, if callbacks are required to set/unset depending on
   * its situation, the third argument could be "false".  In that
   * case, listener objects pointers must be stored to member
   * variables, and set/unset of the listener objects shoud be
   * paerformed throguh
   * addFsmProfileListener()/removeFsmProfileListener() functions.
   *
   * @endif
   */
  class FsmProfileListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief FsmProfileListenerType を文字列に変換
     *
     * FsmProfileListenerType を文字列に変換する
     *
     * @param type 変換対象 FsmProfileListenerType
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert FsmProfileListenerType into the string.
     *
     * Convert FsmProfileListenerType into the string.
     *
     * @param type The target FsmProfileListenerType for transformation
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(FsmProfileListenerType type)
    {
      static const char* const typeString[] =
        {
          "SET_FSM_PROFILE",
          "GET_FSM_PROFILE",
          "ADD_FSM_STATE",
          "REMOVE_FSM_STATE",
          "ADD_FSM_TRANSITION",
          "REMOVE_FSM_TRANSITION",
          "BIND_FSM_EVENT",
          "UNBIND_FSM_EVENT",
          "FSM_PROFILE_LISTENER_NUM"
        };
      if (type < FsmProfileListenerType::FSM_PROFILE_LISTENER_NUM)
        {
          return typeString[static_cast<uint8_t>(type)];
        }
      return "";
    }

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~FsmProfileListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * FsmProfileListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for FsmProfileListener
     *
     * @endif
     */
    virtual void operator()(const ::RTC::FsmProfile& fsmprof) = 0;
  };


  //============================================================
  /*!
   * @if jp
   * @brief FsmStructureListener のタイプ
   *
   * - SET_FSM_STRUCTURE: FSM構造の設定
   * - GET_FSM_STRUCTURE: FSM構造の取得
   *
   * @else
   * @brief The types of FsmStructureListener
   *
   * - SET_FSM_STRUCTURE: Setting FSM structure
   * - GET_FSM_STRUCTURE: Getting FSM structure
   *
   * @endif
   */
  enum class FsmStructureListenerType : uint8_t
    {
      SET_FSM_STRUCTURE,
      GET_FSM_STRUCTURE,
      FSM_STRUCTURE_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class FsmStructureListener クラス
   * @brief FsmStructureListener クラス
   *
   * FsmStructureListener クラスは、FSM Structureのアクションに関するコー
   * ルバックを実現するリスナーオブジェクトの基底クラスである。FSM
   * Structure のアクションの直後の動作をフックしたい場合、以下の例のよ
   * うに、このクラスを継承したコールバックオブジェクトを定義し、適切な
   * コールバック設定関数からRTObjectに対してコールバックオブジェクトを
   * セットする必要がある。
   *
   * <pre>
   * class MyListener
   *   : public FsmStructureListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *   virtual void operator()(::RTC::FsmStructure& pprof)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * このようにして定義されたリスナクラスは、以下のようにRTObjectに対し
   * て、セットされる。
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addFsmStructureListener(FsmStructureListenerType::SET_FSM_STRUCTURE,
   *                             new MyListener("set structure listener"),
   *                             true);
   *    :
   * </pre>
   *
   * 第1引数の "SET_FSM_STRUCTURE" は、コールバックをフックするポイン
   * トであり、以下の値を取ることが可能である。なお、すべてのコールバッ
   * クポイントが実装されているとは限らず、これらが呼び出されるかどうか
   * は、FSMの実装に依存する。
   *
   * - SET_FSM_STRUCTURE: FSM構造の設定
   * - GET_FSM_STRUCTURE: FSM構造の取得
   *
   * 第2引数はリスナオブジェクトのポインタである。第3引数はオブジェクト
   * 自動削除フラグであり、true の場合は、RTObject削除時に自動的にリス
   * ナオブジェクトが削除される。falseの場合は、オブジェクトの所有権は
   * 呼び出し側に残り、削除は呼び出し側の責任で行わなければならない。
   * RTObject のライフサイクル中にコールバックが必要ならば上記のような
   * 呼び出し方で第3引数を true としておくとよい。逆に、コールバックを
   * 状況等に応じてセットしたりアンセットしたりする必要がある場合は
   * falseとして置き、リスナオブジェクトのポインタをメンバ変数などに保
   * 持しておき、
   * RTObject_impl::addPostFsmActionListener()/removePostFsmActionListener()
   * により、セットとアンセットを管理するといった使い方も可能である。
   *
   * @else
   * @class FsmStructureListener class
   * @brief FsmStructureListener class
   *
   * FsmStructureListener class is a base class for the listener
   * objects which realize callback to hook FSM structure profile
   * related actions. To hook execution just before a FSM action, the
   * callback object should be defined as follows, and set to RTObject
   * through appropriate callback set function.
   *
   * <pre>
   * class MyListener
   *   : public FsmStructureListener
   * {
   *   std::string m_name;
   * public:
   *   MyListener(const char* name) : m_name(name) {}
   *   virtual ~MyListener() {}
   *   virtual void operator()(::RTC::FsmStructure& pprof)
   *   {
   *     std::cout << "Listner name:  " m_name << std::endl;
   *   };
   * };
   * </pre>
   *
   * The listener class defined above is set to RTObject as follows.
   *
   * <pre>
   * RTC::ReturnCode_t ConsoleIn::onInitialize()
   * {
   *     addFsmStructureListener(FsmStructureListenerType::SET_FSM_STRUCTURE,
   *                             new MyListener("set structure listener"),
   *                             true);
   *    :
   * </pre>
   *
   * The first argument "SET_FSM_STRUCTURE" specifies callback hook
   * point, and the following values are available. Not all the
   * callback points are implemented. It depends on the FSM
   * implementations.
   *
   * - SET_FSM_STRUCTURE: Setting FSM structure
   * - GET_FSM_STRUCTURE: Getting FSM structure
   *
   * The second argument is a pointers to the listener object. The
   * third argument is a flag for automatic object destruction. When
   * "true" is given to the third argument, the given object in second
   * argument is automatically destructed with RTObject. In the "false
   * " case, the ownership of the object is left in the caller side,
   * and then destruction of the object must be done by users'
   * responsibility.
   *
   * It is good for setting "true" as third argument, if the listener
   * object life span is equals to the RTObject's life cycle.  On the
   * otehr hand, if callbacks are required to set/unset depending on
   * its situation, the third argument could be "false".  In that
   * case, listener objects pointers must be stored to member
   * variables, and set/unset of the listener objects shoud be
   * paerformed throguh
   * RTObject_impl::addFsmStructureListener()/removeFsmStructureListener()
   * functions.
   *
   * @endif
   */
  class FsmStructureListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief FsmStructureListenerType を文字列に変換
     *
     * FsmStructureListenerType を文字列に変換する
     *
     * @param type 変換対象 FsmStructureListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert FsmStructureListenerType into the string.
     *
     * Convert FsmStructureListenerType into the string.
     *
     * @param type The target FsmStructureListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(FsmStructureListenerType type)
    {
      static const char* const typeString[] =
        {
          "SET_FSM_STRUCTURE",
          "GET_FSM_STRUCTURE",
          "FSM_STRUCTURE_LISTENER_NUM"
        };
      if (type < FsmStructureListenerType::FSM_STRUCTURE_LISTENER_NUM)
       {
         return typeString[static_cast<uint8_t>(type)];
       }
      return "";
    }

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~FsmStructureListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * FsmStructureListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for FsmStructureListener
     *
     * @endif
     */
    virtual void operator()(::RTC::FsmStructure& pprof) = 0;
  };

  //============================================================
  // Holder classes
  //============================================================
  /*!
   * @if jp
   * @class PreFsmActionListenerHolder
   * @brief PreFsmActionListener ホルダクラス
   *
   * 複数の PreFsmActionListener を保持し管理するクラス。
   *
   * @else
   * @class PreFsmActionListenerHolder
   * @brief PreFsmActionListener holder class
   *
   * This class manages one ore more instances of
   * PreFsmActionListener class.
   *
   * @endif
   */
  class PreFsmActionListenerHolder
  {
     using Entry = std::pair<PreFsmActionListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    PreFsmActionListenerHolder();
    
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PreFsmActionListenerHolder();
    
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * リスナーを追加する。
     *
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @else
     *
     * @brief Add the listener.
     *
     * This method adds the listener. 
     *
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor. 
     * @endif
     */
    void addListener(PreFsmActionListener* listener, bool autoclean);
    
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * リスナを削除する。
     *
     * @param listener 削除するリスナ
     * @else
     *
     * @brief Remove the listener. 
     *
     * This method removes the listener. 
     *
     * @param listener Removed listener
     * @endif
     */
    void removeListener(PreFsmActionListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param state 
     * @else
     *
     * @brief Notify listeners. 
     *
     * This calls the Callback method of the registered listener. 
     *
     * @param state
     * @endif
     */
    void notify(const char* state);
      
  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };


  /*!
   * @if jp
   * @class PostFsmActionListenerHolder
   * @brief PostFsmActionListener ホルダクラス
   *
   * 複数の PostFsmActionListener を保持し管理するクラス。
   *
   * @else
   * @class PostFsmActionListenerHolder
   * @brief PostFsmActionListener holder class
   *
   * This class manages one ore more instances of
   * PostFsmActionListener class.
   *
   * @endif
   */
  class PostFsmActionListenerHolder
  {
    using Entry = std::pair<PostFsmActionListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    PostFsmActionListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PostFsmActionListenerHolder();
    
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * リスナーを追加する。
     *
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @else
     *
     * @brief Add the listener.
     *
     * This method adds the listener. 
     *
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor. 
     * @endif
     */
    void addListener(PostFsmActionListener* listener, bool autoclean);
    
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * リスナを削除する。
     *
     * @param listener 削除するリスナ
     * @else
     *
     * @brief Remove the listener. 
     *
     * This method removes the listener. 
     *
     * @param listener Removed listener
     * @endif
     */
    void removeListener(PostFsmActionListener* listener);
    
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param state 
     * @param ret 
     * @else
     *
     * @brief Notify listeners. 
     *
     * This calls the Callback method of the registered listener. 
     *
     * @param state
     * @param ret
     * @endif
     */
    void notify(const char* state, ReturnCode_t ret);
    
  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };


  //============================================================
  /*!
   * @if jp
   * @class FsmProfileListenerHolder
   * @brief FsmProfileListener ホルダクラス
   *
   * 複数の FsmProfileListener を保持し管理するクラス。
   *
   * @else
   * @class FsmProfileListenerHolder
   * @brief FsmProfileListener holder class
   *
   * This class manages one ore more instances of
   * FsmProfileListener class.
   *
   * @endif
   */
  class FsmProfileListenerHolder
  {
    using Entry = std::pair<FsmProfileListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    FsmProfileListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~FsmProfileListenerHolder();

    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * リスナーを追加する。
     *
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @else
     *
     * @brief Add the listener.
     *
     * This method adds the listener.
     *
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @endif
     */
    void addListener(FsmProfileListener* listener, bool autoclean);

    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * リスナを削除する。
     *
     * @param listener 削除するリスナ
     * @else
     *
     * @brief Remove the listener.
     *
     * This method removes the listener.
     *
     * @param listener Removed listener
     * @endif
     */
    void removeListener(FsmProfileListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param profile FsmProfile
     * @else
     *
     * @brief Notify listeners. 
     *
     * This calls the Callback method of the registered listener. 
     *
     * @param profile
     * @endif
     */
    void notify(RTC::FsmProfile& profile);

  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };

  /*!
   * @if jp
   * @class FsmStructureListenerHolder
   * @brief FsmStructureListener ホルダクラス
   *
   * 複数の FsmStructureListener を保持し管理するクラス。
   *
   * @else
   * @class FsmStructureListenerHolder
   * @brief FsmStructureListener holder class
   *
   * This class manages one ore more instances of
   * FsmStructureListener class.
   *
   * @endif
   */
  class FsmStructureListenerHolder
  {
    using Entry = std::pair<FsmStructureListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    FsmStructureListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~FsmStructureListenerHolder();

    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * リスナーを追加する。
     *
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @else
     *
     * @brief Add the listener.
     *
     * This method adds the listener.
     *
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @endif
     */
    void addListener(FsmStructureListener* listener, bool autoclean);

    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * リスナを削除する。
     *
     * @param listener 削除するリスナ
     * @else
     *
     * @brief Remove the listener.
     *
     * This method removes the listener.
     *
     * @param listener Removed listener
     * @endif
     */
    void removeListener(FsmStructureListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param structure FsmStructure
     * @else
     *
     * @brief Notify listeners. 
     *
     * This calls the Callback method of the registered listener. 
     *
     * @param structure
     * @endif
     */
    void notify(RTC::FsmStructure& structure);

  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };

  //============================================================
  /*!
   * @if jp
   * @class FsmActionListeners
   * @brief FsmActionListeners クラス
   *
   *
   * @else
   * @class FsmActionListeners
   * @brief FsmActionListeners class
   *
   *
   * @endif
   */
  class FsmActionListeners
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    FsmActionListeners();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~FsmActionListeners();
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のPreFsmActionListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(PreFsmActionListenerType type, PreFsmActionListener* listener, bool autoclean=true);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のPreFsmActionListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(PreFsmActionListenerType type, PreFsmActionListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のPostFsmActionListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(PostFsmActionListenerType type, PostFsmActionListener* listener, bool autoclean=true);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のPostFsmActionListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(PostFsmActionListenerType type, PostFsmActionListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のFsmProfileListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(FsmProfileListenerType type, FsmProfileListener* listener, bool autoclean=true);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のFsmProfileListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(FsmProfileListenerType type, FsmProfileListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のFsmStructureListenerを追加する。
     *
     * @param type リスナの種類
     * @param listener 追加するリスナ
     * @param autoclean true:デストラクタで削除する,
     *                  false:デストラクタで削除しない
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief Add the listener.
     *
     *
     *
     * @param type
     * @param listener Added listener
     * @param autoclean true:The listener is deleted at the destructor.,
     *                  false:The listener is not deleted at the destructor.
     * @return
     * @endif
     */
    bool addListener(FsmStructureListenerType type, FsmStructureListener* listener, bool autoclean=true);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のFsmStructureListenerを削除する。
     *
     * @param type リスナの種類
     * @param listener 削除するリスナ
     * @return false：指定の種類のリスナが存在しない
     *
     * @else
     *
     * @brief Remove the listener.
     *
     *
     * @param type
     * @param listener
     * @return
     *
     * @endif
     */
    bool removeListener(FsmStructureListenerType type, FsmStructureListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のPreFsmActionListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param state ポートプロファイル
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief
     *
     *
     * @param type
     * @param state
     * @return
     * @endif
     */
    inline bool notify(PreFsmActionListenerType type, const char* state)
    {
        if (static_cast<uint8_t>(type) < preaction_.size())
        {
            preaction_[static_cast<uint8_t>(type)].notify(state);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のPostFsmActionListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param state 
     * @param ret 
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief
     *
     *
     * @param type
     * @param state
     * @param ret
     * @return
     * @endif
     */
    inline bool notify(PostFsmActionListenerType type, const char* state, ReturnCode_t ret)
    {
        if (static_cast<uint8_t>(type) < postaction_.size())
        {
            postaction_[static_cast<uint8_t>(type)].notify(state, ret);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のFsmProfileListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param profile FsmProfile
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief
     *
     *
     * @param type
     * @param profile FsmProfile
     * @return
     * @endif
     */
    inline bool notify(FsmProfileListenerType type, RTC::FsmProfile& profile)
    {
        if (static_cast<uint8_t>(type) < profile_.size())
        {
            profile_[static_cast<uint8_t>(type)].notify(profile);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のFsmStructureListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param structure FsmStructure
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief
     *
     *
     * @param type
     * @param structure
     * @return
     * @endif
     */
    inline bool notify(FsmStructureListenerType type, RTC::FsmStructure& structure)
    {
        if (static_cast<uint8_t>(type) < structure_.size())
        {
            structure_[static_cast<uint8_t>(type)].notify(structure);
            return true;
        }
        return false;
    }
  private:
    /*!
     * @if jp
     * @brief PreFsmActionListenerTypeリスナ配列
     * PreFsmActionListenerTypeリスナを格納
     * @else
     * @brief PreFsmActionListenerType listener array
     * The PreFsmActionListenerType listener is stored. 
     * @endif
     */
    std::array<PreFsmActionListenerHolder, static_cast<uint8_t>(PreFsmActionListenerType::PRE_FSM_ACTION_LISTENER_NUM)> preaction_;
    /*!
     * @if jp
     * @brief PostFsmActionTypeリスナ配列
     * PostFsmActionTypeリスナを格納
     * @else
     * @brief PostFsmActionType listener array
     * The PostFsmActionType listener is stored.
     * @endif
     */
    std::array<PostFsmActionListenerHolder, static_cast<uint8_t>(PostFsmActionListenerType::POST_FSM_ACTION_LISTENER_NUM)> postaction_;
    /*!
     * @if jp
     * @brief FsmProfileTypeリスナ配列
     * FsmProfileTypeリスナを格納
     * @else
     * @brief FsmProfileType listener array
     * The FsmProfileType listener is stored.
     * @endif
     */
    std::array<FsmProfileListenerHolder, static_cast<uint8_t>(FsmProfileListenerType::FSM_PROFILE_LISTENER_NUM)> profile_;
    /*!
     * @if jp
     * @brief FsmStructureTypeリスナ配列
     * FsmStructureTypeリスナを格納
     * @else
     * @brief FsmStructureType listener array
     * The FsmStructureType listener is stored.
     * @endif
     */
    std::array<FsmStructureListenerHolder, static_cast<uint8_t>(FsmStructureListenerType::FSM_STRUCTURE_LISTENER_NUM)> structure_;
  };

} // namespace RTC

#endif // RTC_FSMACTIONLISTENER_H
