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
#include <sstream>

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
    if (s_flbContext == nullptr)
      {
        s_flbContext = flb_create();
        if (s_flbContext == nullptr)
          {
            std::cerr << "flb_create() failed." << std::endl;
            throw std::bad_alloc();
          }
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
        else if (key == "option")
        {
            setServiceOption(*lprop);
        }
      }
    // Start the background worker
    if (flb_start(s_flbContext) < 0)
      {
        std::cerr << "flb_start() failed." << std::endl;
      }
    return true;
  }

  int FluentBitStream::setServiceOption(const coil::Properties& prop)
  {
    const std::vector<Properties*>& leaf = prop.getLeaf();
    int ret = 0;
    for(auto & lprop : leaf)
      {
        std::string key(lprop->getName()), value(lprop->getValue());
        ret = flb_service_set(s_flbContext, key.c_str(), value.c_str(), NULL);
      }
    return ret;
  }

  bool FluentBitStream::createOutputStream(const coil::Properties& prop)
  {
    std::string plugin = prop["plugin"];
    FlbHandler flbout = flb_output(s_flbContext,
                                   (char*)plugin.c_str(), nullptr);
    m_flbOut.emplace_back(flbout);
    
    if(prop.findNode("conf") != nullptr)
      {
        Properties confprop = prop;
        const std::vector<Properties*> &leaf = confprop.getNode("conf").getLeaf();
        for(auto & lprop : leaf)
          {
            std::string key(lprop->getName()), value(lprop->getValue());

//オプション有効チェック
//FluentBit v1.9では動作していたが v3.2では無効と判断されてしまう
//コードはコメントアウトして残した
//                int ret = flb_output_property_check(s_flbContext,
//                                                flbout, &key[0], &value[0]);
//            if (ret == FLB_LIB_ERROR || ret == FLB_LIB_NO_CONFIG_MAP)
//              {
//                std::cerr << "Unknown property for \"" << plugin << "\" plugin: ";
//                std::cerr << key << ": " << value << std::endl;
//              }
            flb_output_set(s_flbContext, flbout, key.c_str(), value.c_str(), NULL);
          }
      }
    return true;
  }
  
  bool FluentBitStream::createInputStream(const coil::Properties& prop)
  {
    std::string plugin = prop["plugin"];
    FlbHandler flbin = flb_input(s_flbContext,
                                 (char*)plugin.c_str(), nullptr);
    if (flbin < 0) {
      plugin = "lib";
      flbin = flb_input(s_flbContext,
                                 (char*)plugin.c_str(), nullptr);
    }
    m_flbIn.emplace_back(flbin);
    if(prop.findNode("conf") != nullptr)
      {
        Properties confprop = prop;
        const std::vector<Properties*> &leaf = confprop.getNode("conf").getLeaf();
        for(auto & lprop : leaf)
          {
            std::string key(lprop->getName()), value(lprop->getValue());

//オプション有効チェック
//FluentBit v1.9では動作していたが v3.2では無効と判断されてしまう
//コードはコメントアウトして残した
//            int ret = flb_input_property_check(s_flbContext,
//                                              flbin, &key[0], &value[0]);
//            if (ret == FLB_LIB_ERROR || ret == FLB_LIB_NO_CONFIG_MAP)
//              {
//                std::cerr << "Unknown property for \"" << plugin << "\" plugin: ";
//                std::cerr << key << ": " << value << std::endl;
//              }
            flb_input_set(s_flbContext, flbin, key.c_str(), value.c_str(), NULL);
          }
      }
    else {
      std::cout << "createInputStream : Default settings for input.conf : tag:rtclog" << std::endl;
      flb_input_set(s_flbContext, flbin, "tag", "rtclog", NULL);
    }
    return true;
  }

  std::streamsize FluentBitStream::pushLogger(int level, const std::string &name, const std::string &date, const std::string &mes)
  {
    std::streamsize n(0);
    coil::Properties& conf = ::RTC::Manager::instance().getConfig();
    const std::string& pid = conf["manager.pid"];
    const std::string& host_name = conf["os.hostname"];
    const std::string& manager_name = conf["manager.instance_name"];
    std::ostringstream oss;
    
    for(auto & flb : m_flbIn)
      {
        oss << "[";
		    oss << std::time(nullptr);
		    oss << ", {";
		    oss << "\"time\":\"" << date << "\",";
		    oss << "\"name\":\"" << name << "\",";
		    oss << "\"level\":\"" << Logger::getLevelString(level) << "\",";
		    oss << "\"pid\":\"" << pid << "\",";
		    oss << "\"host\":\"" << host_name << "\",";
		    oss << "\"manager\":\"" << manager_name << "\",";
		    oss << "\"message\":\"" << mes << "\"";
		    oss << "}]";
		    
		    std::string formatted = oss.str();
		    n = formatted.size();

        flb_lib_push(s_flbContext, flb, formatted.c_str(), n);
      }
    return n;
  }
  
  void FluentBitStream::write(int level, const std::string &name, const std::string &date, const std::string &mes)
  {
    std::string message{coil::replaceString(mes, "\"", "\'")};
    std::string line;

	for (char c : message)
    {
      if (c == '\n' || c == '\r') {
        if (!line.empty()) {
            pushLogger(level, name, date, line);
            line.clear();
        }
      } else {
        line += c;
      }
    }

    // 最後に残った行も送信
    if (!line.empty()) {
      pushLogger(level, name, date, line);
    }
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
