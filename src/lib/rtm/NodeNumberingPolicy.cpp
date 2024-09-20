// -*- C++ -*-
/*!
* @file NodeNumberingPolicy.cpp
* @brief Object numbering policy class
* @date $Date: 2018-1-11 03:08:04 $
* @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
*
*  Copyright (C) 2018
*     Intelligent Systems Research Institute,
*     National Institute of
*         Advanced Industrial Science and Technology (AIST), Japan
*     All rights reserved.
*
*
*/

#include <rtm/NodeNumberingPolicy.h>
#include <rtm/Manager.h>
#include <coil/stringutil.h>
#include <rtm/NamingManager.h>
#include <rtm/RTObject.h>

namespace RTM
{
  //============================================================
  // NodeNumberingPolicy
  //============================================================
  NodeNumberingPolicy::NodeNumberingPolicy()
    {
      m_mgr = &RTC::Manager::instance();
    }
  /*!
   * @if jp
   * @brief オブジェクト生成時の名称作成
   * @else
   * @brief Create the name when creating objects
   * @endif
   */
  std::string NodeNumberingPolicy::onCreate(void* obj)
    {
      int num = 0;
      while (true)
        {
          std::string num_str = coil::otos<int>(num);
          RTC::RTObject_impl *rtobj = static_cast<RTC::RTObject_impl *>(obj);

          std::string name = rtobj->getTypeName() + num_str;

          if (!find(name))
            {
              return num_str;
            }
          else
            {
              num++;
            }
        }
      return  coil::otos<int>(num);
    }
  
  /*!
   * @if jp
   * @brief オブジェクト削除時の名称解放
   * @else
   * @brief Delete the name when deleting objects
   * @endif
   */
  void NodeNumberingPolicy::onDelete(void* /*obj*/)
    {
    }
  
  /*!
  * @if jp
  *
  * @brief オブジェクトの検索
  *
  * マスターマネージャ、およびスレーブマネージャに登録されたRTCを検索し、
  * 　　　　名前が一致するRTCが存在する場合はTrueを返す
  * このプロセスで起動したマネージャがマスターマネージャではなく、
  *  さらにマスターマネージャが1つも登録されていない場合はこのプロセスのマネージャから検索
  *
  * @param name 検索対象オブジェクトの名前
  *
  * @return 判定
  *
  * @else
  *
  * @brief
  *
  *
  * @param name
  *
  * @return
  *
  * @endif
  */
  bool NodeNumberingPolicy::find(std::string name)
    {
      RTC::RTCList rtcs;
      std::string rtc_name = "rtcloc://*/*/";
      rtc_name += name;

      rtcs = m_mgr->getNaming()->string_to_component(rtc_name);

      return rtcs.length() > 0;
    }
} //namespace RTM 

extern "C"
{
  void NodeNumberingPolicyInit()
  {
    ::RTM::NumberingPolicyFactory::
      instance().addFactory("node_unique",
                            ::coil::Creator< ::RTM::NumberingPolicyBase,
                                              ::RTM::NodeNumberingPolicy>,
                            ::coil::Destructor< ::RTM::NumberingPolicyBase,
                                              ::RTM::NodeNumberingPolicy>);
  }
}

