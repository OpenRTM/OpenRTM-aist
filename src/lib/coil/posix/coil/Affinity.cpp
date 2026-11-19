// -*- C++ -*-
/*!
 * @file Affinity.cpp
 * @brief Processor affinity operation class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2016-2020
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
#if defined(COIL_OS_LINUX)
  namespace
  {
    // The libc macros index the opaque cpu_set_t representation internally.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    bool cpuIsSet(std::size_t index, const cpu_set_t& set)
    {
      return index < CPU_SETSIZE && CPU_ISSET(index, &set);
    }

    void setCpu(std::size_t index, cpu_set_t& set)
    {
      if (index < CPU_SETSIZE) { CPU_SET(index, &set); }
    }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
  }
#endif

  bool getProcCpuAffinity(CpuMask& cpu_mask)
  {
#if defined(COIL_OS_LINUX)
    pid_t pid(getpid());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    int result = sched_getaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }

    for (size_t i(0); i < CPU_SETSIZE; ++i)
      {
        if (cpuIsSet(i, cpu_set))
          {
            cpu_mask.emplace_back(static_cast<unsigned int>(i)+1);
          }
      }
#endif
    return true;
  }

  bool setProcCpuAffinity(const CpuMask& cpu_mask)
  {
#if defined(COIL_OS_LINUX)
    pid_t pid(getpid());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    for (size_t i(0); i < cpu_mask.size(); ++i)
      {
        setCpu(i, cpu_set);
      }

    int result = sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }
#endif
    return true;
  }

  bool setProcCpuAffinity(const std::string& cpu_mask)
  {
#if defined(COIL_OS_LINUX)
    coil::vstring masklist = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for (auto & maskstr : masklist)
      {
        int num;
        if (coil::stringTo(num, maskstr.c_str()))
          {
            mask.emplace_back(num);
          }
      }
    return setProcCpuAffinity(mask);
#else
    return true;    
#endif
  }

  bool getThreadCpuAffinity(CpuMask& cpu_mask)
  {
#if defined(COIL_OS_LINUX)
    pthread_t tid(pthread_self());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    int result = pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }

    for (size_t i(0); i < CPU_SETSIZE; ++i)
      {
        if (cpuIsSet(i, cpu_set))
          {
            cpu_mask.emplace_back(static_cast<unsigned int>(i)+1);
          }
      }
#endif
    return true;
  }

  bool setThreadCpuAffinity(const CpuMask& cpu_mask)
  {
#if defined(COIL_OS_LINUX)
    pthread_t tid(pthread_self());
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    for (size_t i(0); i < cpu_mask.size(); ++i)
      {
        setCpu(i, cpu_set);
      }

    int result = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu_set);
    if (result != 0) { return false; }
#endif
    return true;
  }

  bool setThreadCpuAffinity(const std::string& cpu_mask)
  {
#if defined(COIL_OS_LINUX)
    coil::vstring masklist = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for (auto & maskstr : masklist)
      {
        int num;
        if (coil::stringTo(num, maskstr.c_str()))
          {
            mask.emplace_back(num);
          }
      }
    return setThreadCpuAffinity(mask);
#else
    return true;
#endif
  }
} // namespace coil
