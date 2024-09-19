// -*- C++ -*-
/*!
 * @file Factory.h
 * @brief RT component manager class
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

#include <rtm/Factory.h>
#include <rtm/RTObject.h>
#include <string>

namespace RTC
{


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
  void deleteRTObject(RTObject_impl* rtc)
  {
    delete rtc;
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  FactoryBase::FactoryBase(const coil::Properties& profile)
    : m_Profile(profile), m_Number(-1)
  {
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  FactoryBase::~FactoryBase() = default;

  /*!
   * @if jp
   * @brief コンポーネントプロファイルの取得
   * @else
   * @brief Get the component profile
   * @endif
   */
  coil::Properties& FactoryBase::profile()
  {
    return m_Profile;
  }

  /*!
   * @if jp
   * @brief 現在のインスタンス数の取得
   * @else
   * @brief Get the number of current instances
   * @endif
   */
  int FactoryBase::number()
  {
    return m_Number;
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  FactoryCXX::FactoryCXX(const coil::Properties& profile,
                         RtcNewFunc new_func,
                         RtcDeleteFunc delete_func,
                         RTM::NumberingPolicyBase* policy)
    : FactoryBase(profile),
      m_New(new_func),
      m_Delete(delete_func),
      m_policy(policy)
  {
    if (m_policy == nullptr)
      throw std::bad_alloc();
  }

  /*!
   * @if jp
   * @brief コンポーネントの生成
   * @else
   * @brief Create RT-Components
   * @endif
   */
  RTObject_impl* FactoryCXX::create(Manager* mgr)
  {
    try
      {
        RTObject_impl* rtobj(m_New(mgr));
        if (rtobj == nullptr) { return nullptr; }

        ++m_Number;
        rtobj->setProperties(this->profile());

        // create instance_name
        std::string instance_name(rtobj->getTypeName());
        instance_name.append(m_policy->onCreate(rtobj));
        rtobj->setInstanceName(instance_name.c_str());

        return rtobj;
      }
    catch (...)
      {
        return nullptr;
      }
  }

  /*!
   * @if jp
   * @brief コンポーネントの破棄
   * @else
   * @brief Destroy RT-Components
   * @endif
   */
  void FactoryCXX::destroy(RTObject_impl* comp)
  {
    try
      {
        --m_Number;
        m_policy->onDelete(comp);
        m_Delete(comp);
      }
    catch (...)
      {

      }
  }
} // namespace RTC
