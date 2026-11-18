// -*- C++ -*-
/*!
 * @file SNMPClient.cpp
 * @brief SNMP sending and receiving (Net-SNMP)
 *
 * @~japanese
 * SNMP の送受信 (Net-SNMP)
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
#include "SNMPManagerUtil.hpp"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <regex>
#include <sys/select.h>

namespace snmpmgr
{
  namespace
  {
    /*!
     * @~japanese
     * @brief Net-SNMP の OID を文字列にする
     *
     * @param name OID
     * @param len OID の長さ
     * @return OID の文字列
     * @~english
     * @brief Converts a Net-SNMP OID into a string
     *
     * @param name OID
     * @param len length of the OID
     * @return OID string
     * @~
     */
    std::string oid_to_string(const oid* name, size_t len)
    {
      std::string s;
      for (size_t i = 0; i < len; ++i)
        {
          if (i) { s += '.'; }
          s += std::to_string(name[i]);
        }
      return s;
    }

    /*!
     * @~japanese
     * @brief OID の文字列を Net-SNMP の OID にする
     *
     * @param text OID の文字列
     * @param out OID
     * @param len OID の長さ (入力は最大の長さ)
     * @return 成功すれば true
     * @~english
     * @brief Parses an OID string into a Net-SNMP OID
     *
     * @param text OID string
     * @param out OID
     * @param len length of the OID (input: maximum length)
     * @return true on success
     * @~
     */
    bool parse_oid(const std::string& text, oid* out, size_t* len)
    {
      std::string t = text;
      if (t.empty()) { return false; }
      if (t[0] != '.') { t = "." + t; }   // MIB を読まないので絶対表記にする
      return read_objid(t.c_str(), out, len) != 0;
    }

    /*!
     * @~japanese
     * @brief Net-SNMP の値を文字列にする
     *
     * @param v 変数
     * @return 文字列 (noSuchObject などは nullopt)
     * @~english
     * @brief Converts a Net-SNMP value into a string
     *
     * @param v variable
     * @return string (nullopt for noSuchObject etc.)
     * @~
     */
    std::optional<std::string> value_to_string(const netsnmp_variable_list* v)
    {
      switch (v->type)
        {
        case ASN_INTEGER: return std::to_string(*v->val.integer);
        case ASN_COUNTER: case ASN_GAUGE: case ASN_TIMETICKS: case ASN_UINTEGER:
          return std::to_string(static_cast<unsigned long>(*v->val.integer) & 0xffffffffUL);
        case ASN_COUNTER64:
          return std::to_string((static_cast<unsigned long long>(v->val.counter64->high) << 32) |
                                v->val.counter64->low);
        case ASN_OCTET_STR:
          return std::string(reinterpret_cast<const char*>(v->val.string), v->val_len);
        case ASN_OBJECT_ID:
          return oid_to_string(v->val.objid, v->val_len / sizeof(oid));
        case ASN_IPADDRESS:
          {
            const unsigned char* p = v->val.string;
            return std::to_string(p[0]) + "." + std::to_string(p[1]) + "." + std::to_string(p[2]) + "." +
                   std::to_string(p[3]);
          }
        case SNMP_NOSUCHOBJECT: case SNMP_NOSUCHINSTANCE: case SNMP_ENDOFMIBVIEW: case ASN_NULL:
          return std::nullopt;
        default:
          return std::nullopt;
        }
    }

    /*!
     * @~japanese
     * @brief Net-SNMP のセッションを開く
     *
     * @param community コミュニティ名
     * @param ip IP アドレス
     * @param port ポート番号
     * @param timeout タイムアウト (秒)
     * @param retries 再送の回数
     * @param local 送り元のアドレス (空なら OS に任せる)
     * @return セッション (失敗なら nullptr)
     * @~english
     * @brief Opens a Net-SNMP session
     *
     * @param community community name
     * @param ip IP address
     * @param port port number
     * @param timeout timeout (seconds)
     * @param retries number of retries
     * @param local source address (left to the OS if empty)
     * @return session (nullptr on failure)
     * @~
     */
    void* open_session(const std::string& community, const std::string& ip, int port, double timeout, int retries,
                       const std::string& local = "")
    {
      netsnmp_session s;
      snmp_sess_init(&s);
      s.version = SNMP_VERSION_2c;
      if (!local.empty()) { s.localname = const_cast<char*>(local.c_str()); }
      std::string peer = "udp:" + ip + ":" + std::to_string(port);
      s.peername = const_cast<char*>(peer.c_str());
      s.community = reinterpret_cast<u_char*>(const_cast<char*>(community.c_str()));
      s.community_len = community.size();
      s.timeout = static_cast<long>(timeout * 1000000.0);
      s.retries = retries;
      return snmp_sess_open(&s);   // peername / community は内部でコピーされる
    }
  } // namespace

  void SnmpClient::global_init()
  {
    static std::once_flag once;
    std::call_once(once, [] {
      setenv("MIBS", "", 1);              // MIB ファイルは読まない (数値 OID だけを使う)
      netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DONT_PERSIST_STATE, 1);
      netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DISABLE_PERSISTENT_LOAD, 1);
      init_snmp("snmpmanager");
    });
  }

  SnmpClient::SnmpClient(std::string community, int port) : community_(std::move(community)), port_(port)
  {
    global_init();
  }

  std::optional<SnmpClient::Values> SnmpClient::get(const std::string& ip, const std::vector<std::string>& oids,
                                                     double timeout, int retries) const
  {
    void* sp = open_session(community_, ip, port_, timeout, retries);
    if (!sp) { return std::nullopt; }
    netsnmp_pdu* pdu = snmp_pdu_create(SNMP_MSG_GET);
    for (auto& o : oids)
      {
        oid name[MAX_OID_LEN];
        size_t len = MAX_OID_LEN;
        if (parse_oid(o, name, &len)) { snmp_add_null_var(pdu, name, len); }
      }
    netsnmp_pdu* resp = nullptr;
    int status = snmp_sess_synch_response(sp, pdu, &resp);
    std::optional<Values> out;
    if (status == STAT_SUCCESS && resp && resp->errstat == SNMP_ERR_NOERROR)
      {
        Values v;
        for (auto* var = resp->variables; var; var = var->next_variable)
          {
            auto s = value_to_string(var);
            if (s) { v[oid_to_string(var->name, var->name_length)] = *s; }
          }
        out = std::move(v);
      }
    if (resp) { snmp_free_pdu(resp); }
    snmp_sess_close(sp);
    return out;
  }

  std::optional<int> SnmpClient::find_index(const std::string& ip, const std::string& base,
                                            const std::string& value) const
  {
    void* sp = open_session(community_, ip, port_, 2.0, 1);
    if (!sp) { return std::nullopt; }
    oid root[MAX_OID_LEN], cur[MAX_OID_LEN];
    size_t root_len = MAX_OID_LEN;
    if (!parse_oid(base, root, &root_len)) { snmp_sess_close(sp); return std::nullopt; }
    std::memcpy(cur, root, root_len * sizeof(oid));
    size_t cur_len = root_len;
    std::optional<int> found;
    for (int guard = 0; guard < 1024 && !found; ++guard)
      {
        netsnmp_pdu* pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, cur, cur_len);
        netsnmp_pdu* resp = nullptr;
        int status = snmp_sess_synch_response(sp, pdu, &resp);
        if (status != STAT_SUCCESS || !resp || resp->errstat != SNMP_ERR_NOERROR || !resp->variables)
          {
            if (resp) { snmp_free_pdu(resp); }
            break;
          }
        auto* var = resp->variables;
        bool inside = var->name_length > root_len &&
                      std::memcmp(var->name, root, root_len * sizeof(oid)) == 0 &&
                      var->type != SNMP_ENDOFMIBVIEW;
        if (inside)
          {
            auto s = value_to_string(var);
            // Windows の Net-SNMP は ifDescr の末尾に NUL を付けて返すので、比べる前に取り除く
            if (s) { while (!s->empty() && s->back() == '\0') { s->pop_back(); } }
            if (s && *s == value) { found = static_cast<int>(var->name[var->name_length - 1]); }
            std::memcpy(cur, var->name, var->name_length * sizeof(oid));
            cur_len = var->name_length;
          }
        snmp_free_pdu(resp);
        if (!inside) { break; }
      }
    snmp_sess_close(sp);
    return found;
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
    void* sp = open_session(community_, ip, port, 1.0, 0, local_address_);
    if (!sp) { return false; }
    netsnmp_pdu* pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
    oid uptime[] = {1, 3, 6, 1, 2, 1, 1, 3, 0};
    oid trapoid[] = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};
    long ticks = static_cast<long>(monotonic() * 100.0) & 0x7fffffff;
    snmp_add_var(pdu, uptime, OID_LENGTH(uptime), 't', std::to_string(ticks).c_str());
    snmp_add_var(pdu, trapoid, OID_LENGTH(trapoid), 'o', ".1.3.6.1.6.3.1.1.5.1");
    oid n1[MAX_OID_LEN], n2[MAX_OID_LEN];
    size_t l1 = MAX_OID_LEN, l2 = MAX_OID_LEN;
    if (parse_oid(name_oid, n1, &l1)) { snmp_pdu_add_variable(pdu, n1, l1, ASN_OCTET_STR, message.data(), message.size()); }
    if (parse_oid(info_oid, n2, &l2)) { snmp_pdu_add_variable(pdu, n2, l2, ASN_OCTET_STR, info.data(), info.size()); }
    bool ok = snmp_sess_send(sp, pdu) != 0;
    if (!ok) { snmp_free_pdu(pdu); }
    snmp_sess_close(sp);
    return ok;
  }

  // ------------------------------------------------------------------ trap receiver
  namespace
  {
    /*!
     * @~japanese
     * @brief トラップを受けたときの Net-SNMP のコールバック
     *
     * @param op 操作
     * @param session セッション
     * @param pdu PDU
     * @param magic TrapReceiver
     * @return 1
     * @~english
     * @brief Net-SNMP callback for received traps
     *
     * @param op operation
     * @param session session
     * @param pdu PDU
     * @param magic TrapReceiver
     * @return 1
     * @~
     */
    int trap_callback(int op, netsnmp_session* session, int, netsnmp_pdu* pdu, void* magic)
    {
      if (op != NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE || !pdu) { return 1; }
      if (pdu->command != SNMP_MSG_TRAP2 && pdu->command != SNMP_MSG_TRAP && pdu->command != SNMP_MSG_INFORM)
        {
          return 1;
        }
      (void)session;
      auto* rx = static_cast<TrapReceiver*>(magic);
      std::string ip;
      auto* t = static_cast<netsnmp_transport*>(rx->transport());
      if (pdu->transport_data && t)
        {
          // f_fmtaddr: "UDP: [送信元]:port->[自分]:162"
          if (t->f_fmtaddr)
            {
              char* s = t->f_fmtaddr(t, pdu->transport_data, pdu->transport_data_length);
              if (s)
                {
                  std::smatch m;
                  std::string str(s);
                  if (std::regex_search(str, m, std::regex(R"(\[([0-9.]+)\])"))) { ip = m[1]; }
                  free(s);
                }
            }
        }
      SnmpClient::Values values;
      for (auto* var = pdu->variables; var; var = var->next_variable)
        {
          auto v = value_to_string(var);
          if (v) { values[oid_to_string(var->name, var->name_length)] = *v; }
        }
      rx->dispatch(ip, values);
      return 1;
    }
  } // namespace

  TrapReceiver::TrapReceiver(int port, Callback cb) : port_(port), cb_(std::move(cb)) { SnmpClient::global_init(); }

  TrapReceiver::~TrapReceiver() { stop(); }

  bool TrapReceiver::start(std::string& error)
  {
    std::string addr = "udp:" + address_ + ":" + std::to_string(port_);
    netsnmp_transport* t = netsnmp_transport_open_server("snmptrap", addr.c_str());
    if (!t) { error = "cannot open " + addr; return false; }
    transport_ = t;
    netsnmp_session s;
    snmp_sess_init(&s);
    s.peername = SNMP_DEFAULT_PEERNAME;
    s.version = SNMP_DEFAULT_VERSION;
    s.community_len = SNMP_DEFAULT_COMMUNITY_LEN;
    s.retries = SNMP_DEFAULT_RETRIES;
    s.timeout = SNMP_DEFAULT_TIMEOUT;
    s.callback = trap_callback;
    s.callback_magic = this;
    s.authenticator = nullptr;
    s.isAuthoritative = SNMP_SESS_UNKNOWNAUTH;
    session_ = snmp_sess_add(&s, t, nullptr, nullptr);
    if (!session_) { error = "snmp_sess_add failed"; return false; }
    running_ = true;
    thread_ = std::thread([this] { run(); });
    return true;
  }

  void TrapReceiver::run()
  {
    while (running_)
      {
        int numfds = 0, block = 0;
        fd_set fds;
        FD_ZERO(&fds);
        struct timeval tv{0, 500000};
        snmp_sess_select_info(session_, &numfds, &fds, &tv, &block);
        struct timeval wait{0, 500000};
        int n = select(numfds, &fds, nullptr, nullptr, &wait);
        if (n > 0) { snmp_sess_read(session_, &fds); }
        else if (n == 0) { snmp_sess_timeout(session_); }
      }
  }

  void TrapReceiver::stop()
  {
    if (running_.exchange(false) && thread_.joinable()) { thread_.join(); }
    if (session_) { snmp_sess_close(session_); session_ = nullptr; }
  }
} // namespace snmpmgr
