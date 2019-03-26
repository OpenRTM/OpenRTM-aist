// -*- C++ -*-
/*!
 * @file NumberingPolicy.cpp
 * @brief Object numbering policy class
 * @date $Date: 2007-12-31 03:08:04 $
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

#include <rtm/NumberingPolicy.h>
#include <coil/stringutil.h>

namespace RTM
{
  //============================================================
  // DefaultNumberingPolicy
  //============================================================
  /*!
   * @if jp
   * @brief オブジェクト生成時の名称作成
   * @else
   * @brief Create the name when creating objects
   * @endif
   */
  std::string ProcessUniquePolicy::onCreate(void* obj)
  {
    std::vector<void*>::size_type pos;
    
    ++m_num;
    
    try
      {
        pos = find(nullptr);
        m_objects[pos] = obj;
        return coil::otos(pos);
      }
    catch (ObjectNotFound& e)
      {
        (void)(e);
        m_objects.push_back(obj);
        return coil::otos(static_cast<int>(m_objects.size() - 1));
      }
  }

  /*!
   * @if jp
   * @brief オブジェクト削除時の名称解放
   * @else
   * @brief Delete the name when deleting objects
   * @endif
   */
  void ProcessUniquePolicy::onDelete(void* obj)
  {
    std::vector<void*>::size_type pos;
    pos = find(obj);
    if (pos < m_objects.size())
      {
        m_objects[pos] = nullptr;
      }
    --m_num;
  }

  /*!
   * @if jp
   * @brief オブジェクトの検索
   * @else
   * @brief Find the object
   * @endif
   */
  long int ProcessUniquePolicy::find(void* obj)
  {
    std::vector<void*>::size_type len(m_objects.size());
    std::vector<void*>::size_type i(0);
    for (i = 0; i < len; ++i)
      {
        if (m_objects[i] == obj) return static_cast<int>(i);
      }
    throw ObjectNotFound();
  }
} //namespace RTM 

extern "C"
{
  void ProcessUniquePolicyInit()
  {
    ::RTM::NumberingPolicyFactory::
      instance().addFactory("default",
                            ::coil::Creator< ::RTM::NumberingPolicyBase,
                                             ::RTM::ProcessUniquePolicy>,
                            ::coil::Destructor< ::RTM::NumberingPolicyBase,
                                                ::RTM::ProcessUniquePolicy>);
    ::RTM::NumberingPolicyFactory::
      instance().addFactory("process_unique",
                            ::coil::Creator< ::RTM::NumberingPolicyBase,
                                             ::RTM::ProcessUniquePolicy>,
                            ::coil::Destructor< ::RTM::NumberingPolicyBase,
                                                ::RTM::ProcessUniquePolicy>);
  }
}

