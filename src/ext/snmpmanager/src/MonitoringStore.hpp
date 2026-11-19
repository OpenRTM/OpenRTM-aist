// -*- C++ -*-
/*!
 * @file MonitoringStore.hpp
 * @brief Saving monitoring information (metrics and events) to SQLite. The tables are the same as the Python version (MonitoringStore.py)
 *
 * @~japanese
 * 監視情報 (メトリクスとイベント) の SQLite への保存。テーブルは Python 版 (MonitoringStore.py) と同じ
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

#ifndef SNMPMGR_STORE_HPP
#define SNMPMGR_STORE_HPP

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <variant>
#include <vector>

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief ロボットのメトリクスの 1 行 (テーブル metrics)
   * @~english
   * @brief One row of robot metrics (table metrics)
   * @~
   */
  struct MetricRow
  {
    std::string robot, ip, status;
    std::optional<double> cpu_usage, memory_usage, network_in, network_out, wifi_signal_dbm;
    std::optional<double> latency_ms, jitter_ms, packet_loss;
    std::optional<double> cg_cpu_usage, cg_cpu_limit, cg_mem_usage_mb, cg_mem_limit_mb;
  };

  /*!
   * @~japanese
   * @brief マネージャが計測した RTC の実測値 (計測 1 回分。テーブル rtc_measurements)
   * @~english
   * @brief RTC measurement by a manager (one run; table rtc_measurements)
   * @~
   */
  struct MeasurementRow
  {
    std::string robot, manager, instance, type, language;
    std::optional<double> samples, cpu_avg, cpu_max, cpu_p95, mem_avg_mb, mem_max_mb;
    std::string ports_json{"{}"};
  };

  /*!
   * @~japanese
   * @brief 追加の監視の値 (snmp.metrics) の 1 件 (テーブル custom_metrics)
   * @~english
   * @brief One item of additional monitoring values (snmp.metrics) (table custom_metrics)
   * @~
   */
  struct CustomMetricRow
  {
    std::string robot, name;
    std::optional<double> value;
  };

  /*!
   * @~japanese
   * @brief 監視情報を SQLite に保存する (書き込みは専用スレッド)
   * @~english
   * @brief Stores monitoring information in SQLite (writes are done on a dedicated thread)
   * @~
   */
  class Store
  {
  public:
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param path SQLite のファイル
     * @param observer 記録するロボット (自分) の名前
     * @param retention_hours 古い行を消すまでの時間 (時間)
     * @~english
     * @brief Constructor
     *
     * @param path SQLite file
     * @param observer name of the recording robot (this one)
     * @param retention_hours hours to keep rows
     * @~
     */
    Store(std::string path, std::string observer, double retention_hours);
    /*!
     * @~japanese
     * @brief デストラクタ。閉じる
     * @~english
     * @brief Destructor. Closes the store
     * @~
     */
    ~Store();
    /*!
     * @~japanese
     * @brief データベースを開き、書き込みのスレッドを起動する
     *
     * @param error 失敗したときの理由
     * @return 成功すれば true
     * @~english
     * @brief Opens the database and starts the writer thread
     *
     * @param error reason on failure
     * @return true on success
     * @~
     */
    bool open(std::string& error);
    /*!
     * @~japanese
     * @brief ロボットのメトリクスを記録する
     *
     * @param rows 行
     * @~english
     * @brief Records robot metrics
     *
     * @param rows rows
     * @~
     */
    void record_metrics(const std::vector<MetricRow>& rows);
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
    void record_event(const std::string& robot, const std::string& kind, const std::string& detail);
    /*!
     * @~japanese
     * @brief RTC の実測値を記録する
     *
     * @param rows 行
     * @~english
     * @brief Records RTC measurements
     *
     * @param rows rows
     * @~
     */
    void record_measurements(const std::vector<MeasurementRow>& rows);
    /*!
     * @~japanese
     * @brief 追加の監視の値を記録する
     *
     * @param rows 行
     * @~english
     * @brief Records additional monitoring values
     *
     * @param rows rows
     * @~
     */
    void record_custom_metrics(const std::vector<CustomMetricRow>& rows);
    /*!
     * @~japanese
     * @brief 書き込みのスレッドを止めて、データベースを閉じる
     * @~english
     * @brief Stops the writer thread and closes the database
     * @~
     */
    void close();

  private:
    /*!
     * @~japanese
     * @brief イベントの 1 行
     * @~english
     * @brief One event row
     * @~
     */
    struct EventRow { double ts; std::string robot, kind, detail; };
    using Item = std::variant<std::vector<MetricRow>, EventRow, std::vector<MeasurementRow>, std::vector<CustomMetricRow>>;
    /*!
     * @~japanese
     * @brief 書き込みのスレッドの本体
     * @~english
     * @brief Body of the writer thread
     * @~
     */
    void run();
    std::string path_, observer_;
    double retention_;
    void* db_{nullptr};
    std::deque<std::pair<double, Item>> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_{false};
    std::thread thread_;
  };
} // namespace snmpmgr

#endif
