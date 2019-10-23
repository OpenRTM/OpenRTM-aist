// -*- C++ -*-
/*!
 * @file Factory.h
 * @brief generic Factory template class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2009, 2012
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

#ifndef COIL_FACTORY_H
#define COIL_FACTORY_H

#include <coil/Singleton.h>

#include <cassert>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <mutex>

// for Windows DLL export
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   ifdef LIBRARY_EXPORTS
#      define EXTERN
#      define DLL_PLUGIN __declspec(dllexport)
#   else
#      define EXTERN extern
#      define DLL_PLUGIN __declspec(dllimport)
#   endif
#else
#   define DLL_PLUGIN
#ifndef EXTERN
#   define EXTERN
#endif // ifndef EXTERN
#endif /* Windows */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace coil
{
  /*!
   * @if jp
   *
   * @brief reutrn codes of Factory class.
   *
   * @else
   *
   * @brief reutrn codes of Factory class.
   *
   * @endif
   */
  enum class FactoryReturn : uint8_t
    {
      OK,
      FACTORY_ERROR,
      ALREADY_EXISTS,
      NOT_FOUND,
      INVALID_ARG,
      UNKNOWN_ERROR
    };

  /*!
   * @if jp
   *
   * @brief Creator テンプレート
   *
   * @else
   *
   * @brief Creator template
   *
   * @endif
   */
  template <class AbstractClass, class ConcreteClass>
  AbstractClass* Creator()
  {
      return new ConcreteClass();
  }

  /*!
   * @if jp
   *
   * @class Factory
   * @brief Factory テンプレートクラス
   *
   * @else
   *
   * @class Factory
   * @brief Factory template class
   *
   * @endif
   */
  template <
    class AbstractClass,
    typename Identifier = std::string,
    typename Compare = std::less<Identifier>,
    typename Creator = AbstractClass* (*)()
    >
  class Factory
  {
    class FactoryEntry;
  public:
    using FactoryMap = std::map<Identifier, Creator>;
    using FactoryMapIt = typename FactoryMap::iterator;

    /*!
     * @if jp
     *
     * @brief ファクトリー有無チェック
     *
     * 指定IDのファクトリー有無を返す。
     *
     * @param id ファクトリーID
     *
     * @return true: 有り, false: 無し
     *
     * @else
     *
     * @brief Factory presence check
     *
     * Return a factory having ID or not.
     *
     * @param id Factory ID.
     *
     * @return true: found, false: not found
     *
     * @endif
     */
    bool hasFactory(const Identifier& id)
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      return static_cast<bool>(m_creators.count(id) != 0);
    }

    /*!
     * @if jp
     *
     * @brief ファクトリーIDリスト取得
     *
     * ファクトリーIDリストを返す。
     *
     * @return ID リスト
     *
     * @else
     *
     * @brief Get factory ID list
     *
     * Return a list of factory ID.
     *
     * @return ID list
     *
     * @endif
     */
    std::vector<Identifier> getIdentifiers()
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      std::vector<Identifier> idlist;
      idlist.reserve(m_creators.size());

      FactoryMapIt it(m_creators.begin());
      FactoryMapIt it_end(m_creators.end());

      while (it != it_end)
        {
          idlist.emplace_back(it->first);
          ++it;
        }
      return idlist;
    }

    /*!
     * @if jp
     *
     * @brief ファクトリー登録
     *
     * ファクトリーを登録する。
     *
     * @param id ファクトリーID
     * @param creator クリエータ用ファンクタ
     *
     * @return OK: 正常終了
     *         ALREADY_EXISTS: 登録済み
     *         INVALID_ARG: creator が不正な値を含む
     *
     * @else
     *
     * @brief Add factory
     *
     * Add a factory.
     *
     * @param id Factory ID.
     * @param creator Functor for creator.
     *
     * @return OK: Successful
     *         ALREADY_EXISTS: already exists.
     *         INVALID_ARG: creator with invalid values.
     *
     * @endif
     */
    FactoryReturn addFactory(const Identifier& id,
                             Creator creator)
    {
      if (creator == nullptr) { return FactoryReturn::INVALID_ARG; }
      std::lock_guard<std::mutex> guard(m_mutex);
      if (m_creators.count(id) != 0) { return FactoryReturn::ALREADY_EXISTS; }
      m_creators[id] = creator;
      return FactoryReturn::OK;
    }

    /*!
     * @if jp
     *
     * @brief ファクトリー削除
     *
     * ファクトリーを削除する。
     *
     * @param id ファクトリーID
     *
     * @return OK: 正常終了
     *         NOT_FOUND: ID未登録
     *
     * @else
     *
     * @brief Remove factory
     *
     * Remove a factory.
     *
     * @param id Factory ID.
     *
     * @return OK: Successful
     *         NOT_FOUND: ID not found
     *
     * @endif
     */
    FactoryReturn removeFactory(const Identifier& id)
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      if (m_creators.count(id) == 0) { return FactoryReturn::NOT_FOUND; }
      m_creators.erase(id);
      return FactoryReturn::OK;
    }

    /*!
     * @if jp
     *
     * @brief ファクトリーオブジェクト生成
     *
     * ファクトリーオブジェクトを生成する。
     *
     * @param id ファクトリーID
     *
     * @return AbstractClass のポインタ
     *
     * @else
     *
     * @brief Create factory object
     *
     * Create a factory object.
     *
     * @param id Factory ID.
     *
     * @return Pointer of AbstractClass
     *
     * @endif
     */
    AbstractClass* createObject(const Identifier& id)
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      if (m_creators.count(id) == 0) { return nullptr; }
      AbstractClass* obj = m_creators[id]();
      return obj;
    }

  private:
    FactoryMap m_creators;
    std::mutex m_mutex;
  };



  /*!
   * @if jp
   *
   * @class GlobalFactory
   * @brief GlobalFactory テンプレートクラス
   *
   * @else
   *
   * @class GlobalFactory
   * @brief GlobalFactory template class
   *
   * @endif
   */
  template <
    class AbstractClass,
    typename Identifier = std::string,
    typename Compare = std::less<Identifier>,
    typename Creator = AbstractClass* (*)()
    >
  class GlobalFactory
    : public Factory<AbstractClass, Identifier, Compare, Creator>,
      public coil::Singleton<GlobalFactory<AbstractClass,
                                           Identifier,
                                           Compare,
                                           Creator> >
  {
  public:
  private:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    GlobalFactory() = default;

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * デストラクタ。
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~GlobalFactory() = default;

    friend class Singleton<GlobalFactory>;
  };

} // namespace coil


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( pop )
#endif

#endif // COIL_FACTORY_H