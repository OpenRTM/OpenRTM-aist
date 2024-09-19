// -*- C++ -*-
/*!
 * @file NVUtil.cpp
 * @brief NameValue and NVList utility functions
 * @date $Date: 2008-01-13 07:41:08 $
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
 * $Id$
 *
 */

#include <coil/stringutil.h>
#include <rtm/NVUtil.h>
#include <rtm/CORBA_SeqUtil.h>

#include <algorithm>
#include <map>
#include <vector>

namespace NVUtil
{
  /*!
   * @if jp
   * @brief value が CORBA::Char の NameValue を生成する
   * @else
   * @brief Create NameValue typed CORBA::Char
   * @endif
   */
  SDOPackage::NameValue newNVChar(const char* name, const CORBA::Char value)
  {
    SDOPackage::NameValue nv;
    nv.name = CORBA::string_dup(name);
    nv.value <<= CORBA::Any::from_char(value);
    return nv;
  }

  /*!
   * @if jp
   * @brief value が CORBA::Boolean の NameValue を生成する
   * @else
   * @brief This operation creates NameValue typed CORBA::Boolean.
   * @endif
   */
  SDOPackage::NameValue newNVBool(const char* name, const CORBA::Boolean value)
  {
    SDOPackage::NameValue nv;
    nv.name = CORBA::string_dup(name);
    nv.value <<= CORBA::Any::from_boolean(value);
    return nv;
  }

  /*!
   * @if jp
   * @brief value が CORBA::Octet の NameValue を生成する
   * @else
   * @brief Create NameValue typed CORBA::Octet
   * @endif
   */
  SDOPackage::NameValue newNVOctet(const char* name, const CORBA::Octet value)
  {
    SDOPackage::NameValue nv;
    nv.name = CORBA::string_dup(name);
    nv.value <<= CORBA::Any::from_octet(value);
    return nv;
  }

  /*!
   * @if jp
   * @brief value が CORBA::Any の NameValue を生成する
   * @else
   * @brief Create NameValue typed CORBA::Any
   * @endif
   */
  SDOPackage::NameValue newNVAny(const char* name, const CORBA::Any& value)
  {
    SDOPackage::NameValue nv;
    nv.name = CORBA::string_dup(name);
    nv.value = value;
    return nv;
  }

  /*!
   * @if jp
   * @brief Properties を NVList へコピーする
   * @else
   * @brief Copy the properties to NVList
   * @endif
   */
#ifndef ORB_IS_RTORB
  void copyFromProperties(SDOPackage::NVList& nv, const coil::Properties& prop)
#else  // ORB_IS_RTORB
    void copyFromProperties(SDOPackage_NVList& nv, const coil::Properties& prop)
#endif  // ORB_IS_RTORB
  {
    std::vector<std::string> keys;
    keys = prop.propertyNames();
    CORBA::ULong len(static_cast<CORBA::ULong>(keys.size()));
    nv.length(len);

    for (CORBA::ULong i = 0; i < len; ++i)
      {
// Why RtORB does not copy string to Properties.
        nv[i].name = CORBA::string_dup(keys[i].c_str());
        nv[i].value <<= prop[keys[i]].c_str();
      }
  }


  /*!
   * @if jp
   * @brief Properties を NVList へマージする
   * @else
   * @brief Merge the properties to NVList
   * @endif
   */
#ifndef ORB_IS_RTORB
  void mergeFromProperties(SDOPackage::NVList& nv, const coil::Properties& prop)
#else  // ORB_IS_RTORB
  void mergeFromProperties(SDOPackage_NVList& nv, const coil::Properties& prop)
#endif  // ORB_IS_RTORB
  {
    std::vector<std::string> keys;
    keys = prop.propertyNames();
#ifndef ORB_IS_RTORB
    SDOPackage::NVList nve;
#else  // ORB_IS_RTORB
    SDOPackage_NVList nve;
#endif  // ORB_IS_RTORB
    nve.length(static_cast<CORBA::ULong>(keys.size()));

    CORBA::ULong nsize(0);
    for (CORBA::ULong i = 0; i < nv.length(); ++i)
      {
        if (prop.findNode(static_cast<const char*>(nv[i].name)) == nullptr)
          {
            nve[nsize] = nv[i];
            nsize++;
          }
      }

    CORBA::ULong ksize(static_cast<CORBA::ULong>(keys.size()));
    CORBA::ULong len(ksize + nsize);
    nv.length(len);

    for (CORBA::ULong i = 0; i < ksize; ++i)
      {
        nv[i].name = CORBA::string_dup(keys[i].c_str());
        nv[i].value <<= prop[keys[i]].c_str();
      }

    for (CORBA::ULong i = 0; i < nsize; ++i)
      {
        nv[i+ ksize] = nve[i];
      }

  }

  /*!
   * @if jp
   * @brief NVList を Properties へコピーする
   * @else
   * @brief Copy NVList to the Proeprties
   * @endif
   */
  void copyToProperties(coil::Properties& prop, const SDOPackage::NVList& nv)
  {
    for (CORBA::ULong i(0), len(nv.length()); i < len; ++i)
      {
        const char* value(nullptr);
        if (nv[i].value >>= value)
          {
            const char* name(nv[i].name);
            prop[name] = value;
          }
      }
  }

  /*!
   * @if jp
   * @brief NVList を Properties に変換するためのファンクタ
   * @else
   * @brief Functor to transform NVList into the properties
   * @endif
   */
  struct to_prop
  {
    to_prop()
      : m_prop(coil::Properties())
    {
    }
    void operator()(const SDOPackage::NameValue& nv)
    {
      const char* value(nullptr);
      if (nv.value >>= value)
        {
          m_prop.setProperty(CORBA::string_dup(nv.name), value);
        }
    }
    coil::Properties m_prop;
  };

  /*!
   * @if jp
   * @brief NVList を Properties へ変換する
   * @else
   * @brief Transform NVList to the properties
   * @endif
   */
  coil::Properties toProperties(const SDOPackage::NVList& nv)
  {
    to_prop p;
    p = CORBA_SeqUtil::for_each(nv, p);
    return p.m_prop;
  }

  /*!
   * @if jp
   * @brief NVList を検索するためのファンクタ
   * @else
   * @brief Functor to find a NVList
   * @endif
   */
  struct nv_find
  {
    explicit nv_find(const char* name) : m_name(name) {}
    bool operator()(const SDOPackage::NameValue& nv)
    {
      std::string name(nv.name);
      return m_name == name;
    }
    std::string m_name;
  };

  /*!
   * @if jp
   * @brief NVList から name で指定された value を返す
   * @else
   * @brief Return the value specified by name from NVList
   * @endif
   */
  const CORBA::Any& find(const SDOPackage::NVList& nv, const char* name)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(nv, NVUtil::nv_find(name));
    if (index < 0) throw std::string("Not found");
    return nv[index].value;
  }

  /*!
   * @if jp
   * @brief name で指定された要素のインデックスを返す
   * @else
   * @brief Return the index of element specified by name from NVList
   * @endif
   */
  CORBA::Long find_index(const SDOPackage::NVList& nv, const char* name)
  {
    return  CORBA_SeqUtil::find(nv, NVUtil::nv_find(name));
  }

  /*!
   * @if jp
   * @brief 指定された name の value の型が string であるか検証する
   * @else
   * @brief Validate whether value type specified by name is string type
   * @endif
   */
  bool isString(const SDOPackage::NVList& nv, const char* name)
  {
    try
      {
        CORBA::Any value;
        value = find(nv, name);
        const char* str_value(nullptr);
        return value >>= str_value;
      }
    catch (...)
      {
        return false;
      }
  }

  /*!
   * @if jp
   * @brief 指定された name の value の値が指定した文字列と一致するか検証する
   * @else
   * @brief Check whether the value of specified name specified matches
   *        the specified string
   * @endif
   */
  bool isStringValue(const SDOPackage::NVList& nv,
                     const char* name, const char* value)
  {
    if (isString(nv, name))
      {
        if (toString(nv, name) == value)
          {
            return true;
          }
      }
    return false;
  }

  /*!
   * @if jp
   * @brief 指定された name の NVList を string として返す。
   * @else
   * @brief Get NVList of specifid name as string
   * @endif
   */
  std::string toString(const SDOPackage::NVList& nv, const char* name)
  {
    const char* str_value(nullptr);
    try
      {
        if (!(find(nv, name) >>= str_value))
          {
            str_value = "";
          }
      }
    catch (...)
      {
        str_value = "";
      }

    if (str_value == nullptr)
      {
        str_value = "";
      }

    return str_value;
  }

  /*!
   * @if jp
   * @brief 指定された文字列を NVList の要素に追加する。
   * @else
   * @brief Append the specified string to element of NVList
   * @endif
   */
#ifndef ORB_IS_RTORB
  bool appendStringValue(SDOPackage::NVList& nv, const char* name,
                         const char* value)
#else  // ORB_IS_RTORB
  bool appendStringValue(SDOPackage_NVList& nv, const char* name,
                         const char* value)
#endif  // ORB_IS_RTORB
  {

    CORBA::Long index;
    index = find_index(nv, name);

    if (index < 0)
      {
        CORBA_SeqUtil::push_back(nv, newNV(name, value));
      }
    else
      {
        const char* tmp_char(nullptr);
        nv[index].value >>= tmp_char;
        std::string tmp_str(tmp_char);

        std::vector<std::string> values;
        values = coil::split(tmp_str, ",");
        if (values.end() == std::find(values.begin(), values.end(), value))
          {
            tmp_str.append(",");
            tmp_str.append(value);
            nv[index].value <<= tmp_str.c_str();
          }
      }
    return true;
  }

  /*!
   * @if jp
   * @brief NVList に要素を追加する。
   * @else
   * @brief Append an element to NVList
   * @endif
   */
  void append(SDOPackage::NVList& dest, const SDOPackage::NVList& src)
  {
    for (CORBA::ULong i = 0, len = src.length(); i < len; ++i)
      {
        CORBA_SeqUtil::push_back(dest, src[i]);
      }
  }

  /*!
   * @if jp
   * @brief NameValue に設定されている内容を文字列として出力する。
   * @else
   * @brief Print information configured in NVList as a string type
   * @endif
   */
  std::ostream& dump_to_stream(std::ostream& out,
                               const SDOPackage::NameValue& nv)
  {
    const char* str_value;
    if (nv.value >>= str_value)
      {
        out << nv.name << ": " << str_value << std::endl;
      }
    else
      {
        out << nv.name << ": not a string value" << std::endl;
      }
    return out;
  }
  std::ostream& operator<<(std::ostream& os, const SDOPackage::NameValue& nv)
  {
    return dump_to_stream(os, nv);
  }

  /*!
   * @if jp
   * @brief NVList に設定されている内容を文字列として出力する。
   * @else
   * @brief Print information configured in NVList as a string type
   * @endif
   */
  std::ostream& dump_to_stream(std::ostream& out,
                               const SDOPackage::NVList& nv)
  {
    for (CORBA::ULong i(0), n(nv.length()); i < n; ++i)
      {
        out << nv[i];
      }
    return out;
  }
  std::ostream& operator<<(std::ostream& os, const SDOPackage::NVList& nvlist)
  {
    return dump_to_stream(os, nvlist);
  }

  /*!
   * @if jp
   * @brief NVList に設定されている内容を文字列として標準出力する。
   * @else
   * @brief Print information configured in NVList as a string type
   *        to Standard Outport.
   * @endif
   */
  void dump(const SDOPackage::NVList& nv)
  {
    dump_to_stream(std::cout, nv);
  }

  /*!
   * @if jp
   * @brief NVList に設定されている内容を文字列にする
   * @else
   * @brief Get information configured in NVList as a string type
   * @endif
   */
  std::string toString(const SDOPackage::NVList& nv)
  {
    std::stringstream s;
    dump_to_stream(s, nv);
    return s.str();
  }
} // namespace NVUtil
