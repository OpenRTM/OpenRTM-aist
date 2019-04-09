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
#include <algorithm>
#include <functional>
#include <string>
#include <map>
#include <utility>
#include <vector>

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
   * @brief Destructor テンプレート
   *
   * @else
   *
   * @brief Destructor template
   *
   * @endif
   */
  template <class AbstractClass, class ConcreteClass>
  void Destructor(AbstractClass*& obj)
  {
    if (obj == nullptr) { return; }
    ConcreteClass* tmp = dynamic_cast<ConcreteClass*>(obj);
    if (tmp == nullptr) { return; }
    delete obj;
    obj = nullptr;
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
    typename Creator = AbstractClass* (*)(),
    typename Destructor = void (*)(AbstractClass*&)
    >
  class Factory
  {
    class FactoryEntry;
  public:
    typedef std::map<Identifier, FactoryEntry> FactoryMap;
    typedef typename FactoryMap::iterator      FactoryMapIt;
    typedef std::map<AbstractClass*, FactoryEntry> ObjectMap;
    typedef typename ObjectMap::iterator           ObjectMapIt;

    enum ReturnCode
      {
        FACTORY_OK,
        FACTORY_ERROR,
        ALREADY_EXISTS,
        NOT_FOUND,
        INVALID_ARG,
        UNKNOWN_ERROR
      };

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
      std::vector<Identifier> idlist;
      idlist.reserve(m_creators.size());

      FactoryMapIt it(m_creators.begin());
      FactoryMapIt it_end(m_creators.end());

      while (it != it_end)
        {
          idlist.push_back(it->first);
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
     * @param destructor デストラクタ用ファンクタ
     *
     * @return FACTORY_OK: 正常終了
     *         ALREADY_EXISTS: 登録済み
     *         INVALID_ARG: creator か destructor が不正な値を含む
     *
     * @else
     *
     * @brief Add factory
     *
     * Add a factory.
     *
     * @param id Factory ID.
     * @param creator Functor for creator.
     * @param destructor Functor for destructor.
     *
     * @return FACTORY_OK: Successful
     *         ALREADY_EXISTS: already exists.
     *         INVALID_ARG: creator or destructor with invalid values.
     *
     * @endif
     */
    ReturnCode addFactory(const Identifier& id,
                          Creator creator,
                          Destructor destructor)
    {
      if (creator == 0 || destructor == 0) { return INVALID_ARG; }
      if (m_creators.count(id) != 0) { return ALREADY_EXISTS; }
      FactoryEntry f(id, creator, destructor);
      m_creators[id] = f;
      return FACTORY_OK;
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
     * @return FACTORY_OK: 正常終了
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
     * @return FACTORY_OK: Successful
     *         NOT_FOUND: ID not found
     *
     * @endif
     */
    ReturnCode removeFactory(const Identifier& id)
    {
      if (m_creators.count(id) == 0) { return NOT_FOUND; }
      m_creators.erase(id);
      return FACTORY_OK;
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
      if (m_creators.count(id) == 0) { return nullptr; }
      AbstractClass* obj = m_creators[id].creator_();
      assert(m_objects.count(obj) == 0);
      m_objects[obj] = m_creators[id];
      return obj;
    }

    /*!
     * @if jp
     *
     * @brief ファクトリーオブジェクト削除
     *
     * ファクトリーオブジェクトを削除する。
     *
     * @param id ファクトリーID
     * @param obj ファクトリーオブジェクト
     *
     * @else
     *
     * @brief Delete factory object
     *
     * Delete a factory object.
     *
     * @param id Factory ID.
     * @param obj Factory object.
     *
     * @endif
     */
    ReturnCode deleteObject(const Identifier& id, AbstractClass*& obj)
    {
      if (m_creators.count(id) == 0)
        {
          return deleteObject(obj);
        }
      m_creators[id].destructor_(obj);
      m_objects.erase(obj);
      return FACTORY_OK;
    }

    /*!
     * @if jp
     *
     * @brief ファクトリーオブジェクト削除
     *
     * ファクトリーオブジェクトを削除する。
     *
     * @param obj ファクトリーオブジェクト
     *
     * @else
     *
     * @brief Delete factory object
     *
     * Delete a factory object.
     *
     * @param obj Factory object.
     *
     * @endif
     */
    ReturnCode deleteObject(AbstractClass*& obj)
    {
      if (m_objects.count(obj) == 0) { return NOT_FOUND; }
      AbstractClass* tmp(obj);
      m_objects[obj].destructor_(obj);
      m_objects.erase(tmp);
      return FACTORY_OK;
    }

    /*!
     * @if jp
     *
     * @brief 生成済みオブジェクトリストの取得
     *
     * このファクトリで生成されたオブジェクトのリストを取得する。
     *
     * @return 生成済みオブジェクトリスト
     *
     * @else
     *
     * @brief Getting created objects
     *
     * This operation returns a list of created objects by the factory.
     *
     * @return created object list
     *
     * @endif
     */
    std::vector<AbstractClass*> createdObjects()
    {
      std::vector<AbstractClass*> objects;
      for (ObjectMapIt it(m_objects.begin()); it != m_objects.end(); ++it)
        {
          objects.push_back(it->first);
        }
      return objects;
    }

    /*!
     * @if jp
     *
     * @brief オブジェクトがこのファクトリの生成物かどうか調べる
     *
     * @param obj 対象オブジェクト
     * @return true: このファクトリの生成物
     *         false: このファクトリの生成物ではない
     *
     * @else
     *
     * @brief Whether a object is a product of this factory
     *
     * @param obj A target object
     * @return true: The object is a product of the factory
     *         false: The object is not a product of the factory
     *
     * @return created object list
     *
     * @endif
     */
    bool isProducerOf(AbstractClass* obj)
    {
      return m_objects.count(obj) != 0;
    }

    /*!
     * @if jp
     *
     * @brief オブジェクトからクラス識別子(ID)を取得する
     *
     * 当該オブジェクトのクラス識別子(ID)を取得する。
     *
     * @param obj [in] クラス識別子(ID)を取得したいオブジェクト
     * @param id [out] クラス識別子(ID)
     * @return リターンコード NOT_FOUND: 識別子が存在しない
     *                        FACTORY_OK: 正常終了
     * @else
     *
     * @brief Getting class identifier (ID) from a object
     *
     * This operation returns a class identifier (ID) from a object.
     *
     * @param obj [in] An object to investigate its class ID.
     * @param id [out] Class identifier (ID)
     * @return Return code NOT_FOUND: ID not found
     *                        FACTORY_OK: normal return
     * @endif
     */
    ReturnCode objectToIdentifier(AbstractClass* obj, Identifier& id)
    {
      if (m_objects.count(obj) == 0) { return NOT_FOUND; }
      id = m_objects[obj].id_;
      return FACTORY_OK;
    }

    /*!
     * @if jp
     *
     * @brief オブジェクトのコンストラクタを取得する
     *
     * このファクトリで生成されたオブジェクトのコンストラクタを取得する。
     * obj はこのファクトリで生成されたものでなければならない。予め
     * isProducerOf() 関数で当該オブジェクトがこのファクトリの生成物で
     * あるかどうかをチェックしなければならない。
     *
     * @return オブジェクトのデストラクタ
     *
     * @else
     *
     * @brief Getting destructor of the object
     *
     * This operation returns a constructor of the object created by
     * the factory.  obj must be a product of the factory.  User must
     * check if the object is a product of the factory by using
     * isProducerOf()-function, before using this function.
     *
     * @return destructor of the object
     *
     * @endif
     */
    Creator objectToCreator(AbstractClass* obj)
    {
      return m_objects[obj].creator_;
    }

    /*!
     * @if jp
     *
     * @brief オブジェクトのデストラクタを取得する
     *
     * このファクトリで生成されたオブジェクトのデストラクタを取得する。
     * obj はこのファクトリで生成されたものでなければならない。予め
     * isProducerOf() 関数で当該オブジェクトがこのファクトリの生成物で
     * あるかどうかをチェックしなければならない。
     *
     * @return オブジェクトのデストラクタ
     *
     * @else
     *
     * @brief Getting destructor of the object
     *
     * This operation returns a destructor of the object created by
     * the factory.  obj must be a product of the factory.  User must
     * check if the object is a product of the factory by using
     * isProducerOf()-function, before using this function.
     *
     * @return destructor of the object
     *
     * @endif
     */
    Destructor objectToDestructor(AbstractClass* obj)
    {
      return m_objects[obj].destructor_;
    }

  private:
    /*!
     * @if jp
     *
     * @class FactoryEntry
     * @brief FactoryEntry クラス
     *
     * @else
     *
     * @class FactoryEntry
     * @brief FactoryEntry class
     *
     * @endif
     */
    class FactoryEntry
    {
    public:
      FactoryEntry()
      {
      }

      /*!
       * @if jp
       *
       * @brief コンストラクタ
       *
       * コンストラクタ。
       *
       * @param creator クリエータ用ファンクタ
       * @param destructor デストラクタ用ファンクタ
       *
       * @else
       *
       * @brief Constructor
       *
       * Constructor
       *
       * @param creator Functor for creator.
       * @param destructor Functor for destructor.
       *
       * @endif
       */
      FactoryEntry(Identifier id, Creator creator, Destructor destructor)
        : id_(std::move(id)), creator_(creator), destructor_(destructor)
      {
      }
      std::string id_;
      Creator creator_;
      Destructor destructor_;
    };
    FactoryMap m_creators;
    ObjectMap  m_objects;
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
    typename Creator = AbstractClass* (*)(),
    typename Destructor = void (*)(AbstractClass*&)
    >
  class GlobalFactory
    : public Factory<AbstractClass, Identifier, Compare, Creator, Destructor>,
      public coil::Singleton<GlobalFactory<AbstractClass,
                                           Identifier,
                                           Compare,
                                           Creator,
                                           Destructor> >
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
    GlobalFactory() {}

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
    ~GlobalFactory() {}

    friend class Singleton<GlobalFactory>;
  };

} // namespace coil


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( pop )
#endif

#endif // COIL_FACTORY_H
