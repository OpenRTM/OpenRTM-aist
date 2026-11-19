// -*- C++ -*-
/*!
 * @file SNMPManager.cpp
 * @brief Main part of SNMPManager (C++ version). The flow, logs and event names are the same as the Python version SNMPManager.py
 *
 * @~japanese
 * SNMPManager (C++ 版) の本体。処理の流れ・ログ・イベント名は Python 版 SNMPManager.py と同じ
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
#include "MeasurementAggregator.hpp"
#include "CustomMetrics.hpp"
#include "OSPlatform.hpp"
#include "SlaveProcessObserver.hpp"
#include "SNMPManagerUtil.hpp"

#include <rtm/NVUtil.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <regex>

namespace snmpmgr
{
  namespace
  {
    const std::vector<std::pair<std::string, std::string>> kCpuRaw = {
        {"user", "1.3.6.1.4.1.2021.11.50.0"},   {"nice", "1.3.6.1.4.1.2021.11.51.0"},
        {"system", "1.3.6.1.4.1.2021.11.52.0"}, {"idle", "1.3.6.1.4.1.2021.11.53.0"},
        {"iowait", "1.3.6.1.4.1.2021.11.54.0"}, {"kernel", "1.3.6.1.4.1.2021.11.55.0"},
        {"interrupt", "1.3.6.1.4.1.2021.11.56.0"}};
    const char* kMemTotal = "1.3.6.1.4.1.2021.4.5.0";
    const char* kMemAvail = "1.3.6.1.4.1.2021.4.6.0";
    const char* kTrapName = "1.3.6.1.2.1.1.1.0";
    const char* kTrapInfo = "1.3.6.1.2.1.1.5.0";
    const char* kIfDescr = "1.3.6.1.2.1.2.2.1.2";
    const char* kIfIn = "1.3.6.1.2.1.2.2.1.10";
    const char* kIfOut = "1.3.6.1.2.1.2.2.1.16";
    const char* kWirelessLegacy = "1.3.6.1.4.1.8072.1.3.2.3.1.2.12.119.105.102.105.83.116.114.101.110.103.116.104";
    const char* kSysUpTime = "1.3.6.1.2.1.1.3.0";
    const std::vector<std::string> kBaseMetrics = {"wifi_strength", "wifi_signal_dbm", "wifi_signal_client_dbm",
                                                   "wifi_signal_ap_dbm", "cpu_usage", "memory_usage",
                                                   "network_in", "network_out", "battery_pct"};
    const std::vector<std::string> kLivenessMetrics = {"alive", "latency_ms", "jitter_ms", "packet_loss"};
    const std::map<std::string, std::string> kCgroupTokens = {{"mem_limit", "cgroupMemLimit"}, {"mem_usage", "cgroupMemUsage"},
                                                              {"cpu_limit", "cgroupCpuLimit"}, {"cpu_usage", "cgroupCpuUsage"}};

    /*!
     * @~japanese
     * @brief snmpd の extend の出力 (nsExtendOutput1Line) の OID を返す
     *
     * @param token extend の名前
     * @return OID
     * @~english
     * @brief Returns the OID of the output of an snmpd extend (nsExtendOutput1Line)
     *
     * @param token name of the extend
     * @return OID
     * @~
     */
    std::string extend_output_oid(const std::string& token)
    {
      // NET-SNMP-EXTEND-MIB::nsExtendOutput1Line."token"
      std::string s = "1.3.6.1.4.1.8072.1.3.2.3.1.2." + std::to_string(token.size());
      for (unsigned char c : token) { s += "." + std::to_string(c); }
      return s;
    }

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
     * @brief 自分の IPv4 アドレスを返す
     *
     * @return IP アドレスの集合
     * @~english
     * @brief Returns the local IPv4 addresses
     *
     * @return set of IP addresses
     * @~
     */
    std::set<std::string> local_ips() { return platform::local_ipv4_addresses(); }

    /*!
     * @~japanese
     * @brief IP アドレスを 32 ビットの値にする
     *
     * @param ip IP アドレス
     * @return 値 (正しくなければ std::runtime_error)
     * @~english
     * @brief Converts an IP address into a 32-bit value
     *
     * @param ip IP address
     * @return value (std::runtime_error if invalid)
     * @~
     */
    uint32_t ip_to_u32(const std::string& ip)
    {
      auto v = platform::parse_ipv4(ip);
      if (!v) { throw std::runtime_error("invalid IPv4 address: " + ip); }
      return *v;
    }

    /*!
     * @~japanese
     * @brief 32 ビットの値を IP アドレスにする
     *
     * @param v 値
     * @return IP アドレス
     * @~english
     * @brief Converts a 32-bit value into an IP address
     *
     * @param v value
     * @return IP address
     * @~
     */
    std::string u32_to_ip(uint32_t v) { return platform::format_ipv4(v); }

    /*!
     * @~japanese
     * @brief 文字列の最後の行を返す
     *
     * @param s 文字列
     * @return 最後の行
     * @~english
     * @brief Returns the last line of a string
     *
     * @param s string
     * @return last line
     * @~
     */
    std::string last_line(const std::string& s)
    {
      auto lines = split(s, '\n');
      return lines.empty() ? s : lines.back();
    }
  } // namespace

  // ==================================================================== 構築・設定
  SNMPManager::SNMPManager(RTC::Manager& mgr) : mgr_(mgr)
  {
    orb_ = CORBA::ORB::_duplicate(mgr_.getORB());
    name_ = trim(prop("manager.instance_name", ""));
    if (name_.empty()) { throw std::runtime_error("manager.instance_name is not configured"); }
    Log::set_name(name_);
    snmp_port_ = static_cast<int>(prop_d("snmp.port", 161));
    trap_port_ = static_cast<int>(prop_d("snmp.trap_port", 162));
    corba_port_ = static_cast<int>(prop_d("snmp.manager_corba_port", 2810));
    community_ = prop("snmp.community", "public");
    discovery_mode_ = prop("snmp.discovery_mode", "trap");
    if (discovery_mode_ != "trap" && discovery_mode_ != "sysname") { throw std::runtime_error("snmp.discovery_mode must be trap or sysname"); }
    migration_config_file_ = prop("snmp.migration_config", "migration.json");
    started_at_ = monotonic();
    // 自分のロボットの IP を明示する (1 台の PC で複数のマスターマネージャを動かすとき。127.0.0.11 など)。
    // 書くと、自分の判定・スキャンからの除外・自分のマネージャへの接続・トラップの送り元と待ち受けに、このアドレスを使う
    own_ip_ = trim(prop("snmp.own_ip", ""));
    snmp_ = std::make_unique<SnmpClient>(community_, snmp_port_);
    snmp_->set_local_address(own_ip_);
    std::ofstream("snmpmgr.txt") << "seconds_since_midnight\tinfo\trtc_name\trtc_status\n";
    measure_enabled_ = prop_b("snmp.measure.enabled", false);
    observer_enabled_ = prop_b("snmp.process_observer.enabled", false);
    observer_rtc_enabled_ = prop_b("snmp.observer.enabled", false);
    // 追加の監視の値 (snmp.metrics)。設定が正しくなければ例外 (起動しない)
    custom_metrics_ = metrics::parse_config([this](const std::string& k, const std::string& d) { return prop(k, d); });
    // slave の起動より前に置き換える (起動時の構成で最初の slave が起動される)
    install_process_observer();
  }

  SNMPManager::~SNMPManager() { stop(); }

  std::string SNMPManager::prop(const std::string& key, const std::string& def) const
  {
    std::string v = mgr_.getConfig().getProperty(key, def);
    return trim(v);
  }

  double SNMPManager::prop_d(const std::string& key, double def) const
  {
    auto v = to_double(prop(key, ""));
    return v ? *v : def;
  }

  bool SNMPManager::prop_b(const std::string& key, bool def) const
  {
    std::string v = upper(prop(key, def ? "YES" : "NO"));
    return v == "YES" || v == "TRUE" || v == "1";
  }

  std::vector<std::string> SNMPManager::scan_targets() const
  {
    std::set<uint32_t> out;
    std::string ranges = prop("snmp.ip_ranges", "");
    if (ranges.empty())
      {
        std::string prefix = prop("snmp.ip_mask", "192.168.215.");
        int lo = static_cast<int>(prop_d("snmp.ip_range_min", 104)), hi = static_cast<int>(prop_d("snmp.ip_range_max", 123));
        for (int i = lo; i <= hi; ++i) { out.insert(ip_to_u32(prefix + std::to_string(i))); }
      }
    else
      {
        for (auto& e : split(ranges, ','))
          {
            if (auto d = e.find('-'); d != std::string::npos)
              {
                uint32_t a = ip_to_u32(trim(e.substr(0, d))), b = ip_to_u32(trim(e.substr(d + 1)));
                for (uint32_t v = a; v <= b && v >= a; ++v) { out.insert(v); }
              }
            else if (auto s = e.find('/'); s != std::string::npos)
              {
                uint32_t base = ip_to_u32(e.substr(0, s));
                int bits = std::stoi(e.substr(s + 1));
                uint32_t mask = bits == 0 ? 0 : 0xffffffffu << (32 - bits);
                uint32_t net = base & mask, bc = net | ~mask;
                for (uint32_t v = net + 1; v < bc; ++v) { out.insert(v); }
              }
            else { out.insert(ip_to_u32(e)); }
          }
      }
    std::vector<std::string> ips;
    for (auto v : out) { ips.push_back(u32_to_ip(v)); }
    return ips;
  }

  // ==================================================================== 起動・停止
  bool SNMPManager::init()
  {
    if (running_) { return true; }
    running_ = true;
    traps_ = std::make_unique<TrapReceiver>(trap_port_, [this](const std::string& ip, const SnmpClient::Values& v) { on_trap(ip, v); });
    {
      std::string addr = trim(prop("snmp.trap_address", ""));
      traps_->set_address(addr.empty() ? own_ip_ : addr);
    }
    std::string err;
    if (!traps_->start(err)) { log("SNMP trap receiver initialization failed: " + err); return false; }
    // 構成ファイルを先に読み、プラグインを読み込んでおく (起動直後のイベントも届くように)
    try { load_migration_config(); }
    catch (std::exception& e) { log(std::string("migration config not loaded at init: ") + e.what()); }
    if (prop_b("snmp.db.enabled", false))
      {
        store_ = std::make_unique<Store>(prop("snmp.db.path", "snmpmgr.db"), name_, prop_d("snmp.db.retention_hours", 24));
        std::string e2;
        if (!store_->open(e2)) { log("db open failed: " + e2); store_.reset(); }
        else { event(name_, "started", "db=" + prop("snmp.db.path", "snmpmgr.db")); }
      }

    auto locals = local_ips();
    auto all = scan_targets();
    std::string own;
    if (!own_ip_.empty())
      {
        // 同じ PC のほかのマネージャ (127.0.0.12 など) は自分ではないので、自分の IP だけを自分とする
        locals = {own_ip_};
        own = own_ip_;
      }
    for (auto& ip : all) { if (own.empty() && locals.count(ip)) { own = ip; break; } }
    if (own.empty() && !locals.empty()) { own = *locals.begin(); }
    if (!own.empty())
      {
        self_ = std::make_shared<Robot>();
        self_->name = name_;
        self_->ip = own;
        self_->is_local = true;
        self_->status = "alive";
      }
    for (auto& ip : all) { if (!locals.count(ip)) { targets_.push_back(ip); } }
    {
      std::vector<std::future<bool>> fs;
      for (auto& ip : targets_) { fs.push_back(std::async(std::launch::async, [this, ip] { return check_device(ip); })); }
      for (auto& f : fs) { f.wait(); }
    }
    last_discovery_ = monotonic();
    log("discovered robots: " + robot_summary());

    threads_.emplace_back([this] { self_loop(); });
    threads_.emplace_back([this] { polling_loop(); });
    if (store_) { threads_.emplace_back([this] { store_loop(); }); }
    if (prop_b("snmp.liveness.enabled", true)) { threads_.emplace_back([this] { liveness_loop(); }); }
    if (prop_b("snmp.heartbeat.enabled", true)) { threads_.emplace_back([this] { heartbeat_loop(); }); }
    if (prop_b("snmp.startup_enabled", true)) { threads_.emplace_back([this] { startup_configuration(); }); }
    if (measure_enabled_) { threads_.emplace_back([this] { measure_loop(); }); }
    if (prop_d("snmp.inventory.interval", 3) > 0) { threads_.emplace_back([this] { inventory_loop(); }); }
    threads_.emplace_back([this] { recovery_loop(); });
    return true;
  }

  void SNMPManager::stop()
  {
    if (!running_.exchange(false)) { return; }
    process::set_callback(nullptr);
    wake_recovery();
    if (traps_) { traps_->stop(); }
    for (auto& t : threads_) { if (t.joinable()) { t.join(); } }
    threads_.clear();
    if (store_) { store_->close(); }
  }

  // ==================================================================== ロボット・探索
  std::shared_ptr<Robot> SNMPManager::find_robot(const std::string& name)
  {
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    if (name == "self" || name == name_) { return self_; }
    for (auto& r : robots_) { if (r->name == name) { return r; } }
    return nullptr;
  }

  std::shared_ptr<Robot> SNMPManager::find_robot_by_ip(const std::string& ip)
  {
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    for (auto& r : robots_) { if (r->ip == ip) { return r; } }
    return nullptr;
  }

  bool SNMPManager::register_robot(const std::string& ip, const std::string& name)
  {
    {
      std::lock_guard<std::recursive_mutex> g(robots_mutex_);
      for (auto& r : robots_)
        {
          if (r->ip == ip) { r->name = name; return false; }
        }
      auto r = std::make_shared<Robot>();
      r->name = name;
      r->ip = ip;
      r->registered_at = monotonic();
      r->probes.clear();
      robots_.push_back(r);
    }
    event(name, "discovered", ip);
    return true;
  }

  bool SNMPManager::check_device(const std::string& ip)
  {
    if (discovery_mode_ == "sysname")
      {
        std::string oid = prop("snmp.oid.SysNameOid", "1.3.6.1.2.1.1.5.0");
        auto v = snmp_->get(ip, {oid});
        if (!v || !v->count(oid)) { return false; }
        std::string name = trim(v->at(oid));
        if (name.empty()) { return false; }
        register_robot(ip, name);
        return true;
      }
    std::string oid = prop("snmp.oid.SysDescrOid", "1.3.6.1.2.1.1.1.0");
    auto v = snmp_->get(ip, {oid});
    if (!v) { return false; }
    snmp_->send_trap(ip, trap_port_, kTrapName, "Register Device", kTrapInfo, name_);
    return true;
  }

  void SNMPManager::rediscover()
  {
    double interval = prop_d("snmp.discovery_interval", 0);
    if (interval <= 0 || monotonic() - last_discovery_ < interval) { return; }
    last_discovery_ = monotonic();
    std::vector<std::string> pending;
    for (auto& ip : targets_) { if (!find_robot_by_ip(ip)) { pending.push_back(ip); } }
    if (pending.empty()) { return; }
    std::vector<std::future<bool>> fs;
    for (auto& ip : pending) { fs.push_back(std::async(std::launch::async, [this, ip] { return check_device(ip); })); }
    bool any = false;
    for (auto& f : fs) { any = f.get() || any; }
    if (any && discovery_mode_ == "sysname") { log("discovered robots: " + robot_summary()); }
  }

  std::string SNMPManager::robot_summary()
  {
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    std::string s = name_ + "@" + (self_ ? self_->ip : "?") + "(self)";
    for (auto& r : robots_) { s += ", " + r->name + "@" + r->ip; }
    return s;
  }

  bool SNMPManager::is_alive(const std::string& name)
  {
    auto r = find_robot(name);
    return r && (r->is_local || r->status != "dead");
  }

  // ==================================================================== 監視
  std::string SNMPManager::wifi_oid() const
  {
    std::string iface = prop("snmp.wifi_interface", "wlan0");
    if (prop("snmp.wifi_signal_source", "client") == "client")
      {
        std::string o = prop("snmp.wifi_client_oid", "");
        if (!o.empty()) { return o; }
        if (iface == "wlan0" && prop("snmp.wifi_client_extend_prefix", "").empty()) { return kWirelessLegacy; }
        return extend_output_oid(prop("snmp.wifi_client_extend_prefix", "wifiSignalClient_") + iface);
      }
    std::string o = prop("snmp.wifi_ap_oid", "");
    if (!o.empty()) { return o; }
    return extend_output_oid(prop("snmp.wifi_ap_extend_prefix", "wifiSignalAP_") + iface);
  }

  void SNMPManager::update_cgroup(const std::shared_ptr<Robot>& r)
  {
    if (!prop_b("snmp.cgroup.enabled", false)) { return; }
    std::map<std::string, std::string> oids;
    std::vector<std::string> list;
    for (auto& [k, t] : kCgroupTokens) { oids[k] = extend_output_oid(t); list.push_back(oids[k]); }
    auto v = snmp_->get(r->ip, list);
    if (!v) { return; }
    std::map<std::string, std::vector<std::string>> raw;
    for (auto& [k, o] : oids)
      {
        if (!v->count(o)) { return; }
        raw[k] = split(v->at(o), ' ');
        if (raw[k].empty()) { return; }
      }
    long long cpu_usage, cpu_ts;
    double mem_limit, mem_usage, cpu_limit;
    try
      {
        cpu_usage = std::stoll(raw["cpu_usage"][0]);
        cpu_ts = raw["cpu_usage"].size() > 1 ? std::stoll(raw["cpu_usage"][1]) : static_cast<long long>(monotonic() * 1e9);
        mem_limit = std::stod(raw["mem_limit"][0]);
        mem_usage = std::stod(raw["mem_usage"][0]);
        cpu_limit = std::stod(raw["cpu_limit"][0]);
      }
    catch (...) { return; }
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    // snmpd の extend はキャッシュされるので、リセット前に取られた値は捨てる
    if (raw["cpu_usage"].size() > 1 && cpu_ts < r->cg_ignore_before_ns) { return; }
    // snmpd のキャッシュで同じ値が返った場合 (時刻が進んでいない) は捨てる
    if (r->cg_last_ts >= 0 && cpu_ts > r->cg_last_ts && cpu_usage >= r->cg_last_usage)
      {
        double pct = static_cast<double>(cpu_usage - r->cg_last_usage) / static_cast<double>(cpu_ts - r->cg_last_ts) * 100.0;
        r->cg_cpu_samples.push_back(pct);
        while (r->cg_cpu_samples.size() > static_cast<size_t>(prop_d("snmp.cgroup.cpu_window", 5))) { r->cg_cpu_samples.pop_front(); }
        double sum = 0;
        for (double s : r->cg_cpu_samples) { sum += s; }
        r->cg_cpu_usage_pct = std::round(sum / r->cg_cpu_samples.size() * 100) / 100;
      }
    if (r->cg_last_ts < 0 || cpu_ts > r->cg_last_ts) { r->cg_last_usage = cpu_usage; r->cg_last_ts = cpu_ts; }
    r->cg_mem_limit_mb = std::round(mem_limit / 1048576.0 * 10) / 10;
    r->cg_mem_usage_mb = std::round(mem_usage / 1048576.0 * 10) / 10;
    r->cg_cpu_limit_pct = std::round(cpu_limit / 10.0 * 10) / 10;
    r->cg_valid = true;
  }

  void SNMPManager::set_robot_data(const std::shared_ptr<Robot>& r)
  {
    try { update_cgroup(r); } catch (...) {}
    if (prop_b("snmp.battery.enabled", false))
      {
        // バッテリー残量 (%) を snmpd の extend (snmp.battery.extend) から取る。取れなければ不明
        std::string oid = extend_output_oid(prop("snmp.battery.extend", "batteryLevel"));
        auto v = snmp_->get(r->ip, {oid});
        std::optional<double> pct = v && v->count(oid) ? to_double(v->at(oid)) : std::nullopt;
        std::lock_guard<std::recursive_mutex> g(robots_mutex_);
        r->battery_pct = pct;
      }
    if (r->os == "unknown")
      {
        std::string o = prop("snmp.oid.SysDescrOid", "1.3.6.1.2.1.1.1.0");
        if (auto v = snmp_->get(r->ip, {o}); v && v->count(o)) { r->os = v->at(o); }
      }
    // 値は 1 つずつ使う: エージェントによっては返さない値がある (Windows の Net-SNMP は UCD-SNMP-MIB の
    // CPU・メモリを返さない)。返ってきた値だけを更新し、1 つも取れなければ「データなし」にする
    int ifid = r->ifindex;
    if (ifid <= 0)
      {
        auto idx = snmp_->find_index(r->ip, kIfDescr, prop("snmp.wifi_interface", "wlan0"));
        ifid = idx ? *idx : 0;
      }
    std::string in_oid = std::string(kIfIn) + "." + std::to_string(ifid), out_oid = std::string(kIfOut) + "." + std::to_string(ifid);
    std::string w = wifi_oid();
    std::vector<std::string> oids{kMemTotal, kMemAvail, w};
    for (auto& [n, o] : kCpuRaw) { oids.push_back(o); }
    if (ifid > 0) { oids.push_back(in_oid); oids.push_back(out_oid); }
    for (auto& [n, o] : custom_metrics_) { if (!contains(oids, o)) { oids.push_back(o); } }
    auto v = snmp_->get(r->ip, oids);
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    if (!v)
      {
        r->last_data_update.reset();
        r->ifindex = 0;
        return;
      }
    auto num = [&](const std::string& o) -> std::optional<double> {
      return v->count(o) ? to_double(trim(v->at(o))) : std::nullopt;
    };
    bool got = false;
    double now = monotonic();
    // CPU (UCD-SNMP-MIB ssCpuRaw*): 累計 tick の差分から (1 回目は求めない)。iowait は idle とみなす
    std::map<std::string, long long> counters;
    for (auto& [n, o] : kCpuRaw) { if (auto x = num(o)) { counters[n] = static_cast<long long>(*x); } }
    if (counters.size() == kCpuRaw.size())
      {
        std::optional<double> cpu;
        if (!r->last_cpu.empty())
          {
            double total = 0, idle = 0;
            bool valid = true;
            for (auto& [n, c] : counters)
              {
                long long d = c - r->last_cpu[n];
                if (d < 0) { valid = false; }
                total += d;
                if (n == "idle" || n == "iowait") { idle += d; }
              }
            if (valid && total > 0) { cpu = std::clamp((total - idle) * 100.0 / total, 0.0, 100.0); }
          }
        r->last_cpu = counters;
        r->cpu_usage = cpu;
        got = true;
      }
    else { r->cpu_usage.reset(); }
    // メモリ (memTotalReal / memAvailReal。kB)
    auto mem_total = num(kMemTotal), mem_avail = num(kMemAvail);
    if (mem_total && mem_avail && *mem_total > 0)
      {
        r->memory_usage = (*mem_total - *mem_avail) / *mem_total * 100.0;
        r->mem_total_mb = std::round(*mem_total / 1024.0 * 10) / 10;
        got = true;
      }
    else { r->memory_usage.reset(); }
    // Wi-Fi の電波強度 (dBm)
    if (auto dbm = num(w))
      {
        r->wifi_dbm = *dbm;
        r->wifi_strength = std::clamp((70.0 + *dbm) / 70.0 * 100.0, 0.0, 100.0);
        got = true;
      }
    else { r->wifi_dbm.reset(); r->wifi_strength.reset(); }
    // 通信量 (IF-MIB の ifInOctets / ifOutOctets)
    auto recv = ifid > 0 ? num(in_oid) : std::nullopt, sent = ifid > 0 ? num(out_oid) : std::nullopt;
    if (recv && sent)
      {
        r->ifindex = ifid;
        long long rv = static_cast<long long>(*recv), sv = static_cast<long long>(*sent);
        if (r->last_if_time)
          {
            double el = now - *r->last_if_time;
            long long rd = rv - r->last_if_recv, sd = sv - r->last_if_send;
            if (el > 0 && rd >= 0 && sd >= 0) { r->network_in = rd * 8.0 / el / 1e6; r->network_out = sd * 8.0 / el / 1e6; }
          }
        r->last_if_recv = rv;
        r->last_if_send = sv;
        r->last_if_time = now;
        got = true;
      }
    else
      {
        r->ifindex = 0;   // 次の回にインタフェースを探し直す
        r->network_in.reset();
        r->network_out.reset();
        r->last_if_time.reset();
      }
    // 追加の監視の値 (snmp.metrics)
    for (auto& [n, o] : custom_metrics_)
      {
        auto x = v->count(o) ? metrics::parse_value(v->at(o)) : std::nullopt;
        r->metrics[n] = x;
        if (x) { got = true; }
      }
    if (!got)
      {
        r->last_data_update.reset();
        return;
      }
    r->last_data_update = now;
    r->last_seen = now;   // ポーリングの成功も応答があった証拠にする
  }

  void SNMPManager::polling_loop()
  {
    bool liveness = prop_b("snmp.liveness.enabled", true);
    while (running_)
      {
        try { rediscover(); } catch (std::exception& e) { log(std::string("Rediscovery failed: ") + e.what()); }
        std::vector<std::shared_ptr<Robot>> rs;
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          rs = robots_;
        }
        for (auto& r : rs)
          {
            if (liveness && r->status == "dead") { continue; }   // 停止中は飛ばす (復帰はプローブで検知)
            set_robot_data(r);
          }
        std::this_thread::sleep_for(std::chrono::duration<double>(prop_d("snmp.get_interval", 3.0)));
      }
  }

  void SNMPManager::self_loop()
  {
    int count = 0;
    while (running_)
      {
        double interval = prop_d("snmp.trap_interval", 3.0);
        if (!self_) { std::this_thread::sleep_for(std::chrono::duration<double>(interval)); continue; }
        set_robot_data(self_);
        std::string dbm = self_->last_data_update && self_->wifi_dbm ? fmt(*self_->wifi_dbm) : "None";
        if (!last_logged_dbm_ || *last_logged_dbm_ != dbm)
          {
            log("wifi_signal_dbm=" + dbm + " cpu=" + (self_->cpu_usage ? fmt(*self_->cpu_usage) : "None") +
                " mem=" + (self_->memory_usage ? fmt(*self_->memory_usage) : "None"));
            last_logged_dbm_ = dbm;
          }
        check_migration_conditions();
        ++count;
        // 旧来の WiFi Low トラップとシャットダウン
        int trap_count = static_cast<int>(prop_d("snmp.trap_count", -1));
        bool low = self_->last_data_update && self_->wifi_strength && *self_->wifi_strength < prop_d("snmp.wifi_strength_min", 45);
        if (low || (trap_count > 0 && trap_count <= count))
          {
            if (!wifi_low_) { wifi_low_ = true; wifi_low_time_ = monotonic(); }
            else if (monotonic() - wifi_low_time_ > prop_d("snmp.wifi_low_limit", 6))
              {
                if (!shutdown_started_ && prop_b("snmp.shutdown_on_wifi_low", false))
                  {
                    logfile(fmt(wallclock(), 6) + "\tMigrationStart\t\t");
                    shutdown_started_ = true;
                    std::thread([this] { shutdown_own_managers(); }).detach();
                  }
                snmp_->send_trap(prop("snmp.main_robot", "192.168.215.137"), trap_port_, kTrapName, "WiFi Low", kTrapInfo, name_);
              }
          }
        std::this_thread::sleep_for(std::chrono::duration<double>(interval));
      }
  }

  void SNMPManager::shutdown_own_managers()
  {
    try
      {
        RTM::Manager_var master = rtcops::resolve_manager(orb_, own_ip_.empty() ? "localhost" : own_ip_, corba_port_, name_);
        RTM::ManagerList_var slaves = master->get_slave_managers();
        for (CORBA::ULong i = 0; i < slaves->length(); ++i)
          {
            try { slaves[i]->shutdown(); } catch (CORBA::SystemException&) {}
          }
        master->shutdown();
      }
    catch (std::exception& e) { log(std::string("Manager shutdown failed: ") + e.what()); shutdown_started_ = false; }
  }

  void SNMPManager::on_trap(const std::string& ip, const SnmpClient::Values& values)
  {
    auto it = values.find(kTrapName);
    if (it == values.end()) { return; }
    const std::string& msg = it->second;
    if (msg == "Register Device" || msg == "Register Device Reply")
      {
        if (discovery_mode_ != "trap") { return; }
        auto info = values.find(kTrapInfo);
        if (info != values.end() && !trim(info->second).empty())
          {
            register_robot(ip, trim(info->second));
            if (msg == "Register Device")
              {
                std::thread([this, ip] { snmp_->send_trap(ip, trap_port_, kTrapName, "Register Device Reply", kTrapInfo, name_); }).detach();
              }
          }
      }
    else if (msg == "Heartbeat")
      {
        if (auto r = find_robot_by_ip(ip))
          {
            std::lock_guard<std::recursive_mutex> g(robots_mutex_);
            r->last_heartbeat = monotonic();
            update_status(r, monotonic());
          }
      }
    else if (msg == "WiFi Low")
      {
        std::thread([this] { check_migration_conditions(); }).detach();
      }
    else if (msg == "Slave Exited")
      {
        // ほかのロボットの slave の異常終了 ("<ロボット>|<RTC,...>")。自動復旧を待たずに始める
        auto info = values.find(kTrapInfo);
        std::string v = info != values.end() ? info->second : "";
        auto bar = v.find('|');
        std::string robot = bar == std::string::npos ? v : v.substr(0, bar);
        std::string rtcs = bar == std::string::npos ? "" : v.substr(bar + 1);
        log("slave of " + (robot.empty() ? ip : robot) + " exited abnormally: " + (rtcs.empty() ? "-" : rtcs));
        mark_crashed(robot.empty() ? ip : robot, split(rtcs, ','));
      }
    else if (msg == "RTC Error")
      {
        // ほかのロボットの RTC の ERROR (コンポーネントオブザーバーの通知。"<ロボット>|<インスタンス>")
        auto info = values.find(kTrapInfo);
        std::string v = info != values.end() ? info->second : "";
        auto bar = v.find('|');
        std::string robot = bar == std::string::npos ? ip : v.substr(0, bar);
        std::string inst = bar == std::string::npos ? "" : v.substr(bar + 1);
        if (!inst.empty())
          {
            log("RTC " + inst + " on " + robot + " reported ERROR (component observer)");
            mark_error(robot, inst);
          }
      }
    else if (msg == "Startup Query") { on_startup_query(ip); }
    else if (msg == "Startup Pending") { on_startup_pending(ip); }
    else if (msg == "Startup Completed")
      {
        auto info = values.find(kTrapInfo);
        on_startup_notice(ip, info != values.end() ? trim(info->second) : "");
      }
  }

  // ==================================================================== 死活監視・通信品質
  void SNMPManager::record_probe(const std::shared_ptr<Robot>& r, bool ok, double rtt, double now)
  {
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    size_t window = static_cast<size_t>(prop_d("snmp.liveness.window", 20));
    r->probes.emplace_back(ok, ok ? rtt : 0.0);
    while (r->probes.size() > window) { r->probes.pop_front(); }
    std::vector<double> rtts;
    for (auto& [o, t] : r->probes) { if (o) { rtts.push_back(t); } }
    r->packet_loss = std::round(1000.0 * (1.0 - static_cast<double>(rtts.size()) / r->probes.size())) / 10.0;
    if (!rtts.empty())
      {
        double sum = 0;
        for (double t : rtts) { sum += t; }
        r->latency_ms = std::round(sum / rtts.size() * 1000) / 1000;
        // ジッタ: 連続して成功した往復時間どうしの差の絶対値の平均
        double dsum = 0;
        for (size_t i = 1; i < rtts.size(); ++i) { dsum += std::fabs(rtts[i] - rtts[i - 1]); }
        r->jitter_ms = rtts.size() > 1 ? std::round(dsum / (rtts.size() - 1) * 1000) / 1000 : 0.0;
      }
    else { r->latency_ms.reset(); r->jitter_ms.reset(); }
    if (ok) { r->last_seen = now; }
    update_status(r, now);
  }

  void SNMPManager::update_status(const std::shared_ptr<Robot>& r, double now)
  {
    if (r->is_local) { return; }
    double timeout = prop_d("snmp.liveness.timeout", 6.0);
    std::optional<double> evidence;
    for (auto& t : {r->last_seen, r->last_heartbeat}) { if (t && (!evidence || *t > *evidence)) { evidence = t; } }
    double since = now - (evidence ? *evidence : r->registered_at);
    std::string next = (evidence && since <= timeout) ? "alive" : (since > timeout ? "dead" : r->status);
    if (next == r->status) { return; }
    std::string old = r->status;
    r->status = next;
    if (next == "dead") { event(r->name, "down", "no response for " + fmt(since) + "s (was " + old + ")"); }
    else if (old == "dead") { event(r->name, "up", "responding again"); }
    else { event(r->name, "alive", "first response"); }
  }

  void SNMPManager::liveness_loop()
  {
    double timeout = prop_d("snmp.liveness.probe_timeout", 1.0);
    while (running_)
      {
        double start = monotonic();
        std::vector<std::shared_ptr<Robot>> rs;
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          rs = robots_;
        }
        std::vector<std::future<std::pair<bool, double>>> fs;
        for (auto& r : rs)
          {
            std::string ip = r->ip;
            fs.push_back(std::async(std::launch::async, [this, ip, timeout] { return snmp_->probe(ip, kSysUpTime, timeout); }));
          }
        for (size_t i = 0; i < rs.size(); ++i)
          {
            auto res = fs[i].get();
            record_probe(rs[i], res.first, res.second, monotonic());
          }
        double rest = prop_d("snmp.liveness.probe_interval", 1.0) - (monotonic() - start);
        std::this_thread::sleep_for(std::chrono::duration<double>(std::max(0.05, rest)));
      }
  }

  void SNMPManager::heartbeat_loop()
  {
    while (running_)
      {
        std::vector<std::string> ips;
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          for (auto& r : robots_) { ips.push_back(r->ip); }
        }
        for (auto& ip : ips) { snmp_->send_trap(ip, trap_port_, kTrapName, "Heartbeat", kTrapInfo, name_); }
        std::this_thread::sleep_for(std::chrono::duration<double>(prop_d("snmp.heartbeat.interval", 2.0)));
      }
  }

  // ==================================================================== イベント・保存
  MetricRow SNMPManager::snapshot(const Robot& r) const
  {
    MetricRow m;
    m.robot = r.name; m.ip = r.ip; m.status = r.status;
    m.cpu_usage = r.cpu_usage; m.memory_usage = r.memory_usage; m.network_in = r.network_in; m.network_out = r.network_out;
    m.wifi_signal_dbm = r.wifi_dbm; m.latency_ms = r.latency_ms; m.jitter_ms = r.jitter_ms; m.packet_loss = r.packet_loss;
    if (r.cg_valid)
      {
        m.cg_cpu_usage = r.cg_cpu_usage_pct; m.cg_cpu_limit = r.cg_cpu_limit_pct;
        m.cg_mem_usage_mb = r.cg_mem_usage_mb; m.cg_mem_limit_mb = r.cg_mem_limit_mb;
      }
    return m;
  }

  RobotInfo SNMPManager::info(const Robot& r) const
  {
    RobotInfo i;
    i.name = r.name; i.ip = r.ip; i.status = r.status; i.is_local = r.is_local;
    i.cpu_usage = r.cpu_usage; i.memory_usage = r.memory_usage; i.network_in = r.network_in; i.network_out = r.network_out;
    i.wifi_signal_dbm = r.wifi_dbm; i.latency_ms = r.latency_ms; i.jitter_ms = r.jitter_ms; i.packet_loss = r.packet_loss;
    if (r.cg_valid)
      {
        i.cg_cpu_usage_pct = r.cg_cpu_usage_pct; i.cg_cpu_limit_pct = r.cg_cpu_limit_pct;
        i.cg_mem_usage_mb = r.cg_mem_usage_mb; i.cg_mem_limit_mb = r.cg_mem_limit_mb;
      }
    i.battery_pct = r.battery_pct;
    i.inventory = r.inventory;
    i.base_mem_mb = r.base_mem_mb;
    i.metrics = r.metrics;
    return i;
  }

  void SNMPManager::store_loop()
  {
    while (running_)
      {
        std::this_thread::sleep_for(std::chrono::duration<double>(prop_d("snmp.db.interval", 5)));
        std::vector<MetricRow> rows;
        {
          std::lock_guard<std::recursive_mutex> g(robots_mutex_);
          if (self_) { rows.push_back(snapshot(*self_)); }
          for (auto& r : robots_) { rows.push_back(snapshot(*r)); }
        }
        store_->record_metrics(rows);
        if (!custom_metrics_.empty())
          {
            std::vector<CustomMetricRow> cs;
            {
              std::lock_guard<std::recursive_mutex> g(robots_mutex_);
              std::vector<std::shared_ptr<Robot>> all = robots_;
              if (self_) { all.insert(all.begin(), self_); }
              for (auto& r : all) { for (auto& [n, v] : r->metrics) { cs.push_back({r->name, n, v}); } }
            }
            store_->record_custom_metrics(cs);
          }
      }
  }

  std::vector<std::string> SNMPManager::custom_names() const
  {
    std::vector<std::string> out;
    for (auto& [n, o] : custom_metrics_) { out.push_back(n); }
    return out;
  }

  void SNMPManager::event(const std::string& robot, const std::string& kind, const std::string& detail)
  {
    log(trim("event " + kind + ": " + (robot.empty() ? "-" : robot) + " " + detail));
    if (store_) { store_->record_event(robot, kind, detail); }
    if (plugins_) { plugins_->publish(Event{wallclock(), name_, robot, kind, detail}); }
  }

  void SNMPManager::write_status(const json& status)
  {
    std::ofstream f(prop("snmp.startup_status_file", "startup_status.json"));
    if (f) { f << status.dump(1) << "\n"; }
  }

  void SNMPManager::logfile(const std::string& line)
  {
    std::lock_guard<std::mutex> g(logfile_mutex_);
    std::ofstream("snmpmgr.txt", std::ios::app) << line << "\n";
  }

  void SNMPManager::trap(const std::string& ip, const std::string& message, const std::string& info)
  {
    snmp_->send_trap(ip, trap_port_, kTrapName, message, kTrapInfo, info.empty() ? name_ : info);
  }

  std::vector<std::string> SNMPManager::peer_ips(bool alive_only)
  {
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    std::vector<std::string> ips;
    for (auto& r : robots_) { if (!alive_only || r->status != "dead") { ips.push_back(r->ip); } }
    return ips;
  }

  // ==================================================================== 構成ファイル
  PluginManager* SNMPManager::plugins()
  {
    if (!plugins_)
      {
        // 既定: 実行ファイルの隣の plugins/ と、インストールしたプラグインの場所 (CMake の SNMPMGR_INSTALLED_PLUGIN_DIR)
        std::string defaults = platform::executable_dir() + "/plugins";
#ifdef SNMPMGR_INSTALLED_PLUGIN_DIR
        defaults += std::string(",") + SNMPMGR_INSTALLED_PLUGIN_DIR;
#endif
        plugins_ = std::make_unique<PluginManager>(*this, split(prop("snmp.plugin.path", defaults), ','));
      }
    return plugins_.get();
  }

  json SNMPManager::load_migration_config()
  {
    std::ifstream f(migration_config_file_);
    if (!f) { throw std::runtime_error("cannot open " + migration_config_file_); }
    json cfg = json::parse(f);
    if (!cfg.is_object()) { throw std::runtime_error("Migration configuration must be a JSON object"); }
    json managers = cfg.value("managers", json::object());
    if (!managers.is_object()) { throw std::runtime_error("managers has an invalid type"); }
    auto* pm = plugins();
    if (pm->load_all(cfg.value("plugins", json::array()))) { log(pm->describe()); }
    for (const char* section : {"startup", "migration"})
      {
        if (!cfg.contains(section)) { continue; }
        const json& plan = cfg[section];
        if (!plan.is_object()) { throw std::runtime_error(std::string(section) + " must be an object"); }
        if (plan.contains("plugin") && !plan["plugin"].is_null())
          {
            if (!pm->has_action(plan["plugin"])) { throw std::runtime_error(std::string(section) + ": unknown action plugin " + plan["plugin"].get<std::string>()); }
            continue;
          }
        if (plan.contains("placement_plugin") && !pm->has_placement(plan["placement_plugin"]))
          {
            throw std::runtime_error(std::string(section) + ": unknown placement plugin " + plan["placement_plugin"].get<std::string>());
          }
        validate_plan(plan, managers, section);
      }
    validate_plan(json{{"components", cfg.value("components", json::array())},
                       {"connections", cfg.value("connections", json::array())}}, managers, "legacy");
    if (cfg.contains("migration_conditions")) { validate_conditions(cfg["migration_conditions"]); }
    validate_extensions(cfg, managers);
    return cfg;
  }

  void SNMPManager::validate_plan(const json& plan, const json& managers, const std::string& label)
  {
    std::string mode = plan.value("mode", "manual");
    if (mode != "manual" && mode != "auto") { throw std::runtime_error(label + ": mode must be manual or auto"); }
    std::set<std::string> names;
    for (auto& e : plan.value("components", json::array()))
      {
        if (!e.is_object() || !e.contains("name") || (mode == "manual" && !e.contains("manager")))
          {
            throw std::runtime_error(label + ": component needs name and manager");
          }
        std::string n = e["name"];
        if (!names.insert(n).second) { throw std::runtime_error(label + ": duplicate component " + n); }
        if (e.contains("manager") && !managers.contains(e["manager"].get<std::string>())) { throw std::runtime_error(label + ": unknown manager " + e["manager"].get<std::string>()); }
        bool c = e.contains("create") && !e["create"].get<std::string>().empty();
        bool x = e.contains("existing") && !e["existing"].get<std::string>().empty();
        if (c == x) { throw std::runtime_error(label + ": specify exactly one of create or existing"); }
        // auto でも、動かさない既存の RTC は manager を書いて計画に含められる (通信量の計算に使う)
        if (mode == "auto" && !c && !e.contains("manager"))
          {
            throw std::runtime_error(label + ": auto mode needs create, or existing with manager, for " + n);
          }
      }
    if (mode == "auto" && plan.contains("primary") && !managers.contains(plan["primary"].get<std::string>()))
      {
        throw std::runtime_error(label + ": unknown primary manager " + plan["primary"].get<std::string>());
      }
    if (plan.contains("executor") && (!plan["executor"].is_string() || plan["executor"].get<std::string>().empty()))
      {
        throw std::runtime_error(label + ": executor must be a manager name or auto");
      }
    for (auto& e : plan.value("connections", json::array()))
      {
        if (!e.contains("name")) { throw std::runtime_error(label + ": invalid connection"); }
        for (const char* end : {"source", "target"})
          {
            if (!e.contains(end) || !names.count(e[end].value("component", "")) || e[end].value("port", "").empty())
              {
                throw std::runtime_error(label + ": invalid connection endpoint in " + e["name"].get<std::string>());
              }
          }
      }
    for (auto& e : plan.value("remove", json::array()))
      {
        if (!e.contains("name") || !e.contains("manager") || !managers.contains(e["manager"].get<std::string>()))
          {
            throw std::runtime_error(label + ": remove entry needs name and known manager");
          }
      }
  }

  void SNMPManager::validate_conditions(const json& g)
  {
    std::string op = g.value("operator", "");
    if (op != "AND" && op != "OR") { throw std::runtime_error("Condition group must use AND or OR"); }
    if (!g.contains("conditions") || !g["conditions"].is_array() || g["conditions"].empty()) { throw std::runtime_error("Condition group requires a nonempty conditions list"); }
    for (auto& c : g["conditions"])
      {
        if (c.contains("conditions")) { validate_conditions(c); continue; }
        if (c.contains("plugin"))
          {
            if (!plugins()->has_condition(c["plugin"])) { throw std::runtime_error("Unknown condition plugin: " + c["plugin"].get<std::string>()); }
            continue;
          }
        std::string m = c.value("metric", "");
        if (!contains(kBaseMetrics, m) && !contains(kLivenessMetrics, m) && !contains(custom_names(), m))
          {
            throw std::runtime_error("Unknown metric: " + m);
          }
        static const std::vector<std::string> ops{"<", "<=", ">", ">=", "==", "!="};
        if (!contains(ops, c.value("operator", ""))) { throw std::runtime_error("Unknown comparison operator"); }
        if (!c.contains("value") || !c["value"].is_number()) { throw std::runtime_error("condition value must be a number"); }
      }
    if (g.value("duration_seconds", 0.0) < 0 || g.value("cooldown_seconds", 0.0) < 0) { throw std::runtime_error("Duration and cooldown must be nonnegative"); }
  }

  // ==================================================================== 構成の適用
  RTM::Manager_var SNMPManager::resolve(const std::string& robot)
  {
    auto r = find_robot(robot);
    if (!r) { throw std::runtime_error("unknown robot " + robot); }
    return rtcops::resolve_manager(orb_, r->ip, corba_port_, r->name);
  }

  std::string SNMPManager::qualify_create(const std::string& create, const std::string& language) const
  {
    // C++ のマネージャは言語の指定がない create を C++ とみなすので、言語を補う
    std::string lang = language.empty() ? prop("snmp.create.language", "") : language;
    std::string base = create.substr(0, create.find('?'));
    if (lang.empty() || base.find(':') != std::string::npos) { return create; }
    std::string params = create.find('?') == std::string::npos ? "" : create.substr(create.find('?'));
    return "RTC:::" + base + ":" + lang + ":" + (params.empty() ? "" : params);
  }

  void SNMPManager::startup_failed(const std::string& message, const std::vector<std::string>& reasons)
  {
    log("SYSTEM STARTUP FAILED: " + message);
    for (auto& r : reasons) { log("  " + r); }
    write_status(json{{"status", "failed"}, {"time", iso_now()}, {"error", message}, {"reasons", reasons}});
    event(name_, "startup_failed", message);
  }

  PlacementResult SNMPManager::plugin_placement(const json& plan, const std::vector<PlacementItem>& items,
                                                const std::vector<PlacementRobot>& robots,
                                                const std::vector<PlacementEdge>& edges, const std::string& primary)
  {
    std::string name = plan["placement_plugin"];
    std::map<std::string, std::string> assign;
    try { assign = plugins()->placement(name, items, robots, edges, primary, plan.value("placement_args", json::object())); }
    catch (PlacementError&) { throw; }
    // .so 側で投げた PlacementError は型情報の比較で一致しないことがあるので、一般の例外も配置の失敗として扱う
    catch (std::exception& e) { throw PlacementError(e.what()); }
    std::vector<std::string> reasons;
    for (auto& it : items)
      {
        auto a = assign.find(it.name);
        if (a == assign.end()) { reasons.push_back(it.name + " が割り当てられていない"); }
        else if (!contains(it.candidates, a->second)) { reasons.push_back(it.name + " を置けないロボット " + a->second + " に割り当てた"); }
      }
    if (!reasons.empty()) { throw PlacementError("placement plugin " + name + " の結果が不正です", reasons); }
    auto r = evaluate(assign, items, robots, edges, primary);
    if (!r) { throw PlacementError("placement plugin " + name + " の結果はリソースの制限を超えます", capacity_report(items, robots)); }
    log("placement by plugin " + name);
    return *r;
  }

  // ==================================================================== マイグレーション条件
  std::optional<double> SNMPManager::metric(const Robot& r, const std::string& n) const
  {
    if (n == "cpu_usage") { return r.cpu_usage; }
    if (n == "memory_usage") { return r.memory_usage; }
    if (n == "network_in") { return r.network_in; }
    if (n == "network_out") { return r.network_out; }
    if (n == "wifi_signal_dbm") { return r.wifi_dbm; }
    if (n == "wifi_strength") { return r.wifi_strength; }
    bool client = prop("snmp.wifi_signal_source", "client") == "client";
    if (n == "wifi_signal_client_dbm") { return client ? r.wifi_dbm : std::nullopt; }
    if (n == "wifi_signal_ap_dbm") { return client ? std::nullopt : r.wifi_dbm; }
    if (n == "latency_ms") { return r.latency_ms; }
    if (n == "jitter_ms") { return r.jitter_ms; }
    if (n == "packet_loss") { return r.packet_loss; }
    if (n == "battery_pct") { return r.battery_pct; }
    if (auto it = r.metrics.find(n); it != r.metrics.end()) { return it->second; }   // 追加の監視の値
    if (n == "alive") { return r.is_local ? 1.0 : (r.status == "alive" ? 1.0 : (r.status == "dead" ? std::optional<double>(0.0) : std::nullopt)); }
    return std::nullopt;
  }

  bool SNMPManager::evaluate_condition(const json& c, double now)
  {
    if (c.contains("plugin"))
      {
        try { return plugins()->condition(c["plugin"], c.value("args", json::object()), now); }
        catch (std::exception& e) { log(std::string("condition plugin failed: ") + e.what()); return false; }
      }
    auto r = find_robot(c.value("robot", "self"));
    if (!r) { return false; }
    std::string m = c["metric"];
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    if (!contains(kLivenessMetrics, m))
      {
        // 死活・通信品質以外は、新しいサンプルがあるときだけ判定する
        if (!r->last_data_update) { return false; }
        double max_age = prop_d("snmp.condition_max_data_age", 10);
        if (max_age >= 0 && now - *r->last_data_update > max_age) { return false; }
      }
    auto v = metric(*r, m);
    if (!v) { return false; }
    double x = c["value"].get<double>();
    std::string op = c["operator"];
    if (op == "<") { return *v < x; }
    if (op == "<=") { return *v <= x; }
    if (op == ">") { return *v > x; }
    if (op == ">=") { return *v >= x; }
    if (op == "==") { return *v == x; }
    return *v != x;
  }

  bool SNMPManager::evaluate_conditions(const json& g, double now)
  {
    bool is_and = g["operator"] == "AND";
    bool result = is_and;
    for (auto& c : g["conditions"])
      {
        bool v = c.contains("conditions") ? evaluate_conditions(c, now) : evaluate_condition(c, now);
        result = is_and ? (result && v) : (result || v);
      }
    return result;
  }

  void SNMPManager::check_migration_conditions()
  {
    try
      {
        json cfg = load_migration_config();
        if (!startup_completed_ || !cfg.contains("migration_conditions")) { return; }
        json g = cfg["migration_conditions"];
        if (!g.value("enabled", true)) { return; }
        json mig = cfg.value("migration", json());
        // mode: auto の移動先を決める get_loadable_modules() を先に取っておく (C++ のマスターでは数秒かかる)。
        // 実行ロボットでなくても取る (実行ロボットが止まって引き継いだとき、すぐ移せるように)
        if (mig.is_object() && mig.value("mode", "manual") == "auto" && !loadable_prefetched_.exchange(true)) { prefetch_loadable(cfg); }
        if (!is_executor(cfg, {&g, &mig}, std::nullopt, "migration")) { return; }
        if (migration_done_ && !g.value("repeat", true)) { return; }
        double now = monotonic();
        {
          std::lock_guard<std::mutex> lk(condition_mutex_);
          if (!evaluate_conditions(g, now))
            {
              if (condition_first_true_) { log("migration conditions no longer satisfied"); }
              condition_first_true_.reset();
              return;
            }
          double duration = g.value("duration_seconds", 0.0);
          if (!condition_first_true_)
            {
              log("migration conditions satisfied; waiting " + fmt(duration) + "s before migrating");
              condition_first_true_ = now;
            }
          if (now - *condition_first_true_ < duration) { return; }
          double cooldown = g.value("cooldown_seconds", 60.0);
          if (last_migration_attempt_ && now - *last_migration_attempt_ < cooldown) { return; }
          {
            std::lock_guard<std::mutex> mg(migration_mutex_);
            if (migrating_) { return; }
          }
          // 配置の計算からまとめて 1 回だけ (自動復旧とも重ねない)
          if (migration_in_progress_.exchange(true)) { return; }
          last_migration_attempt_ = now;
        }
        // マイグレーションは別スレッドで (ポーリングやトラップ受信を止めない)
        std::thread([this] {
          run_migration_from_conditions();
          migration_in_progress_ = false;
        }).detach();
      }
    catch (std::exception& e) { log(std::string("Migration condition check failed: ") + e.what()); }
  }

  void SNMPManager::run_migration_from_conditions()
  {
    try
      {
        log("migration started");
        event(name_, "migration_started");
        if (migration())
          {
            migration_done_ = true;
            log("migration completed");
            event(name_, "migration_completed");
          }
      }
    catch (std::exception& e)
      {
        log(std::string("migration failed: ") + e.what());
        event(name_, "migration_failed", last_line(e.what()));
      }
    catch (CORBA::SystemException& e)
      {
        log(std::string("migration failed: CORBA ") + e._name());
        event(name_, "migration_failed", std::string("CORBA ") + e._name());
      }
  }

  // ==================================================================== PluginHost
  void SNMPManager::host_log(const std::string& m) { log(m); }

  std::vector<RobotInfo> SNMPManager::host_robots()
  {
    std::lock_guard<std::recursive_mutex> g(robots_mutex_);
    std::vector<RobotInfo> out;
    if (self_) { out.push_back(info(*self_)); }
    for (auto& r : robots_) { out.push_back(info(*r)); }
    return out;
  }

  void* SNMPManager::host_manager_object(const std::string& robot)
  {
    RTM::Manager_var m = resolve(robot);
    return RTM::Manager::_duplicate(m.in());   // 呼び出し側で CORBA::release する
  }

  bool SNMPManager::host_has_component(const std::string& instance, const std::string& robot)
  {
    RTM::Manager_var m = resolve(robot);
    RTC::RTObject_var rtc = rtcops::find(m, instance);
    return !CORBA::is_nil(rtc);
  }

  void SNMPManager::host_apply_plan(const json& plan, const std::string& label)
  {
    json cfg = load_migration_config();
    validate_plan(plan, cfg["managers"], label);
    apply_plan(cfg, plan, label);
  }
  // ==================================================================== 実測値の計測と保存
  std::string SNMPManager::profile_roots() const
  {
    std::string root = prop("snmp.placement.profile_dir", "placement");
    if (measure_enabled_ && prop_b("snmp.measure.use_for_placement", true)) { root += "," + prop("snmp.measure.dir", "measured"); }
    return root;
  }

  void SNMPManager::measure_loop()
  {
    double interval = prop_d("snmp.measure.save_interval", 10);
    double next = monotonic() + interval;
    while (running_)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (monotonic() < next) { continue; }
        next = monotonic() + interval;
        try { collect_measurements(); }
        catch (std::exception& e) { log(std::string("measurement save failed: ") + e.what()); }
      }
  }

  std::vector<std::string> SNMPManager::collect_measurements()
  {
    namespace fs = std::filesystem;
    fs::path own = fs::path(prop("snmp.measure.dir", "measured")) / measure::safe_name(name_);
    fs::path runs_path = own / "runs.json";
    if (!measure_runs_loaded_)
      {
        measure_runs_loaded_ = true;
        std::ifstream f(runs_path);
        try { if (f) { f >> measure_runs_; } }
        catch (...) {}
        if (!measure_runs_.is_object()) { measure_runs_ = json::object(); }
      }
    std::vector<std::string> changed;
    std::error_code ec;
    std::vector<fs::path> files;
    for (auto& e : fs::directory_iterator(prop("snmp.measure.runtime_dir", "/run/rtm/measure"), ec))
      {
        if (e.path().extension() == ".json") { files.push_back(e.path()); }
      }
    std::sort(files.begin(), files.end());
    for (auto& p : files)
      {
        try
          {
            std::ifstream f(p);
            json data;
            f >> data;
            for (auto& k : measure::update_runs(measure_runs_, name_, data)) { changed.push_back(k); }
          }
        catch (...) {}
      }
    if (changed.empty()) { return {}; }
    fs::create_directories(own, ec);
    {
      std::ofstream f(runs_path.string() + ".tmp");
      f << measure_runs_.dump(1) << "\n";
    }
    fs::rename(runs_path.string() + ".tmp", runs_path, ec);
    if (store_)
      {
        std::vector<MeasurementRow> rows;
        for (auto& k : changed)
          {
            const json& r = measure_runs_[k];
            auto opt = [](const json& j, const char* key) -> std::optional<double> {
              if (j.is_object() && j.contains(key) && j[key].is_number()) { return j[key].get<double>(); }
              return std::nullopt;
            };
            MeasurementRow row;
            row.robot = r.value("robot", "");
            row.manager = r.value("manager", "");
            row.instance = r.value("instance", "");
            row.type = r.value("type", "");
            row.language = r.value("language", "");
            row.samples = opt(r, "samples");
            json cpu = r.value("cpu", json::object()), mem = r.value("memory_mb", json::object());
            row.cpu_avg = opt(cpu, "avg"); row.cpu_max = opt(cpu, "max"); row.cpu_p95 = opt(cpu, "p95");
            row.mem_avg_mb = opt(mem, "avg"); row.mem_max_mb = opt(mem, "max");
            row.ports_json = r.value("ports", json::object()).dump();
            rows.push_back(row);
          }
        store_->record_measurements(rows);
      }
    auto groups = measure::aggregate(measure_runs_, static_cast<int>(prop_d("snmp.measure.min_samples", 10)),
                                     static_cast<int>(prop_d("snmp.measure.max_runs", 20)),
                                     prop("snmp.measure.cpu_stat", "max"));
    std::set<std::pair<std::string, std::string>> touched;
    for (auto& k : changed)
      {
        const json& r = measure_runs_[k];
        std::string lang = r.value("language", "");
        touched.insert({lang.empty() ? "Python" : lang, r.value("type", "")});
      }
    std::vector<std::string> saved;
    std::string env = prop("snmp.placement.environment", "");
    for (auto& key : touched)
      {
        auto g = groups.find(key);
        if (g == groups.end()) { continue; }   // 標本がまだ少ない
        const json& agg = g->second;
        fs::path d = own / measure::safe_name(key.first) / measure::safe_name(key.second);
        fs::create_directories(d, ec);
        fs::path path = d / "placement.xml";
        {
          std::ofstream f(path.string() + ".tmp");
          f << measure::render_placement(agg, env, name_);
        }
        fs::rename(path.string() + ".tmp", path, ec);
        std::pair<double, double> value{std::round(agg["cpu_value"].get<double>() * 10) / 10,
                                        std::round(agg["memory_mb"]["max"].get<double>() * 10) / 10};
        auto old = measure_saved_.find(key);
        if (old == measure_saved_.end() || old->second != value)
          {
            bool first = old == measure_saved_.end();
            measure_saved_[key] = value;
            std::string ports;
            for (auto& [n, p] : agg["ports"].items())
              {
                if (p.value("msgs", 0LL) <= 0) { continue; }
                ports += (ports.empty() ? "" : ", ") + n + " " + fmt(p.value("bytes_per_msg", 0.0)) + "B x " +
                         fmt(p.value("hz", 0.0), 3) + "Hz";
              }
            log("measured " + key.second + " (" + key.first + "): cpu " + fmt(value.first) + "% (" +
                agg.value("cpu_stat", std::string("max")) + "), memory " + fmt(value.second) + "MB, " +
                std::to_string(agg["samples"].get<long long>()) + " samples" + (ports.empty() ? "" : ", ports " + ports));
            if (first)
              {
                event(name_, "measurement_saved", key.second + " cpu=" + fmt(value.first) + " mem=" + fmt(value.second) +
                                                      "MB -> " + path.string());
              }
          }
        saved.push_back(path.string());
      }
    return saved;
  }

  // ==================================================================== slave マネージャの終了の検知
  void SNMPManager::install_process_observer()
  {
    if (!observer_enabled_) { return; }
    process::set_callback([this](int code, const process::ExitInfo& info) { on_slave_exit(code, info.manager_name, info.pid); });
    log(process::replaces_launch()
            ? "process observer: slaves are launched and waited by SNMPManager (coil::launch_shell replaced)"
            : "process observer: slaves found in the RTC list are watched by their pid (manager.pid)");
  }

  void SNMPManager::on_slave_exit(int code, const std::string& slave, int pid)
  {
    // どの RTC があったかは RTC の一覧 (slave の名前) から求める
    std::vector<std::string> rtcs;
    {
      std::lock_guard<std::recursive_mutex> g(robots_mutex_);
      if (self_ && self_->inventory)
        {
          for (auto& c : *self_->inventory) { if (c.slave_name == slave) { rtcs.push_back(c.name); } }
        }
    }
    std::string list;
    for (auto& r : rtcs) { list += (list.empty() ? "" : ",") + r; }
    std::string code_s = code == process::kUnknownExit ? std::string("unknown") : std::to_string(code);
    std::string detail = slave + " pid=" + std::to_string(pid) + " code=" + code_s + (list.empty() ? "" : " rtcs=" + list);
    if (code == 0)
      {
        // RTC がなくなって終わった (manager.shutdown_on_nortcs) などの正常な終了
        event(name_, "slave_exited", detail);
        return;
      }
    event(name_, "slave_crashed", detail);
    logfile(fmt(wallclock(), 6) + "\tSLAVE_CRASHED\t" + slave + "\t" + code_s);
    mark_crashed(name_, rtcs);
    auto ips = peer_ips();
    std::string info = name_ + "|" + list;
    std::thread([this, ips, info] {
      for (auto& ip : ips) { trap(ip, "Slave Exited", info); }
    }).detach();
  }
} // namespace snmpmgr
