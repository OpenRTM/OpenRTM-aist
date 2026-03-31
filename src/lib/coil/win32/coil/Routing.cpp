// -*- C++ -*-
/*!
 * @file  Routing.cpp
 * @brief Network routing information handling functions
 * @date  $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2010
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */


#ifdef __arm__
#include <coil/Routing.h>
namespace coil
{
  bool dest_to_endpoint(std::string dest_addr, std::string& endpoint)
  {
    return false;
  }
};
#else

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>





#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include <coil/Routing.h>
#include <coil/stringutil.h>
#include <coil/config_coil.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

namespace coil
{
  // Winsock initializer
  class Winsock
  {
  public:
    Winsock() {
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    virtual ~Winsock()
    {
      WSACleanup();
    }
  };

  /*!
   * @if jp
   * @brief 宛先アドレスから利用されるエンドポイントアドレスを得る
   * @else
   * @brief Getting network interface name from destination address
   * @endif
   */
  bool dest_to_endpoint(std::string dest_addr, std::string& endpoint)
  {
    PADDRINFOA res;
    ADDRINFOA hints;

    WSADATA data;
    WSAStartup(MAKEWORD(2, 0), &data);

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    if (getaddrinfo(dest_addr.c_str(), nullptr, &hints, &res) != 0) {
        return false;
    }

    if (res->ai_family == AF_INET)
    {
        struct sockaddr_in *addr;
        addr = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
        char str_buffer[INET_ADDRSTRLEN] = { 0 };

        if (inet_ntop(AF_INET, &addr->sin_addr, str_buffer, sizeof(str_buffer)) != nullptr)
        {
            dest_addr = str_buffer;
        }
        else
        {
            freeaddrinfo(res);
            WSACleanup();
            return false;
        }
    }
    else
    {
        freeaddrinfo(res);
        WSACleanup();
        return false;
    }
    
    freeaddrinfo(res);
    WSACleanup();

    ULONG ipaddress;

    struct sockaddr_in ip_addr;
    ip_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, dest_addr.c_str(), &ip_addr.sin_addr.S_un.S_addr) == 1)
    {
        ipaddress = ip_addr.sin_addr.S_un.S_addr;
    }
    else
    {
        return false;
    }

    DWORD bestifindex;
    if (NO_ERROR != GetBestInterface(ipaddress, &bestifindex)) { return false; }

    PMIB_IPADDRTABLE ipaddr_table;
    ipaddr_table =
          reinterpret_cast<MIB_IPADDRTABLE *>(MALLOC(sizeof (MIB_IPADDRTABLE)));
    if (ipaddr_table == nullptr) { return false; }

    // Make an initial call to GetIpAddrTable to get the
    // necessary size into the size variable
    DWORD size(0);
    if (GetIpAddrTable(ipaddr_table, &size, 0) == ERROR_INSUFFICIENT_BUFFER)
      {
        FREE(ipaddr_table);
        ipaddr_table = reinterpret_cast<MIB_IPADDRTABLE *>(MALLOC(size));
      }
    if (ipaddr_table == nullptr) { return false; }
    if (GetIpAddrTable(ipaddr_table, &size, 0) != NO_ERROR) { return false; }

    for (int i(0); i < static_cast<int>(ipaddr_table->dwNumEntries); ++i)
      {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
        if (bestifindex == ipaddr_table->table[i].dwIndex)
          {
            IN_ADDR inipaddr;
            inipaddr.S_un.S_addr = static_cast<u_long>(ipaddr_table->table[i].dwAddr);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
            char str_buffer[INET_ADDRSTRLEN] = { 0 };
            if (inet_ntop(AF_INET, &inipaddr.S_un, str_buffer, sizeof(str_buffer)) != nullptr)
            {
                endpoint = str_buffer;
            }
            else
            {
                return false;
            }
            return true;
          }
      }
    return false;
  }
  
  
} // namespace coil

#endif
