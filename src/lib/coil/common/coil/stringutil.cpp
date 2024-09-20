// -*- C++ -*-
/*!
 * @file StringUtil.cpp
 * @brief String operation utility
 * @date $Date: 2007-12-31 03:08:07 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: StringUtil.cpp 826 2008-08-26 08:13:39Z n-ando $
 *
 */

#include <coil/stringutil.h>

#include <array>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <cctype>
#include <regex>
#include <utility>
#include <unordered_set>
#include <coil/OS.h>

#ifdef __QNX__
using std::toupper;
using std::isalpha;
#endif

namespace coil
{
  /*!
   * @if jp
   * @brief string から wstring への変換
   * @else
   * @brief string to wstring conversion
   * @endif
   */
  std::wstring string2wstring(const std::string& str)
  {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    int buff_size = MultiByteToWideChar(CP_UTF7, 0, str.c_str(), -1, (wchar_t*)nullptr, 0);
    wchar_t* ret = new wchar_t[buff_size];
    MultiByteToWideChar(CP_UTF7, 0, str.c_str(), -1, ret, buff_size);
    std::wstring wstr(ret, ret + buff_size - 1);
    delete[] ret;
#else
    std::wstring wstr(str.length(), L' ');
    std::copy(str.begin(), str.end(), wstr.begin());
#endif
    return wstr;
  }

  /*!
   * @if jp
   * @brief wstring から string への変換
   * @else
   * @brief wstring to string conversion
   * @endif
   */
  std::string wstring2string(const std::wstring& wstr)
  {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
      int buff_size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, (char*)nullptr, 0, nullptr, nullptr);
      CHAR* ret = new CHAR[buff_size];
      WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, ret, buff_size, nullptr, nullptr);
      std::string str(ret, ret + buff_size - 1);
      delete[] ret;
#else
      std::string str(wstr.length(), ' ');
      std::copy(wstr.begin(), wstr.end(), str.begin());
#endif
    return str;
  }

  /*!
   * @if jp
   * @brief 大文字への変換
   * @else
   * @brief Uppercase String Transformation
   * @endif
   */
  std::string toUpper(std::string str) noexcept
  {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char x){
      return static_cast<char>(std::toupper(x));
    });
    return str;
  }

  /*!
   * @if jp
   * @brief 小文字への変換
   * @else
   * @brief Lowercase String Transformation
   * @endif
   */
  std::string toLower(std::string str) noexcept
  {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char x){
      return static_cast<char>(std::tolower(x));
    });
    return str;
  }

  /*!
   * @if jp
   * @brief 入力ストリームから1行読み込む
   * @else
   * @brief Read a line from input stream
   * @endif
   */
  std::string getlinePortable(std::istream& istr)
  {
    char c;
    std::stringstream s;

    while (istr.get(c))
      {
        if (c == '\n')
          {
            break;
          }
        else if (c == '\r')
          {
            if (istr.peek() == '\n')
              {
                istr.ignore();
              }
            break;
          }
        else
          {
            s << c;
          }
      }
    return s.str();
  }

  /*!
   * @if jp
   * @brief 文字列がエスケープされているか判断する
   * @else
   * @brief Check whether the character is escaped or not
   * @endif
   */
  bool isEscaped(const std::string& str, std::string::size_type pos)
  {
    if (pos == 0) { return false; }
    --pos;
    size_t i = 0;
    for ( ; str[pos] == '\\'; ++i)
      {
        if (pos == 0) { break; }
        --pos;
      }
    // If the number of \ is odd, delimiter is escaped.
    return (i % 2) == 1;
  }

  /*!
   * @if jp
   * @brief 文字列をエスケープする
   * @else
   * @brief Escape string
   * @endif
   */
  std::string escape(const std::string& str)
  {
    std::string ret;
    ret.reserve(str.length()*2);
    std::for_each(str.begin(), str.end(), [&ret](char c) {
      switch (c)
      {
        case '\t': ret += "\\t"; break;
        case '\n': ret += "\\n"; break;
        case '\f': ret += "\\f"; break;
        case '\r': ret += "\\r"; break;
        case '\\': ret += "\\\\"; break;
        default: ret += c; break;
      }
    });
    return ret;
   }

  /*!
   * @if jp
   * @brief 文字列のエスケープを戻す
   * @else
   * @brief Unescape string
   * @endif
   */
  std::string unescape(std::string str) noexcept
  {
    std::string::size_type wp{0};
    bool is_escaped{false};

    for(std::string::size_type rp{0}; rp < str.length(); ++rp)
      {
        if (!is_escaped)
          {
            if (str[rp] != '\\')
              str[wp++] = str[rp];
            else
              is_escaped = true;
          }
        else
          {
            is_escaped = false;
            switch (str[rp])
            {
              case 't':  str[wp++] = '\t'; break;
              case 'n':  str[wp++] = '\n'; break;
              case 'f':  str[wp++] = '\f'; break;
              case 'r':  str[wp++] = '\r'; break;
              case '\"': str[wp++] = '\"'; break;
              case '\'': str[wp++] = '\''; break;
              default: str[wp++] = str[rp]; break;
            }
          }
      }
    str.resize(wp);
    return str;
  }

  /*!
   * @if jp
   * @brief 文字列の空白文字を削除する
   * @else
   * @brief Erase blank characters of string
   * @endif
   */
  std::string eraseBlank(std::string str) noexcept
  {
    str.erase(std::remove_if(str.begin(), str.end(),
                [](unsigned char x){ return std::isblank(x); }),
              str.end());
    return str;
  }

  /*!
   * @if jp
   * @brief 文字列の先頭の空白文字を削除する
   * @else
   * @brief Erase the head blank characters of string
   * @endif
   */
  std::string eraseHeadBlank(std::string str) noexcept
  {
    return str.erase(0, str.find_first_not_of(" \t"));
  }

  /*!
   * @if jp
   * @brief 文字列の末尾の空白文字を削除する
   * @else
   * @brief Erase the tail blank characters of string
   * @endif
   */
  std::string eraseTailBlank(std::string str) noexcept
  {
    return str.erase(str.find_last_not_of(" \t") + 1); // npos + 1 = 0
  }

  /*!
   * @if jp
   * @brief 文字列の先頭・末尾の空白文字を削除する
   * @else
   * @brief Erase the head blank and the tail blank characters of string
   * @endif
   */
  std::string eraseBothEndsBlank(std::string str) noexcept
  {
    return eraseHeadBlank(eraseTailBlank(std::move(str)));
  }

  /*!
   * @if jp
   * @brief 文字列を正規化する
   * @else
   * @brief Erase the head/tail blank and replace upper case to lower case
   * @endif
   */
  std::string normalize(std::string str) noexcept
  {
    return toLower(eraseBothEndsBlank(std::move(str)));
  }

  /*!
   * @if jp
   * @brief 文字列を置き換える
   * @else
   * @brief Replace string
   * @endif
   */
  std::string replaceString(std::string str, const std::string& from,
                            const std::string& to)
  {
    if (from.empty()) { return str; }
    for (std::string::size_type pos{str.find(from, 0)};
         pos != std::string::npos;
         pos = str.find(from, pos + to.length()))
      {
        str.replace(pos, from.length(), to);
      }
    return str;
  }

  /*!
   * @if jp
   * @brief 文字列を分割文字で分割する
   * @else
   * @brief Split string by delimiter
   * @endif
   */
  vstring split(const std::string& input,
                const std::string& delimiter,
                bool ignore_empty)
  {
    if (input.empty()) { return {}; }
    if (delimiter.empty()) { return {eraseBothEndsBlank(input)}; }

    vstring results;
    std::string::size_type pos{0};
    for (auto found_pos = input.find(delimiter, pos);
         found_pos != std::string::npos;
         found_pos = input.find(delimiter, pos))
      {
        std::string str{eraseBothEndsBlank(input.substr(pos, found_pos - pos))};
        if (!(ignore_empty && str.empty())) { results.emplace_back(std::move(str)); }
        pos = found_pos + delimiter.length();
      }

    std::string str{eraseBothEndsBlank(input.substr(pos))};
    if (!(ignore_empty && str.empty())) { results.emplace_back(std::move(str)); }
    return results;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列をbool値に変換する
   * @else
   * @brief Convert given string into bool value
   * @endif
   */
  bool toBool(std::string str, std::string yes, std::string no,
              bool default_value)
  {
    std::string s = toUpper(std::move(str));

    if (s.find(toUpper(std::move(yes))) != std::string::npos)
      return true;
    else if (s.find(toUpper(std::move(no))) != std::string::npos)
      return false;
    else
      return default_value;
  }

  /*!
   * @if jp
   * @brief 文字列リスト中にある文字列が含まれるかどうかを判断する
   * @else
   * @brief Include if a string is included in string list
   * @endif
   */
  bool includes(const vstring& list, std::string value, bool ignore_case)
  {
    std::string const& v{ignore_case ? toLower(std::move(value)) : value};
    for (auto const& str : list)
      {
        std::string const& x{ignore_case ? toLower(str) : str};
        if (v == x) return true;
      }
    return false;
  }

  /*!
   * @if jp
   * @brief 文字列リスト中にある文字列が含まれるかどうかを判断する
   * @else
   * @brief Include if a string is included in string list
   * @endif
   */
  bool includes(const std::string& list, std::string value, bool ignore_case)
  {
    return includes(split(list, ","), std::move(value), ignore_case);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列が絶対パスかどうかを判断する
   * @else
   * @brief Investigate whether the given string is absolute path or not
   * @endif
   */
  bool isAbsolutePath(const std::string& str)
  {
    static std::regex const path{
      "(?:"
        "^/" // UNIX absolute path
        R"(|^[a-zA-Z]{1}:\\)" // Windows absolute path
        R"(|^[a-zA-Z]{1}:/)" // Windows absolute path
        R"(|^\\\\)" // Windows network file path
      ")"} ;
    return std::regex_search(str, path);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列がURLかどうかを判断する
   * @else
   * @brief Investigate whether the given string is URL or not
   * @endif
   */
  bool isURL(const std::string& str)
  {
    static std::regex const url{R"(\w://)"};
    return std::regex_search(str, url);
  }

  /*!
   * @if jp
   * @brief 文字列を引数として解釈する
   * @else
   * @brief Parse string as argument list
   * @endif
   */
  vstring parseArgs(const std::string &args)
  {
    bool inArg(false);    // -> " " or "\t"
    bool inEscape(false); // -> "\"
    bool inDquote(false); // -> "\""
    bool inSquote(false); // -> "\'"
    vstring ret;
    std::string anArg;

    for (size_t i(0); i < args.size(); ++i)
      {
        if (args[i] == ' ' || args[i] == '\t')
          {
            // skip escaped spaces and/or spces in quotation
            if (inEscape || inDquote || inSquote)
              {
                anArg.push_back(args[i]);
                continue;
              }
            // skip spaces between args
            if (!inArg) { continue; }
            // end of arg
            if (inArg)
              {
                ret.push_back(anArg);
                anArg.clear();
                inArg = false; // exit arg
                continue;
              }
          }
        inArg = true;

        if (args[i] == '\\')
          {
            if (inEscape) { anArg.push_back(args[i]); }
            inEscape = !inEscape;
            continue;
          }

        if (args[i] == '\"')
          { // escaped (") is stored in arg in any cases
            if (inEscape)
              {
                inEscape = false;
                if (inSquote) { anArg.push_back('\\'); }
                anArg.push_back(args[i]);
                continue;
              }
            // (") in S-quote is stored in arg
            if (inSquote)
              {
                anArg.push_back(args[i]);
                continue;
              }
            // inDquote: enter(false->true), exit(true->false)
            inDquote = !inDquote;
            continue;
          }

        if (args[i] == '\'')
          { // escaped (') is stored in arg in any cases
            if (inEscape)
              {
                inEscape = false;
                if (inDquote) { anArg.push_back('\\'); }
                anArg.push_back(args[i]);
                continue;
              }
            // (') in S-quote is stored in arg
            if (inDquote)
              {
                anArg.push_back(args[i]);
                continue;
              }
            // inSquote: enter(false->true), exit(true->false)
            inSquote = !inSquote;
            continue;
          }

        // here arg[i] != (' ') or (\t) or (") or (')
        if (inEscape)
          {
            inEscape = false;
            if (inDquote || inSquote) { anArg.push_back('\\'); }
          }
        anArg.push_back(args[i]);
        continue;
      }
    ret.push_back(anArg);
    return ret;
  }

  bool isIPv4(const std::string& str)
  {
    // IPv4 address must be dotted-decimal format.
    // not support: 0x1.0x1.0x1.0x1, 01.01.01.01, 100.100000
    static std::regex const ipv4{
      "(?:(?:"
        R"(\d)"        // x
        R"(|[1-9]\d)"  // Xx
        R"(|1\d\d)"    // 1xx
        R"(|2[0-4]\d)" // 2Xx
        R"(|25[0-5])"  // 25x
      ")\\.){3}" // <num>.<num>.<num>.
      "(?:"
        R"(\d)"        // x
        R"(|[1-9]\d)"  // Xx
        R"(|1\d\d)"    // 1xx
        R"(|2[0-4]\d)" // 2Xx
        R"(|25[0-5])"  // 25X
      ")"}; // <num>
    return std::regex_match(str, ipv4);
  }

  bool isIPv6(const std::string& str)
  {
    // IPv6 address must be
    // 1111:1111:1111:1111:1111:1111:1111:1111
    static std::regex const ipv6{
      "(?:[0-9a-fA-F]{1,4}:){7}" // xxxx:xxxx: ...
      "[0-9a-fA-F]{1,4}"}; // xxxx
    return std::regex_match(str, ipv6);
  }

  bool isIPPort(const std::string& str)
  {
    static std::regex const port{
      "(?:"
        R"(\d)"            // x
        R"(|[1-9]\d{1,3})" // Xx, Xxx, Xxxx
        R"(|[1-5]\d{4})"   // Xxxxx
        R"(|6[0-4]\d{3})"  // 6Xxxx
        R"(|65[0-4]\d{2})" // 65Xxx
        R"(|655[0-2]\d)"   // 655Xx
        R"(|6553[0-5])"    // 6553X
      ")"};
    return std::regex_match(str, port);
  }

  /*!
   * @if jp
   * @brief URLパラメータをmapstringに分解して返す
   *
   * URLパラメータ表現 something?key0=value0&key1=value1.... のうち
   * '?' 以降の部分を分解して、std::map<std::string, std::string> 形式
   * に変換する。与えられた文字列を左からサーチし、'?' より右側の部分に
   * ついて解析を行う。'&'で分割し、左から '=' を検索し、最初の '=' の
   * 右辺と左辺をそれぞれ、key と value として map に格納する。
   *
   * @param str 分解対象文字列
   * @return mapstring 型の key/valueデータ
   *
   * @else
   * @brief Investigate whether the given string is URL or not
   *
   * URL parameter description such as
   * something?key0=value0&key1=value1.... is analyzed. Right hand
   * side string of '?' character is decomposed and it is converted
   * into std::map<std::string, std::string> type.The following string
   * are devided by '&' and then '=' character is
   * searched. Right-hand-side value and left-hand-side value of '='
   * are stored as key and value in the map.
   *
   * @param str The target string for decomposed
   *
   * @return decomposed key-values in map
   *
   * @endif
   */
  coil::mapstring urlparam2map(const std::string& str)
  {
    std::string::size_type qpos = str.find('?');
    if (qpos == std::string::npos) { qpos = 0; }
    else { ++qpos; }
    coil::vstring params = coil::split(str.substr(qpos), "&");

    std::map<std::string, std::string> retmap;
    for (auto& param : params)
    {
      if (coil::eraseBothEndsBlank(param).empty())
      {
        continue;
      }
      std::string::size_type pos = param.find('=');
      if (pos != std::string::npos)
      {
        const std::string key{ param.substr(0, pos) };
        if (!coil::eraseBothEndsBlank(key).empty())
        {
          retmap[key] = param.substr(pos + 1);
        }
      }
    }
    return retmap;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列をstd::stringに変換
   * @else
   * @brief Convert the given string to std::string.
   * @endif
   */
  template<>
  bool stringTo<std::string>(std::string& val, const char* str)
  {
    if (str == nullptr) { return false; }
    val = str;
    return true;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列をboolに変換
   * @else
   * @brief Convert the given string to bool.
   * @endif
   */
  template <>
  bool stringTo<bool>(bool& val, const char* str)
  {
    if (str == nullptr) { return false; }
    std::string boolstr{coil::normalize(str)};
    if (boolstr == "true" || boolstr == "1" ||
        boolstr == "yes"  || boolstr == "on")
      {
        val = true;
        return true;
      }
    else if (boolstr == "false" || boolstr == "0" ||
             boolstr == "no"    || boolstr == "off")
      {
        val = false;
        return true;
      }
    else
      {
      }
    return false;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列をstd::chrono::duratoin<double>に変換
   * @else
   * @brief Convert the given string to std::chrono::duratoin<double>.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::duration<double>>(std::chrono::duration<double>& val,
                                               const char* str)
  {
    double num;
    if (stringTo(num, str)) {
      val = std::chrono::duration<double>(num);
      return true;
    } else {
      return false;
    }
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::duratoin<>に変換
   * @else
   * @brief Convert the given string to std::chrono::duratoin<>.
   * @endif
   */
  template<typename duration>
  bool stringToDuration(duration& val, const char* str)
  {
    std::chrono::duration<double> val_d;
    if (stringTo(val_d, str)) {
      val = std::chrono::duration_cast<duration>(val_d);
      return true;
    } else {
      return false;
    }
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::nanosecondsに変換
   * @else
   * @brief Convert the given string to std::chrono::nanoseconds.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::nanoseconds>(std::chrono::nanoseconds& val,
                                          const char* str)
  {
    return stringToDuration(val, str);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::microsecondsに変換
   * @else
   * @brief Convert the given string to std::chrono::microseconds.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::microseconds>(std::chrono::microseconds& val,
                                           const char* str)
  {
    return stringToDuration(val, str);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::millisecondsに変換
   * @else
   * @brief Convert the given string to std::chrono::milliseconds.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::milliseconds>(std::chrono::milliseconds& val,
                                           const char* str)
  {
    return stringToDuration(val, str);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::secondsに変換
   * @else
   * @brief Convert the given string to std::chrono::seconds.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::seconds>(std::chrono::seconds& val,
                                      const char* str)
  {
    return stringToDuration(val, str);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::minutesに変換
   * @else
   * @brief Convert the given string to std::chrono::minutes.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::minutes>(std::chrono::minutes& val,
                                      const char* str)
  {
    return stringToDuration(val, str);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列 (実数､単位[s]) をstd::chrono::hoursに変換
   * @else
   * @brief Convert the given string to std::chrono::hours.
   * @endif
   */
  template<>
  bool stringTo<std::chrono::hours>(std::chrono::hours& val, const char* str)
  {
    return stringToDuration(val, str);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列リストから重複を削除
   * @else
   * @brief Eliminate duplication from the given string list
   * @endif
   */
  vstring unique_sv(vstring sv)
  {
    std::unordered_set<std::string> set{std::make_move_iterator(sv.begin()),
                                        std::make_move_iterator(sv.end()),
                                        sv.size()};
#if (defined(__GNUC__) && (__GNUC__ < 5) && !defined(__clang__))    \
    || (defined(_MSC_VER) && (_MSC_VER < 1900))
    sv.clear();
    for (auto&& itr : set) { sv.emplace_back(std::move(itr)); }
#else
    sv.assign(std::make_move_iterator(set.begin()),
              std::make_move_iterator(set.end()));
#endif
    return sv;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列リストからCSVを生成
   * @else
   * @brief Create CSV file from the given string list
   * @endif
   */
  std::string flatten(vstring sv, const std::string& delimiter)
  {
    if (sv.empty()) { return ""; }

    std::string str = std::string();
    for (size_t i(0), len(sv.size() - 1); i < len; ++i)
      {
        str += sv[i] + delimiter;
      }
    return str + sv.back();
  }

  /*!
   * @if jp
   * @brief 与えられた文字列リストを引数リストに変換
   * @else
   * @brief Convert the given string list into the argument list
   * @endif
   */
  Argv::Argv(const vstring& args)
  {
    // make m_args.
    m_args.reserve(args.size());
    for (auto&& arg : args)
      {
        m_args.emplace_back(arg.c_str(), arg.c_str() + arg.size() + 1);
      }
    // make m_argv.
    m_argv.reserve(m_args.size() + 1);
    for(auto&& arg : m_args)
      {
        m_argv.emplace_back(arg.data());
      }
    m_argv.emplace_back(nullptr);
  }

#if defined(_MSC_VER) && (_MSC_VER < 1900) // Visual Studio 2013
  Argv::Argv(Argv&& x) noexcept { *this = std::move(x); }

  Argv& Argv::operator=(Argv&& x) noexcept
  {
    m_argv = std::move(x.m_argv);
    m_args = std::move(x.m_args);
    return *this;
  }
#endif

  Argv::~Argv() = default; // No inline for gcc warning, too big.


    /*!
   * @if jp
   * @brief 指定された書式に変換
   * @else
   * @brief Convert it into a format given with an argumen
   * @endif
   */
  std::string sprintf(char const * __restrict fmt, ...)
  {
    std::array<char, 1024> str;
    va_list ap;

    va_start(ap, fmt);
#ifdef WIN32
    _vsnprintf_s(&str[0], str.size() - 1, _TRUNCATE, fmt, ap);
#else
    vsnprintf(&str[0], str.size() - 1, fmt, ap);
#endif
    va_end(ap);
    return &str[0];
  }


  /*!
  * @if jp
  * @brief 文字列中の環境変数を置き換える
  *
  * 文字列中に${}で囲まれた文字列がある場合に、環境変数と置き換える
  * 例：${RTM_ROOT}\bin -> C:\Program Files (x86)\OpenRTM-aist\1.1.2\
  *
  * @param str 置き換え前の文字列
  * @return 置き換え後の文字列
  *
  * @else
  * @brief
  *
  * @param str
  * @return
  *
  * @return
  *
  * @endif
  */
  std::string replaceEnv(std::string str)
  {
    static std::regex const e{R"(\$\{(\w+)\})"}; // ${xxx}
    std::smatch m;
    while (std::regex_search(str, m, e))
    {
      std::string env;
      coil::getenv(m.str(1).c_str(), env); // env="" if getenv() fail.
      str.replace(m.position(), m.length(), env);
    }
    return str;
  }
} // namespace coil
