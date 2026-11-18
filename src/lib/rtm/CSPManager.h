// -*- C++ -*-
/*!
 * @file CSPManager.h
 * @if jp
 * @brief CSPポートの選択と再走査を管理する。 宣言。
 * @else
 * @brief Manage CSP port selection and rescanning. Declarations.
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

// CSP port selection and process-local priority arbitration.
#ifndef RTC_CSPMANAGER_H
#define RTC_CSPMANAGER_H
#include <rtm/idl/CSPPortSkel.h>
#include <rtm/idl/RTCSkel.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace RTC {
class CSPSelectable;
class CSPManager;
namespace csp_detail {
using Clock = std::chrono::steady_clock;
/*!
 * @if jp
 * @brief 一回の選択の予約・待機状態と制御ロックを保持する。
 * @else
 * @brief Hold reservations, wait state and the control lock for a selection.
 * @endif
 */
struct Selection {
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  Selection();
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~Selection();
  std::mutex mutex;
  std::condition_variable changed;
  bool active{false}, parked{false}, cancelled{false}, yieldScan{false};
  CSPSelectable* winner{nullptr};
  std::vector<CSPSelectable*> pending;
  std::vector<CSPSelectable*> candidates;
  size_t priority{0}, grants{0}, deferred{0}, timeouts{0};
  std::string identity;
  size_t priorityRescans{0}, yielded{0}, unavailableRetained{0};
  std::map<CSPSelectable*, std::map<std::string, std::string>> remotePending;
  /*!
   * @if jp
   * @brief 固定優先度と選択状態から受付結果を原子的に確定する。
   * @param port 操作対象のポート。
   * @param remote `remote`の値。
   * @param connector `connector`の値。
   * @else
   * @brief Atomically determine admission from the fixed priority and selection state.
   * @param port Port to operate on.
   * @param remote Value of remote.
   * @param connector Value of connector.
   * @endif
   */
  CSP::Readiness claimReadiness(CSPSelectable* port, bool remote = false, const std::string& connector = "");
  /*!
   * @if jp
   * @brief `claim`に対応する処理を行う。
   * @param port 操作対象のポート。
   * @param remote `remote`の値。
   * @else
   * @brief Perform the claim operation.
   * @param port Port to operate on.
   * @param remote Value of remote.
   * @endif
   */
  bool claim(CSPSelectable* port, bool remote = false) { return claimReadiness(port, remote) == CSP::READY; }
  /*!
   * @if jp
   * @brief 固定優先度で許可された遠隔再走査候補を取得する。
   * @else
   * @brief Get remote rescan candidates permitted by fixed priorities.
   * @endif
   */
  std::vector<CSPSelectable*> remoteRescan();
};
}
// One shared registry in the RTC library, rather than one per including DLL.
/*!
 * @if jp
 * @brief 同一プロセス内のManagerの再走査権を調停する。
 * @else
 * @brief Arbitrate rescan grants among managers in the same process.
 * @endif
 */
class CSPArbitration {
public:
  /*!
   * @if jp
   * @brief `instance`に対応する処理を行う。
   * @else
   * @brief Perform the instance operation.
   * @endif
   */
  static CSPArbitration& instance();
  /*!
   * @if jp
   * @brief Managerの選択状態をローカル調停器へ登録する。
   * @param state `state`の値。
   * @else
   * @brief Register the manager selection state with the local arbiter.
   * @param state Value of state.
   * @endif
   */
  size_t registerManager(const std::shared_ptr<csp_detail::Selection>& state);
  /*!
   * @if jp
   * @brief ポートの接続を確立する。
   * @param id `id`の値。
   * @param port 操作対象のポート。
   * @param state `state`の値。
   * @else
   * @brief Establish a port connection.
   * @param id Value of id.
   * @param port Port to operate on.
   * @param state Value of state.
   * @endif
   */
  void connect(const std::string& id, CSPSelectable* port, std::weak_ptr<csp_detail::Selection> state);
  /*!
   * @if jp
   * @brief 接続を切断して通信資源を解除する。
   * @param id `id`の値。
   * @param port 操作対象のポート。
   * @else
   * @brief Disconnect the connector and release communication resources.
   * @param id Value of id.
   * @param port Port to operate on.
   * @endif
   */
  void disconnect(const std::string& id, CSPSelectable* port);
  /*!
   * @if jp
   * @brief `update`に対応する処理を行う。
   * @param port 操作対象のポート。
   * @param state `state`の値。
   * @else
   * @brief Perform the update operation.
   * @param port Port to operate on.
   * @param state Value of state.
   * @endif
   */
  void update(CSPSelectable* port, std::weak_ptr<csp_detail::Selection> state);
  /*!
   * @if jp
   * @brief ``を取り除く。
   * @param port 操作対象のポート。
   * @else
   * @brief Remove .
   * @param port Port to operate on.
   * @endif
   */
  void remove(CSPSelectable* port);
  /*!
   * @if jp
   * @brief ローカル再走査権を取得して対象候補を返す。
   * @param state `state`の値。
   * @else
   * @brief Acquire a local rescan grant and return its candidates.
   * @param state Value of state.
   * @endif
   */
  std::vector<CSPSelectable*> acquire(const std::shared_ptr<csp_detail::Selection>& state);
  /*!
   * @if jp
   * @brief 対応するローカル再走査権を解放する。
   * @param state `state`の値。
   * @else
   * @brief Release the matching local rescan grant.
   * @param state Value of state.
   * @endif
   */
  void release(const std::shared_ptr<csp_detail::Selection>& state);
private:
  struct Impl;
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  CSPArbitration();
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~CSPArbitration();
  std::unique_ptr<Impl> m_impl;
};
/*!
 * @if jp
 * @brief `CSPSelectable`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of CSPSelectable.
 * @endif
 */
class CSPSelectable {
public:
  /*!
   * @if jp
   * @brief `PeerPriority`の状態と操作をまとめる。
   * @else
   * @brief Encapsulate the state and operations of PeerPriority.
   * @endif
   */
  struct PeerPriority { size_t portIndex; std::string managerId, priority; bool requestArbitration; };
  using ConnectionPriorities = std::map<std::string, std::vector<PeerPriority>>;
  /*!
   * @if jp
   * @brief 接続時に保存した相手の固定優先度情報を取得する。
   * @else
   * @brief Get the fixed peer priority information stored at connection time.
   * @endif
   */
  ConnectionPriorities getPeerPriorities() const {
    auto snapshot = std::atomic_load(&m_connections);
    return snapshot ? *snapshot : ConnectionPriorities{};
  }
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  virtual ~CSPSelectable();
  /*!
   * @if jp
   * @brief 通信可能な登録ポートを一つ選択する。
   * @else
   * @brief Select one ready registered port.
   * @endif
   */
  virtual bool select() = 0;
  /*!
   * @if jp
   * @brief 保持した通信候補を再走査する。
   * @else
   * @brief Rescan retained communication candidates.
   * @endif
   */
  virtual bool reselect() { return select(); }
  /*!
   * @if jp
   * @brief 新しい選択に必要な予約・走査状態を初期化する。
   * @else
   * @brief Initialize reservation and scan state for a new selection.
   * @endif
   */
  virtual void beginSelection() = 0;
  /*!
   * @if jp
   * @brief 選択終了を通知し、未選択側の状態を解除する。
   * @param selected `selected`の値。
   * @else
   * @brief Mark the end of selection and clear unselected state.
   * @param selected Value of selected.
   * @endif
   */
  virtual void endSelection(bool selected) = 0;
  /*!
   * @if jp
   * @brief `is Input`に対応する処理を行う。
   * @else
   * @brief Perform the is input operation.
   * @endif
   */
  virtual bool isInput() const = 0;
  /*!
   * @if jp
   * @brief `selected IO`に対応する処理を行う。
   * @else
   * @brief Perform the selected io operation.
   * @endif
   */
  virtual bool selectedIO() = 0;
  /*!
   * @if jp
   * @brief ポートを選択Managerへ関連付ける。
   * @param manager 関連付けるManager。
   * @else
   * @brief Associate the port with a selection manager.
   * @param manager Manager to associate with the object.
   * @endif
   */
  void setManager(CSPManager* manager);
  /*!
   * @if jp
   * @brief ポートと選択Managerの関連付けを解除する。
   * @else
   * @brief Remove the association between the port and its selection manager.
   * @endif
   */
  void releaseManager();
  /*!
   * @if jp
   * @brief `Sync Mode`を設定する。
   * @param enabled `enabled`の値。
   * @else
   * @brief Set sync mode.
   * @param enabled Value of enabled.
   * @endif
   */
  void setSyncMode(bool enabled) { m_sync.store(enabled); }
  /*!
   * @if jp
   * @brief `Sync Mode`を取得する。
   * @else
   * @brief Get sync mode.
   * @endif
   */
  bool getSyncMode() const { return m_sync.load(); }
protected:
  /*!
   * @if jp
   * @brief 接続先の優先度を考慮して選択予約を要求する。
   * @param remote `remote`の値。
   * @param connector `connector`の値。
   * @else
   * @brief Request a selection reservation with the peer priority taken into account.
   * @param remote Value of remote.
   * @param connector Value of connector.
   * @endif
   */
  CSP::Readiness claimSelectionReadiness(bool remote = false, const std::string& connector = "") {
    auto state = std::atomic_load(&m_selection);
    return state ? state->claimReadiness(this, remote, connector) : CSP::READY;
  }
  /*!
   * @if jp
   * @brief `claim Selection`に対応する処理を行う。
   * @param remote `remote`の値。
   * @else
   * @brief Perform the claim selection operation.
   * @param remote Value of remote.
   * @endif
   */
  bool claimSelection(bool remote = false) {
    return claimSelectionReadiness(remote) == CSP::READY;
  }
  /*!
   * @if jp
   * @brief `publish Selection`に対応する処理を行う。
   * @param profile 接続プロファイル。
   * @param self 対象インスタンス。
   * @else
   * @brief Perform the publish selection operation.
   * @param profile Connector profile.
   * @param self Target instance.
   * @endif
   */
  void publishSelection(ConnectorProfile& profile, PortService_ptr self);
  /*!
   * @if jp
   * @brief `subscribe Selection`に対応する処理を行う。
   * @param profile 接続プロファイル。
   * @param self 対象インスタンス。
   * @else
   * @brief Perform the subscribe selection operation.
   * @param profile Connector profile.
   * @param self Target instance.
   * @endif
   */
  void subscribeSelection(const ConnectorProfile& profile, PortService_ptr self);
  /*!
   * @if jp
   * @brief 未受付の遠隔接続を再試行候補として保持する。
   * @param connector `connector`の値。
   * @param reply `reply`の値。
   * @else
   * @brief Retain unavailable remote connections as retry candidates.
   * @param connector Value of connector.
   * @param reply Value of reply.
   * @endif
   */
  void rememberUnavailable(const std::string& connector, CSP::Readiness reply);
  /*!
   * @if jp
   * @brief `selection Readiness`に対応する処理を行う。
   * @else
   * @brief Perform the selection readiness operation.
   * @endif
   */
  CSP::Readiness selectionReadiness() const {
    auto state = std::atomic_load(&m_selection);
    if (!state) return CSP::NOT_READY;
    std::lock_guard<std::mutex> lock(state->mutex);
    if (!state->active || state->cancelled || state->winner) return CSP::NOT_SELECTING;
    return state->parked ? CSP::NOT_READY : CSP::SCANNING;
  }
  /*!
   * @if jp
   * @brief `selection Available`に対応する処理を行う。
   * @else
   * @brief Perform the selection available operation.
   * @endif
   */
  bool selectionAvailable() const {
    auto state = std::atomic_load(&m_selection);
    if (!state) return true;
    std::lock_guard<std::mutex> l(state->mutex);
    return state->active && !state->cancelled && (!state->winner || state->winner == this);
  }
  /*!
   * @if jp
   * @brief `has Manager`に対応する処理を行う。
   * @else
   * @brief Perform the has manager operation.
   * @endif
   */
  bool hasManager() const { return static_cast<bool>(std::atomic_load(&m_selection)); }
  /*!
   * @if jp
   * @brief `connect Selection`に対応する処理を行う。
   * @param id `id`の値。
   * @else
   * @brief Perform the connect selection operation.
   * @param id Value of id.
   * @endif
   */
  void connectSelection(const std::string& id) { CSPArbitration::instance().connect(id, this, std::atomic_load(&m_selection)); }
  /*!
   * @if jp
   * @brief `disconnect Selection`に対応する処理を行う。
   * @param id `id`の値。
   * @else
   * @brief Perform the disconnect selection operation.
   * @param id Value of id.
   * @endif
   */
  void disconnectSelection(const std::string& id);
private:
  friend class CSPManager;
  friend struct csp_detail::Selection;
  std::mutex m_metadataMutex;
  std::shared_ptr<const ConnectionPriorities> m_connections;
  CSPManager* m_manager{nullptr};
  std::shared_ptr<csp_detail::Selection> m_selection;
  std::atomic<bool> m_sync{false};
};
/*!
 * @if jp
 * @brief CSPポートの選択と再走査を管理する。
 * @else
 * @brief Manage CSP port selection and rescanning.
 * @endif
 */
class CSPManager {
public:
  /*!
   * @if jp
   * @brief `Result`の状態と操作をまとめる。
   * @else
   * @brief Encapsulate the state and operations of Result.
   * @endif
   */
  struct Result {
    bool success{false};
    CSPSelectable* outport{nullptr};
    CSPSelectable* inport{nullptr};
    /*!
     * @if jp
     * @brief `operator bool`に対応する処理を行う。
     * @else
     * @brief Perform the operator bool operation.
     * @endif
     */
    explicit operator bool() const { return success; }
  };
  /*!
   * @if jp
   * @brief `Statistics`の状態と操作をまとめる。
   * @else
   * @brief Encapsulate the state and operations of Statistics.
   * @endif
   */
  struct Statistics { size_t priority, rescan_grants, deferred, timeouts, priority_rescans, yielded, unavailable_retained; };
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  CSPManager() : m_state(std::make_shared<csp_detail::Selection>()) {
    m_state->priority = CSPArbitration::instance().registerManager(m_state);
  }
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~CSPManager() {
    reset();
    std::lock_guard<std::recursive_mutex> l(m_members);
    for (auto* port : m_ports) {
      port->m_manager = nullptr; std::atomic_store(&port->m_selection, std::shared_ptr<csp_detail::Selection>());
      CSPArbitration::instance().update(port, {});
    }
  }
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  CSPManager(const CSPManager&) = delete;
  /*!
   * @if jp
   * @brief `operator=`に対応する処理を行う。
   * @else
   * @brief Perform the operator= operation.
   * @endif
   */
  CSPManager& operator=(const CSPManager&) = delete;
  /*!
   * @if jp
   * @brief プロセスUUIDと登録番号からなる固定Manager IDを取得する。
   * @else
   * @brief Get the fixed manager ID composed of a process UUID and registration number.
   * @endif
   */
  std::string getManagerId() const { return m_state->identity; }
  /*!
   * @if jp
   * @brief 辞書順で比較可能な固定優先度キーを取得する。
   * @else
   * @brief Get the fixed priority key compared in lexicographical order.
   * @endif
   */
  std::string getPriority() const { return m_state->identity; }
  /*!
   * @if jp
   * @brief `In Port`を追加する。
   * @param port 操作対象のポート。
   * @else
   * @brief Add in port.
   * @param port Port to operate on.
   * @endif
   */
  void addInPort(CSPSelectable* port) { if (port && !port->isInput()) throw std::invalid_argument("Not an input port"); add(port); }
  /*!
   * @if jp
   * @brief `Out Port`を追加する。
   * @param port 操作対象のポート。
   * @else
   * @brief Add out port.
   * @param port Port to operate on.
   * @endif
   */
  void addOutPort(CSPSelectable* port) { if (port && port->isInput()) throw std::invalid_argument("Not an output port"); add(port); }
  /*!
   * @if jp
   * @brief `In Port`を取り除く。
   * @param port 操作対象のポート。
   * @else
   * @brief Remove in port.
   * @param port Port to operate on.
   * @endif
   */
  void removeInPort(CSPSelectable* port) { remove(port); }
  /*!
   * @if jp
   * @brief `Out Port`を取り除く。
   * @param port 操作対象のポート。
   * @else
   * @brief Remove out port.
   * @param port Port to operate on.
   * @endif
   */
  void removeOutPort(CSPSelectable* port) { remove(port); }
  /*!
   * @if jp
   * @brief 登録を維持して待機中の選択を中断する。
   * @else
   * @brief Cancel a waiting selection while retaining registrations.
   * @endif
   */
  void cancelSelection() {
    std::lock_guard<std::mutex> l(m_state->mutex);
    m_state->cancelled = true;
    m_state->changed.notify_all();
  }
  /*!
   * @if jp
   * @brief 選択を中断して登録ポートと保留要求を解除する。
   * @else
   * @brief Cancel selection and clear registered ports and pending requests.
   * @endif
   */
  void reset() {
    cancelSelection();
    std::lock_guard<std::recursive_mutex> selection(m_selectMutex);
    std::lock_guard<std::recursive_mutex> l(m_members);
    for (auto* port : m_ports) {
      port->endSelection(false);
      port->m_manager = nullptr;
      std::atomic_store(&port->m_selection, std::shared_ptr<csp_detail::Selection>());
      CSPArbitration::instance().update(port, {});
    }
    m_ports.clear();
    CSPArbitration::instance().release(m_state);
  }
  /*!
   * @if jp
   * @brief 通信要求をManagerへ通知し、選択の受付を判定する。
   * @param outport 出力ポート。
   * @param inport 入力ポート。
   * @else
   * @brief Notify the manager of a request and determine selection admission.
   * @param outport Output port.
   * @param inport Input port.
   * @endif
   */
  bool notify(CSPSelectable* outport = nullptr, CSPSelectable* inport = nullptr) {
    return notifyReadiness(outport, inport) == CSP::READY;
  }
  /*!
   * @if jp
   * @brief `notify Readiness`に対応する処理を行う。
   * @param outport 出力ポート。
   * @param inport 入力ポート。
   * @param connector `connector`の値。
   * @else
   * @brief Perform the notify readiness operation.
   * @param outport Output port.
   * @param inport Input port.
   * @param connector Value of connector.
   * @endif
   */
  CSP::Readiness notifyReadiness(CSPSelectable* outport = nullptr, CSPSelectable* inport = nullptr,
                                const std::string& connector = "") {
    if ((outport == nullptr) == (inport == nullptr)) return CSP::NOT_READY;
    auto* port = outport ? outport : inport;
    auto state = std::atomic_load(&port->m_selection);
    return state == m_state ? m_state->claimReadiness(port, true, connector) : CSP::NOT_READY;
  }
  /*!
   * @if jp
   * @brief `statistics`に対応する処理を行う。
   * @else
   * @brief Perform the statistics operation.
   * @endif
   */
  Statistics statistics() const {
    std::lock_guard<std::mutex> l(m_state->mutex);
    return {m_state->priority, m_state->grants, m_state->deferred, m_state->timeouts,
            m_state->priorityRescans, m_state->yielded, m_state->unavailableRetained};
  }
  /*!
   * @if jp
   * @brief 同期・非同期の登録ポートを取得する。
   * @else
   * @brief Get the registered synchronous and asynchronous ports.
   * @endif
   */
  std::pair<std::vector<CSPSelectable*>, std::vector<CSPSelectable*>> getPorts() const {
    std::lock_guard<std::recursive_mutex> l(m_members);
    std::pair<std::vector<CSPSelectable*>, std::vector<CSPSelectable*>> result;
    for (bool input : {true, false})
      for (auto* port : m_ports) if (port->isInput() == input)
        (port->getSyncMode() ? result.first : result.second).push_back(port);
    return result;
  }
  /*!
   * @if jp
   * @brief 通信可能な登録ポートを一つ選択する。
   * @param timeout 待機期限。秒単位。
   * @else
   * @brief Select one ready registered port.
   * @param timeout Wait timeout in seconds.
   * @endif
   */
  Result select(double timeout) {
    std::lock_guard<std::recursive_mutex> selection(m_selectMutex);
    std::vector<CSPSelectable*> ports;
    { std::lock_guard<std::recursive_mutex> l(m_members); ports = m_ports; }
    return run(ports, timeout, false);
  }
  /*!
   * @if jp
   * @brief 通信可能な入力ポートを選択する。
   * @param timeout 待機期限。秒単位。
   * @else
   * @brief Select a ready input port.
   * @param timeout Wait timeout in seconds.
   * @endif
   */
  Result selectInPort(double timeout = -1) { return filtered(true, timeout); }
  /*!
   * @if jp
   * @brief 通信可能な出力ポートを選択する。
   * @param timeout 待機期限。秒単位。
   * @else
   * @brief Select a ready output port.
   * @param timeout Wait timeout in seconds.
   * @endif
   */
  Result selectOutPort(double timeout = -1) { return filtered(false, timeout); }
  /*!
   * @if jp
   * @brief 指定されたポート候補を検索する。
   * @param ports 検索するポート候補。
   * @else
   * @brief Search the supplied port candidates.
   * @param ports Port candidates to search.
   * @endif
   */
  Result selectPort(const std::vector<CSPSelectable*>& ports) {
    std::lock_guard<std::recursive_mutex> selection(m_selectMutex); return run(ports, -1, false, false);
  }
  /*!
   * @if jp
   * @brief 指定された候補を再検索する。
   * @param ports 検索するポート候補。
   * @else
   * @brief Rescan the supplied candidates.
   * @param ports Port candidates to search.
   * @endif
   */
  Result reselectPort(const std::vector<CSPSelectable*>& ports) {
    std::lock_guard<std::recursive_mutex> selection(m_selectMutex); return run(ports, -1, true, false);
  }
private:
  friend class CSPSelectable;
  /*!
   * @if jp
   * @brief `filtered`に対応する処理を行う。
   * @param input `input`の値。
   * @param timeout 待機期限。秒単位。
   * @else
   * @brief Perform the filtered operation.
   * @param input Value of input.
   * @param timeout Wait timeout in seconds.
   * @endif
   */
  Result filtered(bool input, double timeout) {
    std::lock_guard<std::recursive_mutex> selection(m_selectMutex);
    std::vector<CSPSelectable*> ports;
    { std::lock_guard<std::recursive_mutex> l(m_members);
      for (auto* port : m_ports) if (port->isInput() == input) ports.push_back(port); }
    return run(ports, timeout, false, false);
  }
  /*!
   * @if jp
   * @brief `run`に対応する処理を行う。
   * @param ports 検索するポート候補。
   * @param timeout 待機期限。秒単位。
   * @param retry 再試行の問い合わせかどうか。
   * @param grouped `grouped`の値。
   * @else
   * @brief Perform the run operation.
   * @param ports Port candidates to search.
   * @param timeout Wait timeout in seconds.
   * @param retry Whether this query is a retry.
   * @param grouped Value of grouped.
   * @endif
   */
  Result run(const std::vector<CSPSelectable*>& ports, double timeout, bool retry, bool grouped = true) {
    for (auto* port : ports) if (!port || port->m_manager != this) throw std::invalid_argument("Unregistered CSP candidate");
    {
      std::lock_guard<std::mutex> l(m_state->mutex);
      if (m_state->active) throw std::logic_error("Concurrent CSPManager::select");
      m_state->active = true; m_state->parked = false; m_state->cancelled = false;
      m_state->winner = nullptr; m_state->pending.clear(); m_state->candidates = ports;
      m_state->remotePending.clear(); m_state->yieldScan = false;
    }
    auto finish = [this, &ports]() {
      CSPSelectable* winner;
      {
        std::lock_guard<std::mutex> l(m_state->mutex);
        winner = m_state->cancelled ? nullptr : m_state->winner;
        m_state->active = m_state->parked = false;
        m_state->pending.clear();
        m_state->remotePending.clear();
        m_state->candidates.clear();
      }
      CSPArbitration::instance().release(m_state);
      for (auto* port : ports) port->endSelection(port == winner);
      Result result;
      result.success = winner != nullptr;
      if (winner && winner->isInput()) result.inport = winner; else result.outport = winner;
      return result;
    };
    auto probe = [this](const std::vector<CSPSelectable*>& candidates, bool again) {
      for (auto* port : candidates) {
        { std::lock_guard<std::mutex> l(m_state->mutex); if (m_state->winner || m_state->cancelled || m_state->yieldScan) return; }
        if ((again ? port->reselect() : port->select()) && m_state->claim(port)) return;
      }
    };
    const auto deadline = csp_detail::Clock::now() + std::chrono::duration<double>(std::max(timeout, 0.0));
    try {
      for (auto* port : ports) port->beginSelection();
      std::vector<CSPSelectable*> synchronous, asynchronous;
      if (grouped) {
        for (bool input : {true, false})
          for (auto* port : ports) if (port->isInput() == input)
            (port->getSyncMode() ? synchronous : asynchronous).push_back(port);
        probe(asynchronous, retry); probe(asynchronous, true); probe(synchronous, retry);
      } else probe(ports, retry);
      while (true) {
        {
          std::lock_guard<std::mutex> l(m_state->mutex);
          if (m_state->winner || m_state->cancelled) break;
          if (timeout < 0 || csp_detail::Clock::now() >= deadline) { ++m_state->timeouts; break; }
          m_state->parked = true;
        }
        CSPArbitration::instance().release(m_state);
        auto pending = m_state->remoteRescan();
        const bool remoteRetry = !pending.empty();
        if (pending.empty()) pending = CSPArbitration::instance().acquire(m_state);
        if (!pending.empty()) {
          { std::lock_guard<std::mutex> l(m_state->mutex); m_state->yieldScan = false; }
          probe(pending, !remoteRetry);
          CSPArbitration::instance().release(m_state);
          if (remoteRetry) {
            std::unique_lock<std::mutex> l(m_state->mutex);
            if (!m_state->winner && !m_state->cancelled) {
              m_state->parked = true;
              m_state->changed.wait_for(l, std::chrono::milliseconds(10));
            }
          }
          continue;
        }
        std::unique_lock<std::mutex> l(m_state->mutex);
        if (!m_state->winner && !m_state->cancelled) m_state->changed.wait_for(l, std::chrono::milliseconds(1));
      }
      return finish();
    } catch (...) { { std::lock_guard<std::mutex> l(m_state->mutex); m_state->cancelled = true; } finish(); throw; }
  }
  /*!
   * @if jp
   * @brief ``を追加する。
   * @param port 操作対象のポート。
   * @else
   * @brief Add .
   * @param port Port to operate on.
   * @endif
   */
  void add(CSPSelectable* port) {
    if (!port) return;
    if (port->m_manager != this) port->releaseManager();
    std::lock_guard<std::recursive_mutex> l(m_members);
    if (std::find(m_ports.begin(), m_ports.end(), port) == m_ports.end()) m_ports.push_back(port);
    port->m_manager = this; std::atomic_store(&port->m_selection, m_state);
    CSPArbitration::instance().update(port, m_state);
  }
  /*!
   * @if jp
   * @brief ``を取り除く。
   * @param port 操作対象のポート。
   * @else
   * @brief Remove .
   * @param port Port to operate on.
   * @endif
   */
  void remove(CSPSelectable* port) {
    cancelSelection();
    std::lock_guard<std::recursive_mutex> selection(m_selectMutex);
    std::lock_guard<std::recursive_mutex> l(m_members);
    m_ports.erase(std::remove(m_ports.begin(), m_ports.end(), port), m_ports.end());
    if (port && port->m_manager == this) {
      port->m_manager = nullptr; std::atomic_store(&port->m_selection, std::shared_ptr<csp_detail::Selection>());
      CSPArbitration::instance().update(port, {});
    }
  }
  std::shared_ptr<csp_detail::Selection> m_state;
  std::recursive_mutex m_selectMutex;
  mutable std::recursive_mutex m_members;
  std::vector<CSPSelectable*> m_ports;
};

}
#endif
