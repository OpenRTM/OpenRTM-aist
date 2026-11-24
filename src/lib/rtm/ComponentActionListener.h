// -*- C++ -*-
/*!
 * @file ComponentActionListener.h
 * @brief component action listener class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_COMPONENTACTIONLISTENER_H
#define RTC_COMPONENTACTIONLISTENER_H

#include <mutex>

#include <rtm/RTC.h>
#include <rtm/idl/RTCSkel.h>
#include <rtm/ConnectorBase.h>

#include <vector>
#include <utility>
#include <array>

namespace RTC
{
  using UniqueId = ExecutionContextHandle_t;
  //============================================================
  /*!
   * @if jp
   * @brief PreComponentActionListener のタイプ
   *
   * - PRE_ON_INITIALIZE:    onInitialize 直前
   * - PRE_ON_FINALIZE:      onFinalize 直前
   * - PRE_ON_STARTUP:       onStartup 直前
   * - PRE_ON_SHUTDOWN:      onShutdown 直前
   * - PRE_ON_ACTIVATED:     onActivated 直前
   * - PRE_ON_DEACTIVATED:   onDeactivated 直前
   * - PRE_ON_ABORTING:      onAborted 直前
   * - PRE_ON_ERROR:         onError 直前
   * - PRE_ON_RESET:         onReset 直前
   * - PRE_ON_EXECUTE:       onExecute 直前
   * - PRE_ON_STATE_UPDATE:  onStateUpdate 直前
   * - PRE_ON_RATE_CHANGED:  onRateChanged 直前
   *
   * @else
   * @brief The types of ConnectorDataListener
   *
   * @endif
   */

  enum class PreComponentActionListenerType : uint8_t
    {
      PRE_ON_INITIALIZE,
      PRE_ON_FINALIZE,
      PRE_ON_STARTUP,
      PRE_ON_SHUTDOWN,
      PRE_ON_ACTIVATED,
      PRE_ON_DEACTIVATED,
      PRE_ON_ABORTING,
      PRE_ON_ERROR,
      PRE_ON_RESET,
      PRE_ON_EXECUTE,
      PRE_ON_STATE_UPDATE,
      PRE_ON_RATE_CHANGED,
      PRE_COMPONENT_ACTION_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class PreComponentActionListener クラス
   * @brief PreComponentActionListener クラス
   *
   * OMG RTC仕様で定義されている以下のコンポーネントアクショントについ
   * て、
   *
   * - on_initialize()
   * - on_finalize()
   * - on_startup()
   * - on_shutdown()
   * - on_activated
   * - on_deactivated()
   * - on_aborted()
   * - on_error()
   * - on_reset()
   * - on_execute()
   * - on_state_update()
   * - on_rate_changed()
   *
   * 各アクションに対応するユーザーコードが呼ばれる直前のタイミング
   * でコールされるリスナクラスの基底クラス。
   *
   * - PRE_ON_INITIALIZE:
   * - PRE_ON_FINALIZE:
   * - PRE_ON_STARTUP:
   * - PRE_ON_SHUTDOWN:
   * - PRE_ON_ACTIVATED:
   * - PRE_ON_DEACTIVATED:
   * - PRE_ON_ABORTING:
   * - PRE_ON_ERROR:
   * - PRE_ON_RESET:
   * - PRE_IN_EXECUTE:
   * - PRE_ON_STATE_UPDATE:
   * - PRE_ON_RATE_CHANGED:
   *
   * @else
   * @class PreComponentActionListener class
   * @brief PreComponentActionListener class
   *
   * This class is abstract base class for listener classes that
   * provides callbacks for various events in rtobject.
   *
   * @endif
   */
  class PreComponentActionListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief PreComponentActionListenerType を文字列に変換
     *
     * PreComponentActionListenerType を文字列に変換する
     *
     * @param type 変換対象 PreComponentActionListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert PreComponentActionListenerType into the string.
     *
     * Convert PreComponentActionListenerType into the string.
     *
     * @param type The target PreComponentActionListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(PreComponentActionListenerType type)
    {
      if (type <
      PreComponentActionListenerType::PRE_COMPONENT_ACTION_LISTENER_NUM)
        {
      static const std::array<const char*, 13> typeString =
        {{
          "PRE_ON_INITIALIZE",
          "PRE_ON_FINALIZE",
          "PRE_ON_STARTUP",
          "PRE_ON_SHUTDOWN",
          "PRE_ON_ACTIVATED",
          "PRE_ON_DEACTIVATED",
          "PRE_ON_ABORTING",
          "PRE_ON_ERROR",
          "PRE_ON_RESET",
          "PRE_ON_EXECUTE",
          "PRE_ON_STATE_UPDATE",
          "PRE_ON_RATE_CHANGED",
          "PRE_COMPONENT_ACTION_LISTENER_NUM"
        }};
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
    virtual ~PreComponentActionListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * PreComponentActionListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for PreComponentActionListener.
     *
     * @endif
     */
    virtual void operator()(UniqueId ec_id) = 0;
  };


  //============================================================
  /*!
   * @if jp
   * @brief PostCompoenntActionListener のタイプ
   *
   * - POST_ON_INITIALIZE:
   * - POST_ON_FINALIZE:
   * - POST_ON_STARTUP:
   * - POST_ON_SHUTDOWN:
   * - POST_ON_ACTIVATED:
   * - POST_ON_DEACTIVATED:
   * - POST_ON_ABORTING:
   * - POST_ON_ERROR:
   * - POST_ON_RESET:
   * - POST_ON_EXECUTE:
   * - POST_ON_STATE_UPDATE:
   * - POST_ON_RATE_CHANGED:
   *
   * @else
   * @brief The types of ConnectorDataListener
   *
   * @endif
   */
  enum class PostComponentActionListenerType : uint8_t
    {
      POST_ON_INITIALIZE,
      POST_ON_FINALIZE,
      POST_ON_STARTUP,
      POST_ON_SHUTDOWN,
      POST_ON_ACTIVATED,
      POST_ON_DEACTIVATED,
      POST_ON_ABORTING,
      POST_ON_ERROR,
      POST_ON_RESET,
      POST_ON_EXECUTE,
      POST_ON_STATE_UPDATE,
      POST_ON_RATE_CHANGED,
      POST_COMPONENT_ACTION_LISTENER_NUM
    };


  /*!
   * @if jp
   * @class PostComponentActionListener クラス
   * @brief PostComponentActionListener クラス
   *
   * OMG RTC仕様で定義されている以下のコンポーネントアクショントについ
   * て、
   *
   * - on_initialize()
   * - on_finalize()
   * - on_startup()
   * - on_shutdown()
   * - on_activated
   * - on_deactivated()
   * - on_aborted()
   * - on_error()
   * - on_reset()
   * - on_execute()
   * - on_state_update()
   * - on_rate_changed()
   *
   * 各アクションに対応するユーザーコードが呼ばれる直前のタイミング
   * でコールされるリスなクラスの基底クラス。
   *
   * - POST_ON_INITIALIZE:
   * - POST_ON_FINALIZE:
   * - POST_ON_STARTUP:
   * - POST_ON_SHUTDOWN:
   * - POST_ON_ACTIVATED:
   * - POST_ON_DEACTIVATED:
   * - POST_ON_ABORTING:
   * - POST_ON_ERROR:
   * - POST_ON_RESET:
   * - POST_ON_EXECUTE:
   * - POST_ON_STATE_UPDATE:
   * - POST_ON_RATE_CHANGED:
   *
   * @else
   * @class PostComponentActionListener class
   * @brief PostComponentActionListener class
   *
   * This class is abstract base class for listener classes that
   * provides callbacks for various events in rtobject.
   *
   * @endif
   */
  class PostComponentActionListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief PostComponentActionListenerType を文字列に変換
     *
     * PostComponentActionListenerType を文字列に変換する
     *
     * @param type 変換対象 PostComponentActionListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert PostComponentActionListenerType into the string.
     *
     * Convert PostComponentActionListenerType into the string.
     *
     * @param type The target PostComponentActionListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(PostComponentActionListenerType type)
    {
      if (type < 
      PostComponentActionListenerType::POST_COMPONENT_ACTION_LISTENER_NUM)
        {
      static const std::array<const char*, 13> typeString =
        {{
          "POST_ON_INITIALIZE",
          "POST_ON_FINALIZE",
          "POST_ON_STARTUP",
          "POST_ON_SHUTDOWN",
          "POST_ON_ACTIVATED",
          "POST_ON_DEACTIVATED",
          "POST_ON_ABORTING",
          "POST_ON_ERROR",
          "POST_ON_RESET",
          "POST_ON_EXECUTE",
          "POST_ON_STATE_UPDATE",
          "POST_ON_RATE_CHANGED",
          "POST_COMPONENT_ACTION_LISTENER_NUM"
        }};
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
    virtual ~PostComponentActionListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * PostComponentActionListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for PostComponentActionListener.
     *
     * @endif
     */
    virtual void operator()(UniqueId ec_id,
                            ReturnCode_t ret) = 0;
  };

  //============================================================
  /*!
   * @if jp
   * @brief PortActionListener のタイプ
   *
   * - ADD_PORT:             Port 追加時
   * - REMOVE_PORT:          Port 削除時
   *
   * @else
   * @brief The types of PortActionListener
   *
   * @endif
   */

  enum class PortActionListenerType : uint8_t
    {
      ADD_PORT,
      REMOVE_PORT,
      PORT_ACTION_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class PortActionListener クラス
   * @brief PortActionListener クラス
   *
   * 各アクションに対応するユーザーコードが呼ばれる直前のタイミング
   * でコールされるリスなクラスの基底クラス。
   *
   * - ADD_PORT:
   * - REMOVE_PORT:
   *
   * @else
   * @class PortActionListener class
   * @brief PortActionListener class
   *
   * This class is abstract base class for listener classes that
   * provides callbacks for various events in rtobject.
   *
   * @endif
   */
  class PortActionListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief PreComponentActionListenerType を文字列に変換
     *
     * PreComponentActionListenerType を文字列に変換する
     *
     * @param type 変換対象 PreComponentActionListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert PreComponentActionListenerType into the string.
     *
     * Convert PreComponentActionListenerType into the string.
     *
     * @param type The target PreComponentActionListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(PortActionListenerType type)
    {
      if (type < PortActionListenerType::PORT_ACTION_LISTENER_NUM)
        {
      static const std::array<const char*, 3> typeString =
        {{
          "ADD_PORT",
          "REMOVE_PORT",
          "PORT_ACTION_LISTENER_NUM"
        }};
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
    virtual ~PortActionListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * PortActionListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for PortActionListener
     *
     * @endif
     */
    virtual void operator()(const ::RTC::PortProfile& pprof) = 0;
  };


  //============================================================
  /*!
   * @if jp
   * @brief ExecutionContextActionListener のタイプ
   *
   * - ADD_PORT:             ExecutionContext 追加時
   * - REMOVE_PORT:          ExecutionContext 削除時
   *
   * @else
   * @brief The types of ExecutionContextActionListener
   *
   * @endif
   */

  enum class ExecutionContextActionListenerType : uint8_t
    {
      EC_ATTACHED,
      EC_DETACHED,
      EC_ACTION_LISTENER_NUM
    };

  /*!
   * @if jp
   * @class ExecutionContextActionListener クラス
   * @brief ExecutionContextActionListener クラス
   *
   * 各アクションに対応するユーザーコードが呼ばれる直前のタイミング
   * でコールされるリスなクラスの基底クラス。
   *
   * - ADD_PORT:
   * - REMOVE_PORT:
   *
   * @else
   * @class ExecutionContextActionListener class
   * @brief ExecutionContextActionListener class
   *
   * This class is abstract base class for listener classes that
   * provides callbacks for various events in rtobject.
   *
   * @endif
   */
  class ExecutionContextActionListener
  {
  public:
    /*!
     * @if jp
     *
     * @brief PreComponentActionListenerType を文字列に変換
     *
     * PreComponentActionListenerType を文字列に変換する
     *
     * @param type 変換対象 PreComponentActionListenerType
     *
     * @return 文字列変換結果
     *
     * @else
     *
     * @brief Convert PreComponentActionListenerType into the string.
     *
     * Convert PreComponentActionListenerType into the string.
     *
     * @param type The target PreComponentActionListenerType for transformation
     *
     * @return Trnasformation result of string representation
     *
     * @endif
     */
    static const char* toString(ExecutionContextActionListenerType type)
    {
      if (type < ExecutionContextActionListenerType::EC_ACTION_LISTENER_NUM)
        {
      static const std::array<const char*, 3> typeString =
        {{
          "ATTACH_EC",
          "DETACH_ECT",
          "EC_ACTION_LISTENER_NUM"
        }};
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
    virtual ~ExecutionContextActionListener();

    /*!
     * @if jp
     *
     * @brief 仮想コールバック関数
     *
     * ExecutionContextActionListener のコールバック関数
     *
     * @else
     *
     * @brief Virtual Callback function
     *
     * This is a the Callback function for ExecutionContextActionListener
     *
     * @endif
     */
    virtual void operator()(UniqueId ec_id) = 0;
  };



  //============================================================
  /*!
   * @if jp
   * @class PreComponentActionListenerHolder
   * @brief PreComponentActionListener ホルダクラス
   *
   * 複数の PreComponentActionListener を保持し管理するクラス。
   *
   * @else
   * @class PreComponentActionListenerHolder
   * @brief PreComponentActionListener holder class
   *
   * This class manages one ore more instances of
   * PreComponentActionListener class.
   *
   * @endif
   */
  class PreComponentActionListenerHolder
  {
    using Entry = std::pair<PreComponentActionListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    PreComponentActionListenerHolder();

    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PreComponentActionListenerHolder();

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
    void addListener(PreComponentActionListener* listener, bool autoclean);

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
    void removeListener(PreComponentActionListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param ec_id 実行コンテキストのID
     * @else
     *
     * @brief Notify listeners.
     *
     * This calls the Callback method of the registered listener.
     *
     * @param ec_id 
     * @endif
     */
    void notify(UniqueId ec_id);

  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };


  /*!
   * @if jp
   * @class PostComponentActionListenerHolder
   * @brief PostComponentActionListener ホルダクラス
   *
   * 複数の PostComponentActionListener を保持し管理するクラス。
   *
   * @else
   * @class PostComponentActionListenerHolder
   * @brief PostComponentActionListener holder class
   *
   * This class manages one ore more instances of
   * PostComponentActionListener class.
   *
   * @endif
   */
  class PostComponentActionListenerHolder
  {
    using Entry = std::pair<PostComponentActionListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    PostComponentActionListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PostComponentActionListenerHolder();

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
    void addListener(PostComponentActionListener* listener, bool autoclean);

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
    void removeListener(PostComponentActionListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param ec_id 実行コンテキストのID
     * @param ret リターンコード
     * @else
     *
     * @brief Notify listeners.
     *
     * This calls the Callback method of the registered listener.
     *
     * @param ec_id 
     * @param ret 
     * @endif
     */
    void notify(UniqueId ec_id, ReturnCode_t ret);

  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };


  //============================================================
  /*!
   * @if jp
   * @class PortActionListenerHolder
   * @brief PortActionListener ホルダクラス
   *
   * 複数の PortActionListener を保持し管理するクラス。
   *
   * @else
   * @class PortActionListenerHolder
   * @brief PortActionListener holder class
   *
   * This class manages one ore more instances of
   * PortActionListener class.
   *
   * @endif
   */
  class PortActionListenerHolder
  {
    using Entry = std::pair<PortActionListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    PortActionListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~PortActionListenerHolder();

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
    void addListener(PortActionListener* listener, bool autoclean);

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
    void removeListener(PortActionListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param info ConnectorInfo
     * @param cdrdata データ
     * @else
     *
     * @brief Notify listeners.
     *
     * This calls the Callback method of the registered listener.
     *
     * @param info ConnectorInfo
     * @param cdrdata Data
     * @endif
     */
    void notify(const RTC::PortProfile& pprofile);

  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };

  /*!
   * @if jp
   * @class ExecutionContextActionListenerHolder
   * @brief ExecutionContextActionListener ホルダクラス
   *
   * 複数の ExecutionContextActionListener を保持し管理するクラス。
   *
   * @else
   * @class ExecutionContextActionListenerHolder
   * @brief ExecutionContextActionListener holder class
   *
   * This class manages one ore more instances of
   * ExecutionContextActionListener class.
   *
   * @endif
   */
  class ExecutionContextActionListenerHolder
  {
    using Entry = std::pair<ExecutionContextActionListener*, bool>;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ExecutionContextActionListenerHolder();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    virtual ~ExecutionContextActionListenerHolder();

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
    void addListener(ExecutionContextActionListener* listener, bool autoclean);

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
    void removeListener(ExecutionContextActionListener* listener);

    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 登録されているリスナのコールバックメソッドを呼び出す。
     *
     * @param ec_id 実行コンテキストのID
     * @else
     *
     * @brief Notify listeners.
     *
     * This calls the Callback method of the registered listener.
     *
     * @param ec_id 
     * @endif
     */
    void notify(UniqueId ec_id);

  private:
    std::vector<Entry> m_listeners;
    std::mutex m_mutex;
  };


  /*!
   * @if jp
   * @class ComponentActionListeners
   * @brief ComponentActionListeners クラス
   *
   *
   * @else
   * @class ComponentActionListeners
   * @brief ComponentActionListeners class
   *
   *
   * @endif
   */
  class ComponentActionListeners
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     * @else
     * @brief Constructor
     * @endif
     */
    ComponentActionListeners();
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief Destructor
     * @endif
     */
    ~ComponentActionListeners();
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のPreComponentActionListenerを追加する。
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
    bool addListener(PreComponentActionListenerType type, PreComponentActionListener* listener, bool autoclean);
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のPostComponentActionListenerを追加する。
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
    bool addListener(PostComponentActionListenerType type, PostComponentActionListener* listener, bool autoclean);
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のPortActionListenerを追加する。
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
    bool addListener(PortActionListenerType type, PortActionListener* listener, bool autoclean);
    /*!
     * @if jp
     *
     * @brief リスナーの追加
     *
     * 指定の種類のExecutionContextActionListenerを追加する。
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
    bool addListener(ExecutionContextActionListenerType type, ExecutionContextActionListener* listener, bool autoclean);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のPreComponentActionListenerを削除する。
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
    bool removeListener(PreComponentActionListenerType type, PreComponentActionListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のPreComponentActionListenerを削除する。
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
    bool removeListener(PostComponentActionListenerType type, PostComponentActionListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のPortActionListenerを削除する。
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
    bool removeListener(PortActionListenerType type, PortActionListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーの削除
     *
     * 指定の種類のExecutionContextActionListenerを削除する。
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
    bool removeListener(ExecutionContextActionListenerType type, ExecutionContextActionListener* listener);
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のPreComponentActionListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param ec_id 実行コンテキストのID
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief 
     *
     *
     * @param type 
     * @param ec_id 
     * @return
     * @endif
     */
    inline bool notify(PreComponentActionListenerType type, UniqueId ec_id)
    {
        if (static_cast<uint8_t>(type) < preaction_.size())
        {
            preaction_[static_cast<uint8_t>(type)].notify(ec_id);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のPostComponentActionListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param ec_id 実行コンテキストのID
     * @param ret リターンコード
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief 
     *
     *
     * @param type 
     * @param ec_id 
     * @param ret
     * @return
     * @endif
     */
    inline bool notify(PostComponentActionListenerType type, UniqueId ec_id, ReturnCode_t ret)
    {
        if (static_cast<uint8_t>(type) < postaction_.size())
        {
            postaction_[static_cast<uint8_t>(type)].notify(ec_id, ret);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のPortActionListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param pprofile ポートプロファイル
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief 
     *
     *
     * @param type 
     * @param pprofile 
     * @return
     * @endif
     */
    inline bool notify(PortActionListenerType type, const RTC::PortProfile& pprofile)
    {
        if (static_cast<uint8_t>(type) < portaction_.size())
        {
            portaction_[static_cast<uint8_t>(type)].notify(pprofile);
            return true;
        }
        return false;
    }
    /*!
     * @if jp
     *
     * @brief リスナーへ通知する
     *
     * 指定の種類のExecutionContextActionListenerのコールバック関数を呼び出す。
     *
     * @param type リスナの種類
     * @param ec_id 実行コンテキストのID
     * @return false：指定の種類のリスナが存在しない
     * @else
     *
     * @brief
     *
     *
     * @param type
     * @param pprofile
     * @return
     * @endif
     */
    inline bool notify(ExecutionContextActionListenerType type, UniqueId ec_id)
    {
        if (static_cast<uint8_t>(type) < ecaction_.size())
        {
            ecaction_[static_cast<uint8_t>(type)].notify(ec_id);
            return true;
        }
        return false;
    }
  private:
    /*!
     * @if jp
     * @brief PreComponentActionListenerTypeリスナ配列
     * PreComponentActionListenerTypeリスナを格納
     * @else
     * @brief PreComponentActionListenerType listener array
     * The PreComponentActionListenerType listener is stored.
     * @endif
     */
    std::array<PreComponentActionListenerHolder, static_cast<uint8_t>(PreComponentActionListenerType::PRE_COMPONENT_ACTION_LISTENER_NUM)> preaction_;
    /*!
     * @if jp
     * @brief PostComponentActionTypeリスナ配列
     * PostComponentActionTypeリスナを格納
     * @else
     * @brief PostComponentActionType listener array
     * The PostComponentActionType listener is stored.
     * @endif
     */
    std::array<PostComponentActionListenerHolder, static_cast<uint8_t>(PostComponentActionListenerType::POST_COMPONENT_ACTION_LISTENER_NUM)> postaction_;
    /*!
     * @if jp
     * @brief PortActionTypeリスナ配列
     * PortActionTypeリスナを格納
     * @else
     * @brief PortActionType listener array
     * The PortActionType listener is stored.
     * @endif
     */
    std::array<PortActionListenerHolder, static_cast<uint8_t>(PortActionListenerType::PORT_ACTION_LISTENER_NUM)> portaction_;
    /*!
     * @if jp
     * @brief ExecutionContextActionTypeリスナ配列
     * ExecutionContextActionTypeリスナを格納
     * @else
     * @brief ExecutionContextActionType listener array
     * The ExecutionContextActionType listener is stored.
     * @endif
     */
    std::array<ExecutionContextActionListenerHolder, static_cast<uint8_t>(ExecutionContextActionListenerType::EC_ACTION_LISTENER_NUM)> ecaction_;
  };


} // namespace RTC

#endif  // RTC_COMPONENTACTIONLISTENER_H
