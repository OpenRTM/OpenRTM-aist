// -*- C++ -*-
/*!
 * @file CSPTransport.h
 * @if jp
 * @brief CORBAを介したCSPポートの準備確認とデータ転送を実装する。 宣言。
 * @else
 * @brief Implement CSP readiness queries and data transfer over CORBA. Declarations.
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

#ifndef RTC_CSPTRANSPORT_H
#define RTC_CSPTRANSPORT_H
#include <rtm/CSPPort.h>
#include <rtm/InPortProvider.h>
#include <rtm/OutPortProvider.h>
#include <rtm/InPortConsumer.h>
#include <rtm/OutPortConsumer.h>
#include <rtm/InPortConnector.h>
#include <rtm/OutPortConnector.h>
#include <rtm/RingBuffer.h>

namespace RTC
{
class InPortDuplexConnector;
class OutPortDuplexConnector;
namespace csp_detail
{
// Servants drain in-flight callbacks before their connector is destroyed.
/*!
 * @if jp
 * @brief `CallGate`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of CallGate.
 * @endif
 */
class CallGate
{
  std::mutex m_mutex;
  std::condition_variable m_idle;
  size_t m_calls{0};
  bool m_closed{false};
public:
  /*!
   * @if jp
   * @brief `enter`に対応する処理を行う。
   * @else
   * @brief Perform the enter operation.
   * @endif
   */
  bool enter() { std::lock_guard<std::mutex> l(m_mutex); if (m_closed) return false; ++m_calls; return true; }
  /*!
   * @if jp
   * @brief `leave`に対応する処理を行う。
   * @else
   * @brief Perform the leave operation.
   * @endif
   */
  void leave() { std::lock_guard<std::mutex> l(m_mutex); if (--m_calls == 0) m_idle.notify_all(); }
  /*!
   * @if jp
   * @brief 新規I/Oを停止し、待機解除と処理中操作の終了を行う。
   * @else
   * @brief Stop new I/O, wake waiters and finish in-flight operations.
   * @endif
   */
  void close() { std::unique_lock<std::mutex> l(m_mutex); m_closed = true; m_idle.wait(l, [this] { return m_calls == 0; }); }
};
/*!
 * @if jp
 * @brief `Call`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of Call.
 * @endif
 */
struct Call
{
  CallGate& gate; bool entered;
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @param value 設定または受け渡す値。
   * @else
   * @brief Initialize the instance and its internal state.
   * @param value Value to configure or transfer.
   * @endif
   */
  explicit Call(CallGate& value) : gate(value), entered(gate.enter()) {}
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~Call() { if (entered) gate.leave(); }
  /*!
   * @if jp
   * @brief `operator bool`に対応する処理を行う。
   * @else
   * @brief Perform the operator bool operation.
   * @endif
   */
  explicit operator bool() const { return entered; }
};
ByteData toBytes(const OctetSeq& data, bool little = true);
OctetSeq toOctets(const ByteData& data);
PortStatus wireStatus(BufferStatus status);
DataPortStatus portStatus(PortStatus status);
ConnectorInfo connectorInfo(const ConnectorProfile& profile);
}

/*!
 * @if jp
 * @brief `InPortCSPProvider`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of InPortCSPProvider.
 * @endif
 */
class InPortCSPProvider : public InPortProvider, public virtual POA_CSP::InPortCsp,
                         public virtual PortableServer::RefCountServantBase
{
public:
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  InPortCSPProvider();
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~InPortCSPProvider() override;
  /*!
   * @if jp
   * @brief 新規I/Oを停止し、待機解除と処理中操作の終了を行う。
   * @else
   * @brief Stop new I/O, wake waiters and finish in-flight operations.
   * @endif
   */
  void close();
  /*!
   * @if jp
   * @brief `init`に対応する処理を行う。
   * @else
   * @brief Perform the init operation.
   * @endif
   */
  void init(coil::Properties&) override {}
  /*!
   * @if jp
   * @brief `Buffer`を設定する。
   * @else
   * @brief Set buffer.
   * @endif
   */
  void setBuffer(BufferBase<ByteData>*) override {}
  /*!
   * @if jp
   * @brief `Listener`を設定する。
   * @param info `info`の値。
   * @param listeners `listeners`の値。
   * @else
   * @brief Set listener.
   * @param info Value of info.
   * @param listeners Value of listeners.
   * @endif
   */
  void setListener(ConnectorInfo& info, ConnectorListenersBase* listeners) override;
  /*!
   * @if jp
   * @brief `Connector`を設定する。
   * @param connector `connector`の値。
   * @else
   * @brief Set connector.
   * @param connector Value of connector.
   * @endif
   */
  void setConnector(InPortConnector* connector) override;
  /*!
   * @if jp
   * @brief CDRデータを受信側へ渡す。
   * @param data 送受信するデータ。
   * @else
   * @brief Deliver CDR data to the receiver.
   * @param data Data to transmit or receive.
   * @endif
   */
  PortStatus put(const OctetSeq& data) override;
  /*!
   * @if jp
   * @brief 受信側の受付可否をReadinessとして返す。
   * @param retry 再試行の問い合わせかどうか。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Return receive-side admission as a Readiness value.
   * @param retry Whether this query is a retry.
   * @param option Options used for guard matching.
   * @endif
   */
  CSP::Readiness is_writable(CORBA::Boolean retry, const CSP::NVList& option) override;
  /*!
   * @if jp
   * @brief 送信側のオプションを受信側ガードと照合する。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Match the sender options against the receiver guard.
   * @param option Options used for guard matching.
   * @endif
   */
  CORBA::Boolean match_condition(const CSP::NVList& option) override;
  /*!
   * @if jp
   * @brief `object`に対応する処理を行う。
   * @else
   * @brief Perform the object operation.
   * @endif
   */
  CSP::InPortCsp_ptr object() const { return CSP::InPortCsp::_duplicate(m_ref); }
private:
  CSP::InPortCsp_var m_ref;
  InPortDuplexConnector* m_connector{nullptr};
  ConnectorInfo m_info;
  ConnectorListenersBase* m_listeners{nullptr};
  csp_detail::CallGate m_gate;
  std::atomic<bool> m_deactivated{false};
};
/*!
 * @if jp
 * @brief `OutPortCSPProvider`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of OutPortCSPProvider.
 * @endif
 */
class OutPortCSPProvider : public OutPortProvider, public virtual POA_CSP::OutPortCsp,
                          public virtual PortableServer::RefCountServantBase
{
public:
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  OutPortCSPProvider();
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~OutPortCSPProvider() override;
  /*!
   * @if jp
   * @brief 新規I/Oを停止し、待機解除と処理中操作の終了を行う。
   * @else
   * @brief Stop new I/O, wake waiters and finish in-flight operations.
   * @endif
   */
  void close();
  /*!
   * @if jp
   * @brief `Buffer`を設定する。
   * @else
   * @brief Set buffer.
   * @endif
   */
  void setBuffer(CdrBufferBase*) override {}
  /*!
   * @if jp
   * @brief `Listener`を設定する。
   * @param info `info`の値。
   * @param listeners `listeners`の値。
   * @else
   * @brief Set listener.
   * @param info Value of info.
   * @param listeners Value of listeners.
   * @endif
   */
  void setListener(ConnectorInfo& info, ConnectorListenersBase* listeners) override;
  /*!
   * @if jp
   * @brief `Connector`を設定する。
   * @param connector `connector`の値。
   * @else
   * @brief Set connector.
   * @param connector Value of connector.
   * @endif
   */
  void setConnector(OutPortConnector* connector) override;
  /*!
   * @if jp
   * @brief 送信側からCDRデータを取り出す。
   * @param data 送受信するデータ。
   * @else
   * @brief Retrieve CDR data from the sender.
   * @param data Data to transmit or receive.
   * @endif
   */
  PortStatus get(OctetSeq_out data) override;
  /*!
   * @if jp
   * @brief 送信側の受付可否をReadinessとして返す。
   * @param retry 再試行の問い合わせかどうか。
   * @param provider 受信側のInPortCsp参照。
   * @else
   * @brief Return send-side admission as a Readiness value.
   * @param retry Whether this query is a retry.
   * @param provider Reference to the receiver InPortCsp interface.
   * @endif
   */
  CSP::Readiness is_readable(CORBA::Boolean retry, CSP::InPortCsp_ptr provider) override;
  /*!
   * @if jp
   * @brief `object`に対応する処理を行う。
   * @else
   * @brief Perform the object operation.
   * @endif
   */
  CSP::OutPortCsp_ptr object() const { return CSP::OutPortCsp::_duplicate(m_ref); }
private:
  CSP::OutPortCsp_var m_ref;
  OutPortDuplexConnector* m_connector{nullptr};
  ConnectorInfo m_info;
  ConnectorListenersBase* m_listeners{nullptr};
  csp_detail::CallGate m_gate;
  std::atomic<bool> m_deactivated{false};
};
/*!
 * @if jp
 * @brief `InPortCSPConsumer`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of InPortCSPConsumer.
 * @endif
 */
class InPortCSPConsumer : public InPortConsumer
{
public:
  /*!
   * @if jp
   * @brief `init`に対応する処理を行う。
   * @param prop 初期化に使用するプロパティ。
   * @else
   * @brief Perform the init operation.
   * @param prop Properties used for initialization.
   * @endif
   */
  void init(coil::Properties& prop) override;
  /*!
   * @if jp
   * @brief CDRデータを受信側へ渡す。
   * @param data 送受信するデータ。
   * @else
   * @brief Deliver CDR data to the receiver.
   * @param data Data to transmit or receive.
   * @endif
   */
  DataPortStatus put(ByteData& data) override;
  /*!
   * @if jp
   * @brief 受信準備が整っているか確認する。
   * @param retry 再試行の問い合わせかどうか。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Check whether the receiver is ready.
   * @param retry Whether this query is a retry.
   * @param option Options used for guard matching.
   * @endif
   */
  bool isWritable(bool retry, const CSP::NVList& option);
  /*!
   * @if jp
   * @brief 受信側の準備確認結果を取得する。
   * @param retry 再試行の問い合わせかどうか。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Get the receive-side readiness response.
   * @param retry Whether this query is a retry.
   * @param option Options used for guard matching.
   * @endif
   */
  CSP::Readiness writableReadiness(bool retry, const CSP::NVList& option);
  /*!
   * @if jp
   * @brief 送信側のオプションを受信側ガードと照合する。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Match the sender options against the receiver guard.
   * @param option Options used for guard matching.
   * @endif
   */
  bool matchCondition(const CSP::NVList& option);
  /*!
   * @if jp
   * @brief `publish Interface Profile`に対応する処理を行う。
   * @else
   * @brief Perform the publish interface profile operation.
   * @endif
   */
  void publishInterfaceProfile(SDOPackage::NVList&) override {}
  /*!
   * @if jp
   * @brief 相手のCORBAインターフェース参照を取得する。
   * @param prop 初期化に使用するプロパティ。
   * @else
   * @brief Obtain the peer CORBA interface reference.
   * @param prop Properties used for initialization.
   * @endif
   */
  bool subscribeInterface(const SDOPackage::NVList& prop) override;
  /*!
   * @if jp
   * @brief 相手のCORBAインターフェース参照を解除する。
   * @else
   * @brief Release the peer CORBA interface reference.
   * @endif
   */
  void unsubscribeInterface(const SDOPackage::NVList&) override { m_ref = CSP::InPortCsp::_nil(); }
  /*!
   * @if jp
   * @brief `object`に対応する処理を行う。
   * @else
   * @brief Perform the object operation.
   * @endif
   */
  CSP::InPortCsp_ptr object() const { return CSP::InPortCsp::_duplicate(m_ref); }
private:
  CSP::InPortCsp_var m_ref;
  double m_timeout{10};
};
/*!
 * @if jp
 * @brief `OutPortCSPConsumer`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of OutPortCSPConsumer.
 * @endif
 */
class OutPortCSPConsumer : public OutPortConsumer
{
public:
  /*!
   * @if jp
   * @brief `init`に対応する処理を行う。
   * @param prop 初期化に使用するプロパティ。
   * @else
   * @brief Perform the init operation.
   * @param prop Properties used for initialization.
   * @endif
   */
  void init(coil::Properties& prop) override;
  /*!
   * @if jp
   * @brief `Buffer`を設定する。
   * @else
   * @brief Set buffer.
   * @endif
   */
  void setBuffer(CdrBufferBase*) override {}
  /*!
   * @if jp
   * @brief `Listener`を設定する。
   * @else
   * @brief Set listener.
   * @endif
   */
  void setListener(ConnectorInfo&, ConnectorListenersBase*) override {}
  /*!
   * @if jp
   * @brief 送信側からCDRデータを取り出す。
   * @param data 送受信するデータ。
   * @else
   * @brief Retrieve CDR data from the sender.
   * @param data Data to transmit or receive.
   * @endif
   */
  DataPortStatus get(ByteData& data) override;
  /*!
   * @if jp
   * @brief 送信準備が整っているか確認する。
   * @param retry 再試行の問い合わせかどうか。
   * @param provider 受信側のInPortCsp参照。
   * @else
   * @brief Check whether the sender is ready.
   * @param retry Whether this query is a retry.
   * @param provider Reference to the receiver InPortCsp interface.
   * @endif
   */
  bool isReadable(bool retry, CSP::InPortCsp_ptr provider);
  /*!
   * @if jp
   * @brief 送信側の準備確認結果を取得する。
   * @param retry 再試行の問い合わせかどうか。
   * @param provider 受信側のInPortCsp参照。
   * @else
   * @brief Get the send-side readiness response.
   * @param retry Whether this query is a retry.
   * @param provider Reference to the receiver InPortCsp interface.
   * @endif
   */
  CSP::Readiness readableReadiness(bool retry, CSP::InPortCsp_ptr provider);
  /*!
   * @if jp
   * @brief 相手のCORBAインターフェース参照を取得する。
   * @param prop 初期化に使用するプロパティ。
   * @else
   * @brief Obtain the peer CORBA interface reference.
   * @param prop Properties used for initialization.
   * @endif
   */
  bool subscribeInterface(const SDOPackage::NVList& prop) override;
  /*!
   * @if jp
   * @brief 相手のCORBAインターフェース参照を解除する。
   * @else
   * @brief Release the peer CORBA interface reference.
   * @endif
   */
  void unsubscribeInterface(const SDOPackage::NVList&) override { m_ref = CSP::OutPortCsp::_nil(); }
  /*!
   * @if jp
   * @brief `object`に対応する処理を行う。
   * @else
   * @brief Perform the object operation.
   * @endif
   */
  CSP::OutPortCsp_ptr object() const { return CSP::OutPortCsp::_duplicate(m_ref); }
private:
  CSP::OutPortCsp_var m_ref;
  double m_timeout{10};
};

/*!
 * @if jp
 * @brief `InPortDuplexConnector`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of InPortDuplexConnector.
 * @endif
 */
class InPortDuplexConnector : public InPortConnector
{
public:
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @param info `info`の値。
   * @param provider 受信側のInPortCsp参照。
   * @param listeners `listeners`の値。
   * @else
   * @brief Initialize the instance and its internal state.
   * @param info Value of info.
   * @param provider Reference to the receiver InPortCsp interface.
   * @param listeners Value of listeners.
   * @endif
   */
  InPortDuplexConnector(ConnectorInfo& info, InPortCSPProvider* provider, ConnectorListenersBase* listeners);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~InPortDuplexConnector() override;
  using InPortConnector::read;
  /*!
   * @if jp
   * @brief 接続からデータを受信する。
   * @param data 送受信するデータ。
   * @else
   * @brief Receive data from a connection.
   * @param data Data to transmit or receive.
   * @endif
   */
  DataPortStatus read(ByteDataStreamBase* data) override;
  /*!
   * @if jp
   * @brief 準備が整った一つの接続へデータを送信する。
   * @param data 送受信するデータ。
   * @else
   * @brief Send data to one ready connection.
   * @param data Data to transmit or receive.
   * @endif
   */
  BufferStatus write(ByteData& data) override;
  /*!
   * @if jp
   * @brief 接続を切断して通信資源を解除する。
   * @else
   * @brief Disconnect the connector and release communication resources.
   * @endif
   */
  DataPortStatus disconnect() override;
  /*!
   * @if jp
   * @brief `activate`に対応する処理を行う。
   * @else
   * @brief Perform the activate operation.
   * @endif
   */
  void activate() override {}
  /*!
   * @if jp
   * @brief `deactivate`に対応する処理を行う。
   * @else
   * @brief Perform the deactivate operation.
   * @endif
   */
  void deactivate() override {}
  /*!
   * @if jp
   * @brief `Consumer`を設定する。
   * @param consumer `consumer`の値。
   * @else
   * @brief Set consumer.
   * @param consumer Value of consumer.
   * @endif
   */
  void setConsumer(std::shared_ptr<OutPortCSPConsumer> consumer) { m_consumer = std::move(consumer); }
  /*!
   * @if jp
   * @brief `consumer`に対応する処理を行う。
   * @else
   * @brief Perform the consumer operation.
   * @endif
   */
  std::shared_ptr<OutPortCSPConsumer> consumer() const { return m_consumer; }
  /*!
   * @if jp
   * @brief `provider`に対応する処理を行う。
   * @else
   * @brief Perform the provider operation.
   * @endif
   */
  InPortCSPProvider* provider() const { return m_provider; }
  /*!
   * @if jp
   * @brief 受信準備が整っているか確認する。
   * @param retry 再試行の問い合わせかどうか。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Check whether the receiver is ready.
   * @param retry Whether this query is a retry.
   * @param option Options used for guard matching.
   * @endif
   */
  bool isWritable(bool retry, const CSP::NVList& option);
  /*!
   * @if jp
   * @brief 受信側の準備確認結果を取得する。
   * @param retry 再試行の問い合わせかどうか。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Get the receive-side readiness response.
   * @param retry Whether this query is a retry.
   * @param option Options used for guard matching.
   * @endif
   */
  CSP::Readiness writableReadiness(bool retry, const CSP::NVList& option);
  /*!
   * @if jp
   * @brief 送信準備が整っているか確認する。
   * @param retry 再試行の問い合わせかどうか。
   * @else
   * @brief Check whether the sender is ready.
   * @param retry Whether this query is a retry.
   * @endif
   */
  bool isReadable(bool retry = false);
  /*!
   * @if jp
   * @brief 送信側のオプションを受信側ガードと照合する。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Match the sender options against the receiver guard.
   * @param option Options used for guard matching.
   * @endif
   */
  bool matchCondition(const CSP::NVList& option);
  /*!
   * @if jp
   * @brief 受付判断に用いるローカルな準備状態を取得する。
   * @else
   * @brief Get the local readiness state used for admission.
   * @endif
   */
  CSP::Readiness readinessState() const;
  /*!
   * @if jp
   * @brief `Readiness Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set readiness listener.
   * @param callback Value of callback.
   * @endif
   */
  void setReadinessListener(std::function<CSP::Readiness()> callback) { m_state = std::move(callback); }
  /*!
   * @if jp
   * @brief `Is Writable Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set is writable listener.
   * @param callback Value of callback.
   * @endif
   */
  void setIsWritableListener(std::function<bool(bool, const CSP::NVList&)> callback) { m_writableState = {}; m_writable = std::move(callback); }
  /*!
   * @if jp
   * @brief `Writable Readiness Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set writable readiness listener.
   * @param callback Value of callback.
   * @endif
   */
  void setWritableReadinessListener(std::function<CSP::Readiness(bool, const CSP::NVList&)> callback) { m_writableState = std::move(callback); }
  /*!
   * @if jp
   * @brief `Write Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set write listener.
   * @param callback Value of callback.
   * @endif
   */
  void setWriteListener(std::function<BufferStatus(ByteData&)> callback) { m_write = std::move(callback); }
  /*!
   * @if jp
   * @brief `Read Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set read listener.
   * @param callback Value of callback.
   * @endif
   */
  void setReadListener(std::function<DataPortStatus(ByteData&)> callback) { m_read = std::move(callback); }
  /*!
   * @if jp
   * @brief `Condition`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set condition.
   * @param callback Value of callback.
   * @endif
   */
  void setCondition(std::function<bool(const CSP::NVList&)> callback) { m_condition = std::move(callback); }
private:
  InPortCSPProvider* m_provider;
  std::shared_ptr<OutPortCSPConsumer> m_consumer;
  RingBuffer<ByteData> m_storage;
  std::recursive_mutex m_mutex;
  bool m_closed{false}, m_reserved{false};
  double m_timeout{10};
  csp_detail::Clock::time_point m_grantExpires;
  std::function<CSP::Readiness()> m_state;
  std::function<bool(bool, const CSP::NVList&)> m_writable;
  std::function<CSP::Readiness(bool, const CSP::NVList&)> m_writableState;
  std::function<bool(const CSP::NVList&)> m_condition;
  std::function<BufferStatus(ByteData&)> m_write;
  std::function<DataPortStatus(ByteData&)> m_read;
};
/*!
 * @if jp
 * @brief `OutPortDuplexConnector`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of OutPortDuplexConnector.
 * @endif
 */
class OutPortDuplexConnector : public OutPortConnector
{
public:
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @param info `info`の値。
   * @param provider 受信側のInPortCsp参照。
   * @param listeners `listeners`の値。
   * @else
   * @brief Initialize the instance and its internal state.
   * @param info Value of info.
   * @param provider Reference to the receiver InPortCsp interface.
   * @param listeners Value of listeners.
   * @endif
   */
  OutPortDuplexConnector(ConnectorInfo& info, OutPortCSPProvider* provider, ConnectorListenersBase* listeners);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~OutPortDuplexConnector() override;
  using OutPortConnector::write;
  /*!
   * @if jp
   * @brief 準備が整った一つの接続へデータを送信する。
   * @param data 送受信するデータ。
   * @else
   * @brief Send data to one ready connection.
   * @param data Data to transmit or receive.
   * @endif
   */
  DataPortStatus write(ByteDataStreamBase* data) override;
  /*!
   * @if jp
   * @brief 接続からデータを受信する。
   * @param data 送受信するデータ。
   * @else
   * @brief Receive data from a connection.
   * @param data Data to transmit or receive.
   * @endif
   */
  BufferStatus read(ByteData& data) override;
  /*!
   * @if jp
   * @brief `send`に対応する処理を行う。
   * @param data 送受信するデータ。
   * @param buffered `buffered`の値。
   * @else
   * @brief Perform the send operation.
   * @param data Data to transmit or receive.
   * @param buffered Value of buffered.
   * @endif
   */
  DataPortStatus send(ByteData& data, bool buffered = true);
  /*!
   * @if jp
   * @brief 接続を切断して通信資源を解除する。
   * @else
   * @brief Disconnect the connector and release communication resources.
   * @endif
   */
  DataPortStatus disconnect() override;
  /*!
   * @if jp
   * @brief `Buffer`を取得する。
   * @else
   * @brief Get buffer.
   * @endif
   */
  CdrBufferBase* getBuffer() override { return &m_storage; }
  /*!
   * @if jp
   * @brief `activate`に対応する処理を行う。
   * @else
   * @brief Perform the activate operation.
   * @endif
   */
  void activate() override {}
  /*!
   * @if jp
   * @brief `deactivate`に対応する処理を行う。
   * @else
   * @brief Perform the deactivate operation.
   * @endif
   */
  void deactivate() override {}
  /*!
   * @if jp
   * @brief `Consumer`を設定する。
   * @param consumer `consumer`の値。
   * @else
   * @brief Set consumer.
   * @param consumer Value of consumer.
   * @endif
   */
  void setConsumer(std::shared_ptr<InPortCSPConsumer> consumer) { m_consumer = std::move(consumer); }
  /*!
   * @if jp
   * @brief `consumer`に対応する処理を行う。
   * @else
   * @brief Perform the consumer operation.
   * @endif
   */
  std::shared_ptr<InPortCSPConsumer> consumer() const { return m_consumer; }
  /*!
   * @if jp
   * @brief `provider`に対応する処理を行う。
   * @else
   * @brief Perform the provider operation.
   * @endif
   */
  OutPortCSPProvider* provider() const { return m_provider; }
  /*!
   * @if jp
   * @brief 送信準備が整っているか確認する。
   * @param retry 再試行の問い合わせかどうか。
   * @param provider 受信側のInPortCsp参照。
   * @else
   * @brief Check whether the sender is ready.
   * @param retry Whether this query is a retry.
   * @param provider Reference to the receiver InPortCsp interface.
   * @endif
   */
  bool isReadable(bool retry, CSP::InPortCsp_ptr provider);
  /*!
   * @if jp
   * @brief 送信側の準備確認結果を取得する。
   * @param retry 再試行の問い合わせかどうか。
   * @param provider 受信側のInPortCsp参照。
   * @else
   * @brief Get the send-side readiness response.
   * @param retry Whether this query is a retry.
   * @param provider Reference to the receiver InPortCsp interface.
   * @endif
   */
  CSP::Readiness readableReadiness(bool retry, CSP::InPortCsp_ptr provider);
  /*!
   * @if jp
   * @brief 受信準備が整っているか確認する。
   * @param retry 再試行の問い合わせかどうか。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Check whether the receiver is ready.
   * @param retry Whether this query is a retry.
   * @param option Options used for guard matching.
   * @endif
   */
  bool isWritable(bool retry = false, const CSP::NVList& option = CSP::NVList());
  /*!
   * @if jp
   * @brief 受付判断に用いるローカルな準備状態を取得する。
   * @else
   * @brief Get the local readiness state used for admission.
   * @endif
   */
  CSP::Readiness readinessState() const;
  /*!
   * @if jp
   * @brief `Readiness Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set readiness listener.
   * @param callback Value of callback.
   * @endif
   */
  void setReadinessListener(std::function<CSP::Readiness()> callback) { m_state = std::move(callback); }
  /*!
   * @if jp
   * @brief `Is Readable Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set is readable listener.
   * @param callback Value of callback.
   * @endif
   */
  void setIsReadableListener(std::function<bool(bool, CSP::InPortCsp_ptr)> callback) { m_readableState = {}; m_readable = std::move(callback); }
  /*!
   * @if jp
   * @brief `Readable Readiness Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set readable readiness listener.
   * @param callback Value of callback.
   * @endif
   */
  void setReadableReadinessListener(std::function<CSP::Readiness(bool, CSP::InPortCsp_ptr)> callback) { m_readableState = std::move(callback); }
  /*!
   * @if jp
   * @brief `Read Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set read listener.
   * @param callback Value of callback.
   * @endif
   */
  void setReadListener(std::function<BufferStatus(ByteData&)> callback) { m_read = std::move(callback); }
  /*!
   * @if jp
   * @brief `Write Listener`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set write listener.
   * @param callback Value of callback.
   * @endif
   */
  void setWriteListener(std::function<DataPortStatus(ByteData&)> callback) { m_write = std::move(callback); }
private:
  OutPortCSPProvider* m_provider;
  std::shared_ptr<InPortCSPConsumer> m_consumer;
  RingBuffer<ByteData> m_storage;
  std::recursive_mutex m_mutex;
  std::condition_variable_any m_changed;
  bool m_closed{false};
  double m_timeout{10};
  std::function<CSP::Readiness()> m_state;
  std::function<bool(bool, CSP::InPortCsp_ptr)> m_readable;
  std::function<CSP::Readiness(bool, CSP::InPortCsp_ptr)> m_readableState;
  std::function<BufferStatus(ByteData&)> m_read;
  std::function<DataPortStatus(ByteData&)> m_write;
};
}
extern "C" {
/*!
 * @if jp
 * @brief `InPortCSPProvider`のFactoryを登録する。
 * @else
 * @brief Register the factory for InPortCSPProvider.
 * @endif
 */
void InPortCSPProviderInit();
/*!
 * @if jp
 * @brief `OutPortCSPProvider`のFactoryを登録する。
 * @else
 * @brief Register the factory for OutPortCSPProvider.
 * @endif
 */
void OutPortCSPProviderInit();
/*!
 * @if jp
 * @brief `InPortCSPConsumer`のFactoryを登録する。
 * @else
 * @brief Register the factory for InPortCSPConsumer.
 * @endif
 */
void InPortCSPConsumerInit();
/*!
 * @if jp
 * @brief `OutPortCSPConsumer`のFactoryを登録する。
 * @else
 * @brief Register the factory for OutPortCSPConsumer.
 * @endif
 */
void OutPortCSPConsumerInit();
}
#endif
