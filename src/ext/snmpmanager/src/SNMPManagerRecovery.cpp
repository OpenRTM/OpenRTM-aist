// -*- C++ -*-
/*!
 * @file SNMPManagerRecovery.cpp
 * @brief Automatic recovery (switching conditions and staged recovery). Same as the Python version SNMPManager.py
 *
 * @~japanese
 * 自動復旧 (切り替えの条件と段階的な復旧)。Python 版 SNMPManager.py と同じ
 *
 * 実行ロボット (recovery.executor。"auto" なら選出) が interval 秒ごとに、起動時の構成の RTC を見て、切り替えの条件が duration_seconds 続いたら復旧する。slave の異常終了を知ったら待たずに始める。
 * @~english
 * The executor robot (recovery.executor; elected if "auto") checks the RTCs of the startup configuration every interval seconds and recovers them when a switching condition lasts for duration_seconds. When a slave crash is known, it starts without waiting.
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

#include "SNMPManager.hpp"
#include "CustomMetrics.hpp"
#include "SNMPManagerUtil.hpp"

#include <algorithm>
#include <cmath>

namespace snmpmgr
{
  namespace
  {
    struct Threshold { const char* key; const char* cause; };
    // 切り替えの条件のしきい値 (recovery.conditions) -> 原因
    const std::vector<Threshold> kThresholds = {
        {"memory_free_min_mb", "memory"}, {"memory_max_pct", "memory"}, {"cpu_max_pct", "cpu"},
        {"battery_min_pct", "battery"}, {"rssi_min_dbm", "communication"}, {"latency_max_ms", "communication"},
        {"jitter_max_ms", "communication"}, {"throughput_min_mbps", "communication"}, {"packet_loss_max", "communication"}};
    // 同じロボットの中で直すことを試す原因 (RTC の ERROR・消失と、メモリ・CPU の不足)。ほかは別のロボットへ移す
    const std::vector<std::string> kLocalCauses = {"error", "missing", "memory", "cpu"};

    /*!
     * @~japanese
     * @brief v に s があるか
     *
     * @param v 文字列のリスト
     * @param s 文字列
     * @return あれば true
     * @~english
     * @brief Whether v contains s
     *
     * @param v list of strings
     * @param s string
     * @return true if contained
     * @~
     */
    bool contains(const std::vector<std::string>& v, const std::string& s) { return std::find(v.begin(), v.end(), s) != v.end(); }

    /*!
     * @~japanese
     * @brief 文字列をつなぐ
     *
     * @param v 文字列のリスト
     * @param sep 区切り
     * @return つないだ文字列
     * @~english
     * @brief Joins strings
     *
     * @param v list of strings
     * @param sep separator
     * @return joined string
     * @~
     */
    std::string join(const std::vector<std::string>& v, const std::string& sep = ", ")
    {
      std::string s;
      for (auto& x : v) { s += (s.empty() ? "" : sep) + x; }
      return s;
    }

    /*!
     * @~japanese
     * @brief 数値を Python 版と同じ形の文字列にする
     *
     * @param v 値
     * @return 文字列
     * @~english
     * @brief Formats a number in the same way as the Python version
     *
     * @param v value
     * @return string
     * @~
     */
    std::string num_str(double v)
    {
      char buf[64];
      std::snprintf(buf, sizeof(buf), "%g", v);
      return buf;
    }
  } // namespace

  const json* SNMPManager::recovery_cfg(const json& c)
  {
    if (!c.is_object() || !c.contains("recovery") || !c["recovery"].is_object() || !c["recovery"].value("enabled", true)) { return nullptr; }
    return &c["recovery"];
  }

  void SNMPManager::wake_recovery()
  {
    std::lock_guard<std::mutex> g(wake_mutex_);
    wake_ = true;
    wake_cv_.notify_all();
  }

  void SNMPManager::recovery_loop()
  {
    while (running_)
      {
        double interval = 2.0;
        try
          {
            json cfg = load_migration_config();
            if (const json* rc = recovery_cfg(cfg))
              {
                interval = rc->value("interval", 2.0);
                recovery_tick(cfg, *rc);
              }
          }
        catch (std::exception& e) { log(std::string("recovery check failed: ") + e.what()); }
        catch (CORBA::SystemException& e) { log(std::string("recovery check failed: CORBA ") + e._name()); }
        // slave の異常終了を知ったら、待たずに次の確認をする
        std::unique_lock<std::mutex> lk(wake_mutex_);
        wake_cv_.wait_for(lk, std::chrono::duration<double>(std::max(0.5, interval)), [this] { return wake_ || !running_; });
        wake_ = false;
      }
  }

  std::vector<std::pair<std::string, std::string>> SNMPManager::robot_problems(const std::shared_ptr<Robot>& robot, const json& conds)
  {
    // ロボットの切り替えの条件 のうち、今満たしているもの: [(原因, 説明)]
    std::vector<std::pair<std::string, std::string>> out;
    if (!robot) { return out; }
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    if (!robot->is_local && robot->status == "dead")
      {
        if (conds.value("down", true)) { out.push_back({"down", "no response"}); }
        return out;
      }
    struct Check { std::string key; std::optional<double> value; bool lower; };
    std::vector<Check> checks;
    if (robot->cg_valid && robot->cg_mem_limit_mb > 0)
      {
        checks.push_back({"memory_free_min_mb", robot->cg_mem_limit_mb - robot->cg_mem_usage_mb, true});
        checks.push_back({"memory_max_pct", robot->cg_mem_usage_mb / robot->cg_mem_limit_mb * 100.0, false});
      }
    else { checks.push_back({"memory_max_pct", robot->memory_usage, false}); }
    if (robot->cg_valid && robot->cg_cpu_usage_pct && robot->cg_cpu_limit_pct > 0)
      {
        checks.push_back({"cpu_max_pct", *robot->cg_cpu_usage_pct / robot->cg_cpu_limit_pct * 100.0, false});
      }
    else { checks.push_back({"cpu_max_pct", robot->cpu_usage, false}); }
    checks.push_back({"battery_min_pct", robot->battery_pct, true});
    if (!robot->is_local)
      {
        checks.push_back({"rssi_min_dbm", robot->wifi_dbm, true});
        checks.push_back({"latency_max_ms", robot->latency_ms, false});
        checks.push_back({"jitter_max_ms", robot->jitter_ms, false});
        checks.push_back({"packet_loss_max", robot->packet_loss, false});
      }
    std::optional<double> tp;
    if (robot->network_in && robot->network_out) { tp = *robot->network_in + *robot->network_out; }
    checks.push_back({"throughput_min_mbps", tp, true});
    for (auto& c : checks)
      {
        if (!c.value || !conds.contains(c.key) || !conds[c.key].is_number()) { continue; }
        double limit = conds[c.key].get<double>();
        if (c.lower ? *c.value < limit : *c.value > limit)
          {
            auto t = std::find_if(kThresholds.begin(), kThresholds.end(), [&](auto& x) { return c.key == x.key; });
            out.push_back({t->cause, c.key + ": " + fmt(*c.value) + (c.lower ? " < " : " > ") + num_str(limit)});
          }
      }
    // 追加の監視の値 ("<名前>_max" / "<名前>_min")。ロボットの状態なので、別のロボットへ移す原因
    for (auto& v : metrics::violations(robot->metrics, conds, custom_names())) { out.push_back({"metric", v.key + ": " + metrics::describe(v)}); }
    return out;
  }

  SNMPManager::Location SNMPManager::recovery_locate(const std::string& name, const json& entry)
  {
    // 計画の RTC が今どこで動いているか。状態は ACTIVE / INACTIVE / ERROR、ロボットが止まっていれば "down"、
    // ロボットは生きているのに RTC がなければ "missing"、分からなければ ""
    std::optional<std::pair<std::string, std::string>> where;
    {
      std::lock_guard<std::mutex> g(recovery_mutex_);
      if (auto it = recovery_where_.find(name); it != recovery_where_.end()) { where = it->second; }
    }
    std::vector<std::string> cands;
    if (where) { cands.push_back(where->second); }
    std::string planned = instance_name(entry);
    if (!contains(cands, planned)) { cands.push_back(planned); }
    std::vector<std::shared_ptr<Robot>> robots;
    {
      std::lock_guard<std::recursive_mutex> g(robots_mutex_);
      if (self_) { robots.push_back(self_); }
      for (auto& r : robots_) { robots.push_back(r); }
    }
    // 覚えている場所を先に、なければ全ロボットから同じインスタンス名を探す
    std::stable_sort(robots.begin(), robots.end(), [&](auto& a, auto& b) {
      return where && a->name == where->first && b->name != where->first;
    });
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    for (auto& r : robots)
      {
        if (!r->inventory) { continue; }
        for (auto& c : *r->inventory)
          {
            if (!contains(cands, c.name)) { continue; }
            {
              std::lock_guard<std::mutex> lg(recovery_mutex_);
              recovery_where_[name] = {r->name, c.name};
            }
            return {r->name, c.name, c.state};
          }
      }
    if (where)
      {
        for (auto& r : robots)
          {
            if (r->name != where->first) { continue; }
            if (!r->is_local && r->status == "dead") { return {where->first, where->second, "down"}; }
            // 計画を適用する前に取った一覧では、生成したばかりの RTC がまだ載っていない (消えたとは言えない)
            if (r->inventory && r->inventory_time >= last_plan_applied_.load()) { return {where->first, where->second, "missing"}; }
          }
        return {where->first, where->second, ""};
      }
    return {"", "", ""};
  }

  std::vector<SNMPManager::Problem> SNMPManager::recovery_problems(const json& cfg, const json& rc)
  {
    // 切り替えの条件を満たす RTC
    std::map<std::string, json> entries;
    std::vector<std::string> order;
    for (auto& c : cfg["startup"].value("components", json::array())) { entries[c["name"]] = c; order.push_back(c["name"]); }
    std::vector<std::string> monitored;
    for (auto& n : rc.value("components", json::array())) { monitored.push_back(n); }
    if (monitored.empty()) { monitored = order; }
    json conds = rc.value("conditions", json::object());
    std::map<std::string, Location> located;
    for (auto& n : monitored) { located[n] = recovery_locate(n, entries[n]); }
    std::vector<Problem> out;
    std::vector<std::string> robot_order;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> robot_causes;
    for (auto& n : monitored)
      {
        auto& l = located[n];
        if (l.state == "ERROR" && conds.value("error", true)) { out.push_back({n, "error", l.robot, l.instance + " is in ERROR state"}); }
        else if (l.state == "missing" && conds.value("missing", true)) { out.push_back({n, "missing", l.robot, l.instance + " disappeared from " + l.robot}); }
        else if (l.state == "down" && conds.value("down", true)) { out.push_back({n, "down", l.robot, l.robot + " is down"}); }
        if (!l.robot.empty() && l.state != "down" && !l.state.empty() && !robot_causes.count(l.robot))
          {
            robot_causes[l.robot] = robot_problems(find_robot(l.robot), conds);
            robot_order.push_back(l.robot);
          }
      }
    auto db = profiles();
    for (auto& robot : robot_order)
      {
        std::vector<std::string> on_robot;
        for (auto& n : monitored)
          {
            auto& l = located[n];
            if (l.robot == robot && l.state != "down" && l.state != "missing") { on_robot.push_back(n); }
          }
        for (auto& [cause, detail] : robot_causes[robot])
          {
            std::vector<std::string> targets = on_robot;
            if ((cause == "memory" || cause == "cpu") && rc.value("target", "largest") == "largest" && !on_robot.empty())
              {
                // 資源の不足: 見込みが最も大きい RTC から 1 つずつ移す
                auto inv = robot_inventory(robot);
                auto size = [&](const std::string& n) {
                  if (!inv) { return 0.0; }
                  for (auto& c : *inv)
                    {
                      if (c.name != located[n].instance) { continue; }
                      auto p = db->get(c.type, c.language.empty() ? "Python" : c.language);
                      return p ? (cause == "memory" ? p->memory_mb : p->cpu) : 0.0;
                    }
                  return 0.0;
                };
                std::string best = on_robot.front();
                for (auto& n : on_robot) { if (size(n) > size(best)) { best = n; } }
                targets = {best};
              }
            for (auto& n : targets)
              {
                if (std::none_of(out.begin(), out.end(), [&](auto& o) { return o.name == n; })) { out.push_back({n, cause, robot, detail}); }
              }
          }
      }
    return out;
  }

  void SNMPManager::prefetch_loadable(const json& cfg)
  {
    // 起動の構成が終わったら、各マスターマネージャの get_loadable_modules() を先に取っておく (復旧・mode: auto のマイグレーションで使う)。
    // C++ のマスターは Python の RTC のプロファイルを rtcprof2_python (別プロセス) で取るので数秒かかり、
    // 起動時に自動配置をしない構成だと、最初の復旧 (別のロボットへの切り替え) や移動がその分遅れるため
    ManagerSet ms = manager_refs(cfg, true);
    std::vector<std::string> names;
    for (auto& [n, v] : cfg["managers"].items())
      {
        if (ms.has(n)) { names.push_back(n); }
      }
    prefetching_ = true;
    std::thread([this, ms, names] {
      double t0 = monotonic();
      std::vector<std::string> done;
      for (auto& n : names)
        {
          {
            std::lock_guard<std::mutex> g(module_mutex_);
            if (loadable_checked_.count(n)) { continue; }
          }
          std::vector<std::map<std::string, std::string>> mods;
          try { mods = rtcops::loadable_modules(ms.at(n)); }
          catch (...) { continue; }       // 取れなかったロボットは、自動配置のときに取り直す
          std::lock_guard<std::mutex> g(module_mutex_);
          auto& cache = loadable_modules_[n];
          for (auto& m : mods)
            {
              std::string impl = !m["implementation_id"].empty() ? m["implementation_id"] : m["type_name"];
              if (!impl.empty() && !cache.count(impl)) { cache[impl] = m; }
            }
          loadable_checked_.insert(n);
          done.push_back(n);
        }
      if (!done.empty()) { log("prefetched loadable modules of " + join(done, ", ") + " (" + fmt(monotonic() - t0) + "s)"); }
      // get_loadable_modules() は、相手のマスターで rtcprof2_python を起動して CPU を使う (CPU の制限が小さい
      // コンテナでは数秒間 100% 近くになる)。その間の CPU のサンプルで、自動復旧の切り替えの条件 (cpu_max_pct) や
      // 配置の空きを判断しないように、捨てて取り直す (起動時の自動配置と同じ扱い)
      if (!done.empty())
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          std::vector<std::shared_ptr<Robot>> all = robots_;
          if (self_) { all.push_back(self_); }
          for (auto& r : all)
            {
              r->cg_cpu_samples.clear();
              r->cg_cpu_usage_pct.reset();
              r->cg_last_ts = -1;
              r->cg_ignore_before_ns = static_cast<long long>(wallclock() * 1e9);
            }
          cgroup_reset_at_ = monotonic();
        }
      prefetching_ = false;
    }).detach();
  }

  void SNMPManager::recovery_tick(const json& cfg, const json& rc)
  {
    if (!startup_completed_ || recovering_) { return; }
    // 実行ロボットでなくても取っておく (実行ロボットが止まって引き継いだとき、すぐ移せるように)
    if (!loadable_prefetched_.exchange(true)) { prefetch_loadable(cfg); }
    // 先に取っている間は、CPU の値があてにならないので判断しない (起動の直後に 1 回、十数秒)
    if (prefetching_) { return; }
    if (!is_executor(cfg, {&rc}, std::nullopt, "recovery"))
      {
        std::lock_guard<std::mutex> g(recovery_mutex_);
        recovery_first_seen_.clear();
        return;
      }
    {
      std::lock_guard<std::mutex> g(migration_mutex_);
      if (migrating_) { return; }
    }
    if (migration_in_progress_) { return; }
    auto problems = recovery_problems(cfg, rc);
    double now = monotonic();
    double duration = rc.value("conditions", json::object()).value("duration_seconds", 3.0);
    std::vector<Problem> due;
    {
      std::lock_guard<std::mutex> g(recovery_mutex_);
      std::set<std::pair<std::string, std::string>> current;
      for (auto& p : problems) { current.insert({p.name, p.cause}); }
      for (auto it = recovery_first_seen_.begin(); it != recovery_first_seen_.end();)
        {
          it = current.count(it->first) ? std::next(it) : recovery_first_seen_.erase(it);
        }
    }
    for (auto& p : problems)
      {
        double first;
        bool is_new = false;
        {
          std::lock_guard<std::mutex> g(recovery_mutex_);
          auto key = std::make_pair(p.name, p.cause);
          if (!recovery_first_seen_.count(key)) { recovery_first_seen_[key] = now; is_new = true; }
          first = recovery_first_seen_[key];
        }
        bool crashed = p.cause == "missing" && crash_hint(p.name, now);
        if (p.cause == "error" && crash_hint(p.name, now, true))
          {
            crashed = true;
            bool first_log = false;
            {
              std::lock_guard<std::mutex> g(recovery_mutex_);
              first_log = !is_new && notified_logged_.insert(p.name).second;
            }
            if (first_log) { log("recovery: " + p.name + " on " + p.robot + ": ERROR notified by the component observer, recovering now"); }
          }
        if (is_new)
          {
            std::string reason = p.cause == "missing" ? "its slave manager exited abnormally" : "ERROR notified by the component observer";
            log("recovery: " + p.name + " on " + p.robot + ": " + p.cause + " (" + p.detail + "); " +
                (crashed ? reason + ", recovering now" : "waiting " + num_str(duration) + "s"));
          }
        if (now - first < duration && !crashed) { continue; }
        {
          std::lock_guard<std::mutex> g(recovery_mutex_);
          auto a = recovery_cooldown_.find(p.name), b = recovery_cooldown_.find(p.robot + "|" + p.cause);
          if ((a != recovery_cooldown_.end() && a->second > now) || (b != recovery_cooldown_.end() && b->second > now)) { continue; }
        }
        due.push_back(p);
      }
    if (due.empty()) { return; }
    recovering_ = true;
    migration_in_progress_ = true;   // 条件によるマイグレーションと重ねない
    try { recover(cfg, rc, due); }
    catch (...) {}
    recovering_ = false;
    migration_in_progress_ = false;
    std::lock_guard<std::mutex> g(recovery_mutex_);
    for (auto& p : due)
      {
        recovery_first_seen_.erase({p.name, p.cause});
        crashed_.erase(p.name);
        error_notified_.erase(p.name);
        notified_logged_.erase(p.name);
        if (auto w = recovery_where_.find(p.name); w != recovery_where_.end())
          {
            crashed_.erase(w->second.second);
            error_notified_.erase(w->second.second);
          }
      }
  }

  void SNMPManager::recover(const json& cfg, const json& rc, const std::vector<Problem>& due)
  {
    // 段階的な復旧: 同じロボットで直せる原因は段階的に試し、だめなら別のロボットへ
    double started = monotonic();
    double cooldown = rc.value("cooldown_seconds", 20.0);
    json stages = {{"local", {"reset", "standby_same", "restart", "other"}}, {"remote", {"other"}}};
    json stage_cfg = rc.value("stages", json::object());
    for (auto& [k, v] : stage_cfg.items()) { stages[k] = v; }
    std::vector<std::string> d;
    for (auto& p : due) { d.push_back(p.name + "@" + p.robot + ": " + p.cause); }
    event(name_, "recovery_started", join(d));
    std::vector<Problem> switching;
    std::vector<std::string> results;
    for (auto& p : due)
      {
        std::string kind = contains(kLocalCauses, p.cause) ? "local" : "remote";
        bool done = false, to_other = false;
        for (auto& st : stages[kind])
          {
            std::string stage = st;
            if (stage == "other") { switching.push_back(p); to_other = true; break; }
            try { done = recovery_stage_local(cfg, rc, stage, p.name, p.cause); }
            catch (std::exception& e) { log("recovery: " + p.name + ": stage " + stage + " failed: " + e.what()); done = false; }
            catch (CORBA::SystemException& e) { log("recovery: " + p.name + ": stage " + stage + " failed: CORBA " + e._name()); done = false; }
            if (done) { results.push_back(p.name + ": " + stage); break; }
          }
        if (!done && !to_other) { results.push_back(p.name + ": not recovered"); }
      }
    if (!switching.empty())
      {
        wait_cgroup_samples();
        try
          {
            auto where = recovery_switch_other(cfg, rc, switching);
            for (auto& p : switching) { results.push_back(p.name + ": other -> " + where[p.name]); }
          }
        catch (std::exception& e)
          {
            log(std::string("recovery: switching to other robots failed: ") + e.what());
            for (auto& p : switching) { results.push_back(p.name + ": failed (" + e.what() + ")"); }
          }
        catch (CORBA::SystemException& e)
          {
            log(std::string("recovery: switching to other robots failed: CORBA ") + e._name());
            for (auto& p : switching) { results.push_back(p.name + ": failed (CORBA " + e._name() + ")"); }
          }
      }
    double now = monotonic();
    {
      std::lock_guard<std::mutex> g(recovery_mutex_);
      for (auto& p : due)
        {
          recovery_cooldown_[p.name] = now + cooldown;
          recovery_cooldown_[p.robot + "|" + p.cause] = now + cooldown;
        }
    }
    double elapsed = monotonic() - started;
    bool ok = std::none_of(results.begin(), results.end(), [](auto& r) {
      return r.find("failed") != std::string::npos || r.find("not recovered") != std::string::npos;
    });
    event(name_, ok ? "recovery_completed" : "recovery_failed", join(results, "; ") + " (" + fmt(elapsed, 2) + "s)");
    logfile(fmt(wallclock(), 6) + "\tRecoveryTime:" + std::to_string(elapsed) + "\t" + join(results, ";") + "\t");
  }

  void SNMPManager::wait_cgroup_samples()
  {
    size_t min_samples = static_cast<size_t>(prop_d("snmp.placement.min_cpu_samples", 3));
    bool waited = false;
    double t0 = monotonic();
    while (running_)
      {
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          if (cgroup_reset_at_ <= 0) { break; }
          std::vector<std::string> missing;
          std::vector<std::shared_ptr<Robot>> all = robots_;
          if (self_) { all.push_back(self_); }
          for (auto& r : all)
            {
              if (r->cg_valid && r->cg_cpu_samples.size() < min_samples) { missing.push_back(r->name); }
            }
          bool expired = monotonic() - cgroup_reset_at_ >= cgroup_wait_seconds_;
          if (missing.empty() || expired)
            {
              cgroup_reset_at_ = 0;
              if (waited) { log("recovery: CPU samples " + std::string(missing.empty() ? "collected" : "still missing for " + join(missing, ", ")) +
                                " after " + fmt(monotonic() - t0) + "s"); }
              break;
            }
          if (!waited) { log("recovery: waiting for CPU samples of " + join(missing, ", ") + " (discarded after prefetching loadable modules)"); }
          waited = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
  }

  bool SNMPManager::recovery_resolved(const json& cfg, const json& rc, const std::string& name, const std::string& cause)
  {
    // 段階の後、settle_seconds 待ってから、その RTC の切り替えの条件がまだ成り立つかを調べる
    std::this_thread::sleep_for(std::chrono::duration<double>(rc.value("settle_seconds", 4.0)));
    refresh_inventory();
    json entry;
    for (auto& c : cfg["startup"]["components"]) { if (c["name"] == name) { entry = c; } }
    auto l = recovery_locate(name, entry);
    if (l.state.empty() || l.state == "down" || l.state == "missing" || l.state == "ERROR") { return false; }
    if (cause == "memory" || cause == "cpu")
      {
        for (auto& [c, d] : robot_problems(find_robot(l.robot), rc.value("conditions", json::object())))
          {
            if (c == cause) { return false; }
          }
      }
    return true;
  }

  json SNMPManager::recovery_peer_plan(const json& cfg, const std::set<std::string>& names, const json& extra)
  {
    // names の RTC につながる接続と、相手の RTC (動かさない既存の RTC) を含む計画
    std::map<std::string, json> entries;
    for (auto& c : cfg["startup"].value("components", json::array())) { entries[c["name"]] = c; }
    json comps = extra, conns = json::array();
    std::map<std::string, json> peers;
    for (auto& conn : cfg["startup"].value("connections", json::array()))
      {
        std::string ends[2] = {conn["source"]["component"], conn["target"]["component"]};
        if (!names.count(ends[0]) && !names.count(ends[1])) { continue; }
        bool ok = true;
        for (auto& e : ends)
          {
            if (names.count(e) || peers.count(e)) { continue; }
            auto l = recovery_locate(e, entries[e]);
            if (l.state.empty() || l.state == "down" || l.state == "missing")
              {
                ok = false;
                log("recovery: skip connection " + conn["name"].get<std::string>() + ": " + e + " is not running");
                break;
              }
            peers[e] = {{"name", e}, {"existing", l.instance}, {"manager", l.robot}};
          }
        if (ok) { conns.push_back(conn); }
      }
    for (auto& [n, p] : peers) { comps.push_back(p); }
    return {{"components", comps}, {"connections", conns}};
  }

  bool SNMPManager::recovery_stage_local(const json& cfg, const json& rc, const std::string& stage,
                                         const std::string& name, const std::string& cause)
  {
    // 同じロボットでの段階: reset / standby_same / restart。直れば true
    json entry;
    for (auto& c : cfg["startup"]["components"]) { if (c["name"] == name) { entry = c; } }
    auto l = recovery_locate(name, entry);
    if (l.robot.empty() || l.state == "down") { return false; }
    ManagerSet ms = manager_refs(cfg, true);
    RTM::Manager_ptr mgr = ms.at(l.robot);
    RTC::RTObject_var comp;
    if (l.state != "missing") { comp = rtcops::find(mgr, l.instance); }
    std::string module = entry.value("create", "").empty() ? "" : module_name_of(entry["create"]);
    bool activate = entry.value("activate", true);
    if (stage == "reset")
      {
        // ERROR のときだけリセットできる (リセットすると INACTIVE に戻るので、アクティブにし直す)
        if (CORBA::is_nil(comp) || l.state != "ERROR") { return false; }
        log("recovery: " + name + ": reset " + l.instance + " on " + l.robot);
        if (!rtcops::reset(comp)) { return false; }
        if (activate) { rtcops::activate(comp); }
        bool ok = recovery_resolved(cfg, rc, name, cause);
        event(l.robot, "recovery_stage", name + ": reset -> " + (ok ? "ok" : "not resolved"));
        return ok;
      }
    if (module.empty()) { return false; }
    if (stage == "standby_same")
      {
        std::vector<RTC::RTObject_var> used;
        if (!CORBA::is_nil(comp)) { used.push_back(RTC::RTObject::_duplicate(comp)); }
        auto found = find_standby(ms, module, {l.robot}, used, {l.instance});
        if (!found) { return false; }
        log("recovery: " + name + ": switch to standby " + found->second.info.name + " on " + l.robot);
        recovery_terminate(name, l.instance, l.robot, comp, ms);
        json e = entry;
        e["manager"] = l.robot;
        e["standby_instance"] = found->second.info.name;
        e["activate"] = activate;
        apply_plan(cfg, recovery_peer_plan(cfg, {name}, json::array({e})), "Recovery");
        bool ok = recovery_resolved(cfg, rc, name, cause);
        event(l.robot, "recovery_stage", name + ": standby_same " + found->second.info.name + " -> " + (ok ? "ok" : "not resolved"));
        return ok;
      }
    if (stage == "restart")
      {
        log("recovery: " + name + ": restart " + l.instance + " on " + l.robot);
        recovery_terminate(name, l.instance, l.robot, comp, ms);
        json e = entry;
        e["manager"] = l.robot;
        e["standby"] = false;
        e["activate"] = activate;
        json plan = recovery_peer_plan(cfg, {name}, json::array({e}));
        plan["_no_standby"] = true;
        apply_plan(cfg, plan, "Recovery");
        bool ok = recovery_resolved(cfg, rc, name, cause);
        event(l.robot, "recovery_stage", name + ": restart -> " + (ok ? "ok" : "not resolved"));
        return ok;
      }
    return false;
  }

  void SNMPManager::recovery_terminate(const std::string& name, const std::string& inst, const std::string& robot,
                                       RTC::RTObject_ptr comp, const ManagerSet& ms)
  {
    // 故障した RTC を終了させ、消えるまで待つ (同じ名前で作り直すため)
    if (CORBA::is_nil(comp)) { return; }
    try { remove_one(inst, robot, comp); }
    catch (std::exception& e) { log("recovery: terminating " + inst + " on " + robot + " failed: " + e.what()); }
    catch (CORBA::SystemException& e) { log("recovery: terminating " + inst + " on " + robot + " failed: CORBA " + e._name()); }
    if (ms.has(robot))
      {
        double deadline = monotonic() + 5;
        while (monotonic() < deadline)
          {
            try { if (CORBA::is_nil(rtcops::find(ms.at(robot), inst))) { break; } }
            catch (...) { break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
          }
      }
    std::lock_guard<std::mutex> g(recovery_mutex_);
    recovery_where_.erase(name);
  }

  std::map<std::string, std::string> SNMPManager::recovery_switch_other(const json& cfg, const json& rc,
                                                                       const std::vector<Problem>& switching)
  {
    // 別のロボットへの切り替え: 故障した RTC を終了させ、別のロボットの待機モジュールに切り替えるか、別のロボットで起動する。
    // 移す先からは、元のロボットと、切り替えの条件を今満たしているロボットを除く
    std::set<std::string> names;
    std::map<std::string, json> entries;
    for (auto& c : cfg["startup"]["components"]) { entries[c["name"]] = c; }
    json conds = rc.value("conditions", json::object());
    ManagerSet ms = manager_refs(cfg, true);
    std::set<std::string> exclude;
    for (auto& p : switching)
      {
        names.insert(p.name);
        exclude.insert(p.robot);
        auto l = recovery_locate(p.name, entries[p.name]);
        if (l.state != "down" && l.state != "missing" && !l.state.empty() && ms.has(p.robot))
          {
            RTC::RTObject_var comp = rtcops::find(ms.at(p.robot), l.instance);
            recovery_terminate(p.name, l.instance, p.robot, comp, ms);
          }
      }
    for (auto& [rname, v] : cfg["managers"].items())
      {
        if (exclude.count(rname)) { continue; }
        if (!robot_problems(find_robot(rname), conds).empty())
          {
            log("recovery: " + rname + " is excluded (it meets a switching condition)");
            exclude.insert(rname);
          }
      }
    json creates = json::array();
    for (auto& n : names)
      {
        json e = entries[n];
        e.erase("manager");
        e["activate"] = entries[n].value("activate", true);
        creates.push_back(e);
      }
    json plan = recovery_peer_plan(cfg, names, creates);
    plan["mode"] = "auto";
    plan["logic"] = rc.value("logic", cfg["startup"].value("logic", std::string("first_fit")));
    if (rc.contains("link_constraints")) { plan["link_constraints"] = rc["link_constraints"]; }
    refresh_inventory();
    auto manual = auto_plan(cfg, plan, std::vector<std::string>(exclude.begin(), exclude.end()), false, true).first;
    manual["_force_standby"] = true;
    ApplyResult res = apply_plan(cfg, manual, "Recovery");
    std::map<std::string, std::string> where;
    for (auto& n : names)
      {
        where[n] = res.assign.count(n) ? res.assign[n] : "";
        event(where[n].empty() ? "-" : where[n], "recovery_stage",
              n + ": other -> " + where[n] + " (" + (res.instances.count(n) ? res.instances[n] : n) + ")");
      }
    return where;
  }

  void SNMPManager::mark_crashed(const std::string& robot, const std::vector<std::string>& rtcs)
  {
    double now = monotonic();
    {
      std::lock_guard<std::mutex> g(recovery_mutex_);
      for (auto& n : rtcs) { crashed_[n] = {robot, now}; }
    }
    std::thread([this] {
      // RTC がなくなったことを、次のポーリングを待たずに反映する
      try { refresh_inventory(); } catch (...) {}
      wake_recovery();
    }).detach();
  }

  bool SNMPManager::crash_hint(const std::string& name, double now, bool error)
  {
    // 計画の RTC name (かそのインスタンス) の slave が最近 (60 秒以内) 異常終了したか
    // (error なら、コンポーネントオブザーバーで ERROR の通知を受けたか)
    std::lock_guard<std::mutex> g(recovery_mutex_);
    auto& table = error ? error_notified_ : crashed_;
    std::vector<std::string> keys{name};
    if (auto w = recovery_where_.find(name); w != recovery_where_.end()) { keys.push_back(w->second.second); }
    for (auto& k : keys)
      {
        auto it = table.find(k);
        if (it != table.end() && now - it->second.second < 60.0) { return true; }
      }
    return false;
  }

  // ==================================================================== コンポーネントオブザーバー
  void SNMPManager::attach_observers(const std::vector<rtcops::Component>& comps)
  {
    // 自分のロボットの RTC にサーバーオブジェクトを関連付ける。なくなった RTC の分は止める
    std::map<std::string, const rtcops::Component*> current;
    for (auto& c : comps)
      {
        try
          {
            CORBA::String_var ior = orb_->object_to_string(c.ref.in());
            current[std::string(ior.in())] = &c;
          }
        catch (...) {}
      }
    PortableServer::POA_var poa = PortableServer::POA::_duplicate(mgr_.getPOA());
    std::lock_guard<std::mutex> g(observer_mutex_);
    for (auto it = observers_.begin(); it != observers_.end();)
      {
        if (current.count(it->first)) { ++it; continue; }
        observer::detach(poa, it->second);
        it = observers_.erase(it);
      }
    for (auto it = observer_failed_.begin(); it != observer_failed_.end();)
      {
        it = current.count(*it) ? std::next(it) : observer_failed_.erase(it);
      }
    std::string status = prop("snmp.observer.observed_status", "RTC_STATUS");
    for (auto& [ior, c] : current)
      {
        if (observers_.count(ior) || observer_failed_.count(ior)) { continue; }
        std::string error;
        auto h = observer::attach(poa, c->ref.in(), c->info.name, status,
                                  [this](const std::string& i, const std::string& k, const std::string& hint) {
                                    on_observer_status(i, k, hint);
                                  },
                                  error);
        if (!h)
          {
            // RTC 側に ComponentObserverConsumer がない (preload していない) と失敗する。一覧ごとに試し直さない
            observer_failed_.insert(ior);
            log("observer: cannot attach to " + c->info.name + " (" + error + ")");
            continue;
          }
        log("observer: attached to " + c->info.name + " (" + status + ", " + h->interface_type + ")");
        observers_[ior] = *h;
      }
  }

  void SNMPManager::on_observer_status(const std::string& instance, const std::string& kind, const std::string& hint)
  {
    // オブザーバーの通知 (RTC のスレッドから CORBA で呼ばれる)。ERROR なら自動復旧を待たずに始める
    {
      std::lock_guard<std::mutex> g(observer_mutex_);
      if (observer_noticed_.insert(instance).second) { log("observer: first notice from " + instance + ": " + kind + " " + hint); }
    }
    if (kind != "RTC_STATUS" || hint.rfind("ERROR", 0) != 0) { return; }
    event(name_, "rtc_error_notified", instance + " (" + hint + ")");
    mark_error(name_, instance);
    auto ips = peer_ips();
    std::string info = name_ + "|" + instance;
    std::thread([this, ips, info] { for (auto& ip : ips) { trap(ip, "RTC Error", info); } }).detach();
  }

  void SNMPManager::mark_error(const std::string& robot, const std::string& instance)
  {
    {
      std::lock_guard<std::mutex> g(recovery_mutex_);
      error_notified_[instance] = {robot, monotonic()};
    }
    std::thread([this] {
      // ERROR になったことを、次のポーリングを待たずに反映する
      try { refresh_inventory(); } catch (...) {}
      wake_recovery();
    }).detach();
  }
} // namespace snmpmgr
