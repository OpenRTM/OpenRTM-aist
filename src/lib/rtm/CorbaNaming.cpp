// -*- C++ -*-
/*!
 * @file CorbaNaming.cpp
 * @brief CORBA naming service helper class
 * @date $Date: 2007-12-31 03:08:02 $
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

#include <cassert>
#include <rtm/CorbaNaming.h>
#include <rtm/CORBA_RTCUtil.h>
#include <iostream>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  CorbaNaming::CorbaNaming(CORBA::ORB_ptr orb)
    : m_ptrORB(orb), m_nameServer(""),
      m_rootContext(CosNaming::NamingContextExt::_nil()),
      m_blLength(100)
  {
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  CorbaNaming::CorbaNaming(CORBA::ORB_ptr orb, const char* name_server)
    : m_ptrORB(orb), m_nameServer(name_server),
      m_rootContext(CosNaming::NamingContextExt::_nil()),
      m_blLength(100)
  {
    CORBA::Object_var obj;
    m_nameServer = CORBA_RTCUtil::CorbaURI(m_nameServer, "NameService").toString();
    try
      {
        obj = m_ptrORB->string_to_object(m_nameServer.c_str());
        m_rootContext = CosNaming::NamingContextExt::_narrow(obj);
        if (CORBA::is_nil(m_rootContext)) throw std::bad_alloc();
      }
    catch(...)
      {
        throw std::bad_alloc();
      }
  }

  CorbaNaming::~CorbaNaming() = default; // No inline for gcc warning, too big

  /*!
   * @if jp
   * @brief ネーミングサービスの初期化
   * @else
   * @brief Initialize the Naming Service
   * @endif
   */
  void CorbaNaming::init(const char* name_server)
  {
    m_nameServer = name_server;
    m_nameServer = CORBA_RTCUtil::CorbaURI(m_nameServer, "NameService").toString();
    CORBA::Object_var obj;
    obj = m_ptrORB->string_to_object(m_nameServer.c_str());
    m_rootContext = CosNaming::NamingContextExt::_narrow(obj);
    if (CORBA::is_nil(m_rootContext)) throw std::bad_alloc();
  }

  bool CorbaNaming::isAlive()
  {
    try
      {
        return !m_rootContext->_non_existent();
      }
    catch (...)
      {
        return false;
      }
  }

  /*!
   * @if jp
   * @brief Object を bind する
   * @else
   * @brief Bind object on specified name component position
   * @endif
   */
  void CorbaNaming::bind(const CosNaming::Name& name, CORBA::Object_ptr obj,
                         const bool force)
  {
    try
      {
        m_rootContext->bind(name, obj);
      }
    catch (NotFound&)
      {
        force ? bindRecursive(m_rootContext, name, obj) : throw;
      }
    catch (CannotProceed& e)
      {
#ifndef ORB_IS_RTORB
        force ? bindRecursive(e.cxt, e.rest_of_name, obj) : throw;
#else  // ORB_IS_RTORB
        force ? bindRecursive(e.cxt(), e.rest_of_name(), obj) : throw;
#endif  // ORB_IS_RTORB
      }
  }

  /*!
   * @if jp
   * @brief Object を bind する
   * @else
   * @brief Bind object on specified string name position
   * @endif
   */
  void CorbaNaming::bindByString(const char* string_name, CORBA::Object_ptr obj,
                                 const bool force)
  {
    this->bind(toName(string_name), obj, force);
  }

  /*!
   * @if jp
   * @brief 途中のコンテキストを再帰的に bind しながら Object を bind する
   * @else
   * @brief Bind intermediate context recursively and bind object
   * @endif
   */
  void CorbaNaming::bindRecursive(CosNaming::NamingContext_ptr context,
                                  const CosNaming::Name& name,
                                  CORBA::Object_ptr obj)
  {
    CORBA::ULong len(name.length());
    CosNaming::NamingContext_var cxt;
    cxt = CosNaming::NamingContext::_duplicate(context);

    for (CORBA::ULong i = 0; i < len; ++i)
      {
        if (i == (len - 1))
          { // this operation may throw AlreadyBound,
            cxt->bind(subName(name, i, i), obj);
            return;
          }
        else
          { // If the context is not a NamingContext, CannotProceed is thrown
            if (isNamingContext(cxt))
              cxt = bindOrResolveContext(cxt, subName(name, i, i));
            else
              throw CannotProceed(cxt, subName(name, i));
          }
      }
    return;
  }

  /*!
   * @if jp
   * @brief Object を rebind する
   * @else
   * @brief Rebind object
   * @endif
   */
  void CorbaNaming::rebind(const CosNaming::Name& name,
                           CORBA::Object_ptr obj,
                           const bool force)
  {
    try
      {
        m_rootContext->rebind(name, obj);
      }
    catch (NotFound&)
      {
        force ? rebindRecursive(m_rootContext, name, obj) : throw;
      }
    catch (CannotProceed& e)
      {
#ifndef ORB_IS_RTORB
        force ? rebindRecursive(e.cxt, e.rest_of_name, obj) : throw;
#else  // ORB_IS_RTORB
        force ? rebindRecursive(e.cxt(), e.rest_of_name(), obj) : throw;
#endif  // ORB_IS_RTORB
      }
  }

  /*!
   * @if jp
   * @brief Object を rebind する
   * @else
   * @brief Rebind object
   * @endif
   */
  void CorbaNaming::rebindByString(const char* string_name,
                                   CORBA::Object_ptr obj,
                                   const bool force)
  {
    rebind(toName(string_name), obj, force);
  }

  /*!
   * @if jp
   * @brief 途中のコンテキストを bind しながら Object を rebind する
   * @else
   * @brief Bind intermediate context recursively and rebind object
   * @endif
   */
  void CorbaNaming::rebindRecursive(CosNaming::NamingContext_ptr context,
                                    const CosNaming::Name& name,
                                    CORBA::Object_ptr obj)
  {
    CORBA::ULong len(name.length());
    CosNaming::NamingContext_var cxt;
    cxt = CosNaming::NamingContext::_duplicate(context);

    for (CORBA::ULong i = 0; i < len; ++i)
      {
        if (i == (len - 1))
          {
            cxt->rebind(subName(name, i, i), obj);
            return;
          }
        else
          { // If the context is not a NamingContext, CannotProceed is thrown
            if (isNamingContext(cxt))
              {
                try
                  {
                    cxt = cxt->bind_new_context(subName(name, i, i));
                  }
                catch (AlreadyBound& e)
                  {
                    (void)(e);
                    cxt = CosNaming::
                      NamingContextExt::
                      _narrow(cxt->resolve(subName(name, i, i)));
                  }
              }
            else
              throw CannotProceed(cxt, subName(name, i));
          }
      }
    return;
  }

  /*!
   * @if jp
   * @brief NamingContext を bind する
   * @else
   * @brief Bind NamingContext
   * @endif
   */
  void CorbaNaming::bindContext(const CosNaming::Name& name,
                                CosNaming::NamingContext_ptr name_cxt,
                                const bool force)
  {
    bind(name, name_cxt, force);
  }

  /*!
   * @if jp
   * @brief NamingContext を bind する
   * @else
   * @brief Bind NamingContext
   * @endif
   */
  void CorbaNaming::bindContext(const char* string_name,
                                CosNaming::NamingContext_ptr name_cxt,
                                const bool force)
  {
    bindContext(toName(string_name), name_cxt, force);
  }

  /*!
   * @if jp
   * @brief 途中のコンテキストを再帰的に bind し NamingContext を bind する
   * @else
   * @brief Bind intermediate context recursively and bind NamingContext
   * @endif
   */
  void
  CorbaNaming::bindContextRecursive(CosNaming::NamingContext_ptr context,
                                    const CosNaming::Name& name,
                                    CosNaming::NamingContext_ptr name_cxt)
  {
    bindRecursive(context, name, name_cxt);
    return;
  }

  /*!
   * @if jp
   * @brief NamingContext を rebind する
   * @else
   * @brief Rebind NamingContext
   * @endif
   */
  void CorbaNaming::rebindContext(const CosNaming::Name& name,
                                  CosNaming::NamingContext_ptr name_cxt,
                                  const bool force)
  {
    rebind(name, name_cxt, force);
    return;
  }

  /*!
   * @if jp
   * @brief NamingContext を rebind する
   * @else
   * @brief Rebind NamingContext
   * @endif
   */
  void CorbaNaming::rebindContext(const char* string_name,
                                  CosNaming::NamingContext_ptr name_cxt,
                                  const bool force)
  {
    rebindContext(toName(string_name), name_cxt, force);
  }

  /*!
   * @if jp
   * @brief 途中のコンテキストを再帰的に rebind し NamingContext を rebind する
   * @else
   * @brief Rebind intermediate context recursively and rebind NamingContext
   * @endif
   */
  void
  CorbaNaming::rebindContextRecursive(CosNaming::NamingContext_ptr context,
                                      const CosNaming::Name& name,
                                      CosNaming::NamingContext_ptr name_cxt)
  {
    rebindRecursive(context, name, name_cxt);
    return;
  }

  /*!
   * @if jp
   * @brief 与えられた NameComponent にバインドされている Object を返す
   * @else
   * @brief Return object bound on the specified NameComponent
   * @endif
   */
  CORBA::Object_ptr CorbaNaming::resolve(const CosNaming::Name& name)
  {
    return m_rootContext->resolve(name);
  }

  /*!
   * @if jp
   * @brief 与えられた NameComponent にバインドされている Object を返す
   * @else
   * @brief Return object bound on the specified name
   * @endif
   */
  CORBA::Object_ptr CorbaNaming::resolve(const char* string_name)
  {
    return resolve(toName(string_name));
  }

  /*!
   * @if jp
   * @brief 与えられた NameComponent のバインディングを削除する
   * @else
   * @brief Unbind a binding specified by NameComponent
   * @endif
   */
  void CorbaNaming::unbind(const CosNaming::Name& name)
  {
    m_rootContext->unbind(name);
  }

  /*!
   * @if jp
   * @brief 与えられた NameComponent のバインディングを削除する
   * @else
   * @brief Unbind a binding specified by NameComponent
   * @endif
   */
  void CorbaNaming::unbind(const char* string_name)
  {
    unbind(toName(string_name));
  }

  /*!
   * @if jp
   * @brief 新しいコンテキストを生成する
   * @else
   * @brief Create new NamingContext
   * @endif
   */
  CosNaming::NamingContext_ptr CorbaNaming::newContext()
  {
    return m_rootContext->new_context();
  }

  /*!
   * @if jp
   * @brief 新しいコンテキストを bind する
   * @else
   * pbrief Bind new namingContext
   * @endif
   */
  CosNaming::NamingContext_ptr
  CorbaNaming::bindNewContext(const CosNaming::Name& name, bool force)
  {
    try
      {
        return m_rootContext->bind_new_context(name);
      }
    catch (NotFound&)
      {
        force ? bindRecursive(m_rootContext, name, newContext()) : throw;
      }
    catch (CannotProceed& e)
      {
#ifndef ORB_IS_RTORB
        force ? bindRecursive(e.cxt, e.rest_of_name, newContext()) : throw;
#else  // ORB_IS_RTORB
        force ?
          bindRecursive(e.cxt(), e.rest_of_name(), newContext()) : throw;
#endif  // ORB_IS_RTORB
      }
    return CosNaming::NamingContext::_nil();
  }

  /*!
   * @if jp
   * @brief 新しいコンテキストを bind する
   * @else
   * pbrief Bind new namingContext
   * @endif
   */
  CosNaming::NamingContext_ptr
  CorbaNaming::bindNewContext(const char* string_name, bool force)
  {
    return bindNewContext(toName(string_name), force);
  }

  /*!
   * @if jp
   * @brief NamingContext を非アクティブ化する
   * @else
   * @brief Destroy the naming context
   * @endif
   */
  void CorbaNaming::destroy(CosNaming::NamingContext_ptr context)
  {
    context->destroy();
  }

  /*!
   * @if jp
   * @brief NamingContext を再帰的に下って非アクティブ化する
   * @else
   * @brief Destroy the naming context recursively
   * @endif
   */
  void CorbaNaming::destroyRecursive(CosNaming::NamingContext_ptr context)
  {
    CosNaming::BindingList_var     bl;
    CosNaming::BindingIterator_var bi;
    CORBA::Boolean cont(true);

#ifndef ORB_IS_RTORB
    context->list(m_blLength, bl.out(), bi.out());
#else  // ORB_IS_RTORB
    context->list(m_blLength, (CosNaming::BindingList_out)bl,
                  (CosNaming::BindingIterator_ptr)bi);
#endif  // ORB_IS_RTORB

    while (cont)
      {
        CORBA::ULong len(bl->length());

        for (CORBA::ULong i = 0; i < len; ++i)
          {
            if (bl[i].binding_type == CosNaming::ncontext)
              {  // If Object is context, destroy recursive.
                CosNaming::NamingContext_var next_context;
                next_context = CosNaming::NamingContext::
                  _narrow(context->resolve(bl[i].binding_name));

                // Recursive function call
                destroyRecursive(next_context);  // +++ Recursive call +++
                context->unbind(bl[i].binding_name);
                next_context->destroy();
              }
            else if (bl[i].binding_type == CosNaming::nobject)
              {  // If Object is object, unbind it.
                context->unbind(bl[i].binding_name);
              }
            else
              {
                assert(0);  // never comes here
              }
          }

        // no more binding -> do-while loop will be finished
        if (CORBA::is_nil(bi))
          {
            cont = false;
          }
        else
          {
            bi->next_n(m_blLength, bl);
          }
     }

    if (!CORBA::is_nil(bi)) bi->destroy();
    return;
  }

  /*!
   * @if jp
   * @brief すべての Binding を削除する
   * @else
   * @brief Destroy all bindings
   * @endif
   */
  void CorbaNaming::clearAll()
  {
    destroyRecursive(m_rootContext);
  }

  /*!
   * @if jp
   * @brief 与えられた NamingContext の Binding を取得する
   * @else
   * @brief Get Binding on the given NamingContext
   * @endif
   */
  void CorbaNaming::list(CosNaming::NamingContext_ptr name_cxt,
                         CORBA::ULong how_many,
                         CosNaming::BindingList_var& bl,
                         CosNaming::BindingIterator_var& bi)
  {
#ifndef ORB_IS_RTORB
    name_cxt->list(how_many, bl.out(), bi.out());
#else  // ORB_IS_RTORB
    name_cxt->list(how_many, (CosNaming::BindingList_out)bl,
                   (CosNaming::BindingIterator_ptr)bi);
#endif  // ORB_IS_RTORB

  }

  /*!
   * @if jp
   * @brief 与えられた Naming パス以下のすべてのバインディングを取得する
   * @else
   * @brief Get all the binding under given naming path
   * @endif
   */
  void CorbaNaming::list(const char* string_name,
                         CosNaming::BindingList_var& bl)
  {
    if (string_name == nullptr) { return; }
    CORBA::Object_var obj = resolveStr(string_name);
    CosNaming::NamingContext_var nc;
    nc = CosNaming::NamingContext::_narrow(obj);
    if (CORBA::is_nil(nc))
      {
        bl->length(0);
        return;
      }
    CORBA::Long max_list_size(65536);
    CosNaming::BindingIterator_var bi;
#ifndef ORB_IS_RTORB
    nc->list(max_list_size, bl.out(), bi.out());
#else  // ORB_IS_RTORB
    nc->list(max_list_size, (CosNaming::BindingList_out)bl,
             (CosNaming::BindingIterator_ptr)bi);
#endif  // ORB_IS_RTORB
    CORBA::Long   max_remaining = max_list_size - bl->length();
    CORBA::Boolean more_bindings = !CORBA::is_nil(bi);

    if (more_bindings)
      {
        while (more_bindings && (max_remaining > 0))
          {
            CosNaming::BindingList_var tmp_bl;
            more_bindings = bi->next_n(max_remaining, tmp_bl.out());
            // Append 'tmp_bl' to 'bl'
            CORBA::ULong bl_len = bl->length();
            bl->length(bl_len + tmp_bl->length());
            for (CORBA::ULong i(0); i < tmp_bl->length(); ++i)
              {
                bl[i + bl_len] = tmp_bl[i];
              }
            max_remaining = max_list_size - bl->length();
          }
        bi->destroy();
      }
    return;
  }

  /*!
   * @if jp
   * @brief 与えられたパス以下の指定されたkindのバインディングを取得する
   * @else
   * @brief Get all the binding with specified kind under given naming path
   * @endif
   */
  void CorbaNaming::listByKind(const char* string_name,
                               const char* string_kind,
                               CosNaming::BindingList_var& bl)
  {
    if (string_name == nullptr) { bl->length(0); return; }
    if (string_kind == nullptr) { bl->length(0); return; }
    std::string kind(string_kind);

    CosNaming::BindingList_var tmp_bl;
    list(string_name, tmp_bl);

    CORBA::ULong tmp_len(tmp_bl->length());
    bl->length(tmp_bl->length());
    CORBA::ULong list_len(0);
    for (CORBA::ULong i(0); i < tmp_len; ++i)
      {
        if (tmp_bl[i].binding_type != CosNaming::nobject) { continue; }
        CORBA::ULong last_index = tmp_bl[i].binding_name.length() - 1;
        const char* tmp = tmp_bl[i].binding_name[last_index].kind;
        if (kind != tmp) { continue; }

        bl[list_len] = tmp_bl[i];
        ++list_len;
        const char* tmp_char = toString(tmp_bl[i].binding_name);
        delete tmp_char;
      }
    bl->length(list_len);
    return;
  }

  /*!
   * @if jp
   * @brief 与えられた NameComponent の文字列表現を返す
   * @else
   * @brief Get string representation of given NameComponent
   * @endif
   */
  char* CorbaNaming::toString(const CosNaming::Name& name)
  {
    if (name.length() == 0)
      throw InvalidName();

    CORBA::ULong slen = 0;
    slen = getNameLength(name);

    CORBA::String_var string_name = CORBA::string_alloc(slen);
    nameToString(name, static_cast<char*>(string_name), slen);

    return string_name._retn();
  }

  /*!
   * @if jp
   * @brief 与えられた文字列表現を NameComponent に分解する
   * @else
   * @brief Resolve given string representation to NameComponent
   * @endif
   */
  CosNaming::Name CorbaNaming::toName(const char* sname)
  {
    if (sname == nullptr)         throw InvalidName();
    if (*sname == '\0') throw InvalidName();

    std::string string_name(sname);
    std::vector<std::string> name_comps;

    // String name should include 1 or more names
    CORBA::ULong nc_length = 0;
    nc_length = split(string_name, std::string("/"), name_comps);
    if (!(nc_length > 0)) throw InvalidName();

    // Name components are allocated
    CosNaming::Name_var    name = new CosNaming::Name();
    name->length(nc_length);

    // Insert id and kind to name components
    for (CORBA::ULong i = 0; i < nc_length; ++i)
      {
        std::string::size_type pos;
        pos = name_comps[i].find_last_of('.');
        if (pos != name_comps[i].npos)
          {
            name[i].id   =
              CORBA::string_dup(name_comps[i].substr(0, pos).c_str());
            name[i].kind =
              CORBA::string_dup(name_comps[i].substr(pos + 1).c_str());
          }
        else
          {
            name[i].id   = CORBA::string_dup(name_comps[i].c_str());
#ifndef ORB_IS_RTORB
            name[i].kind = "";
#else  // ORB_IS_RTORB
            name[i].kind = CORBA::string_dup("");
#endif  // ORB_IS_RTORB
          }
      }
    return name;
  }

  /*!
   * @if jp
   * @brief 与えられた addre と string_name から URL表現を取得する
   * @else
   * @brief Get URL representation from given addr and string_name
   * @endif
   */
  char* CorbaNaming::toUrl(char* addr, char* string_name)
  {
    return m_rootContext->to_url(addr, string_name);
  }

  /*!
   * @if jp
   * @brief 与えられた文字列表現を resolve しオブジェクトを返す
   * @else
   * @brief Resolve from name of string representation and get object
   * @endif
   */
  CORBA::Object_ptr CorbaNaming::resolveStr(const char* string_name)
  {
    return resolve(string_name);
  }

  //======================================================================
  // Util functions
  //======================================================================
  /*!
   * @if jp
   * @brief オブジェクトの名前をバインドまたは解決する
   * @else
   * @brief Bind or resolve the given name component
   * @endif
   */
  CORBA::Object_ptr
  CorbaNaming::bindOrResolve(CosNaming::NamingContext_ptr context,
                             const CosNaming::Name& name,
                             CORBA::Object_ptr obj)
  {
    try
      {
        context->bind(name, obj);
        return obj;
      }
    catch (AlreadyBound& e)
      {
        (void)(e);
        return context->resolve(name);
      }
  }

  /*!
   * @if jp
   * @brief 名前をバインドまたは解決する
   * @else
   * @brief Bind or resolve the given name component
   * @endif
   */
  CosNaming::NamingContext_ptr
  CorbaNaming::bindOrResolveContext(CosNaming::NamingContext_ptr context,
                                    const CosNaming::Name& name,
                                    CosNaming::NamingContext_ptr new_context)
  {
    return CosNaming::NamingContext
      ::_narrow(bindOrResolve(context, name, new_context));
  }

  /*!
   * @if jp
   * @brief 名前をバインドまたは解決する
   * @else
   * @brief Bind or resolve the given name component
   * @endif
   */
  CosNaming::NamingContext_ptr
  CorbaNaming::bindOrResolveContext(CosNaming::NamingContext_ptr context,
                                    const CosNaming::Name& name)
  {
    return bindOrResolveContext(context, name, newContext());
  }

  /*!
   * @if jp
   * @brief ネームサーバの名前を取得する
   * @else
   * @brief Get the name of naming server
   * @endif
   */
  const char* CorbaNaming::getNameServer()
  {
    return m_nameServer.c_str();
  }

  /*!
   * @if jp
   * @brief ルートコンテキストを取得する
   * @else
   * @brief Get the root context
   * @endif
   */
  CosNaming::NamingContext_ptr CorbaNaming::getRootContext()
  {
    return CosNaming::NamingContext::_duplicate(m_rootContext);
  }

  /*!
   * @if jp
   * @brief オブジェクトがネーミングコンテキストか判別する
   * @else
   * @brief Determine whether the object is NamingContext
   * @endif
   */
  bool CorbaNaming::isNamingContext(CORBA::Object_ptr obj)
  {
    CosNaming::NamingContext_var nc;
    nc = CosNaming::NamingContext::_narrow(obj);
    return !CORBA::is_nil(nc);
  }

  /*!
   * @if jp
   * @brief 与えられた名前がネーミングコンテキストかどうか
   * @else
   * @brief Determine whether the given name component is NamingContext
   * @endif
   */
  bool CorbaNaming::isNamingContext(const CosNaming::Name& name)
  {
    return isNamingContext(resolve(name));
  }

  /*!
   * @if jp
   * @brief 与えられた名前がネーミングコンテキストかどうか
   * @else
   * @brief Determine whether the given string name is NamingContext
   * @endif
   */
  bool CorbaNaming::isNamingContext(const char* string_name)
  {
    return isNamingContext(resolve(string_name));
  }

  /*!
   * @if jp
   * @brief ネームコンポーネントの部分を返す
   * @else
   * @brief Get subset of given name component
   * @endif
   */
  CosNaming::Name CorbaNaming::subName(const CosNaming::Name& name,
                                       CORBA::Long begin,
                                       CORBA::Long end)
  {
    if (end < 0) end = name.length() - 1;

    CosNaming::Name sub_name;
    CORBA::ULong sub_len(end - (begin - 1));
    if (sub_len > 0)
      {
        sub_name.length(sub_len);
      }
    else
      {
        sub_name.length(0);
        return sub_name;
      }

    for (CORBA::ULong i = 0; i < sub_len; ++i)
      {
        sub_name[i] = name[begin + i];
      }
    return sub_name;
  }

  //------------------------------------------------------------
  // Protected member functions
  //------------------------------------------------------------
  /*!
   * @if jp
   * @brief ネームコンポーネントの文字列表現を取得する
   * @else
   * @brief Get string representation of name component
   * @endif
   */
  void CorbaNaming::nameToString(const CosNaming::Name& name,
                                 char* string_name,
                                 CORBA::ULong slen)
  {
    char* s = string_name;
    for (CORBA::ULong i = 0; i < name.length(); ++i)
      {
        // Copy id to string_name
        for (const char* id = name[i].id; *id != '\0'; ++id)
          {
            if (*id == '/' || *id == '.' || *id == '\\') *s++ = '\\';
            *s++ = *id;
          }
        // '.' if there is a kind, or no id
        if (((const char*)(name[i].id  ))[0] == '\0' ||
            ((const char*)(name[i].kind))[0] != '\0')
          *s++ = '.';
        // Copy kind to string_name
        for (const char* kind = name[i].kind; *kind != '\0'; ++kind)
          {
            if (*kind == '/' || *kind == '.' || *kind == '\\')
              *s++ = '\\';
            *s++ = *kind;
          }
        // The end of string_name will be overwritten by '\0'
        *s++ = '/';
      }
    string_name[slen-1] = '\0';
  }

  /*!
   * @if jp
   * @brief ネームコンポーネントの文字列表現時の文字長を取得する
   * @else
   * @brief Get string length of the name component's string representation
   * @endif
   */
  CORBA::ULong CorbaNaming::getNameLength(const CosNaming::Name& name)
  {
    CORBA::ULong slen = 0;

    for (CORBA::ULong i = 0; i < name.length(); ++i)
      {
        // Count string length of id(s)
        for (const char* id = name[i].id; *id != '\0'; ++id)
          {
            // Escape character '/', '.', '\' will convert to "\/", "\.", "\\".
            if (*id == '/' || *id == '.' || *id == '\\') slen++;
            slen++;
          }
        // If kind exists, space for '.' is counted
        if (((const char*)(name[i].id  ))[0] == '\0' ||
            ((const char*)(name[i].kind))[0] != '\0')
          {
            slen++;
          }
        // Count string length of kind(s)
        for (const char* kind = name[i].kind; *kind != '\0'; kind++)
          {
            if (*kind == '/' || *kind == '.' || *kind == '\\') slen++;
            slen++;
          }
        // Space for '/' or '\0'
        slen++;
      }
    return slen;
  }

  /*!
   * @if jp
   * @brief 文字列の分割
   * @else
   * @brief Split of string
   * @endif
   */
  unsigned int CorbaNaming::split(const std::string& input,
                                  const std::string& delimiter,
                                  std::vector<std::string>& results)
  {
    using size = std::string::size_type;
    size delim_size = delimiter.size();
    size found_pos(0), begin_pos(0), pre_pos(0), substr_size(0);

    if (input.substr(0, delim_size) == delimiter)
      begin_pos = pre_pos = delim_size;

    while (true)
      {
      REFIND:
        found_pos = input.find(delimiter, begin_pos);
        if (found_pos == std::string::npos)
          {
            results.emplace_back(input.substr(pre_pos));
            break;
          }
        if ('\\' == input.at(found_pos - 1))
          {
            begin_pos = found_pos + delim_size;
            goto REFIND;
          }

        substr_size = found_pos - pre_pos;

        if (substr_size > 0)
          {
            results.emplace_back(input.substr(pre_pos, substr_size));
          }
        begin_pos = found_pos + delim_size;
        pre_pos   = found_pos + delim_size;
      }
    return static_cast<unsigned int>(results.size());
  }
} // namespace RTC
