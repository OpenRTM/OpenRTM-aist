// -*- C++ -*-
/*!
 * @file CORBA_RTCUtil.h
 * @brief CORBA RTC utility
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2015
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */



#include <rtm/CORBA_RTCUtil.h>
#include <rtm/NamingManager.h>

#include <utility>

namespace CORBA_RTCUtil
{
  /*!
   * @if jp
   * @brief コンポーネントのプロパティ取得
   * @param rtc RTコンポーネント
   * @return コンポーネントのプロパティ
   * @else
   * @brief
   * @param rtc
   * @return
   * @endif
   */
  coil::Properties get_component_profile(const RTC::RTObject_ptr rtc)
  {
    coil::Properties prop;
    if (CORBA::is_nil(rtc)) { return prop; }
    RTC::ComponentProfile_var prof;
    prof = rtc->get_component_profile();
    NVUtil::copyToProperties(prop, prof->properties);
    return prop;
  }
  /*!
   * @if jp
   * @brief コンポーネントのオブジェクトリファレンスが存在しているかを判定
   * @param rtc RTコンポーネント
   * @return True:生存、False:終了済み
   * @else
   * @brief
   * @param rtc
   * @return
   * @endif
   */
  bool is_existing(const RTC::RTObject_ptr rtc)
  {
    try
      {
        if (rtc->_non_existent())
          {
            return false;
          }
        return true;
      }
    catch (...)
      {
        return false;
      }
  }
  /*!
   * @if jp
   * @brief RTCがデフォルトの実行コンテキストでalive状態かを判定する
   * @param rtc RTコンポーネント
   * @return True:alive状態
   * @else
   * @brief
   * @param rtc
   * @return
   * @endif
   */
  bool is_alive_in_default_ec(const RTC::RTObject_ptr rtc)
  {
    RTC::ExecutionContext_var ec = get_actual_ec(rtc);
    if (CORBA::is_nil(ec))
      {
        return false;
      }
    return rtc->is_alive(ec);
  }
  /*!
   * @if jp
   * @brief RTコンポーネントに関連付けしたECから指定したIDのECを取得
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return 実行コンテキストのオブジェクトリファレンス
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  RTC::ExecutionContext_var get_actual_ec(const RTC::RTObject_ptr rtc,
                                          RTC::UniqueId ec_id)
  {
    if (CORBA::is_nil(rtc)) { return RTC::ExecutionContext::_nil(); }
    if (ec_id < 1000) // owned EC
      {
        RTC::ExecutionContextList_var eclist;
        eclist = rtc->get_owned_contexts();
        if (ec_id >= (CORBA::Long)eclist->length())
          { return RTC::ExecutionContext::_nil(); }
        if (CORBA::is_nil(eclist[(CORBA::ULong)ec_id]))
          { return RTC::ExecutionContext::_nil(); }
#ifdef ORB_IS_TAO
        return eclist[(CORBA::ULong)ec_id].in();
#else
        return eclist[(CORBA::ULong)ec_id];
#endif
      }
    if (ec_id >= 1000)
      {
        RTC::UniqueId pec_id(ec_id - 1000);
        RTC::ExecutionContextList_var eclist;
        eclist = rtc->get_participating_contexts();
        if (pec_id >= (CORBA::Long)eclist->length())
          { return RTC::ExecutionContext::_nil(); }
		if (CORBA::is_nil(eclist[(CORBA::ULong)pec_id]))
          { return RTC::ExecutionContext::_nil(); }
#ifdef ORB_IS_TAO
        return eclist[(CORBA::ULong)pec_id].in();
#else
        return eclist[(CORBA::ULong)pec_id];
#endif
      }
    return RTC::ExecutionContext::_nil();
  }
  /*!
   * @if jp
   * @brief 対象のRTコンポーネントから指定した実行コンテキストのIDを取得する
   * @param rtc RTコンポーネント
   * @param ec 実行コンテキスト
   * @return 実行コンテキストのID
   * 指定した実行コンテキストがRTコンポーネントに関連付けられていなかっ
   * た場合は-1を返す
   * @else
   * @brief
   * @param rtc
   * @param ec
   * @return
   * @endif
   */
  RTC::UniqueId get_ec_id(const RTC::RTObject_ptr rtc, const RTC::ExecutionContext_ptr ec)
  {
    if (CORBA::is_nil(rtc))
      {
        return -1;
      }
    if (CORBA::is_nil(ec))
      {
        return -1;
      }

    RTC::ExecutionContextList eclist_own = (*rtc->get_owned_contexts());

    for (unsigned int i = 0; i < eclist_own.length(); i++)
      {
        if (!CORBA::is_nil(eclist_own[i]))
          {
            if (eclist_own[i]->_is_equivalent(ec))
              {
                return i;
              }
          }
      }

    RTC::ExecutionContextList  eclist_pec = (*rtc->get_participating_contexts());

    for (unsigned int i = 0; i < eclist_pec.length(); i++)
      {
        if (!CORBA::is_nil(eclist_pec[i]))
          {
            if (eclist_pec[i]->_is_equivalent(ec))
              {
                return i + ECOTHER_OFFSET;
              }
          }
      }
    return -1;
  }
  /*!
   * @if jp
   * @brief RTCを指定した実行コンテキストでアクティベーションする
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return RTC、ECのオブジェクトリファレンスがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はactivate_component関数の戻り値を返す。RTC_OKの場合はアクティベーションが成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return
   * @endif
   */
  RTC::ReturnCode_t activate(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    if (CORBA::is_nil(rtc)) { return RTC::BAD_PARAMETER; }
    RTC::ExecutionContext_var ec = get_actual_ec(rtc, ec_id);
    if (CORBA::is_nil(ec)) { return RTC::BAD_PARAMETER; }
    return ec->activate_component(rtc);
  }
  /*!
   * @if jp
   * @brief  RTCを指定した実行コンテキストで非アクティベーションする
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return RTC、ECのオブジェクトリファレンスがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はdeactivate_component関数の戻り値を返す。RTC_OKの場合は非アクティベーションが成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return
   * @endif
   */
  RTC::ReturnCode_t deactivate(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    if (CORBA::is_nil(rtc)) { return RTC::BAD_PARAMETER; }
    RTC::ExecutionContext_var ec = get_actual_ec(rtc, ec_id);
    if (CORBA::is_nil(ec)) { return RTC::BAD_PARAMETER; }
    return ec->deactivate_component(rtc);
  }
  /*!
   * @if jp
   * @brief  RTCを指定した実行コンテキストでリセットする
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return RTC、ECのオブジェクトリファレンスがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はreset_component関数の戻り値を返す。RTC_OKの場合はリセットが成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return
   * @endif
   */
  RTC::ReturnCode_t reset(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    if (CORBA::is_nil(rtc)) { return RTC::BAD_PARAMETER; }
    RTC::ExecutionContext_var ec = get_actual_ec(rtc, ec_id);
    if (CORBA::is_nil(ec)) { return RTC::BAD_PARAMETER; }
    return ec->reset_component(rtc);
  }
  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定した実行コンテキストでの状態を取得
   * @param state RTCの状態
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return  rtc、ecがnilの場合はFalseを返す。
   * nilではない場合はstateに状態を代入してTrueを返す。
   * @else
   * @brief
   * @param state
   * @param rtc
   * @param ec_id 
   * @return 
   * @endif
   */
  bool get_state(RTC::LifeCycleState  &state, const RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    if (CORBA::is_nil(rtc))
      {
        return false;
      }
    RTC::ExecutionContext_var ec = get_actual_ec(rtc, ec_id);
    if (CORBA::is_nil(ec))
      {
        return false;
      }
    state = ec->get_component_state(rtc);

    return true;
  }
  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定した実行コンテキストでINACTIVE状態かどうか判定
   * @param rtc 対象のRTコンポーネント
   * @param ec_id  実行コンテキストのID
   * @return  INACTIVE状態の時はTrue、それ以外はFalse
   * rtc、ecがnilの場合もFalseを返す
   * @else
   * @brief
   * @param state
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  bool is_in_inactive(const RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    RTC::LifeCycleState state = RTC::CREATED_STATE;
    if (get_state(state, rtc, ec_id))
      {
        if (state == RTC::INACTIVE_STATE)
          {
            return true;
          }
      }
    return false;
  }
  
  
  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定した実行コンテキストでACTIVE状態かどうか判定
   * @param rtc 対象のRTコンポーネント
   * @param ec_id  実行コンテキストのID
   * @return  ACTIVE状態の時はTrue、それ以外はFalse
   * rtc、ecがnilの場合もFalseを返す
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  bool is_in_active(const RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    RTC::LifeCycleState state = RTC::CREATED_STATE;
    if (get_state(state, rtc, ec_id))
      {
        if (state == RTC::ACTIVE_STATE)
          {
            return true;
          }
      }
    return false;
  }
  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定した実行コンテキストでERROR状態かどうか判定
   * @param rtc 対象のRTコンポーネント
   * @param ec_id  実行コンテキストのID
   * @return  ERROR状態の時はTrue、それ以外はFalse
   * rtc、ecがnilの場合もFalseを返す
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  bool is_in_error(const RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    RTC::LifeCycleState state = RTC::CREATED_STATE;
    if (get_state(state, rtc, ec_id))
      {
        if (state == RTC::ERROR_STATE)
          {
            return true;
          }
      }
    return false;
  }
  /*!
   * @if jp
   * @brief RTCのデフォルトの実行コンテキストの実行周期を取得する
   * @param rtc RTコンポーネント
   * @return  実行周期
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  CORBA::Double get_default_rate(const RTC::RTObject_ptr rtc)
  {
    RTC::ExecutionContext_var ec = get_actual_ec(rtc);
    return ec->get_rate();
  }
  /*!
   * @if jp
   * @brief RTCのデフォルトの実行コンテキストの実行周期を設定する
   * @param rtc RTコンポーネント
   * @param rate 実行周期
   * @return set_rate関数の戻り値を返す。
   * RTC_OKで設定が成功
   * @else
   * @brief
   * @param rtc
   * @param rate 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t set_default_rate(RTC::RTObject_ptr rtc, const CORBA::Double rate)
  {
    RTC::ExecutionContext_var ec = get_actual_ec(rtc);
    return ec->set_rate(rate);
  }
  /*!
   * @if jp
   * @brief RTCの指定IDの実行コンテキストの周期を取得
   * @param rtc RTコンポーネント
   * @param ec_id 指定の実行コンテキストのID
   * @return 実行周期
   * @else
   * @brief
   * @param rtc
   * @param ec_id 
   * @return 
   * @endif
   */
  CORBA::Double get_current_rate(const RTC::RTObject_ptr rtc, RTC::UniqueId ec_id)
  {
    RTC::ExecutionContext_var ec = get_actual_ec(rtc, ec_id);
    return ec->get_rate();
  }
  /*!
   * @if jp
   * @brief RTCの指定IDの実行コンテキストの周期を設定
   * @param rtc RTコンポーネント
   * @param ec_id 指定の実行コンテキストのID
   * @param rate 実行周期
   * @return set_rate関数の戻り値を返す。
   * RTC_OKで設定が成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id 
   * @param rate
   * @return 
   * @endif
   */
  RTC::ReturnCode_t set_current_rate(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id, const CORBA::Double rate)
  {
    RTC::ExecutionContext_var ec = get_actual_ec(rtc, ec_id);
    return ec->set_rate(rate);
  }
  /*!
   * @if jp
   * @brief 対象のRTCのデフォルトの実行コンテキストに指定のRTCを関連付ける
   * @param localcomp 対象のRTコンポーネント
   * @param othercomp 実行コンテキストに関連付けるRTコンポーネント
   * @return ecの取得に失敗した場合はBAD_PARAMETERを返す
   * そうでない場合はaddComponent関数の戻り値を返す。RTC_OKで接続成功。
   * @else
   * @brief
   * @param localcomp
   * @param othercomp 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t add_rtc_to_default_ec(const RTC::RTObject_ptr localcomp, const RTC::RTObject_ptr othercomp)
  {
    if (CORBA::is_nil(othercomp))
      {
        return RTC::BAD_PARAMETER;
      }
    RTC::ExecutionContext_var ec = get_actual_ec(localcomp);
    if (CORBA::is_nil(ec))
      {
        return RTC::BAD_PARAMETER;
      }
    return ec->add_component(othercomp);
  }
  /*!
   * @if jp
   * @brief 対象のRTCのデフォルトの実行コンテキストの指定のRTCへの関連付けを解除する
   * @param localcomp 対象のRTコンポーネント
   * @param othercomp 実行コンテキストとの関連付けを解除するRTコンポーネント
   * @return ecの取得に失敗した場合はBAD_PARAMETERを返す
   * そうでない場合はremoveComponent関数の戻り値を返す。RTC_OKで解除成功。
   * @else
   * @brief
   * @param localcomp
   * @param othercomp 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t remove_rtc_to_default_ec(const RTC::RTObject_ptr localcomp, const RTC::RTObject_ptr othercomp)
  {
    {
      if (CORBA::is_nil(othercomp))
        {
          return RTC::BAD_PARAMETER;
        }
      RTC::ExecutionContext_var ec = get_actual_ec(localcomp);
      if (CORBA::is_nil(ec))
        {
          return RTC::BAD_PARAMETER;
        }
      return ec->remove_component(othercomp);
    }
  }
  /*!
   * @if jp
   * @brief RTCのデフォルトの実行コンテキストに参加しているRTCのリストを取得する
   * 実行コンテキストがnilの場合は空のリストを返す
   * @param rtc 対象のRTコンポーネント
   * @return RTCのリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  const RTC::RTCList get_participants_rtc(const RTC::RTObject_ptr rtc)
  {
    RTC::ExecutionContext_var ec = get_actual_ec(rtc);
    if (CORBA::is_nil(ec))
      {
        return RTC::RTCList();
      }
    try
      {
        RTC::ExecutionContextService_ptr ecservice = RTC::ExecutionContextService::_narrow(ec);
        RTC::ExecutionContextProfile* profile = ecservice->get_profile();
        return profile->participants;
      }
    catch (...)
      {
        return RTC::RTCList();
      }
  }
  /*!
   * @if jp
   * @brief 指定したRTCの保持するポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_port_names(const RTC::RTObject_ptr rtc)
  {
    coil::vstring names;
    if (CORBA::is_nil(rtc))
      {
        return names;
      }
    RTC::PortServiceList ports = (*rtc->get_ports());

    for (unsigned int i = 0; i < ports.length(); i++)
      {
        RTC::PortProfile* pp = ports[i]->get_port_profile();
#ifdef ORB_IS_TAO
		std::string s(pp->name);
#else
        std::string s(static_cast<char*>(pp->name));
#endif
        names.push_back(s);
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 指定したRTCの保持するインポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_inport_names(const RTC::RTObject_ptr rtc)
  {
    coil::vstring names;
    if (CORBA::is_nil(rtc))
      {
        return names;
      }
    RTC::PortServiceList ports = (*rtc->get_ports());

    for (unsigned int i = 0; i < ports.length(); i++)
      {
        RTC::PortProfile* pp = ports[i]->get_port_profile();
        coil::Properties prop;
        NVUtil::copyToProperties(prop, pp->properties);

        if (prop["port.port_type"] == "DataInPort")
          {
#ifdef ORB_IS_TAO
			std::string s(pp->name);
#else
            std::string s(static_cast<char*>(pp->name));
#endif
            names.push_back(s);
          }
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 指定したRTCの保持するアウトポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_outport_names(const RTC::RTObject_ptr rtc)
  {
    coil::vstring names;
    if (CORBA::is_nil(rtc))
      {
        return names;
      }
    RTC::PortServiceList ports = (*rtc->get_ports());

    for (unsigned int i = 0; i < ports.length(); i++)
      {
        RTC::PortProfile* pp = ports[i]->get_port_profile();
        coil::Properties prop;
        NVUtil::copyToProperties(prop, pp->properties);

        if (prop["port.port_type"] == "DataOutPort")
          {
#ifdef ORB_IS_TAO
			std::string s(pp->name);
#else
            std::string s(static_cast<char*>(pp->name));
#endif
            names.push_back(s);
          }
      }
    return names;
  }


  /*!
   * @if jp
   * @brief 指定したRTCの保持するサービスポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return
   * @endif
   */
  coil::vstring get_svcport_names(const RTC::RTObject_ptr rtc)
  {
    coil::vstring names;
    if (CORBA::is_nil(rtc))
      {
        return names;
      }
    RTC::PortServiceList ports = (*rtc->get_ports());

    for (unsigned int i = 0; i < ports.length(); i++)
      {
        RTC::PortProfile* pp = ports[i]->get_port_profile();
        coil::Properties prop;
        NVUtil::copyToProperties(prop, pp->properties);

        if (prop["port.port_type"] == "CorbaPort")
          {
#ifdef ORB_IS_TAO
			std::string s(pp->name);
#else
            std::string s(static_cast<char*>(pp->name));
#endif
            names.push_back(s);
          }
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 指定したポートの保持しているコネクタの名前のリストを取得
   * @param port 対象のポート
   * @return コネクタ名のリスト
   * @else
   * @brief
   * @param port 
   * @return 
   * @endif
   */
  coil::vstring get_connector_names(const RTC::PortService_ptr port)
  {
    coil::vstring names;
    if (CORBA::is_nil(port))
      {
        return names;
      }
    RTC::ConnectorProfileList conprof = (*port->get_connector_profiles());
    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        names.push_back(std::string(conprof[i].name));
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 対象のRTCの指定したポートのコネクタの名前のリストを取得
   * @param rtc 対象のRTコンポーネント
   * @param port_name ポート名
   * @return コネクタ名のリスト
   * @else
   * @brief
   * @param rtc
   * @param port_name 
   * @return 
   * @endif
   */
  coil::vstring get_connector_names(const RTC::RTObject_ptr rtc, const std::string& port_name)
  {
    coil::vstring names;
    RTC::PortService_var port = get_port_by_name(rtc, port_name);
    if (CORBA::is_nil(port))
      {
        return names;
      }
    RTC::ConnectorProfileList conprof = (*port->get_connector_profiles());
    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        names.push_back(std::string(conprof[i].name));
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 指定したポートの保持しているコネクタのIDのリストを取得
   * @param port 対象のポート
   * @return コネクタのIDのリスト
   * @else
   * @brief
   * @param port 
   * @return 
   * @endif
   */
  coil::vstring get_connector_ids(const RTC::PortService_ptr port)
  {
    coil::vstring names;
    if (CORBA::is_nil(port))
      {
        return names;
      }
    RTC::ConnectorProfileList conprof = (*port->get_connector_profiles());
    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        names.push_back(std::string(conprof[i].connector_id));
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 対象のRTCの指定したポートのコネクタのIDのリストを取得
   * @param port RTコンポーネント
   * @param port_name ポート名
   * @return コネクタのIDのリスト
   * @else
   * @brief
   * @param port
   * @param port_name  
   * @return 
   * @endif
   */
  coil::vstring get_connector_ids(const RTC::RTObject_ptr rtc, const std::string& port_name)
  {
    coil::vstring names;
    RTC::PortService_var port = get_port_by_name(rtc, port_name);
    if (CORBA::is_nil(port))
      {
        return names;
      }
    RTC::ConnectorProfileList conprof = (*port->get_connector_profiles());
    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        names.push_back(std::string(conprof[i].connector_id));
      }
    return names;
  }
  /*!
   * @if jp
   * @brief 指定したポートを接続するためのコネクタプロファイルを取得
   * @param name コネクタ名
   * @param prop_arg 設定
   * @param port0 対象のポート1
   * @param port1 対象のポート2
   * @return コネクタプロファイル
   * @else
   * @brief
   * @param name
   * @param prop_arg 
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  RTC::ConnectorProfile_var create_connector(const std::string& name,
                                             const coil::Properties& prop_arg,
                                             const RTC::PortService_ptr port0,
                                             const RTC::PortService_ptr port1)
  {
    coil::Properties prop = prop_arg;
    RTC::ConnectorProfile_var conn_prof = new RTC::ConnectorProfile();
    std::string prof_name;
    conn_prof->name = CORBA::string_dup(name.c_str());
    conn_prof->connector_id = CORBA::string_dup("");
    if (CORBA::is_nil(port1))
    {
        conn_prof->ports.length(1);
        conn_prof->ports[0] = RTC::PortService::_duplicate(port0);
    }
    else
    {
        conn_prof->ports.length(2);
        conn_prof->ports[0] = RTC::PortService::_duplicate(port0);
        conn_prof->ports[1] = RTC::PortService::_duplicate(port1);
    }
    
    if (prop["dataport.dataflow_type"].empty())
      {
        prop["dataport.dataflow_type"] = "push";
      }
    if (prop["dataport.interface_type"].empty())
      {
        prop["dataport.interface_type"] ="corba_cdr";
      }
    NVUtil::copyFromProperties(conn_prof->properties, prop);
    return conn_prof._retn();
  }
  /*!
   * @if jp
   * @brief 指定したポート同士が接続されているかを判定
   * @param port0 対象のポート1
   * @param port1 対象のポート2
   * @return True: 接続済み、False: 未接続
   * @else
   * @brief
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  bool already_connected(const RTC::PortService_ptr localport,
                         const RTC::PortService_ptr otherport)
  {
    RTC::ConnectorProfileList_var conprof;
    conprof = localport->get_connector_profiles();
    for (CORBA::ULong i(0); i < conprof->length(); ++i)
      {
        for (CORBA::ULong j(0); j < conprof[i].ports.length(); ++j)
          {
            if (conprof[i].ports[j]->_is_equivalent(otherport))
              {
                return true;
              }
          }
      }
    return false;
  }
  /*!
   * @if jp
   * @brief 指定したポートを接続する
   * @param name コネクタ名
   * @param prop 設定
   * @param port0 対象のポート1
   * @param port1 対象のポート2
   * @return ポートのオブジェクトリファレンスがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はport0.connect関数の戻り値を返す。RTC_OKの場合は接続が成功
   * @else
   * @brief
   * @param name 
   * @param prop 
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t connect(const std::string& name,
                            const coil::Properties& prop,
                            const RTC::PortService_ptr port0,
                            const RTC::PortService_ptr port1)
  {
    if (CORBA::is_nil(port0)) { return RTC::BAD_PARAMETER; }
    if (!CORBA::is_nil(port1)) {
        if (port0->_is_equivalent(port1)) { return RTC::BAD_PARAMETER; }
    }
    
    RTC::ConnectorProfile_var cprof;
    cprof = create_connector(name, prop, port0, port1);
    return port0->connect(cprof);
  }
  /*!
   * @if jp
   * @brief 対象のRTCの指定したポートのコネクタのIDのリストを取得
   * @param port RTコンポーネント
   * @param port_name ポート名
   * @return コネクタのIDのリスト
   * @else
   * @brief
   * @param port
   * @param port_name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect(RTC::ConnectorProfile connector_prof)
  {
    RTC::PortServiceList ports = connector_prof.ports;
    return disconnect_connector_id(ports[0], std::string(connector_prof.connector_id));
  }
  /*!
   * @if jp
   * @brief 対象のポートで指定した名前のコネクタを切断
   * @param port_ref 対象のポート
   * @param conn_name コネクタ名
   * @return portがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port_ref
   * @param conn_name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_name(const RTC::PortService_ptr port_ref, const std::string& conn_name)
  {
    if (CORBA::is_nil(port_ref))
      {
        return RTC::BAD_PARAMETER;
      }
    RTC::ConnectorProfileList conprof = (*port_ref->get_connector_profiles());
    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        if (std::string(conprof[i].name) == conn_name)
          {
            return disconnect(conprof[i]);
          }
      }
    return RTC::BAD_PARAMETER;
  }
  /*!
   * @if jp
   * @brief 対象の名前のポートで指定した名前のコネクタを切断
   * @param port_name 対象のポート名
   * @param conn_name コネクタ名
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port_name
   * @param conn_name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_name(const std::string& port_name, const std::string& conn_name)
  {
    RTC::PortService_var port_ref = get_port_by_url(port_name);
    if (CORBA::is_nil(port_ref))
      {
        return RTC::BAD_PARAMETER;
      }

    RTC::ConnectorProfileList conprof = (*port_ref->get_connector_profiles());
    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        if (std::string(conprof[i].name) == conn_name)
          {
            return disconnect(conprof[i]);
          }
      }
    return RTC::BAD_PARAMETER;
  }
  /*!
   * @if jp
   * @brief 対象のポートで指定したIDのコネクタを切断
   * @param port 対象のポート
   * @param name コネクタID
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port
   * @param name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_id(const RTC::PortService_ptr port_ref, const std::string& conn_id)
  {
    if (CORBA::is_nil(port_ref))
      {
        return RTC::BAD_PARAMETER;
      }
    return port_ref->disconnect(conn_id.c_str());
  }
  /*!
   * @if jp
   * @brief 対象の名前のポートで指定したIDのコネクタを切断
   * @param port 対象のポート
   * @param name コネクタID
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port
   * @param name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_id(const std::string& port_name, const std::string& conn_id)
  {
    RTC::PortService_var port_ref = get_port_by_url(port_name);
    if (CORBA::is_nil(port_ref))
      {
        return RTC::BAD_PARAMETER;
      }
    return port_ref->disconnect(conn_id.c_str());
  }
  /*!
   * @if jp
   * @brief 対象のポートのコネクタを全て切断
   * @param port_ref ポートのオブジェクトリファレンス
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * 切断できた場合はRTC_OKを返す
   * @else
   * @brief
   * @param port_ref  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_all(const RTC::PortService_ptr port_ref)
  {
    if (CORBA::is_nil(port_ref))
      {
        return RTC::BAD_PARAMETER;
      }
    return port_ref->disconnect_all();
  }
  /*!
   * @if jp
   * @brief 指定ポート名のポートのコネクタを全て切断
   * @param port_name ポート名
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * 切断できた場合はRTC_OKを返す
   * @else
   * @brief
   * @param port_ref  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_all(const std::string& port_name)
  {
    RTC::PortService_var port_ref = get_port_by_url(port_name);
    if (CORBA::is_nil(port_ref))
      {
        return RTC::BAD_PARAMETER;
      }
    return port_ref->disconnect_all();
  }
  /*!
   * @if jp
   * @brief 指定した名前のポートを取得
   * @param port_name ポート名
   * @return ポートのオブジェクトリファレンス
   * portが存在しない場合はnilを返す
   * @else
   * @brief
   * @param port_ref
   * @return
   * @endif
   */
  RTC::PortService_var get_port_by_url(const std::string& port_name)
  {
    RTC::Manager& mgr = RTC::Manager::instance();
    RTC::NamingManager* nm = mgr.getNaming();
    coil::vstring p = coil::split(port_name, ".");
    if (p.size() < 2)
      {
        return RTC::PortService::_nil();
      }
    std::string rtc_name = port_name;
    coil::replaceString(rtc_name, "." + p.back(), "");
    RTC::RTCList rtcs = nm->string_to_component(rtc_name);

    if (rtcs.length() < 1)
      {
        return RTC::PortService::_nil();
      }
    coil::vstring pn = coil::split(port_name, "/");

    return get_port_by_name(rtcs[0], pn.back());
  }
  /*!
   * @if jp
   * @brief 対象ポートと接続しているポートで指定したポート名と一致した場合に切断
   * @param localport 対象のポート
   * @param othername 接続しているポート名
   * @return ポートがnilの場合、localportの名前とothernameが一致する場合、接続しているポートの名前でothernameと一致するものがない場合にBAD_PARAMETERを返す
   * 上記の条件に当てはまらない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param localport 
   * @param othername 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_name(const RTC::PortService_ptr localport, const std::string& othername)
  {
    if (CORBA::is_nil(localport))
      {
        return RTC::BAD_PARAMETER;
      }
    RTC::PortProfile* prof = localport->get_port_profile();

    if (std::string(prof->name) == othername)
      {
        return RTC::BAD_PARAMETER;
      }

    RTC::ConnectorProfileList conprof = (*localport->get_connector_profiles());

    for (unsigned int i = 0; i < conprof.length(); i++)
      {
        for (unsigned int j = 0; j < conprof[i].ports.length(); j++)
          {
            if (!CORBA::is_nil(conprof[i].ports[j]))
              {
                RTC::PortProfile* pp = conprof[i].ports[j]->get_port_profile();
                if (std::string(pp->name) == othername)
                  {
                    return disconnect(conprof[i]);
                  }
              }
          }
      }
    return RTC::BAD_PARAMETER;
  }
  /*!
   * @if jp
   * @brief 指定したポートと指定したリスト内のポート全てと接続する
   * @param name コネクタ名
   * @param prop 設定
   * @param port 対象のポート
   * @param target_ports 対象のポートのリスト
   * @return 全ての接続が成功した場合はRTC_OKを返す。
   * connect関数がRTC_OK以外を返した場合はBAD_PARAMETERを返す。
   * @else
   * @brief
   * @param name 
   * @param prop 
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t connect_multi(const std::string& name,
                                  const coil::Properties& prop,
                                  const RTC::PortService_ptr port,
                                  RTC::PortServiceList_var& target_ports)
  {
    RTC::ReturnCode_t ret(RTC::RTC_OK);
    for (CORBA::ULong i(0); i < target_ports->length(); ++i)
      {
        if (target_ports[i]->_is_equivalent(port)) { continue; }
        if (CORBA_RTCUtil::already_connected(port, target_ports[i]))
          { continue; }
        if (RTC::RTC_OK !=
            CORBA_RTCUtil::connect(name, prop, port, target_ports[i]))
          {
            ret = RTC::RTC_ERROR;
          }
      }
    return ret;
  }
  /*!
   * @if jp
   * @brief 対象のポートの名前と指定したポート名が一致するか判定
   * @param p 対象のポート
   * @return True: 名前が一致、False:　名前が不一致
   * @else
   * @brief
   * @param p
   * @return
   * @endif
   */
  bool find_port::operator()(const RTC::PortService_var& p)
  {
    RTC::PortProfile_var prof = p->get_port_profile();
    std::string c(CORBA::string_dup(prof->name));
    return (m_name == c) ? true : false;
  }
  /*!
   * @if jp
   * @brief 対象のRTCから指定した名前のポートを取得
   * @param rtc RTコンポーネント
   * @param port_name ポート名
   * @return ポート
   * @else
   * @brief
   * @param rtc
   * @param port_name
   * @return
   * @endif
   */
  RTC::PortService_var get_port_by_name(const RTC::RTObject_ptr rtc,
                                        const std::string& name)
  {
    RTC::PortServiceList_var ports = rtc->get_ports();
    RTC::PortService_var port_var;
    for (size_t p(0); p < ports->length(); ++p)
      {
        RTC::PortProfile_var pp = ports[p]->get_port_profile();
        std::string s(CORBA::string_dup(pp->name));
#ifdef ORB_IS_TAO
        if (name == s) { return ports[p].in(); }
#else
        if (name == s) { return ports[p]; }
#endif
      }
    return RTC::PortService::_nil();
  }
  /*!
   * @if jp
   * @brief 対象のRTCの指定した名前のポートを接続する
   * @param name コネクタ名
   * @param prop 設定
   * @param rtc0 対象のRTCコンポーネント1
   * @param portName0 対象のポート名1
   * @param rtc1 対象のRTCコンポーネント2
   * @param portName1 対象のRTCコンポーネント2
   * @return RTC、ポートがnilの場合はBAD_PARAMETERを返す。
   * nilではない場合はport0.connect関数の戻り値を返す。RTC_OKの場合は接続が成功
   * @else
   * @brief
   * @param name 
   * @param prop 
   * @param rtc0 
   * @param portName0 
   * @param rtc1 
   * @param portName1 
   * @return
   * @endif
   */
  RTC::ReturnCode_t connect_by_name(std::string name, const coil::Properties& prop,
                                    RTC::RTObject_ptr rtc0,
                                    const std::string& portName0,
                                    RTC::RTObject_ptr rtc1,
                                    const std::string& portName1)
  {
    if (CORBA::is_nil(rtc0)) { return RTC::BAD_PARAMETER; }
    if (CORBA::is_nil(rtc1)) { return RTC::BAD_PARAMETER; }

    // Port0
    RTC::PortService_var port0 = get_port_by_name(rtc0, portName0);
    if (CORBA::is_nil(port0)) { return RTC::BAD_PARAMETER; }
    RTC::PortServiceList_var ports0 = rtc0->get_ports();

    // Port1
    RTC::PortService_var port1 = get_port_by_name(rtc1, portName1);
    if (CORBA::is_nil(port1)) { return RTC::BAD_PARAMETER; }

    // Connect
    return connect(std::move(name), prop, port0, port1);
  }
  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定した名前のコンフィギュレーションセットをkey-valueで取得
   * @param rtc 対象のRTコンポーネント
   * @param conf_name コンフィギュレーションセット名
   * @return コンフィギュレーションセット
   * @else
   * @brief
   * @param rtc 
   * @param conf_name 
   * @return 
   * @endif
   */
  coil::Properties get_configuration(const RTC::RTObject_ptr rtc, const std::string& conf_name)
  {
    SDOPackage::Configuration_ptr conf = rtc->get_configuration();
    SDOPackage::ConfigurationSet* confset = conf->get_configuration_set(conf_name.c_str());
    SDOPackage::NVList confData = confset->configuration_data;

    coil::Properties prop;
    NVUtil::copyToProperties(prop, confData);
    return prop;
  }
  /*!
   * @if jp
   * @brief 指定したコンフィギュレーションセット名、パラメータ名のコンフィギュレーションパラメータを取得
   * @param rtc RTコンポーネント
   * @param confset_name コンフィギュレーションセット名
   * @param value_name パラメータ名
   * @return パラメータ
   * @else
   * @brief
   * @param rtc 
   * @param conf_name 
   * @param value_name 
   * @return 
   * @endif
   */
  std::string get_parameter_by_key(const RTC::RTObject_ptr rtc, const std::string& confset_name, const std::string& value_name)
  {
    SDOPackage::Configuration_ptr conf = rtc->get_configuration();
    SDOPackage::ConfigurationSet* confset = conf->get_configuration_set(confset_name.c_str());
    SDOPackage::NVList confData = confset->configuration_data;

    coil::Properties prop;
    NVUtil::copyToProperties(prop, confData);
    return prop[value_name];
  }
  /*!
   * @if jp
   * @brief 対象のRTCのアクティブなコンフィギュレーションセット名を取得する
   * @param rtc RTコンポーネント
   * @return コンフィギュレーションセット名
   * コンフィギュレーションの取得に失敗した場合は空の文字列を返す
   * @else
   * @brief
   * @param rtc 
   * @return 
   * @endif
   */
  std::string get_active_configuration_name(const RTC::RTObject_ptr rtc)
  {
    SDOPackage::Configuration_ptr conf = rtc->get_configuration();
    SDOPackage::ConfigurationSet* confset = conf->get_active_configuration_set();
#ifdef ORB_IS_TAO
	return std::string(confset->id);
#else
    return static_cast<char*>(confset->id);
#endif
  }
  /*!
   * @if jp
   * @brief アクティブなコンフィギュレーションセットをkey-valueで取得する
   * @param rtc 対象のRTコンポーネント
   * @return アクティブなコンフィギュレーションセット
   * @else
   * @brief
   * @param rtc 
   * @return 
   * @endif
   */
  coil::Properties get_active_configuration(const RTC::RTObject_ptr rtc)
  {
    SDOPackage::Configuration_ptr conf = rtc->get_configuration();
    SDOPackage::ConfigurationSet* confset = conf->get_active_configuration_set();
    SDOPackage::NVList confData = confset->configuration_data;

    coil::Properties prop;
    NVUtil::copyToProperties(prop, confData);
    return prop;
  }
  /*!
   * @if jp
   * @brief コンフィギュレーションパラメータを設定
   * @param rtc 対象のRTコンポーネント
   * @param confset_name コンフィギュレーションセット名
   * @param value_name パラメータ名 
   * @param value パラメータ 
   * @return True:設定に成功、False:設定に失敗
   * @else
   * @brief
   * @param rtc 
   * @param confset_name 
   * @param value_name 
   * @param value 
   * @return 
   * @endif
   */
  bool set_configuration(const RTC::RTObject_ptr rtc, const std::string& confset_name, const std::string& value_name, const std::string& value)
  {
    SDOPackage::Configuration_ptr conf = rtc->get_configuration();
    SDOPackage::ConfigurationSet* confset = conf->get_configuration_set(confset_name.c_str());

    set_configuration_parameter(conf, confset, value_name, value);

    conf->activate_configuration_set(confset_name.c_str());

    return true;
  }
  /*!
   * @if jp
   * @brief アクティブなコンフィギュレーションセットのパラメータを設定
   * @param rtc 対象のRTコンポーネント
   * @param value_name パラメータ名 
   * @param value パラメータ 
   * @return True:設定に成功、False:設定に失敗
   * @else
   * @brief
   * @param rtc 
   * @param value_name 
   * @param value 
   * @return 
   * @endif
   */
  bool set_active_configuration(const RTC::RTObject_ptr rtc, const std::string& value_name, const std::string& value)
  {
    SDOPackage::Configuration_ptr conf = rtc->get_configuration();
    SDOPackage::ConfigurationSet* confset = conf->get_active_configuration_set();

    set_configuration_parameter(conf, confset, value_name, value);

    conf->activate_configuration_set(confset->id);

    return true;
  }
  /*!
   * @if jp
   * @brief コンフィギュレーションパラメータの設定
   * @param conf コンフィギュレーション
   * @param confset コンフィギュレーションセット 
   * @param value_name パラメータ名 
   * @param value パラメータ 
   * @return True:設定に成功、False:設定に失敗
   * @else
   * @brief
   * @param rtc 
   * @param conf 
   * @param confset 
   * @param value_name 
   * @param value 
   * @return 
   * @endif
   */
  bool set_configuration_parameter(SDOPackage::Configuration_ptr conf, SDOPackage::ConfigurationSet* confset, const std::string& value_name, const std::string& value)
  {
    SDOPackage::NVList confData = confset->configuration_data;
    coil::Properties prop;
    NVUtil::copyToProperties(prop, confData);
    prop[value_name] = value;

    NVUtil::copyFromProperties(confData, prop);

    confset->configuration_data = confData;

    conf->set_configuration_set_values((*confset));

    return true;
  }
}  // namespace CORBA_RTCUtil

