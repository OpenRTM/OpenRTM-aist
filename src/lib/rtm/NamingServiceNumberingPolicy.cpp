// -*- C++ -*-
/*!
* @file NamingServiceNumberingPolicy.cpp
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

#include <rtm/NamingServiceNumberingPolicy.h>
#include <rtm/Manager.h>
#include <coil/stringutil.h>
#include <rtm/NamingManager.h>
#include <rtm/RTObject.h>

namespace RTM
{
  //============================================================
  // NamingServiceNumberingPolicy
  //============================================================
	NamingServiceNumberingPolicy::NamingServiceNumberingPolicy()
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
	std::string NamingServiceNumberingPolicy::onCreate(void* obj)
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
	void NamingServiceNumberingPolicy::onDelete(void* /*obj*/)
  {
  }
  
	/*!
	* @if jp
	*
	* @brief オブジェクトの検索
	*
	* 指定名のインスタンス名のRTCを検索し、
	* 　　　　一致するRTCが存在する場合はTrueを返す
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
	bool NamingServiceNumberingPolicy::find(std::string name)
  {
	  RTC::RTCList rtcs;
	  std::string rtc_name = "rtcname://*/*/";
	  rtc_name += name;

	  rtcs = m_mgr->getNaming()->string_to_component(rtc_name);

	  return rtcs.length() > 0;
  }
} //namespace RTM 

extern "C"
{
	void NamingServiceNumberingPolicyInit()
  {
    ::RTM::NumberingPolicyFactory::
      instance().addFactory("ns_unique",
                            ::coil::Creator< ::RTM::NumberingPolicyBase,
							::RTM::NamingServiceNumberingPolicy>,
                            ::coil::Destructor< ::RTM::NumberingPolicyBase,
							::RTM::NamingServiceNumberingPolicy>);
  }
};

