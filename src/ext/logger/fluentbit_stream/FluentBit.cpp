// -*- C++ -*-
/*!
 * @file LogstreamBase.h
 * @brief Logger stream buffer base class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */
#include <algorithm>

#include <rtm/Manager.h>
#include <rtm/LogstreamBase.h>
#include <rtm/SystemLogger.h>
#include <coil/stringutil.h>
#include "FluentBit.h"

namespace RTC
{
  // Static variables initialization
  flb_ctx_t* FluentBitStream::s_flbContext = nullptr;
  int FluentBitStream::s_instance = 0;

  //============================================================
  // FluentBitStream class
  FluentBitStream::FluentBitStream()
  {
    for (char & i : m_buf) { i = '\0'; }
    if (s_flbContext == nullptr)
      {
        s_flbContext = flb_create();
      }
  }

  FluentBitStream::~FluentBitStream()
  {
    --s_instance;
    if (s_instance == 0)
      {
        flb_stop(s_flbContext);
        flb_destroy(s_flbContext);
      }
  }

  bool FluentBitStream::init(const coil::Properties& prop)
  {
    flb_stop(s_flbContext);



    // Default lib-input setting
    FlbHandler flbhandler = flb_input(s_flbContext, (char*)"lib", nullptr);
    flb_input_set(s_flbContext, flbhandler, "tag", prop.getName(), NULL);
    m_flbIn.emplace_back(flbhandler);

    const std::vector<coil::Properties*>& leaf(prop.getLeaf());

    for(auto & lprop : leaf)
      {
        std::string key(lprop->getName());
        if (key.find("output") != std::string::npos)
          {
            createOutputStream(*lprop);
          }
        else if (key.find("input") != std::string::npos)
          {
            createInputStream(*lprop);
          }
        else if (key.find("option") != std::string::npos)
        {
            setServiceOption(*lprop);
        }
      }
    // Start the background worker
    flb_start(s_flbContext);
    return true;
  }

  int FluentBitStream::setServiceOption(const coil::Properties& prop)
  {
    const std::vector<Properties*>& leaf = prop.getLeaf();
    int ret = 0;
    for(auto & lprop : leaf)
      {
        ret = flb_service_set(s_flbContext, lprop->getName(), lprop->getValue(), NULL);
      }
    return ret;
  }

  bool FluentBitStream::createOutputStream(const coil::Properties& prop)
  {
    std::string plugin = prop["plugin"];
    FlbHandler flbout = flb_output(s_flbContext,
                                   (char*)plugin.c_str(), nullptr);

    m_flbOut.emplace_back(flbout);
    const std::vector<Properties*>& leaf = prop.getLeaf();
    for(auto & lprop : leaf)
      {
        flb_output_set(s_flbContext, flbout,
                       lprop->getName(), lprop->getValue(), NULL);
      }
    return true;
  }
  
  bool FluentBitStream::createInputStream(const coil::Properties& prop)
  {
    std::string plugin = prop["plugin"];
    
    FlbHandler flbin = flb_input(s_flbContext,
                                 (char*)plugin.c_str(), nullptr);
    m_flbIn.emplace_back(flbin);
    const std::vector<Properties*>& leaf = prop.getLeaf();
    for(auto & lprop : leaf)
      {
        flb_input_set(s_flbContext, flbin,
                      lprop->getName(), lprop->getValue(), NULL);
      }
    return true;
  }

  std::streamsize FluentBitStream::pushLogger(int level, const std::string &name, const std::string &date, const char* mes)
  {
    char tmp[BUFFER_LEN];
    std::streamsize n(0);
    coil::Properties& conf = ::RTC::Manager::instance().getConfig();
    const std::string& pid = conf["manager.pid"];
    const std::string& host_name = conf["os.hostname"];
    const std::string& manager_name = conf["manager.instance_name"];
    for(auto & flb : m_flbIn)
      {

        n = snprintf(tmp, sizeof(tmp) - 1,
                     R"([%ld, {"time":"%s","name":"%s","level":"%s","pid":"%s","host":"%s","manager":"%s","message": "%s"}])", time(nullptr), date.c_str(), name.c_str(), Logger::getLevelString(level).c_str(), pid.c_str(), host_name.c_str(), manager_name.c_str(), mes);

        flb_lib_push(s_flbContext, flb, tmp, n);
      }
    return n;
  }
  
  void FluentBitStream::write(int level, const std::string &name, const std::string &date, const std::string &mes)
  {
    std::string message{coil::replaceString(mes, "\"", "\'")};
    std::streamsize insize(message.size());
    for (std::streamsize i(0); i < insize; ++i, ++m_pos)
      {
        m_buf[m_pos] = message[i];
        if (message[i] == '\n' || message[i] == '\r' || m_pos == (BUFFER_LEN - 1))
          {
            m_buf[m_pos] = '\0';
            pushLogger(level, name, date, m_buf);
            m_pos = 0;
          }
        else if(i == insize-1)
          {
            m_buf[m_pos+1] = '\0';
            pushLogger(level, name, date, m_buf);
          }
      }
     m_pos = 0;

  }
  // end of FluentBitStream class
  //============================================================

  //============================================================
  // FluentBit class
  FluentBit::FluentBit() = default;

  FluentBit::~FluentBit() = default;

  bool FluentBit::init(const coil::Properties& prop)
  {
    return m_fbstream.init(prop);
  }

  coil::LogStreamBuffer* FluentBit::getStreamBuffer()
  {
    return &m_fbstream;
  }
  // end of FluentBit class
  //============================================================
} // namespace RTC

extern "C"
{
  void FluentBitInit()
  {
    ::RTC::LogstreamFactory::
      instance().addFactory("fluentd",
                            ::coil::Creator< ::RTC::LogstreamBase,
                                             ::RTC::FluentBit>,
                            ::coil::Destructor< ::RTC::LogstreamBase,
                                                ::RTC::FluentBit>);
  }
}
