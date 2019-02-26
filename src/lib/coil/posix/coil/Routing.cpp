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

#include <cstdio>
#include <utility>
#include <netdb.h>       // gethostbyname
#include <arpa/inet.h>   // inet_ntoa
#include <netinet/in.h>  // sockaddr_in
#include <sys/wait.h>

#include <coil/Routing.h>
#include <coil/stringutil.h>
#include <coil/config_coil.h>

namespace coil
{
  /*!
   * @if jp
   * @brief 宛先アドレスから利用されるエンドポイントアドレスを得る
   * @else
   * @brief Getting network interface name from destination address
   * @endif
   */
  bool dest_to_endpoint(std::string dest_addr, std::string& endpoint)
  {
    std::string dest_if = std::string();
    if (!find_dest_ifname(std::move(dest_addr), dest_if))
      {
        return false;
      }
    return ifname_to_ipaddr(dest_if, endpoint);
  }

  /*!
   * @if jp
   * @brief 宛先アドレスから利用されるネットワークインターフェース名を得る
   * @else
   * @brief Getting network interface name from destination address
   * @endif
   */
  bool find_dest_ifname(std::string dest_addr, std::string& dest_if)
  {
    // This logic should be replaced by direct retrieving using
    // routing interface like AFROUTE or sysctl.
    struct ::hostent *hostent;
    struct ::sockaddr_in addr;

    hostent = gethostbyname(dest_addr.c_str());
    addr.sin_addr.s_addr = **(unsigned int **)(hostent->h_addr_list);
    dest_addr = inet_ntoa(addr.sin_addr);

#if defined(COIL_OS_FREEBSD) || defined(COIL_OS_DARWIN) \
                             || defined(COIL_OS_CYGWIN) \
                             || defined(COIL_OS_QNX)
    std::string cmd("PATH=/bin:/sbin:/usr/bin:/usr/sbin "
                    "route get ");
    const char* match_str = "interface";
    const char* delimiter = ":";
    size_t ifname_pos(1);
    cmd += dest_addr;
    cmd += " 2> /dev/null";
#endif  // COIL_OS_IS_FREEBSD || COIL_OS_DARWIN || COIL_OS_CYGWIN || COIL_OS_QNX
#if defined(COIL_OS_LINUX)
    std::string cmd("PATH=/bin:/sbin:/usr/bin:/usr/sbin "
                    "ip route get ");
    const char* match_str = "dev ";
    const char* delimiter = " ";
    cmd += dest_addr;
    cmd += " 2> /dev/null";
#endif  // COIL_OS_IS_LINUX

    FILE* fp;
    if ((fp = popen(cmd.c_str(), "r")) == nullptr)
      {
        return false;
      }

    do
      {
        char str[512];
        fgets(str, 512, fp);
        std::string line(str);

        if (std::string::npos == line.find(match_str)) { continue; }

        line.erase(line.end() - 1);
        coil::vstring vs(coil::split(line, delimiter));

#if defined(COIL_OS_FREEBSD) || defined(COIL_OS_DARWIN) \
                             || defined(COIL_OS_CYGWIN) \
                             || defined(COIL_OS_QNX)
        if (vs.size() > ifname_pos)
          {
            dest_if = vs[ifname_pos];
            pclose(fp);
            wait(NULL);
            return true;
          }
#endif  // COIL_OS_FREEBSD || COIL_OS_DARWIN || COIL_OS_CYGWIN || COIL_OS_QNX
#if defined(COIL_OS_LINUX)
        for (int i(0); i < vs.size(); ++i)
          {
            if (vs[i] == "dev")
              {
                dest_if = vs[i + 1];
                pclose(fp);
                return true;
              }
          }
#endif  // COIL_OS_LINUX
      } while (!feof(fp));
    pclose(fp);
    wait(nullptr);
    return false;
  }

  /*!
   * @if jp
   * @brief ネットワークインターフェース名からIPアドレスを得る
   * @else
   * @brief Get IP address from a network interface name
   * @endif
   */
  bool ifname_to_ipaddr(const std::string& ifname, std::string& ipaddr)
  {
    std::string cmd("ifconfig ");
    cmd += ifname;
    cmd += " 2> /dev/null";

    FILE* fp;
    if ((fp = popen(cmd.c_str(), "r")) == nullptr)
      {
        return false;
      }

    do
      {
        char str[512];
        fgets(str, 512, fp);
        std::string line(str);

        if (std::string::npos == line.find("inet ")) { continue; }

        line.erase(line.end() - 1);
        coil::eraseHeadBlank(line);
        coil::vstring vs(coil::split(line, " "));
        if (vs.size() == 6)
          {
            ipaddr = vs[1];
            pclose(fp);
            wait(nullptr);
            return true;
          }
      } while (!feof(fp));
    pclose(fp);
    wait(nullptr);
    return false;
  }

};  // namespace coil
