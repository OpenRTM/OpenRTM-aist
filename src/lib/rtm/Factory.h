// -*- C++ -*-
/*!
 * @file Factory.h
 * @brief RT-Component factory class
 * @date $Date: 2007-12-31 03:08:03 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2003-2008
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

#ifndef RTC_FACTORY_H
#define RTC_FACTORY_H

#include <coil/Properties.h>
#include <rtm/NumberingPolicyBase.h>
#include <rtm/NumberingPolicy.h>
#include <atomic>


namespace RTC
{
  class RTObject_impl;
  class Manager;

  using RtcNewFunc = RTObject_impl* (*)(Manager*);
  using RtcDeleteFunc = void (*)(RTObject_impl*);

  /*!
   * @if jp
   *
   * @brief RTコンポーネント生成用テンプレート関数
   *
   * RTコンポーネントのインスタンスを生成するためのテンプレート関数。
   * RTコンポーネント管理用マネージャから呼び出される。
   * 実際には各コンポーネントのコンストラクタが呼び出される。
   * \<T_New\>で生成対象RTコンポーネントの型を指定する。
   *
   * @param manager マネージャオブジェクト
   *
   * @return 生成した RTコンポーネント インスタンス
   *
   * @else
   * @brief Template function to create RT-Components
   *
   * This is the template function to create RT-Component's instances.
   * This is invoked from RT-Components manager.
   * Actually, each component's constructor is invoked.
   * Specify the type of the target RT-Components for creation by \<T_New\>.
   *
   * @param manager Manager object
   *
   * @return Created RT-Component's instances
   *
   * @endif
   */
  template <class T_New>
  RTObject_impl* Create(Manager* manager)
  {
    return new T_New(manager);
  }

  /*!
   * @if jp
   *
   * @brief RTコンポーネント破棄用関数
   *
   *
   * @param rtc 破棄対象RTコンポーネントのインスタンス
   *
   * @else
   *
   * @brief 
   *
   * @param rtc The target RT-Component's instances for destruction
   *
   * @endif
   */
  void deleteRTObject(RTObject_impl* rtc);

  /*!
   * @if jp
   *
   * @brief RTコンポーネント破棄用テンプレート関数
   *
   * RTコンポーネントのインスタンスを破棄するためのテンプレート関数。
   * \<T_Delete\>にて破棄対象RTコンポーネントの型を指定する。
   *
   * @param rtc 破棄対象RTコンポーネントのインスタンス
   *
   * @else
   *
   * @brief Template function to destroy RT-Components
   *
   * This is the template function to destroy RT-Component's instances.
   * Specify the type of the target RT-Components for destroy by \<T_Delete\>.
   *
   * @param rtc The target RT-Component's instances for destruction
   *
   * @endif
   */
  template <class T_Delete>
  void Delete(RTObject_impl* rtc)
  {
    deleteRTObject(rtc);
  }


  /*!
   * @if jp
   *
   * @class FactoryBase
   * @brief FactoryBase 基底クラス
   *
   * コンポーネントファクトリの基底クラス。
   *
   * @since 0.2.0
   *
   * @else
   *
   * @class FactoryBase
   * @brief FactoryBase base class
   *
   * This is a base class for RT-Component factory.
   *
   * @since 0.2.0
   *
   * @endif
   */
  class FactoryBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param profile コンポーネントのプロファイル
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor.
     *
     * @param profile Component profile
     *
     * @endif
     */
    explicit FactoryBase(const coil::Properties& profile);

    /*!
     * @if jp
     * @brief デストラクタ
     *
     * デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    virtual ~FactoryBase();

    /*!
     * @if jp
     *
     * @brief コンポーネントの生成
     *
     * RT-Component のインスタンスを生成するための純粋仮想関数。
     *
     * @param mgr マネージャオブジェクト
     *
     * @return 生成したコンポーネント
     *
     * @else
     *
     * @brief Create components
     *
     * Pure virtual function to create RT-Component's instances
     *
     * @param mgr Manager object
     *
     * @return Created RT-Components
     *
     * @endif
     */
    virtual RTObject_impl* create(Manager* mgr) = 0;

    /*!
     * @if jp
     *
     * @brief コンポーネントの破棄
     *
     * RT-Component のインスタンスを破棄するための純粋仮想関数。
     *
     * @param comp 破棄対象 RTコンポーネント
     *
     * @else
     *
     * @brief Destroy components
     *
     * Pure virtual function to destroy RT-Component's instances
     *
     * @param comp The target RT-Component for destruction
     *
     * @endif
     */
    virtual void destroy(RTObject_impl* comp) = 0;

    /*!
     * @if jp
     *
     * @brief コンポーネントプロファイルの取得
     *
     * コンポーネントのプロファイルを取得する
     *
     * @return コンポーネントのプロファイル
     *
     * @else
     *
     * @brief Get the component profile
     *
     * Get the component profile.
     *
     * @return The component profile
     *
     * @endif
     */
    virtual coil::Properties& profile();

    /*!
     * @if jp
     *
     * @brief 現在のインスタンス数の取得
     *
     * コンポーネントの現在のインスタンス数を取得する。
     *
     * @return コンポーネントのインスタンス数
     *
     * @else
     *
     * @brief Get the number of current instances
     *
     * Get the number of current RT-Component's instances.
     *
     * @return Number of RT-Component's instances
     *
     * @endif
     */
    virtual int number();

  protected:
    /*!
     * @if jp
     * @brief コンポーネントのプロファイル
     * @else
     * @brief Component profile
     * @endif
     */
    coil::Properties m_Profile;

    /*!
     * @if jp
     * @brief 現在のインスタンス数
     * @else
     * @brief Number of current RT-Component's instances.
     * @endif
     */
    std::atomic<int> m_Number;
  };

  /*!
   * @if jp
   * @class FactoryCXX
   * @brief FactoryCXX クラス
   *
   * C++用コンポーネントファクトリクラス。
   *
   * @since 0.2.0
   *
   *
   * @else
   *
   * @class FactoryCXX
   * @brief FactoryCXX class
   *
   * RT-Component factory class for C++.
   *
   * @since 0.2.0
   *
   * @endif
   */
  class FactoryCXX
    : public FactoryBase
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     * プロファイル、生成関数へのポインタ、破棄関数へのポインタ、
     * コンポーネント生成時の命名ポリシーを引数に取り、
     * C++ で実装されたコンポーネントのファクトリクラスを生成する。
     *
     * @param profile コンポーネントのプロファイル
     * @param new_func コンポーネントの生成関数へのポインタ
     * @param delete_func コンポーネントの破棄関数へのポインタ
     * @param policy コンポーネント生成時の命名ポリシー
     * (デフォルト値:DefaultNumberingPolicy)
     *
     * @else
     *
     * @brief Constructor.
     *
     * Constructor.
     * Create component factory class with three arguments:
     * component profile, function pointer to object create function and
     * object destroy function.
     *
     * @param profile Component profile
     * @param new_func Pointer to component create function
     * @param delete_func Pointer to component destroy function
     * @param policy The naming policy at component creation
     * (The default value:DefaultNumberingPolicy)
     *
     * @endif
     */
    FactoryCXX(const coil::Properties& profile,
               RtcNewFunc new_func,
               RtcDeleteFunc delete_func,
               RTM::NumberingPolicyBase* policy = new RTM::ProcessUniquePolicy());

    ~FactoryCXX() override
    {
      if (RTM::NumberingPolicyFactory::instance().deleteObject(m_policy)
          != coil::FactoryReturn::OK)
        {
          delete m_policy;
        }
    }

    /*!
     * @if jp
     *
     * @brief コンポーネントの生成
     *
     * RT-Component のインスタンスを生成する。
     *
     * @param mgr マネージャオブジェクト
     *
     * @return 生成したコンポーネント
     *
     * @else
     *
     * @brief Create RT-Components
     *
     * Create RT-Component's instances
     *
     * @param mgr Manager object
     *
     * @return Created RT-Components
     *
     * @endif
     */
    RTObject_impl* create(Manager* mgr) override;

    /*!
     * @if jp
     *
     * @brief コンポーネントの破棄
     *
     * RT-Component のインスタンスを破棄する。
     *
     * @param comp 破棄対象 RT-Component
     *
     * @else
     *
     * @brief Destroy RT-Components
     *
     * Destroy RT-Component's instances
     *
     * @param comp The target RT-Component for destruction
     *
     * @endif
     */
    void destroy(RTObject_impl* comp) override;

  protected:
    /*!
     * @if jp
     * @brief コンポーネントオブジェクト生成関数へのポインタ
     * @else
     * @brief The pointer to component object create function
     * @endif
     */
    RtcNewFunc m_New;

    /*!
     * @if jp
     * @brief コンポーネントオブジェクト破棄関数へのポインタ
     * @else
     * @brief The pointer to component object destroy function
     * @endif
     */
    RtcDeleteFunc m_Delete;

    /*!
     * @if jp
     * @brief コンポーネント生成時の命名ポリシー
     * @else
     * @brief The naming policy on creating the components
     * @endif
     */
    RTM::NumberingPolicyBase* m_policy;
  };
} // namespace RTC
#endif  // RTC_FACTORY_H
