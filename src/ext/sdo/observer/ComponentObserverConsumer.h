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

#include <coil/Mutex.h>
#include <coil/Factory.h>
#include <coil/stringutil.h>
#include <rtm/SdoServiceConsumerBase.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/ComponentActionListener.h>
#include <rtm/idl/SDOPackageStub.h>
#include "ComponentObserverStub.h"

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
    typedef coil::Guard<coil::Mutex> Guard;
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
    virtual ~ComponentObserverConsumer();

    /*!
     * @if jp
     * @brief 初期化
     * @else
     * @brief Initialization
     * @endif
     */
    virtual bool init(RTObject_impl& rtobj,
                      const SDOPackage::ServiceProfile& profile);

    /*!
     * @if jp
     * @brief 再初期化
     * @else
     * @brief Re-initialization
     * @endif
     */
    virtual bool reinit(const SDOPackage::ServiceProfile& profile);

    /*!
     * @if jp
     * @brief ServiceProfile を取得する
     * @else
     * @brief getting ServiceProfile
     * @endif
     */
    virtual const SDOPackage::ServiceProfile& getProfile() const;
    
    /*!
     * @if jp
     * @brief 終了処理
     * @else
     * @brief Finalization
     * @endif
     */
    virtual void finalize();

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
      Guard guard(mutex);
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
    inline const char* toString(OpenRTM::StatusKind kind)
    {
      static const char* kinds[] = 
        {
          "COMPONENT_PROFILE",
          "RTC_STATUS",
          "EC_STATUS",
          "PORT_PROFILE",
          "CONFIGURATION",
          "HEARTBEAT"
        };
      return (size_t)kind < sizeof(kind)/sizeof(char*) ? kinds[kind] : "";
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
     * @brief ハートビートをオブザーバに伝える
     * @else
     * @brief Sending a heartbeart signal to observer
     * @endif
     */
    void heartbeat();

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
        : activatedListener(NULL), deactivatedListener(NULL),
          resetListener(NULL), abortingListener(NULL),
          finalizeListener(NULL), m_coc(coc) {}
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
        : portAddListener(NULL), portRemoveListener(NULL),
          portConnectListener(NULL), portDisconnectListener(NULL),
          m_coc(coc) {}
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
                     ::RTC::ConnectorProfile& pprof, ReturnCode_t ret)
      {
        if (ret == RTC::RTC_OK)
          {
            onGeneric("CONNECT:", portname);
          }
      }
      void onDisconnect(const char* portname,
                        ::RTC::ConnectorProfile& pprof, ReturnCode_t ret)
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
                     const std::string& msg,
                     coil::TimeValue interval)
        : m_coc(coc), m_msg(msg), m_interval(interval),
          m_last(coil::gettimeofday())
      {
      }
      virtual ~DataPortAction() {}

      virtual ReturnCode operator()(ConnectorInfo& info,
                                    ByteData& data)
      {
        coil::TimeValue curr = coil::gettimeofday();
        coil::TimeValue intvl = curr - m_last;
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
      coil::TimeValue m_interval;
      coil::TimeValue m_last;
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
        : ecAttached(NULL), ecDetached(NULL), ecRatechanged(NULL),
          ecStartup(NULL), ecShutdown(NULL),
          m_coc(coc) {}
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
      ExecutionContextActionListener* ecAttached;
      ExecutionContextActionListener* ecDetached;
      PostComponentActionListener* ecRatechanged;
      PostComponentActionListener* ecStartup;
      PostComponentActionListener* ecShutdown;
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
        : updateConfigParamListener(NULL), setConfigSetListener(NULL),
          addConfigSetListener(NULL), updateConfigSetListener(NULL),
          removeConfigSetListener(NULL), activateConfigSetListener(NULL),
          m_coc(coc) {}
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
      ConfigurationParamListener*   updateConfigParamListener;
      ConfigurationSetListener*     setConfigSetListener;
      ConfigurationSetListener*     addConfigSetListener;
      ConfigurationSetNameListener* updateConfigSetListener;
      ConfigurationSetNameListener* removeConfigSetListener;
      ConfigurationSetNameListener* activateConfigSetListener;

    private:
      ComponentObserverConsumer& m_coc;
    };



    RTC::RTObject_impl* m_rtobj;
    SDOPackage::ServiceProfile m_profile;
    CorbaConsumer<OpenRTM::ComponentObserver> m_observer;

    bool m_observed[OpenRTM::STATUS_KIND_NUM];

    // ComponentProfile
    CompStatMsg m_compstat;

    // PortProfile
    PortAction m_portaction;
    coil::TimeValue m_inportInterval;
    coil::TimeValue m_outportInterval;

    // Execution Context
    ECAction m_ecaction;
    ConfigAction m_configMsg;

    // Heartbeat
    coil::TimeValue m_interval;
    bool m_heartbeat;
    ListenerId m_hblistenerid;

    // このタイマーはいずれグローバルなタイマにおきかえる
    coil::Timer m_timer;
    coil::Mutex mutex;

    std::vector<DataPortAction*> m_recievedactions;
    std::vector<DataPortAction*> m_sendactions;

  };

}; // namespace RTC

extern "C"
{
  DLL_EXPORT void ComponentObserverConsumerInit();
};

#endif // RTC_COMPONENTOBSERVERCONSUMER_H


