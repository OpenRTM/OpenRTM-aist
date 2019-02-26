// -*- C++ -*-
/*!
 * @file LocalServiceAdmin.cpp
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
#include <rtm/LocalServiceAdmin.h>
#include <rtm/LocalServiceBase.h>

#include <cstring>
#include <algorithm>
#include <memory>
#include <vector>

namespace RTM
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  LocalServiceAdmin::LocalServiceAdmin()
    : rtclog("LocalServiceAdmin")
  {
    RTC_TRACE(("LocalServiceAdmin::LocalServiceAdmin()"));
  }

  /*!
   * @if jp
   * @brief 仮想デストラクタ
   * @else
   * @brief Virtual destractor
   * @endif
   */
  LocalServiceAdmin::~LocalServiceAdmin()
  {
    finalize();
  }

  /*!
   * @if jp
   * @brief "all" 文字列探索Functor
   * @else
   * @brief A functor to search "all"
   * @endif
   */
  struct find_all
  {
    bool operator()(const std::string& str)
    {
      std::string a = str;
      return coil::normalize(a) == "all" ? true : false;
    }
  };

  /*!
   * @if jp
   *
   * @brief LocaServiceAdminの初期化
   * @else
   * @brief Initialization of LocalServiceAdmin
   * @endif
   */
  void LocalServiceAdmin::init(coil::Properties& props)
  {
    RTC_TRACE(("LocalServiceAdmin::init():"));
    RTC_DEBUG_STR((props));
    coil::vstring svcs(coil::split(props["enabled_services"], ","));
    bool all_enable(false);
    if (std::find_if(svcs.begin(), svcs.end(), find_all()) != svcs.end())
      {
        RTC_INFO(("All the local services are enabled."));
        all_enable = true;
      }

    RTM::LocalServiceFactory& factory(RTM::LocalServiceFactory::instance());
    coil::vstring ids = factory.getIdentifiers();
    RTC_DEBUG(("Available services: %s", coil::flatten(ids).c_str()));

    for (size_t i(0); i < ids.size(); ++i)
      {
        if (all_enable || isEnabled(ids[i], svcs))
          {
            if (notExisting(ids[i]))
              {
                LocalServiceBase* service(factory.createObject(ids[i]));
                RTC_DEBUG(("Service created: %s", ids[i].c_str()));
                coil::Properties& prop(props.getNode(ids[i]));
                service->init(prop);
                addLocalService(service);
              }
          }
      }
  }

  /*!
   * @if jp
   * @brief LocalserviceAdmin の終了処理
   * @else
   * @brief Finalization ofLocalServiceAdmin
   * @endif
   */
  void LocalServiceAdmin::finalize()
  {
    RTM::LocalServiceFactory& factory(RTM::LocalServiceFactory::instance());
    for (size_t i(0); i < m_services.size(); ++i)
      {
        m_services[i]->finalize();
        factory.deleteObject(m_services[i]);
      }
    m_services.clear();
  }

  /*!
   * @if jp
   * @brief LocalServiceProfileListの取得
   * @else
   * @brief Getting LocalServiceProfileList
   * @endif
   */
  RTM::LocalServiceProfileList LocalServiceAdmin::getServiceProfiles()
  {
    RTM::LocalServiceProfileList profs(0);
    for (size_t i(0); i < m_services.size(); ++i)
      {
        profs.push_back(m_services[i]->getProfile());
      }
    return profs;
  }

  /*!
   * @if jp
   * @brief LocalServiceProfile を取得する
   * @else
   * @brief Get LocalServiceProfile of an LocalService
   * @endif
   */
  bool
  LocalServiceAdmin::getServiceProfile(const std::string& name,
                                       ::RTM::LocalServiceProfile& prof)
  {
    Guard guard(m_services_mutex);
    for (size_t i(0); i < m_services.size(); ++i)
      {
        if (name == m_services[i]->getProfile().name)
          {
            prof = m_services[i]->getProfile();
            return true;
          }
      }
    return false;
  }

  /*!
   * @if jp
   * @brief LocalService の Service を取得する
   * @else
   * @brief Get a pointer of a LocalService
   * @endif
   */
  RTM::LocalServiceBase* LocalServiceAdmin::getService(const char* id)
  {
    for (size_t i(0); i < m_services.size(); ++i)
      {
        if (m_services[i]->getProfile().name == id)
          {
            return m_services[i];
          }
      }
    return nullptr;
  }

  /*!
   * @if jp
   * @brief SDO service provider をセットする
   * @else
   * @brief Set a SDO service provider
   * @endif
   */
  bool
  LocalServiceAdmin::addLocalService(::RTM::LocalServiceBase* service)
  {
    if (service == nullptr)
      {
        RTC_ERROR(("Invalid argument: addLocalService(service == NULL)"));
        return false;
      }
    RTC_TRACE(("LocalServiceAdmin::addLocalService(%s)",
               service->getProfile().name.c_str()));
    Guard guard(m_services_mutex);
    m_services.push_back(service);
    return true;
  }

  /*!
   * @if jp
   * @brief LocalService を削除する
   * @else
   * @brief Remove a LocalService
   * @endif
   */
  bool LocalServiceAdmin::removeLocalService(const std::string& name)
  {
    RTC_TRACE(("removeLocalService(%d)", name.c_str()));
    Guard gurad(m_services_mutex);

    std::vector<LocalServiceBase*>::iterator it = m_services.begin();
    std::vector<LocalServiceBase*>::iterator it_end = m_services.end();
    while (it != it_end)
      {
        if (name == (*it)->getProfile().name)
          {
            (*it)->finalize();
            LocalServiceFactory&
              factory(LocalServiceFactory::instance());
            factory.deleteObject(*it);
            m_services.erase(it);
            RTC_INFO(("SDO service  has been deleted: %s", name.c_str()));
            return true;
          }
        ++it;
      }
    RTC_WARN(("Specified SDO service  not found: %s", name.c_str()));
    return false;
  }

  //============================================================
  // private functions
  /*!
   * @if jp
   * @brief 指定されたIDが有効かどうかチェックする
   * @else
   * @brief Check if specified ID is enabled
   * @endif
   */
  bool LocalServiceAdmin::isEnabled(const std::string& id,
                                    const coil::vstring& enabled)
  {
    bool ret = std::find(enabled.begin(), enabled.end(), id) != enabled.end();
    RTC_DEBUG(("Local service %s %s enabled.", id.c_str(),
               ret ? "is" : "is not"));
    return ret;
  }


  /*!
   * @if jp
   * @brief 指定されたIDがすでに存在するかどうかチェックする
   * @else
   * @brief Check if specified ID is existing
   * @endif
   */
  bool LocalServiceAdmin::notExisting(const std::string& id)
  {
    Guard gurad(m_mutex);
    for (size_t i(0); i < m_services.size(); ++i)
      {
        if (m_services[i]->getProfile().name == id)
          {
            RTC_WARN(("Local service %s already exists.", id.c_str()));
            return false;
          }
      }
    RTC_DEBUG(("Local service %s does not exist.", id.c_str()));
    return true;
  }

};  // namespace RTM
