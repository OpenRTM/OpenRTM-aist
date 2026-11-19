// -*- C++ -*-
/*!
 * @file ComponentObserverServant.hpp
 * @brief Server objects of the component observer (observer-extended migration). Same as the Python version SNMPManager.py
 *
 * @~japanese
 * コンポーネントオブザーバーのサーバーオブジェクト (オブザーバ拡張マイグレーション)。Python 版 SNMPManager.py と同じ
 *
 * RTC 側 (クライアントオブジェクト) は ComponentObserverConsumer。RTC の SDO 設定に ServiceProfile としてサーバーオブジェクトを関連付けると、RTC の状態が変わったときに update_status(種類, ヒント) が届く。通知にはどの RTC からかが入らないので、RTC ごとに別のサーバーオブジェクトを起動する。インターフェースは 2 種類あり、RTC 側の言語で違う (関連付けに失敗したらもう一方を試す)。
 * - IDL:OpenRTM/ComponentObserver:1.0: OpenRTM-aist-Python の ComponentObserverConsumer
 * - IDL:omg.org/RTC/ComponentObserver:1.0: OpenRTM-aist C++ 2.1 の ComponentObserverConsumer.so
 * @~english
 * The RTC side (client object) is ComponentObserverConsumer. When a server object is attached to the SDO configuration of an RTC as a ServiceProfile, update_status(kind, hint) arrives when the state of the RTC changes. The notification does not identify the RTC, so a separate server object is started for each RTC. There are two interfaces depending on the language of the RTC (if attaching fails, the other is tried).
 * - IDL:OpenRTM/ComponentObserver:1.0: ComponentObserverConsumer of OpenRTM-aist-Python
 * - IDL:omg.org/RTC/ComponentObserver:1.0: ComponentObserverConsumer.so of OpenRTM-aist C++ 2.1
 * @~
 *
 * @date $Date: 2026-11-13 16:24:28 $
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

#ifndef SNMPMGR_OBSERVER_HPP
#define SNMPMGR_OBSERVER_HPP

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <omniORB4/CORBA.h>
#include <rtm/idl/RTCSkel.h>

namespace snmpmgr
{
  namespace observer
  {
    /*!
     * @~japanese
     * @brief 通知を受ける関数 (RTC のインスタンス名, 種類 "RTC_STATUS" など, ヒント "ERROR:0" など)。ORB のスレッドから呼ばれる
     * @~english
     * @brief Function receiving notifications (instance name of the RTC, kind such as "RTC_STATUS", hint such as "ERROR:0"). Called from ORB threads
     * @~
     */
    using Notify = std::function<void(const std::string& instance, const std::string& kind, const std::string& hint)>;

    /*!
     * @~japanese
     * @brief 関連付けたサーバーオブジェクト (インスタンス名、インターフェース、ServiceProfile の ID、オブジェクト ID)
     * @~english
     * @brief Attached server object (instance name, interface, ServiceProfile ID, object ID)
     * @~
     */
    struct Handle
    {
      std::string instance;
      std::string interface_type;   // 関連付けに使ったインターフェース
      std::string profile_id;
      PortableServer::ObjectId oid;
      bool active{false};
    };

    /*!
     * @~japanese
     * @brief RTC にサーバーオブジェクトを関連付ける
     *
     * @param poa POA
     * @param rtc RTC
     * @param instance インスタンス名
     * @param observed_status 観測する状態 (RTC_STATUS など)
     * @param notify 通知を受ける関数
     * @param error 失敗したときの理由
     * @param extra ServiceProfile に加えるプロパティ (heartbeat.enable など)
     * @return Handle (RTC 側にコンシューマがなければ nullopt)
     * @~english
     * @brief Attaches a server object to an RTC
     *
     * @param poa POA
     * @param rtc RTC
     * @param instance instance name
     * @param observed_status statuses to observe (RTC_STATUS, etc.)
     * @param notify function receiving notifications
     * @param error reason on failure
     * @param extra properties added to the ServiceProfile (heartbeat.enable, etc.)
     * @return Handle (nullopt if the RTC has no consumer)
     * @~
     */
    std::optional<Handle> attach(PortableServer::POA_ptr poa, RTC::RTObject_ptr rtc, const std::string& instance,
                                 const std::string& observed_status, const Notify& notify, std::string& error,
                                 const std::vector<std::pair<std::string, std::string>>& extra = {});
    /*!
     * @~japanese
     * @brief サーバーオブジェクトを止める
     *
     * @param poa POA
     * @param handle Handle
     * @~english
     * @brief Stops a server object
     *
     * @param poa POA
     * @param handle Handle
     * @~
     */
    void detach(PortableServer::POA_ptr poa, Handle& handle);
  } // namespace observer
} // namespace snmpmgr

#endif
