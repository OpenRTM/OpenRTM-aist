// -*- C++ -*-
/*!
 * @file Plugin.hpp
 * @brief Plugin API of SNMPManager (C++ version)
 *
 * @~japanese
 * SNMPManager (C++ 版) のプラグイン API
 *
 * プラグインは共有ライブラリ (lib\<名前\>.so または \<名前\>.so。Windows は \<名前\>.dll) で、次の 2 つの C 関数を公開する。
 * @code
 *   extern "C" int  snmpmgr_plugin_api_version();          // SNMPMGR_PLUGIN_API_VERSION を返す
 *   extern "C" void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char* config_json);
 * @endcode
 * setup の中で ctx->register_condition / register_action / register_placement / register_objective / subscribe を呼んで登録する。拡張ポイントと構成ファイルでの書き方は Python 版 (plugins/README.md) と同じ。
 * @~english
 * A plugin is a shared library (lib\<name\>.so or \<name\>.so; \<name\>.dll on Windows) that exports the following two C functions.
 * @code
 *   extern "C" int  snmpmgr_plugin_api_version();          // returns SNMPMGR_PLUGIN_API_VERSION
 *   extern "C" void snmpmgr_plugin_setup(snmpmgr::PluginContext* ctx, const char* config_json);
 * @endcode
 * In setup, functions are registered by calling ctx->register_condition / register_action / register_placement / register_objective / subscribe. The extension points and the configuration file syntax are the same as the Python version (plugins/README.md).
 * @~
 *
 * @date $Date: 2026-11-13 16:24:25 $
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

#ifndef SNMPMGR_PLUGIN_HPP
#define SNMPMGR_PLUGIN_HPP

#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

/*!
 * @~japanese
 * @brief プラグインの API のバージョン (3: RobotInfo に metrics (追加の監視の値) を足した)
 * @~english
 * @brief Version of the plugin API (3: added metrics (additional monitoring values) to RobotInfo)
 * @~
 */
#define SNMPMGR_PLUGIN_API_VERSION 3   // 3: RobotInfo に metrics (追加の監視の値) を足した

namespace RTM { class Manager; }

namespace snmpmgr
{
  using json = nlohmann::json;

  /*!
   * @~japanese
   * @brief ロボットで動いている RTC (RTC の一覧。Python 版の robots()[n]["inventory"] の要素と同じ)
   * @~english
   * @brief RTC running on a robot (RTC list; same as an element of robots()[n]["inventory"] in the Python version)
   * @~
   */
  struct ComponentInfo
  {
    std::string name, type, impl, language;
    std::string state;                // ACTIVE / INACTIVE / ERROR / CREATED / UNKNOWN
    int slave{0};                     // 0 = マスター、1.. = slave マネージャ
    std::string slave_name;           // slave マネージャの manager.instance_name
  };

  /*!
   * @~japanese
   * @brief ロボットの状態のスナップショット
   * @~english
   * @brief Snapshot of the state of a robot
   * @~
   */
  struct RobotInfo
  {
    std::string name;
    std::string ip;
    std::string status;               // alive / dead / unknown
    bool is_local{false};
    std::optional<double> cpu_usage, memory_usage, network_in, network_out, wifi_signal_dbm;
    std::optional<double> latency_ms, jitter_ms, packet_loss;
    std::optional<double> cg_cpu_usage_pct, cg_cpu_limit_pct, cg_mem_usage_mb, cg_mem_limit_mb;
    std::optional<double> battery_pct;
    std::optional<std::vector<ComponentInfo>> inventory;   // 分からなければ nullopt
    std::optional<double> base_mem_mb;                     // RTC がないときのメモリ使用量 (予約の土台)
    std::map<std::string, std::optional<double>> metrics;  // 追加の監視の値 (rtc.conf の snmp.metrics)
  };

  /*!
   * @~japanese
   * @brief 配置の対象の RTC (RTCPlacement.py の Item と同じ)
   * @~english
   * @brief RTC to place (same as Item in RTCPlacement.py)
   * @~
   */
  struct PlacementItem
  {
    std::string name;                 // インスタンス名 (components[].name)
    std::string module;
    std::string language;
    double cpu{0};
    double memory_mb{0};
    double slave_overhead_mb{0};
    std::vector<std::string> candidates;   // 置いてよいロボット
    std::string standby;              // 待機モジュールを使う場合、そのインスタンス名
    bool fixed{false};                // 既存の RTC (動かさない。通信量の計算にだけ使う)
  };

  /*!
   * @~japanese
   * @brief 配置先のロボット (RTCPlacement.py の Robot と同じ)
   * @~english
   * @brief Robot to place on (same as Robot in RTCPlacement.py)
   * @~
   */
  struct PlacementRobot
  {
    std::string name;
    double cpu_free{0};
    double mem_free_mb{0};
    std::vector<std::string> tags;
    std::vector<std::string> languages;    // 既に slave マネージャが動いている言語
    double cpu_limit{0};
    double mem_limit_mb{0};
    bool link_ok{true};                    // 通信状態がロボット間の制約を満たすか (false ならまたぐ接続を置かない)
    std::map<std::string, double> link;    // latency_ms / jitter_ms / packet_loss / rssi_dbm (分かるものだけ)
    double reserved_mb{0};                 // 予約済みのメモリ (説明用。mem_free_mb には反映済み)
  };

  /*!
   * @~japanese
   * @brief RTC 間の接続 (RTCPlacement.py の Edge と同じ)
   * @~english
   * @brief Connection between RTCs (same as Edge in RTCPlacement.py)
   * @~
   */
  struct PlacementEdge
  {
    std::string a, b;
    double weight{0};
    std::string kind;                      // data / service
  };

  /*!
   * @~japanese
   * @brief イベント (時刻、観測したロボット、対象のロボット、種別、詳細)
   * @~english
   * @brief Event (time, observing robot, target robot, kind, details)
   * @~
   */
  struct Event
  {
    double time{0};
    std::string observer, robot, kind, detail;
  };

  /*!
   * @~japanese
   * @brief 目的関数 (objective のロジック) に渡す途中の状態 (RTCPlacement.py の state と同じ)
   * @~english
   * @brief Intermediate state passed to the objective function (objective logic) (same as state in RTCPlacement.py)
   * @~
   */
  struct ObjectiveState
  {
    /*!
     * @~japanese
     * @brief ロボットに置いた分 (CPU、メモリ、言語)
     * @~english
     * @brief Amount placed on a robot (CPU, memory, languages)
     * @~
     */
    struct Usage { double cpu{0}; double mem{0}; std::vector<std::string> langs; };
    std::map<std::string, std::string> assign;   // 置き終わった RTC -> ロボット
    std::map<std::string, Usage> usage;          // ロボット -> 置いた分
    std::vector<PlacementEdge> edges;
    std::string primary;
    std::vector<PlacementRobot> robots;
  };

  class PluginContext;
  /*!
   * @~japanese
   * @brief 条件の関数 fn(ctx, args, now) -> bool
   * @~english
   * @brief Condition function fn(ctx, args, now) -> bool
   * @~
   */
  using ConditionFn = std::function<bool(PluginContext&, const json& args, double now)>;
  /*!
   * @~japanese
   * @brief 処理の関数 fn(ctx, args) -> bool (成功したか)
   * @~english
   * @brief Action function fn(ctx, args) -> bool (whether it succeeded)
   * @~
   */
  using ActionFn = std::function<bool(PluginContext&, const json& args)>;
  /*!
   * @~japanese
   * @brief 配置の関数 fn(ctx, items, robots, edges, primary, args) -> {RTC 名: ロボット名}
   * @~english
   * @brief Placement function fn(ctx, items, robots, edges, primary, args) -> {RTC name: robot name}
   * @~
   */
  using PlacementFn = std::function<std::map<std::string, std::string>(
      PluginContext&, const std::vector<PlacementItem>&, const std::vector<PlacementRobot>&,
      const std::vector<PlacementEdge>&, const std::string& primary, const json& args)>;
  /*!
   * @~japanese
   * @brief イベントを受ける関数 fn(ctx, event)
   * @~english
   * @brief Event function fn(ctx, event)
   * @~
   */
  using EventFn = std::function<void(PluginContext&, const Event&)>;
  /*!
   * @~japanese
   * @brief 目的関数 (大きいほどよい)。RTC を 1 つずつ、制約を満たすロボットのうち値が最大のものに置く
   * @~english
   * @brief Objective function (larger is better). Each RTC is placed on the robot with the largest value among those satisfying the constraints
   * @~
   */
  using ObjectiveFn = std::function<double(PluginContext&, const PlacementItem&, const PlacementRobot&,
                                           const ObjectiveState&, const json& args)>;

  /*!
   * @~japanese
   * @brief 配置できないときにプラグインが投げる例外
   * @~english
   * @brief Exception thrown by plugins when the placement is impossible
   * @~
   */
  class PlacementError : public std::runtime_error
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param msg メッセージ
     * @param reasons 理由の一覧
     * @~english
     * @brief Constructor
     *
     * @param msg message
     * @param reasons list of reasons
     * @~
     */
    PlacementError(const std::string& msg, std::vector<std::string> reasons = {})
      : std::runtime_error(msg), reasons(std::move(reasons)) {}
    std::vector<std::string> reasons;
  };

  /*!
   * @~japanese
   * @brief プラグインに渡す API (SNMPManager が実装する)
   * @~english
   * @brief API given to plugins (implemented by SNMPManager)
   * @~
   */
  class PluginContext
  {
  public:
    /*!
     * @~japanese
     * @brief デストラクタ
     * @~english
     * @brief Destructor
     * @~
     */
    virtual ~PluginContext() = default;
    // ---- 登録
    /*!
     * @~japanese
     * @brief 条件を登録する
     *
     * @param name 名前
     * @param fn 条件の関数
     * @~english
     * @brief Registers a condition
     *
     * @param name name
     * @param fn condition function
     * @~
     */
    virtual void register_condition(const std::string& name, ConditionFn fn) = 0;
    /*!
     * @~japanese
     * @brief 処理を登録する
     *
     * @param name 名前
     * @param fn 処理の関数
     * @~english
     * @brief Registers an action
     *
     * @param name name
     * @param fn action function
     * @~
     */
    virtual void register_action(const std::string& name, ActionFn fn) = 0;
    /*!
     * @~japanese
     * @brief 配置のロジックを登録する
     *
     * @param name 名前
     * @param fn 配置の関数
     * @~english
     * @brief Registers a placement logic
     *
     * @param name name
     * @param fn placement function
     * @~
     */
    virtual void register_placement(const std::string& name, PlacementFn fn) = 0;
    /*!
     * @~japanese
     * @brief イベントを購読する
     *
     * @param kind イベントの種別 ("*" で全部)
     * @param fn イベントを受ける関数
     * @~english
     * @brief Subscribes to events
     *
     * @param kind event kind ("*" for all)
     * @param fn event function
     * @~
     */
    virtual void subscribe(const std::string& kind, EventFn fn) = 0;   // kind "*" で全部
    /*!
     * @~japanese
     * @brief 目的関数を登録する
     *
     * @param name 名前
     * @param fn 目的関数
     * @~english
     * @brief Registers an objective function
     *
     * @param name name
     * @param fn objective function
     * @~
     */
    virtual void register_objective(const std::string& name, ObjectiveFn fn) = 0;
    // ---- SNMPManager の機能
    /*!
     * @~japanese
     * @brief 自分のロボット名を返す
     *
     * @return ロボット名
     * @~english
     * @brief Returns the name of this robot
     *
     * @return robot name
     * @~
     */
    virtual std::string name() const = 0;                              // 自分のロボット名
    /*!
     * @~japanese
     * @brief このプラグインの名前を返す
     *
     * @return プラグインの名前
     * @~english
     * @brief Returns the name of this plugin
     *
     * @return plugin name
     * @~
     */
    virtual std::string plugin() const = 0;                            // このプラグインの名前
    /*!
     * @~japanese
     * @brief ログを出す
     *
     * @param message メッセージ
     * @~english
     * @brief Outputs a log message
     *
     * @param message message
     * @~
     */
    virtual void log(const std::string& message) = 0;
    /*!
     * @~japanese
     * @brief 自分を含む全ロボットの状態を返す
     *
     * @return ロボットの状態
     * @~english
     * @brief Returns the states of all robots including this one
     *
     * @return robot states
     * @~
     */
    virtual std::vector<RobotInfo> robots() = 0;                       // 自分を含む全ロボット
    /*!
     * @~japanese
     * @brief 生きているロボットの状態を返す
     *
     * @return ロボットの状態
     * @~english
     * @brief Returns the states of the alive robots
     *
     * @return robot states
     * @~
     */
    std::vector<RobotInfo> alive_robots()
    {
      std::vector<RobotInfo> out;
      for (auto& r : robots()) { if (r.status == "alive") { out.push_back(r); } }
      return out;
    }
    /*!
     * @~japanese
     * @brief ロボットのマスターマネージャを返す (RTM::Manager の参照。CORBA::Object を _narrow して使う)
     *
     * @param robot ロボット名
     * @return CORBA::Object* (なければ nullptr)
     * @~english
     * @brief Returns the master manager of a robot (reference of RTM::Manager; narrow CORBA::Object to use it)
     *
     * @param robot robot name
     * @return CORBA::Object* (nullptr if not available)
     * @~
     */
    virtual void* manager_object(const std::string& robot) = 0;
    /*!
     * @~japanese
     * @brief インスタンス名の RTC がそのロボット (スレーブを含む) にあるか
     *
     * @param instance インスタンス名
     * @param robot ロボット名
     * @return あれば true
     * @~english
     * @brief Whether an RTC with the instance name exists on the robot (including slaves)
     *
     * @param instance instance name
     * @param robot robot name
     * @return true if it exists
     * @~
     */
    virtual bool has_component(const std::string& instance, const std::string& robot) = 0;
    /*!
     * @~japanese
     * @brief migration と同じ形式の計画を適用する (生成・接続・アクティベート・削除)。失敗は例外
     *
     * @param plan 計画
     * @param label ログに出す名前
     * @~english
     * @brief Applies a plan in the same format as migration (create, connect, activate, remove). Failures throw exceptions
     *
     * @param plan plan
     * @param label name shown in logs
     * @~
     */
    virtual void apply_plan(const json& plan, const std::string& label) = 0;
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
    virtual std::string conf(const std::string& key, const std::string& def = "") = 0;
    /*!
     * @~japanese
     * @brief 現在の構成ファイルの内容を返す
     *
     * @return 構成ファイルの内容
     * @~english
     * @brief Returns the contents of the current configuration file
     *
     * @return contents of the configuration file
     * @~
     */
    virtual json migration_config() = 0;
    /*!
     * @~japanese
     * @brief イベントを記録する (ログと SQLite。ほかのプラグインの subscribe にも届く)
     *
     * @param robot ロボット名
     * @param kind イベントの種別
     * @param detail 詳細
     * @~english
     * @brief Records an event (logs and SQLite; also delivered to the subscribers of other plugins)
     *
     * @param robot robot name
     * @param kind event kind
     * @param detail details
     * @~
     */
    virtual void event(const std::string& robot, const std::string& kind, const std::string& detail = "") = 0;
  };
} // namespace snmpmgr

#ifdef _WIN32
/*!
 * @~japanese
 * @brief プラグインの C 関数を公開するための指定
 * @~english
 * @brief Specifier to export the C functions of a plugin
 * @~
 */
#define SNMPMGR_PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#define SNMPMGR_PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

#endif // SNMPMGR_PLUGIN_HPP
