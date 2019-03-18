// -*- C++ -*-
/*!
 * @file NVUtil.h
 * @brief NameValue and NVList utility functions
 * @date $Date: 2007-12-31 03:08:04 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef NVUTIL_NVUTIL_H
#define NVUTIL_NVUTIL_H

#include <coil/Properties.h>
#include <rtm/idl/SDOPackageSkel.h>

#include <string>
#include <iostream>

/*!
 * @if jp
 * @namespace NVUtil
 *
 * @brief NameValue 用ユーティリティ
 *
 * NameValue に対してのユーティリティ関数を提供する。
 *
 * @else
 *
 * @namespace NVUtil
 *
 * @brief Utility for NameValue
 *
 * This class provides the utility function of NameValue.
 *
 * @endif
 */
namespace NVUtil
{
  /*!
   * @if jp
   *
   * @brief NameValue を生成する
   *
   * このオペレーションはNameValueを作成する。
   * CORBA::Char, CORBA::Boolean, CORBA::Octet は作成できない。
   * これらの値は newNVChar(), newNVBool(), newNVOctet() で作成する。
   *
   * @param name NameValue の name
   * @param value NameValue の value
   *
   * @return NameValue
   *
   * @else
   *
   * @brief Create NameValue
   *
   * This operation creates NameValue.
   * CORBA::Char, CORBA::Boolean, CORBA::Octet creation is not supported.
   * These type of NameValue should be created by using
   * newNVChar(), newNVBool(), newNVOctet() functions.
   *
   * @param name Name of NameValue
   * @param value The value of NameValue
   *
   * @return NameValue
   *
   * @endif
   */
  template <class Value>
  SDOPackage::NameValue newNV(const char* name, Value value)
  {
    SDOPackage::NameValue nv;
    nv.name = CORBA::string_dup(name);
    nv.value <<= value;
    return nv;
  }

  /***
   * @if jp
   *
   * @brief value が CORBA::string の NameValue を生成する
   *
   * このオペレーションはf value が CORBA::string の NameValueを作成する。
   *
   * @param name NameValue の name
   * @param value NameValue の value
   *
   * @return NameValue
   *
   * @else
   *
   * @brief Create NameValue typed CORBA::string
   *
   * This operation creates NameValue typed CORBA::string.
   *
   * @param name Name of NameValue
   * @param value The value of NameValue
   *
   * @return NameValue
   *
   * @endif
   */
  /*
    template <>
    SDOPackage::NameValue newNV(const char* name, const char* value)
    {
    SDOPackage::NameValue nv;
    nv.name = CORBA::string_dup(name);
    nv.value <<= value;
    return nv;
    }
  */

  /*!
   * @if jp
   *
   * @brief value が CORBA::Char の NameValue を生成する
   *
   * このオペレーションはf value が CORBA::Char の NameValueを作成する。
   *
   * @param name NameValue の name
   * @param value NameValue の value
   *
   * @return NameValue
   *
   * @else
   *
   * @brief Create NameValue typed CORBA::Char
   *
   * This operation creates NameValue typed CORBA::Char.
   *
   * @param name Name of NameValue
   * @param value The value of NameValue
   *
   * @return NameValue
   *
   * @endif
   */
  SDOPackage::NameValue newNVChar(const char* name, CORBA::Char value);

  /*!
   * @if jp
   *
   * @brief value が CORBA::Boolean の NameValue を生成する
   *
   * このオペレーションはf value が CORBA::Boolean の NameValueを作成する。
   *
   * @param name NameValue の name
   * @param value NameValue の value
   *
   * @return NameValue
   *
   * @else
   *
   * @brief Create NameValue typed CORBA::Boolean
   *
   * This operation creates NameValue typed CORBA::Boolean.
   *
   * @param name Name of NameValue
   * @param value The value of NameValue
   *
   * @return NameValue
   *
   * @endif
   */
  SDOPackage::NameValue newNVBool(const char* name,
                                  CORBA::Boolean value);

  /*!
   * @if jp
   *
   * @brief value が CORBA::Octet の NameValue を生成する
   *
   * このオペレーションは value が CORBA::Octet の NameValueを作成する。
   *
   * @param name NameValue の name
   * @param value NameValue の value
   *
   * @return NameValue
   *
   * @else
   *
   * @brief Create NameValue typed CORBA::Octet
   *
   * This operation creates NameValue typed CORBA::Octet.
   *
   * @param name Name of NameValue
   * @param value The value of NameValue
   *
   * @return NameValue
   *
   * @endif
   */
  SDOPackage::NameValue newNVOctet(const char* name, CORBA::Octet value);

  /*!
   * @if jp
   *
   * @brief value が CORBA::Any の NameValue を生成する
   *
   * このオペレーションはf value が CORBA::Any の NameValueを作成する。
   *
   * @param name NameValue の name
   * @param value NameValue の value
   *
   * @return NameValue
   *
   * @else
   *
   * @brief Create NameValue typed CORBA::Any
   *
   * This operation creates NameValue typed CORBA::Any.
   *
   * @param name Name of NameValue
   * @param value The value of NameValue
   *
   * @return NameValue
   *
   * @endif
   */
  SDOPackage::NameValue newNVAny(const char* name, const CORBA::Any& value);

  /*!
   * @if jp
   *
   * @brief Properties を NVList へコピーする
   *
   * このオペレーションは Properties を NVList へコピーする。
   * NVList の value は全て CORBA::string 型としてコピーする。
   *
   * @param nv Properties の値を格納する NVList
   * @param prop コピー元の Properties
   *
   * @else
   *
   * @brief Copy the properties to NVList
   *
   * This operation copies the properties to NVList.
   * All NVList's values are copied as CORBA::string.
   *
   * @param nv NVList to store properties values
   * @param prop Properties that is copies from
   *
   * @endif
   */
#ifndef ORB_IS_RTORB
  void copyFromProperties(SDOPackage::NVList& nv, const coil::Properties& prop);
#else  // ORB_IS_RTORB
  void copyFromProperties(SDOPackage_NVList& nv, const coil::Properties& prop);
#endif  // ORB_IS_RTORB

  /*!
   * @if jp
   *
   * @brief NVList を Properties へコピーする
   *
   * このオペレーションは NVList を Properties へコピーする。
   *
   * @param prop NVList の値を格納する Properties
   * @param nv コピー元の NVList
   *
   * @else
   *
   * @brief Copy NVList to the Proeprties
   *
   * This operation copies NVList to properties.
   *
   * @param prop Properties to store NVList values
   * @param nv NVList of copy source
   *
   * @endif
   */
  void copyToProperties(coil::Properties& prop, const SDOPackage::NVList& nv);

  /*!
   * @if jp
   *
   * @brief NVList を Properties へ変換する
   *
   * このオペレーションは NVList を Properties へ変換する。
   *
   * @param nv 変換元の NVList
   *
   * @return 変換結果Property
   *
   * @else
   *
   * @brief Transform NVList to the properties
   *
   * This operation transforms NVList to properties
   *
   * @param nv NVList of tranformation source
   *
   * @return Transformation result Property
   *
   * @endif
   */
  coil::Properties toProperties(const SDOPackage::NVList& nv);

  /*!
   * @if jp
   *
   * @brief NVList から name で指定された value を返す
   *
   * このオペレーションは name で指定された value を Any 型で返す。
   * 指定した名称の要素が存在しない場合は例外を発生させる。
   *
   * @param nv 検索対象の NVList
   * @param name 検索する名前
   *
   * @return 検索結果
   *
   * @else
   *
   * @brief Return the value specified by name from NVList
   *
   * This operation returns Any type of value specified by name.
   * When an element of specified name doesn't exist, the exception will occur.
   *
   * @param nv The target NVList for the find
   * @param name  Name for the find
   *
   * @return Find result
   *
   * @endif
   */
  const CORBA::Any& find(const SDOPackage::NVList& nv, const char* name);

  /*!
   * @if jp
   *
   * @brief name で指定された要素のインデックスを返す
   *
   * このオペレーションは name で指定された要素が格納されている位置の
   * インデックスを返す。
   *
   * @param nv 検索対象の NVList
   * @param name 検索する名前
   *
   * @return 検索対象のインデックス
   *
   * @else
   *
   * @brief Return the index of element specified by name from NVList
   *
   * This operation returns the index at the position where the element
   * specified by name is stored.
   *
   * @param nv The target NVList for the find
   * @param name  Name for the find
   *
   * @return Index of target object for the find
   *
   * @endif
   */
  CORBA::Long find_index(const SDOPackage::NVList& nv, const char* name);

  /*!
   * @if jp
   *
   * @brief 指定された name の value の型が string であるか検証する
   *
   * このオペレーションは name で指定された value の型が CORBA::string
   * かどうかを bool 値で返す。
   *
   * @param nv 検索対象の NVList
   * @param name 検索する名前
   *
   * @return string検証結果(string:true、それ以外:false)
   *
   * @else
   *
   * @brief Validate whether value type specified by name is string type
   *
   * This operation returns the bool value by checking whether the type of
   * value specified with name is CORBA::string.
   *
   * @param nv The target NVList for the search
   * @param name Name for the search
   *
   * @return String validation result (String:true, Else:false)
   *
   * @endif
   */
  bool isString(const SDOPackage::NVList& nv, const char* name);

  /*!
   * @if jp
   *
   * @brief 指定された name の value の値が指定した文字列と一致するか検証する
   *
   * このオペレーションは name で指定された value の型が CORBA::string
   * かどうかを判断し、  CORBA::string である場合には指定した文字列と一致するか
   * をbool 値で返す。
   *
   * @param nv 検索対象の NVList
   * @param name 検索する名前
   * @param value 比較対象文字列
   *
   * @return 検証結果(文字列と一致:true、非一致:false)
   *
   * @else
   *
   * @brief Check whether the value of specified name matches the specified
   *        string
   *
   * This operation checks whether the value specified with name is
   * CORBA::string and returns the bool value which matches spcified string.
   *
   * @param nv The target NVList for the search
   * @param name Name for the search
   * @param value String value to compare
   *
   * @return Check result (Match:true, Unmatch:false)
   *
   * @endif
   */
  bool isStringValue(const SDOPackage::NVList& nv, const char* name,
                     const char* value);

  /*!
   * @if jp
   *
   * @brief 指定された name の NVList を string として返す。
   *
   * このオペレーションは name で指定された NVList の値を string で返す。
   * もし、name で指定した value の値が CORBA::string でなければ、
   * 空の文字列のstringを返す。
   *
   * @param nv 検索対象の NVList
   * @param name 検索する名前
   *
   * @return name に対応する値のstring型の値
   *
   * @else
   *
   * @brief Get NVList of specifid name as string
   *
   * This operation returns string value in NVList specified by name.
   * If the value in NVList specified by name is not CORBA::string type
   * this operation returns empty string value.
   *
   * @param nv The target NVList for the search
   * @param name Name for the search
   *
   * @return String value corresponding to name
   *
   * @endif
   */
  std::string toString(const SDOPackage::NVList& nv, const char* name);

  /*!
   * @if jp
   *
   * @brief 指定された文字列を NVList の要素に追加する。
   *
   * このオペレーションは name で指定された要素に value で指定された文字列を
   * 追加する。
   * name で指定した要素に既に value の値が設定されている場合には何もしない。
   * name で指定した要素に value の値が設定されていない場合は、 ","区切りで
   * value の値を追加する。
   * 指定された値を設定する。
   * name で指定した要素が存在しない場合は、 NVList の最後に新たな要素を追加し、
   * 指定された値を設定する。
   *
   * @param nv 検索対象の NVList
   * @param name 追加対象要素名
   * @param value 追加する文字列
   *
   * @return 追加操作結果
   *
   * @else
   *
   * @brief Append the specified string to element of NVList
   *
   * This operation appends the string value specified by value to the element
   * specified by name.
   * Operate nothing when the 'value' value has already been set to the
   * element specified by name.
   * Add the 'value' value each separating by a comma "," when the 'value'
   * value is not set to the element specified by name.
   * Set the specified value.
   * Add a new element at the end of NVList, and set the specified value,
   * when the element specified by name does not exist.
   *
   * @param nv The target NVList for the search
   * @param name The target element name for the appending
   * @param value String to append
   *
   * @return Append operation result
   *
   * @endif
   */
#ifndef ORB_IS_RTORB
  bool appendStringValue(SDOPackage::NVList& nv, const char* name,
                         const char* value);
#else  // ORB_IS_RTORB
  bool appendStringValue(SDOPackage_NVList& nv, const char* name,
                         const char* value);
#endif  // ORB_IS_RTORB

  /*!
   * @if jp
   *
   * @brief NVList に要素を追加する。
   *
   * このオペレーションは dest で指定された NVList に src で指定された要素を
   * 追加する。
   *
   * @param dest 追加される NVList
   * @param src 追加する NVList
   *
   * @else
   *
   * @brief Append an element to NVList
   *
   * This operation appends elements specified by src to NVList specified
   * by dest.
   *
   * @param dest NVList to be appended
   * @param src NVList to append
   *
   * @endif
   */
  void append(SDOPackage::NVList& dest, const SDOPackage::NVList& src);

  /*!
   * @if jp
   *
   * @brief NameValue に設定されている内容を文字列として出力する。
   *
   * 指定された NVList に設定された内容を文字列として出力する。なお、設
   * 定されている要素が文字列型以外の場合には、その旨(文字列ではない)を
   * 出力する。
   *
   * @param nv 出力対象 NameValue
   *
   * @else
   *
   * @brief Print information configured in NameValue as string type
   *
   * Print configured information as string type in specified NVList.
   * Also, print the reason (this is not string type) if the configured
   * element is other than string type.
   *
   * @param nv The target NameValue for the print
   *
   * @endif
   */
  std::ostream& dump_to_stream(std::ostream& out,
                               const SDOPackage::NameValue& nv);
  std::ostream& operator<<(std::ostream& os, const SDOPackage::NameValue& nv);

/*!
   * @if jp
   *
   * @brief NVList に設定されている内容を文字列として出力する。
   *
   * 指定された NVList に設定された内容を文字列として出力する。なお、設
   * 定されている要素が文字列型以外の場合には、その旨(文字列ではない)を
   * 出力する。
   *
   * @param nv 出力対象 NVList
   *
   * @else
   *
   * @brief Print information configured in NVList as string type
   *
   * Print configured information as string type in specified NVList.
   * Also, print the reason (this is not string type) if the configured
   * element is other than string type.
   *
   * @param nv The target NVList for the print
   *
   * @endif
   */
  std::ostream& operator<<(std::ostream& os, const SDOPackage::NVList& nvlist);

  /*!
   * @if jp
   *
   * @brief NVList に設定されている内容を文字列として出力する。
   *
   * 指定された NVList に設定された内容を文字列として出力する。
   * なお、設定されている要素が文字列型以外の場合には、その旨(文字列ではない)を
   * 出力する。
   *
   * @param nv 出力対象 NVList
   *
   * @else
   *
   * @brief Print information configured in NVList as string type
   *
   * Print configured information as string type in specified NVList.
   * Also, print the reason (this is not string type) if the configured
   * element is other than string type.
   *
   * @param nv The target NVList for the print
   *
   * @endif
   */
  std::ostream& dump(std::ostream& out, const SDOPackage::NVList& nv);

  /*!
   * @if jp
   * @brief NVList に設定されている内容を文字列として標準出力する。
   *
   * @param nv 出力対象 NVList
   *
   * @else
   * @brief Print information configured in NVList as a string type
   *        to Standard Outport.
   *
   * @param nv The target NVList for the print
   *
   * @endif
   */
  void dump(const SDOPackage::NVList& nv);

  /*!
   * @if jp
   * @brief NVList に設定されている内容を文字列にする
   *
   * @param nv 出力対象 NVList
   *
   * @else
   * @brief Get information configured in NVList as a string type.
   *
   * @param nv The target NVList for the print
   *
   * @endif
   */
  std::string toString(const SDOPackage::NVList& nv);


} // namespace NVUtil
#endif  // NVUTIL_NVUTIL_H
