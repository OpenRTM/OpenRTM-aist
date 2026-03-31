// -*- C++ -*-
/*!
 * @file properties.cpp
 * @brief hrtm::Peoperties wrapper class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     National Institute of
 *     Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <hrtm/properties.h>

namespace hrtm
{
namespace utils
{
  Properties::Properties()
    : coil::Properties("", "")
  {
  }
  /*
  Properties::Properties(const char* const defaults[], long num)
    : coil::Properties(defaults, num)
  {
  }
  */
  Properties::~Properties() = default;
} // namespace utils
} // namespace hrtm
