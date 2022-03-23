#include <rtm/RTC.h>
#include <string.h>
#include <algorithm>
#include "NamingContext.h"
#include "ObjectBinding.h"
#include "RTObjectBinding.h"
#include "ManagerBinding.h"
#include "BindingIterator.h"
#ifdef ORB_IS_OMNIORB
#include <omniORB4/omniURI.h>
#else
#include <coil/stringutil.h>
#include <regex>
#endif
#include <iostream>


#define RTOBJECT_KIND "rtc"
#define MANAGER_KIND "mgr"


namespace RTM
{

  /*!
   * @if jp
   *
   * @brief コンストラクタ
   *
   * コンストラクタ
   *
   * @param poa POAオブジェクト
   *
   * @else
   *
   * @brief Constructor
   *
   * Constructor
   *
   * @param poa
   *
   * @endif
   */
  NamingContext::NamingContext(PortableServer::POA_ptr poa)
  : m_poa(PortableServer::POA::_duplicate(poa))
  {
    poa->activate_object(this);
  }

  /*!
   * @if jp
   *
   * @brief デストラクタ
   *
   * デストラクタ
   *
   *
   * @else
   *
   * @brief Destructor
   *
   * Destructor
   *
   *
   * @endif
   */
  NamingContext::~NamingContext()
  {
    
    std::lock_guard<std::mutex> guard(m_mutex);

    for (auto& ob : m_objects) {
      delete ob;
    }
    
  }

  /*
  PortableServer::ObjectId* NamingContext::PR_id() {
    return m_poa->servant_to_id(this);
  }
  */

  /*!
   * @if jp
   *
   * @brief CORBAオブジェクトをバインドする
   * バインドするパスはCosNaming::Name形式で指定する。
   * CosNaming::Name形式はidとkindのペアのリスト
   *
   *
   * @param n CosNaming::Name形式の登録パス
   * @param obj CORBAオブジェクト
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param obj
   *
   *
   * @endif
   */
  void NamingContext::bind(const CosNaming::Name& n, CORBA::Object_ptr obj) {
    bind_sub(n, obj, CosNaming::nobject, false);
  }

  /*!
   * @if jp
   *
   * @brief CORBAオブジェクトをバインドする
   * 指定パスでバインド済みの場合は、バインド済みオブジェクトはアンバインドする
   *
   *
   * @param n CosNaming::Name形式の登録パス
   * @param obj CORBAオブジェクト
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param obj
   *
   *
   * @endif
   */
  void NamingContext::rebind(const CosNaming::Name& n, CORBA::Object_ptr obj) {
    bind_sub(n, obj, CosNaming::nobject, true);
  }

  /*!
   * @if jp
   *
   * @brief ネーミングコンテキストをバインドする
   *
   *
   * @param n CosNaming::Name形式の登録パス
   * @param nc ネーミングコンテキスト
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param nc
   *
   *
   * @endif
   */
  void NamingContext::bind_context(const CosNaming::Name& n,
        CosNaming::NamingContext_ptr nc) {
    bind_sub(n, nc, CosNaming::ncontext, false);
  }

  /*!
   * @if jp
   *
   * @brief ネーミングコンテキストをバインドする
   * 指定パスでバインド済みの場合は、バインド済みオブジェクトはアンバインドする
   *
   *
   * @param n CosNaming::Name形式の登録パス
   * @param nc ネーミングコンテキスト
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param nc
   *
   *
   * @endif
   */
  void NamingContext::rebind_context(const CosNaming::Name& n,
          CosNaming::NamingContext_ptr nc) {
    bind_sub(n, nc, CosNaming::ncontext, true);
  }

  /*!
   * @if jp
   *
   * @brief 指定パスに登録したCORBAオブジェクトを取得する。
   * 指定パスのオブジェクトが存在しない場合はCosNaming::NamingContext::NotFoundの例外処理を行う
   *
   * @param n CosNaming::Name形式の登録パス
   * @return CORBAオブジェクト
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @return 
   *
   *
   * @endif
   */
  CORBA::Object_ptr NamingContext::resolve(const CosNaming::Name& n)
  {
    if (n.length() == 1) {
      std::lock_guard<std::mutex> guard(m_mutex);
      ObjectBinding *ob = resolve(n[0]);
      if(ob == nullptr)
      {
        throw CosNaming::NamingContext::NotFound(CosNaming::NamingContext::
              missing_node, n);
      }
      return CORBA::Object::_duplicate(ob->get_object());
    }
    else {
      CosNaming::Name restOfName;
      std::lock_guard<std::mutex> guard(m_mutex);
      CosNaming::NamingContext_var context = resolve(n, restOfName);
      return context->resolve(restOfName);
    }
  }

  /*!
   * @if jp
   *
   * @brief 指定パスのCORBAオブジェクトをアンバインドする
   * 指定パスのオブジェクトが存在しない場合はCosNaming::NamingContext::NotFoundの例外処理を行う
   *
   * @param n CosNaming::Name形式の登録パス
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   *
   *
   * @endif
   */
  void NamingContext::unbind(const CosNaming::Name& n)
  {
    if (n.length() == 1) {
      std::lock_guard<std::mutex> guard(m_mutex);

      ObjectBinding *ob = resolve(n[0]);
      if(ob == nullptr)
      {
        throw CosNaming::NamingContext::NotFound(CosNaming::NamingContext::
              missing_node, n);
      }
      remove_object(ob);
    }
    else {
      CosNaming::Name restOfName;
      std::lock_guard<std::mutex> guard(m_mutex);
      CosNaming::NamingContext_var context = resolve(n, restOfName);

      context->unbind(restOfName);
    }
  }

  /*!
   * @if jp
   *
   * @brief ネーミングコンテキストを生成する
   *
   * @return ネーミングコンテキスト
   *
   * @else
   *
   * @brief 
   *
   * @return
   *
   *
   * @endif
   */
  CosNaming::NamingContext_ptr NamingContext::new_context()
  {
    NamingContext* nc = new NamingContext(m_poa);
    CosNaming::NamingContext_ptr ncref = nc->_this();
    nc->_remove_ref();

    return ncref;

    //CosNaming::NamingContext_var result = nc->_this ();
    //return result._retn ();
  }

  /*!
   * @if jp
   *
   * @brief ネーミングコンテキストを生成し、指定パスにバインドする
   *
   * @param n CosNaming::Name形式の登録パス
   *
   * @return ネーミングコンテキスト
   *
   * @else
   *
   * @brief 
   *
   * @param n
   *
   * @return
   *
   *
   * @endif
   */
  CosNaming::NamingContext_ptr
  NamingContext::bind_new_context(const CosNaming::Name& n)
  {
    if (n.length() == 1) {

      CosNaming::NamingContext_ptr nc = new_context();
      try {
        bind_context(n, nc);
      }
      catch (...) {
        nc->destroy();
        CORBA::release(nc);
        throw;
      }
      return nc;
    }
    else {
      CosNaming::Name restOfName;
      CosNaming::NamingContext_var context = resolve(n, restOfName);

      return context->bind_new_context(restOfName);
    }
  }

  /*!
   * @if jp
   *
   * @brief 終了処理
   * ただし、バインド済みのオブジェクトが全てアンバインドされていない場合は
   * CosNaming::NamingContext::NotEmptyの例外処理を行う
   *
   *
   * @else
   *
   * @brief 
   *
   *
   *
   * @endif
   */
  void NamingContext::destroy()
  {
    if (!m_objects.empty())
      throw CosNaming::NamingContext::NotEmpty();

    PortableServer::ObjectId_var id = m_poa->servant_to_id(this);
    m_poa->deactivate_object(id);
  }

  /*!
   * @if jp
   *
   * @brief バインド済みオブジェクトのリストとイテレータオブジェクトを取得する
   * how_manyで指定した位置までの要素を取得できる
   * ただし、how_manyが要素数を超えている場合はイテレータオブジェクトにnilを格納し、
   * 全ての要素を格納したリストを返す。
   *
   * @param how_many 要素の位置
   * @param bl バインド済みオブジェクトのリスト
   * @param bi イテレータオブジェクト
   *
   *
   * @else
   *
   * @brief 
   *
   * @param how_many 
   * @param bl 
   * @param bi 
   *
   * @endif
   */
  void NamingContext::list(CORBA::ULong how_many, CosNaming::BindingList_out bl,
            CosNaming::BindingIterator_out bi)
  {
    CosNaming::BindingList_var all;
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      all = new CosNaming::BindingList();
      all->length(static_cast<CORBA::ULong>(m_objects.size()));

      unsigned int i=0;

      for (const auto& ob : m_objects) {
        all[i] = ob->get_binding();
        i++;
      }
    }

    if (all->length() <= how_many) {
        bi = CosNaming::BindingIterator::_nil();
        bl = all._retn();
        return;
    }

    BindingIterator* bii = new BindingIterator(m_poa, all._retn());
    bii->next_n(how_many, bl);

    bi = bii->_this();
    bii->_remove_ref();
  }

  /*!
   * @if jp
   *
   * @brief CosNaming::Nameオブジェクトを文字列に変換する
   * 例：{{id:"id1", kind:"kind1"},{id:"id2", kind:"kind2"},{id:"id3", kind:"kind3"}}
   * -> id1.kind1/id2.kind2/id3.kind3
   *
   * @param n CosNaming::Name形式のパス
   * @return 文字列によるパス
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @return 
   *
   * @endif
   */
  char* NamingContext::to_string(const CosNaming::Name& name)
  {
#ifdef ORB_IS_OMNIORB
    return omni::omniURI::nameToString(name);
#else
    if (name.length() == 0)
      throw CosNaming::NamingContext::InvalidName();

    std::string str_name;
    for (CORBA::ULong i = 0; i < name.length(); i++)
    {
      std::string id(name[i].id.in());
      id = coil::replaceString(id, ".", "\\.");
      id = coil::replaceString(id, "\\", "\\\\");
      id = coil::replaceString(id, "/", "\\/");
      std::string kind(name[i].kind.in());
      kind = coil::replaceString(kind, ".", "\\.");
      kind = coil::replaceString(kind, "\\", "\\\\");
      kind = coil::replaceString(kind, "/", "\\/");
      str_name += id + "." + kind;
      if (i < name.length() - 1)
      {
        str_name += "/";
      }
    }
    return CORBA::string_dup(str_name.c_str());
#endif
  }

  /*!
   * @if jp
   *
   * @brief 文字列をCosNaming::Nameオブジェクトに変換する
   * 例：id1.kind1/id2.kind2/id3.kind3
   * -> {{id:"id1", kind:"kind1"},{id:"id2", kind:"kind2"},{id:"id3", kind:"kind3"}}
   *
   * @param sn 文字列によるパス
   * @return CosNaming::Name形式のパス
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @return 
   *
   * @endif
   */
  CosNaming::Name* NamingContext::to_name(const char* sn)
  {
#ifdef ORB_IS_OMNIORB
    return omni::omniURI::stringToName(sn);
#else
    CosNaming::Name_var n = new CosNaming::Name();
    std::string str_name(sn);
    std::string name;
    std::vector<std::string> names;

    std::regex reslash{ "(\\w+)/" };

    auto split_s_ite = std::sregex_token_iterator(str_name.begin(), str_name.end(), reslash, { -1, 1 });
    auto end = std::sregex_token_iterator();

    while (split_s_ite != end)
    {
      std::string m(*split_s_ite++);
      if (!m.empty())
      {
        names.emplace_back(std::move(m));
      }
    }


    n->length(static_cast<CORBA::ULong>(names.size()));
    for (CORBA::ULong i=0;i < n->length();i++)
    {
      std::string &id_kind(names[i]);
      std::regex redot{ "(.*\\w)\\.(.*)" };

      auto split_d_ite = std::sregex_token_iterator(id_kind.begin(), id_kind.end(), redot, { 1, 2 });

      coil::vstring v_str_id_kind = coil::split(names[i], ".");

      unsigned int count = 0;
      while (split_d_ite != end)
      {
        std::string m(*split_d_ite++);
        coil::replaceString(m, "\\.", ".");
        coil::replaceString(m, "\\/", "/");
        coil::replaceString(m, "\\\\", "\\");
        if (count == 0)
        {
          n[i].id = CORBA::string_dup(m.c_str());
        }
        else
        {
          n[i].kind = CORBA::string_dup(m.c_str());
        }
        count++;

      }

    }
    return n._retn();
#endif
  }

  /*!
   * @if jp
   *
   * @brief アドレス、オブジェクトキーからcorbaname形式のURIを取得する
   * 例：アドレス:iiop:192.168.11.1:2809、オブジェクトキー：ConsoleOut0.rtc
   * -> corbaname:iiop:192.168.11.1:2809#ConsoleOut0.rtc
   *
   * @param addr アドレス
   * @param sn オブジェクトキー
   * @return corbaname形式のURI
   *
   *
   * @else
   *
   * @brief 
   *
   * @param addr 
   * @param sn 
   * @return 
   *
   * @endif
   */
  char* NamingContext::to_url(const char* addr, const char* sn)
  {
#ifdef ORB_IS_OMNIORB
    return omni::omniURI::addrAndNameToURI(addr, sn);
#else
    std::string url("corbaname:");
    url += addr;
    url += "#";
    static const char* non_escaped_punctuation[] =
    { ";", "/", ":", "?", "@", "=", "+", "$", ",", "-",
      "_", ".", "!", "~", "*", "\"", "(", ")" };
    static const char* hex_chars = "0123456789abcdef";
    size_t size = sizeof(non_escaped_punctuation) / sizeof(char);
    std::string sn_str(sn);
    for (size_t i = 0; i < size; i++)
    {
      if (sn_str.find(non_escaped_punctuation[i]) != std::string::npos)
      {
        std::string to("%");
        const char& c = non_escaped_punctuation[i][0];
        unsigned int v1 = static_cast<unsigned int>(c) >> 4;
        to += hex_chars[v1 & 0x0f];
        unsigned int v2 = static_cast<unsigned int>(c);
        to += hex_chars[v2 & 0x0f];
        sn_str = coil::replaceString(sn_str, non_escaped_punctuation[i], to);
      }
    }
    url += sn_str;
    return CORBA::string_dup(url.c_str());
#endif
  }

  /*!
   * @if jp
   *
   * @brief 文字列で指定したパスのCORBAオブジェクトを取得する
   *
   * @param n パス(例：id1.kind1/id2.kind2/object.rtc)
   * @return CORBAオブジェクト
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n
   * @return 
   *
   * @endif
   */
  CORBA::Object_ptr NamingContext::resolve_str(const char* sn)
  {
#ifdef ORB_IS_OMNIORB
    CosNaming::Name_var name = omni::omniURI::stringToName(sn);
    return resolve(name);
#else
    CosNaming::Name_var name = to_name(sn);
    return resolve(name);
#endif
  }

  /*!
   * @if jp
   *
   * @brief 指定のオブジェクトをリストから削除する
   *
   * @param obj バインディングオブジェクト
   * @return true：削除成功、false：指定のオブジェクトが存在しない
   *
   *
   * @else
   *
   * @brief 
   *
   * @param obj
   * @return 
   *
   * @endif
   */
  bool NamingContext::remove_object(const ObjectBinding* obj)
  {
    size_t count = std::count(m_objects.begin(), m_objects.end(), obj);

    if(count == 0)return false;
    m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), obj), m_objects.end());
    delete obj;
    return true;
  }

  /*!
   * @if jp
   *
   * @brief 指定のオブジェクトを指定位置にリストに追加する
   *
   * @param obj バインディングオブジェクト
   * @param nextobj 追加位置の次のオブジェクト
   * @return true：追加成功、false：追加位置のオブジェクトが存在しない
   *
   *
   * @else
   *
   * @brief 
   *
   * @param obj 
   * @param nextobj 
   * @return 
   *
   * @endif
   */
  bool NamingContext::insert_object(ObjectBinding* obj, const ObjectBinding* nextobj)
  {
    auto obj_itr = std::find(m_objects.begin(), m_objects.end(), nextobj);
    if(obj_itr == m_objects.end())return false;
    m_objects.insert(obj_itr, obj);
    return true;
  }

  /*!
   * @if jp
   *
   * @brief 指定のCORBAオブジェクトからバインディングオブジェクトを生成しリストに追加する
   * kindが"rtc"の場合はRTObjectBindingオブジェクト、kindが"mgr"の場合はManagerBindingオブジェクト、
   * それ以外の場合はObjectBindingオブジェクトをリストに追加する
   *
   * @param n バインディングオブジェクト
   * @param t バインディング型
   * @param obj CORBAオブジェクト
   * @return true：追加成功
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param t 
   * @param obj 
   * @return 
   *
   * @endif
   */
  bool NamingContext::add_object(const CosNaming::Name& n, CosNaming::BindingType t, CORBA::Object_ptr obj)
  {
    if(t == CosNaming::nobject && (strcmp(n[0].kind, RTOBJECT_KIND) == 0))
    {
      m_objects.push_back((ObjectBinding*)(new RTObjectBinding(n, t, obj, this)));
    }
    else if(t == CosNaming::nobject && (strcmp(n[0].kind, MANAGER_KIND) == 0))
    {
      m_objects.push_back((ObjectBinding*)(new ManagerBinding(n, t, obj, this)));
    }
    else
    {
      m_objects.push_back(new ObjectBinding(n, t, obj, this));
    }
    return true;
  }

  /*!
   * @if jp
   *
   * @brief CORBAオブジェクト追加の実装関数
   *
   * @param n CosNaming::Name形式の登録パス
   * @param obj CORBAオブジェクト
   * @param t バインディング型
   * @param rebind true：指定パスでオブジェクトがバインド済みの場合に再バインドする
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param obj 
   * @param t 
   * @param rebind 
   *
   * @endif
   */
  void NamingContext::bind_sub(const CosNaming::Name& n, CORBA::Object_ptr obj,
            CosNaming::BindingType t, CORBA::Boolean rebind)
  {
    if (n.length() == 1) {
      std::lock_guard<std::mutex> guard(m_mutex);

      ObjectBinding *ob = resolve(n[0]);
      if(ob != nullptr)
      {
        if (!rebind)
        {
          throw CosNaming::NamingContext::AlreadyBound();
        }
        remove_object(ob);
      }
      add_object(n, t, obj);
    }
    else {
      CosNaming::Name restOfName;
      std::lock_guard<std::mutex> guard(m_mutex);
      
      CosNaming::NamingContext_var context = resolve(n, restOfName);

      if (t == CosNaming::nobject) {
        if (rebind)
        {
          context->rebind(restOfName, obj);
        }
        else
        {
          context->bind(restOfName, obj);
        }
      }
      else {
        if (rebind)
        {
          context->rebind_context(restOfName,
                CosNaming::NamingContext::_narrow(obj));
        }
        else
        {
          context->bind_context(restOfName,
                    CosNaming::NamingContext::_narrow(obj));
        }
      }
    }
  }

  /*!
   * @if jp
   *
   * @brief 指定のネームコンポーネントのオブジェクトを取得する
   *
   * @param name CosNaming::NameComponent形式のオブジェクト名
   * @return バインディングオブジェクト
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @return 
   *
   * @endif
   */
  ObjectBinding* NamingContext::resolve(const CosNaming::NameComponent& n)
  {
    for (const auto& ob : m_objects) {
      if ((strcmp(n.id,ob->get_binding().binding_name[0].id) == 0) &&
         (strcmp(n.kind,ob->get_binding().binding_name[0].kind) == 0))
        {
          return ob;
        }
    }

    return nullptr;
  }

  /*!
   * @if jp
   *
   * @brief 指定パスの先頭のネームコンポーネントのネーミングコンテキストと
   *        先頭のネームコンポーネントを除いたパスを取得する
   *
   * @param name CosNaming::Name形式の登録パス
   * @param restOfName 先頭を除いたパス
   * @return ネーミングコンテキスト
   *
   *
   * @else
   *
   * @brief 
   *
   * @param n 
   * @param restOfName 
   * @return 
   *
   * @endif
   */
  CosNaming::NamingContext_ptr
  NamingContext::resolve(const CosNaming::Name& n,
            CosNaming::Name& restOfName)
  {
    
    if (n.length() == 0) {
      throw CosNaming::NamingContext::InvalidName();
    }
    
    CosNaming::Name contextName = n;
    contextName.length(1);
    restOfName.length(n.length() - 1);
    for (unsigned int i = 0; i < n.length() - 1; i++) {
      restOfName[i] = n[i + 1];
    }

    ObjectBinding* ob = resolve(contextName[0]);
    if(ob == nullptr)
    {
      throw CosNaming::NamingContext::NotFound(CosNaming::NamingContext::
              missing_node, n);
    }

    CosNaming::NamingContext_var context = CosNaming::NamingContext::_narrow(ob->get_object());

    if (CORBA::is_nil(static_cast<CosNaming::NamingContext_ptr>(context)) ||
        (ob->get_binding().binding_type != CosNaming::ncontext))
    {
      throw CosNaming::NamingContext::NotFound(CosNaming::NamingContext::
                not_context, n);
    }
    return CosNaming::NamingContext::_duplicate(context);
  }

} // namespace RTM
