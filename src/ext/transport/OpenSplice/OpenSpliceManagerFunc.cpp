// -*- C++ -*-
/*!
 * @file  OpenSpliceManagerFunc.cpp
 * @brief OpenSpliceManagerFunc class
 * @date  $Date: 2019-01-31 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include "OpenSpliceManager.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning(pop)
#endif
#include "OpenSpliceManagerFunc.h"

namespace RTC_OpenSplice
{
    void start(coil::Properties& prop)
    {
      RTC::OpenSpliceManager::init(prop);
    }
    void shutdown()
    {
      RTC::OpenSpliceManager::shutdown_global();
        
    }
    bool registerType(const std::string& datatype, const std::string& idlpath)
    {
      RTC::OpenSpliceManager& topicmgr = RTC::OpenSpliceManager::instance();
      return topicmgr.registerType(datatype, idlpath);
        
    }

    bool registeredType(const std::string& datatype)
    {
      RTC::OpenSpliceManager& topicmgr = RTC::OpenSpliceManager::instance();
      return topicmgr.registeredType(datatype);
    }
}

