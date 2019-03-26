// -*- C++ -*-
/*!
 * @file SdoConfiguration.cpp
 * @brief SDO's Configuration implementation class
 * @date $Date: 2008-01-14 07:49:37 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include "rtm/SdoConfiguration.h"

#include <coil/UUID.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/NVUtil.h>
#include <rtm/ExecutionContextBase.h>

#include <memory>
#include <iostream>
#include <vector>
// ACE

namespace SDOPackage
{
  /*!
   * @if jp
   *
   * @brief SDO ConfigurationSetを プロパティに格納
   *
   * SDO ConfigurationSetを RTC用プロパティに格納する。
   *
   * @param prop 格納先プロパティ
   * @param conf SDO ConfigurationSet
   *
   * @else
   *
   * @brief Store SDO ConfigurationSet to properties
   *
   * Store SDO ConfigurationSet to properties for RTC.
   *
   * @param prop Properties to store
   * @param conf SDO ConfigurationSet
   *
   * @endif
   */
  void
  toProperties(coil::Properties& prop, const SDOPackage::ConfigurationSet& conf)
  {
    NVUtil::copyToProperties(prop, conf.configuration_data);
  }

  /*!
   * @if jp
   *
   * @brief プロパティを SDO ConfigurationSetに変換
   *
   * RTC用プロパティをSDO ConfigurationSetに変換する。
   *
   * @param conf 格納先 SDO ConfigurationSet
   * @param prop プロパティ
   *
   * @else
   *
   * @brief Convert properties into SDO ConfigurationSet
   *
   * Convert properties for RTC into SDO ConfigurationSet.
   *
   * @param conf SDO ConfigurationSet to store
   * @param prop Properties
   *
   * @endif
   */
  void
  toConfigurationSet(SDOPackage::ConfigurationSet& conf,
                     const coil::Properties& prop)
  {
#ifndef ORB_IS_RTORB
    conf.description = CORBA::string_dup(prop["description"].c_str());
    conf.id = CORBA::string_dup(prop.getName());
#else  // ORB_IS_RTORB
    conf.description = reinterpret_cast<char *>(prop["description"].c_str());
    conf.id = reinterpret_cast<char *>(prop.getName());
#endif  // ORB_IS_RTORB
    NVUtil::copyFromProperties(conf.configuration_data, prop);
  }

  //============================================================
  // Ctor and Dtor
  //============================================================

  /* @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  Configuration_impl::Configuration_impl(RTC::ConfigAdmin& configsets,
                                         RTC::SdoServiceAdmin& sdoServiceAdmin)
    : rtclog("sdo_config"), m_configsets(configsets),
      m_sdoservice(sdoServiceAdmin)
  {
    m_objref = this->_this();
  }

  /* @if jp
   * @brief 仮想デストラクタ
   * @else
   * @brief Virtual destructor
   * @endif
   */
  Configuration_impl::~Configuration_impl()
  {
  }


  //============================================================
  // Basic Configuration
  //============================================================
  /* @if jp
   * @brief [CORBA interface] SDO の DeviceProfile をセットする
   * @else
   * @brief [CORBA interface] Set DeviceProfile of SDO
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::set_device_profile(const DeviceProfile& dProfile)
  {
    RTC_TRACE(("set_device_profile()"));
    try
      {
        Guard gurad(m_dprofile_mutex);
        m_deviceProfile = dProfile;
      }
    catch (...)
      {
        throw InternalError("Unknown Error");
        // never reach here
        return false;
      }
    return true;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] SDO の ServiceProfile のセット
   * @else
   * @brief [CORBA interface] Set SDO's ServiceProfile
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::add_service_profile(const ServiceProfile& sProfile)
  {
    RTC_TRACE(("add_service_profile()"));
    // SDO specification defines that InvalidParameter() exception
    // is thrown when sProfile is null.
    // But sProfile is reference and it becomes never null.
    // So this operation does not throw InvalidParameter exception.
    //    if (CORBA::is_nil(sProfile.service)) throw InvalidParameter();
    try
      {
        return m_sdoservice.addSdoServiceConsumer(sProfile);
      }
    catch (...)
      {
        throw InternalError("Configuration::set_service_profile");
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Organization の追加
   * @else
   * @brief [CORBA interface] Add Organization
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::add_organization(Organization_ptr org)
  {
    RTC_TRACE(("add_organization()"));
    try
      {
        CORBA_SeqUtil::push_back(m_organizations,
                                 ::SDOPackage::Organization::_duplicate(org));
      }
    catch (...)
      {
        throw InternalError("Configuration::set_service_profile");
        // never reach here
        return false;
      }
    return true;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ServiceProfile の削除
   * @else
   * @brief [CORBA interface] Remove ServiceProfile
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::remove_service_profile(const char* id)
  {
    RTC_TRACE(("remove_service_profile(%s)", id));
    try
      {
        return m_sdoservice.removeSdoServiceConsumer(id);
      }
    catch (...)
      {
        throw InternalError("Configuration::remove_service_profile");
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Organization の参照の削除
   * @else
   * @brief [CORBA interface] Remove the reference of Organization
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::remove_organization(const char* organization_id)
  {
    RTC_TRACE(("remove_organization(%s)", organization_id));
    try
      {
        Guard gurad(m_org_mutex);
        CORBA_SeqUtil::erase_if(m_organizations, org_id(organization_id));
      }
    catch (...)
      {
        throw InternalError("Configuration::remove_organization");
        // never reach here
        return false;
      }
    return true;
  }

  //============================================================
  // Configuration Parameter manipulation
  //============================================================
  /*!
   * @if jp
   * @brief [CORBA interface] 設定パラメータのリストの取得
   * @else
   * @brief [CORBA interface] Get a list of configuration parameters
   * @endif
   */
  ParameterList*
  Configuration_impl::get_configuration_parameters()
  {
    RTC_TRACE(("get_configuration_parameters()"));
    try
      {
        Guard gaurd(m_params_mutex);
        ParameterList_var param;
        param = new ParameterList(m_parameters);
        return param._retn();
      }
    catch (...)
      {
        throw InternalError("Configuration::get_configuration_parameters()");
        // never reach here
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Configuration parameter の値のリストの取得
   * @else
   * @brief [CORBA interface] Get a list of the value of configuration parameters
   * @endif
   */
  NVList*
  Configuration_impl::get_configuration_parameter_values()
  {
    RTC_TRACE(("get_configuration_parameter_values()"));
    Guard guard(m_config_mutex);
    NVList_var nvlist;
    nvlist = new NVList(static_cast<CORBA::ULong>(0));

    /*
      CORBA::Long index;
      index = getActiveConfigIndex();
      if (index >= 0)
      {
      nvlist = new NVList(m_configurations[index].configuration_data);
      }
    */
    return nvlist._retn();
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Configuration parameter の値の取得
   * @else
   * @brief [CORBA interface] Get the value of configuration parameter
   * @endif
   */
  CORBA::Any*
  Configuration_impl::get_configuration_parameter_value(const char* name)
  {
    RTC_TRACE(("get_configuration_parameter_value(%s)", name));
    if (std::string(name).empty()) throw InvalidParameter("Name is empty.");

    //    CORBA::Long index;
    CORBA::Any_var value;
    value = new CORBA::Any();
    /*
      index = getActiveConfigIndex();
      if (index < 0) throw InternalError("No active configuration.");

      CORBA::Long item;
      item = CORBA_SeqUtil::find(m_configurations[index].configuration_data,
      nv_name(name));
      if (item < 0) throw InvalidParameter("No such name.");

      value = new CORBA::Any(m_configurations[index].configuration_data[item].value);
    */
    return value._retn();
  }

  /*!
   * @if jp
   * @brief [CORBA interface] Configuration パラメータの変更
   * @else
   * @brief [CORBA interface] Modify the configuration parameter value
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::set_configuration_parameter(const char* name,
                                                  const CORBA::Any&  /*value*/)
  {
    RTC_TRACE(("set_configuration_parameter(%s, value)", name));
    /*
      if (name == "") throw InvalidParameter("Name is empty.");

      CORBA::Long index(getActiveConfigIndex());
      if (index < 0) throw InternalError("No active config.");

      CORBA::Long item;
      item = CORBA_SeqUtil::find(m_configurations[index].configuration_data,
      nv_name(name));
      if (item < 0) throw InvalidParameter("No such name.");

      m_configurations[index].configuration_data[item].value = value;
    */
    return true;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConfigurationSet リストの取得
   * @else
   * @brief [CORBA interface] Get a list of ConfigurationSet
   * @endif
   */
  ConfigurationSetList*
  Configuration_impl::get_configuration_sets()
  {
    RTC_TRACE(("get_configuration_sets()"));
    try
      {
        Guard guard(m_config_mutex);

        std::vector<coil::Properties*> cf(m_configsets.getConfigurationSets());
        ConfigurationSetList_var config_sets =
          new ConfigurationSetList(static_cast<CORBA::ULong>(cf.size()));
        // Ctor's first arg is max length. Actual length has to be set.
        config_sets->length(static_cast<CORBA::ULong>(cf.size()));

        for (CORBA::ULong i(0), len(static_cast<CORBA::ULong>(cf.size())); i < len; ++i)
          {
            toConfigurationSet(config_sets[i], *(cf[i]));
          }

        return config_sets._retn();
      }
    catch (CORBA::SystemException& e)
      {
#ifndef ORB_IS_RTORB
#ifdef ORB_IS_ORBEXPRESS
	oe_out << e << oe_endl << oe_flush;
#else
        RTC_ERROR(("CORBA::SystemException cought: %s", e._name()));
#endif
#else
        RTC_ERROR(("CORBA::SystemException cought."));
#endif
        throw InternalError("Configuration::get_configuration_sets()");

      }
    catch (...)
      {
        RTC_ERROR(("Unknown exception cought."));
        throw InternalError("Configuration::get_configuration_sets()");
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConfigurationSet の取得
   * @else
   * @brief [CORBA interface] Get a ConfigurationSet
   * @endif
   */
  ConfigurationSet*
  Configuration_impl::get_configuration_set(const char* id)
  {
    RTC_TRACE(("get_configuration_set(%s)", id));
    if (std::string(id).empty()) throw InternalError("ID is empty");
    // Originally getConfigurationSet raises InvalidParameter according to the
    // SDO specification. However, SDO's IDL lacks InvalidParameter.

    Guard guard(m_config_mutex);

    try
      {
        if (!m_configsets.haveConfig(id))
          {
            RTC_ERROR(("No such ConfigurationSet"));
            throw InvalidParameter("No such ConfigurationSet");
          }
      }
    catch(...)
      {
        RTC_ERROR(("Unknown exception"));
        throw InternalError("Unknown exception");
      }

    const coil::Properties& configset(m_configsets.getConfigurationSet(id));

    try
      {
        ConfigurationSet_var config;
        config = new ConfigurationSet();
        toConfigurationSet(config, configset);

        return config._retn();
      }
    catch (...)
      {
        throw InternalError("Configuration::get_configuration_set()");
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConfigurationSet をセットする
   * @else
   * @brief [CORBA interface] Set ConfigurationSet
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::
  set_configuration_set_values(const ConfigurationSet& configuration_set)
  {
    RTC_TRACE(("set_configuration_set_values()"));
    std::string id(configuration_set.id);
    if (id.empty()) throw InvalidParameter("ID is empty.");

    try
      {
        coil::Properties conf(id.c_str());
        toProperties(conf, configuration_set);

        //------------------------------------------------------------
        // Because the format of port-name had been changed from
        // <port_name> to <instance_name>.<port_name>, the following
        // processing was added.  (since r1648)
        if (conf.findNode("exported_ports") != nullptr)
          {
            coil::vstring
              exported_ports(coil::split(conf["exported_ports"], ","));
            std::string exported_ports_str;
            for (size_t i(0), len(exported_ports.size()); i < len; ++i)
              {
                coil::vstring keyval(coil::split(exported_ports[i], "."));
                if (keyval.size() > 2)
                  {
                    exported_ports_str += (keyval[0] + "." + keyval.back());
                  }
                else
                  {
                    exported_ports_str += exported_ports[i];
                  }
                if (i != exported_ports.size() - 1)
                  {
                    exported_ports_str += ",";
                  }
              }
            conf["exported_ports"] = exported_ports_str;
          }
        //------------------------------------------------------------

        return m_configsets.setConfigurationSetValues(conf);
      }
    catch (...)
      {
        throw InternalError("Configuration::set_configuration_set_values()");
      }
    return true;
  }

  /*!
   * @if jp
   * @brief [CORBA interface] アクティブな ConfigurationSet を取得する
   * @else
   * @brief [CORBA interface] Get active ConfigurationSet
   * @endif
   */
  ConfigurationSet*
  Configuration_impl::get_active_configuration_set()
  {
    RTC_TRACE(("get_active_configuration_set()"));
    // activeなConfigurationSetは無い
    if (!m_configsets.isActive()) throw NotAvailable();

    try
      {
        Guard gurad(m_config_mutex);
        // activeなConfigurationSetを返す
        ConfigurationSet_var config;
        config = new ConfigurationSet();
        toConfigurationSet(config, m_configsets.getActiveConfigurationSet());
        return config._retn();
      }
    catch (...)
      {
        throw InternalError("Configuration::get_active_configuration_set()");
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConfigurationSet を追加する
   * @else
   * @brief [CORBA interface] Add ConfigurationSet
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::
  add_configuration_set(const ConfigurationSet& configuration_set)
  {
    RTC_TRACE(("add_configuration_set()"));
    try
      {
        Guard gurad(m_config_mutex);
        const char* config_id(configuration_set.id);
        coil::Properties config(config_id);
        toProperties(config, configuration_set);
        return m_configsets.addConfigurationSet(config);
      }
    catch (...)
      {
        throw InternalError("Configuration::add_configuration_set()");
        return false;
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConfigurationSet を削除する
   * @else
   * @brief [CORBA interface] Remove ConfigurationSet
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::remove_configuration_set(const char* id)
  {
    RTC_TRACE(("remove_configuration_set(%s)", id));
    if (std::string(id).empty())
      throw InvalidParameter("ID is empty.");

    try
      {
        Guard guard(m_config_mutex);
        return m_configsets.removeConfigurationSet(id);
      }
    catch (...)
      {
        throw InternalError("Configuration::remove_configuration_set()");
        return false;
      }
  }

  /*!
   * @if jp
   * @brief [CORBA interface] ConfigurationSet のアクティブ化
   * @else
   * @brief [CORBA interface] Activate ConfigurationSet
   * @endif
   */
  CORBA::Boolean
  Configuration_impl::activate_configuration_set(const char* id)
  {
    RTC_TRACE(("activate_configuration_set(%s)", id));
    if (std::string(id).empty())
      throw InvalidParameter("ID is empty.");

    if (m_configsets.activateConfigurationSet(id))
      {
        return true;
      }
    else
      {
        throw InvalidParameter("Configuration::activate_configuration_set()");
      }
/*
    try
      {
        return m_configsets.activateConfigurationSet(id);
      }
    catch (...)
      {
        throw InternalError("Configuration::activate_configuration_set()");
        return false;
      }
*/
  }

  //============================================================
  // Local interfaces
  //============================================================

  /*!
   * @if jp
   * @brief オブジェクト　リファレンスを取得する
   * @else
   * @brief Get object reference
   * @endif
   */
  Configuration_ptr Configuration_impl::getObjRef()
  {
    return m_objref;
  }

  /*!
   * @if jp
   * @brief SDO の DeviceProfile を取得する
   * @else
   * @brief Get the DeviceProfile of SDO
   * @endif
   */
  const DeviceProfile Configuration_impl::getDeviceProfile()
  {
    return m_deviceProfile;
  }

  /*!
   * @if jp
   * @brief SDO の Organization リストを取得する
   * @else
   * @brief Get a list of Organization of SDO
   * @endif
   */
  const OrganizationList Configuration_impl::getOrganizations()
  {
    return m_organizations;
  }

  /*!
   * @if jp
   * @brief UUIDを生成する
   * @else
   * @brief Generate UUID
   * @endif
   */
  const std::string Configuration_impl::getUUID() const
  {
    coil::UUID_Generator uugen = coil::UUID_Generator();
    uugen.init();
    std::unique_ptr<coil::UUID> uuid(uugen.generateUUID(2, 0x01));

    return uuid->to_string();
  }
} // namespace SDOPackage
