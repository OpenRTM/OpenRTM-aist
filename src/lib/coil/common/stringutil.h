// -*- C++ -*-
/*!
 * @file StringUtil.h
 * @brief String operation utility
 * @date $Date: 2007-12-31 03:08:07 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2003-2008
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: StringUtil.h 826 2008-08-26 08:13:39Z n-ando $
 *
 */

#ifndef COIL_STRINGUTIL_H
#define COIL_STRINGUTIL_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>
#include <cerrno>
#include <cstdlib>

#if defined (_MSC_VER) && (_MSC_VER <=1500)  // VC2008(VC9.0) or before
#elif defined(VXWORKS_66) && !defined(__RTP__)
#else
#include <cstdint>
#endif
// Cygwin's gcc does not provide wstring type
#if defined(Cygwin) && ( __GNUC__ < 4 )
namespace std
{
  typedef basic_string<wchar_t> wstring;
}  // namespace std
#endif

namespace coil
{
  typedef std::vector<std::string> vstring;
  typedef std::map<std::string, std::string> mapstring;
  /*!
   * @if jp
   * @brief string から wstring への変換
   *
   * 与えられた string 文字列を wstring 文字列に変換
   *
   * @param str std::string 型の入力文字列
   * @return std::wstring 型の出力文字列
   *
   * @else
   * @brief string to wstring conversion
   *
   * This function convert from a string to a wstring.
   *
   * @param str The input std::string type string
   * @return Converted std::wstring type string
   *
   * @endif
   */
  std::wstring string2wstring(std::string str);

  /*!
   * @if jp
   * @brief wstring から string への変換
   *
   * 与えられた wstring 文字列を string 文字列に変換
   *
   * @param str std::wstring 型の入力文字列
   * @return std::string 型の出力文字列
   *
   * @else
   * @brief wstring to string conversion
   *
   * This function convert from a wstring to a string.
   *
   * @param str The input std::wstring type string
   * @return Converted std::string type string
   *
   * @endif
   */
  std::string wstring2string(std::wstring wstr);

  /*!
   * @if jp
   * @brief 大文字への変換
   *
   * 与えられた文字列を大文字に変換
   *
   * @param str 入力文字列
   *
   * @else
   * @brief Uppercase String Transformation
   *
   * This function transforms a given string to uppercase letters
   *
   * @param str The input string
   *
   * @endif
   */
  void toUpper(std::string& str);

  /*!
   * @if jp
   * @brief 小文字への変換
   *
   * 与えられた文字列を小文字に変換
   *
   * @param str 入力文字列
   *
   * @else
   * @brief Lowercase String Transformation
   *
   * This function transforms a given string to lowercase letters
   *
   * @param str The input string
   *
   * @endif
   */
  void toLower(std::string& str);

  /*!
   * @if jp
   * @brief 入力ストリームから1行読み込む
   *
   * 入力ストリームから1行読み込む。
   * 入力ストリームはの改行コードはUNIX, Windowsの改行コードのいずれか、
   * もしくは混在していてもよい。
   *
   * @param istr 入力ストリーム
   * @param line 読み込んだ文字列を格納する変数
   *
   * @return 改行文字を除く読み込んだ文字列の長さ
   *
   * @else
   * @brief Read a line from input stream
   *
   * This function reads a line from input stream.
   * UNIX, Windows or mixed line feed code is acceptable.
   *
   * @param istr The input stream.
   * @param line The output variable to store string to be read.
   *
   * @return The length of read string except line feed character.
   *
   * @endif
   */
  int getlinePortable(std::istream& istr, std::string& line);

  /*!
   * @if jp
   * @brief 文字列がエスケープされているか判断する
   *
   * 指定された文字がエスケープされているかどうかを判断する。
   *
   * @param str エスケープされているかどうか判断する文字を含む文字列
   * @param pos エスケープされているかどうか判断する文字の位置
   *
   * @return 指定した文字がエスケープされていれば true, それ以外は false
   *
   * @else
   * @brief Check whether the character is escaped or not
   *
   * Check whether the specified character is escaped or not
   *
   * @param str The string that includes the character to be investigated.
   * @param pos The position of the character to be investigated.
   *
   * @return True if the specified character is escaped, else False.
   *
   * @endif
   */
  bool isEscaped(const std::string& str, std::string::size_type pos);

  /*!
   * @if jp
   * @brief 文字列をエスケープする
   *
   * 次の文字をエスケープシーケンスに変換する。<br>
   * HT -> "\t" <br>
   * LF -> "\n" <br>
   * CR -> "\r" <br>
   * FF -> "\f" <br>
   * シングルクオート、ダブルクオートについてはとくに処理はしない。
   *
   * @param str エスケープ処理対象文字列
   *
   * @return エスケープ処理結果文字列
   *
   * @else
   *
   * @brief Escape string
   *
   * The following characters are converted. <br>
   * HT -> "\t" <br>
   * LF -> "\n" <br>
   * CR -> "\r" <br>
   * FF -> "\f" <br>
   * Single quote and double quote are not processed.
   *
   * @param str The target string for the escape
   *
   * @return Result string of the escape
   *
   * @endif
   */
  std::string escape(const std::string& str);

  /*!
   * @if jp
   * @brief 文字列のエスケープを戻す
   *
   * 次のエスケープシーケンスを文字に変換する。<br>
   * "\t" -> HT <br>
   * "\n" -> LF <br>
   * "\r" -> CR <br>
   * "\f" -> FF <br>
   * "\"" -> "  <br>
   * "\'" -> '  <br>
   * ※エスケープ処理の完全な逆変換にはなっていないため、注意が必要。
   *
   * @param str アンエスケープ処理対象文字列
   *
   * @return アンエスケープ処理結果文字列
   *
   * @else
   *
   * @brief Unescape string
   *
   * The following characters are converted. <br>
   * "\t" -> HT <br>
   * "\n" -> LF <br>
   * "\r" -> CR <br>
   * "\f" -> FF <br>
   * "\"" -> "  <br>
   * "\'" -> '  <br>
   * Note: This is not complete inversion of the escape processing.
   *
   * @param str The target string for the unescape
   *
   * @return Result string of the unescape
   *
   * @endif
   */
  std::string unescape(const std::string& str);

  /*!
   * @if jp
   * @brief 文字列の空白文字を削除する
   *
   * 与えられた文字列の空白文字を削除する。
   * 空白文字として扱うのは' '(スペース)と'\\t'(タブ)。
   *
   * @param str 空白文字削除処理文字列
   *
   * @else
   * @brief Erase blank characters of string
   *
   * Erase blank characters that exist at the head of the given string.
   * Space ' 'and tab '\\t' are supported as the blank character.
   *
   * @param str The target blank characters of string for the erase
   *
   * @endif
   */
  void eraseBlank(std::string& str);

  /*!
   * @if jp
   * @brief 文字列の先頭の空白文字を削除する
   *
   * 与えられた文字列の先頭に存在する空白文字を削除する。
   * 空白文字として扱うのは' '(スペース)と'\\t'(タブ)。
   *
   * @param str 先頭空白文字削除処理文字列
   *
   * @else
   * @brief Erase the head blank characters of string
   *
   * Erase the blank characters that exist at the head of the given string.
   * Space ' 'and tab '\\t' are supported as the blank character.
   *
   * @param str The target head blank characters of string for the erase
   *
   * @endif
   */
  void eraseHeadBlank(std::string& str);

  /*!
   * @if jp
   * @brief 文字列の末尾の空白文字を削除する
   *
   * 与えられた文字列の末尾に存在する空白文字を削除する。
   * 空白文字として扱うのは' '(スペース)と'\\t'(タブ)。
   *
   * @param str 末尾空白文字削除処理文字列
   *
   * @else
   * @brief Erase the tail blank characters of string
   *
   * Erase the blank characters that exist at the tail of the given
   * string.  Space ' 'and tab '\\t' are supported as the blank
   * character.
   *
   * @param str The target tail blank characters of string for the erase
   *
   * @endif
   */
  void eraseTailBlank(std::string& str);

  /*!
   * @if jp
   * @brief 文字列の先頭・末尾の空白文字を削除する
   *
   * 与えられた文字列の先頭および末尾に存在する空白文字を削除する。
   * 空白文字として扱うのは' '(スペース)と'\\t'(タブ)。
   *
   * @param str 先頭末尾空白文字削除処理文字列
   *
   * @else
   * @brief Erase the head blank and the tail blank characters of string
   *
   * Erase the head blank characters and the blank characters that
   * exist at the tail of the given string.  Space ' 'and tab '\\t'
   * are supported as the blank character.
   *
   * @param str The target tail blank characters of string for the erase
   *
   * @endif
   */
  void eraseBothEndsBlank(std::string& str);

  /*!
   * @if jp
   * @brief 文字列を正規化する
   *
   * 与えられた文字列の先頭および末尾に存在する空白文字を削除し、
   * 英字をすべて小文字に変換する。
   *
   * @param str 処理対象文字列
   *
   * @else
   * @brief Erase the head/tail blank and replace upper case to lower case
   *
   * Erase the head blank characters and the blank characters that
   * exist at the tail of the given string.  Space ' 'and tab '\\t'
   * are supported as the blank character.
   * And all upper case cahracters are converted into lower case.
   *
   * @param str The target string for the erase
   *
   * @endif
   */
  std::string normalize(std::string& str);

  /*!
   * @if jp
   * @brief 文字列を置き換える
   *
   * 与えられた文字列に対して、指定した文字の置き換えを行う。
   *
   * @param str 置き換え処理対象文字列
   * @param from 置換元文字
   * @param to 置換先文字
   *
   * @else
   * @brief Replace string
   *
   * Replace the given string with the specified characters.
   *
   * @param str The target characters of string for replacement processing
   * @param from Characters of replacement source
   * @param to Characters of replacement destination
   *
   * @endif
   */
  unsigned int replaceString(std::string& str, const std::string& from,
                             const std::string& to);

  /*!
   * @if jp
   * @brief 文字列を分割文字で分割する
   *
   * 設定された文字列を与えられたデリミタで分割する。
   *
   * @param input 分割対象文字列
   * @param delimiter 分割文字列(デリミタ)
   *
   * @return 文字列分割結果リスト
   *
   * @else
   * @brief Split string by delimiter
   *
   * Split the set string by the given delimiter
   *
   * @param input The target characters of string for split
   * @param delimiter Split string (delimiter)
   *
   * @return Split string result list
   *
   * @endif
   */
  vstring split(const std::string& input,
                const std::string& delimiter,
                bool ignore_empty = false);

  /*!
   * @if jp
   * @brief 与えられた文字列をbool値に変換する
   *
   * 指定された文字列を、true表現文字列、false表現文字列と比較し、その結果を
   * bool値として返す。
   * 比較の結果、true表現文字列、false表現文字列のどちらとも一致しない場合は、
   * 与えられたデフォルト値を返す。
   *
   * @param str 判断対象文字列
   * @param yes true表現文字列
   * @param no false表現文字列
   * @param default_value デフォルト値(デフォルト値:true)
   * @else
   * @brief Convert given string into bool value
   *
   * Compare the specified string with the true representation string and
   * the false representation string, and return the result as bool value.
   * If it matches neither the true representation string nor the false
   * representation string as a result of the comparison, the given default
   * value will be return.
   *
   * @param str The target string for investigation
   * @param yes The true representation string
   * @param no The false representation string
   * @param default_value The default value (The default value:true)
   * @endif
   */
  bool toBool(std::string str, std::string yes, std::string no,
              bool default_value = true);
  /*!
   * @if jp
   * @brief 文字列リスト中にある文字列が含まれるかどうかを判断する
   *
   * 第1引数にカンマ区切りのリストを、第2引数に探索対象文字列を指定し、
   * その文字列が第1引数の中に含まれるかを判断する。
   *
   * @param list 対象リスト
   * @param value 探索文字列
   * @return true: 含まれる、false: 含まれない
   *
   * @else
   * @brief Include if a string is included in string list
   *
   * if the second argument is included in the comma separated string
   * list of the first argument, This operation returns "true value".
   *
   * @param list The target comma separated string
   * @param value The searched string
   * @return true: included, false: not included
   *
   * @endif
   */
  bool includes(const vstring& list, std::string value,
                bool ignore_case = true);

  /*!
   * @if jp
   * @brief 文字列リスト中にある文字列が含まれるかどうかを判断する
   *
   * 第1引数にカンマ区切りのリストを、第2引数に探索対象文字列を指定し、
   * その文字列が第1引数の中に含まれるかを判断する。
   *
   * @param list 対象文字列
   * @param value 探索文字列
   * @return true: 含まれる、false: 含まれない
   *
   * @else
   * @brief Include if a string is included in string list
   *
   * if the second argument is included in the comma separated string
   * list of the first argument, This operation returns "true value".
   *
   * @param list The target comma separated string
   * @param value The searched string
   * @return true: included, false: not included
   *
   * @endif
   */
  bool includes(const std::string& list, std::string value,
                bool ignore_case = true);

  /*!
   * @if jp
   * @brief 与えられた文字列が絶対パスかどうかを判断する
   *
   * 与えられた文字列が絶対パス表現であるかどうかを判断する。
   * 文字列が以下の場合には絶対パスとして判断する。
   *  - 先頭文字が'/' (UNIXの場合)
   *  - 先頭３文字がアルファベット＋'/'＋'\\' (Windowsの場合)
   *  - 先頭２文字が'\\\\' (Windowsネットワークパスの場合)
   *
   * @param str 判定対象文字列
   *
   * @return 絶対パス判定結果
   *
   * @else
   * @brief Investigate whether the given string is absolute path or not
   *
   * Investigate whether the given string is absolute path or not.
   * Investigate it as an absolute path, if the string is as follows:
   *  - The first character '/' (UNIX)
   *  - The first 3 characters are alphabet ＋'/'＋'\\' (Windows)
   *  - The first 2 characters are '\\\\' (Windows network path)
   *
   * @param str The target string for the investigation
   *
   * @return Investigation result of absolute path
   *
   * @endif
   */
  bool isAbsolutePath(const std::string& str);

  /*!
   * @if jp
   * @brief 与えられた文字列がURLかどうかを判断する
   *
   * 与えられた文字列がURL表現かどうかを判断する。
   * 与えられた文字列中に、'://'という文字列が含まれている場合には
   * URL表現として判断する。
   *
   * @param str 判定対象文字列
   *
   * @return URL判定結果
   *
   * @else
   * @brief Investigate whether the given string is URL or not
   *
   * Investigate whether the given string is URL or not.
   * When the string '://' is included in the given character string,
   * make it of URL representation.
   *
   * @param str The target string for investigation
   *
   * @return URL investigation result
   *
   * @endif
   */
  bool isURL(const std::string& str);
  bool isIPv4(const std::string& str);
  bool isIPv6(const std::string& str);

  /*!
   * @if jp
   * @brief 与えられた文字列がURLかどうかを判断する
   *
   * 与えられた文字列がURL表現かどうかを判断する。
   * 与えられた文字列中に、'://'という文字列が含まれている場合には
   * URL表現として判断する。
   *
   * @param str 判定対象文字列
   *
   * @return URL判定結果
   *
   * @else
   * @brief Investigate whether the given string is URL or not
   *
   * Investigate whether the given string is URL or not.
   * When the string '://' is included in the given character string,
   * make it of URL representation.
   *
   * @param str The target string for investigation
   *
   * @return URL investigation result
   *
   * @endif
   */
  coil::mapstring urlparam2map(const std::string& str);

  /*!
   * @if jp
   * @brief 与えられたオブジェクトをstd::stringに変換
   *
   * 引数で指定されたオブジェクトを文字列に変換する。
   *
   * @param n 変換対象オブジェクト
   *
   * @return 文字列変換結果
   *
   * @else
   * @brief Convert the given object to std::string
   *
   * Convert the object specified by the argument to string.
   *
   * @param n The target object for conversion
   *
   * @return String conversion result
   *
   * @endif
   */
  template <class Printable>
  std::string otos(Printable n)
  {
    std::stringstream str_stream;
    str_stream << n;
    return str_stream.str();
  };

  /*!
   * @if jp
   * @brief 与えられたstd::stringをオブジェクトに変換
   *
   * 引数で与えられた文字列を指定されたオブジェクトに変換する。
   *
   * @param val 変換先オブジェクト
   * @param str 変換元文字列
   *
   * @return true: 成功, false: 失敗
   *
   * @else
   * @brief Convert the given std::string to object.
   *
   * Convert string given by the argument to specified object.
   *
   * @param val The object of conversion destination
   * @param str String of conversion source
   *
   * @return true: successful, false: failed
   *
   * @endif
   */
  template <typename To>
  bool stringTo(To& val, const char* str)
  {
    if (str == nullptr) { return false; }

    std::stringstream s;
    if ((s << str).fail()) { return false; }
    if ((s >> val).fail()) { return false; }
    return true;
  }

  /*!
   * @if jp
   * @brief 与えられた文字列をstd::stringに変換
   *
   * 引数で与えられた文字列をstd::stringに変換する。
   *
   * @param val 変換先文字列
   * @param str 変換元文字列
   *
   * @return true: 成功, false: 失敗
   *
   * @else
   * @brief Convert the given string to std::string.
   *
   * Convert string given by the argument to std::string.
   *
   * @param val String of conversion destination
   * @param str String of conversion source
   *
   * @return true: successful, false: failed
   *
   * @endif
   */
  template<>
  bool stringTo<std::string>(std::string& val, const char* str);

  /*!
   * @if jp
   * @brief 与えられた文字列リストから重複を削除
   *
   * 引数で与えられた文字列リストから重複を削除したリストを作成する。
   *
   * @param sv 確認元文字列リスト
   *
   * @return 重複削除処理結果リスト
   *
   * @else
   * @brief Eliminate duplication from the given string list
   *
   * Create a list of eliminating duplication from the string list
   * given by the argument.
   *
   * @param sv The string list for confirmation source
   *
   * @return Eliminating duplication result list
   *
   * @endif
   */
  template <>
  bool stringTo<bool>(bool& val, const char* str);

  /*!
   * @if jp
   * @brief ポインタを16進数文字列に変換する
   *
   * 引数で与えられた文字列を16進数文字列に変換する。変換された文字列の
   * 先頭には "0x" が付加される。
   *
   * @param ptr ポインタ
   *
   * @return 16進数文字列
   *
   * @else
   * @brief Converting a pointer to hexadecimal string
   *
   * This function converts given string to hexadecimal string. "0x"
   * will be added the head of the converted string.
   *
   * @param ptr A pointer to be converted
   *
   * @return Hexadecimal string
   *
   * @endif
   */
  template<class T>
  std::string ptrToHex(T* n)
  {
#if defined(VXWORKS_66) && !defined(__RTP__)
    return "";
#else
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << reinterpret_cast<uintptr_t>(n);
    return ss.str();
#endif
  };

  /*!
   * @if jp
   * @brief 16進数文字列をポインタに変換する
   *
   * 引数で与えられた16進数文字列を文字列をに変換する。文字列の
   * 先頭には "0x" が付加されているべきである。
   *
   * @param ptr ポインタ
   * @param str 16進数文字列
   * @return 変換が成功したら true、それ以外は false
   *
   * @else
   * @brief Converting hexadecimal string to a pointer
   *
   * This function converts given hexadecimal string to a
   * pointer. Hexadecimal string should have "0x" string in the head
   * of string.
   *
   * @param ptr Pointer
   * @param str Hexadeciaml string
   * @return True will be returned when conversion successful
   *
   * @endif
   */
  template <class T>
  bool hexToPtr(T*& ptr, const std::string& str)
  {
#if defined(VXWORKS_66) && !defined(__RTP__)
    return false;
#else
    std::stringstream s;
    if ((s << std::hex << str).fail()) { return false; }
    uintptr_t intval = 0;
    if ((s >> intval).fail()) { return false; }
    ptr = reinterpret_cast<T*>(intval);
    if (ptr == NULL) { return false; }
    return true;
#endif
  }

  /*!
   * @if jp
   * @brief 与えられた文字列リストから重複を削除
   *
   * 引数で与えられた文字列リストから重複を削除したリストを作成する。
   *
   * @param sv 確認元文字列リスト
   *
   * @return 重複削除処理結果リスト
   *
   * @else
   * @brief Eliminate duplication from the given string list
   *
   * Create a list of eliminating duplication from the string list
   * given by the argument.
   *
   * @param sv The string list for confirmation source
   *
   * @return Eliminating duplication result list
   *
   * @endif
   */
  vstring unique_sv(vstring sv);

  /*!
   * @if jp
   * @brief 与えられた文字列リストからCSVを生成
   *
   * 引数で与えられた文字列リストの各要素を並べたCSVを生成する。
   * 文字列リストが空の場合には空白文字を返す。
   *
   * @param sv CSV変換対象文字列リスト
   *
   * @return CSV変換結果文字列
   *
   * @else
   * @brief Create CSV file from the given string list
   *
   * Create CSV that arranged each element of the character string
   * list given by the argument.
   * If the string list is empty, the null will be returned.
   *
   * @param sv The target string list for creating CSV
   *
   * @return String of CSV creating result
   *
   * @endif
   */
  std::string flatten(vstring sv, const std::string& delimiter = ", ");

  /*!
   * @if jp
   * @brief 与えられた文字列リストを引数リストに変換
   *
   * 引数で与えられた文字列リストの各要素末尾に'\\0'を加え、引数リスト
   * に変換する。
   *
   * @param args 変換対象文字列リスト
   *
   * @return 引数変換結果文字列
   *
   * @else
   * @brief Convert the given string list into the argument list
   *
   * Convert the string list into the argument list by adding '\\0' to
   * each element at the end of the string list given by the argument
   *
   * @param args The target string list for conversion
   *
   * @return String of argument conversion result
   *
   * @endif
   */
  char** toArgv(const vstring& args);


  /*!
   * @if jp
   * @brief 指定された書式に変換
   *
   * 引数で与えられた書式に変換して返す。
   *
   * @param fmt 書式
   *
   * @return 変換結果文字列
   *
   * @else
   * @brief Convert it into a format given with an argumen
   *
   * Convert it into a format given with an argument and return it.
   *
   * @param fmt Format
   *
   * @return String of conversion result
   *
   * @endif
   */
  std::string sprintf(char const * __restrict fmt, ...);

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
  std::string replaceEnv(std::string str);
  
}  // namespace coil
#endif  // COIL_STRINGUTIL_H
