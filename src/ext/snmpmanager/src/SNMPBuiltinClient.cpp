// -*- C++ -*-
/*!
 * @file SNMPBuiltinClient.cpp
 * @brief SNMP sending and receiving (built-in implementation; encodes SNMPv2c with SNMPBerCodec.cpp and sends/receives over UDP)
 *
 * @~japanese
 * SNMP の送受信 (組み込みの実装。SNMPv2c を SNMPBerCodec.cpp で符号化し、UDP で送受信する)
 *
 * Net-SNMP 版 (SNMPClient.cpp) と同じ動作: GET、GETNEXT による WALK、v2c トラップの送信と受信 (v1 のトラップも読む)。
 * @~english
 * Same behavior as the Net-SNMP version (SNMPClient.cpp): GET, WALK by GETNEXT, sending and receiving v2c traps (v1 traps are also read).
 * @~
 *
 * @date $Date: 2026-11-13 16:24:31 $
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

#include "SNMPClient.hpp"
#include "SNMPBerCodec.hpp"
#include "SNMPManagerUtil.hpp"

#include <chrono>
#include <cstring>
#include <mutex>
#include <random>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
static const socket_t kBadSocket = INVALID_SOCKET;
static void close_socket(socket_t s) { closesocket(s); }
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
static const socket_t kBadSocket = -1;
static void close_socket(socket_t s) { ::close(s); }
#endif

namespace snmpmgr
{
  namespace
  {
    /*!
     * @~japanese
     * @brief 次の要求の ID を返す
     *
     * @return 要求の ID
     * @~english
     * @brief Returns the next request ID
     *
     * @return request ID
     * @~
     */
    int32_t next_request_id()
    {
      static std::mutex m;
      static std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
      std::lock_guard<std::mutex> g(m);
      return static_cast<int32_t>(rng() & 0x7fffffff);
    }

    /*!
     * @~japanese
     * @brief IP アドレスとポートからソケットのアドレスを作る
     *
     * @param ip IP アドレス
     * @param port ポート番号
     * @param a アドレス
     * @return 成功すれば true
     * @~english
     * @brief Makes a socket address from an IP address and port
     *
     * @param ip IP address
     * @param port port number
     * @param a address
     * @return true on success
     * @~
     */
    bool make_addr(const std::string& ip, int port, sockaddr_in& a)
    {
      std::memset(&a, 0, sizeof(a));
      a.sin_family = AF_INET;
      a.sin_port = htons(static_cast<uint16_t>(port));
      return inet_pton(AF_INET, ip.c_str(), &a.sin_addr) == 1;
    }

    /*!
     * @~japanese
     * @brief ソケットが読めるようになるまで待つ
     *
     * @param s ソケット
     * @param timeout タイムアウト (秒)
     * @return 読めれば true
     * @~english
     * @brief Waits until a socket becomes readable
     *
     * @param s socket
     * @param timeout timeout (seconds)
     * @return true if readable
     * @~
     */
    bool wait_readable(socket_t s, double timeout)
    {
#ifdef _WIN32
      WSAPOLLFD p{};
      p.fd = s;
      p.events = POLLRDNORM;
      return WSAPoll(&p, 1, static_cast<int>(timeout * 1000.0)) > 0;
#else
      pollfd p{};
      p.fd = s;
      p.events = POLLIN;
      return ::poll(&p, 1, static_cast<int>(timeout * 1000.0)) > 0;
#endif
    }

    /*!
     * @~japanese
     * @brief 要求を送り、同じ request-id の応答を待つ (再送 retries 回)
     *
     * @param community コミュニティ名
     * @param ip IP アドレス
     * @param port ポート番号
     * @param pdu 要求の PDU
     * @param timeout タイムアウト (秒)
     * @param retries 再送の回数
     * @return 応答の PDU (なければ nullopt)
     * @~english
     * @brief Sends a request and waits for the response with the same request-id (retries times)
     *
     * @param community community name
     * @param ip IP address
     * @param port port number
     * @param pdu request PDU
     * @param timeout timeout (seconds)
     * @param retries number of retries
     * @return response PDU (nullopt if none)
     * @~
     */
    std::optional<ber::Pdu> request(const std::string& community, const std::string& ip, int port, ber::Pdu pdu,
                                    double timeout, int retries)
    {
      sockaddr_in to{};
      if (!make_addr(ip, port, to)) { return std::nullopt; }
      socket_t s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (s == kBadSocket) { return std::nullopt; }
      pdu.request_id = next_request_id();
      std::string msg = ber::encode_message(community, pdu);
      std::optional<ber::Pdu> out;
      char buf[65536];
      for (int attempt = 0; attempt <= retries && !out; ++attempt)
        {
          if (::sendto(s, msg.data(), static_cast<int>(msg.size()), 0, reinterpret_cast<sockaddr*>(&to), sizeof(to)) < 0) { break; }
          double deadline = monotonic() + timeout;
          while (!out)
            {
              double rest = deadline - monotonic();
              if (rest <= 0 || !wait_readable(s, rest)) { break; }
              sockaddr_in from{};
              socklen_t fl = sizeof(from);
              int n = static_cast<int>(::recvfrom(s, buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(&from), &fl));
              if (n <= 0) { continue; }
              auto r = ber::decode_message(std::string(buf, static_cast<size_t>(n)));
              if (r && r->type == ber::RESPONSE && r->request_id == pdu.request_id) { out = std::move(r); }
            }
        }
      close_socket(s);
      return out;
    }
  } // namespace

  void SnmpClient::global_init()
  {
#ifdef _WIN32
    static std::once_flag once;
    std::call_once(once, [] {
      WSADATA d;
      WSAStartup(MAKEWORD(2, 2), &d);
    });
#endif
  }

  SnmpClient::SnmpClient(std::string community, int port) : community_(std::move(community)), port_(port)
  {
    global_init();
  }

  std::optional<SnmpClient::Values> SnmpClient::get(const std::string& ip, const std::vector<std::string>& oids,
                                                     double timeout, int retries) const
  {
    ber::Pdu pdu;
    pdu.type = ber::GET;
    for (auto& o : oids)
      {
        if (o.empty()) { continue; }
        pdu.vars.push_back({o[0] == '.' ? o.substr(1) : o, ber::NULL_, ""});
      }
    std::optional<ber::Pdu> r;
    try { r = request(community_, ip, port_, pdu, timeout, retries); }
    catch (std::exception&) { return std::nullopt; }     // OID が読めない
    if (!r || r->error_status != 0) { return std::nullopt; }
    Values v;
    for (auto& var : r->vars)
      {
        auto s = ber::value_string(var);
        if (s) { v[var.oid] = *s; }
      }
    return v;
  }

  std::optional<int> SnmpClient::find_index(const std::string& ip, const std::string& base,
                                            const std::string& value) const
  {
    std::string root = (!base.empty() && base[0] == '.') ? base.substr(1) : base;
    std::string cur = root;
    for (int guard = 0; guard < 1024; ++guard)
      {
        ber::Pdu pdu;
        pdu.type = ber::GETNEXT;
        pdu.vars.push_back({cur, ber::NULL_, ""});
        std::optional<ber::Pdu> r;
        try { r = request(community_, ip, port_, pdu, 2.0, 1); }
        catch (std::exception&) { return std::nullopt; }
        if (!r || r->error_status != 0 || r->vars.empty()) { return std::nullopt; }
        auto& var = r->vars.front();
        if (var.type == ber::END_OF_MIB_VIEW || !ber::oid_under(var.oid, root)) { return std::nullopt; }
        auto s = ber::value_string(var);
        // Windows の Net-SNMP は ifDescr の末尾に NUL を付けて返すので、比べる前に取り除く
        if (s) { while (!s->empty() && s->back() == '\0') { s->pop_back(); } }
        if (s && *s == value)
          {
            auto last = ber::oid_last(var.oid);
            return last ? std::optional<int>(static_cast<int>(*last)) : std::nullopt;
          }
        cur = var.oid;
      }
    return std::nullopt;
  }

  std::pair<bool, double> SnmpClient::probe(const std::string& ip, const std::string& o, double timeout) const
  {
    double t0 = monotonic();
    auto v = get(ip, {o}, timeout, 0);
    return {v.has_value() && !v->empty(), (monotonic() - t0) * 1000.0};
  }

  bool SnmpClient::send_trap(const std::string& ip, int port, const std::string& name_oid, const std::string& message,
                             const std::string& info_oid, const std::string& info) const
  {
    sockaddr_in to{};
    if (!make_addr(ip, port, to)) { return false; }
    ber::Pdu pdu;
    pdu.type = ber::TRAP_V2;
    pdu.request_id = next_request_id();
    long long ticks = static_cast<long long>(monotonic() * 100.0) & 0x7fffffff;
    pdu.vars.push_back({"1.3.6.1.2.1.1.3.0", ber::TIMETICKS, std::to_string(ticks)});         // sysUpTime.0
    pdu.vars.push_back({"1.3.6.1.6.3.1.1.4.1.0", ber::OBJECT_ID, "1.3.6.1.6.3.1.1.5.1"});    // snmpTrapOID.0
    auto strip = [](const std::string& o) { return (!o.empty() && o[0] == '.') ? o.substr(1) : o; };
    if (!name_oid.empty()) { pdu.vars.push_back({strip(name_oid), ber::OCTET_STRING, message}); }
    if (!info_oid.empty()) { pdu.vars.push_back({strip(info_oid), ber::OCTET_STRING, info}); }
    std::string msg;
    try { msg = ber::encode_message(community_, pdu); }
    catch (std::exception&) { return false; }
    socket_t s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == kBadSocket) { return false; }
    if (!local_address_.empty())
      {
        // 受け手は送り元の IP でロボットを見分けるので、自分の IP から送る (snmp.own_ip)
        sockaddr_in from{};
        if (!make_addr(local_address_, 0, from) || ::bind(s, reinterpret_cast<sockaddr*>(&from), sizeof(from)) != 0)
          {
            close_socket(s);
            return false;
          }
      }
    bool ok = ::sendto(s, msg.data(), static_cast<int>(msg.size()), 0, reinterpret_cast<sockaddr*>(&to), sizeof(to)) >= 0;
    close_socket(s);
    return ok;
  }

  // ------------------------------------------------------------------ trap receiver
  TrapReceiver::TrapReceiver(int port, Callback cb) : port_(port), cb_(std::move(cb)) { SnmpClient::global_init(); }

  TrapReceiver::~TrapReceiver() { stop(); }

  bool TrapReceiver::start(std::string& error)
  {
    socket_t s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == kBadSocket) { error = "cannot create a UDP socket"; return false; }
    int on = 1;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof(on));
    sockaddr_in a{};
    if (!make_addr(address_, port_, a) || ::bind(s, reinterpret_cast<sockaddr*>(&a), sizeof(a)) != 0)
      {
        close_socket(s);
        error = "cannot open udp:" + address_ + ":" + std::to_string(port_);
        return false;
      }
    sock_ = static_cast<long long>(s);
    running_ = true;
    thread_ = std::thread([this] { run(); });
    return true;
  }

  void TrapReceiver::run()
  {
    socket_t s = static_cast<socket_t>(sock_);
    char buf[65536];
    while (running_)
      {
        if (!wait_readable(s, 0.5)) { continue; }
        sockaddr_in from{};
        socklen_t fl = sizeof(from);
        int n = static_cast<int>(::recvfrom(s, buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(&from), &fl));
        if (n <= 0) { continue; }
        std::string community;
        auto pdu = ber::decode_message(std::string(buf, static_cast<size_t>(n)), &community);
        if (!pdu || (pdu->type != ber::TRAP_V2 && pdu->type != ber::TRAP_V1 && pdu->type != ber::INFORM)) { continue; }
        char ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
        if (pdu->type == ber::INFORM)
          {
            // INFORM には応答を返す (Net-SNMP の受信と同じ)
            ber::Pdu resp = *pdu;
            resp.type = ber::RESPONSE;
            try
              {
                std::string m = ber::encode_message(community, resp);
                ::sendto(s, m.data(), static_cast<int>(m.size()), 0, reinterpret_cast<sockaddr*>(&from), fl);
              }
            catch (std::exception&) {}
          }
        SnmpClient::Values values;
        for (auto& var : pdu->vars)
          {
            auto v = ber::value_string(var);
            if (v) { values[var.oid] = *v; }
          }
        try { dispatch(ip, values); }
        catch (std::exception& e) { Log::info(std::string("trap callback failed: ") + e.what()); }
      }
  }

  void TrapReceiver::stop()
  {
    if (running_.exchange(false) && thread_.joinable()) { thread_.join(); }
    if (sock_ >= 0)
      {
        close_socket(static_cast<socket_t>(sock_));
        sock_ = -1;
      }
  }
} // namespace snmpmgr
