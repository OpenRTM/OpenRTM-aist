// -*- C++ -*-
/*!
 * @file PluginManager.cpp
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

#include "PluginManager.hpp"
#include "SNMPManagerUtil.hpp"

#include "OSPlatform.hpp"

#include <filesystem>

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief プラグインに渡す PluginContext の実装 (登録は PluginManager の Registry に入れ、機能は PluginHost に回す)
   * @~english
   * @brief Implementation of PluginContext given to plugins (registrations go to the Registry of PluginManager; functions are forwarded to PluginHost)
   * @~
   */
  class PluginManager::Context : public PluginContext
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param pm PluginManager
     * @param plugin プラグインの名前
     * @~english
     * @brief Constructor
     *
     * @param pm PluginManager
     * @param plugin plugin name
     * @~
     */
    Context(PluginManager& pm, std::string plugin) : pm_(pm), plugin_(std::move(plugin)) {}

    void register_condition(const std::string& n, ConditionFn fn) override { put(pm_.registry_.conditions, "condition", n, std::move(fn)); }
    void register_action(const std::string& n, ActionFn fn) override { put(pm_.registry_.actions, "action", n, std::move(fn)); }
    void register_placement(const std::string& n, PlacementFn fn) override { put(pm_.registry_.placements, "placement", n, std::move(fn)); }
    void register_objective(const std::string& n, ObjectiveFn fn) override { put(pm_.registry_.objectives, "objective", n, std::move(fn)); }
    void subscribe(const std::string& kind, EventFn fn) override
    {
      std::lock_guard<std::recursive_mutex> g(pm_.mutex_);
      pm_.registry_.subscribers.emplace_back(kind, std::move(fn), plugin_);
    }

    std::string name() const override { return pm_.host_.host_name(); }
    std::string plugin() const override { return plugin_; }
    void log(const std::string& m) override { pm_.host_.host_log("[plugin " + plugin_ + "] " + m); }
    std::vector<RobotInfo> robots() override { return pm_.host_.host_robots(); }
    void* manager_object(const std::string& robot) override { return pm_.host_.host_manager_object(robot); }
    bool has_component(const std::string& i, const std::string& r) override { return pm_.host_.host_has_component(i, r); }
    void apply_plan(const json& plan, const std::string& label) override { pm_.host_.host_apply_plan(plan, label); }
    std::string conf(const std::string& k, const std::string& d) override { return pm_.host_.host_conf(k, d); }
    json migration_config() override { return pm_.host_.host_migration_config(); }
    void event(const std::string& r, const std::string& k, const std::string& d) override { pm_.host_.host_event(r, k, d); }

  private:
    /*!
     * @~japanese
     * @brief 関数を表に登録する (名前が登録済みなら PluginError)
     *
     * @tparam Fn 関数の型
     * @param table 登録先の表
     * @param kind 種別 (condition など)
     * @param n 名前
     * @param fn 関数
     * @~english
     * @brief Registers a function to a table (PluginError if the name is already registered)
     *
     * @tparam Fn function type
     * @param table table
     * @param kind kind (condition, etc.)
     * @param n name
     * @param fn function
     * @~
     */
    template <class Fn>
    void put(std::map<std::string, std::pair<Fn, std::string>>& table, const char* kind, const std::string& n, Fn fn)
    {
      std::lock_guard<std::recursive_mutex> g(pm_.mutex_);
      if (!fn) { throw PluginError(plugin_ + ": " + kind + " " + n + " is empty"); }
      if (table.count(n)) { throw PluginError(plugin_ + ": " + kind + " '" + n + "' is already registered by " + table[n].second); }
      table[n] = {std::move(fn), plugin_};
    }
    PluginManager& pm_;
    std::string plugin_;
  };

  PluginManager::PluginManager(PluginHost& host, std::vector<std::string> paths) : host_(host), paths_(std::move(paths))
  {
    ev_thread_ = std::thread([this] { dispatch_loop(); });
  }

  PluginManager::~PluginManager()
  {
    {
      std::lock_guard<std::mutex> g(ev_mutex_);
      stop_ = true;
    }
    ev_cv_.notify_one();
    if (ev_thread_.joinable()) { ev_thread_.join(); }
    unload();
  }

  std::unique_ptr<PluginContext> PluginManager::context(const std::string& plugin)
  {
    return std::make_unique<Context>(*this, plugin);
  }

  std::string PluginManager::find(const std::string& module) const
  {
    namespace fs = std::filesystem;
    if (fs::path(module).is_absolute() && fs::exists(module)) { return module; }
    for (auto& d : paths_)
      {
        const std::string ext = platform::plugin_suffix();
        for (auto& name : {"lib" + module + ext, module + ext, module})
          {
            fs::path p = fs::path(d) / name;
            if (fs::is_regular_file(p)) { return p.string(); }
          }
      }
    std::string all;
    for (auto& d : paths_) { all += (all.empty() ? "" : ", ") + d; }
    throw PluginError("plugin module " + module + " not found in [" + all + "]");
  }

  void PluginManager::unload()
  {
    std::lock_guard<std::recursive_mutex> g(mutex_);
    registry_ = Registry{};       // 関数オブジェクトを先に捨ててから dlclose する
    for (auto* h : handles_) { platform::dl_close(h); }
    handles_.clear();
    loaded_.clear();
  }

  bool PluginManager::load_all(const json& specs)
  {
    std::string sig = specs.is_null() ? "[]" : specs.dump();
    std::lock_guard<std::recursive_mutex> g(mutex_);
    if (sig == signature_) { return false; }
    unload();
    signature_ = sig;
    if (specs.is_null()) { return true; }
    for (auto spec : specs)
      {
        if (spec.is_string()) { spec = json{{"module", spec}}; }
        std::string module = spec.value("module", "");
        if (module.empty()) { signature_.clear(); throw PluginError("plugin entry needs module"); }
        std::string path = find(module);
        std::string error;
        void* h = platform::dl_open(path, error);
        if (!h) { signature_.clear(); throw PluginError("cannot load " + path + ": " + error); }
        handles_.push_back(h);
        auto ver = reinterpret_cast<int (*)()>(platform::dl_symbol(h, "snmpmgr_plugin_api_version"));
        auto setup = reinterpret_cast<void (*)(PluginContext*, const char*)>(platform::dl_symbol(h, "snmpmgr_plugin_setup"));
        if (!setup) { signature_.clear(); throw PluginError(module + ": snmpmgr_plugin_setup is not defined"); }
        if (ver && ver() != SNMPMGR_PLUGIN_API_VERSION)
          {
            signature_.clear();
            throw PluginError(module + ": plugin API " + std::to_string(ver()) + " is not supported");
          }
        std::string plugin = spec.value("name", std::filesystem::path(path).stem().string());
        if (starts_with(plugin, "lib")) { plugin = plugin.substr(3); }
        Context ctx(*this, plugin);
        std::string config = spec.contains("config") ? spec["config"].dump() : "{}";
        setup(&ctx, config.c_str());
        loaded_.push_back(plugin);
      }
    return true;
  }

  std::string PluginManager::describe()
  {
    std::lock_guard<std::recursive_mutex> g(mutex_);
    auto names = [](auto& table) {
      std::string s;
      for (auto& [n, v] : table) { s += (s.empty() ? "" : ", ") + n; }
      return s.empty() ? std::string("-") : s;
    };
    std::string plugins;
    for (auto& p : loaded_) { plugins += (plugins.empty() ? "" : ", ") + p; }
    return "plugins: " + (plugins.empty() ? std::string("-") : plugins) + "; conditions: " + names(registry_.conditions) +
           "; actions: " + names(registry_.actions) + "; placements: " + names(registry_.placements) +
           "; objectives: " + names(registry_.objectives) +
           "; subscribers: " + std::to_string(registry_.subscribers.size());
  }

  bool PluginManager::has_condition(const std::string& n) { std::lock_guard<std::recursive_mutex> g(mutex_); return registry_.conditions.count(n) > 0; }
  bool PluginManager::has_action(const std::string& n) { std::lock_guard<std::recursive_mutex> g(mutex_); return registry_.actions.count(n) > 0; }
  bool PluginManager::has_placement(const std::string& n) { std::lock_guard<std::recursive_mutex> g(mutex_); return registry_.placements.count(n) > 0; }
  bool PluginManager::has_objective(const std::string& n) { std::lock_guard<std::recursive_mutex> g(mutex_); return registry_.objectives.count(n) > 0; }

  double PluginManager::objective(const std::string& n, const PlacementItem& item, const PlacementRobot& robot,
                                  const ObjectiveState& state, const json& args)
  {
    std::pair<ObjectiveFn, std::string> e;
    { std::lock_guard<std::recursive_mutex> g(mutex_); e = registry_.objectives.at(n); }
    Context ctx(*this, e.second);
    return e.first(ctx, item, robot, state, args);
  }

  bool PluginManager::condition(const std::string& n, const json& args, double now)
  {
    std::pair<ConditionFn, std::string> e;
    { std::lock_guard<std::recursive_mutex> g(mutex_); e = registry_.conditions.at(n); }
    Context ctx(*this, e.second);
    return e.first(ctx, args, now);
  }

  bool PluginManager::action(const std::string& n, const json& args)
  {
    std::pair<ActionFn, std::string> e;
    { std::lock_guard<std::recursive_mutex> g(mutex_); e = registry_.actions.at(n); }
    Context ctx(*this, e.second);
    return e.first(ctx, args);
  }

  std::map<std::string, std::string> PluginManager::placement(const std::string& n, const std::vector<PlacementItem>& items,
                                                              const std::vector<PlacementRobot>& robots,
                                                              const std::vector<PlacementEdge>& edges,
                                                              const std::string& primary, const json& args)
  {
    std::pair<PlacementFn, std::string> e;
    { std::lock_guard<std::recursive_mutex> g(mutex_); e = registry_.placements.at(n); }
    Context ctx(*this, e.second);
    return e.first(ctx, items, robots, edges, primary, args);
  }

  void PluginManager::publish(const Event& ev)
  {
    {
      std::lock_guard<std::mutex> g(ev_mutex_);
      events_.push_back(ev);
    }
    ev_cv_.notify_one();
  }

  void PluginManager::dispatch_loop()
  {
    for (;;)
      {
        Event ev;
        {
          std::unique_lock<std::mutex> lk(ev_mutex_);
          ev_cv_.wait(lk, [this] { return stop_ || !events_.empty(); });
          if (stop_ && events_.empty()) { return; }
          ev = events_.front();
          events_.pop_front();
        }
        std::lock_guard<std::recursive_mutex> g(mutex_);   // 配る間はプラグインを読み込み直させない
        for (auto& [kind, fn, plugin] : registry_.subscribers)
          {
            if (kind != "*" && kind != ev.kind) { continue; }
            try
              {
                Context ctx(*this, plugin);
                fn(ctx, ev);
              }
            catch (std::exception& ex) { host_.host_log("[plugin " + plugin + "] event handler failed: " + ex.what()); }
          }
      }
  }
} // namespace snmpmgr
