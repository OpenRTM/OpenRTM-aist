// -*- C++ -*-
/*!
 * @file CSPArbitration.cpp
 * @if jp
 * @brief 同一プロセス内のManagerの再走査権を調停する。 実装。
 * @else
 * @brief Arbitrate rescan grants among managers in the same process. Implementation.
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

#include <rtm/CSPPort.h>
#include <coil/UUID.h>
#include <map>
#include <iomanip>
#include <sstream>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

namespace RTC {
csp_detail::Selection::Selection() = default;
csp_detail::Selection::~Selection() = default;
namespace {
bool priorityKey(const std::string& key) {
  if (key.size() < 49 || key[32] != ':') return false;
  for (size_t i = 0; i < key.size(); ++i)
    if (i != 32 && !((key[i] >= '0' && key[i] <= '9') || (key[i] >= 'a' && key[i] <= 'f'))) return false;
  return true;
}
bool differentProcess(const std::string& a, const std::string& b) { return a.substr(0, 32) != b.substr(0, 32); }
long processId() {
#ifdef _WIN32
  return _getpid();
#else
  return getpid();
#endif
}
}
CSP::Readiness csp_detail::Selection::claimReadiness(CSPSelectable* port, bool remote, const std::string& connector) {
  std::lock_guard<std::mutex> lock(mutex);
  if (!active || cancelled || (winner && winner != port)) return CSP::NOT_SELECTING;
  if (std::find(candidates.begin(), candidates.end(), port) == candidates.end()) return CSP::NOT_READY;
  if (remote && !parked && !winner) {
    auto connections = std::atomic_load(&port->m_connections);
    if (connections) {
      auto found = connections->find(connector);
      if (found != connections->end()) for (const auto& peer : found->second) {
        if (!peer.requestArbitration || peer.managerId == identity) continue;
        if (peer.priority < identity) { yieldScan = true; ++yielded; }
        if (differentProcess(peer.managerId, identity)) remotePending[port][connector] = peer.priority;
      }
    }
    if (std::find(pending.begin(), pending.end(), port) == pending.end()) pending.push_back(port);
    ++deferred;
    changed.notify_all();
    return CSP::SCANNING;
  }
  winner = port;
  changed.notify_all();
  return CSP::READY;
}
std::vector<CSPSelectable*> csp_detail::Selection::remoteRescan() {
  std::lock_guard<std::mutex> lock(mutex);
  if (!active || !parked || winner || cancelled) return {};
  for (auto* port : pending) {
    auto requests = remotePending.find(port);
    if (requests == remotePending.end()) continue;
    bool higher = false, lower = false;
    auto connections = std::atomic_load(&port->m_connections);
    if (!connections) continue;
    for (const auto& request : requests->second) {
      if (!connections->count(request.first)) continue;
      higher |= request.second < identity;
      lower |= identity < request.second;
    }
    if (higher || !lower) continue;
    parked = false; yieldScan = false;
    ++grants; ++priorityRescans;
    return {port}; // Retain the candidate even when this retry is too early.
  }
  return {};
}
void CSPSelectable::rememberUnavailable(const std::string& connector, CSP::Readiness reply) {
  if (reply != CSP::SCANNING && reply != CSP::NOT_SELECTING) return;
  auto state = std::atomic_load(&m_selection);
  auto connections = std::atomic_load(&m_connections);
  if (!state || !connections) return;
  auto found = connections->find(connector);
  if (found == connections->end()) return;
  std::lock_guard<std::mutex> lock(state->mutex);
  if (!state->active || state->winner || state->cancelled) return;
  for (const auto& peer : found->second) {
    if (!peer.requestArbitration || peer.managerId == state->identity) continue;
    if (std::find(state->pending.begin(), state->pending.end(), this) == state->pending.end()) state->pending.push_back(this);
    if (differentProcess(peer.managerId, state->identity)) {
      state->remotePending[this][connector] = peer.priority;
      ++state->unavailableRetained;
    }
  }
}
void CSPSelectable::publishSelection(ConnectorProfile& profile, PortService_ptr self) {
  auto state = std::atomic_load(&m_selection);
  for (CORBA::ULong i = 0; i < profile.ports.length(); ++i) {
    if (!profile.ports[i]->_is_equivalent(self)) continue;
    const auto prefix = "dataport.csp.arbitration." + std::to_string(i) + ".";
    csp_detail::setProperty(profile.properties, (prefix + "version").c_str(), state ? "1" : "");
    csp_detail::setProperty(profile.properties, (prefix + "manager_id").c_str(), state ? state->identity.c_str() : "");
    csp_detail::setProperty(profile.properties, (prefix + "priority").c_str(), state ? state->identity.c_str() : "");
    csp_detail::setProperty(profile.properties, (prefix + "request_arbitration").c_str(), state ? "1" : "");
  }
}
void CSPSelectable::subscribeSelection(const ConnectorProfile& profile, PortService_ptr self) {
  std::vector<PeerPriority> peers;
  for (CORBA::ULong i = 0; i < profile.ports.length(); ++i) {
    if (profile.ports[i]->_is_equivalent(self)) continue;
    const auto prefix = "dataport.csp.arbitration." + std::to_string(i) + ".";
    auto identity = csp_detail::property(profile.properties, (prefix + "manager_id").c_str());
    auto priority = csp_detail::property(profile.properties, (prefix + "priority").c_str());
    if (csp_detail::property(profile.properties, (prefix + "version").c_str()) != "1" ||
        !priorityKey(identity) || !priorityKey(priority)) continue;
    const bool requestArbitration =
      csp_detail::property(profile.properties, (prefix + "request_arbitration").c_str()) == "1";
    peers.emplace_back();
    auto& peer = peers.back();
    peer.portIndex = i;
    peer.managerId = std::move(identity);
    peer.priority = std::move(priority);
    peer.requestArbitration = requestArbitration;
  }
  std::lock_guard<std::mutex> lock(m_metadataMutex);
  auto snapshot = std::atomic_load(&m_connections);
  auto updated = std::make_shared<ConnectionPriorities>(snapshot ? *snapshot : ConnectionPriorities{});
  (*updated)[profile.connector_id.in()] = std::move(peers);
  std::atomic_store(&m_connections, std::shared_ptr<const ConnectionPriorities>(updated));
}
void CSPSelectable::disconnectSelection(const std::string& id) {
  bool empty;
  {
    std::lock_guard<std::mutex> lock(m_metadataMutex);
    auto snapshot = std::atomic_load(&m_connections);
    auto updated = std::make_shared<ConnectionPriorities>(snapshot ? *snapshot : ConnectionPriorities{});
    updated->erase(id); empty = updated->empty();
    std::atomic_store(&m_connections, std::shared_ptr<const ConnectionPriorities>(updated));
  }
  auto state = std::atomic_load(&m_selection);
  if (state) {
    std::lock_guard<std::mutex> lock(state->mutex);
    state->remotePending[this].erase(id);
    if (state->remotePending[this].empty()) state->remotePending.erase(this);
    if (empty) state->pending.erase(std::remove(state->pending.begin(), state->pending.end(), this), state->pending.end());
  }
  CSPArbitration::instance().disconnect(id, this);
}
CSPSelectable::~CSPSelectable() { if (m_manager) m_manager->remove(this); CSPArbitration::instance().remove(this); }
void CSPSelectable::releaseManager() { endSelection(false); if (m_manager) m_manager->remove(this); }
void CSPSelectable::setManager(CSPManager* manager) { releaseManager(); if (manager) manager->add(this); }

/*!
 * @if jp
 * @brief `Impl`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of Impl.
 * @endif
 */
struct CSPArbitration::Impl {
  /*!
   * @if jp
   * @brief `End`の状態と操作をまとめる。
   * @else
   * @brief Encapsulate the state and operations of End.
   * @endif
   */
  struct End { CSPSelectable* port; std::weak_ptr<csp_detail::Selection> state; };
  std::mutex mutex;
  size_t sequence{0};
  long pid{0};
  std::string process;
  std::map<size_t, std::weak_ptr<csp_detail::Selection>> managers;
  std::map<std::string, std::vector<End>> connections;
  std::weak_ptr<csp_detail::Selection> owner;
  /*!
   * @if jp
   * @brief `ready`に対応する処理を行う。
   * @param self 対象インスタンス。
   * @param port 操作対象のポート。
   * @else
   * @brief Perform the ready operation.
   * @param self Target instance.
   * @param port Port to operate on.
   * @endif
   */
  bool ready(const std::shared_ptr<csp_detail::Selection>& self, CSPSelectable* port) {
    bool known = false;
    for (const auto& connection : connections) {
      const auto& ends = connection.second;
      if (std::none_of(ends.begin(), ends.end(), [port](const End& end) { return end.port == port; })) continue;
      known = true;
      if (ends.size() < 2) {
        auto metadata = port->getPeerPriorities();
        auto found = metadata.find(connection.first);
        if (found != metadata.end() && std::any_of(found->second.begin(), found->second.end(),
              [](const CSPSelectable::PeerPriority& peer) { return peer.requestArbitration; })) continue;
        return true; // Standalone/legacy peer; priority peers use retained retries.
      }
      for (const auto& end : ends) {
        if (end.port == port) continue;
        auto peer = end.state.lock();
        if (!peer) return true;
        if (peer == self) continue;
        std::lock_guard<std::mutex> l(peer->mutex);
        if (peer->active && peer->parked && !peer->cancelled && !peer->winner) return true;
      }
    }
    return !known;
  }
};
CSPArbitration::CSPArbitration() : m_impl(new Impl) {}
CSPArbitration::~CSPArbitration() = default;
CSPArbitration& CSPArbitration::instance() { static CSPArbitration value; return value; }
size_t CSPArbitration::registerManager(const std::shared_ptr<csp_detail::Selection>& state) {
  std::lock_guard<std::mutex> l(m_impl->mutex);
  auto priority = m_impl->sequence++;
  if (m_impl->pid != processId()) {
    m_impl->pid = processId();
    std::unique_ptr<coil::UUID> uuid(coil::UUID_Generator::generateUUID(2, 0x01));
    if (!uuid) throw std::runtime_error("Cannot create CSP process identity");
    m_impl->process = uuid->to_string();
    auto& key = m_impl->process;
    key.erase(std::remove(key.begin(), key.end(), '-'), key.end());
    std::transform(key.begin(), key.end(), key.begin(), [](char c) { return c >= 'A' && c <= 'F' ? c - 'A' + 'a' : c; });
  }
  std::ostringstream identity;
  identity << m_impl->process << ':' << std::hex << std::setfill('0') << std::setw(16) << priority;
  state->identity = identity.str();
  m_impl->managers[priority] = state;
  return priority;
}
void CSPArbitration::connect(const std::string& id, CSPSelectable* port, std::weak_ptr<csp_detail::Selection> state) {
  std::lock_guard<std::mutex> l(m_impl->mutex);
  auto& ends = m_impl->connections[id];
  auto it = std::find_if(ends.begin(), ends.end(), [port](const Impl::End& end) { return end.port == port; });
  if (it == ends.end()) ends.push_back({port, state}); else it->state = state;
}
void CSPArbitration::disconnect(const std::string& id, CSPSelectable* port) {
  std::lock_guard<std::mutex> l(m_impl->mutex);
  auto it = m_impl->connections.find(id);
  if (it == m_impl->connections.end()) return;
  auto& ends = it->second;
  ends.erase(std::remove_if(ends.begin(), ends.end(), [port](const Impl::End& end) { return end.port == port; }), ends.end());
  if (ends.empty()) m_impl->connections.erase(it);
}
void CSPArbitration::update(CSPSelectable* port, std::weak_ptr<csp_detail::Selection> state) {
  std::lock_guard<std::mutex> l(m_impl->mutex);
  for (auto& entry : m_impl->connections) for (auto& end : entry.second) if (end.port == port) end.state = state;
}
void CSPArbitration::remove(CSPSelectable* port) {
  std::lock_guard<std::mutex> l(m_impl->mutex);
  for (auto it = m_impl->connections.begin(); it != m_impl->connections.end();) {
    auto& ends = it->second;
    ends.erase(std::remove_if(ends.begin(), ends.end(), [port](const Impl::End& end) { return end.port == port; }), ends.end());
    if (ends.empty()) it = m_impl->connections.erase(it); else ++it;
  }
}
std::vector<CSPSelectable*> CSPArbitration::acquire(const std::shared_ptr<csp_detail::Selection>& state) {
  std::lock_guard<std::mutex> registry(m_impl->mutex);
  if (!m_impl->owner.expired()) return {};
  for (auto it = m_impl->managers.begin(); it != m_impl->managers.end();) {
    auto candidate = it->second.lock();
    if (!candidate) { it = m_impl->managers.erase(it); continue; }
    ++it;
    std::vector<CSPSelectable*> pending;
    {
      std::lock_guard<std::mutex> l(candidate->mutex);
      if (!candidate->active || !candidate->parked || candidate->winner || candidate->cancelled) continue;
      pending = candidate->pending;
    }
    std::vector<CSPSelectable*> eligible;
    for (auto* port : pending) if (m_impl->ready(candidate, port)) eligible.push_back(port);
    if (eligible.empty()) continue;
    if (candidate != state) { candidate->changed.notify_all(); return {}; }
    std::lock_guard<std::mutex> l(state->mutex);
    if (!state->active || !state->parked || state->winner || state->cancelled) return {};
    state->parked = false;
    for (auto* port : eligible) state->pending.erase(std::remove(state->pending.begin(), state->pending.end(), port), state->pending.end());
    ++state->grants;
    m_impl->owner = state;
    return eligible;
  }
  return {};
}
void CSPArbitration::release(const std::shared_ptr<csp_detail::Selection>& state) {
  std::lock_guard<std::mutex> l(m_impl->mutex);
  if (m_impl->owner.lock() == state) m_impl->owner.reset();
}
}
