// -*- C++ -*-
/*!
 * @file PortBase.cpp
 * @brief RTC's Port base class
 * @date $Date: 2008-01-14 10:19:42 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
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

#include <cassert>
#include <memory>
#include <coil/UUID.h>
#include <rtm/PortBase.h>
#include <rtm/PortCallback.h>
#include <rtm/CORBA_RTCUtil.h>

namespace RTC
{
  //============================================================
  // class PortBase
  //============================================================
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  PortBase::PortBase(const char* name)
    : rtclog(name),
      m_ownerInstanceName("unknown"),
      m_connectionLimit(-1),
      m_onPublishInterfaces(nullptr),
      m_onSubscribeInterfaces(nullptr),
      m_onConnected(nullptr),
      m_onUnsubscribeInterfaces(nullptr),
      m_onDisconnected(nullptr),
      m_onConnectionLost(nullptr),
      m_portconnListeners(nullptr),
	  m_directport(nullptr)
  {
    m_objref = this->_this();
    // Now Port name is <instance_name>.<port_name>. r1648
    std::string portname(m_ownerInstanceName);
    portname += ".";
    portname += name;

    m_profile.name = CORBA::string_dup(portname.c_str());
    m_profile.interfaces.length(0);
    m_profile.port_ref = m_objref;
    m_profile.connector_profiles.length(0);
    m_profile.owner = RTC::RTObject::_nil();
    m_profile.properties.length(0);
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  PortBase::~PortBase()
  {
    RTC_TRACE(("~PortBase()"));
    try
      {
        PortableServer::ObjectId_var oid = _default_POA()->servant_to_id(this);
        _default_POA()->deactivate_object(oid);
      }
    catch (PortableServer::POA::ServantNotActive &e)
      {
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("%s", e._name()));
#endif
      }
    catch (PortableServer::POA::WrongPolicy &e)
      {
#ifdef ORB_IS_ORBEXPRESS
        oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("%s", e._name()));
#endif
      }
    catch (...)
      {
        RTC_ERROR(("Unknown exception caught."));
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] PortProfileを取得する
   * @else
   * @brief [CORBA interface] Get the PortProfile of the Port
   * @endif
   */
  PortProfile* PortBase::get_port_profile()
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("get_port_profile()"));

    updateConnectors();
    Guard gaurd(m_profile_mutex);
    PortProfile_var prof;
    prof = new PortProfile(m_profile);
    return prof._retn();
  }

  /*!
   * @if jp
   * @brief [Local interface] PortProfile を取得する。
   * @else
   * @brief Get the PortProfile of the Port
   * @endif
   */
  const PortProfile& PortBase::getPortProfile() const
  {
    RTC_TRACE(("getPortProfile()"));

    return m_profile;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConnectorProfileListを取得する
   * @else
   * @brief [CORBA interface] Get the ConnectorProfileList of the Port
   * @endif
   */
  ConnectorProfileList* PortBase::get_connector_profiles()
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("get_connector_profiles()"));

    updateConnectors();

    Guard gaurd(m_profile_mutex);
    ConnectorProfileList_var conn_prof;
    conn_prof = new ConnectorProfileList(m_profile.connector_profiles);
    return conn_prof._retn();
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConnectorProfile を取得する
   * @else
   * @brief [CORBA interface] Get the ConnectorProfile
   * @endif
   */
  ConnectorProfile* PortBase::get_connector_profile(const char* connector_id)
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("get_connector_profile(%s)", connector_id));

    updateConnectors();

    Guard gaurd(m_profile_mutex);
    CORBA::Long index(findConnProfileIndex(connector_id));

    if (index < 0)
      {
        ConnectorProfile_var conn_prof;
        conn_prof = new ConnectorProfile();
        return conn_prof._retn();
      }
    ConnectorProfile_var conn_prof;
    conn_prof = new ConnectorProfile(m_profile.connector_profiles[index]);
    return conn_prof._retn();
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Port の接続を行う
   * @else
   * @brief [CORBA interface] Connect the Port
   * @endif
   */
  ReturnCode_t PortBase::connect(ConnectorProfile& connector_profile)
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("connect()"));
    if (isEmptyId(connector_profile))
      {
        Guard gurad(m_profile_mutex);
        // "connector_id" stores UUID which is generated at the initial Port
        // in connection process.
        setUUID(connector_profile);
        assert(!isExistingConnId(connector_profile.connector_id));
      }
    else
      {
        Guard gurad(m_profile_mutex);
        if (isExistingConnId(connector_profile.connector_id))
          {
            RTC_ERROR(("Connection already exists."));
            return RTC::PRECONDITION_NOT_MET;
          }
      }

    try
      {
        RTC::PortService_ptr p;
        p = connector_profile.ports[static_cast<CORBA::ULong>(0)];
        ReturnCode_t ret = p->notify_connect(connector_profile);
        if (ret != RTC::RTC_OK)
          {
            RTC_ERROR(("Connection failed. cleanup."));
            disconnect(connector_profile.connector_id);
          }
        return ret;
      }
    catch (...)
      {
        return RTC::BAD_PARAMETER;
      }
    return RTC::RTC_ERROR;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Port の接続通知を行う
   * @else
   * @brief [CORBA interface] Notify the Ports connection
   * @endif
   */
  ReturnCode_t PortBase::notify_connect(ConnectorProfile& connector_profile)
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("notify_connect()"));
    Guard guard(m_connectorsMutex);



	Properties prop;
	NVUtil::copyToProperties(prop, connector_profile.properties);
	bool default_value = coil::toBool(m_properties["allow_dup_connection"], "YES", "NO", false);

	if (!coil::toBool(prop.getProperty("dataport.allow_dup_connection"), "YES", "NO", default_value))
	{
		for (unsigned int i = 0; i < connector_profile.ports.length(); i++)
		{
			if (!getPortRef()->_is_equivalent(connector_profile.ports[i]))
			{
				bool ret = CORBA_RTCUtil::already_connected(connector_profile.ports[i], m_objref);
				if(ret)
				{
					return RTC::PRECONDITION_NOT_MET;
				}
			}
		}
	}



    ReturnCode_t retval[] = {RTC::RTC_OK, RTC::RTC_OK, RTC::RTC_OK};

    onNotifyConnect(getName(), connector_profile);

    // publish owned interface information to the ConnectorProfile
    retval[0] = publishInterfaces(connector_profile);
    if (retval[0] != RTC::RTC_OK)
      {
        RTC_ERROR(("publishInterfaces() in notify_connect() failed."));
      }
    onPublishInterfaces(getName(), connector_profile, retval[0]);
    if (m_onPublishInterfaces != nullptr)
      {
        (*m_onPublishInterfaces)(connector_profile);
      }


    // call notify_connect() of the next Port
    retval[1] = connectNext(connector_profile);
    if (retval[1] != RTC::RTC_OK)
      {
        RTC_ERROR(("connectNext() in notify_connect() failed."));
      }
    onConnectNextport(getName(), connector_profile, retval[1]);

    // subscribe interface from the ConnectorProfile's information

    if (m_onSubscribeInterfaces != nullptr)
      {
        (*m_onSubscribeInterfaces)(connector_profile);
      }

    retval[2] = subscribeInterfaces(connector_profile);
    if (retval[2] != RTC::RTC_OK)
      {
        RTC_ERROR(("subscribeInterfaces() in notify_connect() failed."));
      }
    onSubscribeInterfaces(getName(), connector_profile, retval[2]);

    RTC_PARANOID(("%d connectors are existing",
                  m_profile.connector_profiles.length()));

    Guard gurad(m_profile_mutex);
    CORBA::Long index(findConnProfileIndex(connector_profile.connector_id));
    if (index < 0)
      {
        CORBA_SeqUtil::push_back(m_profile.connector_profiles,
                                 connector_profile);
        RTC_PARANOID(("New connector_id. Push backed."));
      }
    else
      {
        m_profile.connector_profiles[index] = connector_profile;
        RTC_PARANOID(("Existing connector_id. Updated."));
      }

    for (int i(0), len(sizeof(retval)/sizeof(ReturnCode_t)); i < len; ++i)
      {
        if (retval[i] != RTC::RTC_OK)
          {
            onConnected(getName(), connector_profile, retval[i]);
            return retval[i];
          }
      }

    // connection established without errors
    if (m_onConnected != nullptr)
      {
        (*m_onConnected)(connector_profile);
      }
    onConnected(getName(), connector_profile, RTC::RTC_OK);
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief Interface情報を公開する
   * @else
   * @brief Publish interface information
   * @endif
   */
  ReturnCode_t PortBase::_publishInterfaces()
  {
    if (!(m_connectionLimit < 0))
      {
        if (static_cast<::CORBA::ULong>(m_connectionLimit) <=
            m_profile.connector_profiles.length())
          {
            RTC_PARANOID(("Connected number has reached the limitation."));
            RTC_PARANOID(("Can connect the port up to %d ports.",
                      m_connectionLimit));
            RTC_PARANOID(("%d connectors are existing",
                      m_profile.connector_profiles.length()));
            return RTC::RTC_ERROR;
          }
      }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Port の接続を解除する
   * @else
   * @brief [CORBA interface] Disconnect the Port
   * @endif
   */
  ReturnCode_t PortBase::disconnect(const char* connector_id)
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("disconnect(%s)", connector_id));

    CORBA::Long index(findConnProfileIndex(connector_id));
    if (index < 0)
      {
        RTC_ERROR(("Invalid connector id: %s", connector_id));
        return RTC::BAD_PARAMETER;
      }

    ConnectorProfile prof;
    { // lock and copy profile
      Guard guard(m_profile_mutex);
      prof = m_profile.connector_profiles[index];
    }

    if (prof.ports.length() < 1)
      {
        RTC_FATAL(("ConnectorProfile has empty port list."));
        return RTC::PRECONDITION_NOT_MET;
      }

    for (CORBA::ULong i(0), len(prof.ports.length()); i < len; ++i)
      {
        RTC::PortService_var p(prof.ports[i]);
        try
          {
            return p->notify_disconnect(connector_id);
          }
        catch (CORBA::SystemException &e)
          {
#ifndef ORB_IS_RTORB
#ifdef ORB_IS_ORBEXPRESS
            oe_out << e << oe_endl << oe_flush;
#else
            RTC_WARN(("Exception caught: minor code(%d).", e.minor()));;
#endif
#else // ORB_IS_RTORB
            RTC_WARN(("Exception caught"));
#endif  // ORB_IS_RTORB
            continue;
          }
        catch (...)
          {
            RTC_WARN(("Unknown exception caught."));
            continue;
          }
      }
    RTC_ERROR(("notify_disconnect() for all ports failed."));
    return RTC::RTC_ERROR;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Port の接続解除通知を行う
   * @else
   * @brief [CORBA interface] Notify the Ports disconnection
   * @endif
   */
  ReturnCode_t PortBase::notify_disconnect(const char* connector_id)
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("notify_disconnect(%s)", connector_id));
    Guard guard(m_connectorsMutex);
    Guard gaurd(m_profile_mutex);

    // find connector_profile
    CORBA::Long index(findConnProfileIndex(connector_id));
    if (index < 0)
      {
        RTC_ERROR(("Invalid connector id: %s", connector_id));
        return RTC::BAD_PARAMETER;
      }

    ConnectorProfile& prof(m_profile.connector_profiles[static_cast<CORBA::ULong>(index)]);
    onNotifyDisconnect(getName(), prof);

    ReturnCode_t retval(disconnectNext(prof));
    onDisconnectNextport(getName(), prof, retval);

    if (m_onUnsubscribeInterfaces != nullptr)
      {
        (*m_onUnsubscribeInterfaces)(prof);
      }
    onUnsubscribeInterfaces(getName(), prof);
    unsubscribeInterfaces(prof);

    if (m_onDisconnected != nullptr)
      {
        (*m_onDisconnected)(prof);
      }
// Why RtORB does not use CORBA_SeqUtil?
#ifndef ORB_IS_RTORB
    CORBA_SeqUtil::erase(m_profile.connector_profiles, index);
#else  // ORB_IS_RTORB
    CORBA::ULong len(m_profile.connector_profiles.length());
    if (index < (CORBA::Long)len)
      {
        for (CORBA::ULong i(index); i < len - 1; ++i)
          {
            m_profile.connector_profiles[i] =
              m_profile.connector_profiles[i + 1];
          }
        m_profile.connector_profiles._length = len-1;
      }
#endif  // ORB_IS_RTORB
    onDisconnected(getName(), prof, retval);
    return retval;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Port の全接続を解除する
   * @else
   * @brief [CORBA interface] Disconnect the All Ports
   * @endif
   */
  ReturnCode_t PortBase::disconnect_all()
    throw (CORBA::SystemException)
  {
    RTC_TRACE(("disconnect_all()"));

    ::RTC::ConnectorProfileList plist;
    {
      Guard gaurd(m_profile_mutex);
      plist = m_profile.connector_profiles;
    }

    RTC::ReturnCode_t retcode(::RTC::RTC_OK);
    CORBA::ULong len(plist.length());
    RTC_DEBUG(("disconnecting %d connections.", len));
    for (CORBA::ULong i(0); i < len; ++i)
      {
        ReturnCode_t tmpret;
        tmpret = this->disconnect(plist[i].connector_id);
        if (tmpret != RTC::RTC_OK) retcode = tmpret;
      }

    return retcode;
  }

  //============================================================
  // Local operations
  //============================================================
  /*!
   * @if jp
   * @brief Port の名前を設定する
   * @else
   * @brief Set the name of this Port
   * @endif
   */
  void PortBase::setName(const char* name)
  {
    RTC_TRACE(("setName(%s)", name));
    Guard guard(m_profile_mutex);
    m_profile.name = CORBA::string_dup(name);
    rtclog.setName(name);
  }

  /*!
   * @if jp
   * @brief Port の名前を取得する
   * @else
   * @brief Get the name of this Port
   * @return The name of this Port.
   * @endif
   */
  const char* PortBase::getName() const
  {
    RTC_TRACE(("getName() = %s", (const char*)m_profile.name));
    return m_profile.name;
  }

  /*!
   * @if jp
   * @brief PortProfileを取得する
   * @else
   * @brief Get the PortProfile of the Port
   * @endif
   */
  const PortProfile& PortBase::getProfile() const
  {
    RTC_TRACE(("getProfile()"));
    Guard guard(m_profile_mutex);
    return m_profile;
  }

  /*!
   * @if jp
   * @brief Port のオブジェクト参照を設定する
   * @else
   * @brief Set the object reference of the Port
   * @endif
   */
  void PortBase::setPortRef(PortService_ptr port_ref)
  {
    RTC_TRACE(("setPortRef()"));
    Guard gurad(m_profile_mutex);
    m_profile.port_ref = port_ref;
  }

  /*!
   * @if jp
   * @brief Port のオブジェクト参照を取得する
   * @else
   * @brief Get the object reference of the Port
   * @endif
   */
  PortService_ptr PortBase::getPortRef() const
  {
    RTC_TRACE(("getPortRef()"));
    Guard gurad(m_profile_mutex);
#ifdef ORB_IS_ORBEXPRESS
    return m_profile.port_ref.in();
#else
    return m_profile.port_ref;
#endif
  }

  /*!
   * @if jp
   * @brief Port の owner の RTObject を指定する
   * @else
   * @brief Set the owner RTObject of the Port
   * @endif
   */
  void PortBase::setOwner(RTObject_ptr owner)
  {
    RTC::ComponentProfile_var prof = owner->get_component_profile();

    m_ownerInstanceName = prof->instance_name;
    RTC_TRACE(("setOwner(%s)", m_ownerInstanceName.c_str()));

    {
      Guard gurad(m_profile_mutex);
      std::string portname((const char*)m_profile.name);
      coil::vstring p(coil::split(portname, "."));
      // Now Port name is <instance_name>.<port_name>. r1648
      portname = m_ownerInstanceName +"."+ p.back();

      m_profile.owner = RTC::RTObject::_duplicate(owner);
      m_profile.name = CORBA::string_dup(portname.c_str());
    }
  }

  // OnConnect系コールバック (接続に起因するイベントによりコールされる)
  void PortBase::setOnPublishInterfaces(ConnectionCallback* on_publish)
  {
    m_onPublishInterfaces = on_publish;
  }

  void PortBase::setOnSubscribeInterfaces(ConnectionCallback* on_subscribe)
  {
    m_onSubscribeInterfaces = on_subscribe;
  }

  void PortBase::setOnConnected(ConnectionCallback* on_connected)
  {
    m_onConnected = on_connected;
  }

  void PortBase::setOnUnsubscribeInterfaces(ConnectionCallback* on_unsubscribe)
  {
    m_onUnsubscribeInterfaces = on_unsubscribe;
  }

  void PortBase::setOnDisconnected(ConnectionCallback* on_disconnected)
  {
    m_onDisconnected = on_disconnected;
  }

  void PortBase::setOnConnectionLost(ConnectionCallback* on_connection_lost)
  {
    m_onConnectionLost = on_connection_lost;
  }

  void PortBase::
  setPortConnectListenerHolder(PortConnectListeners* portconnListeners)
  {
    m_portconnListeners = portconnListeners;
  }


  //============================================================
  // protected operations
  //============================================================
  /*!
   * @if jp
   * @brief 次の Port に対して notify_connect() をコールする
   * @else
   * @brief Call notify_connect() for the next Port
   * @endif
   */
  ReturnCode_t PortBase::connectNext(ConnectorProfile& connector_profile)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(connector_profile.ports,
                                find_port_ref(m_profile.port_ref));

    if (index < 0) return RTC::BAD_PARAMETER;

    if (++index < static_cast<CORBA::Long>(connector_profile.ports.length()))
      {
        RTC::PortService_ptr p;
        p = connector_profile.ports[index];
        return p->notify_connect(connector_profile);
      }
    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief 次の Port に対して notify_disconnect() をコールする
   * @else
   * @brief Call notify_disconnect() for the next Port
   * @endif
   */
  ReturnCode_t PortBase::disconnectNext(ConnectorProfile& cprof)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(cprof.ports,
                                find_port_ref(m_profile.port_ref));
    if (index < 0)
      {
        return RTC::BAD_PARAMETER;
      }
    if (static_cast<CORBA::ULong>(index) == cprof.ports.length() - 1)
      {
        return RTC::RTC_OK;
      }

    CORBA::ULong len = cprof.ports.length();

    ++index;
    for (CORBA::ULong i(static_cast<CORBA::ULong>(index)); i < len; ++i)
      {
        RTC::PortService_var p;
        p = cprof.ports[i];
        try
          {
            return p->notify_disconnect(cprof.connector_id);
          }
        catch (CORBA::SystemException& e)
          {
#ifndef ORB_IS_RTORB
#ifdef ORB_IS_ORBEXPRESS
            oe_out << e << oe_endl << oe_flush;
#else
            RTC_WARN(("Exception caught: minor code.", e.minor()));
#endif
#else // ORB_IS_RTORB
            RTC_WARN(("Exception caught"));
#endif  // ORB_IS_RTORB
            continue;
          }
        catch (...)
          {
            RTC_WARN(("Unknown exception caught."));
            continue;
          }
      }

    return RTC::RTC_ERROR;
  }
  /*!
   * @if jp
   * @brief 接続の最大数を設定する。
   * @else
   * @brief Set the maximum number of connections
   * @endif
   */
  void PortBase::setConnectionLimit(int limit_value)
  {
    m_connectionLimit = limit_value;
  }

  //============================================================
  // protected utility functions
  //============================================================
  /*!
   * @if jp
   * @brief ConnectorProfile の connector_id フィールドが空かどうか判定
   * @else
   * @brief Check whether connector_id of ConnectorProfile is empty
   * @endif
   */
  bool PortBase::isEmptyId(const ConnectorProfile& connector_profile) const
  {
    return connector_profile.connector_id[static_cast<CORBA::ULong>(0)] == 0;
  }


  /*!
   * @if jp
   * @brief UUIDを生成する
   * @else
   * @brief Generate the UUID
   * @endif
   */
  const std::string PortBase::getUUID() const
  {
    coil::UUID_Generator uugen;
    uugen.init();
    std::unique_ptr<coil::UUID> uuid(uugen.generateUUID(2, 0x01));

    return std::string(uuid->to_string());
  }

  /*!
   * @if jp
   * @brief UUIDを生成し ConnectorProfile にセットする
   * @else
   * @brief Generate the UUID and set it to the ConnectorProfile
   * @endif
   */
  void PortBase::setUUID(ConnectorProfile& connector_profile) const
  {
    connector_profile.connector_id = CORBA::string_dup(getUUID().c_str());
    assert(connector_profile.connector_id[(CORBA::ULong)0] != 0);
  }

  /*!
   * @if jp
   * @brief id が既存の ConnectorProfile のものかどうか判定する
   * @else
   * @brief Check whether the given id exists in stored ConnectorProfiles
   * @endif
   */
  bool PortBase::isExistingConnId(const char* id)
  {
    return CORBA_SeqUtil::find(m_profile.connector_profiles,
                               find_conn_id(id)) >= 0;
  }

  /*!
   * @if jp
   * @brief id を持つ ConnectorProfile を探す
   * @else
   * @brief Find ConnectorProfile with id
   * @endif
   */
  ConnectorProfile PortBase::findConnProfile(const char* id)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_profile.connector_profiles,
                                find_conn_id(id));
    return m_profile.connector_profiles[index];
  }

  /*!
   * @if jp
   * @brief id を持つ ConnectorProfile を探す
   * @else
   * @brief Find ConnectorProfile with id
   * @endif
   */
  CORBA::Long PortBase::findConnProfileIndex(const char* id)
  {
    return CORBA_SeqUtil::find(m_profile.connector_profiles,
                               find_conn_id(id));
  }

  /*!
   * @if jp
   * @brief ConnectorProfile の追加もしくは更新
   * @else
   * @brief Append or update the ConnectorProfile list
   * @endif
   */
  void
  PortBase::updateConnectorProfile(const ConnectorProfile& connector_profile)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_profile.connector_profiles,
                                find_conn_id(connector_profile.connector_id));

    if (index < 0)
      {
        CORBA_SeqUtil::push_back(m_profile.connector_profiles,
                                 connector_profile);
      }
    else
      {
        m_profile.connector_profiles[index] = connector_profile;
      }
  }

  /*!
   * @if jp
   * @brief ConnectorProfile を削除する
   * @else
   * @brief Delete the ConnectorProfile
   * @endif
   */
  bool PortBase::eraseConnectorProfile(const char* id)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_profile.connector_profiles,
                                find_conn_id(id));
    if (index < 0) return false;

    CORBA_SeqUtil::erase(m_profile.connector_profiles, index);
    return true;
  }

  /*!
   * @if jp
   * @brief PortInterfaceProfile に インターフェースを登録する
   * @else
   * @brief Append an interface to the PortInterfaceProfile
   * @endif
   */
  bool PortBase::appendInterface(const char* instance_name,
                                 const char* type_name,
                                 PortInterfacePolarity pol)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_profile.interfaces,
                                find_interface(instance_name, pol));

    if (index >= 0) return false;
    // setup PortInterfaceProfile
    PortInterfaceProfile prof;
    prof.instance_name = CORBA::string_dup(instance_name);
    prof.type_name     = CORBA::string_dup(type_name);
    prof.polarity      = pol;
    CORBA_SeqUtil::push_back(m_profile.interfaces, prof);

    return true;
  }

  /*!
   * @if jp
   * @brief PortInterfaceProfile からインターフェース登録を削除する
   * @else
   * @brief Delete the interface registration from the PortInterfaceProfile
   * @endif
   */
  bool PortBase::deleteInterface(const char* name, PortInterfacePolarity pol)
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_profile.interfaces,
                                find_interface(name, pol));

    if (index < 0) return false;

    CORBA_SeqUtil::erase(m_profile.interfaces, index);
    return true;
  }

  /*!
   * @if jp
   * @brief 存在しないポートをdisconnectする。
   * @else
   * @brief Disconnect ports that doesn't exist. 
   * @endif
   */
  void PortBase::updateConnectors()
  {
    std::vector<std::string> connector_ids;
    {
// Why RtORB copies ConnectorProfile?
#ifndef ORB_IS_RTORB
      Guard guard(m_profile_mutex);
      ConnectorProfileList& clist(m_profile.connector_profiles);

      for (CORBA::ULong i(0); i < clist.length(); ++i)
        {
          if (!checkPorts(clist[i].ports))
            {
              const char* id(clist[i].connector_id);
              connector_ids.emplace_back(id);
              RTC_WARN(("Dead connection: %s", id));
            }
        }
#else  // ORB_IS_RTORB
      ConnectorProfileList* clist;
      clist = new ConnectorProfileList(m_profile.connector_profiles);

      for (CORBA::ULong i(0); i < clist->length(); ++i)
        {
          if (!checkPorts((*clist)[i].ports))
            {
              const char* id((*clist)[i].connector_id);
              connector_ids.push_back(id);
              RTC_WARN(("Dead connection: %s", id));
            }
        }
      delete clist;
#endif  // ORB_IS_RTORB
    }

    for (auto & connector_id : connector_ids)
      {
        this->disconnect(connector_id.c_str());
      }
  }

  /*!
   * @if jp
   * @brief ポートの存在を確認する。
   * @else
   * @brief Existence of ports
   * @endif
   */
#ifndef ORB_IS_RTORB
  bool PortBase::checkPorts(::RTC::PortServiceList& ports)
#else  // ORB_IS_RTORB
  bool PortBase::checkPorts(RTC_PortServiceList& ports)
#endif  // ORB_IS_RTORB
  {
    for (CORBA::ULong i(0), len(ports.length()); i < len; ++i)
      {
        try
          {
            if (ports[i]->_non_existent())
              {
                RTC_WARN(("Dead Port reference detected."));
                return false;
              }
          }
        catch (...)
          {
            return false;
          }
      }
    return true;
  }





  /*!
  * @if jp
  * @brief direct通信用ポートオブジェクト取得
  *
  * @return ポートのポインタ
  *
  * @else
  * @brief
  *
  * @return
  *
  * @endif
  */
  DirectPortBase* PortBase::getDirectPort()
  {
	  return m_directport;
  }

} // namespace RTC
