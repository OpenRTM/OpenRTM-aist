// -*- C++ -*-
/*!
 * @file RTCFailover.cpp
 * @brief Configuration-driven RTC creation, wiring and failover (C++ version of RTCFailover.py)
 *
 * @~japanese
 * 設定ファイル方式の RTC の生成・接続・フェイルオーバー (RTCFailover.py の C++ 版)。動作とログは Python 版と同じ
 * @~english
 * Configuration-driven RTC creation, wiring and failover (C++ version of RTCFailover.py). The behavior and logs are
 * the same as the Python version
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

#include "RTCFailover.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <rtm/CORBA_RTCUtil.h>
#include <rtm/ManagerServant.h>

#include "RTCOperations.hpp"

namespace rtcfailover
{
  namespace
  {
    std::atomic<int> g_level{static_cast<int>(Level::Info)};
    std::mutex g_log_mutex;

    /*!
     * @~japanese
     * @brief 段階の名前
     *
     * @param level 段階
     * @return 名前
     * @~english
     * @brief Name of a level
     *
     * @param level level
     * @return name
     * @~
     */
    const char* level_name(Level level)
    {
      switch (level)
        {
        case Level::Debug: return "DEBUG";
        case Level::Info: return "INFO";
        case Level::Warning: return "WARNING";
        default: return "ERROR";
        }
    }

    /*!
     * @~japanese
     * @brief JSON の値を文字列で取る (なければ既定値)
     *
     * @param j オブジェクト
     * @param name キー
     * @param def 既定値
     * @return 値
     * @~english
     * @brief Gets a JSON value as a string (default if absent)
     *
     * @param j object
     * @param name key
     * @param def default value
     * @return value
     * @~
     */
    std::string str(const nlohmann::json& j, const char* name, const std::string& def = "")
    {
      if (!j.is_object() || !j.contains(name) || j[name].is_null()) { return def; }
      return j[name].is_string() ? j[name].get<std::string>() : j[name].dump();
    }

    /*!
     * @~japanese
     * @brief 接続の名前 (なければ failover_\<ロール\>)
     *
     * @param conn 接続の設定
     * @return 名前
     * @~english
     * @brief Name of a connection (failover_\<role\> if absent)
     *
     * @param conn connection settings
     * @return name
     * @~
     */
    std::string conn_name(const nlohmann::json& conn)
    {
      return str(conn, "name", "failover_" + str(conn, "role"));
    }

    /*!
     * @~japanese
     * @brief RTC が ERROR 状態か (どれかの実行コンテキストで ERROR なら true)
     *
     * @param comp RTC
     * @return ERROR なら true
     * @~english
     * @brief Whether an RTC is in the ERROR state (true if ERROR in any execution context)
     *
     * @param comp RTC
     * @return true if ERROR
     * @~
     */
    bool in_error(RTC::RTObject_ptr comp)
    {
      RTC::ExecutionContextList_var ecs = comp->get_owned_contexts();
      for (CORBA::ULong i = 0; i < ecs->length(); ++i)
        {
          if (ecs[i]->get_component_state(comp) == RTC::ERROR_STATE) { return true; }
        }
      return false;
    }
  } // namespace

  void set_log_level(const std::string& name)
  {
    std::string n = name;
    std::transform(n.begin(), n.end(), n.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    Level level = Level::Info;
    if (n == "DEBUG") { level = Level::Debug; }
    else if (n == "WARNING" || n == "WARN") { level = Level::Warning; }
    else if (n == "ERROR") { level = Level::Error; }
    g_level = static_cast<int>(level);
  }

  void log(Level level, const std::string& msg)
  {
    if (static_cast<int>(level) < g_level) { return; }
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::lock_guard<std::mutex> g(g_log_mutex);
    std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "," << std::setw(3) << std::setfill('0') << ms
              << std::setfill(' ') << " " << level_name(level) << " RTCFailover: " << msg << std::endl;
  }

  nlohmann::json load_config(const std::string& path_in)
  {
    std::string path = path_in;
    if (path.empty())
      {
        const char* env = std::getenv("RTC_FAILOVER_CONFIG");
        path = env ? env : "rtc_failover.json";
      }
    std::ifstream in(path);
    if (!in) { throw std::runtime_error("cannot open " + path); }
    nlohmann::json cfg = nlohmann::json::parse(in);
    auto fail = [](const std::string& m) { throw std::runtime_error(m); };
    if (!cfg.contains("groups") || !cfg["groups"].is_array() || cfg["groups"].empty())
      {
        fail("groups must be a nonempty list");
      }
    if (!cfg.contains("managers") || !cfg["managers"].is_array() || cfg["managers"].empty())
      {
        fail("managers must be a nonempty list");
      }
    std::set<std::string> ids;
    for (auto& m : cfg["managers"])
      {
        if (!ids.insert(str(m, "id")).second) { fail("manager ids must be unique"); }
      }
    std::set<std::string> gids;
    for (auto& g : cfg["groups"])
      {
        std::string gid = str(g, "id");
        if (gid.empty() || !gids.insert(gid).second) { fail("group ids must be present and unique"); }
      }
    for (auto& group : cfg["groups"])
      {
        std::string gid = str(group, "id");
        bool has_conn = (group.contains("connections") && !group["connections"].empty()) ||
                        (group.contains("internal_connections") && !group["internal_connections"].empty());
        if (!group.contains("alternatives") || group["alternatives"].empty() || !has_conn)
          {
            fail("group requires alternatives and connections: " + gid);
          }
        std::string action = str(group, "on_failure", "keep");
        if (action != "keep" && action != "reset" && action != "exit")
          {
            fail("on_failure must be one of ('keep', 'reset', 'exit')");
          }
        std::set<std::string> roles;
        for (auto& [r, v] : group["alternatives"][0]["roles"].items()) { roles.insert(r); }
        for (auto& alt : group["alternatives"])
          {
            if (!ids.count(str(alt, "manager"))) { fail("unknown manager: " + str(alt, "manager")); }
            if (!alt.contains("roles") || alt["roles"].empty()) { fail("alternative requires roles"); }
            std::set<std::string> rs;
            for (auto& [r, v] : alt["roles"].items()) { rs.insert(r); }
            if (rs != roles) { fail("all alternatives of group " + gid + " must define the same roles"); }
          }
        for (auto& conn : group.value("connections", nlohmann::json::array()))
          {
            if (!roles.count(str(conn, "role")))
              {
                fail("connection role absent from first alternative: " + str(conn, "role"));
              }
            std::string dir = str(conn, "direction");
            if (dir != "input" && dir != "output") { fail("direction must be input or output"); }
            nlohmann::json ext = conn.value("external", nlohmann::json::object());
            bool by_manager = !str(ext, "manager").empty() && !str(ext, "component").empty() && !str(ext, "port").empty();
            if (str(ext, "url").empty() && !by_manager)
              {
                fail("external needs url or manager/component/port: " + str(conn, "name"));
              }
            if (!str(ext, "manager").empty() && !ids.count(str(ext, "manager")))
              {
                fail("unknown manager in external: " + str(ext, "manager"));
              }
          }
        for (auto& conn : group.value("internal_connections", nlohmann::json::array()))
          {
            for (const char* end : {"source", "target"})
              {
                nlohmann::json e = conn.value(end, nlohmann::json::object());
                if (!roles.count(str(e, "role")) || str(e, "port").empty())
                  {
                    fail("internal connection " + str(conn, "name") + " needs " + end + ".role/port");
                  }
              }
          }
      }
    return cfg;
  }

  FailoverController::FailoverController(RTC::Manager& manager, nlohmann::json config)
    : manager_(manager), config_(std::move(config))
  {
    for (auto& g : config_["groups"])
      {
        groups_[str(g, "id")] = g;
        group_order_.push_back(str(g, "id"));
      }
    for (auto& m : config_["managers"]) { manager_configs_[str(m, "id")] = m; }
    nlohmann::json obs = config_.value("observer", nlohmann::json::object());
    observer_enabled_ = obs.value("enabled", true);
    observer_heartbeat_ = obs.value("heartbeat", false);
    liveness_interval_ = config_.value("liveness_interval", 2.0);
  }

  FailoverController::~FailoverController()
  {
    stop();
    try
      {
        // マネージャが終わった後は POA がない (サーバントは POA と一緒に消えている)
        PortableServer::POA_var poa = manager_.getPOA();
        if (CORBA::is_nil(poa)) { return; }
        for (auto& [k, h] : observers_) { snmpmgr::observer::detach(poa, h); }
      }
    catch (...)
      {
      }
  }

  /*!
   * @~japanese
   * @brief マネージャの参照を返す (local なら自分、ほかは corbaloc:iiop:\<host\>:\<port\>/\<name\>)
   *
   * @param manager_id マネージャの ID
   * @return マネージャの参照
   * @~english
   * @brief Returns a manager reference (this manager if local, otherwise corbaloc:iiop:\<host\>:\<port\>/\<name\>)
   *
   * @param manager_id manager ID
   * @return manager reference
   * @~
   */
  RTM::Manager_var FailoverController::manager_ref(const std::string& manager_id)
  {
    const nlohmann::json& spec = manager_configs_.at(manager_id);
    if (spec.value("local", false)) { return manager_.getManagerServant().getObjRef(); }
    // 相手の manager.name で corbaloc を作る (マネージャ名がロボットごとに違う構成もあるため)
    CORBA::ORB_var orb = manager_.getORB();
    return snmpmgr::rtcops::resolve_manager(orb, str(spec, "host"), spec.value("port", 2810),
                                            str(spec, "name", "manager"));
  }

  /*!
   * @~japanese
   * @brief マネージャから、指定 (type と任意の instance) に合う RTC を探す
   *
   * @param manager_id マネージャの ID
   * @param spec 候補の指定
   * @param allow_error ERROR の RTC も許すか
   * @return RTC (なければ nil)
   * @~english
   * @brief Finds an RTC matching the specification (type and optional instance) in a manager
   *
   * @param manager_id manager ID
   * @param spec candidate specification
   * @param allow_error whether RTCs in ERROR are allowed
   * @return RTC (nil if not found)
   * @~
   */
  RTC::RTObject_var FailoverController::find(const std::string& manager_id, const nlohmann::json& spec, bool allow_error)
  {
    RTM::Manager_var mgr = manager_ref(manager_id);
    for (auto& comp : snmpmgr::rtcops::components(mgr))
      {
        try
          {
            RTC::ComponentProfile_var prof = comp->get_component_profile();
            if (std::string(prof->type_name) != str(spec, "type")) { continue; }
            if (!str(spec, "instance").empty() && std::string(prof->instance_name) != str(spec, "instance")) { continue; }
            if (allow_error || !in_error(comp)) { return comp; }
          }
        catch (CORBA::SystemException&)
          {
            log(Level::Debug, "component inspection failed");
          }
      }
    return RTC::RTObject::_nil();
  }

  /*!
   * @~japanese
   * @brief RTC のキー (IOR) を返す
   *
   * @param comp RTC
   * @return IOR
   * @~english
   * @brief Returns the key (IOR) of an RTC
   *
   * @param comp RTC
   * @return IOR
   * @~
   */
  std::string FailoverController::key(RTC::RTObject_ptr comp)
  {
    CORBA::ORB_var orb = manager_.getORB();
    CORBA::String_var ior = orb->object_to_string(comp);
    return std::string(ior.in());
  }

  /*!
   * @~japanese
   * @brief 設定の RTC を生成する (最初の候補に "create": true があるグループ)
   * @~english
   * @brief Creates the RTCs in the configuration (groups whose first alternative has "create": true)
   * @~
   */
  void FailoverController::create_initial()
  {
    for (auto& gid : group_order_)
      {
        const nlohmann::json& first = groups_[gid]["alternatives"][0];
        if (!first.value("create", false)) { continue; }
        std::string mid = str(first, "manager");
        RTM::Manager_var mgr = manager_ref(mid);
        for (auto& [role, spec] : first["roles"].items())
          {
            if (!CORBA::is_nil(find(mid, spec, true))) { continue; }
            std::string creation = str(spec, "create_id");
            if (creation.empty()) { throw std::runtime_error("missing create_id for role " + role); }
            std::map<std::string, std::string> options;
            for (auto& [k, v] : spec.value("create_options", nlohmann::json::object()).items())
              {
                options[k] = v.is_string() ? v.get<std::string>() : v.dump();
              }
            if (!str(spec, "instance").empty() && !options.count("instance_name"))
              {
                options["instance_name"] = str(spec, "instance");
              }
            std::string suffix;
            for (auto& [k, v] : options) { suffix += (suffix.empty() ? "" : "&") + k + "=" + v; }
            std::string request = creation + (suffix.empty() ? "" : "?" + suffix);
            RTC::RTObject_var comp = mgr->create_component(request.c_str());
            if (CORBA::is_nil(comp)) { throw std::runtime_error("create_component failed: " + request); }
            log(Level::Info, "created " + request + " on " + mid);
          }
      }
  }

  /*!
   * @~japanese
   * @brief 外部ポートの参照を返す (一度取れたらキャッシュして使い回す)
   *
   * 外部 RTC を毎回探し直すと、外部 RTC が停止したノードの RTC と接続している間は、OpenRTM のポートが接続先の
   * 生存確認 (checkPorts) で応答しなくなることがあるため。
   *
   * @param endpoint 接続の端
   * @return ポートの参照
   * @~english
   * @brief Returns the reference of an external port (cached once obtained)
   *
   * Because searching for the external RTC each time may stop responding while OpenRTM ports check the liveness of
   * their peers (checkPorts) when connected to RTCs on a stopped node.
   *
   * @param endpoint endpoint
   * @return port reference
   * @~
   */
  RTC::PortService_var FailoverController::external_port(const nlohmann::json& endpoint)
  {
    std::string k = endpoint.dump();
    auto it = ext_ports_.find(k);
    if (it != ext_ports_.end())
      {
        try
          {
            if (!it->second->_non_existent()) { return it->second; }
          }
        catch (CORBA::SystemException&) {}
        ext_ports_.erase(it);
      }
    RTC::PortService_var port = resolve_external_port(endpoint);
    ext_ports_[k] = port;
    return port;
  }

  /*!
   * @~japanese
   * @brief rtcname:// (ネームサービス) か、manager/component/port (マネージャ経由) で外部ポートを得る
   *
   * @param endpoint 接続の端
   * @return ポートの参照
   * @~english
   * @brief Obtains an external port by rtcname:// (name service) or manager/component/port (via a manager)
   *
   * @param endpoint endpoint
   * @return port reference
   * @~
   */
  RTC::PortService_var FailoverController::resolve_external_port(const nlohmann::json& endpoint)
  {
    if (!str(endpoint, "url").empty())
      {
        RTC::PortService_var port = CORBA_RTCUtil::get_port_by_url(str(endpoint, "url"));
        if (CORBA::is_nil(port)) { throw std::runtime_error("external port not found: " + str(endpoint, "url")); }
        return port;
      }
    RTM::Manager_var mgr = manager_ref(str(endpoint, "manager"));
    for (auto& comp : snmpmgr::rtcops::components(mgr))
      {
        try
          {
            if (snmpmgr::rtcops::instance_name(comp) == str(endpoint, "component"))
              {
                return snmpmgr::rtcops::port(comp, str(endpoint, "port"));
              }
          }
        catch (CORBA::SystemException&) {}
      }
    throw std::runtime_error("external component not found: " + str(endpoint, "component") + " on " +
                             str(endpoint, "manager"));
  }

  /*!
   * @~japanese
   * @brief グループの RTC と外部ポートを接続する (direction が output なら RTC → 外部、input なら外部 → RTC)
   *
   * @param conn 接続の設定
   * @param comp RTC
   * @~english
   * @brief Connects an RTC of the group with an external port (RTC -> external if direction is output, external ->
   *        RTC if input)
   *
   * @param conn connection settings
   * @param comp RTC
   * @~
   */
  void FailoverController::connect(const nlohmann::json& conn, RTC::RTObject_ptr comp)
  {
    RTC::PortService_var local = snmpmgr::rtcops::port(comp, str(conn, "port"));
    RTC::PortService_var external = external_port(conn["external"]);
    bool output = str(conn, "direction") == "output";
    try
      {
        snmpmgr::rtcops::connect(conn_name(conn), conn.value("properties", nlohmann::json::object()),
                                 output ? local.in() : external.in(), output ? external.in() : local.in());
      }
    catch (std::exception& e)
      {
        throw std::runtime_error("connect failed for " + str(conn, "role") + ": " + e.what());
      }
  }

  /*!
   * @~japanese
   * @brief グループの中の RTC どうしを接続する (同じ名前の接続があれば何もしない)
   *
   * @param conn 接続の設定
   * @param components グループの RTC
   * @~english
   * @brief Connects RTCs within a group (does nothing if a connection with the same name exists)
   *
   * @param conn connection settings
   * @param components RTCs of the group
   * @~
   */
  void FailoverController::connect_internal(const nlohmann::json& conn,
                                            const std::map<std::string, RTC::RTObject_var>& components)
  {
    RTC::PortService_var src = snmpmgr::rtcops::port(components.at(str(conn["source"], "role")).in(),
                                                     str(conn["source"], "port"));
    RTC::PortService_var dst = snmpmgr::rtcops::port(components.at(str(conn["target"], "role")).in(),
                                                     str(conn["target"], "port"));
    std::string name = str(conn, "name", "failover_internal");
    if (snmpmgr::rtcops::connector_exists(src, dst, name)) { return; }
    try
      {
        snmpmgr::rtcops::connect(name, conn.value("properties", nlohmann::json::object()), src, dst);
      }
    catch (std::exception& e)
      {
        throw std::runtime_error("internal connect failed: " + name + " (" + e.what() + ")");
      }
  }

  /*!
   * @~japanese
   * @brief 切り替え前の接続を切る (外部ポート上の、名前が一致するコネクタだけ。古い RTC が落ちていても外部から切れる)
   *
   * @param group グループ
   * @~english
   * @brief Disconnects before switching (only the connectors with the matching name on the external ports; they can
   *        be disconnected from the external side even if the old RTC is down)
   *
   * @param group group
   * @~
   */
  void FailoverController::disconnect(const nlohmann::json& group)
  {
    for (auto& conn : group.value("connections", nlohmann::json::array()))
      {
        RTC::PortService_var port;
        try
          {
            port = external_port(conn["external"]);
          }
        catch (std::exception&)
          {
            log(Level::Warning, "external port unavailable while disconnecting " + str(conn, "name"));
            continue;
          }
        catch (CORBA::SystemException&)
          {
            log(Level::Warning, "external port unavailable while disconnecting " + str(conn, "name"));
            continue;
          }
        std::string target = conn_name(conn);
        RTC::ConnectorProfileList_var profiles = port->get_connector_profiles();
        for (CORBA::ULong i = 0; i < profiles->length(); ++i)
          {
            if (std::string(profiles[i].name) != target) { continue; }
            RTC::ReturnCode_t ret = port->disconnect(profiles[i].connector_id);
            if (ret != RTC::RTC_OK)
              {
                log(Level::Warning, "disconnect " + target + " returned " + std::to_string(ret));
              }
          }
      }
  }

  /*!
   * @~japanese
   * @brief 代替候補の RTC の組を探す (どのロールにも使える RTC があり、接続するポートがあること)
   *
   * @param group グループ
   * @param alt 代替候補
   * @param components 見つけた RTC の組 (ロール -> RTC)
   * @return 使えれば true
   * @~english
   * @brief Finds the set of RTCs of an alternative (each role has a usable RTC with the ports to connect)
   *
   * @param group group
   * @param alt alternative
   * @param components found set of RTCs (role -> RTC)
   * @return true if usable
   * @~
   */
  bool FailoverController::candidate(const nlohmann::json& group, const nlohmann::json& alt,
                                     std::map<std::string, RTC::RTObject_var>& components)
  {
    for (auto& [role, spec] : alt["roles"].items())
      {
        RTC::RTObject_var comp = find(str(alt, "manager"), spec, false);
        if (CORBA::is_nil(comp)) { return false; }
        components[role] = comp;
      }
    for (auto& conn : group.value("connections", nlohmann::json::array()))
      {
        try
          {
            snmpmgr::rtcops::port(components[str(conn, "role")].in(), str(conn, "port"));
          }
        catch (...)
          {
            return false;
          }
      }
    return true;
  }

  bool FailoverController::switch_group(const std::string& group_id, bool skip_current)
  {
    std::lock_guard<std::recursive_mutex> g(lock_);
    const nlohmann::json& group = groups_.at(group_id);
    // priority の小さい順 (なければ定義の順)
    std::vector<std::pair<long long, size_t>> order;
    for (size_t i = 0; i < group["alternatives"].size(); ++i)
      {
        order.emplace_back(group["alternatives"][i].value("priority", static_cast<long long>(i)), i);
      }
    std::stable_sort(order.begin(), order.end());
    bool has_old = current_.count(group_id) > 0;
    Current old = has_old ? current_[group_id] : Current{};
    for (auto& [prio, index] : order)
      {
        const nlohmann::json& alt = group["alternatives"][index];
        if (skip_current && has_old && index == old.index) { continue; }
        try
          {
            std::map<std::string, RTC::RTObject_var> components;
            if (!candidate(group, alt, components)) { continue; }
            if (has_old) { disconnect(group); }
            for (auto& conn : group.value("internal_connections", nlohmann::json::array()))
              {
                connect_internal(conn, components);
              }
            for (auto& conn : group.value("connections", nlohmann::json::array()))
              {
                connect(conn, components[str(conn, "role")].in());
              }
            if (group.value("activate", true))
              {
                for (auto& [role, comp] : components) { snmpmgr::rtcops::activate(comp); }
              }
            Current cur;
            cur.index = index;
            cur.components = components;
            for (auto& [role, comp] : components) { cur.keys.insert(key(comp)); }
            current_[group_id] = cur;
            log(Level::Info, "group " + group_id + " selected alternative " + std::to_string(index) + " on " +
                str(alt, "manager"));
            if (has_old) { handle_old(group, old); }
            register_observers(group_id);
            return true;
          }
        catch (std::exception& e)
          {
            log(Level::Error, "alternative " + std::to_string(index) + " failed for group " + group_id + ": " +
                e.what());
          }
        catch (CORBA::SystemException& e)
          {
            log(Level::Error, "alternative " + std::to_string(index) + " failed for group " + group_id +
                ": CORBA " + e._name());
          }
      }
    log(Level::Error, "no usable alternative for group " + group_id);
    return false;
  }

  /*!
   * @~japanese
   * @brief 切り替え前の RTC の後始末 (on_failure: keep は何もしない、reset は ERROR なら reset_component、exit は終了)
   *
   * @param group グループ
   * @param old 切り替え前の RTC
   * @~english
   * @brief Cleans up the RTCs before switching (on_failure: keep does nothing, reset calls reset_component if
   *        ERROR, exit terminates them)
   *
   * @param group group
   * @param old RTCs before switching
   * @~
   */
  void FailoverController::handle_old(const nlohmann::json& group, Current& old)
  {
    std::string action = str(group, "on_failure", "keep");
    if (action == "keep") { return; }
    for (auto& [role, comp] : old.components)
      {
        try
          {
            if (action == "reset")
              {
                RTC::ExecutionContextList_var ecs = comp->get_owned_contexts();
                for (CORBA::ULong i = 0; i < ecs->length(); ++i)
                  {
                    if (ecs[i]->get_component_state(comp) == RTC::ERROR_STATE)
                      {
                        RTC::ReturnCode_t ret = ecs[i]->reset_component(comp);
                        log(Level::Info, "reset " + role + " (" + str(group, "id") + "): " + std::to_string(ret));
                      }
                  }
              }
            else if (action == "exit")
              {
                RTC::ReturnCode_t ret = comp->exit();
                log(Level::Info, "exit " + role + " (" + str(group, "id") + "): " + std::to_string(ret));
              }
          }
        catch (...)
          {
            log(Level::Info, "old " + role + " of group " + str(group, "id") + " is unreachable (" + action +
                " skipped)");
          }
      }
  }

  /*!
   * @~japanese
   * @brief 現在と待機中のすべての候補 RTC に ComponentObserver を登録する (登録済みは飛ばす)
   *
   * @param group_id グループの ID (空なら全部)
   * @~english
   * @brief Registers ComponentObserver to all current and standby candidate RTCs (registered ones are skipped)
   *
   * @param group_id group ID (all if empty)
   * @~
   */
  void FailoverController::register_observers(const std::string& group_id)
  {
    if (!observer_enabled_) { return; }
    std::vector<std::string> gids = group_id.empty() ? group_order_ : std::vector<std::string>{group_id};
    for (auto& gid : gids)
      {
        for (auto& alt : groups_[gid]["alternatives"])
          {
            for (auto& [role, spec] : alt["roles"].items())
              {
                RTC::RTObject_var comp;
                try
                  {
                    comp = find(str(alt, "manager"), spec, true);
                  }
                catch (...)
                  {
                    continue;
                  }
                if (!CORBA::is_nil(comp)) { add_observer(gid, comp); }
              }
          }
      }
  }

  /*!
   * @~japanese
   * @brief RTC に ComponentObserver を登録する (RTC_STATUS を観測する。通知は notify_error へ)
   *
   * @param group_id グループの ID
   * @param comp RTC
   * @~english
   * @brief Registers ComponentObserver to an RTC (observes RTC_STATUS; notifications go to notify_error)
   *
   * @param group_id group ID
   * @param comp RTC
   * @~
   */
  void FailoverController::add_observer(const std::string& group_id, RTC::RTObject_ptr comp)
  {
    try
      {
        std::string k = key(comp);
        if (observers_.count(k)) { return; }
        std::string name = snmpmgr::rtcops::instance_name(comp);
        PortableServer::POA_var poa = manager_.getPOA();
        std::string error;
        auto notify = [this, group_id, k](const std::string&, const std::string& kind, const std::string& hint)
        {
          if (kind == "RTC_STATUS" && hint.substr(0, hint.find(':')) == "ERROR") { notify_error(group_id, k, hint); }
        };
        auto h = snmpmgr::observer::attach(poa, comp, name, "RTC_STATUS", notify, error,
                                           {{"heartbeat.enable", observer_heartbeat_ ? "YES" : "NO"}});
        if (!h)
          {
            log(Level::Warning, "observer registration for " + name + " returned false "
                "(ComponentObserverConsumer が RTC 側で読み込まれていない可能性): " + error);
            return;
          }
        observers_[k] = *h;
        log(Level::Info, "observer registered on " + name + " (group " + group_id + ")");
      }
    catch (CORBA::SystemException& e)
      {
        log(Level::Warning, std::string("observer registration failed: CORBA ") + e._name());
      }
    catch (std::exception& e)
      {
        log(Level::Warning, std::string("observer registration failed: ") + e.what());
      }
  }

  /*!
   * @~japanese
   * @brief ComponentObserver から (CORBA のスレッドで) 呼ばれる。現在の RTC なら、切り替えをワーカースレッドへ回す
   *
   * @param group_id グループの ID
   * @param comp_key RTC のキー
   * @param hint 詳細 (ERROR:\<ec_id\>)
   * @~english
   * @brief Called by ComponentObserver (on a CORBA thread). If it is a current RTC, the switching is passed to the
   *        worker thread
   *
   * @param group_id group ID
   * @param comp_key key of the RTC
   * @param hint hint (ERROR:\<ec_id\>)
   * @~
   */
  void FailoverController::notify_error(const std::string& group_id, const std::string& comp_key, const std::string& hint)
  {
    bool current;
    {
      std::lock_guard<std::recursive_mutex> g(lock_);
      auto it = current_.find(group_id);
      current = it != current_.end() && it->second.keys.count(comp_key) > 0;
    }
    if (!current)
      {
        log(Level::Info, "ERROR on a standby RTC of group " + group_id + " ignored (" + hint + ")");
        return;
      }
    log(Level::Warning, "RTC ERROR in group " + group_id + ": " + hint);
    enqueue(group_id, "observer: " + hint);
  }

  /*!
   * @~japanese
   * @brief 切り替えの要求をワーカースレッドに渡す (同じグループの要求が処理待ちなら 1 回にまとめる)
   *
   * @param group_id グループの ID
   * @param reason 理由
   * @~english
   * @brief Passes a switching request to the worker thread (merged into one if a request of the same group is pending)
   *
   * @param group_id group ID
   * @param reason reason
   * @~
   */
  void FailoverController::enqueue(const std::string& group_id, const std::string& reason)
  {
    std::lock_guard<std::mutex> g(queue_mutex_);
    if (!pending_.insert(group_id).second) { return; }
    queue_.emplace_back(group_id, reason);
    queue_cv_.notify_one();
  }

  /*!
   * @~japanese
   * @brief 切り替えの要求を処理するスレッドの本体
   * @~english
   * @brief Body of the thread that handles switching requests
   * @~
   */
  void FailoverController::worker()
  {
    while (!stop_)
      {
        std::pair<std::string, std::string> item;
        {
          std::unique_lock<std::mutex> g(queue_mutex_);
          queue_cv_.wait_for(g, std::chrono::milliseconds(500), [this] { return stop_ || !queue_.empty(); });
          if (queue_.empty()) { continue; }
          item = queue_.front();
          queue_.pop_front();
        }
        try
          {
            log(Level::Warning, "failover of group " + item.first + " (" + item.second + ")");
            handle_failure(item.first);
          }
        catch (std::exception& e)
          {
            log(Level::Error, "failover of group " + item.first + " failed: " + e.what());
          }
        catch (...)
          {
            log(Level::Error, "failover of group " + item.first + " failed");
          }
        std::lock_guard<std::mutex> g(queue_mutex_);
        pending_.erase(item.first);
      }
  }

  /*!
   * @~japanese
   * @brief 生存確認のスレッドの本体。現在の RTC に到達できない (プロセス終了) か ERROR なら切り替える。新しく現れた
   *        待機 RTC にもオブザーバーを付ける
   * @~english
   * @brief Body of the liveness thread. Switches when the current RTC is unreachable (process exit) or in ERROR. Also
   *        attaches observers to newly appeared standby RTCs
   * @~
   */
  void FailoverController::liveness()
  {
    auto period = std::chrono::milliseconds(static_cast<long long>(liveness_interval_ * 1000));
    auto next = std::chrono::steady_clock::now() + period;
    while (!stop_)
      {
        if (std::chrono::steady_clock::now() < next)
          {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
          }
        next = std::chrono::steady_clock::now() + period;
        std::map<std::string, Current> snapshot;
        {
          std::lock_guard<std::recursive_mutex> g(lock_);
          snapshot = current_;
        }
        for (auto& [gid, state] : snapshot)
          {
            for (auto& [role, comp] : state.components)
              {
                try
                  {
                    if (comp->_non_existent()) { throw std::runtime_error("non existent"); }
                    if (in_error(comp))
                      {
                        enqueue(gid, "liveness: " + role + " in ERROR");
                        break;
                      }
                  }
                catch (...)
                  {
                    enqueue(gid, "liveness: " + role + " unreachable");
                    break;
                  }
              }
          }
        try
          {
            std::lock_guard<std::recursive_mutex> g(lock_);
            register_observers();
          }
        catch (...)
          {
            log(Level::Debug, "observer refresh failed");
          }
      }
  }

  std::map<std::string, bool> FailoverController::start()
  {
    create_initial();
    std::map<std::string, bool> results;
    for (auto& gid : group_order_) { results[gid] = switch_group(gid, false); }
    // 外部ポートは起動時 (全ノードが生きている間) に解決してキャッシュしておく
    for (auto& gid : group_order_)
      {
        for (auto& conn : groups_[gid].value("connections", nlohmann::json::array()))
          {
            try
              {
                std::lock_guard<std::recursive_mutex> g(lock_);
                external_port(conn["external"]);
              }
            catch (...)
              {
                log(Level::Warning, "external port " + conn_name(conn) + " not resolved yet");
              }
          }
      }
    {
      std::lock_guard<std::recursive_mutex> g(lock_);
      register_observers();
    }
    threads_.emplace_back(&FailoverController::worker, this);
    if (liveness_interval_ > 0) { threads_.emplace_back(&FailoverController::liveness, this); }
    return results;
  }

  void FailoverController::stop()
  {
    stop_ = true;
    queue_cv_.notify_all();
    for (auto& t : threads_)
      {
        if (t.joinable()) { t.join(); }
      }
    threads_.clear();
  }

  bool FailoverController::handle_failure(const std::string& group_id)
  {
    return switch_group(group_id, true);
  }

  std::map<std::string, bool> FailoverController::reconnect_all()
  {
    std::map<std::string, bool> out;
    for (auto& gid : group_order_) { out[gid] = handle_failure(gid); }
    return out;
  }
} // namespace rtcfailover
