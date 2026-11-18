// -*- C++ -*-
/*!
 * @file PluginManager.hpp
 * @brief Loading and calling C++ plugins (.so)
 *
 * @~japanese
 * C++ プラグイン (.so) の読み込みと呼び出し
 * @~
 *
 * @date $Date: 2026-11-13 16:24:29 $
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

#ifndef SNMPMGR_PLUGINS_HPP
#define SNMPMGR_PLUGINS_HPP

#include <condition_variable>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <snmpmgr/Plugin.hpp>

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief プラグインに渡す SNMPManager の機能 (SNMPManager が実装する)
   * @~english
   * @brief Functions of SNMPManager given to plugins (implemented by SNMPManager)
   * @~
   */
  class PluginHost
  {
  public:
    /*!
     * @~japanese
     * @brief デストラクタ
     * @~english
     * @brief Destructor
     * @~
     */
    virtual ~PluginHost() = default;
    /*!
     * @~japanese
     * @brief 自分のロボットの名前を返す
     *
     * @return 名前
     * @~english
     * @brief Returns the name of this robot
     *
     * @return name
     * @~
     */
    virtual std::string host_name() const = 0;
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
    virtual void host_log(const std::string& message) = 0;
    /*!
     * @~japanese
     * @brief 全ロボットの状態を返す
     *
     * @return ロボットの状態
     * @~english
     * @brief Returns the states of all robots
     *
     * @return robot states
     * @~
     */
    virtual std::vector<RobotInfo> host_robots() = 0;
    /*!
     * @~japanese
     * @brief ロボットのマスターマネージャの参照を返す
     *
     * @param robot ロボット名
     * @return CORBA::Object* (なければ nullptr)
     * @~english
     * @brief Returns the reference of the master manager of a robot
     *
     * @param robot robot name
     * @return CORBA::Object* (nullptr if not available)
     * @~
     */
    virtual void* host_manager_object(const std::string& robot) = 0;
    /*!
     * @~japanese
     * @brief RTC がそのロボットにあるか
     *
     * @param instance インスタンス名
     * @param robot ロボット名
     * @return あれば true
     * @~english
     * @brief Whether an RTC exists on the robot
     *
     * @param instance instance name
     * @param robot robot name
     * @return true if it exists
     * @~
     */
    virtual bool host_has_component(const std::string& instance, const std::string& robot) = 0;
    /*!
     * @~japanese
     * @brief 計画を適用する
     *
     * @param plan 計画
     * @param label ログに出す名前
     * @~english
     * @brief Applies a plan
     *
     * @param plan plan
     * @param label name shown in logs
     * @~
     */
    virtual void host_apply_plan(const json& plan, const std::string& label) = 0;
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
    virtual std::string host_conf(const std::string& key, const std::string& def) = 0;
    /*!
     * @~japanese
     * @brief 構成ファイルの内容を返す
     *
     * @return 構成ファイルの内容
     * @~english
     * @brief Returns the contents of the configuration file
     *
     * @return contents of the configuration file
     * @~
     */
    virtual json host_migration_config() = 0;
    /*!
     * @~japanese
     * @brief イベントを記録する
     *
     * @param robot ロボット名
     * @param kind イベントの種別
     * @param detail 詳細
     * @~english
     * @brief Records an event
     *
     * @param robot robot name
     * @param kind event kind
     * @param detail details
     * @~
     */
    virtual void host_event(const std::string& robot, const std::string& kind, const std::string& detail) = 0;
  };

  /*!
   * @~japanese
   * @brief プラグインの読み込みや登録の誤りを表す例外
   * @~english
   * @brief Exception for errors in loading or registering plugins
   * @~
   */
  class PluginError : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  /*!
   * @~japanese
   * @brief プラグインを読み込み、登録された関数を呼び出す
   * @~english
   * @brief Loads plugins and calls the registered functions
   * @~
   */
  class PluginManager
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param host SNMPManager の機能
     * @param paths プラグインを探すディレクトリ
     * @~english
     * @brief Constructor
     *
     * @param host functions of SNMPManager
     * @param paths directories to search for plugins
     * @~
     */
    PluginManager(PluginHost& host, std::vector<std::string> paths);
    /*!
     * @~japanese
     * @brief デストラクタ。イベントのスレッドを止め、プラグインを閉じる
     * @~english
     * @brief Destructor. Stops the event thread and closes the plugins
     * @~
     */
    ~PluginManager();
    /*!
     * @~japanese
     * @brief プラグインをすべて読み込む
     *
     * @param specs [{"module": ..., "config": {...}, "name": ...}]
     * @return 読み込んだら true (内容が同じなら読み込み直さず false)
     * @~english
     * @brief Loads all plugins
     *
     * @param specs [{"module": ..., "config": {...}, "name": ...}]
     * @return true if loaded (false without reloading if the specs are the same)
     * @~
     */
    bool load_all(const json& specs);
    /*!
     * @~japanese
     * @brief 読み込んだプラグインと登録された関数の説明を返す
     *
     * @return 説明文
     * @~english
     * @brief Returns a description of the loaded plugins and registered functions
     *
     * @return description
     * @~
     */
    std::string describe();
    /*!
     * @~japanese
     * @brief 条件が登録されているか
     *
     * @param n 名前
     * @return 登録されていれば true
     * @~english
     * @brief Whether the condition is registered
     *
     * @param n name
     * @return true if registered
     * @~
     */
    bool has_condition(const std::string& n);
    /*!
     * @~japanese
     * @brief 処理が登録されているか
     *
     * @param n 名前
     * @return 登録されていれば true
     * @~english
     * @brief Whether the action is registered
     *
     * @param n name
     * @return true if registered
     * @~
     */
    bool has_action(const std::string& n);
    /*!
     * @~japanese
     * @brief 配置のロジックが登録されているか
     *
     * @param n 名前
     * @return 登録されていれば true
     * @~english
     * @brief Whether the placement logic is registered
     *
     * @param n name
     * @return true if registered
     * @~
     */
    bool has_placement(const std::string& n);
    /*!
     * @~japanese
     * @brief 目的関数が登録されているか
     *
     * @param n 名前
     * @return 登録されていれば true
     * @~english
     * @brief Whether the objective function is registered
     *
     * @param n name
     * @return true if registered
     * @~
     */
    bool has_objective(const std::string& n);
    /*!
     * @~japanese
     * @brief 目的関数を呼び出す
     *
     * @param n 名前
     * @param item 置く RTC
     * @param robot 置くロボット
     * @param state 配置の途中の状態
     * @param args objective_args
     * @return 目的関数の値
     * @~english
     * @brief Calls an objective function
     *
     * @param n name
     * @param item RTC to place
     * @param robot robot to place on
     * @param state state of the placement
     * @param args objective_args
     * @return value of the objective function
     * @~
     */
    double objective(const std::string& n, const PlacementItem& item, const PlacementRobot& robot,
                     const ObjectiveState& state, const json& args);
    /*!
     * @~japanese
     * @brief 条件を呼び出す
     *
     * @param n 名前
     * @param args 引数
     * @param now 現在の時刻
     * @return 条件を満たせば true
     * @~english
     * @brief Calls a condition
     *
     * @param n name
     * @param args arguments
     * @param now current time
     * @return true if the condition holds
     * @~
     */
    bool condition(const std::string& n, const json& args, double now);
    /*!
     * @~japanese
     * @brief 処理を呼び出す
     *
     * @param n 名前
     * @param args 引数
     * @return 成功すれば true
     * @~english
     * @brief Calls an action
     *
     * @param n name
     * @param args arguments
     * @return true if succeeded
     * @~
     */
    bool action(const std::string& n, const json& args);
    /*!
     * @~japanese
     * @brief 配置のロジックを呼び出す
     *
     * @param n 名前
     * @param items 配置する RTC
     * @param robots ロボット
     * @param edges 接続
     * @param primary 主ロボットの名前
     * @param args placement_args
     * @return RTC 名 -> ロボット名
     * @~english
     * @brief Calls a placement logic
     *
     * @param n name
     * @param items RTCs to place
     * @param robots robots
     * @param edges connections
     * @param primary name of the primary robot
     * @param args placement_args
     * @return RTC name -> robot name
     * @~
     */
    std::map<std::string, std::string> placement(const std::string& n, const std::vector<PlacementItem>& items,
                                                 const std::vector<PlacementRobot>& robots,
                                                 const std::vector<PlacementEdge>& edges,
                                                 const std::string& primary, const json& args);
    /*!
     * @~japanese
     * @brief イベントを購読者に配る (専用スレッドで配る)
     *
     * @param e イベント
     * @~english
     * @brief Delivers an event to the subscribers (on a dedicated thread)
     *
     * @param e event
     * @~
     */
    void publish(const Event& e);

    /*!
     * @~japanese
     * @brief 登録された関数 (Context から使う。内部用)
     * @~english
     * @brief Registered functions (used from Context; internal)
     * @~
     */
    struct Registry
    {
      std::map<std::string, std::pair<ConditionFn, std::string>> conditions;
      std::map<std::string, std::pair<ActionFn, std::string>> actions;
      std::map<std::string, std::pair<PlacementFn, std::string>> placements;
      std::map<std::string, std::pair<ObjectiveFn, std::string>> objectives;
      std::vector<std::tuple<std::string, EventFn, std::string>> subscribers;
    };
    /*!
     * @~japanese
     * @brief プラグインのファイルを探す (なければ PluginError)
     *
     * @param module モジュール名
     * @return ファイルのパス
     * @~english
     * @brief Finds the file of a plugin (PluginError if not found)
     *
     * @param module module name
     * @return file path
     * @~
     */
    std::string find(const std::string& module) const;

  private:
    /*!
     * @~japanese
     * @brief プラグインに渡す PluginContext の実装
     * @~english
     * @brief Implementation of PluginContext given to plugins
     * @~
     */
    class Context;
    /*!
     * @~japanese
     * @brief プラグインに渡すコンテキストを作る
     *
     * @param plugin プラグインの名前
     * @return コンテキスト
     * @~english
     * @brief Makes the context given to a plugin
     *
     * @param plugin name of the plugin
     * @return context
     * @~
     */
    std::unique_ptr<PluginContext> context(const std::string& plugin);
    /*!
     * @~japanese
     * @brief プラグインをすべて閉じる
     * @~english
     * @brief Closes all plugins
     * @~
     */
    void unload();
    /*!
     * @~japanese
     * @brief イベントを配るスレッドの本体
     * @~english
     * @brief Body of the thread that delivers events
     * @~
     */
    void dispatch_loop();

    PluginHost& host_;
    std::vector<std::string> paths_;
    std::recursive_mutex mutex_;
    Registry registry_;
    std::vector<void*> handles_;
    std::vector<std::string> loaded_;
    std::string signature_;
    std::deque<Event> events_;
    std::mutex ev_mutex_;
    std::condition_variable ev_cv_;
    bool stop_{false};
    std::thread ev_thread_;
  };
} // namespace snmpmgr

#endif
