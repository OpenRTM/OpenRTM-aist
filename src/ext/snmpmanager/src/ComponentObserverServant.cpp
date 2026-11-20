// -*- C++ -*-
/*!
 * @file ComponentObserverServant.cpp
 * @brief Server objects of the component observer. See ComponentObserverServant.hpp
 *
 * @~japanese
 * コンポーネントオブザーバーのサーバーオブジェクト。説明は ComponentObserverServant.hpp
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

#include "ComponentObserverServant.hpp"

#if defined(_WIN32)
// OpenRTM のヘッダが USE_stub_in_nt_dll を付けたままにするので、自分のスタブ (この実行ファイルの中) の前で外す
#  undef USE_stub_in_nt_dll
#  undef USE_core_stub_in_nt_dll
#  undef USE_dyn_stub_in_nt_dll
#endif
#include "ComponentObserver_OpenRTM.hh"
#include "ComponentObserver_RTC.hh"

#include <rtm/NVUtil.h>

#include <atomic>
#include <chrono>

namespace snmpmgr
{
  namespace observer
  {
    namespace
    {
      /*!
       * @~japanese
       * @brief 状態の種別の名前を返す
       *
       * @param k 状態の種別
       * @param rtc_module omg.org/RTC の IDL か
       * @return 名前
       * @~english
       * @brief Returns the name of a status kind
       *
       * @param k status kind
       * @param rtc_module whether the IDL is omg.org/RTC
       * @return name
       * @~
       */
      const char* kind_name(int k, bool rtc_module)
      {
        static const char* openrtm[] = {"COMPONENT_PROFILE", "RTC_STATUS", "EC_STATUS", "PORT_PROFILE",
                                        "CONFIGURATION", "HEARTBEAT", "STATUS_KIND_NUM"};
        static const char* rtc[] = {"COMPONENT_PROFILE", "RTC_STATUS", "EC_STATUS", "PORT_PROFILE", "CONFIGURATION",
                                    "RTC_HEARTBEAT", "EC_HEARTBEAT", "FSM_PROFILE", "FSM_STATUS", "FSM_STRUCTURE",
                                    "USER_DEFINED", "STATUS_KIND_NUM"};
        if (rtc_module) { return k >= 0 && k < 12 ? rtc[k] : "UNKNOWN"; }
        return k >= 0 && k < 7 ? openrtm[k] : "UNKNOWN";
      }

      /*!
       * @~japanese
       * @brief IDL:OpenRTM/ComponentObserver:1.0 のサーバーオブジェクト (OpenRTM-aist-Python の RTC 用)
       * @~english
       * @brief Server object of IDL:OpenRTM/ComponentObserver:1.0 (for RTCs of OpenRTM-aist-Python)
       * @~
       */
      class ObserverOpenRTM : public virtual POA_OpenRTM::ComponentObserver
      {
      public:
        /*!
         * @~japanese
         * @brief コンストラクタ
         *
         * @param instance RTC のインスタンス名
         * @param notify 通知を受ける関数
         * @~english
         * @brief Constructor
         *
         * @param instance instance name of the RTC
         * @param notify function receiving notifications
         * @~
         */
        ObserverOpenRTM(std::string instance, Notify notify) : instance_(std::move(instance)), notify_(std::move(notify)) {}
        /*!
         * @~japanese
         * @brief RTC の状態の通知を受ける
         *
         * @param kind 状態の種別
         * @param hint 詳細
         * @~english
         * @brief Receives a status notification of the RTC
         *
         * @param kind status kind
         * @param hint hint
         * @~
         */
        void update_status(OpenRTM::StatusKind kind, const char* hint) override
        {
          notify_(instance_, kind_name(static_cast<int>(kind), false), hint ? hint : "");
        }
      private:
        std::string instance_;
        Notify notify_;
      };

      /*!
       * @~japanese
       * @brief IDL:omg.org/RTC/ComponentObserver:1.0 のサーバーオブジェクト (OpenRTM-aist C++ 2.1 の RTC 用)
       * @~english
       * @brief Server object of IDL:omg.org/RTC/ComponentObserver:1.0 (for RTCs of OpenRTM-aist C++ 2.1)
       * @~
       */
      class ObserverRTC : public virtual POA_RTC::ComponentObserver
      {
      public:
        /*!
         * @~japanese
         * @brief コンストラクタ
         *
         * @param instance RTC のインスタンス名
         * @param notify 通知を受ける関数
         * @~english
         * @brief Constructor
         *
         * @param instance instance name of the RTC
         * @param notify function receiving notifications
         * @~
         */
        ObserverRTC(std::string instance, Notify notify) : instance_(std::move(instance)), notify_(std::move(notify)) {}
        /*!
         * @~japanese
         * @brief RTC の状態の通知を受ける
         *
         * @param kind 状態の種別
         * @param hint 詳細
         * @~english
         * @brief Receives a status notification of the RTC
         *
         * @param kind status kind
         * @param hint hint
         * @~
         */
        void update_status(RTC::StatusKind kind, const char* hint) override
        {
          notify_(instance_, kind_name(static_cast<int>(kind), true), hint ? hint : "");
        }
      private:
        std::string instance_;
        Notify notify_;
      };

      /*!
       * @~japanese
       * @brief ServiceProfile の ID を作る (時刻と通し番号)
       *
       * @return ID
       * @~english
       * @brief Makes an ID of a ServiceProfile (time and sequence number)
       *
       * @return ID
       * @~
       */
      std::string new_id()
      {
        static std::atomic<unsigned long> seq{0};
        auto t = std::chrono::system_clock::now().time_since_epoch().count();
        return "snmpmgr-observer-" + std::to_string(t) + "-" + std::to_string(++seq);
      }

      /*!
       * @~japanese
       * @brief サーバントを有効にして、RTC の SDO 設定に関連付ける。失敗したら止めて false
       *
       * @param poa POA
       * @param servant サーバント
       * @param rtc RTC
       * @param interface_type インターフェースの型
       * @param observed_status 観測する状態
       * @param h 関連付けの情報 (成功すれば入れる)
       * @param error 失敗したときの理由
       * @param extra ServiceProfile に加えるプロパティ
       * @return 成功すれば true
       * @~english
       * @brief Activates a servant and attaches it to the SDO configuration of an RTC. On failure, deactivates it and returns false
       *
       * @param poa POA
       * @param servant servant
       * @param rtc RTC
       * @param interface_type interface type
       * @param observed_status statuses to observe
       * @param h attachment information (filled on success)
       * @param error reason on failure
       * @param extra properties added to the ServiceProfile
       * @return true on success
       * @~
       */
      bool try_attach(PortableServer::POA_ptr poa, PortableServer::ServantBase* servant, RTC::RTObject_ptr rtc,
                      const char* interface_type, const std::string& observed_status, Handle& h, std::string& error,
                      const std::vector<std::pair<std::string, std::string>>& extra)
      {
        PortableServer::ObjectId_var oid = poa->activate_object(servant);
        servant->_remove_ref();   // 以後は POA が持つ (deactivate で消える)
        CORBA::Object_var obj = poa->id_to_reference(oid);
        SDOPackage::ServiceProfile prof;
        h.profile_id = new_id();
        prof.id = CORBA::string_dup(h.profile_id.c_str());
        prof.interface_type = CORBA::string_dup(interface_type);
        prof.properties.length(static_cast<CORBA::ULong>(1 + extra.size()));
        prof.properties[0] = NVUtil::newNV("observed_status", observed_status.c_str());
        for (size_t i = 0; i < extra.size(); ++i)
          {
            prof.properties[static_cast<CORBA::ULong>(i + 1)] = NVUtil::newNV(extra[i].first.c_str(), extra[i].second.c_str());
          }
        prof.service = SDOPackage::SDOService::_narrow(obj);
        bool ok = false;
        try
          {
            SDOPackage::Configuration_var conf = rtc->get_configuration();
            ok = conf->add_service_profile(prof);
            if (!ok) { error = std::string(interface_type) + ": add_service_profile returned false"; }
          }
        catch (CORBA::SystemException& e) { error = std::string(interface_type) + ": CORBA " + e._name(); }
        catch (CORBA::UserException& e) { error = std::string(interface_type) + ": " + e._name(); }
        if (!ok)
          {
            try { poa->deactivate_object(oid); } catch (...) {}
            return false;
          }
        h.interface_type = interface_type;
        h.oid = oid.in();
        h.active = true;
        return true;
      }
    } // namespace

    std::optional<Handle> attach(PortableServer::POA_ptr poa, RTC::RTObject_ptr rtc, const std::string& instance,
                                 const std::string& observed_status, const Notify& notify, std::string& error,
                                 const std::vector<std::pair<std::string, std::string>>& extra)
    {
      Handle h;
      h.instance = instance;
      std::string e1, e2;
      if (try_attach(poa, new ObserverOpenRTM(instance, notify), rtc, OpenRTM::ComponentObserver::_PD_repoId,
                     observed_status, h, e1, extra))
        {
          return h;
        }
      // C++ の RTC (FSM4RTC 版のインターフェース)。種類の名前も違う (HEARTBEAT -> RTC_HEARTBEAT)
      std::string status = observed_status;
      if (auto p = status.find("HEARTBEAT"); p != std::string::npos && status.find("RTC_HEARTBEAT") == std::string::npos)
        {
          status.replace(p, 9, "RTC_HEARTBEAT");
        }
      if (try_attach(poa, new ObserverRTC(instance, notify), rtc, RTC::ComponentObserver::_PD_repoId, status, h, e2, extra))
        {
          return h;
        }
      error = e1 + "; " + e2;
      return std::nullopt;
    }

    void detach(PortableServer::POA_ptr poa, Handle& handle)
    {
      if (!handle.active) { return; }
      try { poa->deactivate_object(handle.oid); } catch (...) {}
      handle.active = false;
    }
  } // namespace observer
} // namespace snmpmgr
