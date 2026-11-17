// -*- C++ -*-
/*!
 * @file MeasurementAggregator.hpp
 * @brief Saving of measurements by the managers and division of the measurements
 *
 * @~japanese
 * マネージャによる実測値の保存と計測の按分
 *
 * Python 版 snmpmgr_measure.py の update_runs / aggregate / render_placement と同じ。slave に読み込んだ計測モジュールが snmp.measure.runtime_dir/\<slave\>.json に書いた計測を、マスターの SNMPManager が集めて runs.json・placement.xml (plc:origin="runtime")・SQLite に保存する。
 * @~english
 * Same as update_runs / aggregate / render_placement of the Python version snmpmgr_measure.py. The master SNMPManager collects the measurements written to snmp.measure.runtime_dir/\<slave\>.json by the measurement module loaded into the slaves, and saves them to runs.json, placement.xml (plc:origin="runtime") and SQLite.
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

#ifndef SNMPMGR_MEASURE_HPP
#define SNMPMGR_MEASURE_HPP

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace snmpmgr
{
  namespace measure
  {
    using json = nlohmann::json;

    /*!
     * @~japanese
     * @brief slave の計測ファイル (data) の RTC を runs (キー -> 計測 1 回分) に入れる
     *
     * キーは slave マネージャ名/インスタンス名/生成時刻 (同じ RTC を作り直せば別の回として残す)。
     *
     * @param runs キー -> 計測 1 回分
     * @param robot ロボット名
     * @param data 計測ファイルの内容
     * @return 更新したキーの一覧
     * @~english
     * @brief Puts the RTCs in a slave's measurement file (data) into runs (key -> one measurement run)
     *
     * The key is slave manager name/instance name/creation time (a recreated RTC is kept as a separate run).
     *
     * @param runs key -> one measurement run
     * @param robot robot name
     * @param data contents of the measurement file
     * @return list of updated keys
     * @~
     */
    std::vector<std::string> update_runs(json& runs, const std::string& robot, const json& data);

    /*!
     * @~japanese
     * @brief runs を (言語, 型名) ごとにまとめる。新しい max_runs 回分、標本数が min_samples 以上の回だけ使う
     *
     * @param runs 計測
     * @param min_samples 使う回の最小の標本数
     * @param max_runs 使う回の数
     * @param cpu_stat CPU の値に使う統計 (avg / max / p95)
     * @return {(言語, 型名): まとめた結果}
     * @~english
     * @brief Aggregates runs by (language, type name). Uses only the latest max_runs runs with at least min_samples samples
     *
     * @param runs measurements
     * @param min_samples minimum number of samples of a run to use
     * @param max_runs number of runs to use
     * @param cpu_stat statistic for the CPU value (avg / max / p95)
     * @return {(language, type name): aggregated result}
     * @~
     */
    std::map<std::pair<std::string, std::string>, json> aggregate(const json& runs, int min_samples = 10,
                                                                 int max_runs = 20, const std::string& cpu_stat = "max");

    /*!
     * @~japanese
     * @brief aggregate() の 1 件を placement.xml (ProfileDB が読む形式) にする
     *
     * @param agg aggregate() の 1 件
     * @param environment 環境の名前
     * @param robot ロボット名
     * @return XML の文字列
     * @~english
     * @brief Renders one item of aggregate() as placement.xml (the format read by ProfileDB)
     *
     * @param agg one item of aggregate()
     * @param environment name of the environment
     * @param robot robot name
     * @return XML string
     * @~
     */
    std::string render_placement(const json& agg, const std::string& environment, const std::string& robot);

    /*!
     * @~japanese
     * @brief ファイル名に使えない文字を置き換える
     *
     * @param text 文字列
     * @return ファイル名に使える文字列
     * @~english
     * @brief Replaces characters that cannot be used in file names
     *
     * @param text string
     * @return string usable as a file name
     * @~
     */
    std::string safe_name(const std::string& text);

    // ---- slave 側の計測 (snmpmgr_measure.so。Python 版 snmpmgr_measure.py の attribute_* / summarize と同じ)
    /*!
     * @~japanese
     * @brief プロセスの CPU 使用率を RTC ごとに分ける (slave 側の計測。Python 版の attribute_cpu と同じ)
     *
     * 残り (proc_cpu - 合計) をスレッドの使用率の比で (すべて 0 なら等分で) 足す。
     *
     * @param proc_cpu プロセス全体の CPU 使用率
     * @param ec_cpu RTC -> 実行コンテキストのスレッドの使用率
     * @return RTC -> CPU 使用率
     * @~english
     * @brief Divides the CPU usage of the process among RTCs (slave-side measurement; same as attribute_cpu of the Python version)
     *
     * The rest (proc_cpu - total) is added in proportion to the thread usages (equally if all are 0).
     *
     * @param proc_cpu CPU usage of the whole process
     * @param ec_cpu RTC -> usage of the execution context threads
     * @return RTC -> CPU usage
     * @~
     */
    std::map<std::string, double> attribute_cpu(double proc_cpu, const std::map<std::string, double>& ec_cpu);
    /*!
     * @~japanese
     * @brief USS の増分 (uss - baseline) を、生成したときの増分の比で RTC ごとに分ける
     *
     * @param uss_mb プロセスの USS (MB)
     * @param baseline_mb 最初の RTC を置く直前の USS (MB)
     * @param increments RTC -> 生成したときの増分 (MB)
     * @return RTC -> メモリ (MB)
     * @~english
     * @brief Divides the USS increment (uss - baseline) among RTCs in proportion to the increments at creation
     *
     * @param uss_mb USS of the process (MB)
     * @param baseline_mb USS just before the first RTC (MB)
     * @param increments RTC -> increment at creation (MB)
     * @return RTC -> memory (MB)
     * @~
     */
    std::map<std::string, double> attribute_memory(double uss_mb, double baseline_mb,
                                                   const std::map<std::string, double>& increments);
    /*!
     * @~japanese
     * @brief 標本の平均・最大・95 パーセンタイルを求める
     *
     * @param values 標本
     * @return {"avg","max","p95"} (標本がなければ null)
     * @~english
     * @brief Computes the average, maximum and 95th percentile of samples
     *
     * @param values samples
     * @return {"avg","max","p95"} (null if there are no samples)
     * @~
     */
    json summarize(std::vector<double> values);
  } // namespace measure
} // namespace snmpmgr

#endif
