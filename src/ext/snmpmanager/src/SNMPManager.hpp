// -*- C++ -*-
/*!
 * @file SNMPManager.hpp
 * @brief SNMPManager (C++ version)
 *
 * @~japanese
 * SNMPManager (C++ 版)
 *
 * Python 版 SNMPManager.py と同じ設定 (rtc.conf の snmp.*) と構成ファイル (migration.json など) で動く。
 * @~english
 * Works with the same settings (snmp.* in rtc.conf) and configuration files (migration.json, etc.) as the Python version SNMPManager.py.
 * @~
 *
 * @date $Date: 2026-11-13 16:24:30 $
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

#ifndef SNMPMGR_SNMPMANAGER_HPP
#define SNMPMGR_SNMPMANAGER_HPP

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <rtm/Manager.h>

#include "ComponentObserverServant.hpp"
#include "RTCPlacement.hpp"
#include "PluginManager.hpp"
#include "RTCOperations.hpp"
#include "SNMPClient.hpp"
#include "MonitoringStore.hpp"
#include "SNMPManagerUtil.hpp"

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief リトライしても解決しない起動時の失敗
   * @~english
   * @brief Startup failure that retrying does not resolve
   * @~
   */
  class StartupFailure : public std::runtime_error
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param m メッセージ
     * @param r 理由の一覧
     * @~english
     * @brief Constructor
     *
     * @param m message
     * @param r list of reasons
     * @~
     */
    StartupFailure(const std::string& m, std::vector<std::string> r = {}) : std::runtime_error(m), reasons(std::move(r)) {}
    std::vector<std::string> reasons;
  };

  /*!
   * @~japanese
   * @brief 監視するロボットの状態 (メトリクス、cgroup、死活、RTC の一覧)
   * @~english
   * @brief State of a monitored robot (metrics, cgroup, liveness, RTC list)
   * @~
   */
  struct Robot
  {
    std::string name, ip;
    bool is_local{false};
    std::string os{"unknown"};
    // メトリクス
    std::optional<double> cpu_usage, memory_usage, network_in, network_out, wifi_dbm, wifi_strength, mem_total_mb;
    std::map<std::string, long long> last_cpu;
    long long last_if_recv{0}, last_if_send{0};
    std::optional<double> last_if_time, last_data_update;
    int ifindex{0};
    // cgroup
    bool cg_valid{false};
    double cg_mem_limit_mb{0}, cg_mem_usage_mb{0}, cg_cpu_limit_pct{0};
    std::optional<double> cg_cpu_usage_pct;
    std::deque<double> cg_cpu_samples;
    long long cg_last_usage{-1}, cg_last_ts{-1};
    long long cg_ignore_before_ns{0};   // この時刻 (コンテナの時計, ns) より前の CPU 値は使わない
    // 死活監視
    std::string status{"unknown"};
    std::optional<double> last_seen, last_heartbeat;
    double registered_at{0};
    std::deque<std::pair<bool, double>> probes;
    std::optional<double> latency_ms, jitter_ms, packet_loss;
    // バッテリー残量 (snmpd の extend。snmp.battery.enabled)
    std::optional<double> battery_pct;
    std::map<std::string, std::optional<double>> metrics;   // 追加の監視の値 (snmp.metrics): 名前 -> 値 (取れなければ nullopt)
    // 動いている RTC の一覧 (inventory_loop が更新する) と、RTC がないときのメモリ使用量 (予約の土台)
    std::optional<std::vector<ComponentInfo>> inventory;
    double inventory_time{0};
    std::optional<double> base_mem_mb;
  };

  /*!
   * @~japanese
   * @brief 計画の適用の結果 (計画の RTC 名 -> 置いたロボット / 実際に使ったインスタンス名)
   * @~english
   * @brief Result of applying a plan (RTC name in the plan -> robot placed on / instance name actually used)
   * @~
   */
  struct ApplyResult
  {
    std::map<std::string, std::string> assign, instances;
  };

  /*!
   * @~japanese
   * @brief 構成ファイルのロボットのマスターマネージャ (止まっているロボットは down)
   * @~english
   * @brief Master managers of the robots in the configuration file (stopped robots are in down)
   * @~
   */
  struct ManagerSet
  {
    std::map<std::string, RTM::Manager_var> refs;
    std::vector<std::string> down;
    /*!
     * @~japanese
     * @brief ロボットのマネージャを返す (なければ std::runtime_error)
     *
     * @param name ロボット名
     * @return マネージャの参照
     * @~english
     * @brief Returns the manager of a robot (std::runtime_error if not available)
     *
     * @param name robot name
     * @return manager reference
     * @~
     */
    RTM::Manager_ptr at(const std::string& name) const
    {
      auto it = refs.find(name);
      if (it == refs.end()) { throw std::runtime_error("robot " + name + " is not available (down or unknown)"); }
      return it->second.in();
    }
    /*!
     * @~japanese
     * @brief ロボットのマネージャがあるか
     *
     * @param name ロボット名
     * @return あれば true
     * @~english
     * @brief Whether the manager of a robot is available
     *
     * @param name robot name
     * @return true if available
     * @~
     */
    bool has(const std::string& name) const { return refs.count(name) > 0; }
  };

  /*!
   * @~japanese
   * @brief SNMP による監視・マイグレーション・自動配置・自動復旧を行うマスターマネージャの拡張
   * @~english
   * @brief Master manager extension for monitoring, migration, automatic placement and recovery via SNMP
   * @~
   */
  class SNMPManager : public PluginHost
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ。rtc.conf の設定を読む
     *
     * @param mgr OpenRTM のマネージャ
     * @~english
     * @brief Constructor. Reads the settings of rtc.conf
     *
     * @param mgr OpenRTM manager
     * @~
     */
    explicit SNMPManager(RTC::Manager& mgr);
    /*!
     * @~japanese
     * @brief デストラクタ。スレッドを止める
     * @~english
     * @brief Destructor. Stops the threads
     * @~
     */
    ~SNMPManager() override;
    /*!
     * @~japanese
     * @brief 初期化する (トラップの受信、探索、監視・起動時の構成・自動復旧などのスレッドを始める)
     *
     * @return 成功すれば true
     * @~english
     * @brief Initializes (starts trap reception, discovery, and the threads for monitoring, startup configuration, recovery, etc.)
     *
     * @return true on success
     * @~
     */
    bool init();
    /*!
     * @~japanese
     * @brief スレッドを止める
     * @~english
     * @brief Stops the threads
     * @~
     */
    void stop();

    // ---- PluginHost
    /*!
     * @~japanese
     * @brief 自分のロボットの名前を返す (PluginHost)
     *
     * @return 名前
     * @~english
     * @brief Returns the name of this robot (PluginHost)
     *
     * @return name
     * @~
     */
    std::string host_name() const override { return name_; }
    /*!
     * @~japanese
     * @brief ログを出す (PluginHost)
     *
     * @param m メッセージ
     * @~english
     * @brief Outputs a log message (PluginHost)
     *
     * @param m message
     * @~
     */
    void host_log(const std::string& m) override;
    /*!
     * @~japanese
     * @brief 全ロボットの状態を返す (PluginHost)
     *
     * @return ロボットの状態
     * @~english
     * @brief Returns the states of all robots (PluginHost)
     *
     * @return robot states
     * @~
     */
    std::vector<RobotInfo> host_robots() override;
    /*!
     * @~japanese
     * @brief ロボットのマスターマネージャの参照を返す (PluginHost)
     *
     * @param robot ロボット名
     * @return CORBA::Object* (なければ nullptr)
     * @~english
     * @brief Returns the reference of the master manager of a robot (PluginHost)
     *
     * @param robot robot name
     * @return CORBA::Object* (nullptr if not available)
     * @~
     */
    void* host_manager_object(const std::string& robot) override;
    /*!
     * @~japanese
     * @brief RTC がそのロボットにあるか (PluginHost)
     *
     * @param instance インスタンス名
     * @param robot ロボット名
     * @return あれば true
     * @~english
     * @brief Whether an RTC exists on the robot (PluginHost)
     *
     * @param instance instance name
     * @param robot robot name
     * @return true if it exists
     * @~
     */
    bool host_has_component(const std::string& instance, const std::string& robot) override;
    /*!
     * @~japanese
     * @brief 計画を適用する (PluginHost)
     *
     * @param plan 計画
     * @param label ログに出す名前
     * @~english
     * @brief Applies a plan (PluginHost)
     *
     * @param plan plan
     * @param label name shown in logs
     * @~
     */
    void host_apply_plan(const json& plan, const std::string& label) override;
    /*!
     * @~japanese
     * @brief rtc.conf の値を返す (PluginHost)
     *
     * @param key キー
     * @param def 既定値
     * @return 値
     * @~english
     * @brief Returns a value of rtc.conf (PluginHost)
     *
     * @param key key
     * @param def default value
     * @return value
     * @~
     */
    std::string host_conf(const std::string& key, const std::string& def) override { return prop(key, def); }
    /*!
     * @~japanese
     * @brief 構成ファイルの内容を返す (PluginHost)
     *
     * @return 構成ファイルの内容
     * @~english
     * @brief Returns the contents of the configuration file (PluginHost)
     *
     * @return contents of the configuration file
     * @~
     */
    json host_migration_config() override { return load_migration_config(); }
    /*!
     * @~japanese
     * @brief イベントを記録する (PluginHost)
     *
     * @param robot ロボット名
     * @param kind イベントの種別
     * @param detail 詳細
     * @~english
     * @brief Records an event (PluginHost)
     *
     * @param robot robot name
     * @param kind event kind
     * @param detail details
     * @~
     */
    void host_event(const std::string& robot, const std::string& kind, const std::string& detail) override
    {
      event(robot, kind, detail);
    }

  private:
    // ---- 設定
    /*!
     * @~japanese
     * @brief rtc.conf の値を返す
     *
     * @param key キー
     * @param def 既定値
     * @return 値
     * @~english
     * @brief Returns a value of rtc.conf
     *
     * @param key key
     * @param def default value
     * @return value
     * @~
     */
    std::string prop(const std::string& key, const std::string& def) const;
    /*!
     * @~japanese
     * @brief rtc.conf の値を数値で返す
     *
     * @param key キー
     * @param def 既定値
     * @return 値
     * @~english
     * @brief Returns a value of rtc.conf as a number
     *
     * @param key key
     * @param def default value
     * @return value
     * @~
     */
    double prop_d(const std::string& key, double def) const;
    /*!
     * @~japanese
     * @brief rtc.conf の値を真偽値で返す (YES / TRUE / ON / 1)
     *
     * @param key キー
     * @param def 既定値
     * @return 値
     * @~english
     * @brief Returns a value of rtc.conf as a boolean (YES / TRUE / ON / 1)
     *
     * @param key key
     * @param def default value
     * @return value
     * @~
     */
    bool prop_b(const std::string& key, bool def) const;
    /*!
     * @~japanese
     * @brief 探索する IPv4 アドレスの一覧を作る
     *
     * @return IP アドレスのリスト
     * @~english
     * @brief Builds the IPv4 discovery targets
     *
     * @return list of IP addresses
     * @~
     */
    std::vector<std::string> scan_targets() const;

    // ---- ロボット
    /*!
     * @~japanese
     * @brief 名前でロボットを探す
     *
     * @param name ロボット名
     * @return ロボット (なければ nullptr)
     * @~english
     * @brief Finds a robot by name
     *
     * @param name robot name
     * @return robot (nullptr if not found)
     * @~
     */
    std::shared_ptr<Robot> find_robot(const std::string& name);
    /*!
     * @~japanese
     * @brief IP アドレスでロボットを探す
     *
     * @param ip IP アドレス
     * @return ロボット (なければ nullptr)
     * @~english
     * @brief Finds a robot by IP address
     *
     * @param ip IP address
     * @return robot (nullptr if not found)
     * @~
     */
    std::shared_ptr<Robot> find_robot_by_ip(const std::string& ip);
    /*!
     * @~japanese
     * @brief ロボットを登録する
     *
     * @param ip IP アドレス
     * @param name ロボット名
     * @return 新しく登録したら true
     * @~english
     * @brief Registers a robot
     *
     * @param ip IP address
     * @param name robot name
     * @return true if newly registered
     * @~
     */
    bool register_robot(const std::string& ip, const std::string& name);
    /*!
     * @~japanese
     * @brief sysName.0 でロボットを探す
     *
     * @param ip IP アドレス
     * @return 見つかれば true
     * @~english
     * @brief Discovers a robot by sysName.0
     *
     * @param ip IP address
     * @return true if found
     * @~
     */
    bool check_device(const std::string& ip);
    /*!
     * @~japanese
     * @brief まだ登録していない探索対象に問い合わせる (後から起動したロボットを参加させる)
     * @~english
     * @brief Probes scan targets not registered yet (lets robots that boot later join)
     * @~
     */
    void rediscover();
    /*!
     * @~japanese
     * @brief 登録済みのロボットの一覧の文字列を返す (ログ用)
     *
     * @return 文字列
     * @~english
     * @brief Returns a string listing the registered robots (for logs)
     *
     * @return string
     * @~
     */
    std::string robot_summary();
    /*!
     * @~japanese
     * @brief ロボットが生きているか
     *
     * @param name ロボット名
     * @return 生きていれば true
     * @~english
     * @brief Whether a robot is alive
     *
     * @param name robot name
     * @return true if alive
     * @~
     */
    bool is_alive(const std::string& name);

    // ---- 監視
    /*!
     * @~japanese
     * @brief 全ロボットの監視の値を定期的に取るスレッドの本体
     * @~english
     * @brief Body of the thread that periodically polls the monitoring values of all robots
     * @~
     */
    void polling_loop();
    /*!
     * @~japanese
     * @brief 自分のロボットの監視の値を取るスレッドの本体
     * @~english
     * @brief Body of the thread that polls the monitoring values of this robot
     * @~
     */
    void self_loop();
    /*!
     * @~japanese
     * @brief アクティブ監視: 全ロボットへ定期的にプローブを送るスレッドの本体
     * @~english
     * @brief Active monitoring: body of the thread that periodically sends probes to all robots
     * @~
     */
    void liveness_loop();
    /*!
     * @~japanese
     * @brief パッシブ監視の送信側: 他のロボットへ Heartbeat のトラップを送り続けるスレッドの本体
     * @~english
     * @brief Sender side of passive monitoring: body of the thread that keeps sending Heartbeat traps to the other robots
     * @~
     */
    void heartbeat_loop();
    /*!
     * @~japanese
     * @brief 監視の値を定期的に SQLite に保存するスレッドの本体
     * @~english
     * @brief Body of the thread that periodically saves the monitoring values to SQLite
     * @~
     */
    void store_loop();
    /*!
     * @~japanese
     * @brief ロボットの監視の値を SNMP で取って更新する
     *
     * @param r ロボット
     * @~english
     * @brief Updates the monitoring values of a robot via SNMP
     *
     * @param r robot
     * @~
     */
    void set_robot_data(const std::shared_ptr<Robot>& r);
    /*!
     * @~japanese
     * @brief コンテナ (cgroup) の制限と使用量を取得する
     *
     * @param r ロボット
     * @~english
     * @brief Gets the limits and usage of the container (cgroup)
     *
     * @param r robot
     * @~
     */
    void update_cgroup(const std::shared_ptr<Robot>& r);
    /*!
     * @~japanese
     * @brief プローブの結果 (往復時間・ジッタ・損失) を記録する
     *
     * @param r ロボット
     * @param ok 成功したか
     * @param rtt 往復時間 (ms)
     * @param now 現在の時刻
     * @~english
     * @brief Records a probe result (round-trip time, jitter, loss)
     *
     * @param r robot
     * @param ok whether it succeeded
     * @param rtt round-trip time (ms)
     * @param now current time
     * @~
     */
    void record_probe(const std::shared_ptr<Robot>& r, bool ok, double rtt, double now);
    /*!
     * @~japanese
     * @brief ロボットの状態を更新する (最後の応答から liveness.timeout 以内なら alive)
     *
     * @param r ロボット
     * @param now 現在の時刻
     * @~english
     * @brief Updates the status of a robot (alive if within liveness.timeout from the last response)
     *
     * @param r robot
     * @param now current time
     * @~
     */
    void update_status(const std::shared_ptr<Robot>& r, double now);
    /*!
     * @~japanese
     * @brief SNMP のトラップを受けたときの処理
     *
     * @param ip 送り元の IP アドレス
     * @param values {OID: 値}
     * @~english
     * @brief Handles a received SNMP trap
     *
     * @param ip source IP address
     * @param values {OID: value}
     * @~
     */
    void on_trap(const std::string& ip, const SnmpClient::Values& values);
    /*!
     * @~japanese
     * @brief 電波強度を取る OID を返す
     *
     * @return OID
     * @~english
     * @brief Returns the OID for the RSSI
     *
     * @return OID
     * @~
     */
    std::string wifi_oid() const;
    /*!
     * @~japanese
     * @brief ロボットの監視の値を名前で返す (条件の metric)
     *
     * @param r ロボット
     * @param name 監視の値の名前
     * @return 値 (分からなければ nullopt)
     * @~english
     * @brief Returns a monitoring value of a robot by name (metric of a condition)
     *
     * @param r robot
     * @param name name of the monitoring value
     * @return value (nullopt if unknown)
     * @~
     */
    std::optional<double> metric(const Robot& r, const std::string& name) const;
    /*!
     * @~japanese
     * @brief ロボットの監視の値を SQLite の行にする
     *
     * @param r ロボット
     * @return MetricRow
     * @~english
     * @brief Converts the monitoring values of a robot into an SQLite row
     *
     * @param r robot
     * @return MetricRow
     * @~
     */
    MetricRow snapshot(const Robot& r) const;
    /*!
     * @~japanese
     * @brief ロボットの状態をプラグインに渡す形にする
     *
     * @param r ロボット
     * @return RobotInfo
     * @~english
     * @brief Converts the state of a robot into the form passed to plugins
     *
     * @param r robot
     * @return RobotInfo
     * @~
     */
    RobotInfo info(const Robot& r) const;

    // ---- イベント・ログ
    /*!
     * @~japanese
     * @brief ログを出す
     *
     * @param m メッセージ
     * @~english
     * @brief Outputs a log message
     *
     * @param m message
     * @~
     */
    void log(const std::string& m) { Log::info(m); }
    /*!
     * @~japanese
     * @brief イベントを記録する (ログ、SQLite、プラグインへの通知)
     *
     * @param robot ロボット名
     * @param kind イベントの種別
     * @param detail 詳細
     * @~english
     * @brief Records an event (log, SQLite and notification to plugins)
     *
     * @param robot robot name
     * @param kind event kind
     * @param detail details
     * @~
     */
    void event(const std::string& robot, const std::string& kind, const std::string& detail = "");
    /*!
     * @~japanese
     * @brief 起動時の構成の状態をファイルに書く
     *
     * @param status 状態
     * @~english
     * @brief Writes the status of the startup configuration to a file
     *
     * @param status status
     * @~
     */
    void write_status(const json& status);
    /*!
     * @~japanese
     * @brief ログのファイルに 1 行書く
     *
     * @param line 行
     * @~english
     * @brief Writes one line to the log file
     *
     * @param line line
     * @~
     */
    void logfile(const std::string& line);
    /*!
     * @~japanese
     * @brief トラップを送る
     *
     * @param ip 送り先の IP アドレス
     * @param message メッセージ (TrapName)
     * @param info 追加の情報 (TrapInfo。空なら自分の名前)
     * @~english
     * @brief Sends a trap
     *
     * @param ip destination IP address
     * @param message message (TrapName)
     * @param info additional information (TrapInfo; this robot's name if empty)
     * @~
     */
    void trap(const std::string& ip, const std::string& message, const std::string& info = "");
    /*!
     * @~japanese
     * @brief ほかのロボットの IP アドレスを返す
     *
     * @param alive_only 生きているロボットだけか
     * @return IP アドレスのリスト
     * @~english
     * @brief Returns the IP addresses of the other robots
     *
     * @param alive_only whether only alive robots
     * @return list of IP addresses
     * @~
     */
    std::vector<std::string> peer_ips(bool alive_only = true);

    // ---- 構成ファイル
    /*!
     * @~japanese
     * @brief 構成ファイルを読んで確かめる
     *
     * @return 構成ファイルの内容
     * @~english
     * @brief Reads and validates the configuration file
     *
     * @return contents of the configuration file
     * @~
     */
    json load_migration_config();
    /*!
     * @~japanese
     * @brief 計画 (components、connections、remove) を確かめる
     *
     * @param plan 計画
     * @param managers ロボット名のリスト
     * @param label エラーのメッセージに出す名前
     * @~english
     * @brief Validates a plan (components, connections, remove)
     *
     * @param plan plan
     * @param managers list of robot names
     * @param label name shown in error messages
     * @~
     */
    void validate_plan(const json& plan, const json& managers, const std::string& label);
    /*!
     * @~japanese
     * @brief マイグレーションの条件のグループを確かめる
     *
     * @param group 条件のグループ
     * @~english
     * @brief Validates a group of migration conditions
     *
     * @param group condition group
     * @~
     */
    void validate_conditions(const json& group);
    /*!
     * @~japanese
     * @brief 拡張の設定 (待機モジュール、メモリの予約、配置のロジック、実行ロボットの選出) を確かめる
     *
     * @param config 構成ファイルの内容
     * @param managers ロボット名のリスト
     * @~english
     * @brief Validates the extension settings (standby, reservation, placement logic, executor election)
     *
     * @param config contents of the configuration file
     * @param managers list of robot names
     * @~
     */
    void validate_extensions(const json& config, const json& managers);
    /*!
     * @~japanese
     * @brief プラグインマネージャを返す
     *
     * @return PluginManager (使えなければ nullptr)
     * @~english
     * @brief Returns the plugin manager
     *
     * @return PluginManager (nullptr if unavailable)
     * @~
     */
    PluginManager* plugins();

    // ---- 実行ロボット (executor。"auto" なら選出する)
    /*!
     * @~japanese
     * @brief このマネージャが計画を適用するかを返す (executor を書いた最初のセクションで決める。"auto" なら選出する)
     *
     * @param config 構成ファイルの内容
     * @param sections executor を探すセクション
     * @param counts ロボットごとの RTC の数 (選出に使う)
     * @param purpose ログに出す目的
     * @return 適用するなら true
     * @~english
     * @brief Returns whether this manager applies the plan (decided by the first section with executor; "auto" elects one)
     *
     * @param config contents of the configuration file
     * @param sections sections to look for executor
     * @param counts number of RTCs on each robot (used for the election)
     * @param purpose purpose shown in logs
     * @return true if this manager applies the plan
     * @~
     */
    bool is_executor(const json& config, std::initializer_list<const json*> sections,
                     const std::optional<std::map<std::string, int>>& counts = std::nullopt,
                     const std::string& purpose = "plan");
    /*!
     * @~japanese
     * @brief 計画を実行するロボットを選ぶ (生きていて通信状態のよい候補のうち、RTC の数が最も多いもの)
     *
     * @param config 構成ファイルの内容
     * @param section executor を書いたセクション
     * @param counts ロボットごとの RTC の数
     * @param purpose ログに出す目的
     * @return ロボット名 (状態の分からない候補があれば nullopt = 保留)
     * @~english
     * @brief Elects the robot that executes the plan (the alive candidate with a good link and the most RTCs)
     *
     * @param config contents of the configuration file
     * @param section section with executor
     * @param counts number of RTCs on each robot
     * @param purpose purpose shown in logs
     * @return robot name (nullopt = pending if some candidates are unknown)
     * @~
     */
    std::optional<std::string> elect_executor(const json& config, const json& section,
                                              const std::optional<std::map<std::string, int>>& counts,
                                              const std::string& purpose);
    /*!
     * @~japanese
     * @brief manual の計画で各ロボットに割り当てた RTC の数を返す
     *
     * @param plan 計画
     * @return {ロボット名: 数} (auto なら nullopt)
     * @~english
     * @brief Returns the number of RTCs assigned to each robot in a manual plan
     *
     * @param plan plan
     * @return {robot name: count} (nullopt for auto)
     * @~
     */
    static std::optional<std::map<std::string, int>> planned_counts(const json& plan);
    /*!
     * @~japanese
     * @brief 計画の項目のインスタンス名を返す
     *
     * @param entry 計画の項目
     * @return インスタンス名
     * @~english
     * @brief Returns the instance name of an entry in a plan
     *
     * @param entry entry of a plan
     * @return instance name
     * @~
     */
    static std::string instance_name(const json& entry);

    // ---- 起動時の構成 (復帰したロボットの参加を含む)
    /*!
     * @~japanese
     * @brief 計画の RTC が (どこかのロボットで) すべて動いているか
     *
     * @param plan 計画
     * @return すべて動いていれば true
     * @~english
     * @brief Whether all RTCs of the plan are running (on some robot)
     *
     * @param plan plan
     * @return true if all are running
     * @~
     */
    bool startup_observed(const json& plan);
    /*!
     * @~japanese
     * @brief ほかのロボットに起動時の構成の完了を知らせる
     * @~english
     * @brief Notifies the other robots that the startup configuration is completed
     * @~
     */
    void notify_startup_completed();
    /*!
     * @~japanese
     * @brief ほかのロボットで起動時の構成が終わっているか ('Startup Query' への返事で判断する)
     *
     * @return 終わっていれば true
     * @~english
     * @brief Whether the startup configuration has been completed on another robot (decided by replies to 'Startup Query')
     *
     * @return true if completed
     * @~
     */
    bool already_started_elsewhere();
    /*!
     * @~japanese
     * @brief 起動時の構成の完了の通知を受けたときの処理
     *
     * @param ip 送り元の IP アドレス
     * @param sender 送り元のロボット名
     * @~english
     * @brief Handles a notice that the startup configuration is completed
     *
     * @param ip source IP address
     * @param sender sending robot name
     * @~
     */
    void on_startup_notice(const std::string& ip, const std::string& sender);
    /*!
     * @~japanese
     * @brief 起動時の構成の問い合わせに答える
     *
     * @param ip 問い合わせ元の IP アドレス
     * @~english
     * @brief Answers a query about the startup configuration
     *
     * @param ip IP address of the inquirer
     * @~
     */
    void on_startup_query(const std::string& ip);
    /*!
     * @~japanese
     * @brief 'Startup Pending' の返事を受けたときの処理
     *
     * @param ip 送り元の IP アドレス
     * @~english
     * @brief Handles a 'Startup Pending' reply
     *
     * @param ip source IP address
     * @~
     */
    void on_startup_pending(const std::string& ip);
    /*!
     * @~japanese
     * @brief 待機モジュールを、アクティブにせずに起動しておく
     *
     * @param config 構成ファイルの内容
     * @~english
     * @brief Starts the standby modules without activating them
     *
     * @param config contents of the configuration file
     * @~
     */
    void launch_standby(const json& config);

    // ---- RTC の一覧・待機モジュール・メモリの予約
    /*!
     * @~japanese
     * @brief 全ロボットの RTC の一覧を更新する
     * @~english
     * @brief Refreshes the RTC lists of all robots
     * @~
     */
    void refresh_inventory();
    /*!
     * @~japanese
     * @brief RTC の一覧を定期的に更新するスレッドの本体
     * @~english
     * @brief Body of the thread that refreshes the RTC lists periodically
     * @~
     */
    void inventory_loop();
    /*!
     * @~japanese
     * @brief ロボットの RTC の一覧を返す
     *
     * @param name ロボット名
     * @return RTC の一覧 (分からなければ nullopt)
     * @~english
     * @brief Returns the RTC list of a robot
     *
     * @param name robot name
     * @return RTC list (nullopt if unknown)
     * @~
     */
    std::optional<std::vector<ComponentInfo>> robot_inventory(const std::string& name);
    /*!
     * @~japanese
     * @brief 待機モジュールの設定を返す
     *
     * @param config 構成ファイルの内容
     * @return 設定 (無効なら nullptr)
     * @~english
     * @brief Returns the standby settings
     *
     * @param config contents of the configuration file
     * @return settings (nullptr if disabled)
     * @~
     */
    static const json* standby_cfg(const json& config);
    /*!
     * @~japanese
     * @brief メモリの予約の設定を返す
     *
     * @param config 構成ファイルの内容
     * @return 設定 (無効なら nullptr)
     * @~english
     * @brief Returns the memory reservation settings
     *
     * @param config contents of the configuration file
     * @return settings (nullptr if disabled)
     * @~
     */
    static const json* reservation_cfg(const json& config);
    /*!
     * @~japanese
     * @brief 配置のプロファイルを返す (計測が更新されていれば読み直す)
     *
     * @return ProfileDB
     * @~english
     * @brief Returns the placement profiles (reloaded if measurements were updated)
     *
     * @return ProfileDB
     * @~
     */
    std::shared_ptr<ProfileDB> profiles();
    /*!
     * @~japanese
     * @brief slave マネージャ 1 つ分のメモリを返す
     *
     * @param language 言語
     * @param prof プロファイル
     * @return メモリ (MB)
     * @~english
     * @brief Returns the memory of one slave manager
     *
     * @param language language
     * @param prof profile
     * @return memory (MB)
     * @~
     */
    double slave_overhead(const std::string& language, const RtcProfile* prof);
    /*!
     * @~japanese
     * @brief RTC 1 つの予約量を返す (reserve_mb > reservation.memory_mb > placement.xml の見込み > default_mb)
     *
     * @param rcfg メモリの予約の設定
     * @param impl 実装 ID
     * @param type 型名
     * @param language 言語
     * @param entry 計画の項目
     * @return 予約量 (MB)
     * @~english
     * @brief Returns the reservation of one RTC (reserve_mb > reservation.memory_mb > estimate in placement.xml > default_mb)
     *
     * @param rcfg memory reservation settings
     * @param impl implementation ID
     * @param type type name
     * @param language language
     * @param entry entry of the plan
     * @return reservation (MB)
     * @~
     */
    double reserve_mb(const json& rcfg, const std::string& impl, const std::string& type, const std::string& language,
                      const json* entry = nullptr);
    /*!
     * @~japanese
     * @brief ロボットで動いている RTC の予約の合計を返す (slave マネージャの分を含む)
     *
     * @param rcfg メモリの予約の設定
     * @param robot ロボット
     * @return 予約の合計 (MB)
     * @~english
     * @brief Returns the total reservation of the RTCs running on a robot (including slave managers)
     *
     * @param rcfg memory reservation settings
     * @param robot robot
     * @return total reservation (MB)
     * @~
     */
    double reserved_on(const json& rcfg, const Robot& robot);
    /*!
     * @~japanese
     * @brief get_loadable_modules() のモジュールの情報を返す (キャッシュする)
     *
     * @param robot ロボット名
     * @param mgr マネージャの参照
     * @param module モジュール名
     * @return モジュールのプロファイル
     * @~english
     * @brief Returns the module information from get_loadable_modules() (cached)
     *
     * @param robot robot name
     * @param mgr manager reference
     * @param module module name
     * @return module profile
     * @~
     */
    std::map<std::string, std::string> module_info(const std::string& robot, RTM::Manager_ptr mgr, const std::string& module);
    /*!
     * @~japanese
     * @brief robots の順に、型が module の非アクティブな RTC (待機モジュール) を探す
     *
     * @param ms マネージャの集まり
     * @param module モジュール名
     * @param robots ロボット名のリスト
     * @param used 使用済みの RTC
     * @param exclude 除くインスタンス名
     * @return (ロボット名, RTC) (なければ nullopt)
     * @~english
     * @brief Searches robots in order for an inactive RTC of type module (standby module)
     *
     * @param ms manager set
     * @param module module name
     * @param robots list of robot names
     * @param used RTCs already used
     * @param exclude instance names to exclude
     * @return (robot name, RTC) (nullopt if not found)
     * @~
     */
    std::optional<std::pair<std::string, rtcops::Component>> find_standby(const ManagerSet& ms, const std::string& module,
                                                                          const std::vector<std::string>& robots,
                                                                          const std::vector<RTC::RTObject_var>& used,
                                                                          const std::set<std::string>& exclude);
    /*!
     * @~japanese
     * @brief 生成する前に、予約を含めてメモリが足りるかを確かめる (enforce なら足りないと失敗)
     *
     * @param rcfg メモリの予約の設定
     * @param robot ロボット名
     * @param mgr マネージャの参照
     * @param entry 計画の項目
     * @param pending このロボットに置く途中の予約
     * @return 必要な量 (MB)
     * @~english
     * @brief Checks before creation whether memory including reservations is sufficient (fails if insufficient with enforce)
     *
     * @param rcfg memory reservation settings
     * @param robot robot name
     * @param mgr manager reference
     * @param entry entry of the plan
     * @param pending reservations being placed on this robot
     * @return required amount (MB)
     * @~
     */
    double check_reservation(const json& rcfg, const std::string& robot, RTM::Manager_ptr mgr, const json& entry,
                             std::map<std::string, double>& pending);

    // ---- 構成の適用
    /*!
     * @~japanese
     * @brief 構成ファイルのロボットのマネージャの参照を得る
     *
     * @param config 構成ファイルの内容
     * @param allow_down 止まっているロボットを許すか (マイグレーション)
     * @return ManagerSet
     * @~english
     * @brief Obtains the manager references of the robots in the configuration file
     *
     * @param config contents of the configuration file
     * @param allow_down whether stopped robots are allowed (migration)
     * @return ManagerSet
     * @~
     */
    ManagerSet manager_refs(const json& config, bool allow_down = false);
    /*!
     * @~japanese
     * @brief ロボットのマスターマネージャの参照を得る
     *
     * @param robot ロボット名
     * @return マネージャの参照
     * @~english
     * @brief Obtains the reference of the master manager of a robot
     *
     * @param robot robot name
     * @return manager reference
     * @~
     */
    RTM::Manager_var resolve(const std::string& robot);
    /*!
     * @~japanese
     * @brief create 文字列に言語を補う
     *
     * @param create create 文字列
     * @param language 言語
     * @return create 文字列
     * @~english
     * @brief Adds the language to a create string
     *
     * @param create create string
     * @param language language
     * @return create string
     * @~
     */
    std::string qualify_create(const std::string& create, const std::string& language) const;
    /*!
     * @~japanese
     * @brief 計画を適用する (生成・接続・アクティブ化・削除)
     *
     * @param config 構成ファイルの内容
     * @param plan 計画
     * @param label ログに出す名前
     * @return ApplyResult
     * @~english
     * @brief Applies a plan (create, connect, activate, remove)
     *
     * @param config contents of the configuration file
     * @param plan plan
     * @param label name shown in logs
     * @return ApplyResult
     * @~
     */
    ApplyResult apply_plan(const json& config, const json& plan, const std::string& label);
    /*!
     * @~japanese
     * @brief 接続するポートの参照と名前を返す (ポートの参照のキャッシュを使う)
     *
     * @param comp RTC
     * @param port ポートの名前
     * @param module モジュール名
     * @param planned_instance 計画のインスタンス名
     * @param calls CORBA の呼び出しの数 (数える)
     * @return (ポートの参照, ポートの完全な名前)
     * @~english
     * @brief Returns the reference and name of a port to connect (uses the port reference cache)
     *
     * @param comp RTC
     * @param port port name
     * @param module module name
     * @param planned_instance instance name in the plan
     * @param calls number of CORBA calls (counted)
     * @return (port reference, full port name)
     * @~
     */
    std::pair<RTC::PortService_var, std::string> lookup_port(RTC::RTObject_ptr comp, const std::string& port,
                                                             const std::string& module, const std::string& planned_instance,
                                                             int& calls);
    /*!
     * @~japanese
     * @brief RTC のポートのキャッシュを消す
     *
     * @param comp RTC
     * @~english
     * @brief Clears the port cache of an RTC
     *
     * @param comp RTC
     * @~
     */
    void forget_ports(RTC::RTObject_ptr comp);
    /*!
     * @~japanese
     * @brief RTC を 1 つ非アクティブにし、接続を切り、削除する
     *
     * @param name RTC の名前
     * @param robot ロボット名
     * @param comp RTC
     * @~english
     * @brief Deactivates, disconnects and deletes one RTC
     *
     * @param name name of the RTC
     * @param robot robot name
     * @param comp RTC
     * @~
     */
    void remove_one(const std::string& name, const std::string& robot, RTC::RTObject_ptr comp);
    /*!
     * @~japanese
     * @brief マイグレーションの計画を適用する
     *
     * @return 適用したら true
     * @~english
     * @brief Applies the migration plan
     *
     * @return true if applied
     * @~
     */
    bool migration();
    /*!
     * @~japanese
     * @brief 条件が成り立ったときのマイグレーションを行う
     * @~english
     * @brief Performs the migration on conditions
     * @~
     */
    void run_migration_from_conditions();
    /*!
     * @~japanese
     * @brief SNMP の探索を待ち、起動時の構成を 1 回だけ適用する
     * @~english
     * @brief Waits for SNMP discovery, then applies the startup plan once
     * @~
     */
    void startup_configuration();
    /*!
     * @~japanese
     * @brief cgroup の空き・get_loadable_modules()・placement.xml から配置先を決め、manual の計画にして返す
     *
     * @param config 構成ファイルの内容
     * @param plan auto の計画
     * @param exclude 置かないロボット
     * @param strict 全ロボットがそろうまで待つか (起動時)
     * @param force_standby 待機モジュールを必ず使うか
     * @return (manual の計画, 説明の行)
     * @~english
     * @brief Decides the placement from free cgroup resources, get_loadable_modules() and placement.xml, and returns a manual plan
     *
     * @param config contents of the configuration file
     * @param plan auto plan
     * @param exclude robots not to place on
     * @param strict whether to wait until all robots are present (startup)
     * @param force_standby whether standby modules must be used
     * @return (manual plan, lines of explanation)
     * @~
     */
    std::pair<json, std::vector<std::string>> auto_plan(const json& config, const json& plan,
                                                        const std::vector<std::string>& exclude = {},
                                                        bool strict = true, bool force_standby = false);
    /*!
     * @~japanese
     * @brief ロボットの空き (CPU・メモリ、予約、通信状態) を PlacementRobot にする
     *
     * @param name ロボット名
     * @param config 構成ファイルの内容
     * @param languages 既に slave が動いている言語
     * @param rcfg メモリの予約の設定
     * @param link_rules ロボット間の制約
     * @param pending 置く途中の予約
     * @return PlacementRobot
     * @~english
     * @brief Makes a PlacementRobot with the free resources of a robot (CPU, memory, reservations, link quality)
     *
     * @param name robot name
     * @param config contents of the configuration file
     * @param languages languages whose slaves are already running
     * @param rcfg memory reservation settings
     * @param link_rules inter-robot constraints
     * @param pending reservations being placed
     * @return PlacementRobot
     * @~
     */
    PlacementRobot robot_capacity(const std::string& name, const json& config, const std::set<std::string>& languages,
                                  const json* rcfg, const json& link_rules, const std::map<std::string, double>& pending);
    /*!
     * @~japanese
     * @brief objective のロジックの目的関数を返す (objective_plugin があればプラグイン、なければ重み付き和)
     *
     * @param plan 計画
     * @return 目的関数
     * @~english
     * @brief Returns the objective function of the objective logic (the plugin if objective_plugin is given, otherwise the weighted sum)
     *
     * @param plan plan
     * @return objective function
     * @~
     */
    Objective objective_fn(const json& plan);

    // ---- 自動復旧 (切り替えの条件と段階的な復旧)
    /*!
     * @~japanese
     * @brief 切り替えの条件を満たす RTC (名前、原因、ロボット、説明)
     * @~english
     * @brief RTC satisfying a switching condition (name, cause, robot, description)
     * @~
     */
    struct Problem { std::string name, cause, robot, detail; };
    /*!
     * @~japanese
     * @brief 計画の RTC が今動いている場所 (ロボット、インスタンス、状態)
     * @~english
     * @brief Where an RTC of the plan is running now (robot, instance, state)
     * @~
     */
    struct Location { std::string robot, instance, state; };   // state: ACTIVE / ERROR / ... / down / missing / "" (不明)
    /*!
     * @~japanese
     * @brief 自動復旧の設定を返す
     *
     * @param config 構成ファイルの内容
     * @return 設定 (無効なら nullptr)
     * @~english
     * @brief Returns the recovery settings
     *
     * @param config contents of the configuration file
     * @return settings (nullptr if disabled)
     * @~
     */
    static const json* recovery_cfg(const json& config);
    /*!
     * @~japanese
     * @brief 自動復旧のスレッドの本体
     * @~english
     * @brief Body of the recovery thread
     * @~
     */
    void recovery_loop();
    /*!
     * @~japanese
     * @brief ロボットの切り替えの条件のうち、今満たしているものを返す
     *
     * @param robot ロボット
     * @param conds 切り替えの条件
     * @return [(原因, 説明)]
     * @~english
     * @brief Returns the robot switching conditions that currently hold
     *
     * @param robot robot
     * @param conds switching conditions
     * @return [(cause, description)]
     * @~
     */
    std::vector<std::pair<std::string, std::string>> robot_problems(const std::shared_ptr<Robot>& robot, const json& conds);
    /*!
     * @~japanese
     * @brief 追加の監視の値の制約 (metric_constraints) を満たさないロボットを返す
     *
     * @param cfg 構成ファイルの内容
     * @param constraints 制約
     * @param already 既に除いたロボット (見ない)
     * @return ロボット名のリスト
     * @~english
     * @brief Returns the robots that violate the custom metric constraints (metric_constraints)
     *
     * @param cfg contents of the configuration file
     * @param constraints constraints
     * @param already robots already excluded (not checked)
     * @return list of robot names
     * @~
     */
    std::vector<std::string> metric_excluded(const json& cfg, const json& constraints, const std::vector<std::string>& already);
    /*!
     * @~japanese
     * @brief 追加の監視の値の名前を返す
     *
     * @return 名前のリスト
     * @~english
     * @brief Returns the names of the custom metrics
     *
     * @return list of names
     * @~
     */
    std::vector<std::string> custom_names() const;
    // 追加の監視の値 (snmp.metrics): [(名前, OID)]
    std::vector<std::pair<std::string, std::string>> custom_metrics_;
    /*!
     * @~japanese
     * @brief 計画の RTC が今どこで動いているかを返す
     *
     * @param name 計画の RTC の名前
     * @param entry 計画の項目
     * @return Location (state は ACTIVE / ERROR / ... / down / missing / "" (不明))
     * @~english
     * @brief Returns where an RTC of the plan is running now
     *
     * @param name name of the RTC in the plan
     * @param entry entry of the plan
     * @return Location (state is ACTIVE / ERROR / ... / down / missing / "" (unknown))
     * @~
     */
    Location recovery_locate(const std::string& name, const json& entry);
    /*!
     * @~japanese
     * @brief 切り替えの条件を満たす RTC を返す
     *
     * @param config 構成ファイルの内容
     * @param rc 自動復旧の設定
     * @return Problem のリスト
     * @~english
     * @brief Returns the RTCs that satisfy the switching conditions
     *
     * @param config contents of the configuration file
     * @param rc recovery settings
     * @return list of Problem
     * @~
     */
    std::vector<Problem> recovery_problems(const json& config, const json& rc);
    /*!
     * @~japanese
     * @brief 自動復旧の 1 回分の確認と処理
     *
     * @param config 構成ファイルの内容
     * @param rc 自動復旧の設定
     * @~english
     * @brief One round of checking and handling for recovery
     *
     * @param config contents of the configuration file
     * @param rc recovery settings
     * @~
     */
    void recovery_tick(const json& config, const json& rc);
    /*!
     * @~japanese
     * @brief 段階的に復旧する (同じロボットで試し、だめなら別のロボットへ)
     *
     * @param config 構成ファイルの内容
     * @param rc 自動復旧の設定
     * @param due 復旧する RTC
     * @~english
     * @brief Recovers step by step (tries on the same robot, otherwise another robot)
     *
     * @param config contents of the configuration file
     * @param rc recovery settings
     * @param due RTCs to recover
     * @~
     */
    void recover(const json& config, const json& rc, const std::vector<Problem>& due);
    /*!
     * @~japanese
     * @brief 段階の後、その RTC の切り替えの条件がまだ成り立つかを調べる
     *
     * @param config 構成ファイルの内容
     * @param rc 自動復旧の設定
     * @param name RTC の名前
     * @param cause 原因
     * @return 直っていれば true
     * @~english
     * @brief Checks after a stage whether the switching condition of the RTC still holds
     *
     * @param config contents of the configuration file
     * @param rc recovery settings
     * @param name name of the RTC
     * @param cause cause
     * @return true if resolved
     * @~
     */
    bool recovery_resolved(const json& config, const json& rc, const std::string& name, const std::string& cause);
    /*!
     * @~japanese
     * @brief RTC につながる接続と、相手の RTC (動かさない既存の RTC) を含む計画を作る
     *
     * @param config 構成ファイルの内容
     * @param names RTC の名前
     * @param extra 追加の項目
     * @return 計画
     * @~english
     * @brief Makes a plan with the connections of the RTCs and their peer RTCs (existing RTCs that are not moved)
     *
     * @param config contents of the configuration file
     * @param names names of the RTCs
     * @param extra additional components
     * @return plan
     * @~
     */
    json recovery_peer_plan(const json& config, const std::set<std::string>& names, const json& extra);
    /*!
     * @~japanese
     * @brief 同じロボットでの段階 (reset / standby_same / restart) を行う
     *
     * @param config 構成ファイルの内容
     * @param rc 自動復旧の設定
     * @param stage 段階
     * @param name RTC の名前
     * @param cause 原因
     * @return 直れば true
     * @~english
     * @brief Performs a stage on the same robot (reset / standby_same / restart)
     *
     * @param config contents of the configuration file
     * @param rc recovery settings
     * @param stage stage
     * @param name name of the RTC
     * @param cause cause
     * @return true if resolved
     * @~
     */
    bool recovery_stage_local(const json& config, const json& rc, const std::string& stage, const std::string& name,
                              const std::string& cause);
    /*!
     * @~japanese
     * @brief 故障した RTC を終了させ、消えるまで待つ
     *
     * @param name RTC の名前
     * @param inst インスタンス名
     * @param robot ロボット名
     * @param comp RTC
     * @param ms マネージャの集まり
     * @~english
     * @brief Terminates a failed RTC and waits until it disappears
     *
     * @param name name of the RTC
     * @param inst instance name
     * @param robot robot name
     * @param comp RTC
     * @param ms manager set
     * @~
     */
    void recovery_terminate(const std::string& name, const std::string& inst, const std::string& robot,
                            RTC::RTObject_ptr comp, const ManagerSet& ms);
    /*!
     * @~japanese
     * @brief 別のロボットへ切り替える (待機モジュールに切り替えるか、別のロボットで起動する)
     *
     * @param config 構成ファイルの内容
     * @param rc 自動復旧の設定
     * @param switching 切り替える RTC
     * @return {RTC 名: 移した先のロボット}
     * @~english
     * @brief Switches to another robot (switches to standby modules or starts on another robot)
     *
     * @param config contents of the configuration file
     * @param rc recovery settings
     * @param switching RTCs to switch
     * @return {RTC name: destination robot}
     * @~
     */
    std::map<std::string, std::string> recovery_switch_other(const json& config, const json& rc,
                                                             const std::vector<Problem>& switching);
    /*!
     * @~japanese
     * @brief 捨てた CPU のサンプル (get_loadable_modules() の先読みの後) がそろうまで待つ
     *
     * 応答のないロボットのサンプルはそろわないので、捨ててから cgroup_wait_seconds_ 秒までで打ち切る
     * @~english
     * @brief Waits until the CPU samples discarded after prefetching get_loadable_modules() are collected again
     *
     * Gives up cgroup_wait_seconds_ seconds after they were discarded, since robots not responding never get samples
     * @~
     */
    void wait_cgroup_samples();
    /*!
     * @~japanese
     * @brief slave の異常終了を記録する
     *
     * @param robot ロボット名
     * @param rtcs その slave の RTC
     * @~english
     * @brief Records a slave crash
     *
     * @param robot robot name
     * @param rtcs RTCs of the slave
     * @~
     */
    void mark_crashed(const std::string& robot, const std::vector<std::string>& rtcs);
    /*!
     * @~japanese
     * @brief slave の異常終了 (error=false) か、オブザーバーの ERROR の通知 (error=true) を最近受けたか
     *
     * @param name RTC の名前
     * @param now 現在の時刻
     * @param error オブザーバーの通知を見るか
     * @return 最近受けていれば true
     * @~english
     * @brief Whether a slave crash (error=false) or an ERROR notification from the observer (error=true) was received recently
     *
     * @param name name of the RTC
     * @param now current time
     * @param error whether to look at observer notifications
     * @return true if received recently
     * @~
     */
    bool crash_hint(const std::string& name, double now, bool error = false);

    // ---- コンポーネントオブザーバー (オブザーバ拡張マイグレーション)
    /*!
     * @~japanese
     * @brief RTC にコンポーネントオブザーバーを関連付ける (オブザーバ拡張マイグレーション)
     *
     * @param comps RTC の一覧
     * @~english
     * @brief Attaches component observers to RTCs (observer-extended migration)
     *
     * @param comps list of RTCs
     * @~
     */
    void attach_observers(const std::vector<rtcops::Component>& comps);
    /*!
     * @~japanese
     * @brief オブザーバーの通知を受ける (ORB のスレッドから呼ばれる)。ERROR なら自動復旧を待たずに始める
     *
     * @param instance インスタンス名
     * @param kind 状態の種別
     * @param hint 詳細
     * @~english
     * @brief Receives a notification of the observer (called from an ORB thread). On ERROR, recovery starts without waiting
     *
     * @param instance instance name
     * @param kind status kind
     * @param hint hint
     * @~
     */
    void on_observer_status(const std::string& instance, const std::string& kind, const std::string& hint);
    /*!
     * @~japanese
     * @brief RTC のエラーを記録する
     *
     * @param robot ロボット名
     * @param instance インスタンス名
     * @~english
     * @brief Records an RTC error
     *
     * @param robot robot name
     * @param instance instance name
     * @~
     */
    void mark_error(const std::string& robot, const std::string& instance);
    /*!
     * @~japanese
     * @brief 自動復旧のスレッドを起こす
     * @~english
     * @brief Wakes up the recovery thread
     * @~
     */
    void wake_recovery();
    /*!
     * @~japanese
     * @brief 配置プラグインの結果を受け取り、制約 (置けるロボット・空き容量) を満たすか確かめる
     *
     * @param plan 計画
     * @param items 配置する RTC
     * @param robots ロボット
     * @param edges 接続
     * @param primary 主ロボットの名前
     * @return PlacementResult
     * @~english
     * @brief Receives the result of a placement plugin and checks the constraints (allowed robots and free capacity)
     *
     * @param plan plan
     * @param items RTCs to place
     * @param robots robots
     * @param edges connections
     * @param primary name of the primary robot
     * @return PlacementResult
     * @~
     */
    PlacementResult plugin_placement(const json& plan, const std::vector<PlacementItem>& items,
                                     const std::vector<PlacementRobot>& robots, const std::vector<PlacementEdge>& edges,
                                     const std::string& primary);
    /*!
     * @~japanese
     * @brief 起動時の構成の失敗 (システム起動失敗) を記録する
     *
     * @param message メッセージ
     * @param reasons 理由の一覧
     * @~english
     * @brief Records a failure of the startup configuration (system startup failure)
     *
     * @param message message
     * @param reasons list of reasons
     * @~
     */
    void startup_failed(const std::string& message, const std::vector<std::string>& reasons);

    // ---- マネージャによる実測値の計測と保存
    /*!
     * @~japanese
     * @brief マネージャによる実測値を定期的に集めて保存するスレッドの本体
     * @~english
     * @brief Body of the thread that periodically collects and saves the measurements made by the managers
     * @~
     */
    void measure_loop();
    /*!
     * @~japanese
     * @brief 自分のロボットの slave が書いた計測を集め、runs.json・placement.xml・SQLite に保存する
     *
     * @return 保存した placement.xml のリスト
     * @~english
     * @brief Collects the measurements written by the slaves of this robot and saves them to runs.json, placement.xml and SQLite
     *
     * @return list of saved placement.xml files
     * @~
     */
    std::vector<std::string> collect_measurements();
    /*!
     * @~japanese
     * @brief placement.xml を探すディレクトリ (カンマ区切り) を返す
     *
     * @return ディレクトリ
     * @~english
     * @brief Returns the directories to search for placement.xml (comma separated)
     *
     * @return directories
     * @~
     */
    std::string profile_roots() const;

    // ---- slave マネージャの終了の検知 (feature/processcallback 相当)
    /*!
     * @~japanese
     * @brief slave マネージャの終了の検知を始める
     * @~english
     * @brief Starts detecting the exit of slave managers
     * @~
     */
    void install_process_observer();
    /*!
     * @~japanese
     * @brief slave が終わったときに呼ばれる。0 以外で終わったら異常終了として復旧を急ぐ
     *
     * @param code 終了コード (シグナルなら負)
     * @param slave slave マネージャの名前
     * @param pid プロセス ID
     * @~english
     * @brief Called when a slave exits. A non-zero code is treated as a crash and recovery is hastened
     *
     * @param code exit code (negative for signals)
     * @param slave name of the slave manager
     * @param pid process ID
     * @~
     */
    void on_slave_exit(int code, const std::string& slave, int pid);

    // ---- マイグレーション条件
    /*!
     * @~japanese
     * @brief マイグレーションの条件を 1 つ評価する
     *
     * @param c 条件
     * @param now 現在の時刻
     * @return 満たせば true
     * @~english
     * @brief Evaluates one migration condition
     *
     * @param c condition
     * @param now current time
     * @return true if satisfied
     * @~
     */
    bool evaluate_condition(const json& c, double now);
    /*!
     * @~japanese
     * @brief 条件のグループを評価する (AND / OR)
     *
     * @param g 条件のグループ
     * @param now 現在の時刻
     * @return 満たせば true
     * @~english
     * @brief Evaluates a condition group (AND / OR)
     *
     * @param g condition group
     * @param now current time
     * @return true if satisfied
     * @~
     */
    bool evaluate_conditions(const json& g, double now);
    /*!
     * @~japanese
     * @brief 監視の値でマイグレーションの条件を評価する
     * @~english
     * @brief Evaluates the migration rules with the monitoring values
     * @~
     */
    void check_migration_conditions();
    /*!
     * @~japanese
     * @brief 自分のマスターマネージャと slave マネージャを終了させる (slave を先に)
     * @~english
     * @brief Shuts down the local master manager and its slave managers (slaves first)
     * @~
     */
    void shutdown_own_managers();

    RTC::Manager& mgr_;
    CORBA::ORB_var orb_;
    std::string name_;
    std::string own_ip_;   // rtc.conf の snmp.own_ip (空なら、自分のインターフェースのアドレスから決める)
    int snmp_port_{161}, trap_port_{162}, corba_port_{2810};
    std::string community_{"public"}, discovery_mode_{"trap"}, migration_config_file_;
    std::unique_ptr<SnmpClient> snmp_;
    std::unique_ptr<TrapReceiver> traps_;
    std::unique_ptr<Store> store_;
    std::unique_ptr<PluginManager> plugins_;
    bool plugins_unavailable_{false};

    std::recursive_mutex robots_mutex_;
    // auto_plan: get_loadable_modules() の結果 (リトライ間で使い回す)
    std::shared_ptr<Robot> self_;
    std::vector<std::shared_ptr<Robot>> robots_;
    std::vector<std::string> targets_;
    double last_discovery_{0};

    std::atomic<bool> running_{false};
    std::vector<std::thread> threads_;

    std::mutex migration_mutex_;
    bool migrating_{false};
    std::atomic<bool> startup_completed_{false};
    std::atomic<bool> migration_done_{false};
    std::mutex condition_mutex_;
    std::optional<double> condition_first_true_, last_migration_attempt_;
    std::optional<std::string> last_logged_dbm_;
    bool wifi_low_{false};
    double wifi_low_time_{0};
    bool shutdown_started_{false};
    std::mutex logfile_mutex_;

    // 実測値の計測と保存
    bool measure_enabled_{false};
    json measure_runs_;
    bool measure_runs_loaded_{false};
    std::map<std::pair<std::string, std::string>, std::pair<double, double>> measure_saved_;
    // slave の終了の検知
    bool observer_enabled_{false};

    // 実行ロボットの選出・起動時の構成
    double started_at_{0};
    std::mutex elect_mutex_;
    std::map<std::string, std::string> last_elected_;
    std::mutex startup_mutex_;
    std::condition_variable startup_cv_;
    bool startup_querying_{false};
    bool startup_replied_{false};
    std::string startup_reported_by_;
    std::set<std::string> startup_pending_from_, startup_query_targets_;
    std::atomic<bool> startup_known_{false};
    std::atomic<bool> startup_failed_{false};
    std::atomic<bool> migration_in_progress_{false};
    // 見込み値 (計測の保存先が変わったら読み直す)
    std::mutex profile_mutex_;
    std::shared_ptr<ProfileDB> profile_db_;
    std::string profile_sig_;
    // get_loadable_modules() の結果 (ロボット -> 実装 ID -> プロファイル)
    std::mutex module_mutex_;
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> loadable_modules_;
    std::set<std::string> loadable_checked_;
    std::atomic<bool> prefetching_{false};           // 先に取っている間 (自動復旧の判断を止める)
    std::atomic<bool> loadable_prefetched_{false};   // 復旧・マイグレーションのために先に取ったか
    // 先読みの後に CPU のサンプルを捨てた時刻 (monotonic。0 は捨てていない、またはそろった)。robots_mutex_ で守る
    double cgroup_reset_at_ = 0;
    static constexpr double cgroup_wait_seconds_ = 30.0;
    /*!
     * @~japanese
     * @brief 復旧・マイグレーションのために get_loadable_modules() の結果を先に取っておく
     *
     * @param cfg 構成ファイルの内容
     * @~english
     * @brief Fetches the results of get_loadable_modules() in advance for recovery and migration
     *
     * @param cfg contents of the configuration file
     * @~
     */
    void prefetch_loadable(const json& cfg);
    std::set<std::string> inventory_failing_;   // RTC の一覧を取れていないロボット (ログを 1 回だけ出す)
    /*!
     * @~japanese
     * @brief ポートの参照のキャッシュの 1 件 (インスタンス名と、ポートの名前 -> 参照)
     * @~english
     * @brief One entry of the port reference cache (instance name and port name -> reference)
     * @~
     */
    struct PortEntry { std::string instance; std::map<std::string, RTC::PortService_var> ports; };
    std::mutex port_mutex_;
    std::map<std::string, PortEntry> port_cache_;                 // RTC の IOR -> ポート
    std::map<std::string, std::vector<std::string>> port_index_;   // モジュール名 -> get_ports() の並びの短い名前
    // 自動復旧
    std::mutex recovery_mutex_;
    std::map<std::string, std::pair<std::string, std::string>> recovery_where_;   // 計画の RTC -> (ロボット, インスタンス)
    std::map<std::pair<std::string, std::string>, double> recovery_first_seen_;
    std::map<std::string, double> recovery_cooldown_;
    std::map<std::string, std::pair<std::string, double>> crashed_;               // インスタンス -> (ロボット, 時刻)
    std::map<std::string, std::pair<std::string, double>> error_notified_;        // オブザーバーの ERROR の通知
    std::set<std::string> notified_logged_;
    // コンポーネントオブザーバー
    bool observer_rtc_enabled_{false};
    std::mutex observer_mutex_;
    std::map<std::string, observer::Handle> observers_;   // RTC の IOR -> サーバーオブジェクト
    std::set<std::string> observer_failed_;               // 関連付けに失敗した RTC の IOR (試し直さない)
    std::set<std::string> observer_noticed_;              // 最初の通知をログに出した RTC
    std::atomic<bool> recovering_{false};
    std::atomic<double> last_plan_applied_{0};
    std::mutex wake_mutex_;
    std::condition_variable wake_cv_;
    bool wake_{false};
  };
} // namespace snmpmgr

#endif
