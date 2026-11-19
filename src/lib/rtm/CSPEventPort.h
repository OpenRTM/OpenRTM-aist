// -*- C++ -*-
/*!
 * @file CSPEventPort.h
 * @if jp
 * @brief 受信したCSPイベントをFSMへ引き渡す。 宣言。
 * @else
 * @brief Deliver received CSP events to the FSM. Declarations.
 * @endif
 * @date $Date: 2026-11-13 15:32:20 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 */

#ifndef RTC_CSPEVENTPORT_H
#define RTC_CSPEVENTPORT_H
#include <rtm/CSPInPort.h>
#include <rtm/StaticFSM.h>
#include <type_traits>

namespace RTC {
/** Typed event port. Handlers run on the reader's thread. */
/*!
 * @if jp
 * @brief 受信したCSPイベントをFSMへ引き渡す。
 * @else
 * @brief Deliver received CSP events to the FSM.
 * @endif
 */
template<class T = void>
class CSPEventPort : public CSPInPort<T> {
public:
  /*!
   * @if jp
   * @brief `CSPEvent Port`に対応する処理を行う。
   * @param name 識別に使用する名前。
   * @param manager 関連付けるManager。
   * @else
   * @brief Perform the cspevent port operation.
   * @param name Name identifying the object.
   * @param manager Manager to associate with the object.
   * @endif
   */
  CSPEventPort(const char* name, CSPManager* manager = nullptr)
    : CSPInPort<T>(name, m_payload, manager) {}
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~CSPEventPort() override { this->close(); std::lock_guard<std::recursive_mutex> dispatch(m_dispatch); }
  /*!
   * @if jp
   * @brief 引数なしイベントとハンドラーを束縛する。
   * @param name 識別に使用する名前。
   * @param handler `handler`の値。
   * @else
   * @brief Bind an event to a handler without a payload argument.
   * @param name Name identifying the object.
   * @param handler Value of handler.
   * @endif
   */
  void bindEvent0(const std::string& name, std::function<void()> handler) {
    std::lock_guard<std::mutex> l(m_handlersMutex);
    m_handlers[name] = [handler](const T&) { handler(); };
  }
  /*!
   * @if jp
   * @brief データ付きイベントとハンドラーを束縛する。
   * @param name 識別に使用する名前。
   * @param handler `handler`の値。
   * @else
   * @brief Bind an event to a payload handler.
   * @param name Name identifying the object.
   * @param handler Value of handler.
   * @endif
   */
  void bindEvent1(const std::string& name, std::function<void(const T&)> handler) {
    std::lock_guard<std::mutex> l(m_handlersMutex); m_handlers[name] = std::move(handler);
  }
  /*!
   * @if jp
   * @brief 接続からデータを受信する。
   * @param connector `connector`の値。
   * @else
   * @brief Receive data from a connection.
   * @param connector Value of connector.
   * @endif
   */
  bool read(std::string connector = "") override {
    std::lock_guard<std::recursive_mutex> dispatch(m_dispatch);
    if (!CSPInPort<T>::read(connector)) return false;
    std::function<void(const T&)> handler;
    { std::lock_guard<std::mutex> l(m_handlersMutex); auto it = m_handlers.find(this->lastEvent());
      if (it == m_handlers.end()) return false;
      handler = it->second; }
    handler(m_payload);
    return true;
  }
protected:
  /*!
   * @if jp
   * @brief `accept Event`に対応する処理を行う。
   * @param name 識別に使用する名前。
   * @else
   * @brief Perform the accept event operation.
   * @param name Name identifying the object.
   * @endif
   */
  bool acceptEvent(const std::string& name) const override {
    std::lock_guard<std::mutex> l(m_handlersMutex); return m_handlers.find(name) != m_handlers.end();
  }
private:
  T m_payload{};
  mutable std::mutex m_handlersMutex;
  std::recursive_mutex m_dispatch;
  std::map<std::string, std::function<void(const T&)>> m_handlers;
};
/** Heterogeneous events: each event name has its own CDR payload type. */
/*!
 * @if jp
 * @brief 受信したCSPイベントをFSMへ引き渡す。
 * @else
 * @brief Deliver received CSP events to the FSM.
 * @endif
 */
template<>
class CSPEventPort<void> : public CSPInPort<csp_detail::EventEnvelope> {
public:
  /*!
   * @if jp
   * @brief `CSPEvent Port`に対応する処理を行う。
   * @param name 識別に使用する名前。
   * @param manager 関連付けるManager。
   * @else
   * @brief Perform the cspevent port operation.
   * @param name Name identifying the object.
   * @param manager Manager to associate with the object.
   * @endif
   */
  CSPEventPort(const char* name, CSPManager* manager = nullptr)
    : CSPInPort<csp_detail::EventEnvelope>(name, m_payload, manager) {}
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~CSPEventPort() override { this->close(); std::lock_guard<std::recursive_mutex> dispatch(m_dispatch); }
  /*!
   * @if jp
   * @brief 引数なしイベントとハンドラーを束縛する。
   * @param name 識別に使用する名前。
   * @param handler `handler`の値。
   * @else
   * @brief Bind an event to a handler without a payload argument.
   * @param name Name identifying the object.
   * @param handler Value of handler.
   * @endif
   */
  void bindEvent0(const std::string& name, std::function<void()> handler) {
    std::lock_guard<std::mutex> l(m_handlersMutex);
    m_handlers[name] = [handler](const csp_detail::EventEnvelope&) { handler(); return true; };
  }
  /*!
   * @if jp
   * @brief データ付きイベントとハンドラーを束縛する。
   * @param name 識別に使用する名前。
   * @param handler `handler`の値。
   * @else
   * @brief Bind an event to a payload handler.
   * @param name Name identifying the object.
   * @param handler Value of handler.
   * @endif
   */
  template<class Data, class Handler>
  void bindEvent1(const std::string& name, Handler handler) {
    std::lock_guard<std::mutex> l(m_handlersMutex);
    m_handlers[name] = [handler](const csp_detail::EventEnvelope& event) {
      Data data{};
      if (!csp_detail::decode(event.bytes, data, event.little)) return false;
      handler(data);
      return true;
    };
  }
  /*!
   * @if jp
   * @brief イベントをFSMのハンドラーへ束縛する。
   * @param name 識別に使用する名前。
   * @param machine `machine`の値。
   * @param handler `handler`の値。
   * @else
   * @brief Bind an event to its FSM handler.
   * @param name Name identifying the object.
   * @param machine Value of machine.
   * @param handler Value of handler.
   * @endif
   */
  template<class MachineType, class TOP, class R>
  void bindEvent(const std::string& name, MachineType& machine, R (TOP::*handler)()) {
    bindEvent0(name, [&machine, handler] { machine.dispatch(Macho::Event(handler)); });
  }
  /*!
   * @if jp
   * @brief イベントをFSMのハンドラーへ束縛する。
   * @param name 識別に使用する名前。
   * @param machine `machine`の値。
   * @param handler `handler`の値。
   * @else
   * @brief Bind an event to its FSM handler.
   * @param name Name identifying the object.
   * @param machine Value of machine.
   * @param handler Value of handler.
   * @endif
   */
  template<class MachineType, class TOP, class R, class Data>
  void bindEvent(const std::string& name, MachineType& machine, R (TOP::*handler)(Data)) {
    using Value = typename std::decay<Data>::type;
    bindEvent1<Value>(name, [&machine, handler](const Value& data) { machine.dispatch(Macho::Event(handler, data)); });
  }
  /*!
   * @if jp
   * @brief 接続からデータを受信する。
   * @param connector `connector`の値。
   * @else
   * @brief Receive data from a connection.
   * @param connector Value of connector.
   * @endif
   */
  bool read(std::string connector = "") override {
    std::lock_guard<std::recursive_mutex> dispatch(m_dispatch);
    if (!CSPInPort<csp_detail::EventEnvelope>::read(connector)) return false;
    std::function<bool(const csp_detail::EventEnvelope&)> handler;
    { std::lock_guard<std::mutex> l(m_handlersMutex); auto it = m_handlers.find(this->lastEvent());
      if (it == m_handlers.end()) return false;
      handler = it->second; }
    return handler(m_payload);
  }
protected:
  /*!
   * @if jp
   * @brief `accept Event`に対応する処理を行う。
   * @param name 識別に使用する名前。
   * @else
   * @brief Perform the accept event operation.
   * @param name Name identifying the object.
   * @endif
   */
  bool acceptEvent(const std::string& name) const override {
    std::lock_guard<std::mutex> l(m_handlersMutex); return m_handlers.find(name) != m_handlers.end();
  }
private:
  csp_detail::EventEnvelope m_payload;
  mutable std::mutex m_handlersMutex;
  std::recursive_mutex m_dispatch;
  std::map<std::string, std::function<bool(const csp_detail::EventEnvelope&)>> m_handlers;
};
}
#endif
