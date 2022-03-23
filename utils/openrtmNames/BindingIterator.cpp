#include "BindingIterator.h"



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
  BindingIterator::BindingIterator(PortableServer::POA_ptr poa, CosNaming::BindingList* l)
    : m_list(l), m_poa(PortableServer::POA::_duplicate(poa))
  {
    PortableServer::ObjectId_var id = poa->activate_object(this);
  }

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
  CORBA::Boolean BindingIterator::next_one(CosNaming::Binding_out b)
  {
    CosNaming::BindingList_var bl;
    CORBA::Boolean ret = next_n(1, bl);

    b = new CosNaming::Binding;
    if (ret)
    {
      *b = bl[0];
    }
    else
    {
      b->binding_type = CosNaming::nobject;
    }
    return ret;
  }
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
  CORBA::Boolean BindingIterator::next_n(CORBA::ULong how_many, CosNaming::BindingList_out bl)
  {

    if (m_list->length() < how_many) {
      how_many = m_list->length();
    }

    if (how_many == 0)
    {
      return false;
    }

    bl = m_list._retn();

    m_list = new CosNaming::BindingList();
    m_list->length(bl->length() - how_many);

    for (CORBA::ULong i = 0; i < m_list->length(); i++) {
      m_list[i] = (*bl)[i + how_many];
    }

    bl->length(how_many);

    return true;
  }
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
  void BindingIterator::destroy()
  {
    PortableServer::ObjectId_var id = m_poa->servant_to_id(this);
    m_poa->deactivate_object(id);
  }
} // namespace RTM
