// -*- C++ -*-
/*!
 * @file  CorbaPort.h
 * @brief CorbaPort class
 * @date  $Date: 2007-12-31 03:08:02 $
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

#include <rtm/SystemLogger.h>
#include <rtm/CorbaPort.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/NVUtil.h>
#include <rtm/Manager.h>
#include <string>

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  CorbaPort::CorbaPort(const char* name)
    : PortBase(name)
  {
    addProperty("port.port_type", "CorbaPort");
  }

  /*!
   * @if jp
   * @brief 仮想デストラクタ
   * @else
   * @brief Virtual destructor
   * @endif
   */
  CorbaPort::~CorbaPort()
  {
  }

  /*!
   * @if jp
   * @brief プロパティの初期化
   * @else
   * @brief Initializing properties
   * @endif
   */
  void CorbaPort::init(coil::Properties& prop)
  {
    RTC_TRACE(("init()"));
    RTC_PARANOID(("given properties:"));
    RTC_DEBUG_STR((prop));

    m_properties << prop;
    NVList nv(0);
    NVUtil::copyFromProperties(nv, m_properties);
    CORBA_SeqUtil::push_back_list(m_profile.properties, nv);
    RTC_PARANOID(("updated properties:"));
    RTC_DEBUG_STR((m_properties));

    int num(-1);
    if (!coil::stringTo(num, m_properties.getProperty("connection_limit",
                                                      "-1").c_str()))
      {
        RTC_ERROR(("invalid connection_limit value: %s",
                   m_properties.getProperty("connection_limit").c_str()));
      }

    setConnectionLimit(num);
  }

  /*!
   * @if jp
   * @brief Provider を登録する
   * @else
   * @brief Register the provider
   * @endif
   */
  bool
  CorbaPort::registerProvider(const char* instance_name,
                              const char* type_name,
                              PortableServer::RefCountServantBase& provider)
  {
    RTC_TRACE(("registerProvider(instance=%s, type_name=%s)",
               instance_name, type_name));

    try
      {
        CorbaProviderHolder providerholder(type_name, instance_name, &provider);
        m_providers.push_back(providerholder);
      }
    catch (...)
      {
        RTC_ERROR(("appending provider interface failed"));
        return false;
      }

    if (!appendInterface(instance_name, type_name, RTC::PROVIDED))
      {
        RTC_ERROR(("appending provider interface failed"));
        return false;
      }

    return true;
  };

  /*!
   * @if jp
   * @brief Consumer を登録する
   * @else
   * @brief Register the consumer
   * @endif
   */
  bool
  CorbaPort::registerConsumer(const char* instance_name,
                              const char* type_name,
                              CorbaConsumerBase& consumer)
  {
    RTC_TRACE(("registerConsumer()"));

    if (!appendInterface(instance_name, type_name, RTC::REQUIRED))
      {
        return false;
      }

    m_consumers.push_back(CorbaConsumerHolder(type_name,
                                              instance_name,
                                              &consumer));

    return true;
  }

  //============================================================
  // Local operations
  //============================================================

  /*!
   * @if jp
   * @brief Port の全てのインターフェースを activates する
   * @else
   * @brief Activate all Port interfaces
   * @endif
   */
  void CorbaPort::activateInterfaces()
  {
    for(auto & provider : m_providers)
      {
        provider.activate();
      }
  }

  /*!
   * @if jp
   * @brief 全ての Port のインターフェースを deactivates する
   * @else
   * @brief Deactivate all Port interfaces
   * @endif
   */
  void CorbaPort::deactivateInterfaces()
  {
    for(auto & provider : m_providers)
      {
        provider.deactivate();
      }
  }

  //============================================================
  // protected functions
  //============================================================
  /*!
   * @if jp
   * @brief Interface 情報を公開する
   * @else
   * @brief Publish interface information
   * @endif
   */
  ReturnCode_t
  CorbaPort::publishInterfaces(ConnectorProfile& connector_profile)
  {
    RTC_TRACE(("publishInterfaces()"));

    ReturnCode_t returnvalue = _publishInterfaces();
    if (returnvalue != RTC::RTC_OK)
      {
        return returnvalue;
      }

    NVList properties;
    for(auto & provider : m_providers)
      {
        //------------------------------------------------------------
        // new version descriptor
        // <comp_iname>.port.<port_name>.provided.<type_name>.<instance_name>
        std::string newdesc((const char*)m_profile.name);
        newdesc.insert(m_ownerInstanceName.size(), ".port");
        newdesc += ".provided." + provider.descriptor();
        CORBA_SeqUtil::
          push_back(properties,
                    NVUtil::newNV(newdesc.c_str(), provider.ior().c_str()));

        //------------------------------------------------------------
        // old version descriptor
        // port.<type_name>.<instance_name>
        std::string olddesc = "port." + provider.descriptor();
        CORBA_SeqUtil::
          push_back(properties,
                    NVUtil::newNV(olddesc.c_str(), provider.ior().c_str()));
      }

#ifdef ORB_IS_RTORB
    {
      CORBA::ULong len1(connector_profile.properties.length());
      CORBA::ULong len2(properties.length());
      CORBA::ULong len(len1 + len2);
      connector_profile.properties.length(len);

      for (CORBA::ULong i = 0; i < len2; ++i)
        {
          connector_profile.properties[len1 + i] = properties[i];
        }
    }
#else  // ORB_IS_RTORB
    CORBA_SeqUtil::push_back_list(connector_profile.properties, properties);
#endif

    RTC_DEBUG_STR((NVUtil::toString(properties)));

    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief Interface に接続する
   * @else
   * @brief Subscribe to interfaces
   * @endif
   */
  ReturnCode_t
  CorbaPort::subscribeInterfaces(const ConnectorProfile& connector_profile)
  {
    RTC_TRACE(("subscribeInterfaces()"));

    const NVList& nv(connector_profile.properties);
    RTC_DEBUG_STR((NVUtil::toString(nv)));

    bool strict(false);  // default is "best_effort"
    CORBA::Long index(NVUtil::find_index(nv, "port.connection.strictness"));
    if (index >=  0)
      {
        const char* strictness = nullptr;
        nv[index].value >>= strictness;
        if (std::string("best_effort") == strictness)
          {
            strict = false;
          }
        else if (std::string("strict") == strictness)
          {
            strict = true;
          }
        else
          {
            strict = false;
          }
        RTC_DEBUG(("Connetion strictness is: %s",
                   strict ? "strict" : "best_effort"))
      }

    for (auto & consumer : m_consumers)
      {
        std::string ior = std::string();
        if (findProvider(nv, consumer, ior))
          {
            setObject(ior, consumer);
            continue;
          }
        if (findProviderOld(nv, consumer, ior))
          {
            setObject(ior, consumer);
            continue;
          }

        // never come here without error
        // if strict connection option is set, error is returned.
        if (strict)
          {
            RTC_ERROR(("subscribeInterfaces() failed."));
            return RTC::RTC_ERROR;
          }
      }

    RTC_TRACE(("subscribeInterfaces() successfully finished."));

    return RTC::RTC_OK;
  }

  /*!
   * @if jp
   * @brief Interface への接続を解除する
   * @else
   * @brief Unsubscribe interfaces
   * @endif
   */
  void
  CorbaPort::unsubscribeInterfaces(const ConnectorProfile& connector_profile)
  {
    RTC_TRACE(("unsubscribeInterfaces()"));

    const NVList& nv(connector_profile.properties);
    RTC_DEBUG_STR((NVUtil::toString(nv)));

    for (auto & consumer : m_consumers)
      {
        std::string ior;
        if (findProvider(nv, consumer, ior))
          {
            RTC_DEBUG(("Correspoinding consumer found."));
            releaseObject(ior, consumer);
            continue;
          }
        if (findProviderOld(nv, consumer, ior))
          {
            RTC_DEBUG(("Correspoinding consumer found."));
            releaseObject(ior, consumer);
            continue;
          }
      }
  }

  /*!
   * @if jp
   * @brief Consumer に合致する Provider を NVList の中から見つける
   * @else
   * @brief Find out a provider corresponding to the consumer from NVList
   * @endif
   */
  bool CorbaPort::findProvider(const NVList& nv, CorbaConsumerHolder& cons,
                               std::string& iorstr)
  {
    // new consumer interface descriptor
    std::string newdesc((const char*)m_profile.name);
    newdesc.insert(m_ownerInstanceName.size(), ".port");
    newdesc += ".required." + cons.descriptor();

    // find a NameValue of the consumer
    CORBA::Long cons_index(NVUtil::find_index(nv, newdesc.c_str()));
    if (cons_index < 0) { return false; }

    const char* provider(nullptr);
    if (!(nv[cons_index].value >>= provider))
      {
        RTC_WARN(("Cannot extract Provider interface descriptor"));
        return false;
      }

    // find a NameValue of the provider
    CORBA::Long prov_index(NVUtil::find_index(nv, provider));
    if (prov_index < 0) { return false; }

    const char* ior(nullptr);
    if (!(nv[prov_index].value >>= ior))
      {
        RTC_WARN(("Cannot extract Provider IOR string"));
        return false;
      }
    iorstr = std::string(ior);
    RTC_DEBUG(("interface matched with new descriptor: %s", newdesc.c_str()));
    return true;
  }

  /*!
   * @if jp
   * @brief Consumer に合致する Provider を NVList の中から見つける
   * @else
   * @brief Find out a provider corresponding to the consumer from NVList
   * @endif
   */
  bool CorbaPort::findProviderOld(const NVList&nv, CorbaConsumerHolder& cons,
                                  std::string& iorstr)
  {
    // old consumer interface descriptor
    std::string olddesc("port."); olddesc += cons.descriptor();

    // find a NameValue of the provider same as olddesc
    CORBA::Long index(NVUtil::find_index(nv, olddesc.c_str()));
    if (index < 0) { return false; }

    const char* ior(nullptr);
    if (!(nv[index].value >>= ior))
      {
        RTC_WARN(("Cannot extract Provider IOR string"));
        return false;
      }
    iorstr = ior;
    RTC_INFO(("interface matched with old descriptor: %s", olddesc.c_str()));
    return true;
  }

  /*!
   * @if jp
   * @brief Consumer に IOR をセットする
   * @else
   * @brief Setting IOR to Consumer
   * @endif
   */
  bool CorbaPort::setObject(const std::string& ior, CorbaConsumerHolder& cons)
  {
    // if ior string is "null" or "nil", ignore it.
    if (std::string("null") == ior) { return true; }
    if (std::string("nil")  == ior) { return true; }
    // IOR should be started by "IOR:"
    if (std::string("IOR:").compare(0, 4, ior.c_str(), 4) != 0)
      {
        return false;
      }

    // set IOR to the consumer
    if (!cons.setObject(ior.c_str()))
      {
        RTC_ERROR(("Cannot narrow reference"));
        return false;
      }
    RTC_TRACE(("setObject() done"));
    return true;
  }

  /*!
   * @if jp
   * @brief Consumer のオブジェクトをリリースする
   * @else
   * @brief Releasing Consumer Object
   * @endif
   */
  bool CorbaPort::releaseObject(const std::string& ior,
                                CorbaConsumerHolder& cons)
  {
    if (ior == cons.getIor())
      {
        cons.releaseObject();
        RTC_DEBUG(("Consumer %s released.", cons.descriptor().c_str()));
        return true;
      }
    RTC_WARN(("IORs between Consumer and Connector are different."));
    return false;
  }

} // namespace RTC
