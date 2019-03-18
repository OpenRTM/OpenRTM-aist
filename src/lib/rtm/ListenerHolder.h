// -*- C++ -*-
/*!
 * @file ListnerHolder.h
 * @brief Listener holder class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTM_UTIL_LISTENERHOLDER_H
#define RTM_UTIL_LISTENERHOLDER_H

#include <coil/Mutex.h>
#include <coil/Guard.h>
#include <coil/NonCopyable.h>

#include <vector>
#include <utility>

namespace RTM
{
namespace util
{
  /*!
   * @if jp
   * @class Listener ホルダークラス
   *
   * このクラスは、リスナクラスの単純な保持、管理を行うリスナホルダクラ
   * スである。このクラスを利用するためには、テンプレートの第１引数に当
   * たるリスナクラス (Listenerクラス) および、このListenerHolderクラス
   * テンプレートを継承して、実際にリスナの呼び出しを行う
   * ListenerHolder実装クラスを実装する必要がある。
   *
   * このクラスは、スレッドセーブを実現するため、リスナの追加と削除につ
   * いてはミューテックスによるロックを行っている。完全にスレッドセーフ
   * なリスナ管理を実現するためにはリスナのコールバックをコールする際に
   * もミューテックするによるロックを行う必要がある。
   *
   * @section Listenerクラスの定義
   *
   * イベント発生時にコールバックされるメンバ関数を持つ基底クラスを定義
   * する。コールバックのためのメンバ関数は、任意の戻り値、引数を持つも
   * のが定義でき、通常の関数であってもよいし、operator()などのファンク
   * タとして定義してもよい。実際には基底クラスにてこれらの関数を純粋仮
   * 想関数として定義し、このクラスを継承して、実際のリスナクラスを実装
   * することになる。また、ひとつのリスナクラスに複数にコールバック関数
   * を定義してもよい。実際には、これらのコールバック関数を実際に呼び出
   * す方法に関しては、次のListenerHolder実装クラスにて詳しく定義するこ
   * とになる。
   * <pre>
   * class MyListenerBase
   * {
   * public:
   *   // コールバック関数1: 関数呼び出し演算子によるコールバック関数
   *   // いわゆるファンクタのようにコールバック関数を定義する例。
   *   virtual void operator()(std::string strarg) = 0; // 純粋仮想関数
   *
   *   // コールバックの関数シグニチャが多様である場合、このように単な
   *   // るメンバ関数として定義することも可能。
   *   virtual void onEvent0(const char* arg0) = 0;
   *   virtual void onEvent1(int arg0) = 0;
   *   virtual void onEvent2(double arg0) = 0;
   *   virtual void onEvent3(HogeProfile& arg0) = 0;
   * };
   * </pre>
   *
   * @section ListenerHolder実装クラス
   *
   * ListenerHolder実装クラスはこのLsitenerHolderクラステンプレートを継
   * 承して、上で定義した MyListenerBase クラスの追加と削除など管理を行
   * い、かつ実際にコールバック関数を呼び出す部分を実装することになる。
   * 実際にコールバックを呼び出す部分では、関数シグニチャが多種多様であっ
   * たり、ひとつのリスナクラスが複数のコールバック関数を持つ場合がある
   * ため、個別のリスナクラスに対応するため、この呼び出し部分が必要とな
   * る。ListenerHolder実装クラスは、MyListenerBaseクラスと同じシグニチャ
   * を持つメンバ関数をもち、関数内部では、ListenerHolderクラスが持つ、
   * m_listeners, m_mutex のこれら二つのメンバ変数を利用して、登録され
   * たリスナオブジェクトのメンバ変数を呼び出す。
   *
   * <pre>
   * class MyListenerHolderImpl
   *  : public ::RTM::util::ListenerHolder<MyListenerBase>
   * {
   * public:
   *   // 関数呼び出し演算子のコールバック関数の場合
   *   virtual void operator()(std::string strarg)
   *   {
   *     Gurad gurad(m_mutex);
   *     for (int i(0), len(m_listeners.size()); i < len; ++i)
   *     {
   *       m_listeners[i].first->operator()(strarg);
   *     }
   *   }
   *
   *   virtual void onEvent0(const char* arg0)
   *   {
   *     Gurad gurad(m_mutex);
   *     for (int i(0), len(m_listeners.size()); i < len; ++i)
   *     {
   *       m_listeners[i].first->onEvent(arg0);
   *     }
   *   }
   * };
   * </pre>
   *
   * リスナオブジェクトへのポインタを格納しているEntryオブジェクトは
   * std::pair<ListenerClass, bool> として定義されており、firstが
   * Listenerオブジェクトへのポインタ、secondが自動削除フラグである。し
   * たがって、リスナオブジェクトへアクセスする場合にはfirstを使用する。
   * マルチスレッド環境で利用することが想定される場合は、Guard
   * guard(m_mutex) によるロックを忘れずに行うこと。
   *
   * @section ListenerHolder実装クラスの利用
   * 実装されたMyListenerHolderImplは一例として以下のように利用する。
   *
   * <pre>
   * // たとえばクラスメンバとして宣言
   * MyListenerHolderImpl m_holder;
   *
   * // 登録、自動クリーンモードで登録、
   * // オブジェクトの削除はHolderクラスに任せる
   * m_holder.addListener(new MyListener0(), true); // MyListener0の
   *
   * // コールバックを呼び出す
   * m_holder.operator()(strarg);
   * m_holder.onEvent0("HogeHoge);
   * </pre>
   *
   * @else
   *
   * @class Listener holder class
   *
   * @endif
   */
  template <typename ListenerClass>
  class ListenerHolder
    : public coil::NonCopyable
  {
  public:
    typedef coil::Guard<coil::Mutex> Guard;
    typedef std::pair<ListenerClass*, bool> Entry;
    typedef std::vector<Entry> EntryList;
    typedef typename EntryList::iterator EntryIterator;

    /*!
     * @if jp
     * @brief ListenerHolderクラスコンストラクタ
     * @else
     * @brief ListenerHolder class ctor
     * @endif
     */
    ListenerHolder()
    {
    }

    /*!
     * @if jp
     * @brief ListenerHolderデストラクタ
     * @else
     * @brief ListenerHolder class dtor
     * @endif
     */
    virtual ~ListenerHolder()
    {
      Guard guard(m_mutex);

      for(auto & listener : m_listeners)
        {
          if (listener.second)
            {
              delete listener.first;
            }
        }
      m_listeners.clear();
    }

    /*!
     * @if jp
     * @brief リスナを追加する
     * @else
     * @brief add listener object
     * @endif
     */
    virtual void addListener(ListenerClass* listener,
                     bool autoclean)
    {
      Guard guard(m_mutex);
      m_listeners.push_back(Entry(listener, autoclean));
    }

    /*!
     * @if jp
     * @brief リスナを削除する
     * @else
     * @brief remove listener object
     * @endif
     */
    virtual void removeListener(ListenerClass* listener)
    {
      Guard guard(m_mutex);
      EntryIterator it(m_listeners.begin());

      for (; it != m_listeners.end(); ++it)
        {
          if ((*it).first == listener)
            {
              if ((*it).second)
                {
                  delete (*it).first;
                }
              m_listeners.erase(it);
              return;
            }
        }
    }

  protected:
    /*!
     * @if jp
     * @brief Mutex
     * @else
     * @brief Mutex
     * @endif
     */
    coil::Mutex m_mutex;

    /*!
     * @if jp
     * @brief リスナのリスト
     * @else
     * @brief List of listener object
     * @endif
     */
    EntryList m_listeners;
  };
} // namespace util
} // namespace RTM

#define LISTENERHOLDER_CALLBACK(func, args)               \
  {                                                       \
    Guard guard(m_mutex);                                 \
    for (std::vector<Entry>::iterator listener = m_listeners.begin(); listener != m_listeners.end(); ++listener) \
      {                                                   \
        (*listener).first->func args;                  \
      }                                                   \
  }

#endif  // RTM_UITL_LISTENERHOLDER_H
