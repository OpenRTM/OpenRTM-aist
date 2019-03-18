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

#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdio>
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
  std::wstring string2wstring(std::string str)
  {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    int buff_size = MultiByteToWideChar(CP_UTF7, 0, str.c_str(), -1, (wchar_t*)NULL, 0);
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
  std::string wstring2string(std::wstring wstr)
  {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
      int buff_size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, (char*)NULL, 0, NULL, NULL);
      CHAR* ret = new CHAR[buff_size];
      WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, ret, buff_size, NULL, NULL);
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
   * @brief std::toupperをchar型引数、char型戻り値でラップした関数
   * @else
   * @brief
   * @endif
   */
  char toupper_(char c)
  {
      return static_cast<char>(std::toupper(static_cast<int>(c)));
  }

  /*!
   * @if jp
   * @brief 大文字への変換
   * @else
   * @brief Uppercase String Transformation
   * @endif
   */
  void toUpper(std::string& str)
  {
    std::transform(str.begin(), str.end(), str.begin(),
                   toupper_);
  }

  /*!
   * @if jp
   * @brief std::tolowerをchar型引数、char型戻り値でラップした関数
   * @else
   * @brief 
   * @endif
   */
  char tolower_(char c)
  {
      return static_cast<char>(std::tolower(static_cast<int>(c)));
  }

  /*!
   * @if jp
   * @brief 小文字への変換
   * @else
   * @brief Lowercase String Transformation
   * @endif
   */
  void toLower(std::string& str)
  {
    std::transform(str.begin(), str.end(), str.begin(),
                   tolower_);
  }

  /*!
   * @if jp
   * @brief 入力ストリームから1行読み込む
   * @else
   * @brief Read a line from input stream
   * @endif
   */
  int getlinePortable(std::istream& istr, std::string& line)
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
    line = s.str();
    return static_cast<int>(line.size());
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
   * @brief 文字列をエスケープするためのFunctor
   * @else
   * @brief Functor to escape string
   * @endif
   */
  struct escape_functor
  {
    escape_functor() {}
    void operator()(const char c)
    {
      if      (c == '\t')  str += "\\t";
      else if (c == '\n')  str += "\\n";
      else if (c == '\f')  str += "\\f";
      else if (c == '\r')  str += "\\r";
      else if (c == '\\')  str += "\\\\";
      //    else if (c == '\"')  str += "\\\"";
      //    else if (c == '\'')  str += "\\\'";
      else
        {
          str.push_back(c);
        }
    }
    std::string str;
  };

  /*!
   * @if jp
   * @brief 文字列をエスケープする
   * @else
   * @brief Escape string
   * @endif
   */
  std::string escape(const std::string& str)
  {
    return for_each(str.begin(), str.end(), escape_functor()).str;
  }

  /*!
   * @if jp
   * @brief 文字列をアンエスケープするためのFunctor
   * @else
   * @brief Functor to unescape string
   * @endif
   */
  struct unescape_functor
  {
    unescape_functor() : count(0) {}
    void operator()(char c)
    {
      if (c == '\\')
        {
          ++count;
          if ((count % 2) == 0)
            {
              str.push_back(c);
            }
        }
      else
        {
          if (count > 0 && ((count % 2) != 0))
            {
              count = 0;
              if      (c == 't')  str.push_back('\t');
              else if (c == 'n')  str.push_back('\n');
              else if (c == 'f')  str.push_back('\f');
              else if (c == 'r')  str.push_back('\r');
              else if (c == '\"') str.push_back('\"');
              else if (c == '\'') str.push_back('\'');
              else
                {
                  str.push_back(c);
                }
            }
          else
            {
              count = 0;
              str.push_back(c);
            }
        }
    }
    std::string str;
    int count;
  };

  /*!
   * @if jp
   * @brief 文字列のエスケープを戻す
   * @else
   * @brief Unescape string
   * @endif
   */
  std::string unescape(const std::string& str)
  {
    return for_each(str.begin(), str.end(), unescape_functor()).str;
  }

  /*!
   * @if jp
   * @brief 文字列の空白文字を削除する
   * @else
   * @brief Erase blank characters of string
   * @endif
   */
  void eraseBlank(std::string& str)
  {
    std::string::iterator it(str.begin());

    while (it != str.end())
      {
        if (*it == ' ' || *it == '\t')
          {
            it = str.erase(it);
          }
        else
          {
            ++it;
          }
      }

  }

  /*!
   * @if jp
   * @brief 文字列の先頭の空白文字を削除する
   * @else
   * @brief Erase the head blank characters of string
   * @endif
   */
  void eraseHeadBlank(std::string& str)
  {
    if (str.empty()) return;
    while (str[0] == ' ' || str[0] == '\t') str.erase(0, 1);
  }

  /*!
   * @if jp
   * @brief 文字列の末尾の空白文字を削除する
   * @else
   * @brief Erase the tail blank characters of string
   * @endif
   */
  void eraseTailBlank(std::string& str)
  {
    if (str.empty()) return;
    while ((str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t') &&
           !isEscaped(str, str.size() - 1))
      str.erase(str.size() - 1, 1);
  }

  /*!
   * @if jp
   * @brief 文字列の先頭・末尾の空白文字を削除する
   * @else
   * @brief Erase the head blank and the tail blank characters of string
   * @endif
   */
  void eraseBothEndsBlank(std::string& str)
  {
    eraseHeadBlank(str);
    eraseTailBlank(str);
  }

  /*!
   * @if jp
   * @brief 文字列を正規化する
   * @else
   * @brief Erase the head/tail blank and replace upper case to lower case
   * @endif
   */
  std::string normalize(std::string& str)
  {
    eraseBothEndsBlank(str);
    toLower(str);
    return str;
  }

  /*!
   * @if jp
   * @brief 文字列を置き換える
   * @else
   * @brief Replace string
   * @endif
   */
  unsigned int replaceString(std::string& str, const std::string& from,
                     const std::string& to)
  {
    std::string::size_type pos(0);
    unsigned int cnt(0);

    while (pos != std::string::npos)
      {
        pos = str.find(from, pos);
        if (pos == std::string::npos) break;
        str.replace(pos, from.size(), to);
        pos += to.size();
        ++cnt;
      }
    return cnt;
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
    typedef std::string::size_type size;
    vstring results(0);
    size delim_size = delimiter.size();
    size found_pos(0), begin_pos(0), pre_pos(0), substr_size(0);

    if (input.empty()) { return results; }

    //  if (input.substr(0, delim_size) == delimiter)
    //    begin_pos = pre_pos = delim_size;

    while (true)
      {
        //    REFIND:
        found_pos = input.find(delimiter, begin_pos);
        if (found_pos == std::string::npos)
          {
            std::string substr(input.substr(pre_pos));
            eraseHeadBlank(substr);
            eraseTailBlank(substr);
            if (!(substr.empty() && ignore_empty))
              {
                results.push_back(substr);
              }
            break;
          }
        /*
          if (isEscaped(input, found_pos))
          {
          begin_pos = found_pos + delim_size;
          goto REFIND;
          }
        */
        substr_size = found_pos - pre_pos;
        if (substr_size > 0)
          {
            std::string substr(input.substr(pre_pos, substr_size));
            eraseHeadBlank(substr);
            eraseTailBlank(substr);
            if (!(substr.empty() && ignore_empty))
              {
                results.push_back(substr);
              }
          }
        begin_pos = found_pos + delim_size;
        pre_pos   = found_pos + delim_size;
      }
    return results;
  }

  /*!
   * @if jp
   * @brief 大文字に変換する Functor
   * @else
   * @brief Functor to convert to capital letters
   * @endif
   */
  struct Toupper
  {
    void operator()(char &c)
    {
      c = static_cast<char>(toupper(static_cast<int>(c)));
    }
  };

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
    std::for_each(str.begin(), str.end(), Toupper());
    std::for_each(yes.begin(), yes.end(), Toupper());
    std::for_each(no.begin(),  no.end(),  Toupper());

    if (str.find(yes) != std::string::npos)
      return true;
    else if (str.find(no) != std::string::npos)
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
    if (ignore_case) { toLower(value); }

    for (auto str : list)
      {
        if (ignore_case) { toLower(str); }
        if (str == value) return true;
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
    vstring vlist(split(list, ","));
    return includes(vlist, value, ignore_case);
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
    // UNIX absolute path is begun from '/'
    if (str[0] == '/') return true;
    // Windows absolute path is begun from '[a-zA-Z]:\'
    if ((isalpha(str[0]) != 0) && (str[1] == ':') && str[2] == '\\') return true;
    // Windows network file path is begun from '\\'
    if (str[0] == '\\' && str[1] == '\\') return true;

    return false;
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
    typedef std::string::size_type size;
    size pos;
    if (str.empty()) return false;
    pos = str.find(':');
    return (pos != 0) &&
        (pos != std::string::npos) &&
        (str[pos + 1] == '/') &&
        (str[pos + 2] == '/');
  }
  bool isIPv4(const std::string& str)
  {
    for (char c : str)
      {
        if (!((c >= '0' && c <= '9') ||
              c == '.' || c == ':'))
          { return false; }
      }
    coil::vstring tmp = coil::split(str, ":");
    coil::vstring ipv4 = coil::split(str, ".");
    if (ipv4.size() != 4) { return false; }
    for (auto & c : ipv4)
      {
        unsigned short int dec;
        if (!coil::stringTo(dec, c.c_str())) { return false; }
        if (dec > 255) { return false; }
      }
    return true;
  }
  bool isIPv6(const std::string& str)
  {
    // IPv6 address must be
    // 1111:1111:1111:1111:1111:1111:1111:1111 (addr)
    // [1111:1111:1111:1111:1111:1111:1111:1111]:11111 (addr, port)
    for (char c : str)
      {
        if (!((c >= '0' && c <= '9') ||
             (c >= 'a' && c <= 'f') ||
             (c >= 'A' && c <= 'F') ||
              c == ':' || c == '[' || c == ']'))
          { return false; }
      }
    coil::vstring tmp = coil::split(str, "]:");
    if (tmp.size() > 2) { return false; }
    if (tmp.size() == 2)
      {
        if (tmp[0][0] != '[') { return false; }
        tmp[0].erase(0, 1);
      }
    
    coil::vstring ipv6 = coil::split(tmp[0], ":");
    if (ipv6.size() > 8) { return false; }
    for (auto & v : ipv6)
      {
        try
          {
            if (v.empty()) { continue; }
            char* endptr = nullptr;
            long int hexval = std::strtol(v.c_str(), &endptr, 16);
            if (errno == ERANGE) { return false; }
            if (hexval < 0x0 || hexval > 0xFFFF) { return false; }
          }
        catch (...)
          {
            return false;
          }
      }
    return true;
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
    for (auto & param : params)
      {
        std::string::size_type pos = param.find('=');
        if (pos != std::string::npos)
          {
            retmap[param.substr(0, pos)] = param.substr(pos + 1);
          }
        else
          {
            retmap[param] = std::string("");
          }
      }
    return retmap;
  }

  /*!
   * @if jp
   * @brief リスト内の文字列を検索する Functor
   * @else
   * @brief Functor to find string in a list
   * @endif
   */
  struct unique_strvec
  {
    void operator()(const std::string& s)
    {
      if (std::find(str.begin(), str.end(), s) == str.end())
        return str.push_back(s);
    }
    vstring str;
  };

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
    std::string boolstr(str);
    coil::normalize(boolstr);
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
   * @brief 与えられた文字列リストから重複を削除
   * @else
   * @brief Eliminate duplication from the given string list
   * @endif
   */
  vstring unique_sv(vstring sv)
  {
    return std::for_each(sv.begin(), sv.end(), unique_strvec()).str;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列リストからCSVを生成
   * @else
   * @brief Create CSV file from the given string list
   * @endif
   */
  std::string flatten(vstring sv, std::string delimiter)
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
  char** toArgv(const vstring& args)
  {
    char** argv;
    size_t argc(args.size());

    argv = new char*[argc + 1];

    for (size_t i(0); i < argc; ++i)
      {
        size_t sz(args[i].size());
        argv[i] = new char[sz + 1];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        strncpy_s(argv[i], sz + 1, args[i].c_str(), sz);
#else
        strncpy(argv[i], args[i].c_str(), sz);
#endif
        argv[i][sz] = '\0';
      }
    argv[argc] = nullptr;
    return argv;
  }

  /*!
   * @if jp
   * @brief 指定された書式に変換
   * @else
   * @brief Convert it into a format given with an argumen
   * @endif
   */
  std::string sprintf(char const * __restrict fmt, ...)
  {
#ifndef LINE_MAX
#define LINE_MAX 1024
#endif
    char str[LINE_MAX];
    va_list ap;

    va_start(ap, fmt);
#ifdef WIN32
    _vsnprintf_s(str, LINE_MAX - 1, _TRUNCATE, fmt, ap);
#else
    vsnprintf(str, LINE_MAX - 1, fmt, ap);
#endif
    va_end(ap);
    return std::string(str);
  }


  /*!
  * @if jp
  * @brief 文字列中の環境変数を置き換える
  *
  *
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
      vstring str_list = split(str, "${");
      if (str_list.size() < 2)
      {
          return str;
      }
      vstring ret;
      for (auto & s : str_list)
      {
          vstring tmp = split(s, "}");
          if (tmp.size() == 2)
          {
              char c[100];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
              strcpy_s(c, sizeof(c), coil::getenv(tmp[0].c_str()));
#else
              strcpy(c, coil::getenv(tmp[0].c_str()));
#endif
              ret.push_back(std::string(c));
              ret.push_back(tmp[1]);
          }
          else
          {
              ret.push_back(s);
          }
      }

      std::string ret_str;
      for (auto & s : ret)
      {
          ret_str.append(s);
      }
      return ret_str;
  }

} // namespace coil
