// -*- C++ -*-
/*!
 * @file MonitoringStore.cpp
 * @brief Saving monitoring information to SQLite (writes are done on a dedicated thread)
 *
 * @~japanese
 * 監視情報の SQLite への保存 (書き込みは専用スレッド)
 * @~
 *
 * @date $Date: 2026-11-13 16:24:32 $
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

#include "MonitoringStore.hpp"
#include "SNMPManagerUtil.hpp"

#ifdef SNMPMGR_WINSQLITE
// winsqlite3.h は NTDDI_VERSION が Windows 10 以上でないと宣言を出さない。OpenRTM-aist の中でビルドすると
// _WIN32_WINNT=0x0603 (Windows 8.1) が付くので、この include の間だけ Windows 10 にする
// (winsqlite3.h only declares the API for NTDDI_VERSION >= Windows 10; OpenRTM-aist defines _WIN32_WINNT=0x0603)
#pragma push_macro("NTDDI_VERSION")
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000000
#include <winsqlite/winsqlite3.h>   // Windows SDK の SQLite (winsqlite3.dll)
#pragma pop_macro("NTDDI_VERSION")
#else
#include <sqlite3.h>
#endif

#include <filesystem>

namespace snmpmgr
{
  namespace
  {
    const char* kSchema = R"SQL(
CREATE TABLE IF NOT EXISTS metrics (
    ts REAL NOT NULL, observer TEXT NOT NULL, robot TEXT NOT NULL, ip TEXT, status TEXT,
    cpu_usage REAL, memory_usage REAL, network_in REAL, network_out REAL, wifi_signal_dbm REAL,
    latency_ms REAL, jitter_ms REAL, packet_loss REAL,
    cg_cpu_usage REAL, cg_cpu_limit REAL, cg_mem_usage_mb REAL, cg_mem_limit_mb REAL);
CREATE INDEX IF NOT EXISTS metrics_robot_ts ON metrics (robot, ts);
CREATE TABLE IF NOT EXISTS events (ts REAL NOT NULL, observer TEXT NOT NULL, robot TEXT, kind TEXT NOT NULL, detail TEXT);
CREATE INDEX IF NOT EXISTS events_ts ON events (ts);
CREATE TABLE IF NOT EXISTS rtc_measurements (
    ts REAL NOT NULL, observer TEXT NOT NULL, robot TEXT, manager TEXT, instance TEXT, type TEXT, language TEXT,
    samples INTEGER, cpu_avg REAL, cpu_max REAL, cpu_p95 REAL, mem_avg_mb REAL, mem_max_mb REAL, ports TEXT);
CREATE INDEX IF NOT EXISTS rtc_measurements_type ON rtc_measurements (type, ts);
CREATE TABLE IF NOT EXISTS custom_metrics (ts REAL NOT NULL, observer TEXT NOT NULL, robot TEXT NOT NULL, name TEXT NOT NULL, value REAL);
CREATE INDEX IF NOT EXISTS custom_metrics_robot_ts ON custom_metrics (robot, name, ts);
)SQL";

    /*!
     * @~japanese
     * @brief 値 (なければ NULL) を SQL の文に結び付ける
     *
     * @param st 文
     * @param idx 番号
     * @param v 値
     * @~english
     * @brief Binds a value (NULL if absent) to an SQL statement
     *
     * @param st statement
     * @param idx index
     * @param v value
     * @~
     */
    void bind_opt(sqlite3_stmt* st, int idx, const std::optional<double>& v)
    {
      if (v) { sqlite3_bind_double(st, idx, *v); }
      else { sqlite3_bind_null(st, idx); }
    }
  } // namespace

  Store::Store(std::string path, std::string observer, double retention_hours)
    : path_(std::move(path)), observer_(std::move(observer)), retention_(retention_hours * 3600.0) {}

  Store::~Store() { close(); }

  bool Store::open(std::string& error)
  {
    try { std::filesystem::create_directories(std::filesystem::absolute(path_).parent_path()); }
    catch (...) {}
    sqlite3* db = nullptr;
    if (sqlite3_open(path_.c_str(), &db) != SQLITE_OK)
      {
        error = db ? sqlite3_errmsg(db) : "sqlite3_open failed";
        if (db) { sqlite3_close(db); }
        return false;
      }
    char* err = nullptr;
    if (sqlite3_exec(db, kSchema, nullptr, nullptr, &err) != SQLITE_OK)
      {
        error = err ? err : "schema failed";
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
      }
    db_ = db;
    thread_ = std::thread([this] { run(); });
    return true;
  }

  void Store::record_metrics(const std::vector<MetricRow>& rows)
  {
    {
      std::lock_guard<std::mutex> g(mutex_);
      queue_.emplace_back(wallclock(), rows);
    }
    cv_.notify_one();
  }

  void Store::record_event(const std::string& robot, const std::string& kind, const std::string& detail)
  {
    {
      std::lock_guard<std::mutex> g(mutex_);
      queue_.emplace_back(wallclock(), EventRow{wallclock(), robot, kind, detail});
    }
    cv_.notify_one();
  }

  void Store::record_measurements(const std::vector<MeasurementRow>& rows)
  {
    if (rows.empty()) { return; }
    {
      std::lock_guard<std::mutex> g(mutex_);
      queue_.emplace_back(wallclock(), rows);
    }
    cv_.notify_one();
  }

  void Store::record_custom_metrics(const std::vector<CustomMetricRow>& rows)
  {
    if (rows.empty()) { return; }
    {
      std::lock_guard<std::mutex> g(mutex_);
      queue_.emplace_back(wallclock(), rows);
    }
    cv_.notify_one();
  }

  void Store::close()
  {
    {
      std::lock_guard<std::mutex> g(mutex_);
      if (stop_) { return; }
      stop_ = true;
    }
    cv_.notify_one();
    if (thread_.joinable()) { thread_.join(); }
    if (db_) { sqlite3_close(static_cast<sqlite3*>(db_)); db_ = nullptr; }
  }

  void Store::run()
  {
    auto* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* ins_m = nullptr;
    sqlite3_stmt* ins_e = nullptr;
    sqlite3_stmt* ins_r = nullptr;
    sqlite3_stmt* ins_c = nullptr;
    sqlite3_prepare_v2(db, "INSERT INTO custom_metrics VALUES (?,?,?,?,?)", -1, &ins_c, nullptr);
    sqlite3_prepare_v2(db, "INSERT INTO rtc_measurements VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)", -1, &ins_r, nullptr);
    sqlite3_prepare_v2(db, "INSERT INTO metrics VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", -1, &ins_m, nullptr);
    sqlite3_prepare_v2(db, "INSERT INTO events VALUES (?,?,?,?,?)", -1, &ins_e, nullptr);
    double last_cleanup = 0;
    for (;;)
      {
        std::deque<std::pair<double, Item>> batch;
        {
          std::unique_lock<std::mutex> lk(mutex_);
          cv_.wait(lk, [this] { return stop_ || !queue_.empty(); });
          batch.swap(queue_);
          if (batch.empty() && stop_) { break; }
        }
        sqlite3_exec(db, "BEGIN", nullptr, nullptr, nullptr);
        for (auto& [ts, item] : batch)
          {
            if (auto* rows = std::get_if<std::vector<MetricRow>>(&item))
              {
                for (auto& r : *rows)
                  {
                    sqlite3_reset(ins_m);
                    sqlite3_bind_double(ins_m, 1, ts);
                    sqlite3_bind_text(ins_m, 2, observer_.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(ins_m, 3, r.robot.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(ins_m, 4, r.ip.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(ins_m, 5, r.status.c_str(), -1, SQLITE_TRANSIENT);
                    int i = 6;
                    for (auto* v : {&r.cpu_usage, &r.memory_usage, &r.network_in, &r.network_out, &r.wifi_signal_dbm,
                                    &r.latency_ms, &r.jitter_ms, &r.packet_loss, &r.cg_cpu_usage, &r.cg_cpu_limit,
                                    &r.cg_mem_usage_mb, &r.cg_mem_limit_mb})
                      {
                        bind_opt(ins_m, i++, *v);
                      }
                    sqlite3_step(ins_m);
                  }
              }
            else if (auto* ms = std::get_if<std::vector<MeasurementRow>>(&item))
              {
                for (auto& r : *ms)
                  {
                    sqlite3_reset(ins_r);
                    sqlite3_bind_double(ins_r, 1, ts);
                    int i = 2;
                    for (auto* s : {&observer_, &r.robot, &r.manager, &r.instance, &r.type, &r.language})
                      {
                        sqlite3_bind_text(ins_r, i++, s->c_str(), -1, SQLITE_TRANSIENT);
                      }
                    for (auto* v : {&r.samples, &r.cpu_avg, &r.cpu_max, &r.cpu_p95, &r.mem_avg_mb, &r.mem_max_mb})
                      {
                        bind_opt(ins_r, i++, *v);
                      }
                    sqlite3_bind_text(ins_r, i, r.ports_json.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_step(ins_r);
                  }
              }
            else if (auto* cs = std::get_if<std::vector<CustomMetricRow>>(&item))
              {
                for (auto& r : *cs)
                  {
                    sqlite3_reset(ins_c);
                    sqlite3_bind_double(ins_c, 1, ts);
                    sqlite3_bind_text(ins_c, 2, observer_.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(ins_c, 3, r.robot.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(ins_c, 4, r.name.c_str(), -1, SQLITE_TRANSIENT);
                    bind_opt(ins_c, 5, r.value);
                    sqlite3_step(ins_c);
                  }
              }
            else
              {
                auto& e = std::get<EventRow>(item);
                sqlite3_reset(ins_e);
                sqlite3_bind_double(ins_e, 1, e.ts);
                sqlite3_bind_text(ins_e, 2, observer_.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(ins_e, 3, e.robot.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(ins_e, 4, e.kind.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(ins_e, 5, e.detail.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(ins_e);
              }
          }
        sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        if (retention_ > 0 && wallclock() - last_cleanup > 600)
          {
            double limit = wallclock() - retention_;
            std::string sql = "DELETE FROM metrics WHERE ts < " + std::to_string(limit) +
                              "; DELETE FROM events WHERE ts < " + std::to_string(limit) +
                              "; DELETE FROM rtc_measurements WHERE ts < " + std::to_string(limit) +
                              "; DELETE FROM custom_metrics WHERE ts < " + std::to_string(limit) + ";";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            last_cleanup = wallclock();
          }
      }
    sqlite3_finalize(ins_m);
    sqlite3_finalize(ins_e);
    sqlite3_finalize(ins_r);
    sqlite3_finalize(ins_c);
  }
} // namespace snmpmgr
