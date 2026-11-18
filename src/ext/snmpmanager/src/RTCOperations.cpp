// -*- C++ -*-
/*!
 * @file RTCOperations.cpp
 * @brief Operations on RTCs / managers via CORBA
 *
 * @~japanese
 * CORBA による RTC / マネージャの操作
 * @~
 *
 * @date $Date: 2026-11-13 16:24:30 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2026
 *     Nobuhiko Miyamoto
 *
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include "RTCOperations.hpp"

#include <rtm/NVUtil.h>
#include <coil/Properties.h>

#include <stdexcept>

namespace snmpmgr
{
  namespace rtcops
  {
    namespace
    {
      /*!
       * @~japanese
       * @brief NVList を表にする (NVUtil::copyToProperties を使わない。SimpleService の RTC のプロファイルで再帰が終わらないため)
       *
       * @param nv NVList
       * @return 名前 -> 値
       * @~english
       * @brief Converts an NVList into a table (NVUtil::copyToProperties is not used because its recursion does not end for the profile of the SimpleService RTC)
       *
       * @param nv NVList
       * @return name -> value
       * @~
       */
      std::map<std::string, std::string> to_map(const SDOPackage::NVList& nv)
      {
        std::map<std::string, std::string> out;
        for (CORBA::ULong i = 0; i < nv.length(); ++i)
          {
            const char* v = nullptr;
            if (nv[i].value >>= v) { out[std::string(nv[i].name)] = v ? v : ""; }
          }
        return out;
      }
    } // namespace

    RTM::Manager_var resolve_manager(CORBA::ORB_ptr orb, const std::string& ip, int port, const std::string& name)
    {
      std::string loc = "corbaloc:iiop:" + ip + ":" + std::to_string(port) + "/" + name;
      CORBA::Object_var obj = orb->string_to_object(loc.c_str());
      RTM::Manager_var mgr = RTM::Manager::_narrow(obj);
      if (CORBA::is_nil(mgr)) { throw std::runtime_error("Manager reference unavailable: " + loc); }
      return mgr;
    }

    std::vector<RTC::RTObject_var> components(RTM::Manager_ptr mgr)
    {
      std::vector<RTC::RTObject_var> out;
      RTC::RTCList_var list = mgr->get_components();
      for (CORBA::ULong i = 0; i < list->length(); ++i) { out.emplace_back(RTC::RTObject::_duplicate(list[i])); }
      return out;
    }

    RTC::RTObject_var find(RTM::Manager_ptr mgr, const std::string& instance)
    {
      std::vector<RTM::Manager_var> mgrs;
      mgrs.emplace_back(RTM::Manager::_duplicate(mgr));
      RTM::ManagerList_var slaves = mgr->get_slave_managers();
      for (CORBA::ULong i = 0; i < slaves->length(); ++i) { mgrs.emplace_back(RTM::Manager::_duplicate(slaves[i])); }
      for (auto& m : mgrs)
        {
          try
            {
              RTC::RTCList_var found = m->get_components_by_name(instance.c_str());
              if (found->length() > 0) { return RTC::RTObject::_duplicate(found[0]); }
            }
          catch (CORBA::SystemException&) {}
        }
      return RTC::RTObject::_nil();
    }

    RTC::RTObject_var create(RTM::Manager_ptr mgr, const std::string& create)
    {
      RTC::RTObject_var rtc = mgr->create_component(create.c_str());
      if (CORBA::is_nil(rtc)) { throw std::runtime_error("RTC creation failed: " + create); }
      return rtc;
    }

    std::string instance_name(RTC::RTObject_ptr rtc)
    {
      RTC::ComponentProfile_var prof = rtc->get_component_profile();
      return std::string(prof->instance_name);
    }

    RTC::PortService_var port(RTC::RTObject_ptr rtc, const std::string& port_name)
    {
      std::string inst = instance_name(rtc);
      std::string full = port_name.rfind(inst + ".", 0) == 0 ? port_name : inst + "." + port_name;
      RTC::PortService_var p = CORBA_RTCUtil::get_port_by_name(rtc, full);
      if (CORBA::is_nil(p)) { throw std::runtime_error("Port not found: " + full); }
      return p;
    }

    bool connector_exists(RTC::PortService_ptr src, RTC::PortService_ptr dst, const std::string& name)
    {
      // ポート名ではなく参照で比べる (移動した RTC は同じインスタンス名のまま)
      RTC::ConnectorProfileList_var profiles = src->get_connector_profiles();
      for (CORBA::ULong i = 0; i < profiles->length(); ++i)
        {
          if (std::string(profiles[i].name) != name) { continue; }
          for (CORBA::ULong j = 0; j < profiles[i].ports.length(); ++j)
            {
              try { if (profiles[i].ports[j]->_is_equivalent(dst)) { return true; } }
              catch (CORBA::SystemException&) {}
            }
        }
      return false;
    }

    void connect(const std::string& name, const nlohmann::json& props, RTC::PortService_ptr src, RTC::PortService_ptr dst)
    {
      coil::Properties prop;
      for (auto& [k, v] : props.items()) { prop.setProperty(k, v.is_string() ? v.get<std::string>() : v.dump()); }
      RTC::ReturnCode_t ret = CORBA_RTCUtil::connect(name, prop, src, dst);
      if (ret != RTC::RTC_OK) { throw std::runtime_error("Port connection failed: " + name + " (" + std::to_string(ret) + ")"); }
    }

    void activate(RTC::RTObject_ptr rtc)
    {
      RTC::ExecutionContextList_var ecs = rtc->get_owned_contexts();
      if (ecs->length() > 0 && ecs[0]->get_component_state(rtc) == RTC::ACTIVE_STATE) { return; }
      RTC::ReturnCode_t ret = CORBA_RTCUtil::activate(rtc, 0);
      if (ret != RTC::RTC_OK) { throw std::runtime_error("RTC activation failed (" + std::to_string(ret) + ")"); }
    }

    void remove(RTC::RTObject_ptr rtc)
    {
      RTC::ExecutionContextList_var ecs = rtc->get_owned_contexts();
      for (CORBA::ULong i = 0; i < ecs->length(); ++i)
        {
          try
            {
              if (ecs[i]->get_component_state(rtc) == RTC::ACTIVE_STATE) { ecs[i]->deactivate_component(rtc); }
            }
          catch (CORBA::SystemException&) {}
        }
      RTC::PortServiceList_var ports = rtc->get_ports();
      for (CORBA::ULong i = 0; i < ports->length(); ++i)
        {
          try { ports[i]->disconnect_all(); } catch (CORBA::SystemException&) {}
        }
      RTC::ReturnCode_t ret = rtc->exit();
      if (ret != RTC::RTC_OK) { throw std::runtime_error("RTC exit failed (" + std::to_string(ret) + ")"); }
    }

    std::vector<std::map<std::string, std::string>> loadable_modules(RTM::Manager_ptr mgr)
    {
      std::vector<std::map<std::string, std::string>> out;
      RTM::ModuleProfileList_var mods = mgr->get_loadable_modules();
      for (CORBA::ULong i = 0; i < mods->length(); ++i) { out.push_back(to_map(mods[i].properties)); }
      return out;
    }

    std::set<std::string> slave_languages(RTM::Manager_ptr mgr)
    {
      std::set<std::string> out;
      RTM::ManagerList_var slaves = mgr->get_slave_managers();
      for (CORBA::ULong i = 0; i < slaves->length(); ++i)
        {
          try
            {
              SDOPackage::NVList_var conf = slaves[i]->get_configuration();
              auto m = to_map(conf.in());
              if (!m["manager.language"].empty()) { out.insert(m["manager.language"]); }
            }
          catch (CORBA::SystemException&) {}
        }
      return out;
    }

    std::string state_of(RTC::RTObject_ptr rtc)
    {
      RTC::ExecutionContextList_var ecs = rtc->get_owned_contexts();
      if (ecs->length() == 0) { return "UNKNOWN"; }
      switch (ecs[0]->get_component_state(rtc))
        {
        case RTC::ACTIVE_STATE: return "ACTIVE";
        case RTC::INACTIVE_STATE: return "INACTIVE";
        case RTC::ERROR_STATE: return "ERROR";
        case RTC::CREATED_STATE: return "CREATED";
        default: return "UNKNOWN";
        }
    }

    bool reset(RTC::RTObject_ptr rtc)
    {
      RTC::ExecutionContextList_var ecs = rtc->get_owned_contexts();
      return ecs->length() > 0 && ecs[0]->reset_component(rtc) == RTC::RTC_OK;
    }

    std::vector<Component> list_components(RTM::Manager_ptr mgr)
    {
      std::vector<Component> out;
      std::set<std::string> seen;
      std::vector<std::pair<int, RTM::Manager_var>> mgrs;
      try
        {
          RTM::ManagerList_var slaves = mgr->get_slave_managers();
          for (CORBA::ULong i = 0; i < slaves->length(); ++i) { mgrs.emplace_back(static_cast<int>(i) + 1, RTM::Manager::_duplicate(slaves[i])); }
        }
      catch (CORBA::SystemException&) {}
      mgrs.emplace_back(0, RTM::Manager::_duplicate(mgr));
      for (auto& [idx, m] : mgrs)
        {
          RTC::RTCList_var comps;
          try { comps = m->get_components(); }
          catch (CORBA::SystemException&) { continue; }
          std::string slave_name;
          int slave_pid = 0;
          if (idx > 0 && comps->length() > 0)
            {
              try
                {
                  SDOPackage::NVList_var conf = m->get_configuration();
                  auto cm = to_map(conf.in());
                  slave_name = cm["manager.instance_name"];
                  try { slave_pid = cm["manager.pid"].empty() ? 0 : std::stoi(cm["manager.pid"]); }
                  catch (...) {}
                }
              catch (CORBA::SystemException&) {}
            }
          for (CORBA::ULong j = 0; j < comps->length(); ++j)
            {
              try
                {
                  RTC::ComponentProfile_var prof = comps[j]->get_component_profile();
                  std::string name(prof->instance_name);
                  if (!seen.insert(name).second) { continue; }
                  auto props = to_map(prof->properties);
                  Component c;
                  c.info.name = name;
                  c.info.type = std::string(prof->type_name);
                  c.info.impl = props["implementation_id"].empty() ? c.info.type : props["implementation_id"];
                  c.info.language = props["language"];
                  c.info.state = state_of(comps[j]);
                  c.info.slave = idx;
                  c.info.slave_name = slave_name;
                  c.slave_pid = slave_pid;
                  c.ref = RTC::RTObject::_duplicate(comps[j]);
                  out.push_back(std::move(c));
                }
              catch (CORBA::SystemException&) {}
            }
        }
      return out;
    }

    std::pair<std::string, std::vector<std::pair<std::string, RTC::PortService_var>>> ports_by_profile(RTC::RTObject_ptr rtc)
    {
      RTC::ComponentProfile_var prof = rtc->get_component_profile();
      std::vector<std::pair<std::string, RTC::PortService_var>> ports;
      for (CORBA::ULong i = 0; i < prof->port_profiles.length(); ++i)
        {
          std::string n(prof->port_profiles[i].name);
          auto dot = n.find('.');
          ports.emplace_back(dot == std::string::npos ? n : n.substr(dot + 1),
                             RTC::PortService::_duplicate(prof->port_profiles[i].port_ref));
        }
      return {std::string(prof->instance_name), ports};
    }

    std::map<std::string, std::vector<std::string>> slave_components(RTM::Manager_ptr mgr)
    {
      std::map<std::string, std::vector<std::string>> out;
      RTM::ManagerList_var slaves = mgr->get_slave_managers();
      for (CORBA::ULong i = 0; i < slaves->length(); ++i)
        {
          try
            {
              SDOPackage::NVList_var conf = slaves[i]->get_configuration();
              std::string name = to_map(conf.in())["manager.instance_name"];
              auto& list = out[name];
              RTC::RTCList_var comps = slaves[i]->get_components();
              for (CORBA::ULong j = 0; j < comps->length(); ++j)
                {
                  try { list.push_back(instance_name(comps[j])); }
                  catch (CORBA::SystemException&) {}
                }
            }
          catch (CORBA::SystemException&) {}
        }
      return out;
    }
  } // namespace rtcops
} // namespace snmpmgr
