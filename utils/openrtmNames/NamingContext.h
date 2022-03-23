#ifndef RTC_NAMINGCONTEXT_H
#define RTC_NAMINGCONTEXT_H

#include <rtm/RTC.h>
#include <mutex>
#include <vector>
#ifdef ORB_IS_OMNIORB
#include <omniORB4/Naming.hh>
#elif defined(ORB_IS_TAO)
#include <orbsvcs/CosNamingS.h>
#endif





namespace RTM
{
  class ObjectBinding;

  class NamingContext : public POA_CosNaming::NamingContextExt
  {

  public:

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
    NamingContext(PortableServer::POA_ptr poa);
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
    ~NamingContext() override;


    //PortableServer::ObjectId* PR_id();

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
    void bind(const CosNaming::Name& n, CORBA::Object_ptr obj) override;
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
    void rebind(const CosNaming::Name& n, CORBA::Object_ptr obj) override;
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
    void bind_context(const CosNaming::Name& n,
          CosNaming::NamingContext_ptr nc) override;
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
    void rebind_context(const CosNaming::Name& n,
            CosNaming::NamingContext_ptr nc) override;

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
    CORBA::Object_ptr resolve(const CosNaming::Name& n) override;

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
    void unbind(const CosNaming::Name& n) override;
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
    CosNaming::NamingContext_ptr new_context() override;
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
    CosNaming::NamingContext_ptr bind_new_context(const CosNaming::Name& n) override;
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
    void destroy() override;
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
    void list(CORBA::ULong how_many, CosNaming::BindingList_out bl,
        CosNaming::BindingIterator_out bi) override;

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
    char* to_string(const CosNaming::Name& n) override;

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
    CosNaming::Name* to_name(const char* sn) override;

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
    char* to_url(const char* addr, const char* sn) override;

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
    CORBA::Object_ptr resolve_str(const char* n) override;

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
    bool remove_object(const ObjectBinding* obj);
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
    bool insert_object(ObjectBinding* obj, const ObjectBinding* nextobj);
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
    bool add_object(const CosNaming::Name& n, CosNaming::BindingType t, CORBA::Object_ptr obj);

  private:

    std::mutex m_mutex;
    std::vector<ObjectBinding*> m_objects;
    PortableServer::POA_var m_poa;


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
    void bind_sub(const CosNaming::Name& n, CORBA::Object_ptr obj,
        CosNaming::BindingType t, CORBA::Boolean rebind);

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
    ObjectBinding* resolve(const CosNaming::NameComponent& name);

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
    CosNaming::NamingContext_ptr resolve(const CosNaming::Name& name,
              CosNaming::Name& restOfName);
  };
} // namespace RTM

#endif
