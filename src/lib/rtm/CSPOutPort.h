// -*- C++ -*-
/*!
 * @file CSPOutPort.h
 * @if jp
 * @brief 型付き出力と送信予約を管理する。 宣言。
 * @else
 * @brief Manage typed output and send reservations. Declarations.
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

#ifndef RTC_CSPOUTPORT_H
#define RTC_CSPOUTPORT_H
#include <rtm/idl/CSPPortSkel.h>
#include <rtm/OutPortBase.h>
#include <rtm/Typename.h>
#include <rtm/CSPTransport.h>
#include <rtm/PortCallback.h>

namespace RTC
{
/** A write succeeds after one ready peer accepts the value (no broadcast). */
/*!
 * @if jp
 * @brief 型付き出力と送信予約を管理する。
 * @else
 * @brief Manage typed output and send reservations.
 * @endif
 */
template<class T>
class CSPOutPort : public OutPortBase, public CSPSelectable
{
  /*!
   * @if jp
   * @brief `Peer`の状態と操作をまとめる。
   * @else
   * @brief Encapsulate the state and operations of Peer.
   * @endif
   */
  struct Peer
  {
    /*!
     * @if jp
     * @brief 保持している資源を解放する。
     * @else
     * @brief Release the resources owned by the object.
     * @endif
     */
    ~Peer();
    std::shared_ptr<OutPortDuplexConnector> connector;
    CSP::OutPortCsp_var local;
    CSP::InPortCsp_var remote;
    CSP::NVList options;
    bool little{true};
  };
public:
  /*!
   * @if jp
   * @brief `CSPOut Port`に対応する処理を行う。
   * @param name 識別に使用する名前。
   * @param value 設定または受け渡す値。
   * @param manager 関連付けるManager。
   * @else
   * @brief Perform the cspout port operation.
   * @param name Name identifying the object.
   * @param value Value to configure or transfer.
   * @param manager Manager to associate with the object.
   * @endif
   */
  CSPOutPort(const char* name, T& value, CSPManager* manager = nullptr)
    : OutPortBase(name, CORBA_Util::toRepositoryId<T>()), m_name(name), m_value(value)
  {
    addProperty("dataport.interface_type", "csp_channel");
    addProperty("dataport.dataflow_type", "duplex");
    addProperty("dataport.marshaling_types", "cdr");
    addProperty("serializer.cdr.endian", "little,big");
    initConnectorListeners();
    setManager(manager);
  }
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~CSPOutPort() override { close(); }
  /*!
   * @if jp
   * @brief 新規I/Oを停止し、待機解除と処理中操作の終了を行う。
   * @else
   * @brief Stop new I/O, wake waiters and finish in-flight operations.
   * @endif
   */
  void close()
  {
    if (m_closed.exchange(true)) return;
    m_changed.notify_all();
    releaseManager();
    m_operations.close();
    disconnect_all();
  }
  /*!
   * @if jp
   * @brief `name`に対応する処理を行う。
   * @else
   * @brief Perform the name operation.
   * @endif
   */
  const char* name() const { return m_name.c_str(); }
protected:
  /*!
   * @if jp
   * @brief `init Connector Listeners`に対応する処理を行う。
   * @else
   * @brief Perform the init connector listeners operation.
   * @endif
   */
  void initConnectorListeners() override
  {
    delete m_listeners;
    m_listeners = csp_detail::makeListeners<T>();
  }
public:
  /*!
   * @if jp
   * @brief `init`に対応する処理を行う。
   * @param props 初期化に使用するプロパティ。
   * @else
   * @brief Perform the init operation.
   * @param props Properties used for initialization.
   * @endif
   */
  void init(coil::Properties& props) override
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_properties << props; m_timeout = csp_detail::timeout(m_properties);
    setSyncMode(coil::toBool(m_properties.getProperty("csp.sync_wait"), "YES", "NO", false));
    for (auto& peer : m_peers) {
      auto consumer = peer.second.connector->consumer();
      if (consumer) consumer->init(m_properties);
    }
  }
  /*!
   * @if jp
   * @brief ポートの接続を確立する。
   * @param profile 接続プロファイル。
   * @else
   * @brief Establish a port connection.
   * @param profile Connector profile.
   * @endif
   */
  ReturnCode_t connect(ConnectorProfile& profile) override { return PortBase::connect(profile); }
  /*!
   * @if jp
   * @brief 接続通知を処理し、接続情報を確定する。
   * @param profile 接続プロファイル。
   * @else
   * @brief Process a connection notification and finalize connector information.
   * @param profile Connector profile.
   * @endif
   */
  ReturnCode_t notify_connect(ConnectorProfile& profile) override {
    try { return PortBase::notify_connect(profile); }
    catch (const std::exception& error) { std::cerr << "CSP connect: " << error.what() << std::endl; return RTC_ERROR; }
  }
  /*!
   * @if jp
   * @brief `is Input`に対応する処理を行う。
   * @else
   * @brief Perform the is input operation.
   * @endif
   */
  bool isInput() const override { return false; }
  /*!
   * @if jp
   * @brief 新しい選択に必要な予約・走査状態を初期化する。
   * @else
   * @brief Initialize reservation and scan state for a new selection.
   * @endif
   */
  void beginSelection() override
  { std::lock_guard<std::mutex> lock(m_mutex); m_armed = true; }
  /*!
   * @if jp
   * @brief 選択終了を通知し、未選択側の状態を解除する。
   * @param selected `selected`の値。
   * @else
   * @brief Mark the end of selection and clear unselected state.
   * @param selected Value of selected.
   * @endif
   */
  void endSelection(bool selected) override
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_armed = selected;
    if (!selected) { m_push.clear(); m_pull.clear(); }
    m_changed.notify_all();
  }
  /*!
   * @if jp
   * @brief 保持した通信候補を再走査する。
   * @else
   * @brief Rescan retained communication candidates.
   * @endif
   */
  bool reselect() override
  {
    std::lock_guard<std::recursive_mutex> serial(m_io);
    m_retry = true;
    try { auto result = select(); m_retry = false; return result; }
    catch (...) { m_retry = false; throw; }
  }
  /*!
   * @if jp
   * @brief 通信可能な登録ポートを一つ選択する。
   * @else
   * @brief Select one ready registered port.
   * @endif
   */
  bool select() override
  {
    csp_detail::Call operation(m_operations);
    if (!operation) return false;
    std::lock_guard<std::recursive_mutex> serial(m_io);
    if (m_closed || !selectionAvailable()) return false;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      expirePull();
      if (!m_pull.empty() || !m_push.empty()) return true;
    }
    CSP::NVList options;
    { std::lock_guard<std::mutex> lock(m_mutex); options = m_option; }
    for (auto& peer : peers())
    {
      if (CORBA::is_nil(peer.second.remote)) continue;
      try
      {
        auto reply = peer.second.connector->consumer()->writableReadiness(m_retry, options);
        if (reply != CSP::READY) { rememberUnavailable(peer.first, reply); continue; }
        if (!claimSelection()) return false;
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pull.empty()) m_push = peer.first;
        return true;
      }
      catch (const CORBA::Exception&) {}
    }
    return false;
  }
  /*!
   * @if jp
   * @brief 相手の入力ガード照合に使う送信オプションを設定する。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Set sender options used to match the peer input guard.
   * @param option Options used for guard matching.
   * @endif
   */
  void setGuard(const CSP::NVList& option) { std::lock_guard<std::mutex> l(m_mutex); m_option = option; }
  /*!
   * @if jp
   * @brief `Guard`を解除する。
   * @else
   * @brief Clear guard.
   * @endif
   */
  void clearGuard() { std::lock_guard<std::mutex> l(m_mutex); m_option.length(0); }
  /*!
   * @if jp
   * @brief `Data`を設定する。
   * @param value 設定または受け渡す値。
   * @else
   * @brief Set data.
   * @param value Value to configure or transfer.
   * @endif
   */
  void setData(const T& value) { std::lock_guard<std::mutex> l(m_mutex); m_value = value; }
  /*!
   * @if jp
   * @brief 選択済み出力のデータを送信する。
   * @else
   * @brief Send data through the selected output.
   * @endif
   */
  bool writeData() { return write(); }
  /*!
   * @if jp
   * @brief `selected IO`に対応する処理を行う。
   * @else
   * @brief Perform the selected io operation.
   * @endif
   */
  bool selectedIO() override { return writeData(); }
  /*!
   * @if jp
   * @brief 選択済み出力のデータを送信する。
   * @param value 設定または受け渡す値。
   * @else
   * @brief Send data through the selected output.
   * @param value Value to configure or transfer.
   * @endif
   */
  bool writeData(const T& value) { return write(value); }
  /*!
   * @if jp
   * @brief 準備が整った一つの接続へデータを送信する。
   * @else
   * @brief Send data to one ready connection.
   * @endif
   */
  bool write() override {
    csp_detail::Call operation(m_operations);
    if (!operation) return false;
    std::lock_guard<std::recursive_mutex> serial(m_io);
    if (m_closed) return false;
    T value;
    { std::lock_guard<std::mutex> l(m_mutex); value = m_value; }
    return write(value);
  }
  /*!
   * @if jp
   * @brief `On Write`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on write.
   * @param callback Value of callback.
   * @endif
   */
  void setOnWrite(std::function<void(const T&)> callback) { std::lock_guard<std::mutex> l(m_mutex); m_onWrite = std::move(callback); }
  /*!
   * @if jp
   * @brief `On Write`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on write.
   * @param callback Value of callback.
   * @endif
   */
  void setOnWrite(OnWrite<T>* callback) { setOnWrite(callback ? std::function<void(const T&)>([callback](const T& data) { (*callback)(data); }) : std::function<void(const T&)>()); }
  /*!
   * @if jp
   * @brief `On Write Convert`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on write convert.
   * @param callback Value of callback.
   * @endif
   */
  void setOnWriteConvert(std::function<T(const T&)> callback) { std::lock_guard<std::mutex> l(m_mutex); m_onWriteConvert = std::move(callback); }
  /*!
   * @if jp
   * @brief `On Write Convert`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on write convert.
   * @param callback Value of callback.
   * @endif
   */
  void setOnWriteConvert(OnWriteConvert<T>* callback) { setOnWriteConvert(callback ? std::function<T(const T&)>([callback](const T& data) { return (*callback)(data); }) : std::function<T(const T&)>()); }
  /*!
   * @if jp
   * @brief 準備が整った一つの接続へデータを送信する。
   * @param value 設定または受け渡す値。
   * @else
   * @brief Send data to one ready connection.
   * @param value Value to configure or transfer.
   * @endif
   */
  bool write(const T& value)
  {
    csp_detail::Call operation(m_operations);
    if (!operation) return false;
    std::lock_guard<std::recursive_mutex> serial(m_io);
    if (m_closed) return false;
    std::function<void(const T&)> onWrite;
    std::function<T(const T&)> convert;
    double seconds;
    size_t topology;
    { std::lock_guard<std::mutex> l(m_mutex); onWrite = m_onWrite; convert = m_onWriteConvert; seconds = m_timeout; topology = m_topology; }
    if (onWrite) onWrite(value);
    T converted = convert ? convert(value) : value;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (m_writing) throw std::logic_error("Concurrent CSPOutPort::write");
      expirePull();
      m_writing = true;
      m_offer.reset(new T(converted));
      m_changed.notify_all();
    }
    csp_detail::Finally finishWrite([this] { std::lock_guard<std::mutex> lock(m_mutex); finish(); });
    const auto deadline = csp_detail::Clock::now() + std::chrono::duration<double>(seconds);
    do
    {
      std::string target;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_closed) { finish(); return false; }
        if (!m_offer) { finish(); return true; }
        target = m_push;
      }
      if (target.empty())
      {
        select();
        std::lock_guard<std::mutex> lock(m_mutex);
        target = m_push;
      }
      if (!target.empty())
      {
        std::shared_ptr<OutPortDuplexConnector> connection;
        bool little = true;
        OctetSeq bytes;
        bool send = false;
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          auto it = m_peers.find(target);
          if (it != m_peers.end() && m_offer && m_pull.empty())
          {
            connection = it->second.connector;
            little = it->second.little;
            send = csp_detail::encode(*m_offer, bytes, little);
            if (send) m_offer.reset(); // Exclude a racing pull before releasing the lock.
          }
          m_push.clear();
        }
        if (send)
        {
          bool ok = false;
          try { auto data = csp_detail::toBytes(bytes, little); ok = connection->send(data) == DataPortStatus::PORT_OK; }
          catch (const CORBA::Exception&) {}
          std::lock_guard<std::mutex> lock(m_mutex);
          finish();
          return ok;
        }
      }
      std::unique_lock<std::mutex> lock(m_mutex);
      if (m_closed) { finish(); return false; }
      if (!m_offer) { finish(); return true; }
      if ((m_peers.empty() && m_topology != topology) || csp_detail::Clock::now() >= deadline) { finish(); return false; }
      m_changed.wait_for(lock, std::chrono::milliseconds(1));
    } while (true);
  }
protected:
  /*!
   * @if jp
   * @brief 接続プロファイルへ提供インターフェースの情報を公開する。
   * @param profile 接続プロファイル。
   * @else
   * @brief Publish provided interface information in the connector profile.
   * @param profile Connector profile.
   * @endif
   */
  ReturnCode_t publishInterfaces(ConnectorProfile& profile) override
  {
    if (m_closed) return PRECONDITION_NOT_MET;
    auto validationStatus = csp_detail::validate(profile);
    if (validationStatus != RTC_OK) return validationStatus;
    auto* provider = dynamic_cast<OutPortCSPProvider*>(OutPortProviderFactory::instance().createObject("csp_channel"));
    if (!provider) return RTC_ERROR;
    auto info = csp_detail::connectorInfo(profile);
    { std::lock_guard<std::mutex> lock(m_mutex); info.properties << m_properties; }
    auto connection = std::make_shared<OutPortDuplexConnector>(info, provider, m_listeners);
    const std::string id(profile.connector_id.in());
    connection->setReadinessListener([this] { return selectionReadiness(); });
    connection->setReadableReadinessListener([this, id](bool, CSP::InPortCsp_ptr ref) { return readable(id, ref); });
    connection->setReadListener([this, id](ByteData& data) {
      OctetSeq_var bytes;
      auto status = get(id, bytes.out());
      if (status == PORT_OK) data = csp_detail::toBytes(bytes.in());
      return status == PORT_OK ? BufferStatus::OK : status == BUFFER_TIMEOUT ? BufferStatus::TIMEOUT : BufferStatus::PRECONDITION_NOT_MET;
    });
    connection->setWriteListener([this](ByteData& data) {
      T value{};
      if (!csp_detail::decode(csp_detail::toOctets(data), value, data.getEndian())) return DataPortStatus::PORT_ERROR;
      return write(value) ? DataPortStatus::PORT_OK : DataPortStatus::BUFFER_TIMEOUT;
    });
    Peer peer;
    peer.connector = connection;
    peer.local = provider->object();
    peer.little = csp_detail::littleEndian(profile);
    csp_detail::publish(profile.properties, "dataport.csp_channel.outport_ior", peer.local);
    CORBA_SeqUtil::push_back(profile.properties, NVUtil::newNV("dataport.csp_channel.outport_ref", peer.local));
    publishSelection(profile, getPortRef());
    connection->setEndian(peer.little);
    {
      // PortBase::notify_connect already owns m_connectorsMutex.
      m_connectors.push_back(connection.get());
    }
    connectSelection(id);
    std::lock_guard<std::mutex> lock(m_mutex);
    m_peers[id] = peer;
    ++m_topology;
    return RTC_OK;
  }
  /*!
   * @if jp
   * @brief 相手のインターフェースと固定メタデータを購読する。
   * @param profile 接続プロファイル。
   * @else
   * @brief Subscribe to peer interfaces and fixed metadata.
   * @param profile Connector profile.
   * @endif
   */
  ReturnCode_t subscribeInterfaces(const ConnectorProfile& profile) override
  {
    try
    {
      auto* baseConsumer = InPortConsumerFactory::instance().createObject("csp_channel");
      auto* typedConsumer = dynamic_cast<InPortCSPConsumer*>(baseConsumer);
      if (!typedConsumer) { if (baseConsumer) InPortConsumerFactory::instance().deleteObject(baseConsumer); return RTC_ERROR; }
      auto consumer = std::shared_ptr<InPortCSPConsumer>(typedConsumer, [](InPortCSPConsumer* value) { InPortConsumer* base = value; InPortConsumerFactory::instance().deleteObject(base); });
      coil::Properties options;
      { std::lock_guard<std::mutex> lock(m_mutex); options.setProperty("channel_timeout", std::to_string(m_timeout)); }
      consumer->init(options);
      if (!consumer->subscribeInterface(profile.properties)) return BAD_PARAMETER;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& peer = m_peers.at(profile.connector_id.in());
        peer.remote = consumer->object();
        peer.connector->setConsumer(consumer);
      }
      subscribeSelection(profile, getPortRef());
      if (m_listeners) { auto info = csp_detail::connectorInfo(profile); m_listeners->notify(ConnectorListenerType::ON_CONNECT, info); }
      return RTC_OK;
    }
    catch (const CORBA::Exception&) { return RTC_ERROR; }
  }
  /*!
   * @if jp
   * @brief 接続に対応する購読情報を解除する。
   * @param profile 接続プロファイル。
   * @else
   * @brief Remove subscriptions associated with the connection.
   * @param profile Connector profile.
   * @endif
   */
  void unsubscribeInterfaces(const ConnectorProfile& profile) override
  {
    std::shared_ptr<OutPortDuplexConnector> connector;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = m_peers.find(profile.connector_id.in());
      if (it == m_peers.end()) return;
      connector = it->second.connector;
      if (m_push == it->first) m_push.clear();
      if (m_pull == it->first) m_pull.clear();
      m_peers.erase(it);
      ++m_topology;
      m_changed.notify_all();
    }
    disconnectSelection(profile.connector_id.in());
    {
      // PortBase::notify_disconnect already owns m_connectorsMutex.
      m_connectors.erase(std::remove(m_connectors.begin(), m_connectors.end(), connector.get()), m_connectors.end());
    }
    connector->disconnect();
  }
private:
  /*!
   * @if jp
   * @brief `peers`に対応する処理を行う。
   * @else
   * @brief Perform the peers operation.
   * @endif
   */
  std::map<std::string, Peer> peers() const
  { std::lock_guard<std::mutex> lock(m_mutex); return m_peers; }
  /*!
   * @if jp
   * @brief `finish`に対応する処理を行う。
   * @else
   * @brief Perform the finish operation.
   * @endif
   */
  void finish() // m_mutex held
  { m_offer.reset(); m_writing = m_armed = false; m_push.clear(); m_pull.clear(); }
  /*!
   * @if jp
   * @brief `expire Pull`に対応する処理を行う。
   * @else
   * @brief Perform the expire pull operation.
   * @endif
   */
  void expirePull() // m_mutex held
  {
    if (!m_pull.empty() && csp_detail::Clock::now() >= m_pullExpires) m_pull.clear();
  }
  /*!
   * @if jp
   * @brief `readable`に対応する処理を行う。
   * @param id `id`の値。
   * @param provider 受信側のInPortCsp参照。
   * @else
   * @brief Perform the readable operation.
   * @param id Value of id.
   * @param provider Reference to the receiver InPortCsp interface.
   * @endif
   */
  CSP::Readiness readable(const std::string& id, CSP::InPortCsp_ptr provider)
  {
    CSP::InPortCsp_var remote;
    CSP::NVList options;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = m_peers.find(id);
      if (it == m_peers.end()) return selectionReadiness();
      remote = it->second.remote;
      options = m_option;
    }
    if (CORBA::is_nil(provider) || CORBA::is_nil(remote) || !remote->_is_equivalent(provider)) return selectionReadiness();
    try { if (options.length() && !remote->match_condition(options)) return selectionReadiness(); }
    catch (const CORBA::Exception&) { return CSP::NOT_READY; }
    std::lock_guard<std::mutex> lock(m_mutex);
    expirePull();
    if (m_closed) return selectionReadiness();
    if (!m_push.empty() || (!m_pull.empty() && m_pull != id)) return selectionReadiness();
    if (!m_offer && !m_armed) return selectionReadiness();
    if (m_pull == id) return CSP::READY;
    auto decision = claimSelectionReadiness(true, id);
    if (decision != CSP::READY) return decision;
    m_pull = id;
    m_pullExpires = csp_detail::Clock::now() + std::chrono::duration_cast<csp_detail::Clock::duration>(std::chrono::duration<double>(m_timeout));
    m_changed.notify_all();
    return CSP::READY;
  }
  /*!
   * @if jp
   * @brief 送信側からCDRデータを取り出す。
   * @param id `id`の値。
   * @param bytes `bytes`の値。
   * @else
   * @brief Retrieve CDR data from the sender.
   * @param id Value of id.
   * @param bytes Value of bytes.
   * @endif
   */
  PortStatus get(const std::string& id, OctetSeq_out bytes)
  {
    bytes = new OctetSeq;
    std::unique_lock<std::mutex> lock(m_mutex);
    expirePull();
    if (m_pull != id) return PORT_ERROR;
    if (!m_changed.wait_for(lock, std::chrono::duration<double>(m_timeout), [this, &id]() {
      return m_closed || m_offer || m_pull != id;
    })) { m_pull.clear(); return BUFFER_TIMEOUT; }
    auto peer = m_peers.find(id);
    if (m_closed || m_pull != id || peer == m_peers.end() || !m_offer) return PORT_ERROR;
    if (!csp_detail::encode(*m_offer, *bytes.ptr(), peer->second.little)) return PORT_ERROR;
    m_offer.reset();
    m_pull.clear();
    m_changed.notify_all();
    return PORT_OK;
  }
  std::string m_name;
  T& m_value;
  mutable std::mutex m_mutex;
  std::recursive_mutex m_io;
  csp_detail::CallGate m_operations;
  std::atomic<bool> m_closed{false};
  bool m_retry{false};
  std::condition_variable m_changed;
  std::map<std::string, Peer> m_peers;
  std::unique_ptr<T> m_offer;
  CSP::NVList m_option;
  std::function<void(const T&)> m_onWrite;
  std::function<T(const T&)> m_onWriteConvert;
  double m_timeout{10.0};
  size_t m_topology{0};
  bool m_writing{false}, m_armed{false};
  std::string m_push, m_pull;
  csp_detail::Clock::time_point m_pullExpires;
};
template<class T> CSPOutPort<T>::Peer::~Peer() = default;

}
#endif
