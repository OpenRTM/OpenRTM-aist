// -*- C++ -*-
/*!
 * @file CSPInPort.h
 * @if jp
 * @brief 型付き入力と受信予約を管理する。 宣言。
 * @else
 * @brief Manage typed input and receive reservations. Declarations.
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

#ifndef RTC_CSPINPORT_H
#define RTC_CSPINPORT_H
#include <rtm/idl/CSPPortSkel.h>
#include <rtm/InPortBase.h>
#include <rtm/Typename.h>
#include <rtm/CSPTransport.h>
#include <rtm/PortCallback.h>

namespace RTC
{
/** Typed CSP channel. buffer.length=0 enables a rendezvous; default capacity=8.
 * Application reads are serialized. close() cancels waiting I/O and drains
 * active operations before the transport and callbacks are released.
 */
/*!
 * @if jp
 * @brief 型付き入力と受信予約を管理する。
 * @else
 * @brief Manage typed input and receive reservations.
 * @endif
 */
template<class T>
class CSPInPort : public InPortBase, public CSPSelectable
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
    std::shared_ptr<InPortDuplexConnector> connector;
    CSP::InPortCsp_var local;
    CSP::OutPortCsp_var remote;
    bool little{true};
    std::string event;
  };
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  struct Item { ~Item(); T value; std::string event; size_t ticket; std::string id; OctetSeq bytes; };
public:
  /*!
   * @if jp
   * @brief `CSPIn Port`に対応する処理を行う。
   * @param name 識別に使用する名前。
   * @param value 設定または受け渡す値。
   * @param manager 関連付けるManager。
   * @else
   * @brief Perform the cspin port operation.
   * @param name Name identifying the object.
   * @param value Value to configure or transfer.
   * @param manager Manager to associate with the object.
   * @endif
   */
  CSPInPort(const char* name, T& value, CSPManager* manager = nullptr)
    : InPortBase(name, csp_detail::repositoryId<T>()), m_name(name), m_value(value)
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
  ~CSPInPort() override { close(); }
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
    m_properties << props;
    setSyncMode(coil::toBool(m_properties.getProperty("csp.sync_wait"), "YES", "NO", false));
    m_timeout = csp_detail::timeout(m_properties);
    int capacity = 8;
    if (!coil::stringTo(capacity, m_properties.getProperty("buffer.length", "8").c_str()) || capacity < 0)
      throw std::invalid_argument("Invalid CSP buffer.length");
    m_capacity = static_cast<size_t>(capacity);
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
  bool isInput() const override { return true; }
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
    if (!selected) m_reservation.clear();
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
      if (!m_queue.empty()) return claimSelection();
      if (!m_reservation.empty() && csp_detail::Clock::now() < m_expires) return true;
      if (!m_reservation.empty()) m_reservation.clear();
    }
    // Do not hold a channel lock across CORBA calls: the peer may call back.
    for (auto& peer : peers())
    {
      if (!acceptEvent(peer.second.event)) continue;
      if (CORBA::is_nil(peer.second.remote)) continue;
      try
      {
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          if (!m_queue.empty()) return claimSelection();
          if (m_receiving || m_probe || !m_reservation.empty()) continue;
          m_probe = true;
        }
        csp_detail::Finally probe([this] { std::lock_guard<std::mutex> lock(m_mutex); m_probe = false; });
        auto reply = peer.second.connector->consumer()->readableReadiness(m_retry, peer.second.local);
        if (reply != CSP::READY) { rememberUnavailable(peer.first, reply); continue; }
        if (!claimSelection()) return false;
        ByteData received;
        if (peer.second.connector->consumer()->get(received) != DataPortStatus::PORT_OK) continue;
        return receive(peer.first, csp_detail::toOctets(received), false) == PORT_OK;
      }
      catch (const CORBA::Exception&) {}
    }
    return false;
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
  bool read(std::string connector = "") override
  {
    csp_detail::Call operation(m_operations);
    if (!operation) return false;
    std::lock_guard<std::recursive_mutex> serial(m_io);
    if (m_closed) return false;
    std::function<void()> onRead;
    double seconds;
    size_t topology;
    { std::lock_guard<std::mutex> lock(m_mutex); onRead = m_onRead; seconds = m_timeout; topology = m_topology; }
    if (onRead) onRead();
    if (!connector.empty()) return false; // Selection is across all CSP connectors.
    const auto deadline = csp_detail::Clock::now() + std::chrono::duration<double>(seconds);
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_reading = true;
    }
    csp_detail::Finally finishRead([this] {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_reading = m_armed = false; m_reservation.clear();
    });
    do
    {
      select();
      std::unique_lock<std::mutex> lock(m_mutex);
      if (!m_queue.empty())
      {
        auto item = std::move(m_queue.front());
        m_queue.pop_front();
        auto peer = m_peers.find(item.id);
        auto connection = peer == m_peers.end() ? std::shared_ptr<InPortDuplexConnector>() : peer->second.connector;
        auto convert = m_onReadConvert;
        m_lastEvent = item.event;
        m_reading = m_armed = false;
        m_reservation.clear();
        m_changed.notify_all();
        lock.unlock();
        if (connection) {
          auto data = csp_detail::toBytes(item.bytes, connection->isLittleEndian());
          auto info = connection->profile();
          if (m_listeners) m_listeners->notifyIn(ConnectorDataListenerType::ON_BUFFER_READ, info, data);
          auto bytes = csp_detail::toOctets(data);
          if (!csp_detail::decode(bytes, item.value, connection->isLittleEndian())) return false;
        }
        if (convert) item.value = convert(item.value);
        { std::lock_guard<std::mutex> valueLock(m_mutex); m_value = item.value; }
        return true;
      }
      if (m_closed || (m_peers.empty() && m_topology != topology) || csp_detail::Clock::now() >= deadline)
      {
        m_reading = m_armed = false;
        m_reservation.clear();
        return false;
      }
      m_changed.wait_for(lock, std::chrono::milliseconds(1));
    } while (true);
  }
  /*!
   * @if jp
   * @brief 選択済み入力のデータまたはイベントを取得する。
   * @else
   * @brief Get the data or event of the selected input.
   * @endif
   */
  bool readData() { return read(); }
  /*!
   * @if jp
   * @brief `selected IO`に対応する処理を行う。
   * @else
   * @brief Perform the selected io operation.
   * @endif
   */
  bool selectedIO() override { return readData(); }
  /*!
   * @if jp
   * @brief `is New`に対応する処理を行う。
   * @else
   * @brief Perform the is new operation.
   * @endif
   */
  bool isNew() const { std::lock_guard<std::mutex> lock(m_mutex); return !m_queue.empty(); }
  /*!
   * @if jp
   * @brief `is Empty`に対応する処理を行う。
   * @else
   * @brief Perform the is empty operation.
   * @endif
   */
  bool isEmpty() const { return !isNew(); }
  /*!
   * @if jp
   * @brief 通信前に評価する入力の受付条件を設定する。
   * @param guard `guard`の値。
   * @else
   * @brief Set the input admission predicate evaluated before communication.
   * @param guard Value of guard.
   * @endif
   */
  void setGuard(std::function<bool(const CSP::NVList&)> guard) { std::lock_guard<std::mutex> l(m_mutex); m_guard = std::move(guard); }
  /*!
   * @if jp
   * @brief `Guard`を解除する。
   * @else
   * @brief Clear guard.
   * @endif
   */
  void clearGuard() { setGuard({}); }
  /*!
   * @if jp
   * @brief `On Read`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on read.
   * @param callback Value of callback.
   * @endif
   */
  void setOnRead(std::function<void()> callback) { std::lock_guard<std::mutex> l(m_mutex); m_onRead = std::move(callback); }
  /*!
   * @if jp
   * @brief `On Read`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on read.
   * @param callback Value of callback.
   * @endif
   */
  void setOnRead(OnRead<T>* callback) { setOnRead(callback ? std::function<void()>([callback] { (*callback)(); }) : std::function<void()>()); }
  /*!
   * @if jp
   * @brief `On Read Convert`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on read convert.
   * @param callback Value of callback.
   * @endif
   */
  void setOnReadConvert(std::function<T(const T&)> callback) { std::lock_guard<std::mutex> l(m_mutex); m_onReadConvert = std::move(callback); }
  /*!
   * @if jp
   * @brief `On Read Convert`を設定する。
   * @param callback `callback`の値。
   * @else
   * @brief Set on read convert.
   * @param callback Value of callback.
   * @endif
   */
  void setOnReadConvert(OnReadConvert<T>* callback) { setOnReadConvert(callback ? std::function<T(const T&)>([callback](const T& data) { return (*callback)(data); }) : std::function<T(const T&)>()); }
protected:
  /*!
   * @if jp
   * @brief `accept Event`に対応する処理を行う。
   * @else
   * @brief Perform the accept event operation.
   * @endif
   */
  virtual bool acceptEvent(const std::string&) const { return true; }
  /*!
   * @if jp
   * @brief `last Event`に対応する処理を行う。
   * @else
   * @brief Perform the last event operation.
   * @endif
   */
  const std::string& lastEvent() const { return m_lastEvent; }
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
    auto* provider = dynamic_cast<InPortCSPProvider*>(InPortProviderFactory::instance().createObject("csp_channel"));
    if (!provider) return RTC_ERROR;
    auto info = csp_detail::connectorInfo(profile);
    { std::lock_guard<std::mutex> lock(m_mutex); info.properties << m_properties; }
    auto connection = std::make_shared<InPortDuplexConnector>(info, provider, m_listeners);
    const std::string id(profile.connector_id.in());
    connection->setReadinessListener([this] { return selectionReadiness(); });
    connection->setWritableReadinessListener([this, id](bool, const CSP::NVList& option) { return reserve(id, option); });
    connection->setCondition([this](const CSP::NVList& option) { return matches(option); });
    connection->setWriteListener([this, id](ByteData& data) {
      return receive(id, csp_detail::toOctets(data), true) == PORT_OK ? BufferStatus::OK : BufferStatus::PRECONDITION_NOT_MET;
    });
    auto* link = connection.get();
    connection->setReadListener([this, link](ByteData& data) {
      if (!read()) return DataPortStatus::BUFFER_TIMEOUT;
      T value;
      { std::lock_guard<std::mutex> lock(m_mutex); value = m_value; }
      OctetSeq bytes;
      if (!csp_detail::encode(value, bytes, link->isLittleEndian())) return DataPortStatus::PORT_ERROR;
      data = csp_detail::toBytes(bytes, link->isLittleEndian());
      return DataPortStatus::PORT_OK;
    });
    Peer peer;
    peer.connector = connection;
    peer.local = provider->object();
    peer.little = csp_detail::littleEndian(profile);
    peer.event = csp_detail::property(profile.properties, "dataport.fsm_event_name", csp_detail::property(profile.properties, "fsm_event_name").c_str());
    csp_detail::publish(profile.properties, "dataport.csp_channel.inport_ior", peer.local);
    CORBA_SeqUtil::push_back(profile.properties, NVUtil::newNV("dataport.csp_channel.inport_ref", peer.local));
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
      auto* baseConsumer = OutPortConsumerFactory::instance().createObject("csp_channel");
      auto* typedConsumer = dynamic_cast<OutPortCSPConsumer*>(baseConsumer);
      if (!typedConsumer) { if (baseConsumer) OutPortConsumerFactory::instance().deleteObject(baseConsumer); return RTC_ERROR; }
      auto consumer = std::shared_ptr<OutPortCSPConsumer>(typedConsumer, [](OutPortCSPConsumer* value) { OutPortConsumer* base = value; OutPortConsumerFactory::instance().deleteObject(base); });
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
    std::shared_ptr<InPortDuplexConnector> connector;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = m_peers.find(profile.connector_id.in());
      if (it == m_peers.end()) return;
      connector = it->second.connector;
      if (m_reservation == it->first) m_reservation.clear();
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
   * @brief `matches`に対応する処理を行う。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Perform the matches operation.
   * @param option Options used for guard matching.
   * @endif
   */
  bool matches(const CSP::NVList& option) const {
    std::function<bool(const CSP::NVList&)> guard;
    { std::lock_guard<std::mutex> l(m_mutex); if (m_closed) return false; guard = m_guard; }
    return !guard || guard(option);
  }
  /*!
   * @if jp
   * @brief `reserve`に対応する処理を行う。
   * @param id `id`の値。
   * @param option ガード照合に使用するオプション。
   * @else
   * @brief Perform the reserve operation.
   * @param id Value of id.
   * @param option Options used for guard matching.
   * @endif
   */
  CSP::Readiness reserve(const std::string& id, const CSP::NVList& option)
  {
    if (!matches(option)) return selectionReadiness();
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_closed || m_receiving || m_probe) return selectionReadiness();
    const auto now = csp_detail::Clock::now();
    if (!m_reservation.empty() && now >= m_expires) m_reservation.clear();
    if (m_peers.find(id) == m_peers.end()) return selectionReadiness();
    if (!acceptEvent(m_peers.at(id).event)) return selectionReadiness();
    if (!m_reservation.empty()) return m_reservation == id ? CSP::READY : selectionReadiness();
    if (m_queue.size() >= std::max(m_capacity, size_t{1})) return selectionReadiness();
    if ((m_capacity == 0 && !m_reading && !m_armed) || (hasManager() && !m_armed)) return selectionReadiness();
    auto decision = claimSelectionReadiness(true, id);
    if (decision != CSP::READY) return decision;
    m_reservation = id;
    m_expires = now + std::chrono::duration_cast<csp_detail::Clock::duration>(std::chrono::duration<double>(m_timeout));
    return CSP::READY;
  }
  /*!
   * @if jp
   * @brief `receive`に対応する処理を行う。
   * @param id `id`の値。
   * @param bytes `bytes`の値。
   * @param grant `grant`の値。
   * @else
   * @brief Perform the receive operation.
   * @param id Value of id.
   * @param bytes Value of bytes.
   * @param grant Value of grant.
   * @endif
   */
  PortStatus receive(const std::string& id, const OctetSeq& bytes, bool grant)
  {
    Peer peer;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto found = m_peers.find(id);
      if (m_closed || m_receiving || found == m_peers.end()) return PORT_ERROR;
      if (grant && (m_reservation != id || csp_detail::Clock::now() >= m_expires)) return PORT_ERROR;
      if (m_queue.size() >= std::max(m_capacity, size_t{1})) return BUFFER_FULL;
      m_reservation.clear(); m_probe = false; m_receiving = true;
      peer = found->second;
    }
    csp_detail::Finally finishReceive([this] { std::lock_guard<std::mutex> lock(m_mutex); m_receiving = false; m_changed.notify_all(); });
    auto data = csp_detail::toBytes(bytes, peer.little);
    auto info = peer.connector->profile();
    if (m_listeners) m_listeners->notifyIn(ConnectorDataListenerType::ON_RECEIVED, info, data);
    auto converted = csp_detail::toOctets(data);
    T value{};
    if (!csp_detail::decode(converted, value, peer.little)) return PORT_ERROR;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (m_closed || m_peers.find(id) == m_peers.end()) return PORT_ERROR;
      m_queue.push_back(Item{value, peer.event, ++m_ticket, id, converted});
      m_changed.notify_all();
    }
    if (m_listeners) m_listeners->notifyIn(ConnectorDataListenerType::ON_BUFFER_WRITE, info, data);
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
  std::deque<Item> m_queue;
  size_t m_capacity{8}, m_ticket{0}, m_topology{0};
  double m_timeout{10.0};
  bool m_reading{false}, m_armed{false}, m_receiving{false}, m_probe{false};
  std::string m_reservation, m_lastEvent;
  csp_detail::Clock::time_point m_expires;
  std::function<bool(const CSP::NVList&)> m_guard;
  std::function<void()> m_onRead;
  std::function<T(const T&)> m_onReadConvert;
};
template<class T> CSPInPort<T>::Peer::~Peer() = default;
template<class T> CSPInPort<T>::Item::~Item() = default;

}
#endif
