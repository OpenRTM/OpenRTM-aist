// -*- C++ -*-
/*!
 * @file Properties.cpp
 * @brief Property list class (derived from Java Properties)
 * @date $Date: 2007-12-31 03:08:06 $
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

#include <coil/Properties.h>
#include <coil/stringutil.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifdef __QNX__
using std::size_t;
#endif

namespace coil
{
  /*!
   * @if jp
   * @brief コンストラクタ(rootノードのみ作成)
   * @else
   * @brief Constructor(Create only root node)
   * @endif
   */
  Properties::Properties(const char* key, const char* value)
    : name(key), value(value), default_value(""), root(nullptr), m_empty("")
  {
    leaf.clear();
  }

  /*!
   * @if jp
   * @brief コンストラクタ(mapでデフォルト値を与える)
   * @else
   * @brief Constructor(Give the default value with map)
   * @endif
   */
  Properties::Properties(std::map<std::string, std::string>& defaults)
    : name(""), value(""), default_value(""), root(nullptr), m_empty("")
  {
    leaf.clear();
    std::map<std::string, std::string>::iterator it(defaults.begin());
    std::map<std::string, std::string>::iterator it_end(defaults.end());

    while (it != it_end)
      {
        setDefault(it->first, it->second);
        ++it;
      }
  }

  /*!
   * @if jp
   * @brief コンストラクタ(char*[] でデフォルト値を与える)
   * @else
   * @brief Constructor(Give the default value with char*[])
   * @endif
   */
  Properties::Properties(const char** defaults, long num)
    : name(""), value(""), default_value(""), root(nullptr), m_empty("")
  {
    leaf.clear();
    setDefaults(defaults, num);
  }

  /*!
   * @if jp
   * @brief コピーコンストラクタ
   * @else
   * @brief Copy Constructor
   * @endif
   */
  Properties::Properties(const Properties& prop)
    : name(prop.name), value(prop.value),
      default_value(prop.default_value), root(nullptr), m_empty("")
  {
    std::vector<std::string> keys;
    keys = prop.propertyNames();
    for (const auto & key : keys)
      {
        const Properties* node(nullptr);
        if ((node = prop.findNode(key)) != nullptr)
          {
            setDefault(key,  node->default_value);
            setProperty(key, node->value);
          }
      }
  }

  /*!
   * @if jp
   * @brief 代入演算子
   * @else
   * @brief Assignment operator
   * @endif
   */
  Properties& Properties::operator=(const Properties& prop)
  {
    clear();
    name = prop.name;
    value = prop.value;
    default_value = prop.default_value;

    std::vector<std::string> keys;
    keys = prop.propertyNames();
    for (const auto & key : keys)
      {
        const Properties* node(prop.findNode(key));
        if (node != nullptr)
          {
            setDefault(key,  node->default_value);
            setProperty(key, node->value);
          }
      }

    return *this;
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  Properties::~Properties()
  {
    // Delete children
    clear();

    // delete myself from parent
    if (root != nullptr)
      {
        root->removeNode(name.c_str());
      }
  };

  //============================================================
  // public fnctions
  //============================================================
  /*!
   * @if jp
   * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
   * @else
   * @brief Search for the property with the specified key in the property list.
   * @endif
   */
  const std::string& Properties::getProperty(const std::string& key) const
  {
    std::vector<std::string> keys;
    split(key, '.', keys);
    Properties* node(nullptr);
    if ((node = _getNode(keys, 0, this)) != nullptr)
      {
        return (!node->value.empty()) ? node->value : node->default_value;
      }
    return m_empty;
  }

  /*!
   * @if jp
   * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
   * @else
   * @brief Search for the property with the specified key in the property list
   * @endif
   */
  const std::string& Properties::getProperty(const std::string& key,
                                             const std::string& def) const
  {
    const std::string& invalue((*this)[key]);

    return invalue.empty() ? def : invalue;
  }

  /*!
   * @if jp
   * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
   * @else
   * @brief Search for the property with the specified key in the property list
   * @endif
   */
  const std::string& Properties::operator[](const std::string& key) const
  {
    return getProperty(key);
  }

  /*!
   * @if jp
   * @brief 指定されたキーを持つプロパティを、プロパティリストから探す
   * @else
   * @brief Search for the property with the specified key in the property list
   * @endif
   */
  std::string& Properties::operator[](const std::string& key)
  {
    setProperty(key, getProperty(key));
    Properties& prop(getNode(key));

    return prop.value;
  }

  /*!
   * @if jp
   * @brief 指定されたキーに対してデフォルト値を取得する
   * @else
   * @brief Get the default value with specified key
   * @endif
   */
  const std::string& Properties::getDefault(const std::string& key) const
  {
    std::vector<std::string> keys;
    split(key, '.', keys);
    Properties* node(nullptr);
    if ((node = _getNode(keys, 0, this)) != nullptr)
      {
        return node->default_value;
      }
    return m_empty;
  }

  /*!
   * @if jp
   * @brief Properties に value を key について登録する
   * @else
   * @brief Set the value associated with key in the property list
   * @endif
   */
  std::string Properties::setProperty(const std::string& key,
                                      const std::string& invalue)
  {
    std::vector<std::string> keys;
    split(key, '.', keys);

    Properties* curr(this);
    for (auto & k : keys)
      {
        Properties* next(curr->hasKey(k.c_str()));
        if (next == nullptr)
          {
            next = new Properties(k.c_str());
            next->root = curr;
            curr->leaf.push_back(next);
          }
        curr = next;
      }
    std::string retval(curr->value);
    curr->value = invalue;
    return retval;
  }

  /*!
   * @if jp
   * @brief デフォルト値を登録する
   * @else
   * @brief Set the default value associated with key in the property list
   * @endif
   */
  std::string Properties::setDefault(const std::string& key,
                                     const std::string& invalue)
  {
    std::vector<std::string> keys;
    split(key, '.', keys);

    Properties* curr(this);
    for (auto & k : keys)
      {
        Properties* next(curr->hasKey(k.c_str()));
        if (next == nullptr)
          {
            next = new Properties(k.c_str());
            next->root = curr;
            curr->leaf.push_back(next);
          }
        curr = next;
      }
    curr->default_value = invalue;
    return invalue;
  }

  /*!
   * @if jp
   * @brief Properties にデフォルト値をまとめて登録する
   * @else
   * @brief Set a default value together in the property list
   * @endif
   */
  void Properties::setDefaults(const char* defaults[], long num)
  {
    for (long i = 0; i < num && defaults[i][0] != '\0' ; i += 2)
      {
        std::string key(defaults[i]);
        std::string invalue(defaults[i + 1]);

        coil::eraseHeadBlank(key);
        coil::eraseTailBlank(key);

        coil::eraseHeadBlank(invalue);
        coil::eraseTailBlank(invalue);

        setDefault(key, invalue);
      }
  }

  //============================================================
  // load and save functions
  //============================================================
  /*!
   * @if jp
   * @brief 指定された出力ストリームに、プロパティリストを出力する
   * @else
   * @brief Print this property list out to the specified output stream
   * @endif
   */
  void Properties::list(std::ostream& out)
  {
    _store(out, "", this);
  }

  /*!
   * @if jp
   * @brief 入力ストリームからキーと要素が対になったプロパティリストを読み込む
   * @else
   * @brief Load property list that consists of key:value from input stream
   * @endif
   */
  void Properties::load(std::istream& inStream)
  {
    std::string pline = std::string();

    while (!inStream.eof())
      {
        std::string tmp = std::string();
        coil::getlinePortable(inStream, tmp);
        coil::eraseHeadBlank(tmp);

        // Skip comments or empty lines
        if (tmp.empty())
          {
            continue;
          }
        if (tmp[0] == '#' || tmp[0] == '!')
          {
            continue;
          }

        // line-end '\' continues entry
        if (tmp[tmp.size() - 1] == '\\'
        && !coil::isEscaped(tmp, tmp.size() - 1))
          {
            tmp.erase(tmp.size() - 1);
            pline += tmp;
            continue;
          }
        pline += tmp;

        // Skip empty line (made of only ' ' or '\t')
        if (pline.empty())
          {
            continue;
          }

        std::string key, invalue;
        splitKeyValue(pline, key, invalue);
        key = coil::unescape(key);
        coil::eraseHeadBlank(key);
        coil::eraseTailBlank(key);

        invalue = coil::unescape(invalue);
        coil::eraseHeadBlank(invalue);
        coil::eraseTailBlank(invalue);

        setProperty(key, invalue);
        pline.clear();
      }
  }

  /*!
   * @if jp
   * @brief プロパティリストを指定されたストリームに保存する
   * @else
   * @brief Save the property list to the specified stream
   * @endif
   */
  void Properties::save(std::ostream& out, const std::string& header)
  {
    store(out, header);
    return;
  }

  /*!
   * @if jp
   * @brief プロパティリストを出力ストリームへ保存する
   * @else
   * @brief Store the property list to the output stream
   * @endif
   */
  void Properties::store(std::ostream& out, const std::string& header)
  {
    out << "# " << header << std::endl;
    _store(out, "", this);
  }

  //============================================================
  // other util functions
  //============================================================
  /*!
   * @if jp
   * @brief プロパティのキーのリストを vector で返す
   * @else
   * @brief Return an vector of all the keys in this property
   * @endif
   */
  std::vector<std::string> Properties::propertyNames() const
  {
    std::vector<std::string> names;
    for (auto prop : leaf)
      {
        _propertiyNames(names, prop->name, prop);
      }
    return names;
  }

  /*!
   * @if jp
   * @brief プロパティの数を取得する
   * @else
   * @brief Get the number of properties
   * @endif
   */
  int Properties::size() const
  {
    return static_cast<int>(propertyNames().size());
  }

  /*!
   * @if jp
   * @brief ノードを検索する
   * @else
   * @brief Find node of properties
   * @endif
   */
  Properties* Properties::findNode(const std::string& key) const
  {
    if (key.empty())
      {
        return nullptr;
      }
    std::vector<std::string> keys;
    //    std::string value;
    split(key, '.', keys);
    return _getNode(keys, 0, this);
  }

  /*!
   * @if jp
   * @brief ノードを取得する
   * @else
   * @brief Get node of properties
   * @endif
   */
  Properties& Properties::getNode(const std::string& key)
  {
    if (key.empty())
      {
        return *this;
      }
    Properties* const leafptr(findNode(key));
    if (leafptr != nullptr)
      {
        return *leafptr;
      }
    this->createNode(key);
    return *findNode(key);
  }

  /*!
   * @if jp
   * @brief 新規ノードを生成する
   * @else
   * @brief Create newly node of Properties
   * @endif
   */
  bool Properties::createNode(const std::string& key)
  {
    if (key.empty())
      {
        return false;
      }

    if (findNode(key) != nullptr)
      {
        return false;
      }
    (*this)[key] = "";
    return true;
  }

  /*!
   * @if jp
   * @brief ノードを削除する
   * @else
   * @brief Remove node of properties
   * @endif
   */
  Properties* Properties::removeNode(const char* leaf_name)
  {
    std::vector<Properties*>::iterator it(leaf.begin()), it_end(leaf.end());
    Properties* prop;
    while (it != it_end)
      {
        if ((*it)->name == leaf_name)
          {
            prop = *it;
            leaf.erase(it);
            return prop;
          }
        ++it;
      }
    return nullptr;
  }

  /*!
   * @if jp
   * @brief 子ノードにkeyがあるかどうか
   * @else
   * @brief Check whether key exists in the children
   * @endif
   */
  Properties* Properties::hasKey(const char* key) const
  {
    for (auto prop : leaf)
      {
        if (prop->name == key)
          {
            return prop;
          }
      }
    return nullptr;
  }

  /*!
   * @if jp
   * @brief 子ノードを全て削除する
   * @else
   * @brief Clear the children
   * @endif
   */
  void Properties::clear()
  {
    while (!leaf.empty())
      {
        if (leaf.back() != nullptr)  // recursive delete
          delete leaf.back();     // back() returns always new
      }
  }

  /*!
   * @if jp
   * @brief Propertyをマージする
   * @else
   * @brief Merge properties
   * @endif
   */
  Properties& Properties::operator<<(const Properties& prop)
  {
    std::vector<std::string> keys;
    keys = prop.propertyNames();
    for (size_t i(0), len(static_cast<size_t>(prop.size())); i < len; ++i)
      {
        (*this)[keys[i]] = prop[keys[i]];
      }
    return (*this);
  }

  //------------------------------------------------------------
  // Protected functions
  //------------------------------------------------------------
  /*!
   * @if jp
   * @brief 文字列をキーと値のペアに分割する
   * @else
   * @brief Split the string into a pair of the key and the value.
   * @endif
   */
  void Properties::splitKeyValue(const std::string& str, std::string& key,
                                 std::string& value)
  {
    std::string::size_type i(0);
    std::string::size_type len(str.size());

    while (i < len)
      {
        if ((str[i] == ':' || str[i] == '=') && !coil::isEscaped(str, i))
          {
            key   = str.substr(0, i);  // substr(0, i) returns 0...(i-1) chars.
            coil::eraseHeadBlank(key);
            coil::eraseTailBlank(key);
            value = str.substr(i + 1);
            coil::eraseHeadBlank(value);
            coil::eraseTailBlank(value);
            return;
          }
        ++i;
      }

    // If no ':' or '=' exist, ' ' would be delimiter.
    i = 0;
    while (i < len)
      {
        if ((str[i] == ' ') && !coil::isEscaped(str, i))
          {
            key   = str.substr(0, i);  // substr(0, i) returns 0...(i-1) chars.
            coil::eraseHeadBlank(key);
            coil::eraseTailBlank(key);
            value = str.substr(i + 1);
            coil::eraseHeadBlank(value);
            coil::eraseTailBlank(value);
            return;
          }
        ++i;
      }

    key = str;
    value = "";
    return;
  }

  /*!
   * @if jp
   * @brief 文字列を分割する
   * @else
   * @brief Split the string
   * @endif
   */
  bool Properties::split(const std::string& str, const char delim,
                         std::vector<std::string>& value)
  {
    if (str.empty()) return false;

    std::string::size_type begin_it(0), end_it(0);
    std::string::size_type len(str.size());

    while (end_it < len)
      {
        if ((str[end_it] == delim) && !coil::isEscaped(str, end_it))
          {
            // substr(0, i) returns 0...(i-1) chars.
            value.push_back(str.substr(begin_it, end_it - begin_it));
            begin_it = end_it + 1;
          }
        ++end_it;
      }
    value.push_back(str.substr(begin_it, end_it));
    return true;
  }

  /*!
   * @if jp
   * @brief プロパティを取得する
   * @else
   * @brief Get properties
   * @endif
   */
  Properties*
  Properties::_getNode(std::vector<std::string>& keys,
                       std::vector<Properties*>::size_type index,
                       const Properties* curr)
  {
    Properties* next(curr->hasKey(keys[index].c_str()));

    if (next == nullptr)
      {
        return nullptr;
      }

    if (index < keys.size() - 1)  // node
      {
        return next->_getNode(keys, ++index, next);
      }
    else
      {
        return next;
      }
    return nullptr;
  }

  /*!
   * @if jp
   * @brief プロパティの名称リストを取得する
   * @else
   * @brief Get a property name list
   * @endif
   */
  void
  Properties::_propertiyNames(std::vector<std::string>& names,
                              std::string curr_name,
                              const Properties* curr)
  {
    if (!curr->leaf.empty())
      {
        for (auto prop : curr->leaf)
          {
            std::string next_name;
            //            if (curr_name == "") next_name = curr->leaf[i]->name;
            next_name = curr_name + "." + prop->name;
            _propertiyNames(names, next_name, prop);
          }
      }
    else
      {
        names.push_back(curr_name);
      }
    return;
  }

  /*!
   * @if jp
   * @brief プロパティの名称リストを保存する
   * @else
   * @brief Store the property name list
   * @endif
   */
  void
  Properties::_store(std::ostream& out, std::string curr_name,
                     Properties* curr)
  {
    if (!curr->leaf.empty())
      {

        for (auto & prop : curr->leaf)
          {
            std::string next_name;
            if (curr_name.empty())
              {
                next_name = prop->name;
              }
            else
              {
                next_name = curr_name + "." + prop->name;
              }
            _store(out, next_name, prop);
          }
      }

    if (curr->root != nullptr)
      {
        if (curr->value.length() > 0)
          {
            out << curr_name << ": " << coil::escape(curr->value) << std::endl;
          }
      }
  }

  /*!
   * @if jp
   * @brief プロパティの内容を保存する
   * @else
   * @brief Save property's contents
   * @endif
   */
  std::ostream&
  Properties::_dump(std::ostream& out, const Properties& curr, int index)
  {
    if (index != 0) out << indent(index) << "- " << curr.name;
    if (curr.leaf.empty())
      {
        if (curr.value.empty())
          {
            out << ": " << curr.default_value << std::endl;
          }
        else
          {
            out << ": " << curr.value << std::endl;
          }
        return out;
      }
    if (index != 0) out << std::endl;
    for (auto prop : curr.leaf)
      {
        _dump(out, *prop, index + 1);
      }
    return out;
  }

  /*!
   * @if jp
   * @brief インデントを生成する
   * @else
   * @brief Create indents
   * @endif
   */
  std::string Properties::indent(int index)
  {
    std::string space = std::string();
    for (int i(0); i < index - 1; ++i)
      {
        space += "  ";
      }
    return space;
  }

  /*!
   * @if jp
   * @brief インデントを生成する
   * @else
   * @brief Create indents
   * @endif
   */
  std::string indent(int index)
  {
    std::string space;
    for (int i(0); i < index - 1; ++i)
      {
        space += "  ";
      }
    return space;
  }

  /*!
   * @if jp
   * @brief Propertyをストリームに出力する
   * @else
   * @brief Output Properties to stream
   * @endif
   */
  std::ostream& operator<<(std::ostream& lhs, const Properties& rhs)
  {
    return rhs._dump(lhs, rhs, 0);
  }


  Properties::operator std::vector<std::string>() const
  {
      std::vector<std::string> out;

      this->_dump(out, *this, 0);

      return out;
  }

  void Properties::_dump(std::string& out, const Properties& curr, int index) const
  {
      if (index != 0) out += indent(index) + "- " + curr.name;
      if (curr.leaf.empty())
      {
          if (curr.value.empty())
          {
              out += ": " + curr.default_value + "\n";
          }
          else
          {
              out += ": " + curr.value + "\n";
          }
          return;
      }
      if (index != 0) out += "\n";
      for (auto prop : curr.leaf)
      {
          _dump(out, *prop, index + 1);
      }
  }

  void Properties::_dump(std::vector<std::string>& out, const Properties& curr, int index) const
  {
      if (index != 0) out.push_back(indent(index) + "- " + curr.name);
      if (curr.leaf.empty())
      {
          if (curr.value.empty())
          {
              out.push_back(": " + curr.default_value);
          }
          else
          {
              out.push_back(": " + curr.value);
          }
          return;
      }
      if (index != 0) { out.emplace_back(""); }
      for (auto prop : curr.leaf)
      {
          _dump(out, *prop, index + 1);
      }
  }
} // namespace coil
