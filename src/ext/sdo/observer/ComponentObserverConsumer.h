// -*- C++ -*-
/*!
 * @file ComponentObserverConsumer.h
 * @brief Component observer SDO service consumer implementation
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
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
#include <rtm/idl/SDOPackageStub.h>
#include <ComponentObserverStub.h>

#include <utility>

namespace RTC
{

  /*!
   * @if jp
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
    inline void updateStatus(OpenRTM::StatusKind statuskind, const char* msg)
    {
      std::lock_guard<std::mutex> guard(mutex);
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
    static inline const char* toString(OpenRTM::StatusKind kind)
    {
      static const std::array<const char*, 6> kinds =
        {{
          "COMPONENT_PROFILE",
          "RTC_STATUS",
          "EC_STATUS",
          "PORT_PROFILE",
          "CONFIGURATION",
          "HEARTBEAT"
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
    void setHeartbeat(coil::Properties& prop);

    /*!
     * @if jp
     * @brief データポートイベントの間隔を設定する
     * @else
     * @brief Setting interval of dataport events
     * @endif
     */
    void setDataPortInterval(coil::Properties& prop);

    /*!
     * @if jp
     * @brief ハートビートを解除する
     * @else
     * @brief Unsetting heartbeat
     * @endif
     */
    void unsetHeartbeat();

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
            m_coc.updateStatus(OpenRTM::RTC_STATUS, msg.c_str());
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
        m_coc.unsetHeartbeat();
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
        : m_coc(coc) {}
      void onGeneric(const char* _msg, const char* portname)
      {
        std::string msg(_msg);
        msg += portname;
        m_coc.updateStatus(OpenRTM::PORT_PROFILE, msg.c_str());
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

      PortActionListener* portAddListener{nullptr};
      PortActionListener* portRemoveListener{nullptr};
      PortConnectRetListener* portConnectListener{nullptr};
      PortConnectRetListener* portDisconnectListener{nullptr};

    private:
      ComponentObserverConsumer& m_coc;
    };

    /*!
     * @if jp
     * @brief DataPort\??\?A |? ￠\￣\・\c\o\?\E
     * @else
     * @brief DataPort's data send/receive action listener
     * @endif
     */
    class DataPortAction
      : public ConnectorDataListener
    {
    public:
      DataPortAction(ComponentObserverConsumer& coc,
                     std::string  msg,
                     std::chrono::nanoseconds interval)
        : m_coc(coc), m_msg(std::move(msg)), m_interval(interval)
      {
      }
      ~DataPortAction() override = default;

      ReturnCode operator()(ConnectorInfo&  /*info*/,
                            ByteData&  /*data*/, const std::string& /*marsharingtype*/) override
      {
        auto curr = std::chrono::steady_clock::now();
        auto intvl = curr - m_last;
        if (intvl > m_interval)
          {
            m_last = curr;
            m_coc.updateStatus(OpenRTM::PORT_PROFILE, m_msg.c_str());
          }

        return NO_CHANGE;
      }
    private:
      ComponentObserverConsumer& m_coc;
      std::string m_msg;
      std::chrono::nanoseconds m_interval;
      std::chrono::steady_clock::time_point m_last{std::chrono::steady_clock::now()};
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
        m_coc.updateStatus(OpenRTM::EC_STATUS, msg.c_str());
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
        m_coc.updateStatus(OpenRTM::CONFIGURATION, msg.c_str());
      }
      void setConfigSet(const coil::Properties& config_set)
      {
        std::string msg("SET_CONFIG_SET: ");
        msg += config_set.getName();
        m_coc.updateStatus(OpenRTM::CONFIGURATION, msg.c_str());
      }
      void addConfigSet(const coil::Properties& config_set)
      {
        std::string msg("ADD_CONFIG_SET: ");
        msg += config_set.getName();
        m_coc.updateStatus(OpenRTM::CONFIGURATION, msg.c_str());
      }
      void updateConfigSet(const char* config_set_name)
      {
        std::string msg("UPDATE_CONFIG_SET: ");
        msg += config_set_name;
        m_coc.updateStatus(OpenRTM::CONFIGURATION, msg.c_str());
      }
      void removeConfigSet(const char* config_set_name)
      {
        std::string msg("REMOVE_CONFIG_SET: ");
        msg += config_set_name;
        m_coc.updateStatus(OpenRTM::CONFIGURATION, msg.c_str());
      }
      void activateConfigSet(const char* config_set_name)
      {
        std::string msg("ACTIVATE_CONFIG_SET: ");
        msg += config_set_name;
        m_coc.updateStatus(OpenRTM::CONFIGURATION, msg.c_str());
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



    RTC::RTObject_impl* m_rtobj{nullptr};
    SDOPackage::ServiceProfile m_profile;
    CorbaConsumer<OpenRTM::ComponentObserver> m_observer;

    std::array<bool, OpenRTM::STATUS_KIND_NUM> m_observed;

    // ComponentProfile
    CompStatMsg m_compstat{*this};

    // PortProfile
    PortAction m_portaction{*this};
    std::chrono::nanoseconds m_inportInterval{std::chrono::seconds(1)};
    std::chrono::nanoseconds m_outportInterval{std::chrono::seconds(1)};

    // Execution Context
    ECAction m_ecaction{*this};
    ConfigAction m_configMsg{*this};

    // Heartbeat
    bool m_heartbeat{false};
    Manager::TaskId m_hbtaskid;

    std::mutex mutex;

    std::vector<DataPortAction*> m_recievedactions;
    std::vector<DataPortAction*> m_sendactions;

  };

} // namespace RTC

extern "C"
{
  DLL_EXPORT void ComponentObserverConsumerInit();
}

#endif // RTC_COMPONENTOBSERVERCONSUMER_H
