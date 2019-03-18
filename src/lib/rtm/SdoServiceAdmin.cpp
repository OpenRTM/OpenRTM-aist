// -*- C++ -*-
/*!
 * @file SdoServiceAdmin.cpp
 * @brief SDO service administration class
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

#include <coil/UUID.h>
#include <coil/Guard.h>
#include <coil/stringutil.h>

#include <rtm/RTObject.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/SdoServiceAdmin.h>
#include <rtm/SdoServiceProviderBase.h>
#include <rtm/SdoServiceConsumerBase.h>

#include <cstring>

#include <memory>
#include <vector>

namespace RTC
{
  typedef coil::Guard<coil::Mutex> Guard;

  /*!
   * @if jp
   * @brief  ServiceProfile用functor
   * @else
   * @brief  Functor for ServiceProfile
   * @endif
   */
  struct service_id
  {
    explicit service_id(const char* id) : m_id(id) {}
    bool operator()(const SDOPackage::ServiceProfile& s)
    {
      std::string id(s.id);
      return m_id == id;
    }
    const std::string m_id;
  };


  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  SdoServiceAdmin::SdoServiceAdmin(::RTC::RTObject_impl& rtobj)
    : m_rtobj(rtobj), m_allConsumerEnabled(false),
      rtclog("SdoServiceAdmin")
  {
    RTC_TRACE(("SdoServiceAdmin::SdoServiceAdmin(%s)",
               rtobj.getProperties()["instance_name"].c_str()));
  }

  /*!
   * @if jp
   * @brief 仮想デストラクタ
   * @else
   * @brief Virtual destractor
   * @endif
   */
  SdoServiceAdmin::~SdoServiceAdmin()
  {
    for (auto & provider : m_providers)
      {
        provider->finalize();
        delete provider;
      }
    m_providers.clear();

    for (auto & consumer : m_consumers)
      {
        consumer->finalize();
        delete consumer;
      }
    m_consumers.clear();
  }

  void SdoServiceAdmin::init(const coil::Properties& prop)
  {
    initProvider(prop);
    initConsumer(prop);
  }

  void SdoServiceAdmin::initProvider(const coil::Properties& prop)
  {
    //------------------------------------------------------------
    // SDO service provider
    ::coil::vstring enabledProviderTypes
      = ::coil::split(prop["sdo.service.provider.enabled_services"], ",", true);
    RTC_DEBUG(("sdo.service.provider.enabled_services: %s",
               prop["sdo.service.provider.enabled_services"].c_str()));

    ::coil::vstring availableProviderTypes
        = SdoServiceProviderFactory::instance().getIdentifiers();
    coil::Properties tmp;
    tmp["sdo.service.provider.available_services"]
      = coil::flatten(availableProviderTypes);
    m_rtobj.setProperties(tmp);
    RTC_DEBUG(("sdo.service.provider.available_services: %s",
               tmp["sdo.service.provider.available_services"].c_str()));

    // If types include '[Aa][Ll][Ll]', all types enabled in this RTC
    ::coil::vstring activeProviderTypes;
    for (auto & enabledProviderType : enabledProviderTypes)
      {
        std::string provtype(enabledProviderType);
        coil::toLower(provtype);
        if (provtype == "all")
          {
            activeProviderTypes = availableProviderTypes;
            RTC_DEBUG(("sdo.service.provider.enabled_services: ALL"));
            break;
          }
        for (auto & availableProviderType : availableProviderTypes)
          {
            if (availableProviderType == enabledProviderType)
              {
                activeProviderTypes.push_back(availableProviderType);
              }
          }
      }

    SdoServiceProviderFactory& factory(SdoServiceProviderFactory::instance());
    for(auto & activeProviderType : activeProviderTypes)
      {
        SdoServiceProviderBase* svc
          = factory.createObject(activeProviderType);

        SDOPackage::ServiceProfile prof;
        prof.id             = CORBA::string_dup(activeProviderType.c_str());
        prof.interface_type = CORBA::string_dup(activeProviderType.c_str());
        prof.service        = svc->_this();
        std::string propkey = ifrToKey(activeProviderType);
        coil::Properties proptmp;
        proptmp = const_cast<coil::Properties*>(&prop)->getNode(propkey);
        NVUtil::copyFromProperties(prof.properties, proptmp);
        // TODO: return value must be concerned
        if (!svc->init(m_rtobj, prof))
          {
            svc->finalize();
            delete svc;
            continue;
          }
        m_providers.push_back(svc);
      }
  }

  void SdoServiceAdmin::initConsumer(const coil::Properties& prop)
  {
    //------------------------------------------------------------
    // SDO service consumer
    // getting consumer types from RTC's properties

    ::std::string constypes = prop["sdo.service.consumer.enabled_services"];
    m_consumerTypes = ::coil::split(constypes, ",", true);
    RTC_DEBUG(("sdo.service.consumer.enabled_services: %s", constypes.c_str()));

    coil::Properties tmp;
    tmp["sdo.service.consumer.available_services"]
      = coil::flatten(SdoServiceConsumerFactory::instance().getIdentifiers());
    m_rtobj.setProperties(tmp);
    RTC_DEBUG(("sdo.service.consumer.available_services: %s",
               prop["sdo.service.consumer.available_services"].c_str()));

    // If types include '[Aa][Ll][Ll]', all types enabled in this RTC
    for (auto consumerType : m_consumerTypes)
      {
        coil::toLower(consumerType);
        if (consumerType == "all")
          {
            m_allConsumerEnabled = true;
            RTC_DEBUG(("sdo.service.consumer.enabled_services: ALL"));
          }
      }
  }

  /*!
   * @if jp
   * @brief SDO Service Provider の ServiceProfileList を取得する
   * @else
   * @brief Get ServiceProfileList of SDO Service Provider
   * @endif
   */
  SDOPackage::ServiceProfileList* SdoServiceAdmin::getServiceProviderProfiles()
  {
    SDOPackage::ServiceProfileList_var prof
      = new SDOPackage::ServiceProfileList();
    Guard guard(m_provider_mutex);
    prof->length(static_cast<CORBA::Long>(m_providers.size()));
    for (size_t i(0); i < m_providers.size(); ++i)
      {
        prof[static_cast<CORBA::Long>(i)] = m_providers[i]->getProfile();
      }
    return prof._retn();
  }

  /*!
   * @if jp
   * @brief SDO Service Provider の ServiceProfile を取得する
   * @else
   * @brief Get ServiceProfile of an SDO Service Provider
   * @endif
   */
  SDOPackage::ServiceProfile*
  SdoServiceAdmin::getServiceProviderProfile(const char* id)
  {
    std::string idstr(id);
    Guard guard(m_provider_mutex);
    for (auto & provider : m_providers)
      {
        if (idstr == static_cast<const char*>(provider->getProfile().id))
          {
            return new SDOPackage::ServiceProfile(provider->getProfile());
          }
      }
    throw SDOPackage::InvalidParameter();
    return new SDOPackage::ServiceProfile();
  }

  /*!
   * @if jp
   * @brief SDO Service Provider の Service を取得する
   * @else
   * @brief Get ServiceProfile of an SDO Service
   * @endif
   */   
  SDOPackage::SDOService_ptr SdoServiceAdmin::getServiceProvider(const char* id)
  {
    SDOPackage::ServiceProfile_var prof;
    prof = getServiceProviderProfile(id);
    SDOPackage::SDOService_var sdo
      = SDOPackage::SDOService::_duplicate(prof->service);
    return sdo._retn();
  }

  /*!
   * @if jp
   * @brief SDO service provider をセットする
   * @else
   * @brief Set a SDO service provider
   * @endif
   */
  bool SdoServiceAdmin::
  addSdoServiceProvider(const SDOPackage::ServiceProfile& prof,
                        SdoServiceProviderBase* provider)
  {
    RTC_TRACE(("SdoServiceAdmin::addSdoServiceProvider(if=%s)",
               static_cast<const char*>(prof.interface_type)));
    Guard guard(m_provider_mutex);

    std::string id(static_cast<const char*>(prof.id));
    for (auto & prov : m_providers)
      {
        if (id == static_cast<const char*>(prov->getProfile().id))
          {
            RTC_ERROR(("SDO service(id=%s, ifr=%s) already exists",
                       static_cast<const char*>(prof.id),
                       static_cast<const char*>(prof.interface_type)));
            return false;
          }
      }
    m_providers.push_back(provider);
    return true;
  }

  /*!
   * @if jp
   * @brief SDO service provider を削除する
   * @else
   * @brief Remove a SDO service provider
   * @endif
   */
  bool SdoServiceAdmin::removeSdoServiceProvider(const char* id)
  {
    RTC_TRACE(("removeSdoServiceProvider(%d)", id));
    Guard gurad(m_provider_mutex);

    std::string strid(id);
    std::vector<SdoServiceProviderBase*>::iterator it = m_providers.begin();
    std::vector<SdoServiceProviderBase*>::iterator it_end = m_providers.end();
    while (it != it_end)
      {
        if (strid == static_cast<const char*>((*it)->getProfile().id))
          {
            (*it)->finalize();
            SdoServiceProviderFactory&
              factory(SdoServiceProviderFactory::instance());
            factory.deleteObject(*it);
            m_providers.erase(it);
            RTC_INFO(("SDO service provider has been deleted: %s", id));
            return true;
          }
        ++it;
      }
    RTC_WARN(("Specified SDO service provider not found: %s", id));
    return false;
  }

  /*!
   * @if jp
   * @brief Service Consumer を追加する
   * @else
   * @brief Add Service Consumer
   * @endif
   */
  bool SdoServiceAdmin::
  addSdoServiceConsumer(const SDOPackage::ServiceProfile& sProfile)
  {
    Guard guard(m_consumer_mutex);
    RTC_TRACE(("addSdoServiceConsumer(IFR = %s)",
               static_cast<const char*>(sProfile.interface_type)));

    // Not supported consumer type -> error return
    if (!isEnabledConsumerType(sProfile))  { return false; }
    if (!isExistingConsumerType(sProfile)) { return false; }
    RTC_DEBUG(("Valid SDO service required"));
    if (strncmp(sProfile.id, "", 1) == 0)
      {
        RTC_WARN(("No id specified. It should be given by clients."));
        return false;
      }
    RTC_DEBUG(("Valid ID specified"));
    { // re-initialization
      std::string id(sProfile.id);
      for (auto & consumer : m_consumers)
        {
          if (id == static_cast<const char*>(consumer->getProfile().id))
            {
              RTC_INFO(("Existing consumer is reinitilized."));
              RTC_DEBUG(("Propeteis are: %s",
                         NVUtil::toString(sProfile.properties).c_str()));
              return consumer->reinit(sProfile);
            }
        }
    }
    RTC_DEBUG(("SDO service properly initialized."));

    // new pofile
    SdoServiceConsumerFactory&
      factory(SdoServiceConsumerFactory::instance());
    const char* ctype = static_cast<const char*>(sProfile.interface_type);
    if (ctype == nullptr) { return false; }
    SdoServiceConsumerBase* consumer(factory.createObject(ctype));
    if (consumer == nullptr)
      {
        RTC_ERROR(("Hmm... consumer must be created."));
        return false;
      }
    RTC_DEBUG(("An SDO service consumer created."));

    // initialize
    if (!consumer->init(m_rtobj, sProfile))
      {
        RTC_WARN(("SDO service initialization was failed."));
        RTC_DEBUG(("id:         %s", static_cast<const char*>(sProfile.id)));
        RTC_DEBUG(("IFR:        %s",
                   static_cast<const char*>(sProfile.interface_type)));
        RTC_DEBUG(("properties: %s",
                   NVUtil::toString(sProfile.properties).c_str()));
        factory.deleteObject(consumer);
        RTC_INFO(("SDO consumer was deleted by initialization failure"));
        return false;
      }
    RTC_DEBUG(("An SDO service consumer initialized."));
    RTC_DEBUG(("id:         %s", static_cast<const char*>(sProfile.id)));
    RTC_DEBUG(("IFR:        %s",
               static_cast<const char*>(sProfile.interface_type)));
    RTC_DEBUG(("properties: %s",
               NVUtil::toString(sProfile.properties).c_str()));

    // store consumer
    m_consumers.push_back(consumer);

    return true;
  }

  /*!
   * @if jp
   * @brief Service Consumer を削除する
   * @else
   * @brief Remove Service Consumer
   * @endif
   */
  bool SdoServiceAdmin::removeSdoServiceConsumer(const char* id)
  {
    Guard guard(m_consumer_mutex);
    if (id == nullptr || id[0] == '\0')
      {
        RTC_ERROR(("removeSdoServiceConsumer(): id is invalid."));
        return false;
      }
    RTC_TRACE(("removeSdoServiceConsumer(id = %s)", id));

    std::string strid(id);
    std::vector<SdoServiceConsumerBase*>::iterator it = m_consumers.begin();
    std::vector<SdoServiceConsumerBase*>::iterator it_end = m_consumers.end();
    while (it != it_end)
      {
        if (strid == static_cast<const char*>((*it)->getProfile().id))
          {
            (*it)->finalize();
            SdoServiceConsumerFactory&
              factory(SdoServiceConsumerFactory::instance());
            factory.deleteObject(*it);
            m_consumers.erase(it);
            RTC_INFO(("SDO service has been deleted: %s", id));
            return true;
          }
        else 
          {
            ++it;
          }
      }
    RTC_WARN(("Specified SDO consumer not found: %s", id));
    return false;
  }

  //------------------------------------------------------------
  // protected functios
  //------------------------------------------------------------

  /*!
   * @if jp
   * @brief 許可されたサービス型かどうか調べる
   * @else
   * @brief If it is enabled service type
   * @endif
   */
  bool SdoServiceAdmin::
  isEnabledConsumerType(const SDOPackage::ServiceProfile& sProfile)
  {
    if (m_allConsumerEnabled) { return true; }

    for (auto & consumerType : m_consumerTypes)
      {
        if (consumerType ==
            static_cast<const char*>(sProfile.interface_type))
          {
            RTC_DEBUG(("%s is supported SDO service.",
                       static_cast<const char*>(sProfile.interface_type)));
            return true;
          }
      }
    RTC_WARN(("Consumer type is not supported: %s",
              static_cast<const char*>(sProfile.interface_type)));
    return false;
  }

  /*!
   * @if jp
   * @brief 存在するサービス型かどうか調べる
   * @else
   * @brief If it is existing service type
   * @endif
   */
  bool SdoServiceAdmin::
  isExistingConsumerType(const SDOPackage::ServiceProfile& sProfile)
  {
    SdoServiceConsumerFactory& factory(SdoServiceConsumerFactory::instance());
    coil::vstring consumerTypes(factory.getIdentifiers());

    for (coil::vstring::iterator contype = consumerTypes.begin(); contype != consumerTypes.end(); ++contype)
      {
        if (*contype ==
            static_cast<const char*>(sProfile.interface_type))
          {
            RTC_DEBUG(("%s exists in the SDO service factory.",
                       static_cast<const char*>(sProfile.interface_type)));
            RTC_PARANOID(("Available SDO serices in the factory: %s",
                          coil::flatten(consumerTypes).c_str()));
            return true;
          }
      }
    RTC_WARN(("No available SDO service in the factory: %s",
              static_cast<const char*>(sProfile.interface_type)));
    return false;
  }

  const std::string SdoServiceAdmin::getUUID() const
  {
    coil::UUID_Generator uugen = coil::UUID_Generator();
    uugen.init();
    std::unique_ptr<coil::UUID> uuid(uugen.generateUUID(2, 0x01));

    return uuid->to_string();
  }

  std::string SdoServiceAdmin::ifrToKey(std::string& ifr)
  {
    ::coil::vstring ifrvstr = ::coil::split(ifr, ":");
    ::coil::toLower(ifrvstr[1]);
    ::coil::replaceString(ifrvstr[1], ".", "_");
    ::coil::replaceString(ifrvstr[1], "/", ".");
    return ifrvstr[1];
  }


} // namespace RTC
