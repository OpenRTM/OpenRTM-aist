// -*- C++ -*-
/*!
 * @file Affinity.cpp
 * @brief Processor affinity operation class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Nobuhiko Miyamoto
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */


#include <windows.h>
#include <coil/stringutil.h>
#include <coil/Affinity.h>

namespace coil
{
  DWORD_PTR listToCUPNUM(CpuMask &cpu_mask)
  {
    DWORD_PTR cpu_num = 0;
    for(auto & m : cpu_mask)
    {
      DWORD_PTR p = (DWORD_PTR)0x01 << m;
      cpu_num += p;
    }
    return cpu_num;
  }
  bool getProcCpuAffinity(CpuMask& cpu_mask)
  {
    DWORD_PTR processMask, systemMask = 0;
    HANDLE h = GetCurrentProcess();
	BOOL success = GetProcessAffinityMask(h, (PDWORD_PTR)&processMask, (PDWORD_PTR)&systemMask);
	if (success)
    {
		for (int i = 0; i < 32; i++)
		{
			if (processMask & ((DWORD_PTR)0x00000001 << i))
			{
				cpu_mask.push_back(i);
			}
		}
      return true;
    }
    else
    {
      return false;
    }
  }

  bool setProcCpuAffinity(std::vector<unsigned int> mask)
  {
    DWORD_PTR cpu_num = listToCUPNUM(mask);
    HANDLE h = GetCurrentProcess();
    BOOL success = SetProcessAffinityMask(h, cpu_num);
	if (success)
	{
		return true;
	}
	else
	{
		return false;
	}
  }

  bool setProcCpuAffinity(std::string cpu_mask)
  {
    coil::vstring masklist = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for (auto & maskstr : masklist)
      {
        int num;
        if (coil::stringTo(num, maskstr.c_str()))
          {
            mask.push_back(num);
          }
      }
    return setProcCpuAffinity(mask);
  }

  bool getThreadCpuAffinity(CpuMask& /*cpu_mask*/)
  {
    return true;
  }

  bool setThreadCpuAffinity(std::vector<unsigned int> mask)
  {
    DWORD_PTR cpu_num = listToCUPNUM(mask);
    HANDLE h = GetCurrentThread();
    DWORD_PTR success = SetThreadAffinityMask(h, cpu_num);
	if (success != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
  }

  bool setThreadCpuAffinity(std::string cpu_mask)
  {
    coil::vstring masklist = coil::split(cpu_mask, ",", true);
    CpuMask mask;
    for (auto & maskstr : masklist)
      {
        int num;
        if (coil::stringTo(num, maskstr.c_str()))
          {
            mask.push_back(num);
          }
      }
    return setThreadCpuAffinity(mask);
  }
} // namespace coil
