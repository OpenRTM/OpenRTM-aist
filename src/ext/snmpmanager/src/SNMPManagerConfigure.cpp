// -*- C++ -*-
/*!
 * @file SNMPManagerConfigure.cpp
 * @brief Applying configurations (same flow, logs and event names as the Python version SNMPManager.py)
 *
 * @~japanese
 * 構成の適用 (Python 版 SNMPManager.py と同じ処理の流れ・同じログ・同じイベント名)
 *
 * - 実行ロボットの選出 (executor: "auto") と、止まっていたロボットの復帰 (Startup Query)
 * - RTC の一覧、待機モジュール、メモリの予約
 * - 自動配置 (ロジック optimal / first_fit / objective、ロボット間の制約、待機モジュール・既存の RTC)
 * - 計画の適用 (ポートの参照のキャッシュ、移動元の RTC の終了の並行実行)
 * - マイグレーション (mode: auto で移動先を選び直す)
 * @~english
 * - Executor election (executor: "auto") and rejoining of robots that were stopped (Startup Query)
 * - RTC lists, standby modules and memory reservation
 * - Automatic placement (logics optimal / first_fit / objective, inter-robot constraints, standby modules and existing RTCs)
 * - Applying plans (port reference cache, parallel termination of source RTCs)
 * - Migration (re-selects destinations with mode: auto)
 * @~
 *
 * @date $Date: 2026-11-13 16:24:27 $
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
#include "SlaveProcessObserver.hpp"
#include "CustomMetrics.hpp"
#include "SNMPManagerUtil.hpp"

#include <rtm/NVUtil.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <future>
#include <regex>

namespace snmpmgr
{
  namespace
  {
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
     * @brief JSON の配列 (または文字列) を文字列のリストにする
     *
     * @param v JSON の値
     * @return 文字列のリスト
     * @~english
     * @brief Converts a JSON array (or string) into a list of strings
     *
     * @param v JSON value
     * @return list of strings
     * @~
     */
    std::vector<std::string> string_list(const json& v)
    {
      std::vector<std::string> out;
      if (v.is_string()) { out.push_back(v.get<std::string>()); }
      else if (v.is_array()) { for (auto& x : v) { if (x.is_string()) { out.push_back(x.get<std::string>()); } } }
      return out;
    }
  } // namespace

  // ==================================================================== 構成ファイルの拡張の確認
  void SNMPManager::validate_extensions(const json& cfg, const json& managers)
  {
    // 追加の監視の値の制約: 置くロボット (metric_constraints) と実行ロボット (executor_constraints)
    for (const char* section : {"startup", "migration", "migration_conditions", "recovery"})
      {
        if (!cfg.contains(section) || !cfg[section].is_object()) { continue; }
        const json& sec = cfg[section];
        if (std::string(section) != "migration_conditions" && sec.contains("metric_constraints"))
          {
            metrics::validate_constraints(sec["metric_constraints"], custom_names(), std::string(section) + ".metric_constraints");
          }
        if (sec.contains("executor_constraints"))
          {
            metrics::validate_constraints(sec["executor_constraints"], custom_names(), std::string(section) + ".executor_constraints");
          }
      }
    if (cfg.contains("standby"))
      {
        const json& sb = cfg["standby"];
        if (!sb.is_object()) { throw std::runtime_error("standby must be an object"); }
        std::string scope = sb.value("scope", "any");
        if (scope != "any" && scope != "same") { throw std::runtime_error("standby.scope must be any or same"); }
        std::set<std::string> names;
        for (auto& m : sb.value("modules", json::array()))
          {
            if (!m.is_object() || !m.contains("name") || !m["name"].is_string() || !m.contains("manager") ||
                !managers.contains(m["manager"].get<std::string>()) || m.value("create", "").empty())
              {
                throw std::runtime_error("standby.modules entries need name, manager (configured) and create");
              }
            if (!names.insert(m["name"].get<std::string>()).second) { throw std::runtime_error("duplicate standby module " + m["name"].get<std::string>()); }
          }
      }
    if (cfg.contains("reservation"))
      {
        const json& rs = cfg["reservation"];
        if (!rs.is_object() || !rs.value("memory_mb", json::object()).is_object())
          {
            throw std::runtime_error("reservation must be an object with memory_mb {type: MB}");
          }
        json sizes = rs.value("memory_mb", json::object());
        for (auto& [k, v] : sizes.items())
          {
            if (!v.is_number() || v.get<double>() < 0) { throw std::runtime_error("reservation sizes must be nonnegative"); }
          }
        if (rs.value("default_mb", 0.0) < 0) { throw std::runtime_error("reservation sizes must be nonnegative"); }
      }
    for (const char* section : {"startup", "migration"})
      {
        if (!cfg.contains(section) || !cfg[section].is_object()) { continue; }
        const json& plan = cfg[section];
        if (plan.contains("logic") && !contains(logics(), plan["logic"].get<std::string>()))
          {
            throw std::runtime_error(std::string(section) + ": logic must be one of optimal, first_fit, objective");
          }
        if (plan.contains("objective_plugin") && !plugins()->has_objective(plan["objective_plugin"]))
          {
            throw std::runtime_error(std::string(section) + ": unknown objective plugin " + plan["objective_plugin"].get<std::string>());
          }
        if (plan.contains("objective") && plan["objective"].is_object())
          {
            json wts = plan["objective"].value("weights", json::object());
            for (auto& [k, v] : wts.items())
              {
                if (!default_weights().count(k)) { throw std::runtime_error(std::string(section) + ": unknown objective weight " + k); }
              }
          }
        for (const char* key : {"from", "exclude"})
          {
            for (auto& v : string_list(plan.value(key, json())))
              {
                if (!managers.contains(v)) { throw std::runtime_error(std::string(section) + ": " + key + " names unknown manager " + v); }
              }
          }
      }
    if (cfg.contains("recovery"))
      {
        const json& rc = cfg["recovery"];
        if (!rc.is_object()) { throw std::runtime_error("recovery must be an object"); }
        if (!cfg.contains("startup") || !cfg["startup"].is_object())
          {
            throw std::runtime_error("recovery needs the startup plan (the system configuration to keep running)");
          }
        std::set<std::string> names;
        for (auto& c : cfg["startup"].value("components", json::array())) { names.insert(c.value("name", "")); }
        for (auto& n : string_list(rc.value("components", json::array())))
          {
            if (!names.count(n)) { throw std::runtime_error("recovery.components names " + n + ", which is not in the startup plan"); }
          }
        static const std::set<std::string> known{"memory_free_min_mb", "memory_max_pct", "cpu_max_pct", "battery_min_pct",
                                                 "rssi_min_dbm", "latency_max_ms", "jitter_max_ms", "throughput_min_mbps",
                                                 "packet_loss_max", "down", "error", "missing", "duration_seconds"};
        json conds = rc.value("conditions", json::object());
        for (auto& [k, v] : conds.items())
          {
            if (!known.count(k) && !metrics::constraint_keys(custom_names()).count(k))
              {
                throw std::runtime_error("recovery.conditions: unknown key " + k);
              }
          }
        static const std::vector<std::string> stages{"reset", "standby_same", "restart", "other"};
        for (const char* kind : {"local", "remote"})
          {
            for (auto& st : string_list(rc.value("stages", json::object()).value(kind, json::array())))
              {
                if (!contains(stages, st)) { throw std::runtime_error(std::string("recovery.stages.") + kind + ": unknown stage " + st); }
              }
          }
        if (rc.contains("logic") && !contains(logics(), rc["logic"].get<std::string>()))
          {
            throw std::runtime_error("recovery.logic must be one of optimal, first_fit, objective");
          }
        std::string target = rc.value("target", "largest");
        if (target != "largest" && target != "all") { throw std::runtime_error("recovery.target must be largest or all"); }
      }
    for (const char* sec : {"startup", "migration", "migration_conditions", "recovery"})
      {
        if (!cfg.contains(sec) || !cfg[sec].is_object()) { continue; }
        for (auto& c : string_list(cfg[sec].value("executor_candidates", json::array())))
          {
            if (!managers.contains(c)) { throw std::runtime_error("executor_candidates names unknown manager " + c); }
          }
      }
  }

  // ==================================================================== 実行ロボット
  bool SNMPManager::is_executor(const json& cfg, std::initializer_list<const json*> sections,
                                const std::optional<std::map<std::string, int>>& counts, const std::string& purpose)
  {
    for (auto* s : sections)
      {
        if (!s || !s->is_object() || !s->contains("executor") || (*s)["executor"].get<std::string>().empty()) { continue; }
        std::string ex = (*s)["executor"];
        if (ex != "auto") { return ex == name_; }
        auto elected = elect_executor(cfg, *s, counts, purpose);
        return elected && *elected == name_;
      }
    return true;
  }

  std::optional<std::string> SNMPManager::elect_executor(const json& cfg, const json& section,
                                                         const std::optional<std::map<std::string, int>>& counts,
                                                         const std::string& purpose)
  {
    // 候補 (executor_candidates、既定は managers の順) のうち、生きていて通信状態がよいロボットから、
    // 割り当てられた RTC の数 (counts、既定は今動いている RTC の数) が最も多いものを選ぶ。同数ならリストの順。
    // まだ状態の分からない候補 (起動直後) があるときは nullopt (保留)
    std::vector<std::string> cands = string_list(section.value("executor_candidates", json::array()));
    if (cands.empty()) { for (auto& [n, v] : cfg["managers"].items()) { cands.push_back(n); } }
    double max_loss = prop_d("snmp.executor.max_packet_loss", 50);
    double max_latency = prop_d("snmp.executor.max_latency_ms", 0);
    bool liveness = prop_b("snmp.liveness.enabled", true);
    double grace = prop_d("snmp.liveness.timeout", 6.0) + std::max(prop_d("snmp.discovery_interval", 0), 0.0) + 1.0;
    // Wi-Fi の電波強度 (RSSI, dBm) がこの値より弱いロボットは選ばない (空なら見ない)。自分のロボットにも使う
    std::optional<double> min_rssi = to_double(prop("snmp.executor.min_rssi_dbm", ""));
    // 追加の監視の値の制約 (executor_constraints) を満たさないロボットは選ばない
    json ex_cons = section.value("executor_constraints", json());
    std::vector<std::string> eligible, weak, hot, hot_desc;
    std::map<std::string, int> cnt;
    std::set<std::string> unknown_inv;
    for (auto& n : cands)
      {
        auto r = find_robot(n);
        if (!r)
          {
            if (monotonic() - started_at_ <= grace) { return std::nullopt; }   // 起動が遅れているだけかもしれない
            continue;
          }
        std::lock_guard<std::recursive_mutex> g(robots_mutex_);
        if (!r->is_local && liveness)
          {
            if (r->status == "unknown") { return std::nullopt; }
            if (r->status != "alive") { continue; }
            if (r->packet_loss && *r->packet_loss > max_loss) { continue; }
            if (max_latency > 0 && r->latency_ms && *r->latency_ms > max_latency) { continue; }
          }
        cnt[n] = counts ? (counts->count(n) ? counts->at(n) : 0) : (r->inventory ? static_cast<int>(r->inventory->size()) : 0);
        if (!counts && !r->inventory) { unknown_inv.insert(n); }
        // 電波強度が分からなければ外さない
        if (min_rssi && r->wifi_dbm && *r->wifi_dbm < *min_rssi) { weak.push_back(n); continue; }
        if (!ex_cons.is_null())
          {
            auto bad = metrics::violations(r->metrics, ex_cons, custom_names());
            if (!bad.empty())
              {
                std::vector<std::string> d;
                for (auto& b : bad) { d.push_back(metrics::describe(b)); }
                hot.push_back(n);
                hot_desc.push_back(n + " (" + join(d) + ")");
                continue;
              }
          }
        eligible.push_back(n);
      }
    std::string excluded = weak.empty() ? "" : "; weak RSSI: " + join(weak);
    if (!hot.empty()) { excluded += "; metric constraints: " + join(hot_desc); }
    if (eligible.empty() && (!weak.empty() || !hot.empty()))
      {
        // 全員が条件を満たさない: 電波強度・追加の値の条件は外して選ぶ (実行するロボットがいなくならないように)
        for (auto& n : cands) { if (contains(weak, n) || contains(hot, n)) { eligible.push_back(n); } }
        excluded = hot.empty() ? "; all robots have weak RSSI" : "; all robots fail the RSSI / metric constraints";
      }
    if (eligible.empty()) { return std::nullopt; }
    std::string best = eligible.front();
    for (auto& n : eligible) { if (cnt[n] > cnt[best]) { best = n; } }
    std::lock_guard<std::mutex> g(elect_mutex_);
    if (last_elected_[purpose] != best)
      {
        last_elected_[purpose] = best;
        std::vector<std::string> d;
        for (auto& n : eligible) { d.push_back(n + ":" + (unknown_inv.count(n) ? std::string("?") : std::to_string(cnt[n]))); }
        event(best, "executor_elected", purpose + " (candidates " + join(d) + excluded + ")");
      }
    return best;
  }

  std::optional<std::map<std::string, int>> SNMPManager::planned_counts(const json& plan)
  {
    if (plan.value("mode", "manual") == "auto") { return std::nullopt; }
    std::map<std::string, int> counts;
    for (auto& c : plan.value("components", json::array())) { counts[c.value("manager", "")]++; }
    return counts;
  }

  std::string SNMPManager::instance_name(const json& entry)
  {
    if (!entry.value("existing", "").empty()) { return entry["existing"]; }
    static const std::regex re(R"((?:\?|&)instance_name=([^&]+))");
    std::smatch m;
    std::string create = entry.value("create", "");
    if (std::regex_search(create, m, re)) { return m[1]; }
    return entry.value("name", "");
  }

  // ==================================================================== 起動時の構成
  void SNMPManager::startup_configuration()
  {
    double retry = std::max(0.2, prop_d("snmp.startup_retry_interval", 3));
    double timeout = prop_d("snmp.startup_timeout", 60);
    double deadline = monotonic() + timeout;
    while (running_ && !startup_completed_)
      {
        try
          {
            json cfg = load_migration_config();
            if (!cfg.contains("startup")) { startup_completed_ = true; return; }
            json plan = cfg["startup"];
            if (plan.value("executor", "") == "auto")
              {
                // 実行するロボットを選ぶ。選ばれなかったロボットは、完了の通知 (トラップ) か、
                // 計画の RTC がそろったのを見て完了とする。選ばれたロボットが止まれば次が選ばれる
                auto elected = elect_executor(cfg, plan, planned_counts(plan), "startup");
                if (!elected) { throw std::runtime_error("electing the executor (waiting for robot status)"); }
                if (*elected != name_)
                  {
                    if (startup_observed(plan))
                      {
                        startup_completed_ = true;
                        startup_known_ = true;
                        log("startup configuration observed (applied by " + *elected + ")");
                        return;
                      }
                    if (already_started_elsewhere())
                      {
                        // 復帰したロボット: 待機モジュールに切り替わった RTC は計画の名前で見つからないので、
                        // 構成が終わっていると知っているロボットの返事で加わる
                        startup_completed_ = true;
                        startup_known_ = true;
                        std::string by;
                        { std::lock_guard<std::mutex> g(startup_mutex_); by = startup_reported_by_; }
                        log("startup configuration already applied (reported by " + by + "); joining the running system");
                        event(name_, "startup_joined", "reported by " + by);
                        write_status(json{{"status", "joined"}, {"time", iso_now()}, {"reported_by", by}});
                        return;
                      }
                    throw std::runtime_error("startup plan is applied by " + *elected + "; waiting for completion");
                  }
              }
            else if (!is_executor(cfg, {&plan}))
              {
                log("startup plan is applied by " + plan["executor"].get<std::string>() + "; skipped here");
                startup_completed_ = true;
                return;
              }
            if (already_started_elsewhere())
              {
                // 止まっていたロボットが復帰した: ほかのロボットで構成が動いているので、起動時の構成はしない
                startup_completed_ = true;
                startup_known_ = true;
                std::string by;
                { std::lock_guard<std::mutex> g(startup_mutex_); by = startup_reported_by_; }
                log("startup configuration already applied (reported by " + by + "); joining the running system");
                event(name_, "startup_joined", "reported by " + by);
                write_status(json{{"status", "joined"}, {"time", iso_now()}, {"reported_by", by}});
                return;
              }
            if (plan.contains("plugin"))
              {
                manager_refs(cfg);   // 全ロボットがそろうまで待つ
                if (!plugins()->action(plan["plugin"], plan.value("args", json::object())))
                  {
                    throw StartupFailure("startup plugin " + plan["plugin"].get<std::string>() + " reported failure");
                  }
                startup_completed_ = true;
                startup_known_ = true;
                event(name_, "startup_completed", "plugin " + plan["plugin"].get<std::string>());
                write_status(json{{"status", "ok"}, {"time", iso_now()}, {"plugin", plan["plugin"]}});
                return;
              }
            std::vector<std::string> details;
            if (plan.value("mode", "manual") == "auto")
              {
                auto res = auto_plan(cfg, plan);
                plan = res.first;
                details = res.second;
              }
            ApplyResult res = apply_plan(cfg, plan, "startup");
            launch_standby(cfg);
            // 作った RTC を一覧に反映しておく (直後の実行ロボットの選出が古い RTC の数を使わないように)
            try { refresh_inventory(); } catch (...) {}
            startup_completed_ = true;
            startup_known_ = true;
            log("startup configuration completed");
            json placement = json::array();
            std::vector<std::string> summary;
            for (auto& c : plan.value("components", json::array()))
              {
                std::string n = c["name"];
                std::string where = res.assign.count(n) ? res.assign[n] : c.value("manager", "");
                placement.push_back({{"name", n}, {"manager", where}});
                summary.push_back(n + "@" + where);
              }
            event(name_, "startup_completed", join(summary));
            write_status(json{{"status", "ok"}, {"time", iso_now()}, {"executor", name_}, {"placement", placement},
                              {"details", details}});
            notify_startup_completed();
            return;
          }
        catch (StartupFailure& e)
          {
            startup_failed_ = true;
            startup_failed(e.what(), e.reasons);
            return;
          }
        catch (std::exception& e)
          {
            if (timeout >= 0 && monotonic() >= deadline)
              {
                startup_failed_ = true;
                startup_failed(std::string("startup configuration timed out: ") + e.what(), {});
                return;
              }
            log(std::string("startup configuration pending (") + e.what() + "); retrying in " + fmt(retry) + "s");
          }
        catch (CORBA::SystemException& e)
          {
            if (timeout >= 0 && monotonic() >= deadline)
              {
                startup_failed_ = true;
                startup_failed("startup configuration timed out (CORBA)", {});
                return;
              }
            log(std::string("startup configuration pending (CORBA ") + e._name() + "); retrying in " + fmt(retry) + "s");
          }
        std::this_thread::sleep_for(std::chrono::duration<double>(retry));
      }
  }

  bool SNMPManager::startup_observed(const json& plan)
  {
    refresh_inventory();
    std::set<std::string> names;
    {
      std::lock_guard<std::recursive_mutex> g(robots_mutex_);
      std::vector<std::shared_ptr<Robot>> all = robots_;
      if (self_) { all.push_back(self_); }
      for (auto& r : all) { if (r->inventory) { for (auto& c : *r->inventory) { names.insert(c.name); } } }
    }
    for (auto& c : plan.value("components", json::array())) { if (!names.count(instance_name(c))) { return false; } }
    return true;
  }

  void SNMPManager::notify_startup_completed()
  {
    auto ips = peer_ips(false);
    std::thread([this, ips] { for (auto& ip : ips) { trap(ip, "Startup Completed"); } }).detach();
  }

  void SNMPManager::on_startup_notice(const std::string& ip, const std::string& sender)
  {
    {
      std::lock_guard<std::mutex> g(startup_mutex_);
      startup_reported_by_ = sender.empty() ? ip : sender;
      if (startup_querying_)
        {
          // 問い合わせへの返事: already_started_elsewhere() が受け取る
          startup_replied_ = true;
          startup_cv_.notify_all();
          return;
        }
    }
    startup_known_ = true;
    if (!startup_completed_.exchange(true)) { log("startup configuration completed by " + (sender.empty() ? ip : sender)); }
  }

  void SNMPManager::on_startup_query(const std::string& ip)
  {
    // 終わっていれば 'Startup Completed'、まだ (executor でないので飛ばしただけを含む) なら 'Startup Pending'
    bool done = startup_known_ && !startup_failed_;
    std::thread([this, ip, done] { trap(ip, done ? "Startup Completed" : "Startup Pending"); }).detach();
  }

  void SNMPManager::on_startup_pending(const std::string& ip)
  {
    std::lock_guard<std::mutex> g(startup_mutex_);
    if (!startup_querying_) { return; }
    startup_pending_from_.insert(ip);
    if (std::includes(startup_pending_from_.begin(), startup_pending_from_.end(),
                      startup_query_targets_.begin(), startup_query_targets_.end()))
      {
        startup_cv_.notify_all();   // 全員がまだ: 待たずに進める
      }
  }

  bool SNMPManager::already_started_elsewhere()
  {
    // 生きているほかのロボットに 'Startup Query' を送り、snmp.startup.query_wait 秒の間に
    // 'Startup Completed' の返事が来れば true。全員から 'Startup Pending' が返った時点で false
    double wait = prop_d("snmp.startup.query_wait", 2);
    if (wait <= 0) { return false; }
    auto ips = peer_ips();
    if (ips.empty()) { return false; }
    std::unique_lock<std::mutex> lk(startup_mutex_);
    startup_replied_ = false;
    startup_reported_by_.clear();
    startup_pending_from_.clear();
    startup_query_targets_ = std::set<std::string>(ips.begin(), ips.end());
    startup_querying_ = true;
    lk.unlock();
    for (auto& ip : ips) { std::thread([this, ip] { trap(ip, "Startup Query"); }).detach(); }
    lk.lock();
    startup_cv_.wait_for(lk, std::chrono::duration<double>(wait), [this] {
      return startup_replied_ || std::includes(startup_pending_from_.begin(), startup_pending_from_.end(),
                                               startup_query_targets_.begin(), startup_query_targets_.end());
    });
    startup_querying_ = false;
    return startup_replied_;
  }

  void SNMPManager::launch_standby(const json& cfg)
  {
    // standby.modules の待機モジュールを、アクティブにせずに起動しておく
    const json* sb = standby_cfg(cfg);
    if (!sb || sb->value("modules", json::array()).empty()) { return; }
    json plan = {{"components", json::array()}, {"connections", json::array()}, {"_no_standby", true}};
    for (auto m : (*sb)["modules"]) { m["activate"] = false; plan["components"].push_back(m); }
    ApplyResult res = apply_plan(cfg, plan, "standby");
    std::vector<std::string> d;
    for (auto& m : (*sb)["modules"])
      {
        std::string n = m["name"];
        d.push_back(n + "@" + (res.assign.count(n) ? res.assign[n] : m.value("manager", "")));
      }
    event(name_, "standby_launched", join(d));
  }

  // ==================================================================== RTC の一覧
  void SNMPManager::refresh_inventory()
  {
    std::vector<std::shared_ptr<Robot>> all;
    {
      std::lock_guard<std::recursive_mutex> g(robots_mutex_);
      if (self_) { all.push_back(self_); }
      for (auto& r : robots_) { all.push_back(r); }
    }
    for (auto& r : all)
      {
        if (!r->is_local && r->status == "dead")
          {
            std::lock_guard<std::recursive_mutex> g(robots_mutex_);
            r->inventory.reset();
            continue;
          }
        std::optional<std::vector<ComponentInfo>> inv;
        std::string why;
        try
          {
            RTM::Manager_var m = rtcops::resolve_manager(orb_, r->ip, corba_port_, r->name);
            std::vector<ComponentInfo> list;
            auto comps = rtcops::list_components(m);
            for (auto& c : comps) { list.push_back(c.info); }
            inv = list;
            if (r->is_local && observer_enabled_ && !process::replaces_launch())
              {
                // Windows: slave の pid を観測する (launch_shell を置き換えられないため)
                for (auto& c : comps) { if (c.slave_pid > 0) { process::watch(c.slave_pid, c.info.slave_name); } }
              }
            if (r->is_local && observer_rtc_enabled_)
              {
                try { attach_observers(comps); }
                catch (std::exception& e) { log(std::string("observer attach failed: ") + e.what()); }
                catch (CORBA::Exception& e) { log(std::string("observer attach failed: CORBA ") + e._name()); }
              }
          }
        catch (CORBA::SystemException& e) { why = std::string("CORBA ") + e._name(); }
        catch (std::exception& e) { why = e.what(); }
        catch (...) { why = "unknown error"; }
        std::lock_guard<std::recursive_mutex> g(robots_mutex_);
        if (!inv)
          {
            // 取れなかった (一時的な CORBA のタイムアウトなど)。生きているロボットは直前の一覧を残す
            // (実行ロボットの選出で RTC の数が 0 に見えて、2 台が同時に実行しないように)
            if (inventory_failing_.insert(r->name).second)
              {
                log("inventory of " + r->name + " unavailable (" + why + ")" + (r->inventory ? "; keeping the last one" : ""));
              }
            continue;
          }
        if (inventory_failing_.erase(r->name)) { log("inventory of " + r->name + " available again"); }
        r->inventory = inv;
        r->inventory_time = monotonic();
        // RTC がないときの使用量 (予約の土台)
        if (inv->empty() && r->cg_valid) { r->base_mem_mb = r->cg_mem_usage_mb; }
      }
  }

  void SNMPManager::inventory_loop()
  {
    double interval = std::max(0.5, prop_d("snmp.inventory.interval", 3));
    while (running_)
      {
        try { refresh_inventory(); }
        catch (std::exception& e) { log(std::string("inventory refresh failed: ") + e.what()); }
        for (double t = 0; t < interval && running_; t += 0.2) { std::this_thread::sleep_for(std::chrono::milliseconds(200)); }
      }
  }

  std::optional<std::vector<ComponentInfo>> SNMPManager::robot_inventory(const std::string& name)
  {
    auto r = find_robot(name);
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    return r ? r->inventory : std::nullopt;
  }

  // ==================================================================== 見込み値・待機モジュール・メモリの予約
  std::shared_ptr<ProfileDB> SNMPManager::profiles()
  {
    std::string sig;
    if (measure_enabled_ && prop_b("snmp.measure.use_for_placement", true))
      {
        // 計測の保存先の placement.xml (ほかのロボットが保存したものも) の一覧と更新時刻
        namespace fs = std::filesystem;
        std::error_code ec;
        std::vector<std::string> parts;
        for (auto& e : fs::recursive_directory_iterator(prop("snmp.measure.dir", "measured"), ec))
          {
            if (e.path().filename() != "placement.xml") { continue; }
            auto t = fs::last_write_time(e.path(), ec).time_since_epoch().count();
            parts.push_back(e.path().string() + "@" + std::to_string(t));
          }
        std::sort(parts.begin(), parts.end());
        sig = join(parts, ";");
      }
    std::lock_guard<std::mutex> g(profile_mutex_);
    if (!profile_db_ || sig != profile_sig_)
      {
        profile_db_ = std::make_shared<ProfileDB>(profile_roots(), prop("snmp.placement.environment", ""),
                                                  prop_b("snmp.measure.use_for_placement", true),
                                                  static_cast<int>(prop_d("snmp.measure.min_samples", 10)));
        profile_sig_ = sig;
      }
    return profile_db_;
  }

  double SNMPManager::slave_overhead(const std::string& language, const RtcProfile* prof)
  {
    for (auto& part : split(prop("snmp.placement.slave_overhead_mb", ""), ','))
      {
        auto p = part.rfind(':');
        if (p != std::string::npos && trim(part.substr(0, p)) == language)
          {
            if (auto v = to_double(part.substr(p + 1))) { return *v; }
          }
      }
    return prof ? prof->slave_overhead_mb : 0.0;
  }

  const json* SNMPManager::standby_cfg(const json& c)
  {
    if (!c.is_object() || !c.contains("standby") || !c["standby"].is_object() || !c["standby"].value("enabled", true)) { return nullptr; }
    return &c["standby"];
  }

  const json* SNMPManager::reservation_cfg(const json& c)
  {
    if (!c.is_object() || !c.contains("reservation") || !c["reservation"].is_object() || !c["reservation"].value("enabled", true)) { return nullptr; }
    return &c["reservation"];
  }

  double SNMPManager::reserve_mb(const json& rcfg, const std::string& impl, const std::string& type,
                                 const std::string& language, const json* entry)
  {
    // RTC 1 つの予約量: reserve_mb > reservation.memory_mb (実装 ID か型名) > placement.xml の見込み > default_mb
    if (entry && entry->contains("reserve_mb") && (*entry)["reserve_mb"].is_number()) { return (*entry)["reserve_mb"].get<double>(); }
    json table = rcfg.value("memory_mb", json::object());
    for (auto& key : {impl, type})
      {
        if (!key.empty() && table.contains(key)) { return table[key].get<double>(); }
      }
    auto db = profiles();
    std::vector<std::string> langs;
    if (!language.empty()) { langs.push_back(language); }
    langs.push_back("Python");
    langs.push_back("C++");
    for (auto& l : langs)
      {
        if (auto p = db->get(type, l)) { return p->memory_mb; }
      }
    return rcfg.value("default_mb", 0.0);
  }

  double SNMPManager::reserved_on(const json& rcfg, const Robot& robot)
  {
    // ロボットで動いている RTC の予約の合計 (slave マネージャの分を含む)。RTC が終われば外れる
    double total = 0;
    std::map<int, std::string> slaves;
    if (robot.inventory)
      {
        for (auto& c : *robot.inventory)
          {
            total += reserve_mb(rcfg, c.impl, c.type, c.language);
            if (c.slave) { slaves[c.slave] = c.language.empty() ? "Python" : c.language; }
          }
      }
    for (auto& [i, lang] : slaves) { total += slave_overhead(lang, nullptr); }
    return total;
  }

  std::map<std::string, std::string> SNMPManager::module_info(const std::string& robot, RTM::Manager_ptr mgr,
                                                             const std::string& module)
  {
    // get_loadable_modules() のモジュールの情報 (type_name, language)。マネージャごとにキャッシュする
    std::lock_guard<std::mutex> g(module_mutex_);
    auto& cache = loadable_modules_[robot];
    if (!cache.count(module))
      {
        try
          {
            for (auto& m : rtcops::loadable_modules(mgr))
              {
                std::string impl = !m["implementation_id"].empty() ? m["implementation_id"] : m["type_name"];
                if (!impl.empty() && !cache.count(impl)) { cache[impl] = m; }
              }
          }
        catch (...) {}
      }
    auto it = cache.find(module);
    return it == cache.end() ? std::map<std::string, std::string>{} : it->second;
  }

  std::optional<std::pair<std::string, rtcops::Component>> SNMPManager::find_standby(
      const ManagerSet& ms, const std::string& module, const std::vector<std::string>& robots,
      const std::vector<RTC::RTObject_var>& used, const std::set<std::string>& exclude)
  {
    // robots の順に、型が module の非アクティブな RTC (待機モジュール) を探す
    for (auto& name : robots)
      {
        if (!ms.has(name)) { continue; }
        for (auto& c : rtcops::list_components(ms.at(name)))
          {
            if (c.info.state != "INACTIVE" || (c.info.impl != module && c.info.type != module) || exclude.count(c.info.name)) { continue; }
            bool taken = false;
            for (auto& u : used)
              {
                try { if (!CORBA::is_nil(u) && c.ref->_is_equivalent(u)) { taken = true; break; } }
                catch (CORBA::SystemException&) {}
              }
            if (!taken) { return std::make_pair(name, c); }
          }
      }
    return std::nullopt;
  }

  double SNMPManager::check_reservation(const json& rcfg, const std::string& robot_name, RTM::Manager_ptr mgr,
                                        const json& entry, std::map<std::string, double>& pending)
  {
    // 生成する前に、予約を含めてメモリが足りるか確かめる (enforce: true なら足りないと失敗)
    std::string module = module_name_of(entry["create"]);
    auto info = module_info(robot_name, mgr, module);
    double need = reserve_mb(rcfg, module, info["type_name"].empty() ? module : info["type_name"], info["language"], &entry);
    auto r = find_robot(robot_name);
    if (r)
      {
        double free = 0;
        bool known = false;
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          if (r->cg_valid && r->cg_mem_limit_mb > 0)
            {
              double committed = r->base_mem_mb.value_or(0) + reserved_on(rcfg, *r) + pending[robot_name];
              free = r->cg_mem_limit_mb - prop_d("snmp.placement.memory_margin_mb", 32) - std::max(r->cg_mem_usage_mb, committed);
              known = true;
            }
        }
        if (known && need > free)
          {
            std::string msg = "memory reservation for " + entry["name"].get<std::string>() + " (" + fmt(need) +
                              "MB) exceeds free memory of " + robot_name + " (" + fmt(free) + "MB)";
            if (rcfg.value("enforce", false)) { throw std::runtime_error(msg); }
            log("warning: " + msg);
          }
      }
    pending[robot_name] += need;
    return need;
  }

  // ==================================================================== 自動配置
  Objective SNMPManager::objective_fn(const json& plan)
  {
    // objective のロジックの目的関数: objective_plugin があればプラグイン、なければ重み付き和
    if (plan.contains("objective_plugin"))
      {
        std::string name = plan["objective_plugin"];
        json args = plan.value("objective_args", json::object());
        return [this, name, args](const PlacementItem& it, const PlacementRobot& r, const ObjectiveState& st) {
          return plugins()->objective(name, it, r, st, args);
        };
      }
    std::map<std::string, double> weights;
    if (plan.contains("objective") && plan["objective"].is_object())
      {
        json wts = plan["objective"].value("weights", json::object());
        for (auto& [k, v] : wts.items()) { weights[k] = v.get<double>(); }
      }
    return [weights](const PlacementItem& it, const PlacementRobot& r, const ObjectiveState& st) {
      return objective_value(it, r, st, weights);
    };
  }

  PlacementRobot SNMPManager::robot_capacity(const std::string& n, const json& cfg, const std::set<std::string>& langs,
                                             const json* rcfg, const json& link_rules,
                                             const std::map<std::string, double>& pending)
  {
    double cpu_margin = prop_d("snmp.placement.cpu_margin", 10), mem_margin = prop_d("snmp.placement.memory_margin_mb", 32);
    double unlimited = prop_d("snmp.placement.unlimited_cpu", 100);
    size_t min_samples = static_cast<size_t>(prop_d("snmp.placement.min_cpu_samples", 3));
    auto default_tags = split(prop("snmp.placement.robot_tags", ""), ',');
    auto r = find_robot(n);
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    if (!r || !r->cg_valid || !r->cg_cpu_usage_pct || r->cg_cpu_samples.size() < min_samples)
      {
        // 起動直後の負荷を拾わないよう、使用率のサンプルがそろうまで待つ
        throw std::runtime_error("waiting for cgroup metrics of " + n);
      }
    double cpu_cap = r->cg_cpu_limit_pct > 0 ? r->cg_cpu_limit_pct : unlimited;
    double mem_cap = r->cg_mem_limit_mb > 0 ? r->cg_mem_limit_mb : r->mem_total_mb.value_or(0);
    PlacementRobot pr;
    pr.name = n;
    pr.cpu_limit = cpu_cap;
    pr.mem_limit_mb = mem_cap;
    const json& mc = cfg["managers"][n];
    pr.tags = mc.is_object() && mc.contains("tags") ? mc["tags"].get<std::vector<std::string>>() : default_tags;
    pr.languages.assign(langs.begin(), langs.end());
    double mem_used = r->cg_mem_usage_mb;
    std::string note;
    if (rcfg)
      {
        // メモリの予約: 動いている RTC (待機モジュールを含む) ごとに予約量を確保済みとみなす
        pr.reserved_mb = reserved_on(*rcfg, *r);
        double committed = r->base_mem_mb.value_or(0) + pr.reserved_mb + (pending.count(n) ? pending.at(n) : 0.0);
        if (committed > mem_used)
          {
            note = " (reserved " + fmt(pr.reserved_mb) + "MB + base " + fmt(r->base_mem_mb.value_or(0)) + "MB = " +
                   fmt(committed) + "MB > used)";
          }
        mem_used = std::max(mem_used, committed);
      }
    if (r->latency_ms) { pr.link["latency_ms"] = *r->latency_ms; }
    if (r->jitter_ms) { pr.link["jitter_ms"] = *r->jitter_ms; }
    if (r->packet_loss) { pr.link["packet_loss"] = *r->packet_loss; }
    if (r->wifi_dbm) { pr.link["rssi_dbm"] = *r->wifi_dbm; }
    pr.link_ok = r->is_local || link_ok(pr.link, link_rules);
    pr.cpu_free = std::max(0.0, cpu_cap - *r->cg_cpu_usage_pct - cpu_margin);
    pr.mem_free_mb = std::max(0.0, mem_cap - mem_used - mem_margin);
    std::string ss;
    for (double v : r->cg_cpu_samples) { ss += (ss.empty() ? "" : " ") + fmt(v); }
    log("capacity " + n + ": CPU limit " + fmt(cpu_cap, 0) + "% used " + fmt(*r->cg_cpu_usage_pct) + "% -> free " +
        fmt(pr.cpu_free) + "%, memory limit " + fmt(mem_cap, 0) + "MB used " + fmt(r->cg_mem_usage_mb) + "MB" + note +
        " -> free " + fmt(pr.mem_free_mb) + "MB (CPU samples: " + ss + ")" +
        (pr.link_ok ? "" : ", link constraints NOT satisfied"));
    return pr;
  }

  std::vector<std::string> SNMPManager::metric_excluded(const json& cfg, const json& constraints,
                                                       const std::vector<std::string>& already)
  {
    // 値が分からないロボットは外さない
    std::vector<std::string> out;
    if (!constraints.is_object() || constraints.empty()) { return out; }
    for (auto& [n, v] : cfg["managers"].items())
      {
        auto r = find_robot(n);
        if (!r || contains(already, n)) { continue; }
        std::vector<metrics::Violation> bad;
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          bad = metrics::violations(r->metrics, constraints, custom_names());
        }
        if (bad.empty()) { continue; }
        std::vector<std::string> d;
        for (auto& b : bad) { d.push_back(metrics::describe(b)); }
        log("placement: " + n + " is excluded (" + join(d) + ")");
        out.push_back(n);
      }
    return out;
  }

  std::pair<json, std::vector<std::string>> SNMPManager::auto_plan(const json& cfg, const json& plan,
                                                                   const std::vector<std::string>& exclude,
                                                                   bool strict, bool force_standby)
  {
    // 起動時 (strict) は全ロボットがそろうまで待つ。マイグレーション・復旧は、止まっているロボットと
    // exclude (移動元) を除いたロボットに置く。待機モジュールがあればそれを優先し、
    // メモリの予約を差し引いた空きで、logic (optimal / first_fit / objective / 配置プラグイン) で置き場所を決める
    ManagerSet ms = manager_refs(cfg, !strict);
    // 追加の監視の値の制約 (metric_constraints) を満たさないロボットには、新しく置かない (移動元と同じ扱い)
    std::vector<std::string> excl = exclude;
    for (auto& n : metric_excluded(cfg, plan.value("metric_constraints", json()), exclude)) { excl.push_back(n); }
    std::vector<std::string> names;
    for (auto& [n, v] : cfg["managers"].items())
      {
        if (ms.has(n) && !contains(excl, n)) { names.push_back(n); }
      }
    if (names.empty()) { throw std::runtime_error("no robot is available for placement"); }
    std::string executor = plan.value("executor", "");
    if (executor == "auto") { executor.clear(); }
    std::string primary = plan.value("primary", executor.empty() ? name_ : executor);
    if (!contains(names, primary)) { primary = names.front(); }
    std::vector<json> creates, existing;
    for (auto& c : plan["components"]) { (c.value("create", "").empty() ? existing : creates).push_back(c); }

    // 1. 各マスターマネージャで生成できるモジュール
    //    C++ のマスターは Python の RTC のプロファイルを rtcprof2_python (別プロセス) で取るため、
    //    get_loadable_modules() 自体が CPU を使う。結果はロボットごとに使い回し、起動時は、
    //    確認より前の CPU のサンプルを捨てて、確認後の値だけで空きを見積もる
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> loadable;
    std::map<std::string, std::set<std::string>> langs;
    std::vector<std::string> fresh;
    {
      std::lock_guard<std::mutex> g(module_mutex_);
      for (auto& n : names)
        {
          if (!loadable_checked_.count(n))
            {
              auto& cache = loadable_modules_[n];
              for (auto& m : rtcops::loadable_modules(ms.at(n)))
                {
                  std::string impl = !m["implementation_id"].empty() ? m["implementation_id"] : m["type_name"];
                  if (!impl.empty() && !cache.count(impl)) { cache[impl] = m; }
                }
              loadable_checked_.insert(n);
              fresh.push_back(n);
            }
          loadable[n] = loadable_modules_[n];
        }
    }
    for (auto& n : names) { langs[n] = rtcops::slave_languages(ms.at(n)); }
    if (strict && !fresh.empty())
      {
        std::lock_guard<std::recursive_mutex> g(robots_mutex_);
        for (auto& n : fresh)
          {
            if (auto r = find_robot(n))
              {
                r->cg_cpu_samples.clear(); r->cg_cpu_usage_pct.reset(); r->cg_last_ts = -1;
                r->cg_ignore_before_ns = static_cast<long long>(wallclock() * 1e9);
              }
          }
      }
    std::vector<std::string> missing;
    for (auto& c : creates)
      {
        std::string module = module_name_of(c["create"]);
        bool ok = false;
        for (auto& n : names) { if (loadable[n].count(module)) { ok = true; } }
        if (!ok) { missing.push_back(c["name"].get<std::string>() + " (" + module + "): どのマスターマネージャの get_loadable_modules() にもない"); }
      }
    if (!missing.empty())
      {
        missing.push_back("ロード可能なモジュール:");
        for (auto& n : names)
          {
            std::vector<std::string> mods;
            for (auto& [m, v] : loadable[n]) { mods.push_back(m); }
            missing.push_back(n + ": " + (mods.empty() ? std::string("(なし)") : join(mods)));
          }
        throw StartupFailure("起動予定の RTC をロードできるマスターマネージャがありません", missing);
      }

    // 2. 各ロボットの空き (cgroup の制限 - max(現在の使用量, 予約) - margin) と通信状態
    const json* rcfg = reservation_cfg(cfg);
    refresh_inventory();
    json link_rules = plan.value("link_constraints", json::object());
    std::vector<PlacementRobot> robots;
    for (auto& n : names) { robots.push_back(robot_capacity(n, cfg, langs[n], rcfg, link_rules, {})); }
    // 除外したロボット (移動元など) でも、生きていれば、そこで動き続ける既存の RTC は置ける (空き 0 で加える)
    std::set<std::string> extra;
    for (auto& c : existing)
      {
        std::string m = c.value("manager", "");
        if (!contains(names, m) && ms.has(m)) { extra.insert(m); }
      }
    for (auto& n : extra)
      {
        PlacementRobot pr;
        pr.name = n;
        robots.push_back(pr);
        log("capacity " + n + ": excluded from placement (keeps its existing RTCs only)");
      }
    std::vector<std::string> robot_names;
    for (auto& r : robots) { robot_names.push_back(r.name); }

    // 3. RTC の見込み値と置けるロボット (待機モジュールがあればそこに固定する)
    auto db = profiles();
    json sb_any = {{"scope", "any"}};
    const json* sb = standby_cfg(cfg);
    if (!sb && force_standby) { sb = &sb_any; }
    std::vector<RTC::RTObject_var> used_standby;
    // 計画の RTC と、計画で消す RTC (移動元。並行して終了させる) は待機モジュールにしない
    std::set<std::string> plan_names;
    for (auto& c : plan["components"]) { plan_names.insert(instance_name(c)); }
    for (auto& r : plan.value("remove", json::array())) { plan_names.insert(r.value("name", "")); }
    std::vector<PlacementItem> items;
    std::map<std::string, RtcProfile> profs;
    std::map<std::string, std::string> item_lang;
    std::vector<std::string> problems;
    for (auto& c : creates)
      {
        std::string module = module_name_of(c["create"]);
        std::map<std::string, std::string> mod;
        for (auto& n : names) { if (loadable[n].count(module)) { mod = loadable[n][module]; break; } }
        std::string type = mod["type_name"].empty() ? module : mod["type_name"];
        std::string lang = mod["language"].empty() ? "C++" : mod["language"];
        auto prof = db->get(type, lang);
        if (!prof && lang == "Python3") { prof = db->get(type, "Python"); }
        if (!prof) { problems.push_back(c["name"].get<std::string>() + " (" + type + "): placement.xml に計測値がない"); continue; }
        PlacementItem it;
        it.name = c["name"];
        it.module = module;
        it.language = prof->language;
        it.cpu = prof->cpu;
        it.memory_mb = prof->memory_mb;
        if (rcfg)
          {
            // 置く RTC も予約量を確保する (見込みより予約が大きければ予約で数える)
            it.memory_mb = std::max(it.memory_mb, reserve_mb(*rcfg, module, prof->rtc, prof->language, &c));
          }
        it.slave_overhead_mb = slave_overhead(prof->language, &*prof);
        for (auto& r : robots)
          {
            bool tags_ok = std::all_of(prof->required_tags.begin(), prof->required_tags.end(),
                                       [&](auto& t) { return contains(r.tags, t); });
            if (loadable.count(r.name) && loadable[r.name].count(module) && (!c.contains("manager") || c["manager"] == r.name) && tags_ok)
              {
                it.candidates.push_back(r.name);
              }
          }
        // 同じ名前の RTC が移動先の候補で既に動いている (別のロボットが一度実行した) なら、そのまま使う
        std::string inst = instance_name(c);
        std::string where;
        for (auto& n : names)
          {
            auto inv = robot_inventory(n);
            if (inv && std::any_of(inv->begin(), inv->end(), [&](auto& x) { return x.name == inst; })) { where = n; break; }
          }
        if (!strict && !where.empty())
          {
            it.candidates = {where};
            it.cpu = it.memory_mb = it.slave_overhead_mb = 0;
            it.fixed = true;
          }
        else if (sb && c.value("standby", true))
          {
            std::vector<std::string> scope;
            if (sb->value("scope", "any") == "same" && c.contains("manager")) { scope.push_back(c["manager"]); }
            else
              {
                if (c.contains("manager")) { scope.push_back(c["manager"]); }
                for (auto& n : names) { if (!c.contains("manager") || n != c["manager"].get<std::string>()) { scope.push_back(n); } }
              }
            std::vector<std::string> in_cands;
            for (auto& n : scope) { if (contains(it.candidates, n)) { in_cands.push_back(n); } }
            if (auto found = find_standby(ms, module, in_cands, used_standby, plan_names))
              {
                used_standby.push_back(RTC::RTObject::_duplicate(found->second.ref));
                // 待機モジュールのメモリは既に使用量 (と予約) に入っている。アクティブにすると CPU を使う
                it.candidates = {found->first};
                it.memory_mb = it.slave_overhead_mb = 0;
                it.standby = found->second.info.name;
              }
          }
        profs[it.name] = *prof;
        item_lang[it.name] = lang;
        items.push_back(it);
      }
    for (auto& c : existing)
      {
        std::string m = c.value("manager", "");
        if (!contains(robot_names, m)) { problems.push_back(c["name"].get<std::string>() + ": 既存の RTC のロボット " + m + " が使えない"); continue; }
        PlacementItem it;
        it.name = c["name"];
        it.module = c["existing"];
        it.language = "Python";
        it.candidates = {m};
        it.fixed = true;
        if (auto inv = robot_inventory(m))
          {
            for (auto& x : *inv)
              {
                if (x.name != c["existing"].get<std::string>()) { continue; }
                it.module = x.type;
                if (!x.language.empty()) { it.language = x.language; }
                if (auto prof = db->get(x.type, it.language)) { profs[it.name] = *prof; }
              }
          }
        items.push_back(it);
      }
    if (!problems.empty()) { throw StartupFailure("RTC の見込み値がないため配置できません", problems); }
    auto edges = build_edges(plan.value("connections", json::array()), items, profs);
    std::string logic = plan.value("logic", prop("snmp.placement.logic", "optimal"));
    PlacementResult result;
    try
      {
        if (plan.contains("placement_plugin"))
          {
            logic = "plugin " + plan["placement_plugin"].get<std::string>();
            result = plugin_placement(plan, items, robots, edges, primary);
          }
        else { result = solve(items, robots, edges, primary, logic, objective_fn(plan)); }
      }
    catch (PlacementError& e) { throw StartupFailure(e.what(), e.reasons); }
    std::vector<std::string> lines{"logic: " + logic +
                                   (plan.contains("objective_plugin") ? ", objective plugin " + plan["objective_plugin"].get<std::string>() : "")};
    std::vector<std::string> measured;
    for (auto& it : items)
      {
        auto p = profs.find(it.name);
        if (it.fixed || p == profs.end() || !p->second.measured) { continue; }
        measured.push_back(it.name + " (" + p->second.rtc + " cpu " + fmt(p->second.cpu, 2) + "% mem " +
                           fmt(p->second.memory_mb, 2) + "MB, " + std::to_string(p->second.samples) + " samples)");
      }
    if (!measured.empty()) { lines.push_back("measured estimates: " + join(measured)); }
    for (auto& l : describe(result, items, robots, primary, profs)) { lines.push_back(l); }
    log("auto placement:");
    for (auto& l : lines) { log("  " + l); }
    json manual = plan;
    manual["mode"] = "manual";
    std::map<std::string, const PlacementItem*> by_name;
    for (auto& it : items) { by_name[it.name] = &it; }
    for (auto& c : manual["components"])
      {
        std::string n = c["name"];
        c["manager"] = result.assign[n];
        if (item_lang.count(n)) { c["language"] = item_lang[n]; }   // C++ マネージャへ言語付きで create する
        if (!by_name[n]->standby.empty()) { c["standby_instance"] = by_name[n]->standby; }
      }
    return {manual, lines};
  }

  // ==================================================================== 構成の適用
  ManagerSet SNMPManager::manager_refs(const json& config, bool allow_down)
  {
    // allow_down (マイグレーション・復旧): 止まっているロボットは飛ばす (down に入れる)
    std::map<std::string, std::string> by_name;
    {
      std::lock_guard<std::recursive_mutex> g(robots_mutex_);
      std::vector<std::shared_ptr<Robot>> all = robots_;
      if (self_) { all.push_back(self_); }
      for (auto& r : all)
        {
          if (by_name.count(r->name) && by_name[r->name] != r->ip) { throw std::runtime_error("Duplicate robot name " + r->name); }
          by_name[r->name] = r->ip;
        }
    }
    bool liveness = prop_b("snmp.liveness.enabled", true);
    ManagerSet ms;
    for (auto& [name, v] : config["managers"].items())
      {
        try
          {
            if (!by_name.count(name)) { throw std::runtime_error("Robot " + name + " not discovered via SNMP"); }
            if (liveness && !is_alive(name)) { throw std::runtime_error("Robot " + name + " is down (no response)"); }
            ms.refs[name] = rtcops::resolve_manager(orb_, by_name[name], corba_port_, name);
          }
        catch (std::exception&)
          {
            if (!allow_down) { throw; }
            ms.down.push_back(name);
          }
      }
    return ms;
  }

  void SNMPManager::forget_ports(RTC::RTObject_ptr comp)
  {
    try
      {
        CORBA::String_var ior = orb_->object_to_string(comp);
        std::lock_guard<std::mutex> g(port_mutex_);
        port_cache_.erase(std::string(ior.in()));
      }
    catch (...) {}
  }

  std::pair<RTC::PortService_var, std::string> SNMPManager::lookup_port(RTC::RTObject_ptr comp, const std::string& port,
                                                                       const std::string& module,
                                                                       const std::string& planned_instance, int& calls)
  {
    // snmp.port_cache.enabled (既定 YES) なら、RTC ごとにポートの参照を一度だけ取ってキャッシュする
    //   * 同じモジュールの RTC を一度見ていれば、get_ports() 1 回で、ポートの並び順 (インデックス) から取る
    //   * 初めてのモジュールは get_component_profile() 1 回で、全ポートの名前と参照を取り、並び順を覚える
    if (!prop_b("snmp.port_cache.enabled", true))
      {
        // 従来の方法 (CORBA_RTCUtil::get_port_by_name と同じ手順。呼び出しの数を数える)
        std::string inst = rtcops::instance_name(comp);
        std::string full = port.rfind(inst + ".", 0) == 0 ? port : inst + "." + port;
        calls += 2;
        RTC::PortServiceList_var ports = comp->get_ports();
        for (CORBA::ULong i = 0; i < ports->length(); ++i)
          {
            ++calls;
            RTC::PortProfile_var pp = ports[i]->get_port_profile();
            if (full == pp->name.in()) { return {RTC::PortService::_duplicate(ports[i]), full}; }
          }
        throw std::runtime_error("Port not found: " + full);
      }
    CORBA::String_var ior_s = orb_->object_to_string(comp);
    std::string key(ior_s.in());
    auto load = [&](bool use_index) {
      PortEntry entry;
      bool done = false;
      std::vector<std::string> names;
      {
        std::lock_guard<std::mutex> g(port_mutex_);
        if (use_index && !module.empty() && port_index_.count(module)) { names = port_index_[module]; }
      }
      if (!names.empty())
        {
          // 同じモジュールの RTC を前に見ている: get_ports() の並び順で取る (プロファイルを取らない)
          RTC::PortServiceList_var ports = comp->get_ports();
          ++calls;
          if (ports->length() == names.size())
            {
              std::string inst = planned_instance;
              if (prop_b("snmp.port_cache.verify", false) || inst.empty())
                {
                  RTC::PortProfile_var pp = ports[0]->get_port_profile();
                  ++calls;
                  std::string full(pp->name.in());
                  auto dot = full.find('.');
                  inst = dot != std::string::npos && full.substr(dot + 1) == names[0] ? full.substr(0, dot) : "";
                }
              if (!inst.empty())
                {
                  entry.instance = inst;
                  for (size_t i = 0; i < names.size(); ++i) { entry.ports[names[i]] = RTC::PortService::_duplicate(ports[i]); }
                  done = true;
                }
            }
        }
      if (!done)
        {
          auto prof = rtcops::ports_by_profile(comp);
          ++calls;
          entry.instance = prof.first;
          std::vector<std::string> shorts;
          for (auto& [n, ref] : prof.second) { entry.ports[n] = ref; shorts.push_back(n); }
          if (!module.empty())
            {
              std::lock_guard<std::mutex> g(port_mutex_);
              port_index_[module] = shorts;
            }
        }
      std::lock_guard<std::mutex> g(port_mutex_);
      port_cache_[key] = entry;
      return entry;
    };
    PortEntry entry;
    {
      std::lock_guard<std::mutex> g(port_mutex_);
      auto it = port_cache_.find(key);
      if (it != port_cache_.end()) { entry = it->second; }
    }
    if (entry.instance.empty()) { entry = load(true); }
    std::string short_name = port.rfind(entry.instance + ".", 0) == 0 ? port.substr(entry.instance.size() + 1) : port;
    auto p = entry.ports.find(short_name);
    if (p == entry.ports.end())
      {
        // 並び順が変わった (別のバージョンの RTC など): 取り直す
        entry = load(false);
        short_name = port.rfind(entry.instance + ".", 0) == 0 ? port.substr(entry.instance.size() + 1) : port;
        p = entry.ports.find(short_name);
        if (p == entry.ports.end()) { throw std::runtime_error("Port not found: " + entry.instance + "." + short_name); }
      }
    return {RTC::PortService::_duplicate(p->second), entry.instance + "." + short_name};
  }

  void SNMPManager::remove_one(const std::string& name, const std::string& robot, RTC::RTObject_ptr comp)
  {
    forget_ports(comp);
    rtcops::remove(comp);
    logfile(fmt(wallclock(), 6) + "\tRTC_STATUS\t" + name + "\t0");
    log("removed " + name + " from " + robot);
  }

  ApplyResult SNMPManager::apply_plan(const json& config, const json& plan, const std::string& label)
  {
    {
      std::lock_guard<std::mutex> g(migration_mutex_);
      if (migrating_) { throw std::runtime_error("Another RTC configuration operation is running"); }
      migrating_ = true;
    }
    struct Reset { SNMPManager* s; ~Reset() { std::lock_guard<std::mutex> g(s->migration_mutex_); s->migrating_ = false; } } reset{this};
    double t0 = monotonic();
    // 起動時は全ロボットがそろっていること。マイグレーション・復旧では止まったロボットを飛ばす
    ManagerSet ms = manager_refs(config, label != "startup" && label != "standby");
    if (!ms.down.empty()) { log(label + ": robots down: " + join(ms.down)); }

    // 移動元の RTC の終了 (計画の RTC と同じロボット・同じ名前は消さない)
    struct Removal { std::string name, robot; RTC::RTObject_var comp; };
    std::vector<Removal> removals;
    for (auto& e : plan.value("remove", json::array()))
      {
        std::string n = e["name"], m = e["manager"];
        if (!ms.has(m)) { log("skip removing " + n + ": " + m + " is down"); continue; }   // ロボットごと止まっている
        RTC::RTObject_var rtc = rtcops::find(ms.at(m), n);
        if (!CORBA::is_nil(rtc)) { removals.push_back({n, m, rtc}); }
      }
    bool parallel = plan.value("parallel_remove", prop_b("snmp.migration.parallel_remove", false)) && !removals.empty();
    std::vector<std::future<void>> removing;
    std::vector<RTC::RTObject_var> removing_refs;
    double remove_started = monotonic();
    if (parallel)
      {
        // 新しい RTC の生成・接続と並行して、別スレッドで終了させる
        for (auto& c : plan.value("components", json::array()))
          {
            for (auto& r : removals)
              {
                if (r.robot == c.value("manager", "") && r.name == instance_name(c))
                  {
                    throw std::runtime_error("Refusing to remove " + r.name + ": it is part of the new configuration");
                  }
              }
          }
        std::vector<std::string> names;
        for (auto& r : removals)
          {
            names.push_back(r.name);
            removing_refs.push_back(RTC::RTObject::_duplicate(r.comp));
            RTC::RTObject_ptr p = r.comp.in();
            std::string n = r.name, m = r.robot;
            removing.push_back(std::async(std::launch::async, [this, n, m, p] { remove_one(n, m, p); }));
          }
        log("removing " + join(names) + " in parallel");
      }
    auto join_removal = [&] {
      std::exception_ptr err;
      for (auto& f : removing)
        {
          try { f.get(); }
          catch (...) { if (!err) { err = std::current_exception(); } }
        }
      removing.clear();
      return err;
    };

    ApplyResult res;
    int calls = 0;
    double t1 = 0, t2 = 0, t3 = 0;
    std::map<std::string, RTC::RTObject_var> comps;
    std::set<std::string> created_now;
    std::map<std::string, std::string> port_module, planned_instance;
    try
      {
        // ---- 生成 (待機モジュール・メモリの予約)
        json sb_any = {{"scope", "any"}};
        const json* sb = plan.value("_no_standby", false) ? nullptr : standby_cfg(config);
        if (!sb && plan.value("_force_standby", false)) { sb = &sb_any; }
        const json* rcfg = reservation_cfg(config);
        std::vector<std::string> order;
        for (auto& [n, v] : config["managers"].items()) { if (ms.has(n)) { order.push_back(n); } }
        std::set<std::string> plan_names;
        for (auto& e : plan.value("components", json::array())) { plan_names.insert(instance_name(e)); }
        std::map<std::string, double> pending;
        for (auto& e : plan.value("components", json::array()))
          {
            std::string name = e["name"], mgr_name = e["manager"];
            std::string create = e.value("create", "");
            std::string actual = instance_name(e), last = actual;
            planned_instance[name] = actual;
            RTC::RTObject_var rtc;
            if (!e.value("standby_instance", "").empty())
              {
                // 自動配置で選んだ待機モジュール
                rtc = rtcops::find(ms.at(mgr_name), e["standby_instance"]);
                if (!CORBA::is_nil(rtc))
                  {
                    last = e["standby_instance"];
                    log("using standby " + last + " on " + mgr_name + " for " + name);
                    event(mgr_name, "standby_used", name + " -> " + last);
                  }
              }
            if (CORBA::is_nil(rtc)) { rtc = rtcops::find(ms.at(mgr_name), actual); }
            if (CORBA::is_nil(rtc) && !create.empty() && sb && e.value("standby", true))
              {
                std::vector<std::string> scope{mgr_name};
                if (sb->value("scope", "any") == "any") { for (auto& n : order) { if (n != mgr_name) { scope.push_back(n); } } }
                std::vector<RTC::RTObject_var> used = removing_refs;
                for (auto& [n, c] : comps) { used.push_back(RTC::RTObject::_duplicate(c)); }
                if (auto found = find_standby(ms, module_name_of(create), scope, used, plan_names))
                  {
                    mgr_name = found->first;
                    rtc = RTC::RTObject::_duplicate(found->second.ref);
                    last = found->second.info.name;
                    log("using standby " + last + " on " + mgr_name + " for " + name);
                    event(mgr_name, "standby_used", name + " -> " + last);
                  }
              }
            if (CORBA::is_nil(rtc))
              {
                if (create.empty()) { throw std::runtime_error("Existing RTC not found: " + actual); }
                if (rcfg) { check_reservation(*rcfg, mgr_name, ms.at(mgr_name), e, pending); }
                rtc = rtcops::create(ms.at(mgr_name), qualify_create(create, e.value("language", "")));
                logfile(fmt(wallclock(), 6) + "\tRTC_STATUS\t" + name + "\t1");
                log("created " + actual + " on " + mgr_name);
                created_now.insert(name);
                port_module[name] = module_name_of(create);
              }
            comps[name] = rtc;
            res.assign[name] = mgr_name;
            res.instances[name] = last;
            planned_instance[name] = last;
          }
        t1 = monotonic();
        // ---- 接続 (この計画で生成した RTC には接続がまだないので、既存の接続の確認を省く)
        for (auto& c : plan.value("connections", json::array()))
          {
            std::string s = c["source"]["component"], t = c["target"]["component"], cname = c["name"];
            for (int attempt = 0; attempt < 2; ++attempt)
              {
                auto src = lookup_port(comps[s], c["source"]["port"], port_module[s], planned_instance[s], calls);
                auto dst = lookup_port(comps[t], c["target"]["port"], port_module[t], planned_instance[t], calls);
                try
                  {
                    bool fresh = created_now.count(s) || created_now.count(t);
                    if (!fresh && rtcops::connector_exists(src.first, dst.first, cname)) { break; }
                    rtcops::connect(cname, c.value("properties", json::object()), src.first, dst.first);
                  }
                catch (CORBA::SystemException& ex)
                  {
                    if (attempt || !prop_b("snmp.port_cache.enabled", true)) { throw; }
                    // キャッシュした参照が古い (RTC が作り直された): 取り直してもう一度
                    forget_ports(comps[s]);
                    forget_ports(comps[t]);
                    continue;
                  }
                log("connected " + src.second + " -> " + dst.second + " (" + cname + ")");
                break;
              }
          }
        t2 = monotonic();
        for (auto& e : plan.value("components", json::array()))
          {
            if (e.value("activate", false)) { rtcops::activate(comps[e["name"]]); }
          }
        t3 = monotonic();
      }
    catch (...)
      {
        join_removal();
        throw;
      }
    double remove_time = 0;
    if (parallel)
      {
        if (auto err = join_removal()) { std::rethrow_exception(err); }
        remove_time = monotonic() - remove_started;
      }
    else
      {
        for (auto& r : removals)
          {
            for (auto& [n, used] : comps)
              {
                if (r.comp->_is_equivalent(used)) { throw std::runtime_error("Refusing to remove " + r.name + ": it is part of the new configuration"); }
              }
            remove_one(r.name, r.robot, r.comp);
          }
        remove_time = monotonic() - t3;
      }
    {
      std::lock_guard<std::mutex> g(recovery_mutex_);
      for (auto& [n, robot] : res.assign) { recovery_where_[n] = {robot, res.instances[n]}; }
    }
    last_plan_applied_ = monotonic();
    double t4 = monotonic();
    logfile(fmt(wallclock(), 6) + "\t" + label + "Time:" + std::to_string(t4 - t0) + ":" + std::to_string(t1 - t0) + ":" +
            std::to_string(t2 - t1) + ":" + std::to_string(t4 - t3) + "\t\t");
    if (!plan.value("connections", json::array()).empty() || !plan.value("remove", json::array()).empty())
      {
        log(label + " timing: total " + fmt(t4 - t0, 3) + "s (create " + fmt(t1 - t0, 3) + "s, connect " + fmt(t2 - t1, 3) +
            "s, activate " + fmt(t3 - t2, 3) + "s, remove " + fmt(remove_time, 3) + "s" + (parallel ? " in parallel" : "") +
            "); port lookups " + std::to_string(calls) + " CORBA calls" +
            (prop_b("snmp.port_cache.enabled", true) ? "" : " (port cache off)"));
      }
    return res;
  }

  bool SNMPManager::migration()
  {
    json cfg = load_migration_config();
    json plan = cfg.value("migration", json{{"components", cfg.value("components", json::array())},
                                            {"connections", cfg.value("connections", json::array())}});
    if (plan.contains("plugin"))
      {
        // 排他は plugin の apply_plan ごとに行う
        if (!plugins()->action(plan["plugin"], plan.value("args", json::object())))
          {
            throw std::runtime_error("migration plugin " + plan["plugin"].get<std::string>() + " reported failure");
          }
        return true;
      }
    if (plan.value("components", json::array()).empty() && plan.value("connections", json::array()).empty() &&
        plan.value("remove", json::array()).empty())
      {
        return false;
      }
    if (plan.value("mode", "manual") == "auto")
      {
        // 移動先を今の状態から決め直す (移動元と止まっているロボットを除く)
        auto exclude = string_list(plan.value("exclude", json()));
        if (exclude.empty()) { exclude = string_list(plan.value("from", json())); }
        plan = auto_plan(cfg, plan, exclude, false).first;
      }
    ApplyResult res = apply_plan(cfg, plan, "Migration");
    std::vector<std::string> d;
    for (auto& [k, v] : res.assign) { d.push_back(k + "@" + v); }
    event(name_, "migration_placement", join(d));
    return true;
  }
} // namespace snmpmgr
