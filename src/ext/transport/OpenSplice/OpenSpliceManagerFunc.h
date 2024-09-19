// -*- C++ -*-
/*!
 * @file  OpenSpliceManagerFunc.h
 * @brief OpenSpliceManagerFunc class
 * @date  $Date: 2019-02-04 03:08:03 $
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

#ifndef RTC_OPENSPLICEMANAGERFUNC_H
#define RTC_OPENSPLICEMANAGERFUNC_H

#include <string>

namespace RTC_OpenSplice
{
    void start(coil::Properties& prop);
    void shutdown();
    bool registerType(const std::string& datatype, const std::string& idlpath);
    bool registeredType(const std::string& datatype);
}


#endif // RTC_OPENSPLICEMANAGERFUNC_H

