#ifndef RTC_BINDINGITERATOR_H
#define RTC_BINDINGITERATOR_H

#include "NamingContext.h"



namespace RTM
{
  
  /*!
   * @if jp
   * @class BindingIterator クラス
   * @brief BindingIterator クラス
   * 
   *
   * @else
   *
   * @class BindingIterator class
   * @brief BindingIterator class
   *
   * @endif
   */
  class  BindingIterator : public POA_CosNaming::BindingIterator
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
     * @param l バインド済みオブジェクトのリスト
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param name_ poa
     * @param l 
     *
     * @endif
     */
    BindingIterator(PortableServer::POA_ptr poa, CosNaming::BindingList* l);
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
    ~BindingIterator() override = default;
    /*!
     * @if jp
     *
     * @brief 現在指しているバインド済みオブジェクトの次の要素を取得する。
     *
     * @param b バインド済みオブジェクトの取得結果
     *
     * @return 現在の要素が末尾の場合はfalse、次の要素が取得可能な場合はtrue
     *
     * @else
     *
     * @brief 
     *
     *
     * @param b 
     *
     * @return 
     *
     * @endif
     */
    CORBA::Boolean next_one(CosNaming::Binding_out b) override;
    /*!
     * @if jp
     *
     * @brief 現在指しているバインド済みオブジェクトの指定位置の要素を取得する。
     * 要素の位置が配列の現在位置以降の要素数を超える場合、現在位置以降の要素を取得する。
     *
     * @param how_many 要素の位置
     * @param bl バインド済みオブジェクトのリストの取得結果 
     *
     * @return 現在の要素が末尾の場合はfalse、それ以外の場合はtrue
     *
     * @else
     *
     * @brief 
     *
     *
     * @param how_many 
     * @param bl 
     *
     * @return 
     *
     * @endif
     */
    CORBA::Boolean next_n(CORBA::ULong how_many, CosNaming::BindingList_out bl) override;

    /*!
     * @if jp
     *
     * @brief 終了処理
     * CORBAオブジェクトをディアクティベートする。
     *
     *
     * @else
     *
     * @brief 
     *
     *
     * @endif
     */
    void destroy() override;


  private:

    CosNaming::BindingList_var m_list;
    PortableServer::POA_var m_poa;

  };
} // namespace RTM

#endif
