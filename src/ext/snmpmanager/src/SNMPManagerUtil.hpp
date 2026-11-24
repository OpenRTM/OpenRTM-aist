// -*- C++ -*-
/*!
 * @file SNMPManagerUtil.hpp
 * @brief Small common functions (time, strings, logging)
 *
 * @~japanese
 * 共通の小さな関数 (時刻・文字列・ログ)
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

#ifndef SNMPMGR_UTIL_HPP
#define SNMPMGR_UTIL_HPP

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief 単調な時計の時刻 (秒) を返す
   *
   * @return 秒
   * @~english
   * @brief Returns the time of the monotonic clock (seconds)
   *
   * @return seconds
   * @~
   */
  inline double monotonic()
  {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
  }

  /*!
   * @~japanese
   * @brief 実時間の時刻 (UNIX 時刻、秒) を返す
   *
   * @return 秒
   * @~english
   * @brief Returns the wall-clock time (UNIX time in seconds)
   *
   * @return seconds
   * @~
   */
  inline double wallclock()
  {
    return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

  /*!
   * @~japanese
   * @brief 前後の空白を除く
   *
   * @param s 文字列
   * @return 文字列
   * @~english
   * @brief Removes leading and trailing whitespace
   *
   * @param s string
   * @return string
   * @~
   */
  inline std::string trim(const std::string& s)
  {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) { return ""; }
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
  }

  /*!
   * @~japanese
   * @brief 文字列を区切る (空の要素は除く)
   *
   * @param s 文字列
   * @param sep 区切り文字
   * @param do_trim 要素の前後の空白を除くか
   * @return 要素のリスト
   * @~english
   * @brief Splits a string (empty elements are dropped)
   *
   * @param s string
   * @param sep separator
   * @param do_trim whether to trim each element
   * @return list of elements
   * @~
   */
  inline std::vector<std::string> split(const std::string& s, char sep, bool do_trim = true)
  {
    std::vector<std::string> out;
    std::string cur;
    std::istringstream is(s);
    while (std::getline(is, cur, sep))
      {
        if (do_trim) { cur = trim(cur); }
        if (!cur.empty()) { out.push_back(cur); }
      }
    return out;
  }

  /*!
   * @~japanese
   * @brief 文字列が p で始まるか
   *
   * @param s 文字列
   * @param p 前置き
   * @return 始まれば true
   * @~english
   * @brief Whether a string starts with p
   *
   * @param s string
   * @param p prefix
   * @return true if it starts with p
   * @~
   */
  inline bool starts_with(const std::string& s, const std::string& p)
  {
    return s.compare(0, p.size(), p) == 0;
  }

  /*!
   * @~japanese
   * @brief 大文字にする
   *
   * @param s 文字列
   * @return 大文字の文字列
   * @~english
   * @brief Converts to upper case
   *
   * @param s string
   * @return upper-case string
   * @~
   */
  inline std::string upper(std::string s)
  {
    for (auto& c : s) { c = static_cast<char>(std::toupper(static_cast<unsigned char>(c))); }
    return s;
  }

  /*!
   * @~japanese
   * @brief 文字列を数値にする
   *
   * @param s 文字列
   * @return 数値 (できなければ nullopt)
   * @~english
   * @brief Converts a string into a number
   *
   * @param s string
   * @return number (nullopt if not possible)
   * @~
   */
  inline std::optional<double> to_double(const std::string& s)
  {
    try
      {
        size_t pos = 0;
        double v = std::stod(trim(s), &pos);
        return v;
      }
    catch (...) { return std::nullopt; }
  }

  /*!
   * @~japanese
   * @brief 数値を小数点以下 digits 桁の文字列にする
   *
   * @param v 数値
   * @param digits 小数点以下の桁数
   * @return 文字列
   * @~english
   * @brief Formats a number with digits decimal places
   *
   * @param v number
   * @param digits number of decimal places
   * @return string
   * @~
   */
  inline std::string fmt(double v, int digits = 1)
  {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.*f", digits, v);
    return buf;
  }

  /*!
   * @~japanese
   * @brief 今の時刻を ISO 8601 の文字列にする
   *
   * @return 文字列
   * @~english
   * @brief Formats the current time as an ISO 8601 string
   *
   * @return string
   * @~
   */
  inline std::string iso_now()
  {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", std::localtime(&t));
    return buf;
  }

  /*!
   * @~japanese
   * @brief "[SNMPManager:\<name\>] message" を標準出力へ出す (docker logs で見る)
   * @~english
   * @brief Outputs "[SNMPManager:\<name\>] message" to stdout (seen with docker logs)
   * @~
   */
  class Log
  {
  public:
    /*!
     * @~japanese
     * @brief ログに出す名前を設定する
     *
     * @param n 名前
     * @~english
     * @brief Sets the name shown in logs
     *
     * @param n name
     * @~
     */
    static void set_name(const std::string& n) { name() = n; }
    /*!
     * @~japanese
     * @brief メッセージを出す
     *
     * @param msg メッセージ
     * @~english
     * @brief Outputs a message
     *
     * @param msg message
     * @~
     */
    static void info(const std::string& msg)
    {
      std::lock_guard<std::mutex> g(mutex());
      std::cout << "[SNMPManager:" << name() << "] " << msg << std::endl;
    }
  private:
    static std::string& name() { static std::string n = "?"; return n; }
    static std::mutex& mutex() { static std::mutex m; return m; }
  };
} // namespace snmpmgr

#endif
