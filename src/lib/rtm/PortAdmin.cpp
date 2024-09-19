// -*- C++ -*-
/*!
 * @file PortAdmin.cpp
 * @brief RTC's Port administration class
 * @date $Date: 2008-01-14 07:56:11 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/RTC.h>
#include <rtm/PortAdmin.h>
#include <rtm/CORBA_SeqUtil.h>
#include <cstring>
#include <functional>
#include <vector>

namespace RTC
{
  /*!
   * @if jp
   * @brief Port検索用ファンクタ
   * @else
   * @brief Functor to fing a Port
   * @endif
   */
  struct PortAdmin::find_port_name
  {
    explicit find_port_name(const char* name) : m_name(name) {}
#ifdef ORB_IS_ORBEXPRESS
    bool operator()(const PortService_var& p)
#else
    bool operator()(const PortService_ptr& p)
#endif
    {
      try
        {
#ifndef ORB_IS_RTORB
          PortProfile_var prof(p->get_port_profile());
          std::string name(prof->name);
#else  // ORB_IS_RTORB
          PortProfile *pp;
          pp = p->get_port_profile();
          std::string name(pp->name);
          delete pp;
#endif  // ORB_IS_RTORB
          return m_name == name;
        }
      catch (...)
        {
          return false;
        }
    }
    const std::string m_name;
  };

  struct PortAdmin::find_port
  {
    explicit find_port(const PortService_ptr& p) : m_port(p) {}
#ifdef ORB_IS_ORBEXPRESS
    bool operator()(const PortService_var& p)
    {
      return m_port->_is_equivalent(p.in());
#else
    bool operator()(const PortService_ptr& p)
    {
      return m_port->_is_equivalent(p);
#endif
    }
    const PortService_ptr m_port;
  };

  /*!
   * @if jp
   * @brief Port削除用ファンクタ
   * @else
   * @brief Functor to delete the Port
   * @endif
   */
  struct PortAdmin::del_port
  {
    PortAdmin* m_pa;
    explicit del_port(PortAdmin* pa) : m_pa(pa) {}
    void operator()(PortBase* p)
    {
      m_pa->removePort(*p);
    }
  };

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  PortAdmin::PortAdmin(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
    : m_pORB(orb),
      m_pPOA(poa),
      rtclog("portadmin")
  {
  }

  PortAdmin::~PortAdmin() = default; // No inline for gcc warning, too big

  /*!
   * @if jp
   * @brief PortServiceList の取得
   * @else
   * @brief Get PortServiceList
   * @endif
   */
  PortServiceList* PortAdmin::getPortServiceList() const
  {
    PortServiceList_var ports;
    ports = new PortServiceList(m_portRefs);
    return ports._retn();
  }

  /*!
   * @if jp
   * @brief PortProfileList の取得
   * @else
   * @brief Get PortProfileList
   * @endif
   */
  PortProfileList PortAdmin::getPortProfileList() const
  {

#ifndef ORB_IS_RTORB
    PortProfileList port_profs(0);
    port_prof_collect2 p(port_profs);
    ::CORBA_SeqUtil::for_each(m_portRefs, p);
#else  // ORB_IS_RTORB
    CORBA::ULong len = m_portRefs.length();

    PortProfileList port_profs = PortProfileList(len);
    PortProfile *pp;

    for (CORBA::ULong i (0); i < len; ++i)
      {
        try
          {
            pp = m_portRefs[i]->get_port_profile();
            port_profs[i] = *(pp);
            delete pp;
          }
        catch (...)
          {
          }
      }
#endif  // ORB_IS_RTORB
    return port_profs;
  }

  /*!
   * @if jp
   * @brief Port のオブジェクト参照の取得
   * @else
   * @brief Get the reference to the Port's object
   * @endif
   */
  PortService_ptr PortAdmin::getPortRef(const char* port_name) const
  {
    CORBA::Long index;
    index = CORBA_SeqUtil::find(m_portRefs, find_port_name(port_name));
    if (index >= 0)
      {
#ifdef ORB_IS_ORBEXPRESS
        return RTC::PortService::_duplicate(m_portRefs[index].in());
#else
        return RTC::PortService::_duplicate(m_portRefs[index]);
#endif
      }
    return RTC::PortService::_nil();
  }

  /*!
   * @if jp
   * @brief Port のサーバントのポインタの取得
   * @else
   * @brief Get pointer to the Port's servant
   * @endif
   */
  PortBase* PortAdmin::getPort(const char* port_name) const
  {
    return m_portServants.find(port_name);
  }

  /*!
   * @if jp
   * @brief Port を登録する
   * @else
   * @brief Regsiter the Port
   * @endif
   */
  bool PortAdmin::addPort(PortBase& port)
  {
    // Check for duplicate
    if (CORBA_SeqUtil::find(m_portRefs, find_port_name(port.getName())) != -1)
      {
        return false;
      }

    // Store Port's ref to PortServiceList
    CORBA_SeqUtil::push_back(m_portRefs, port.getPortRef());

    // Store Port servant
    return m_portServants.registerObject(&port);
  }

  /*!
   * @if jp
   * @brief Port を登録する
   * @else
   * @brief Regsiter the Port
   * @endif
   */
  bool PortAdmin::addPort(PortService_ptr port)
  {
    // Check for duplicate
    try
      {
        PortProfile_var prof(port->get_port_profile());
        std::string name(prof->name);

// Why RtORB delete _var object explicitly?
#ifdef ORB_IS_RTORB
        delete prof._retn();
#endif
        if (CORBA_SeqUtil::find(m_portRefs, find_port_name(name.c_str())) != -1)
          {
            return false;
          }
      }
    catch (...)
      {
        return false;
      }
    CORBA_SeqUtil::push_back(m_portRefs, RTC::PortService::_duplicate(port));
    return true;
  }

  void PortAdmin::registerPort(PortBase& port)
  {
    if (!addPort(port))
      {
        RTC_ERROR(("registerPort(PortBase&) failed."));
      }
  }

  void PortAdmin::registerPort(PortService_ptr port)
  {
    if (!addPort(port))
      {
        RTC_ERROR(("registerPort(PortService_ptr) failed."));
      }
  }

  /*!
   * @if jp
   * @brief Port の登録を解除する
   * @else
   * @brief Unregister the Port's registration
   * @endif
   */
  bool PortAdmin::removePort(PortBase& port)
  {
    try
      {
        port.disconnect_all();

        const char* tmp(port.getProfile().name);
        CORBA_SeqUtil::erase_if(m_portRefs, find_port_name(tmp));

        PortableServer::ObjectId_var oid = m_pPOA->servant_to_id(&port);
        m_pPOA->deactivate_object(oid);
        port.setPortRef(RTC::PortService::_nil());

        return m_portServants.unregisterObject(tmp) != nullptr;
      }
    catch (...)
      {
        return false;
      }
  }

  bool PortAdmin::removePort(PortService_ptr port)
  {
    try
      {
        CORBA_SeqUtil::erase_if(m_portRefs, find_port(port));
        return true;
      }
    catch (...)
      {
        return false;
      }
  }

  void PortAdmin::deletePort(PortBase& port)
  {
    if (!removePort(port))
      {
        RTC_ERROR(("deletePort(PortBase&) failed."));
      }
  }
  void PortAdmin::deletePort(PortService_ptr port)
  {
    if (!removePort(port))
      {
        RTC_ERROR(("deletePort(PortService_ptr) failed."));
      }
  }


  /*!
   * @if jp
   * @brief 名称指定によりPort の登録を解除する
   * @else
   * @brief Unregister the Port's registration by its name
   * @endif
   */
  void PortAdmin::deletePortByName(const char* port_name)
  {
    if (port_name == nullptr) return;
    PortBase& p(*m_portServants.find(port_name));
    removePort(p);
  }

  /*!
   * @if jp
   * @brief 全ての Port のインターフェースを activates する
   * @else
   * @brief Activate all Port interfaces
   * @endif
   */
  void PortAdmin::activatePorts()
  {
    std::vector<PortBase*> ports;
    ports = m_portServants.getObjects();
    for (auto & port : ports)
      {
        port->activateInterfaces();
      }
  }

  /*!
   * @if jp
   * @brief 全ての Port のインターフェースを deactivates する
   * @else
   * @brief Deactivate all Port interfaces
   * @endif
   */
  void PortAdmin::deactivatePorts()
  {
    std::vector<PortBase*> ports;
    ports = m_portServants.getObjects();
    for (auto & port : ports)
      {
        port->deactivateInterfaces();
      }
  }

  /*!
   * @if jp
   * @brief 全ての Port をdeactivateし登録を削除する
   * @else
   * @brief Deactivate all Ports and unregister them
   * @endif
   */
  void PortAdmin::finalizePorts()
  {
    deactivatePorts();
    std::vector<PortBase*> ports;
    ports = m_portServants.getObjects();
    for_each(ports.begin(), ports.end(), del_port(this));
  }
} // namespace RTC
