// -*- C++ -*-
/*!
 * @file Affinity.cpp
 * @brief Processor affinity operation class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016
 *     Noriaki Ando
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <pthread.h>
#endif // _GNU_SOURCE
#include <coil/stringutil.h>
#include <coil/Affinity.h>

#include <taskLib.h>
#include <cpuset.h>

namespace coil
{
  bool getProcCpuAffinity(CpuMask& cpu_mask)
  {
    return false;
  }

  bool setProcCpuAffinity(std::vector<unsigned int> mask)
  {
    return false;
  }

  bool setProcCpuAffinity(std::string cpu_mask)
  {
    return false;
  }

  bool getThreadCpuAffinity(CpuMask& cpu_mask)
  {
    int pid = taskIdSelf();
    cpuset_t cpu_set;
    CPUSET_ZERO (cpu_set);
    
    cpu_mask.clear();

    STATUS result = taskCpuAffinityGet(pid, &cpu_set);
    if (result != OK) { return false; }
    
    cpu_mask.push_back((unsigned int)cpu_set);
    return true;
  }

  bool setThreadCpuAffinity(std::vector<unsigned int> mask)
  {
    int pid = taskIdSelf();
    cpuset_t cpu_set;
    CPUSET_ZERO (cpu_set);
    if(mask.size() > 0)
    {
        CPUSET_SET(cpu_set, mask[0]);
    }
    else
    {
	return false;
    }

    STATUS result = taskCpuAffinitySet(pid, cpu_set);


    if (result != OK) { return false; }

    return true;
  }

  bool setThreadCpuAffinity(std::string cpu_mask)
  {
    coil::vstring tmp = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for(auto & m: tmp)
      {
        int num;
        if (coil::stringTo(num, m.c_str()))
          {
            mask.push_back(num);
          }
      }
    return setThreadCpuAffinity(mask);
  }
} // namespace coil
