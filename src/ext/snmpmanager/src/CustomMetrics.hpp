// -*- C++ -*-
/*!
 * @file CustomMetrics.hpp
 * @brief Additional monitoring values (custom metrics). Same as the Python version CustomMetrics.py
 *
 * @~japanese
 * 追加の監視の値 (カスタムメトリクス)。Python 版 CustomMetrics.py と同じ
 *
 * @verbatim
 * rtc.conf: snmp.metrics: temperature, motor_current
 *           snmp.metric.temperature.extend: cpuTemp           (snmpd の extend の名前)
 *           snmp.metric.motor_current.oid: 1.3.6.1.4.1....    (または OID)
 * 構成ファイル: 条件の metric、recovery.conditions の "<名前>_max" / "<名前>_min"、
 *               metric_constraints (置くロボット)、executor_constraints (実行ロボット)
 * @endverbatim
 * @~english
 * @verbatim
 * rtc.conf: snmp.metrics: temperature, motor_current
 *           snmp.metric.temperature.extend: cpuTemp           (name of extend in snmpd)
 *           snmp.metric.motor_current.oid: 1.3.6.1.4.1....    (or an OID)
 * Configuration file: metric of conditions, "<name>_max" / "<name>_min" of recovery.conditions,
 *                     metric_constraints (robots to place on), executor_constraints (executor robot)
 * @endverbatim
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

#ifndef SNMPMGR_METRICS_HPP
#define SNMPMGR_METRICS_HPP

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace snmpmgr
{
  namespace metrics
  {
    using json = nlohmann::json;
    using Getter = std::function<std::string(const std::string& key, const std::string& def)>;
    using Values = std::map<std::string, std::optional<double>>;

    /*!
     * @~japanese
     * @brief 制約を満たさないもの (キー、名前、値、演算子、しきい値)
     * @~english
     * @brief A violated constraint (key, name, value, operator, threshold)
     * @~
     */
    struct Violation
    {
      std::string key;     // "temperature_max"
      std::string name;    // "temperature"
      double value{0};
      std::string op;      // ">" か "<"
      double limit{0};
    };

    /*!
     * @~japanese
     * @brief NET-SNMP-EXTEND-MIB\::nsExtendOutput1Line."token" の OID を返す
     *
     * @param token extend の名前
     * @return OID
     * @~english
     * @brief Returns the OID of NET-SNMP-EXTEND-MIB\::nsExtendOutput1Line."token"
     *
     * @param token name of the extend
     * @return OID
     * @~
     */
    std::string extend_output_oid(const std::string& token);
    /*!
     * @~japanese
     * @brief rtc.conf からカスタムメトリクスの設定を読む。設定が正しくなければ std::runtime_error
     *
     * @param get rtc.conf の値を返す関数
     * @return [(名前, OID)] (snmp.metrics の順)
     * @~english
     * @brief Reads the custom metrics settings from rtc.conf. std::runtime_error if invalid
     *
     * @param get function returning values of rtc.conf
     * @return [(name, OID)] (in the order of snmp.metrics)
     * @~
     */
    std::vector<std::pair<std::string, std::string>> parse_config(const Getter& get);
    /*!
     * @~japanese
     * @brief SNMP の値を数値にする (前後の空白と末尾の NUL は無視)
     *
     * @param raw SNMP の値
     * @return 数値 (数値でなければ nullopt)
     * @~english
     * @brief Converts an SNMP value into a number (leading/trailing spaces and a trailing NUL are ignored)
     *
     * @param raw SNMP value
     * @return number (nullopt if not a number)
     * @~
     */
    std::optional<double> parse_value(const std::string& raw);
    /*!
     * @~japanese
     * @brief 制約のキーを返す: "\<名前\>_max" -> (名前, ">")、"\<名前\>_min" -> (名前, "<")
     *
     * @param names カスタムメトリクスの名前
     * @return キーの表
     * @~english
     * @brief Returns the constraint keys: "\<name\>_max" -> (name, ">"), "\<name\>_min" -> (name, "<")
     *
     * @param names names of the custom metrics
     * @return table of keys
     * @~
     */
    std::map<std::string, std::pair<std::string, std::string>> constraint_keys(const std::vector<std::string>& names);
    /*!
     * @~japanese
     * @brief 制約 ({"temperature_max": 70}) を確かめる。正しくなければ std::runtime_error (null・なしは何もしない)
     *
     * @param constraints 制約
     * @param names カスタムメトリクスの名前
     * @param where エラーのメッセージに出す場所
     * @~english
     * @brief Validates constraints ({"temperature_max": 70}). std::runtime_error if invalid (does nothing for null / absent)
     *
     * @param constraints constraints
     * @param names names of the custom metrics
     * @param where location shown in the error message
     * @~
     */
    void validate_constraints(const json& constraints, const std::vector<std::string>& names, const std::string& where);
    /*!
     * @~japanese
     * @brief 制約を満たさないものを返す。値が分からない (nullopt) 指標は見ない
     *
     * @param values 名前 -> 値
     * @param constraints 制約
     * @param names カスタムメトリクスの名前
     * @return Violation のリスト
     * @~english
     * @brief Returns the violated constraints. Metrics with unknown values (nullopt) are ignored
     *
     * @param values name -> value
     * @param constraints constraints
     * @param names names of the custom metrics
     * @return list of Violation
     * @~
     */
    std::vector<Violation> violations(const Values& values, const json& constraints, const std::vector<std::string>& names);
    /*!
     * @~japanese
     * @brief Violation を文字列にする (例: "temperature 75 > 70")
     *
     * @param v Violation
     * @return 文字列
     * @~english
     * @brief Converts a Violation into a string (e.g. "temperature 75 > 70")
     *
     * @param v Violation
     * @return string
     * @~
     */
    std::string describe(const Violation& v);
  } // namespace metrics
} // namespace snmpmgr

#endif
