// -*- C++ -*-
/*!
 * @file RTCOperations.hpp
 * @brief Operations on RTCs / managers via CORBA (OpenRTM-aist C++)
 *
 * @~japanese
 * CORBA による RTC / マネージャの操作 (OpenRTM-aist C++)
 * @~
 *
 * @date $Date: 2026-11-13 16:24:30 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2026
 *     Nobuhiko Miyamoto
 *
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef SNMPMGR_RTCOPS_HPP
#define SNMPMGR_RTCOPS_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include <rtm/CORBA_RTCUtil.h>
#include <rtm/idl/ManagerSkel.h>
#include <rtm/idl/RTCSkel.h>

#include <nlohmann/json.hpp>
#include <snmpmgr/Plugin.hpp>

namespace snmpmgr
{
  namespace rtcops
  {
    /*!
     * @~japanese
     * @brief マスターマネージャの参照を得る (corbaloc)
     *
     * @param orb ORB
     * @param ip IP アドレス
     * @param port CORBA のポート番号
     * @param name マネージャの名前
     * @return マネージャの参照
     * @~english
     * @brief Obtains the reference of a master manager (corbaloc)
     *
     * @param orb ORB
     * @param ip IP address
     * @param port CORBA port number
     * @param name manager name
     * @return manager reference
     * @~
     */
    RTM::Manager_var resolve_manager(CORBA::ORB_ptr orb, const std::string& ip, int port, const std::string& name);
    /*!
     * @~japanese
     * @brief マスターとスレーブの RTC を返す
     *
     * @param mgr マネージャ
     * @return RTC のリスト
     * @~english
     * @brief Returns the RTCs of the master and its slaves
     *
     * @param mgr manager
     * @return list of RTCs
     * @~
     */
    std::vector<RTC::RTObject_var> components(RTM::Manager_ptr mgr);
    /*!
     * @~japanese
     * @brief インスタンス名で RTC を探す (スレーブを含む)
     *
     * @param mgr マネージャ
     * @param instance インスタンス名
     * @return RTC (なければ nil)
     * @~english
     * @brief Finds an RTC by instance name (including slaves)
     *
     * @param mgr manager
     * @param instance instance name
     * @return RTC (nil if not found)
     * @~
     */
    RTC::RTObject_var find(RTM::Manager_ptr mgr, const std::string& instance);
    /*!
     * @~japanese
     * @brief RTC を生成する
     *
     * @param mgr マネージャ
     * @param create create 文字列
     * @return 生成した RTC
     * @~english
     * @brief Creates an RTC
     *
     * @param mgr manager
     * @param create create string
     * @return created RTC
     * @~
     */
    RTC::RTObject_var create(RTM::Manager_ptr mgr, const std::string& create);
    /*!
     * @~japanese
     * @brief RTC のインスタンス名を返す
     *
     * @param rtc RTC
     * @return インスタンス名
     * @~english
     * @brief Returns the instance name of an RTC
     *
     * @param rtc RTC
     * @return instance name
     * @~
     */
    std::string instance_name(RTC::RTObject_ptr rtc);
    /*!
     * @~japanese
     * @brief RTC のポートを名前で返す
     *
     * @param rtc RTC
     * @param port_name ポートの名前
     * @return ポートの参照
     * @~english
     * @brief Returns a port of an RTC by name
     *
     * @param rtc RTC
     * @param port_name port name
     * @return port reference
     * @~
     */
    RTC::PortService_var port(RTC::RTObject_ptr rtc, const std::string& port_name);
    /*!
     * @~japanese
     * @brief 同じ名前の接続が既にあるか
     *
     * @param src 接続元のポート
     * @param dst 接続先のポート
     * @param name 接続の名前
     * @return あれば true
     * @~english
     * @brief Whether a connector with the same name already exists
     *
     * @param src source port
     * @param dst destination port
     * @param name connector name
     * @return true if it exists
     * @~
     */
    bool connector_exists(RTC::PortService_ptr src, RTC::PortService_ptr dst, const std::string& name);
    /*!
     * @~japanese
     * @brief ポートを接続する
     *
     * @param name 接続の名前
     * @param props 接続のプロパティ
     * @param src 接続元のポート
     * @param dst 接続先のポート
     * @~english
     * @brief Connects ports
     *
     * @param name connector name
     * @param props connector properties
     * @param src source port
     * @param dst destination port
     * @~
     */
    void connect(const std::string& name, const nlohmann::json& props, RTC::PortService_ptr src,
                 RTC::PortService_ptr dst);
    /*!
     * @~japanese
     * @brief ACTIVE でなければアクティブ化する
     *
     * @param rtc RTC
     * @~english
     * @brief Activates the RTC unless it is ACTIVE
     *
     * @param rtc RTC
     * @~
     */
    void activate(RTC::RTObject_ptr rtc);
    /*!
     * @~japanese
     * @brief 非アクティブ化 -> 全ポートの切断 -> exit
     *
     * @param rtc RTC
     * @~english
     * @brief Deactivate -> disconnect all ports -> exit
     *
     * @param rtc RTC
     * @~
     */
    void remove(RTC::RTObject_ptr rtc);
    /*!
     * @~japanese
     * @brief get_loadable_modules() の結果を返す
     *
     * @param mgr マネージャ
     * @return モジュールのプロファイルのリスト
     * @~english
     * @brief Returns the result of get_loadable_modules()
     *
     * @param mgr manager
     * @return list of module profiles
     * @~
     */
    std::vector<std::map<std::string, std::string>> loadable_modules(RTM::Manager_ptr mgr);
    /*!
     * @~japanese
     * @brief slave マネージャが動いている言語を返す
     *
     * @param mgr マネージャ
     * @return 言語の集合
     * @~english
     * @brief Returns the languages of the running slave managers
     *
     * @param mgr manager
     * @return set of languages
     * @~
     */
    std::set<std::string> slave_languages(RTM::Manager_ptr mgr);
    /*!
     * @~japanese
     * @brief RTC の一覧の 1 件 (情報、参照、slave マネージャの pid)
     * @~english
     * @brief One entry of the RTC list (information, reference, pid of the slave manager)
     * @~
     */
    struct Component
    {
      ComponentInfo info;
      RTC::RTObject_var ref;
      int slave_pid{0};         // slave マネージャの pid (slave の設定の manager.pid。分からなければ 0)
    };
    /*!
     * @~japanese
     * @brief RTC の一覧を返す (slave を先に調べる。マスターの get_components() は slave の RTC も返すので、どの slave か分かるように)
     *
     * @param mgr マネージャ
     * @return Component のリスト
     * @~english
     * @brief Returns the RTC list (slaves are examined first, since get_components() of the master also returns slave RTCs, to know which slave each belongs to)
     *
     * @param mgr manager
     * @return list of Component
     * @~
     */
    std::vector<Component> list_components(RTM::Manager_ptr mgr);
    /*!
     * @~japanese
     * @brief 実行コンテキスト 0 での状態を返す
     *
     * @param rtc RTC
     * @return ACTIVE / INACTIVE / ERROR / CREATED / UNKNOWN
     * @~english
     * @brief Returns the state in execution context 0
     *
     * @param rtc RTC
     * @return ACTIVE / INACTIVE / ERROR / CREATED / UNKNOWN
     * @~
     */
    std::string state_of(RTC::RTObject_ptr rtc);
    /*!
     * @~japanese
     * @brief ERROR の RTC をリセットする
     *
     * @param rtc RTC
     * @return 成功すれば true
     * @~english
     * @brief Resets an RTC in ERROR
     *
     * @param rtc RTC
     * @return true on success
     * @~
     */
    bool reset(RTC::RTObject_ptr rtc);
    /*!
     * @~japanese
     * @brief ポートの短い名前 -> 参照 (get_component_profile() 1 回) と、get_ports() の並び順を返す
     *
     * @param rtc RTC
     * @return (インスタンス名, [(ポートの短い名前, 参照)])
     * @~english
     * @brief Returns short port names -> references (one get_component_profile() call) in the order of get_ports()
     *
     * @param rtc RTC
     * @return (instance name, [(short port name, reference)])
     * @~
     */
    std::pair<std::string, std::vector<std::pair<std::string, RTC::PortService_var>>> ports_by_profile(RTC::RTObject_ptr rtc);
    /*!
     * @~japanese
     * @brief slave マネージャの名前 (manager.instance_name) -> その slave の RTC のインスタンス名を返す
     *
     * @param mgr マネージャ
     * @return 表
     * @~english
     * @brief Returns slave manager names (manager.instance_name) -> instance names of the RTCs in that slave
     *
     * @param mgr manager
     * @return table
     * @~
     */
    std::map<std::string, std::vector<std::string>> slave_components(RTM::Manager_ptr mgr);
  } // namespace rtcops
} // namespace snmpmgr

#endif
