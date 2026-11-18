// -*- C++ -*-
/*!
 * @file OSPlatform.cpp
 * @brief Implementation of the OS-dependent parts. See OSPlatform.hpp
 *
 * @~japanese
 * OS に依存する部分の実装。説明は OSPlatform.hpp
 * @~
 *
 * @date $Date: 2026-11-13 16:24:28 $
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

#include "OSPlatform.hpp"

#include <cstdio>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <dlfcn.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace snmpmgr
{
  namespace platform
  {
    std::optional<uint32_t> parse_ipv4(const std::string& ip)
    {
      unsigned a, b, c, d;
      char tail;
      if (std::sscanf(ip.c_str(), "%u.%u.%u.%u%c", &a, &b, &c, &d, &tail) != 4) { return std::nullopt; }
      if (a > 255 || b > 255 || c > 255 || d > 255) { return std::nullopt; }
      return (a << 24) | (b << 16) | (c << 8) | d;
    }

    std::string format_ipv4(uint32_t v)
    {
      return std::to_string((v >> 24) & 255) + "." + std::to_string((v >> 16) & 255) + "." +
             std::to_string((v >> 8) & 255) + "." + std::to_string(v & 255);
    }

    std::set<std::string> local_ipv4_addresses()
    {
      std::set<std::string> out;
#ifdef _WIN32
      ULONG size = 16 * 1024;
      std::vector<unsigned char> buf(size);
      auto* aa = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buf.data());
      ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
      ULONG r = GetAdaptersAddresses(AF_INET, flags, nullptr, aa, &size);
      if (r == ERROR_BUFFER_OVERFLOW)
        {
          buf.resize(size);
          aa = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buf.data());
          r = GetAdaptersAddresses(AF_INET, flags, nullptr, aa, &size);
        }
      if (r != NO_ERROR) { return out; }
      for (auto* a = aa; a; a = a->Next)
        {
          if (a->OperStatus != IfOperStatusUp) { continue; }
          for (auto* u = a->FirstUnicastAddress; u; u = u->Next)
            {
              if (u->Address.lpSockaddr->sa_family != AF_INET) { continue; }
              auto* sin = reinterpret_cast<sockaddr_in*>(u->Address.lpSockaddr);
              std::string s = format_ipv4(ntohl(sin->sin_addr.s_addr));
              if (s.rfind("127.", 0) != 0) { out.insert(s); }
            }
        }
#else
      ifaddrs* ifa = nullptr;
      if (getifaddrs(&ifa) != 0) { return out; }
      for (auto* p = ifa; p; p = p->ifa_next)
        {
          if (!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) { continue; }
          std::string s = format_ipv4(ntohl(reinterpret_cast<sockaddr_in*>(p->ifa_addr)->sin_addr.s_addr));
          if (s.rfind("127.", 0) != 0) { out.insert(s); }
        }
      freeifaddrs(ifa);
#endif
      return out;
    }

    std::string executable_dir()
    {
#ifdef _WIN32
      wchar_t buf[MAX_PATH * 4];
      DWORD n = GetModuleFileNameW(nullptr, buf, static_cast<DWORD>(sizeof(buf) / sizeof(buf[0])));
      if (n == 0) { return "."; }
      return std::filesystem::path(std::wstring(buf, n)).parent_path().u8string();
#else
      char buf[4096];
      ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
      return n > 0 ? std::filesystem::path(std::string(buf, static_cast<size_t>(n))).parent_path().string() : ".";
#endif
    }

    void* dl_open(const std::string& path, std::string& error)
    {
#ifdef _WIN32
      std::wstring w = std::filesystem::u8path(path).wstring();
      HMODULE h = LoadLibraryExW(w.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
      if (!h) { error = "LoadLibrary failed (error " + std::to_string(GetLastError()) + ")"; }
      return reinterpret_cast<void*>(h);
#else
      void* h = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
      if (!h) { const char* e = dlerror(); error = e ? e : "dlopen failed"; }
      return h;
#endif
    }

    void* dl_symbol(void* handle, const char* name)
    {
#ifdef _WIN32
      return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle), name));
#else
      return dlsym(handle, name);
#endif
    }

    void dl_close(void* handle)
    {
      if (!handle) { return; }
#ifdef _WIN32
      FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
      dlclose(handle);
#endif
    }

    const char* plugin_suffix()
    {
#ifdef _WIN32
      return ".dll";
#else
      return ".so";
#endif
    }
  } // namespace platform
} // namespace snmpmgr
