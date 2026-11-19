// -*- C++ -*-
/*!
 * @file RTCFailover.hpp
 * @brief Configuration-driven RTC creation, wiring and failover (C++ version of RTCFailover.py)
 *
 * @~japanese
 * 設定ファイル方式の RTC の生成・接続・フェイルオーバー (RTCFailover.py の C++ 版)
 *
 * 設定ファイル (JSON。Python 版と同じ形) で定義したグループごとに、代替候補 (alternatives) の中から使える RTC の組を
 * 選んで接続・アクティブ化し、障害時に次の候補へ切り替える。
 *
 * 障害の検知:
 * - ComponentObserver の RTC_STATUS で ERROR を受信 (RTC 側のプロセスで ComponentObserverConsumer を読み込んでおく
 *   必要がある。Python の RTC は IDL:OpenRTM/ComponentObserver、C++ の RTC は IDL:omg.org/RTC/ComponentObserver)
 * - 定期的な生存確認 (liveness): 現在の RTC に到達できない (プロセス終了など)、または ERROR 状態になっている場合
 * @~english
 * For each group defined in the configuration file (JSON, the same format as the Python version), a usable set of RTCs
 * is chosen from the alternatives, connected and activated, and on failure it switches to the next candidate.
 *
 * Failure detection:
 * - ERROR received in RTC_STATUS of ComponentObserver (ComponentObserverConsumer must be loaded in the processes of the
 *   RTCs. IDL:OpenRTM/ComponentObserver for Python RTCs, IDL:omg.org/RTC/ComponentObserver for C++ RTCs)
 * - Periodic liveness check: the current RTC is unreachable (process exit, etc.) or in the ERROR state
 * @~
 *
 * @date $Date: 2026-11-17 09:00:00 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2026
 *     Nobuhiko Miyamoto
 *
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTCFAILOVER_HPP
#define RTCFAILOVER_HPP

#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <rtm/Manager.h>
#include <rtm/idl/ManagerSkel.h>
#include <rtm/idl/RTCSkel.h>

#include <nlohmann/json.hpp>

#include "ComponentObserverServant.hpp"

namespace rtcfailover
{
  /*!
   * @~japanese
   * @brief ログの段階
   * @~english
   * @brief Log levels
   * @~
   */
  enum class Level { Debug = 10, Info = 20, Warning = 30, Error = 40 };

  /*!
   * @~japanese
   * @brief 出力するログの段階を決める
   *
   * @param name DEBUG / INFO / WARNING / ERROR
   * @~english
   * @brief Sets the log level to output
   *
   * @param name DEBUG / INFO / WARNING / ERROR
   * @~
   */
  void set_log_level(const std::string& name);

  /*!
   * @~japanese
   * @brief ログを 1 行出力する (時刻、段階、RTCFailover: メッセージ。Python 版と同じ形)
   *
   * @param level 段階
   * @param msg メッセージ
   * @~english
   * @brief Outputs one log line (time, level, RTCFailover: message; the same format as the Python version)
   *
   * @param level level
   * @param msg message
   * @~
   */
  void log(Level level, const std::string& msg);

  /*!
   * @~japanese
   * @brief 設定ファイルを読んで確かめる (Python 版の load_config と同じ規則)
   *
   * @param path 設定ファイルのパス (空なら環境変数 RTC_FAILOVER_CONFIG、それもなければ rtc_failover.json)
   * @return 設定
   * @~english
   * @brief Reads and validates the configuration file (the same rules as load_config of the Python version)
   *
   * @param path path of the configuration file (environment variable RTC_FAILOVER_CONFIG if empty, otherwise
   *             rtc_failover.json)
   * @return configuration
   * @~
   */
  nlohmann::json load_config(const std::string& path);

  /*!
   * @~japanese
   * @brief グループごとに RTC を生成・接続し、障害時に代替候補へ切り替える
   * @~english
   * @brief Creates and connects RTCs for each group and switches to alternatives on failure
   * @~
   */
  class FailoverController
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param manager マネージャ (ORB と POA を使う)
     * @param config 設定 (load_config の結果)
     * @~english
     * @brief Constructor
     *
     * @param manager manager (its ORB and POA are used)
     * @param config configuration (result of load_config)
     * @~
     */
    FailoverController(RTC::Manager& manager, nlohmann::json config);
    /*!
     * @~japanese
     * @brief デストラクタ (スレッドを止め、オブザーバーを止める)
     * @~english
     * @brief Destructor (stops the threads and the observers)
     * @~
     */
    ~FailoverController();

    /*!
     * @~japanese
     * @brief すべてのグループを始める (生成、切り替え、オブザーバーの登録、スレッドの起動)
     *
     * @return {グループの ID: 切り替えられたか}
     * @~english
     * @brief Starts all groups (creation, switching, observer registration and threads)
     *
     * @return {group ID: whether switched}
     * @~
     */
    std::map<std::string, bool> start();
    /*!
     * @~japanese
     * @brief スレッドを止める
     * @~english
     * @brief Stops the threads
     * @~
     */
    void stop();
    /*!
     * @~japanese
     * @brief グループを次の使える候補に切り替える (接続・アクティブ化)
     *
     * @param group_id グループの ID
     * @param skip_current 現在の候補を飛ばすか
     * @return 切り替えたら true
     * @~english
     * @brief Switches a group to the next usable candidate (connect and activate)
     *
     * @param group_id group ID
     * @param skip_current whether to skip the current candidate
     * @return true if switched
     * @~
     */
    bool switch_group(const std::string& group_id, bool skip_current);
    /*!
     * @~japanese
     * @brief グループの障害を処理する (次の候補に切り替える)。手動で切り替えるときにも使う
     *
     * @param group_id グループの ID
     * @return 切り替えたら true
     * @~english
     * @brief Handles a failure of a group (switches to the next candidate). Also used for manual switching
     *
     * @param group_id group ID
     * @return true if switched
     * @~
     */
    bool handle_failure(const std::string& group_id);
    /*!
     * @~japanese
     * @brief すべてのグループの障害を処理する (Python 版の reconnect())
     *
     * @return {グループの ID: 切り替えたか}
     * @~english
     * @brief Handles failures of all groups (reconnect() of the Python version)
     *
     * @return {group ID: whether switched}
     * @~
     */
    std::map<std::string, bool> reconnect_all();

  private:
    /*!
     * @~japanese
     * @brief 現在使っている候補 (候補の番号、ロール -> RTC、RTC のキー)
     * @~english
     * @brief The candidate in use (index, role -> RTC, keys of the RTCs)
     * @~
     */
    struct Current
    {
      size_t index{0};
      std::map<std::string, RTC::RTObject_var> components;
      std::set<std::string> keys;
    };

    RTM::Manager_var manager_ref(const std::string& manager_id);
    RTC::RTObject_var find(const std::string& manager_id, const nlohmann::json& spec, bool allow_error);
    std::string key(RTC::RTObject_ptr comp);
    void create_initial();
    RTC::PortService_var external_port(const nlohmann::json& endpoint);
    RTC::PortService_var resolve_external_port(const nlohmann::json& endpoint);
    void connect(const nlohmann::json& conn, RTC::RTObject_ptr comp);
    void connect_internal(const nlohmann::json& conn, const std::map<std::string, RTC::RTObject_var>& components);
    void disconnect(const nlohmann::json& group);
    bool candidate(const nlohmann::json& group, const nlohmann::json& alt,
                   std::map<std::string, RTC::RTObject_var>& components);
    void handle_old(const nlohmann::json& group, Current& old);
    void register_observers(const std::string& group_id = "");
    void add_observer(const std::string& group_id, RTC::RTObject_ptr comp);
    void notify_error(const std::string& group_id, const std::string& comp_key, const std::string& hint);
    void enqueue(const std::string& group_id, const std::string& reason);
    void worker();
    void liveness();

    RTC::Manager& manager_;
    nlohmann::json config_;
    std::map<std::string, nlohmann::json> groups_;            // グループの ID -> 設定 (定義の順は group_order_)
    std::vector<std::string> group_order_;
    std::map<std::string, nlohmann::json> manager_configs_;
    std::map<std::string, Current> current_;
    std::map<std::string, snmpmgr::observer::Handle> observers_;   // RTC のキー -> オブザーバー
    std::map<std::string, RTC::PortService_var> ext_ports_;        // 外部の端 -> ポート (キャッシュ)
    std::recursive_mutex lock_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::deque<std::pair<std::string, std::string>> queue_;      // (グループの ID, 理由)
    std::set<std::string> pending_;
    std::atomic<bool> stop_{false};
    std::vector<std::thread> threads_;
    bool observer_enabled_{true};
    bool observer_heartbeat_{false};
    double liveness_interval_{2.0};
  };
} // namespace rtcfailover

#endif
