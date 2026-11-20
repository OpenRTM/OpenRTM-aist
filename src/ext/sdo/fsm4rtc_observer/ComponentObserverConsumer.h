// -*- C++ -*-
/*!
 * @file ComponentObserverConsumer.h
 * @brief Component observer SDO service consumer implementation
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id: SdoConfiguration.cpp 1971 2010-06-03 08:46:40Z n-ando $
 *
 */


#ifndef RTC_COMPONENTOBSERVERCONSUMER_H
#define RTC_COMPONENTOBSERVERCONSUMER_H

#include <array>
#include <coil/Factory.h>
#include <coil/stringutil.h>
#include <rtm/SdoServiceConsumerBase.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/ComponentActionListener.h>
#include <rtm/FsmActionListener.h>
#include <rtm/idl/SDOPackageStub.h>
#include "ComponentObserverStub.h"

namespace RTC
{

  /*!
   * @if jp
   * @class ComponentObserverConsumer クラス
   * @brief ComponentObserver モジュール
   *
   * コンポーネントの各種状態をComponentObserverサービスに対してコール
   * バックするためのクラス。ツール等、コンポーネントの状態変化を知りた
   * いエンティティがサービスプロバイダを当該コンポーネントに対してセッ
   * トすると、対応する本コンシューマがアタッチされ、コンポーネントの状
   * 態変化に応じて、update_status() オペレーションをSTATUS_KIND とヒン
   * トを引数に呼び出す。本機能は、OMG の FSM4RTC仕様
   * (formal/16-04-01) 7.2.4.2 ComponentObserver Interface に記述されて
   * いる。
   *
   * STATUS_KIND には以下の種類がある。
   *
   * - COMPONENT_PROFILE: コンポーネントのプロファイル情報が変化
   * - RTC_STATUS       : コンポーネントの状態 (Init, Alive) が変化
   * - EC_STATUS        : ECの状態 (Inavtive, Active, Error) が変化
   * - PORT_PROFILE     : ポートのプロファイルが変化
   * - CONFIGURATION    : コンフィギュレーションが変化
   * - RTC_HEARTBEAT    : RTCの生存確認のハートビード
   * - EC_HEARTBEAT     : ECの生存確認のハートビート
   * - FSM_PROFILE      : FSMのプロファイルが変化
   * - FSM_STATUS       : FSMの状態が変化
   * - FSM_STRUCTURE    : FSMの構造が変化
   * - USER_DEFINED     : ユーザ定義
   *
   * \subsection COMPONENT_PROFILE COMPONENT_PROFILE
   * コンポーネントのプロファイル情報が変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。(未実装)
   *
   * \subsection RTC_STATUS RTC_STATUS
   *
   * コンポーネントの状態 (Init, Alive) が変化した際にこのタグ名
   * (enum値)を第1引数にして update_status() オペレーションが呼び出され
   * る。厳密にはECの状態であるが、Inavtive, Active, Error, Finalize の
   * 4つの状態に変化したことを検知することができる。以下の状態変化時に、
   * それぞれヒントとして以下の文字列とともにコールバックされる。
   *
   * - onActivated 呼び出し成功時:   ACTIVE: <EC id>
   * - onDeactivated 呼び出し成功時: INACTIVE: <EC id>
   * - onReset 呼び出し成功時:       INACTIVE: <EC id>
   * - onAborting 呼び出し成功時:    ERROR: <EC id>
   * - onFinalize 呼び出し成功時:    FINALIZE: <EC id>
   *
   * \subsection EC_STATUS EC_STATUS
   *
   * ECの状態 (Inavtive, Active, Error) が変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection PORT_PROFILE PORT_PROFILE
   * ポートのプロファイルが変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection CONFIGURATION CONFIGURATION
   * コンフィギュレーションが変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection RTC_HEARTBEAT RTC_HEARTBEAT
   * RTCの生存確認のハートビードした際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection EC_HEARTBEAT EC_HEARTBEAT
   * ECの生存確認のハートビートした際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection FSM_PROFILE FSM_PROFILE
   * FSMのプロファイルが変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection FSM_STATUS FSM_STATUS
   * FSMの状態が変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection FSM_STRUCTURE FSM_STRUCTURE
   * FSMの構造が変化した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   * \subsection USER_DEFINED USER_DEFINED
   * ユーザ定義した際にこのタグ名(enum値)を
   * 第1引数にして update_status() オペレーションが呼び出される。
   *
   *
   *
   * @else
   * @endif
   */
  class ComponentObserverConsumer
    : public SdoServiceConsumerBase
  {
  public:
    /*!
     * @if jp
     * @brief ctor of ComponentObserverConsumer
     * @else
     * @brief ctor of ComponentObserverConsumer
     * @endif
     */
    ComponentObserverConsumer();

    /*!
     * @if jp
     * @brief dtor
     * @else
     * @brief dtor
     * @endif
     */
    ~ComponentObserverConsumer() override;

    /*!
     * @if jp
     * @brief 初期化
     * @else
     * @brief Initialization
     * @endif
     */
    bool init(RTObject_impl& rtobj,
              const SDOPackage::ServiceProfile& profile) override;

    /*!
     * @if jp
     * @brief 再初期化
     * @else
     * @brief Re-initialization
     * @endif
     */
    bool reinit(const SDOPackage::ServiceProfile& profile) override;

    /*!
     * @if jp
     * @brief ServiceProfile を取得する
     * @else
     * @brief getting ServiceProfile
     * @endif
     */
    const SDOPackage::ServiceProfile& getProfile() const override;
    
    /*!
     * @if jp
     * @brief 終了処理
     * @else
     * @brief Finalization
     * @endif
     */
    void finalize() override;

  protected:
    /*!
     * @if jp
     * @brief リモートオブジェクトコール
     * @else
     * @brief Calling remote object
     * @endif
     */
    inline void updateStatus(RTC::StatusKind statuskind, const char* msg)
    {
      try
        {
          m_observer->update_status(statuskind, msg);
        }
      catch (...)
        {
          m_rtobj->removeSdoServiceConsumerStartThread(m_profile.id);
        }
    }

    /*!
     * @if jp
     * @brief Kindを文字列へ変換する
     * @else
     * @brief Converting kind to string
     * @endif
     */
    static inline const char* toString(RTC::StatusKind kind)
    {
      static const std::array<const char*, 11> kinds =
        {{
          "COMPONENT_PROFILE",
          "RTC_STATUS",
          "EC_STATUS",
          "PORT_PROFILE",
          "CONFIGURATION",
          "RTC_HEARTBEAT",
          "EC_HEARTBEAT",
          "FSM_PROFILE",
          "FSM_STATUS",
          "FSM_STRUCTURE",
          "USER_DEFINED"
        }};
      return static_cast<size_t>(kind) < kinds.size() ? kinds[kind] : "";
    }

    /*!
     * @if jp
     * @brief RTObjectへのリスナ接続処理
     * @else
     * @brief Connectiong listeners to RTObject
     * @endif
     */
    void setListeners(coil::Properties& prop);

    /*!
     * @if jp
     * @brief リスナ接続・切断スイッチング処理
     * @else
     * @brief Switching listeners connecting/disconnecting
     * @endif
     */
    void switchListeners(bool& next, bool& pre,
                         void (ComponentObserverConsumer::*setfunc)(), 
                         void (ComponentObserverConsumer::*unsetfunc)());

    //============================================================
    // Heartbeat related functions
    /*!
     * @if jp
     * @brief ハートビートを設定する
     * @else
     * @brief Setting heartbeat
     * @endif
     */
    void setRTCHeartbeat(coil::Properties& prop);

    /*!
     * @if jp
     * @brief ハートビートを解除する
     * @else
     * @brief Unsetting heartbeat
     * @endif
     */
    void unsetRTCHeartbeat();

    /*!
     * @if jp
     * @brief ハートビートをオブザーバに伝える
     * @else
     * @brief Sending a heartbeart signal to observer
     * @endif
     */
    void ecHeartbeat();

    /*!
     * @if jp
     * @brief ハートビートを設定する
     * @else
     * @brief Setting heartbeat
     * @endif
     */
    void setECHeartbeat(coil::Properties& prop);

    /*!
     * @if jp
     * @brief ハートビートを解除する
     * @else
     * @brief Unsetting heartbeat
     * @endif
     */
    void unsetECHeartbeat();

    //============================================================
    // Component status related functions
    /*!
     * @if jp
     * @brief RTC状態変化リスナの設定処理
     * @else
     * @brief Setting RTC status listeners
     * @endif
     */
    void setComponentStatusListeners();

    /*!
     * @if jp
     * @brief RTC状態変化リスナの解除処理
     * @else
     * @brief Unsetting RTC status listeners
     * @endif
     */
    void unsetComponentStatusListeners();

    //============================================================
    // FSM status related functions
    /*!
     * @if jp
     * @brief FSM状態変化リスナの設定処理
     * @else
     * @brief Setting FSM status listeners
     * @endif
     */
    void setFSMStatusListeners();

    /*!
     * @if jp
     * @brief FSM状態変化リスナの解除処理
     * @else
     * @brief Unsetting FSM status listeners
     * @endif
     */
    void unsetFSMStatusListeners();

    //============================================================
    // Port profile related functions
    /*!
     * @if jp
     * @brief Portプロファイル変化リスナの設定処理
     * @else
     * @brief Setting port profile listener
     * @endif
     */
    void setPortProfileListeners();

    /*!
     * @if jp
     * @brief Portプロファイル変化リスナの解除処理
     * @else
     * @brief Unsetting port profile listener
     * @endif
     */
    void unsetPortProfileListeners();


    //============================================================
    // EC profile related functions
    /*!
     * @if jp
     * @brief ECの状態変化リスナの設定
     * @else
     * @brief Setting EC status listener
     * @endif
     */
    void setExecutionContextListeners();

    /*!
     * @if jp
     * @brief ECの状態変化リスナの解除
     * @else
     * @brief Unsetting EC status listener
     * @endif
     */
    void unsetExecutionContextListeners();


    //============================================================
    // ComponentProfile related functions
    /*!
     * @if jp
     * @brief ComponentProfile状態変化リスナの設定
     * @else
     * @brief Setting ComponentProfile listener
     * @endif
     */
    void setComponentProfileListeners();

    /*!
     * @if jp
     * @brief ComponentProfile状態変化リスナの解除
     * @else
     * @brief Unsetting ComponentProfile listener
     * @endif
     */
    void unsetComponentProfileListeners();

    //============================================================
    // FsmProfile related functions
    /*!
     * @if jp
     * @brief FsmProfile状態変化リスナの設定
     * @else
     * @brief Setting FsmProfile listener
     * @endif
     */
    void setFSMProfileListeners();

    /*!
     * @if jp
     * @brief FsmProfile状態変化リスナの解除
     * @else
     * @brief Unsetting FsmProfile listener
     * @endif
     */
    void unsetFSMProfileListeners();

    //============================================================
    // FsmStructure related functions
    /*!
     * @if jp
     * @brief FsmStructure状態変化リスナの設定
     * @else
     * @brief Setting FsmStructure listener
     * @endif
     */
    void setFSMStructureListeners();

    /*!
     * @if jp
     * @brief FsmStructure状態変化リスナの解除
     * @else
     * @brief Unsetting FsmStructure listener
     * @endif
     */
    void unsetFSMStructureListeners();

    //============================================================
    // Configuration related functions

    /*!
     * @if jp
     * @brief Configuration状態変化リスナの設定
     * @else
     * @brief Setting Configuration listener
     * @endif
     */
    void setConfigurationListeners();

    /*!
     * @if jp
     * @brief Configuration状態変化リスナの解除
     * @else
     * @brief Unsetting Configurationlistener
     * @endif
     */
    void unsetConfigurationListeners();

  private:
    /*!
     * @if jp
     * @brief PostComponentActionListener class
     * @else
     * @brief PostComponentActionListener class
     * @endif
     */
    class CompStatMsg
    {
    public:
      CompStatMsg(ComponentObserverConsumer& coc)
        : activatedListener(nullptr), deactivatedListener(nullptr),
          resetListener(nullptr), abortingListener(nullptr),
          finalizeListener(nullptr), m_coc(coc) {}
      void onGeneric(const char* msgprefix, UniqueId ec_id, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            std::string msg(msgprefix);
            msg += coil::otos(ec_id);
            m_coc.updateStatus(RTC::RTC_STATUS, msg.c_str());
          }
      }
      void onActivated(UniqueId ec_id, ReturnCode_t ret)
      {
        onGeneric("ACTIVE:", ec_id, ret);
      }
      void onDeactivated(UniqueId ec_id, ReturnCode_t ret)
      {
        onGeneric("INACTIVE:", ec_id, ret);
      }
      void onReset(UniqueId ec_id, ReturnCode_t ret)
      {
        onGeneric("INACTIVE:", ec_id, ret);
      }
      void onAborting(UniqueId ec_id, ReturnCode_t ret)
      {
        onGeneric("ERROR:", ec_id, ret);
      }
      void onFinalize(UniqueId ec_id, ReturnCode_t ret)
      {
        m_coc.unsetRTCHeartbeat();
        m_coc.unsetECHeartbeat();
        onGeneric("FINALIZE:", ec_id, ret);
      }

      PostComponentActionListener* activatedListener;
      PostComponentActionListener* deactivatedListener;
      PostComponentActionListener* resetListener;
      PostComponentActionListener* abortingListener;
      PostComponentActionListener* finalizeListener;
    private:
      ComponentObserverConsumer& m_coc;
    };

    /*!
     * @if jp
     * @brief PortActionListener
     * @else
     * @brief PortActionListener
     * @endif
     */
    class PortAction
    {
    public:
      PortAction(ComponentObserverConsumer& coc)
        : portAddListener(nullptr), portRemoveListener(nullptr),
          portConnectListener(nullptr), portDisconnectListener(nullptr),
          m_coc(coc) {}
      void onGeneric(const char* _msg, const char* portname)
      {
        std::string msg(_msg);
        msg += portname;
        m_coc.updateStatus(RTC::PORT_PROFILE, msg.c_str());
      }
      void onAddPort(const ::RTC::PortProfile& pprof)
      {
        onGeneric("ADD:", static_cast<const char*>(pprof.name));
      }
      void onRemovePort(const ::RTC::PortProfile& pprof)
      {
        onGeneric("REMOVE:", static_cast<const char*>(pprof.name));
      }
      void onConnect(const char* portname,
                     ::RTC::ConnectorProfile&  /*pprof*/, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            onGeneric("CONNECT:", portname);
          }
      }
      void onDisconnect(const char* portname,
                        ::RTC::ConnectorProfile&  /*pprof*/, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            onGeneric("DISCONNECT:", portname);
          }
      }

      PortActionListener* portAddListener;
      PortActionListener* portRemoveListener;
      PortConnectRetListener* portConnectListener;
      PortConnectRetListener* portDisconnectListener;

    private:
      ComponentObserverConsumer& m_coc;
    };

    /*!
     * @if jp
     * @brief ExecutionContextActionListener
     * @else
     * @brief ExecutionContextActionListener
     * @endif
     */
    class ECAction
    {
    public:
      ECAction(ComponentObserverConsumer& coc)
        : m_coc(coc) {}
      void onGeneric(const char* _msg, UniqueId ec_id)
      {
        std::string msg(_msg + coil::otos(ec_id));
        m_coc.updateStatus(RTC::EC_STATUS, msg.c_str());
      }
      void onAttached(UniqueId ec_id)
      {
        onGeneric("ATTACHED:", ec_id);
      }
      void onDetached(UniqueId ec_id)
      {
        onGeneric("DETACHED:", ec_id);
      }
      void onRateChanged(UniqueId ec_id, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            onGeneric("RATE_CHANGED:", ec_id);
          }
      }
      void onStartup(UniqueId ec_id, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            onGeneric("STARTUP:", ec_id);
          }
      }
      void onShutdown(UniqueId ec_id, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            onGeneric("SHUTDOWN:", ec_id);
          }
      }
      ExecutionContextActionListener* ecAttached{nullptr};
      ExecutionContextActionListener* ecDetached{nullptr};
      PostComponentActionListener* ecRatechanged{nullptr};
      PostComponentActionListener* ecStartup{nullptr};
      PostComponentActionListener* ecShutdown{nullptr};
    private:
      ComponentObserverConsumer& m_coc;
    };

    /*!
     * @if jp
     * @brief ConfigActionListener
     * @else
     * @brief ConfigActionListener
     * @endif
     */
    class ConfigAction
    {
    public:
      ConfigAction(ComponentObserverConsumer& coc)
        : m_coc(coc) {}
      void updateConfigParam(const char* configsetname,
                             const char* configparamname)
      {
        std::string msg("UPDATE_CONFIG_PARAM: ");
        msg += configsetname;
        msg += ".";
        msg += configparamname;
        m_coc.updateStatus(RTC::CONFIGURATION, msg.c_str());
      }
      void setConfigSet(const coil::Properties& config_set)
      {
        std::string msg("SET_CONFIG_SET: ");
        msg += config_set.getName();
        m_coc.updateStatus(RTC::CONFIGURATION, msg.c_str());
      }
      void addConfigSet(const coil::Properties& config_set)
      {
        std::string msg("ADD_CONFIG_SET: ");
        msg += config_set.getName();
        m_coc.updateStatus(RTC::CONFIGURATION, msg.c_str());
      }
      void updateConfigSet(const char* config_set_name)
      {
        std::string msg("UPDATE_CONFIG_SET: ");
        msg += config_set_name;
        m_coc.updateStatus(RTC::CONFIGURATION, msg.c_str());
      }
      void removeConfigSet(const char* config_set_name)
      {
        std::string msg("REMOVE_CONFIG_SET: ");
        msg += config_set_name;
        m_coc.updateStatus(RTC::CONFIGURATION, msg.c_str());
      }
      void activateConfigSet(const char* config_set_name)
      {
        std::string msg("ACTIVATE_CONFIG_SET: ");
        msg += config_set_name;
        m_coc.updateStatus(RTC::CONFIGURATION, msg.c_str());
      }
      // Listener object's pointer holder
      ConfigurationParamListener*   updateConfigParamListener{nullptr};
      ConfigurationSetListener*     setConfigSetListener{nullptr};
      ConfigurationSetListener*     addConfigSetListener{nullptr};
      ConfigurationSetNameListener* updateConfigSetListener{nullptr};
      ConfigurationSetNameListener* removeConfigSetListener{nullptr};
      ConfigurationSetNameListener* activateConfigSetListener{nullptr};

    private:
      ComponentObserverConsumer& m_coc;
    };

    /*!
     * @if jp
     * @brief FSMActionListener
     * @else
     * @brief FSMActionListener
     * @endif
     */
    class FSMAction
    {
    public:
      FSMAction(ComponentObserverConsumer& coc)
        : m_coc(coc) {}
      // Action callback functions here
      void updateFsmStatus(const char* state)
      {
        std::cout << "ComponentObserver::updateFsmStatus(" << state << ")" << std::endl; 
        m_coc.updateStatus(RTC::FSM_STATUS, state);
      }

      void preInit(const char* state)
      {
        std::string msg(state); msg += " PRE_ON_INIT";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void preEntry(const char* state)
      {
        std::string msg(state); msg += " PRE_ONENTRY";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void preDo(const char* state)
      {
        std::string msg(state); msg += " PRE_ON_DO";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void preExit(const char* state)
      {
        std::string msg(state); msg += " PRE_ON_EXIT";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void preStateChange(const char* state)
      {
        std::string msg(state); msg += " PRE_ON_STATE_CHANGE";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }

      void postInit(const char* state, ReturnCode_t  /*ret*/)
      {
        std::string msg(state); msg += " POST_ON_INIT";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void postEntry(const char* state, ReturnCode_t  /*ret*/)
      {
        std::string msg(state); msg += " POST_ONENTRY";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void postDo(const char* state, ReturnCode_t  /*ret*/)
      {
        std::string msg(state); msg += " POST_ON_DO";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void postExit(const char* state, ReturnCode_t  /*ret*/)
      {
        std::string msg(state); msg += " POST_ON_EXIT";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }
      void postStateChange(const char* state, ReturnCode_t  /*ret*/)
      {
        std::string msg(state); msg += " POST_ON_STATE_CHNAGE";
        m_coc.updateStatus(RTC::FSM_STATUS, msg.c_str());
      }

      // Listener object's pointer holder
      PreFsmActionListener* preOnFsmInitListener;
      PreFsmActionListener* preOnFsmEntryListener;
      PreFsmActionListener* preOnFsmDoListener;
      PreFsmActionListener* preOnFsmExitListener;
      PreFsmActionListener* preOnFsmStateChangeListener;
      
      PostFsmActionListener* postOnFsmInitListener;
      PostFsmActionListener* postOnFsmEntryListener;
      PostFsmActionListener* postOnFsmDoListener;
      PostFsmActionListener* postOnFsmExitListener;
      PostFsmActionListener* postOnFsmStateChangeListener;

    private:
      ComponentObserverConsumer& m_coc;
    };



    RTC::RTObject_impl* m_rtobj{nullptr};
    SDOPackage::ServiceProfile m_profile;
    CorbaConsumer<RTC::ComponentObserver> m_observer;

    std::array<bool, RTC::STATUS_KIND_NUM> m_observed;

    CompStatMsg m_compstat{*this};
    PortAction m_portaction{*this};
    ECAction m_ecaction{*this};
    ConfigAction m_configMsg{*this};
    FSMAction m_fsmaction{*this};

    bool m_rtcHeartbeat{false};
    Manager::TaskId m_rtcHbTaskId;
    bool m_ecHeartbeat{false};
    Manager::TaskId m_ecHbTaskId;
  };

} // namespace RTC

extern "C"
{
  DLL_EXPORT void ComponentObserverConsumerInit();
}

#endif // RTC_COMPONENTOBSERVERCONSUMER_H
