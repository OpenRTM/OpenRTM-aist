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
#include <coil/config_coil.h>
#include <coil/stringutil.h>
#include <coil/Affinity.h>

namespace coil
{
  bool getProcCpuAffinity(CpuMask& cpu_mask)
  {
#ifndef COIL_OS_QNX
    pid_t pid(getpid());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    int result = sched_getaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }

    for (size_t i(0); i < CPU_SETSIZE; ++i)
      {
        if (CPU_ISSET(i, &cpu_set))
          {
            cpu_mask.push_back((unsigned int)i+1);
          }
      }
#endif
    return true;
  }

  bool setProcCpuAffinity(const std::vector<unsigned int>& mask)
  {
#ifndef COIL_OS_QNX
    pid_t pid(getpid());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    for (size_t i(0); i < mask.size(); ++i)
      {
        CPU_SET(i, &cpu_set);
      }

    int result = sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }
#endif
    return true;
  }

  bool setProcCpuAffinity(const std::string& cpu_mask)
  {
#ifndef COIL_OS_QNX
    coil::vstring tmp = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for (size_t i(0); i < tmp.size(); ++i)
      {
        int num;
        if (coil::stringTo(num, tmp[i].c_str()))
          {
            mask.push_back(num);
          }
      }
    return setProcCpuAffinity(mask);
#else
    return true;
#endif
    
  }

  bool getThreadCpuAffinity(CpuMask& cpu_mask)
  {
#ifndef COIL_OS_QNX
    pthread_t tid(pthread_self());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    int result = pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }

    for (size_t i(0); i < CPU_SETSIZE; ++i)
      {
        if (CPU_ISSET(i, &cpu_set))
          {
            cpu_mask.push_back((unsigned int)i+1);
          }
      }
#endif
    return true;
  }

  bool setThreadCpuAffinity(const std::vector<unsigned int>& mask)
  {
#ifndef COIL_OS_QNX
    pthread_t tid(pthread_self());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    for (size_t i(0); i < mask.size(); ++i)
      {
        CPU_SET(i, &cpu_set);
      }

    int result = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }
#endif
    return true;
  }

  bool setThreadCpuAffinity(const std::string& cpu_mask)
  {
#ifndef COIL_OS_QNX
    coil::vstring tmp = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for (size_t i(0); i < tmp.size(); ++i)
      {
        int num;
        if (coil::stringTo(num, tmp[i].c_str()))
          {
            mask.push_back(num);
          }
      }
    return setThreadCpuAffinity(mask);
#else
    return true;
#endif
  }
}; // namespace coil
