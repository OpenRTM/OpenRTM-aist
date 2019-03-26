// -*- C++ -*-
/*!
 * @file OutPortBase.cpp
 * @brief Output porty base class
 * @date $Date: 2007-12-31 03:08:05 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2003-2008
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

#include <coil/stringutil.h>

#include <rtm/ConnectorBase.h>
#include <rtm/OutPortPushConnector.h>
#include <rtm/OutPortPullConnector.h>
#include <rtm/OutPortBase.h>
#include <rtm/PublisherBase.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>

#include <rtm/InPortBase.h>

namespace RTC
{
  /*!
   * @if jp
   * @brief Provider を削除するための Functor
   * @else
   * @brief Functor to delete Providers
   * @endif
   */
  struct OutPortBase::provider_cleanup
  {
    provider_cleanup()
      : m_factory(OutPortProviderFactory::instance())
    {
    }
    void operator()(OutPortProvider* p)
    {
      m_factory.deleteObject(p);
    }
    OutPortProviderFactory& m_factory;
  };

  /*!
   * @if jp
   * @brief Connector を削除するための Functor
   * @else
   * @brief Functor to delete Connectors
   * @endif
   */
  struct OutPortBase::connector_cleanup
  {
    connector_cleanup()
    {
    }
    void operator()(OutPortConnector* c)
    {
      delete c;
    }
  };


  /*!------------------------------------------------------------
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  OutPortBase::OutPortBase(const char* name, const char* data_type)
    : PortBase(name), m_littleEndian(true)
  {
    RTC_DEBUG(("Port name: %s", name));

    RTC_DEBUG(("setting port.port_type: DataOutPort"));
    addProperty("port.port_type", "DataOutPort");

    RTC_DEBUG(("setting dataport.data_type: %s", data_type));
    addProperty("dataport.data_type", data_type);

    // publisher list
    PublisherFactory& factory(PublisherFactory::instance());
    std::string pubs = coil::flatten(factory.getIdentifiers());

    // blank characters are deleted for RTSE's bug
    coil::eraseBlank(pubs);
    RTC_DEBUG(("available subscription_type: %s",  pubs.c_str()));
    addProperty("dataport.subscription_type", pubs.c_str());
    // FSM4RTC formal/16-04-01 p.25
    // In the FSM4RTC specification, publisher type is defined as "io_mode"
    addProperty("dataport.io_mode", pubs.c_str());

  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  OutPortBase::~OutPortBase()
  {
    RTC_TRACE(("~OutPortBase()"));
    // connector のクリーンナップ
    std::for_each(m_connectors.begin(),
                  m_connectors.end(),
                  connector_cleanup());
  }

  /*!
   * @if jp
   * @brief プロパティの初期化
   * @else
   * @brief Initializing properties
   * @endif
   */
  void OutPortBase::init(coil::Properties& prop)
  {
    RTC_TRACE(("init()"));
    RTC_PARANOID(("given properties:"));
    RTC_DEBUG_STR((prop));

    // merge properties to PortProfile.properties
    m_properties << prop;
    NVList nv;
    NVUtil::copyFromProperties(nv, m_properties);
    CORBA_SeqUtil::push_back_list(m_profile.properties, nv);
    RTC_PARANOID(("updated properties:"));
    RTC_DEBUG_STR((m_properties));

    configure();

    initConsumers();
    initProviders();
    int num(-1);
    if (!coil::stringTo(num,
               m_properties.getProperty("connection_limit", "-1").c_str()))
      {
        RTC_ERROR(("invalid connection_limit value: %s",
                   m_properties.getProperty("connection_limit").c_str()));
      }

    setConnectionLimit(num);
  }

  /*!
   * @if jp
   * @brief プロパティを取得する
   * @else
   * @brief Get properties
   * @endif
   */
  coil::Properties& OutPortBase::properties()
  {
    RTC_TRACE(("properties()"));
    return m_properties;
  }

  /*!
   * @if jp
   * @brief Connector を取得
   * @else
   * @brief Connector list
   * @endif
   */
  const std::vector<OutPortConnector*>& OutPortBase::connectors()
  {
    RTC_TRACE(("connectors(): size = %d", m_connectors.size()));
    return m_connectors;
  }

  /*!
   * @if jp
   * @brief ConnectorProfile を取得
   * @else
   * @brief ConnectorProfile list
   * @endif
   */
  ConnectorInfoList OutPortBase::getConnectorProfiles()
  {
    RTC_TRACE(("getConnectorProfiles(): size = %d", m_connectors.size()));
    ConnectorInfoList profs;
    for (auto & connector : m_connectors)
      {
        profs.push_back(connector->profile());
      }
    return profs;
  }

  /*!
   * @if jp
   * @brief ConnectorId を取得
   * @else
   * @brief ConnectorId list
   * @endif
   */
  coil::vstring OutPortBase::getConnectorIds()
  {
    coil::vstring ids;
    for (auto & connector : m_connectors)
      {
        ids.push_back(connector->id());
      }
    RTC_TRACE(("getConnectorIds(): %s", coil::flatten(ids).c_str()));
    return ids;
  }

  /*!
   * @if jp
   * @brief Connectorの名前を取得
   * @else
   * @brief Connector name list
   * @endif
   */
  coil::vstring OutPortBase::getConnectorNames()
  {
    coil::vstring names;
    for (auto & connector : m_connectors)
      {
        names.push_back(connector->name());
      }
    RTC_TRACE(("getConnectorNames(): %s", coil::flatten(names).c_str()));
    return names;
  }

  /*!
   * @if jp
   * @brief ConnectorをIDで取得
   * @else
   * @brief Getting Connector by ID
   * @endif
   */
  OutPortConnector* OutPortBase::getConnectorById(const char* id)
  {
    RTC_TRACE(("getConnectorById(id = %s)", id));

    std::string sid(id);
    for (auto & connector : m_connectors)
      {
        if (sid  == connector->id())
          {
            return connector;
          }
      }
    RTC_WARN(("ConnectorProfile with the id(%s) not found.", id));
    return nullptr;
  }

  /*!
   * @if jp
   * @brief Connectorを名前で取得
   * @else
   * @brief Getting ConnectorProfile by name
   * @endif
   */
  OutPortConnector* OutPortBase::getConnectorByName(const char* name)
  {
    RTC_TRACE(("getConnectorByName(name = %s)", name));

    std::string sname(name);
    for (auto & connector : m_connectors)
      {
        if (sname  == connector->name())
          {
            return connector;
          }
      }
    RTC_WARN(("ConnectorProfile with the name(%s) not found.", name));
    return nullptr;
  }

  /*!
   * @if jp
   * @brief ConnectorProfileをIDで取得
   * @else
   * @brief Getting ConnectorProfile by name
   * @endif
   */
  bool OutPortBase::getConnectorProfileById(const char* id,
                                            ConnectorInfo& prof)
  {
    RTC_TRACE(("getConnectorProfileById(id = %s)", id));
    OutPortConnector* conn(getConnectorById(id));
    if (conn == nullptr)
      {
        return false;
      }
    prof = conn->profile();
    return true;
  }

  /*!
   * @if jp
   * @brief ConnectorProfileを名前で取得
   * @else
   * @brief Getting ConnectorProfile by name
   * @endif
   */
  bool OutPortBase::getConnectorProfileByName(const char* name,
                                              ConnectorInfo& prof)
  {
    RTC_TRACE(("getConnectorProfileByName(name = %s)", name));
    OutPortConnector* conn(getConnectorByName(name));
    if (conn == nullptr)
      {
        return false;
      }
    prof = conn->profile();
    return true;
  }

  /*!
   * @if jp
   * @brief OutPortを activates する
   * @else
   * @brief Activate all Port interfaces
   * @endif
   */
  void OutPortBase::activateInterfaces()
  {
    RTC_TRACE(("activateInterfaces()"));

    for (auto & connector : m_connectors)
      {
        connector->activate();
      }
  }

  /*!
   * @if jp
   * @brief 全ての Port のインターフェースを deactivates する
   * @else
   * @brief Deactivate all Port interfaces
   * @endif
   */
  void OutPortBase::deactivateInterfaces()
  {
    RTC_TRACE(("deactivateInterfaces()"));

    for (auto & connector : m_connectors)
      {
        connector->deactivate();
      }
  }


  /*!
   * @if jp
   * @brief ConnectorDataListener リスナを追加する
   *
   * バッファ書き込みまたは読み出しイベントに関連する各種リスナを設定する。
   *
   * @else
   * @brief Adding BufferDataListener type listener
   *
   * @endif
   */
  void OutPortBase::
  addConnectorDataListener(ConnectorDataListenerType type,
                           ConnectorDataListener* listener,
                           bool autoclean)
  {
    if (type < CONNECTOR_DATA_LISTENER_NUM)
      {
        RTC_TRACE(("addConnectorDataListener(%s)",
                   ConnectorDataListener::toString(type)));
        m_listeners.connectorData_[type].addListener(listener, autoclean);
        return;
      }
    RTC_ERROR(("addConnectorDataListener(): Unknown Listener Type"));
    return;
  }

  /*!
   * @if jp
   * @brief ConnectorDataListener リスナを削除する
   *
   * @else
   * @brief Removing ConnectorDataListener type listener
   *
   * @endif
   */
  void OutPortBase::
  removeConnectorDataListener(ConnectorDataListenerType type,
                              ConnectorDataListener* listener)
  {
    if (type < CONNECTOR_DATA_LISTENER_NUM)
      {
        RTC_TRACE(("removeConnectorDataListener(%s)",
                   ConnectorDataListener::toString(type)));
        m_listeners.connectorData_[type].removeListener(listener);
        return;
      }
    RTC_ERROR(("removeConnectorDataListener(): Unknown Listener Type"));
    return;
  }

  /*!
   * @if jp
   * @brief ConnectorListener リスナを追加する
   *
   * @else
   * @brief Adding ConnectorListener type listener
   *
   * @endif
   */
  void OutPortBase::addConnectorListener(ConnectorListenerType type,
                                         ConnectorListener* listener,
                                         bool autoclean)
  {
    if (type < CONNECTOR_LISTENER_NUM)
      {
        RTC_TRACE(("addConnectorListener(%s)",
                   ConnectorListener::toString(type)));
        m_listeners.connector_[type].addListener(listener, autoclean);
        return;
      }
    RTC_ERROR(("addConnectorListener(): Unknown Listener Type"));
    return;
  }

  /*!
   * @if jp
   * @brief ConnectorListener リスナを削除する
   *
   * @else
   * @brief Removing ConnectorListener type listener
   *
   * @endif
   */
  void OutPortBase::removeConnectorListener(ConnectorListenerType type,
                                            ConnectorListener* listener)
  {
    if (type < CONNECTOR_LISTENER_NUM)
      {
        RTC_TRACE(("removeConnectorListener(%s)",
                   ConnectorListener::toString(type)));
        m_listeners.connector_[type].removeListener(listener);
        return;
      }
    RTC_ERROR(("removeConnectorListener(): Unknown Listener Type"));
    return;
  }

  /*!
   * @if jp
   * @brief endian 設定がlittleか否か返す
   * @else
   * @brief return it whether endian setting is little
   * @endif
   */
  bool OutPortBase::isLittleEndian()
  {
    return m_littleEndian;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Port の接続を行う
   * @else
   * @brief [CORBA interface] Connect the Port
   * @endif
   */
  ReturnCode_t OutPortBase::connect(ConnectorProfile& connector_profile)
  {
    RTC_TRACE(("OutPortBase::connect()"));

    // endian infomation check
    CORBA::Long index(NVUtil::find_index(connector_profile.properties,
                                       "dataport.serializer.cdr.endian"));
    if (index < 0)
      {
        RTC_TRACE(("ConnectorProfile dataport.serializer.cdr.endian set."));
        // endian infomation set
        CORBA_SeqUtil::push_back(connector_profile.properties,
            NVUtil::newNV("dataport.serializer.cdr.endian", "little,big"));
      }
    return PortBase::connect(connector_profile);
  }


  //======================================================================
  // protected member functions
  //======================================================================
  /*!
   * @if jp
   * @brief OutPortの設定を行う
   * @else
   * @brief Configureing outport
   * @endif
   */
  void OutPortBase::configure()
  {

  }


  /*!
   * @if jp
   * @brief Interface情報を公開する
   * @else
   * @brief Publish interface information
   * @endif
   */
  ReturnCode_t OutPortBase::publishInterfaces(ConnectorProfile& cprof)
  {
    RTC_TRACE(("publishInterfaces()"));

    ReturnCode_t returnvalue = _publishInterfaces();
    if (returnvalue != RTC::RTC_OK)
      {
        return returnvalue;
      }

    // prop: [port.outport].
    coil::Properties prop(m_properties);
    {
      coil::Properties conn_prop;
      NVUtil::copyToProperties(conn_prop, cprof.properties);
      prop << conn_prop.getNode("dataport");  // marge ConnectorProfile
      /*
       * marge ConnectorProfile for buffer property.
       * e.g.
       *  prof[buffer.write.full_policy]
       *       << cprof[dataport.outport.buffer.write.full_policy]
       */
      prop << conn_prop.getNode("dataport.outport");
    }
    RTC_DEBUG(("ConnectorProfile::properties are as follows."));
    RTC_PARANOID_STR((prop));

    /*
     * ここで, ConnectorProfile からの properties がマージされたため、
     * prop["dataflow_type"]: データフロータイプ
     * prop["interface_type"]: インターフェースタイプ
     * などがアクセス可能になる。
     */
    std::string dflow_type(prop["dataflow_type"]);
    coil::normalize(dflow_type);

    if (dflow_type == "push")
      {
        RTC_PARANOID(("dataflow_type = push .... do nothing"));
        return RTC::RTC_OK;
      }
    else if (dflow_type == "pull")
      {
        RTC_PARANOID(("dataflow_type = pull .... create PullConnector"));

        OutPortProvider* provider(createProvider(cprof, prop));
        if (provider == nullptr)
          {
            return RTC::BAD_PARAMETER;
          }

        // create OutPortPullConnector
        OutPortConnector* connector(createConnector(cprof, prop, provider));
        if (connector == nullptr)
          {
            return RTC::RTC_ERROR;
          }

        // connector set
        provider->setConnector(connector);

        RTC_DEBUG(("publishInterface() successfully finished."));
        return RTC::RTC_OK;
      }

    RTC_ERROR(("unsupported dataflow_type"));

    return RTC::BAD_PARAMETER;
  }

  /*!
   * @if jp
   * @brief Interface情報を取得する
   * @else
   * @brief Subscribe interface
   * @endif
   */
  ReturnCode_t OutPortBase::subscribeInterfaces(const ConnectorProfile& cprof)
  {
    RTC_TRACE(("subscribeInterfaces()"));

    // prop: [port.outport].
    coil::Properties prop(m_properties);
    {
      coil::Properties conn_prop;
      NVUtil::copyToProperties(conn_prop, cprof.properties);
      prop << conn_prop.getNode("dataport");  // marge ConnectorProfile
      /*
       * marge ConnectorProfile for buffer property.
       * e.g.
       *  prof[buffer.write.full_policy]
       *       << cprof[dataport.outport.buffer.write.full_policy]
       */
      prop << conn_prop.getNode("dataport.outport");
    }
    RTC_DEBUG(("ConnectorProfile::properties are as follows."));
    RTC_DEBUG_STR((prop));

    bool littleEndian;
    if (!checkEndian(prop, littleEndian))
      {
        RTC_ERROR(("unsupported endian"));
        return RTC::UNSUPPORTED;
      }
    RTC_TRACE(("endian: %s", m_littleEndian ? "little":"big"));

    /*
     * ここで, ConnectorProfile からの properties がマージされたため、
     * prop["dataflow_type"]: データフロータイプ
     * prop["interface_type"]: インターフェースタイプ
     * などがアクセス可能になる。
     */
    std::string& dflow_type(prop["dataflow_type"]);
    coil::normalize(dflow_type);

    if (dflow_type == "push")
      {
        RTC_PARANOID(("dataflow_type is push."));

        // interface
        InPortConsumer* consumer(createConsumer(cprof, prop));
        if (consumer == nullptr)
          {
            return RTC::BAD_PARAMETER;
          }

        // create OutPortPushConnector
        OutPortConnector* connector(createConnector(cprof, prop, consumer));
        if (connector == nullptr)
          {
            return RTC::RTC_ERROR;
          }

        RTC_DEBUG(("subscribeInterfaces() successfully finished."));
        return RTC::RTC_OK;
      }
    else if (dflow_type == "pull")
      {
        RTC_PARANOID(("dataflow_type is pull."));

        // set endian type
        OutPortConnector* conn(getConnectorById(cprof.connector_id));
        if (conn == nullptr)
          {
            RTC_ERROR(("specified connector not found: %s",
                       (const char*)cprof.connector_id));
            return RTC::RTC_ERROR;
          }
        conn->setEndian(littleEndian);
        RTC_DEBUG(("subscribeInterfaces() successfully finished."));
        return RTC::RTC_OK;
      }

    RTC_ERROR(("unsupported dataflow_type: %s", dflow_type.c_str()));
    return RTC::BAD_PARAMETER;
  }

  /*!
   * @if jp
   * @brief 登録されているInterface情報を解除する
   * @else
   * @brief Unsubscribe interface
   * @endif
   */
  void
  OutPortBase::unsubscribeInterfaces(const ConnectorProfile& connector_profile)
  {
    RTC_TRACE(("unsubscribeInterfaces()"));

    std::string id(connector_profile.connector_id);
    RTC_PARANOID(("connector_id: %s", id.c_str()));

    ConnectorList::iterator it(m_connectors.begin());

    while (it != m_connectors.end())
      {
        if (id == (*it)->id())
          {
            // Connector's dtor must call disconnect()
            delete *it;
            m_connectors.erase(it);
            RTC_TRACE(("delete connector: %s", id.c_str()));
            return;
          }
        ++it;
      }
    RTC_ERROR(("specified connector not found: %s", id.c_str()));
    return;
  }

  /*!
   * @if jp
   * @brief OutPort provider の初期化
   * @else
   * @brief OutPort provider initialization
   * @endif
   */
  void OutPortBase::initProviders()
  {
    RTC_TRACE(("initProviders()"));

    // create OutPort providers
    OutPortProviderFactory& factory(OutPortProviderFactory::instance());
    coil::vstring provider_types(factory.getIdentifiers());
    RTC_PARANOID(("available OutPortProviders: %s",
                  coil::flatten(provider_types).c_str()));

#ifndef RTC_NO_DATAPORTIF_ACTIVATION_OPTION
    if ((m_properties.hasKey("provider_types") != nullptr) &&
        coil::normalize(m_properties["provider_types"]) != "all")
      {
        RTC_DEBUG(("allowed providers: %s",
                   m_properties["provider_types"].c_str()));

        coil::vstring temp_types(provider_types);
        provider_types.clear();
        coil::vstring
          active_types(coil::split(m_properties["provider_types"], ","));

        std::sort(temp_types.begin(), temp_types.end());
        std::sort(active_types.begin(), active_types.end());
        std::set_intersection(temp_types.begin(), temp_types.end(),
                              active_types.begin(), active_types.end(),
                              std::back_inserter(provider_types));
      }
#endif

    // OutPortProvider supports "pull" dataflow type
    if (!provider_types.empty())
      {
        RTC_DEBUG(("dataflow_type pull is supported"));
        appendProperty("dataport.dataflow_type", "pull");
        for (auto & provider_type : provider_types)
        {
            appendProperty("dataport.interface_type",
                        provider_type.c_str());
        }
      }

    m_providerTypes = provider_types;
  }

  /*!
   * @if jp
   * @brief InPort consumer の初期化
   * @else
   * @brief InPort consumer initialization
   * @endif
   */
  void OutPortBase::initConsumers()
  {
    RTC_TRACE(("initConsumers()"));

    // create InPort consumers
    InPortConsumerFactory& factory(InPortConsumerFactory::instance());
    coil::vstring consumer_types(factory.getIdentifiers());
    RTC_PARANOID(("available InPortConsumer: %s",
                  coil::flatten(consumer_types).c_str()));

#ifndef RTC_NO_DATAPORTIF_ACTIVATION_OPTION
    if ((m_properties.hasKey("consumer_types") != nullptr) &&
        coil::normalize(m_properties["consumer_types"]) != "all")
      {
        RTC_DEBUG(("allowed consumers: %s",
                   m_properties["consumer_types"].c_str()));

        coil::vstring temp_types(consumer_types);
        consumer_types.clear();
        coil::vstring
          active_types(coil::split(m_properties["consumer_types"], ","));

        std::sort(temp_types.begin(), temp_types.end());
        std::sort(active_types.begin(), active_types.end());
        std::set_intersection(temp_types.begin(), temp_types.end(),
                              active_types.begin(), active_types.end(),
                              std::back_inserter(consumer_types));
      }
#endif

    // InPortConsumer supports "push" dataflow type
    if (!consumer_types.empty())
      {
        RTC_PARANOID(("dataflow_type push is supported"));
        appendProperty("dataport.dataflow_type", "push");
        for (auto & consumer_type : consumer_types)
        {
            appendProperty("dataport.interface_type",
                        consumer_type.c_str());
        }
      }

    m_consumerTypes = consumer_types;
  }

  /*!
   * @if jp
   * @brief シリアライザのエンディアンをチェックする
   * @else
   * @brief Checking endian flag of serializer
   * @endif
   */
  bool OutPortBase::checkEndian(const coil::Properties& prop,
                                bool& littleEndian)
  {
    // old version check
    if (prop.hasKey("serializer") == nullptr)
      {
        littleEndian = true;
        return true;
      }

    // endian type check
    std::string endian_type(prop.getProperty("serializer.cdr.endian", ""));
    RTC_DEBUG(("endian_type: %s", endian_type.c_str()));
    coil::normalize(endian_type);
    std::vector<std::string> endian(coil::split(endian_type, ","));

    if (endian.empty()) { return false; }
    if (endian[0] == "little")
      {
        littleEndian = true;
        return true;
      }
    else if (endian[0] == "big")
      {
        littleEndian = false;
        return true;
      }
    return false;
  }

  /*!
   * @if jp
   * @brief OutPort provider の生成
   * @else
   * @brief OutPort provider creation
   * @endif
   */
  OutPortProvider*
  OutPortBase::createProvider(ConnectorProfile& cprof, coil::Properties& prop)
  {
    if (!prop["interface_type"].empty() &&
        !coil::includes(coil::vstring(m_providerTypes), prop["interface_type"]))
      {
        RTC_ERROR(("no provider found"));
        RTC_DEBUG(("interface_type:  %s", prop["interface_type"].c_str()));
        RTC_DEBUG(("interface_types: %s",
                   coil::flatten(m_providerTypes).c_str()));
        return nullptr;
      }

    RTC_DEBUG(("interface_type: %s", prop["interface_type"].c_str()));
    OutPortProvider* provider;
    provider = OutPortProviderFactory::
      instance().createObject(prop["interface_type"]);

    if (provider != nullptr)
      {
        RTC_TRACE(("provider created"));
        provider->init(prop.getNode("provider"));

#ifndef ORB_IS_RTORB
        if (!provider->publishInterface(cprof.properties))
          {
            RTC_ERROR(("publishing interface information error"));
            OutPortProviderFactory::instance().deleteObject(provider);
            return nullptr;
          }
#else  // ORB_IS_RTORB
        ::SDOPackage::NVList_ptr prop_ref(cprof.properties);
        if (!provider->publishInterface(*prop_ref))
          {
            RTC_ERROR(("publishing interface information error"));
            OutPortProviderFactory::instance().deleteObject(provider);
            return 0;
          }
#endif  // ORB_IS_RTORB
        return provider;
      }

    RTC_ERROR(("provider creation failed"));
    return nullptr;
  }


  /*!
   * @if jp
   * @brief InPort consumer の生成
   * @else
   * @brief InPort consumer creation
   * @endif
   */
  InPortConsumer* OutPortBase::createConsumer(const ConnectorProfile& cprof,
                                              coil::Properties& prop)
  {
    if (!prop["interface_type"].empty() &&
        !coil::includes(coil::vstring(m_consumerTypes), prop["interface_type"]))
      {
        RTC_ERROR(("no consumer found"));
        RTC_DEBUG(("interface_type:  %s", prop["interface_type"].c_str()));
        RTC_DEBUG(("interface_types: %s",
                   coil::flatten(m_consumerTypes).c_str()));
        return nullptr;
      }

    RTC_DEBUG(("interface_type: %s", prop["interface_type"].c_str()));
    InPortConsumer* consumer;
    consumer = InPortConsumerFactory::
      instance().createObject(prop["interface_type"]);

    if (consumer != nullptr)
      {
        RTC_TRACE(("consumer created"));
        consumer->init(prop.getNode("consumer"));

        if (!consumer->subscribeInterface(cprof.properties))
          {
            RTC_ERROR(("interface subscription failed."));
            InPortConsumerFactory::instance().deleteObject(consumer);
            return nullptr;
          }

        return consumer;
      }

    RTC_ERROR(("consumer creation failed"));
    return nullptr;
  }

  /*!
   * @if jp
   * @brief OutPortPushConnector の生成
   * @else
   * @brief OutPortPushConnector creation
   * @endif
   */
  OutPortConnector*
  OutPortBase::createConnector(const ConnectorProfile& cprof,
                               coil::Properties& prop,
                               InPortConsumer* consumer)
  {
#ifndef ORB_IS_RTORB
    ConnectorInfo profile(cprof.name,
                          cprof.connector_id,
                          CORBA_SeqUtil::refToVstring(cprof.ports),
                          prop);
#else  // ORB_IS_RTORB
    ConnectorInfo profile(cprof.name,
                          cprof.connector_id,
                          CORBA_SeqUtil::
                          refToVstring(RTC::PortServiceList(cprof.ports)),
                          prop);
#endif  // ORB_IS_RTORB

    try
      {
        OutPortConnector* connector(nullptr);
        connector = new OutPortPushConnector(profile, consumer, m_listeners);

        if (connector == nullptr)
          {
            RTC_ERROR(("old compiler? new returned 0;"));
            return nullptr;
          }
        RTC_TRACE(("OutPortPushConnector created"));

        // endian type set
        connector->setEndian(m_littleEndian);

        // "interface_type" == "direct"
        if (coil::normalize(prop["interface_type"]) == "direct")
          {
            InPortBase* inport = getLocalInPort(profile);
            if (inport == nullptr)
              {
                RTC_DEBUG(("interface_type is direct, "
                           "but a peer InPort servant could not be obtained."));
                delete connector;
                return nullptr;
              }
            connector->setInPort(inport);
          }
        // end of direct interface_type

        m_connectors.push_back(connector);
        RTC_PARANOID(("connector pushback done: size = %d",
                      m_connectors.size()));
        return connector;
      }
    catch (std::bad_alloc&)
      {
        RTC_ERROR(("OutPortPushConnector creation failed"));
        return nullptr;
      }
  }

  /*!
   * @if jp
   * @brief OutPortPullConnector の生成
   * @else
   * @brief OutPortPullConnector creation
   * @endif
   */
  OutPortConnector*
  OutPortBase::createConnector(const ConnectorProfile& cprof,
                               coil::Properties& prop,
                               OutPortProvider* provider)
  {
    RTC_VERBOSE(("createConnector()"));
#ifndef ORB_IS_RTORB
    ConnectorInfo profile(cprof.name,
                          cprof.connector_id,
                          CORBA_SeqUtil::refToVstring(cprof.ports),
                          prop);
#else  // ORB_IS_RTORB
    ConnectorInfo profile(cprof.name,
                          cprof.connector_id,
                          CORBA_SeqUtil::
                          refToVstring(RTC::PortServiceList(cprof.ports)),
                          prop);
#endif  // ORB_IS_RTORB

    try
      {
        OutPortConnector* connector(nullptr);
        connector = new OutPortPullConnector(profile, provider, m_listeners);

        if (connector == nullptr)
          {
            RTC_ERROR(("old compiler? new returned 0;"));
            return nullptr;
          }
        RTC_TRACE(("OutPortPullConnector created"));


		if (coil::normalize(prop["interface_type"]) == "direct")
		{
			connector->setPullDirectMode();
		}

        m_connectors.push_back(connector);
        RTC_PARANOID(("connector pushback done: size = %d",
                      m_connectors.size()));
        return connector;
      }
    catch (std::bad_alloc&)
      {
        RTC_ERROR(("OutPortPullConnector creation failed"));
        return nullptr;
      }
  }

  /*!
   * @if jp
   * @brief ローカルのピアInPortを取得
   * @else
   * @brief Getting local peer InPort if available
   * @endif
   */
  InPortBase*
  OutPortBase::getLocalInPort(const ConnectorInfo& profile)
  {
    RTC_DEBUG(("Trying direct port connection."));
    CORBA::ORB_var orb = ::RTC::Manager::instance().getORB();
    RTC_DEBUG(("Current connector profile: name=%s, id=%s",
               profile.name.c_str(), profile.id.c_str()));
    // finding peer port object
    for (const auto & port : profile.ports)
      {
        CORBA::Object_var obj;
        obj = orb->string_to_object(port.c_str());
        if (getPortRef()->_is_equivalent(obj)) { continue; }
        RTC_DEBUG(("Peer port found: %s.", port.c_str()));
        try
          {
            PortableServer::POA_var poa = ::RTC::Manager::instance().getPOA();
            InPortBase* inport = dynamic_cast<InPortBase*>
              (poa->reference_to_servant(obj));
            RTC_DEBUG(("InPortBase servant pointer is obtained."));
            return inport;
          }
        catch (...)
          {
            RTC_DEBUG(("Peer port might be a remote port"));
          }
      }
    return nullptr;
  }

  ReturnCode_t OutPortBase::notify_connect(ConnectorProfile& connector_profile)
  {
	  Properties prop;
	  NVUtil::copyToProperties(prop, connector_profile.properties);

	  Properties node = prop.getNode("dataport.outport");

	  Properties portprop(m_properties);

	  node << portprop;

	  

	  NVUtil::copyFromProperties(connector_profile.properties, prop);

	  std::string _str = node["fan_out"];
	  unsigned int value = 100;

	  coil::stringTo<unsigned int>(value, _str.c_str());

	  if (value <= m_connectors.size())
	  {
		  return RTC::PRECONDITION_NOT_MET;
	  }
	  
	  



	  return PortBase::notify_connect(connector_profile);
  }

  ConnectorListeners& OutPortBase::getListeners()
  {
	  return m_listeners;
  }

} // namespace RTC
