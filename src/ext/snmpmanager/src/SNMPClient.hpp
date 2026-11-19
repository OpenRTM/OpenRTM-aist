// -*- C++ -*-
/*!
 * @file SNMPClient.hpp
 * @brief SNMP GET / WALK / probe / trap sending and receiving
 *
 * @~japanese
 * SNMP の GET / WALK / プローブ / トラップ送受信
 *
 * 実装は 2 つ (CMake の SNMPMGR_SNMP で選ぶ):
 * - netsnmp: Net-SNMP (single-session API)。SNMPClient.cpp。Linux の既定
 * - builtin: SNMPv2c を自前で符号化して UDP で送受信する。SNMPBuiltinClient.cpp。Windows (Net-SNMP がない) の既定
 * @~english
 * There are two implementations (selected with SNMPMGR_SNMP in CMake):
 * - netsnmp: Net-SNMP (single-session API). SNMPClient.cpp. Default on Linux
 * - builtin: encodes SNMPv2c by itself and sends/receives over UDP. SNMPBuiltinClient.cpp. Default on Windows (where Net-SNMP is not available)
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

#ifndef SNMPMGR_SNMPCLIENT_HPP
#define SNMPMGR_SNMPCLIENT_HPP

#include <atomic>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace snmpmgr
{
  /*!
   * @~japanese
   * @brief SNMP のクライアント (GET / WALK / プローブ / トラップの送信)
   * @~english
   * @brief SNMP client (GET / WALK / probe / trap sending)
   * @~
   */
  class SnmpClient
  {
  public:
    using Values = std::map<std::string, std::string>;   // 数値 OID (先頭の '.' なし) -> 値の文字列

    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param community コミュニティ名
     * @param port SNMP のポート番号
     * @~english
     * @brief Constructor
     *
     * @param community community name
     * @param port SNMP port number
     * @~
     */
    SnmpClient(std::string community, int port);
    /*!
     * @~japanese
     * @brief SNMP のライブラリを初期化する (1 回だけ)
     * @~english
     * @brief Initializes the SNMP library (once)
     * @~
     */
    static void global_init();

    /*!
     * @~japanese
     * @brief GET する。noSuchObject などの OID は結果に含めない
     *
     * @param ip IP アドレス
     * @param oids OID のリスト
     * @param timeout タイムアウト (秒)
     * @param retries 再送の回数
     * @return {OID: 値} (応答がない・エラーなら nullopt)
     * @~english
     * @brief Performs GET. OIDs such as noSuchObject are not included in the result
     *
     * @param ip IP address
     * @param oids list of OIDs
     * @param timeout timeout (seconds)
     * @param retries number of retries
     * @return {OID: value} (nullopt if no response or error)
     * @~
     */
    std::optional<Values> get(const std::string& ip, const std::vector<std::string>& oids,
                              double timeout = 2.0, int retries = 1) const;
    /*!
     * @~japanese
     * @brief base 以下を WALK して値が value と一致する OID の最後の番号を返す (ifDescr -> ifIndex)
     *
     * @param ip IP アドレス
     * @param base WALK する OID
     * @param value 探す値
     * @return 番号 (なければ nullopt)
     * @~english
     * @brief WALKs under base and returns the last number of the OID whose value equals value (ifDescr -> ifIndex)
     *
     * @param ip IP address
     * @param base OID to walk
     * @param value value to find
     * @return number (nullopt if not found)
     * @~
     */
    std::optional<int> find_index(const std::string& ip, const std::string& base, const std::string& value) const;
    /*!
     * @~japanese
     * @brief 1 回だけ GET する (再送なし)
     *
     * @param ip IP アドレス
     * @param oid OID
     * @param timeout タイムアウト (秒)
     * @return (成功したか, 往復時間 ms)
     * @~english
     * @brief GETs once (no retries)
     *
     * @param ip IP address
     * @param oid OID
     * @param timeout timeout (seconds)
     * @return (whether it succeeded, round-trip time in ms)
     * @~
     */
    std::pair<bool, double> probe(const std::string& ip, const std::string& oid, double timeout) const;
    /*!
     * @~japanese
     * @brief v2c のトラップを送る (TrapName = message, TrapInfo = info)
     *
     * @param ip 送り先の IP アドレス
     * @param port ポート番号
     * @param name_oid TrapName の OID
     * @param message メッセージ
     * @param info_oid TrapInfo の OID
     * @param info 追加の情報
     * @return 送れたら true
     * @~english
     * @brief Sends a v2c trap (TrapName = message, TrapInfo = info)
     *
     * @param ip destination IP address
     * @param port port number
     * @param name_oid OID of TrapName
     * @param message message
     * @param info_oid OID of TrapInfo
     * @param info additional information
     * @return true if sent
     * @~
     */
    bool send_trap(const std::string& ip, int port, const std::string& name_oid, const std::string& message,
                   const std::string& info_oid, const std::string& info) const;
    /*!
     * @~japanese
     * @brief トラップの送り元のアドレスを設定する (受け手は送り元の IP でロボットを見分ける。rtc.conf の snmp.own_ip)
     *
     * @param ip 送り元の IP アドレス (空なら OS に任せる)
     * @~english
     * @brief Sets the source address of traps (receivers identify robots by the source IP; snmp.own_ip in rtc.conf)
     *
     * @param ip source IP address (left to the OS if empty)
     * @~
     */
    void set_local_address(const std::string& ip) { local_address_ = ip; }

  private:
    std::string community_;
    int port_;
    std::string local_address_;
  };

  /*!
   * @~japanese
   * @brief トラップの受信 (専用スレッド)。callback(送信元 IP, {OID: 値})
   * @~english
   * @brief Trap receiver (dedicated thread). callback(source IP, {OID: value})
   * @~
   */
  class TrapReceiver
  {
  public:
    using Callback = std::function<void(const std::string&, const SnmpClient::Values&)>;
    /*!
     * @~japanese
     * @brief コンストラクタ
     *
     * @param port 受けるポート番号
     * @param cb コールバック
     * @~english
     * @brief Constructor
     *
     * @param port port number to receive on
     * @param cb callback
     * @~
     */
    TrapReceiver(int port, Callback cb);
    /*!
     * @~japanese
     * @brief デストラクタ。受信を止める
     * @~english
     * @brief Destructor. Stops receiving
     * @~
     */
    ~TrapReceiver();
    /*!
     * @~japanese
     * @brief 受信を始める
     *
     * @param error 失敗したときの理由
     * @return 成功すれば true
     * @~english
     * @brief Starts receiving
     *
     * @param error reason on failure
     * @return true on success
     * @~
     */
    bool start(std::string& error);
    /*!
     * @~japanese
     * @brief 受信を止める
     * @~english
     * @brief Stops receiving
     * @~
     */
    void stop();
    /*!
     * @~japanese
     * @brief 受信するアドレスを設定する (start の前に呼ぶ。rtc.conf の snmp.trap_address / snmp.own_ip)
     *
     * @param address IP アドレス (既定 0.0.0.0)
     * @~english
     * @brief Sets the address to receive on (call before start; snmp.trap_address / snmp.own_ip in rtc.conf)
     *
     * @param address IP address (default 0.0.0.0)
     * @~
     */
    void set_address(const std::string& address) { address_ = address.empty() ? "0.0.0.0" : address; }
  private:
    /*!
     * @~japanese
     * @brief 受信のスレッドの本体
     * @~english
     * @brief Body of the receiving thread
     * @~
     */
    void run();
    int port_;
    std::string address_{"0.0.0.0"};
    Callback cb_;
    std::atomic<bool> running_{false};
    std::thread thread_;
    void* session_{nullptr};
    void* transport_{nullptr};
    long long sock_{-1};      // 組み込みの実装 (SNMPBuiltinClient.cpp) の受信ソケット
  public:
    /*!
     * @~japanese
     * @brief コールバックを呼ぶ (Net-SNMP のコールバックから呼ぶ)
     *
     * @param ip 送信元の IP アドレス
     * @param values {OID: 値}
     * @~english
     * @brief Calls the callback (called from the Net-SNMP callback)
     *
     * @param ip source IP address
     * @param values {OID: value}
     * @~
     */
    void dispatch(const std::string& ip, const SnmpClient::Values& values) { cb_(ip, values); }
    /*!
     * @~japanese
     * @brief Net-SNMP のトランスポートを返す
     *
     * @return トランスポート
     * @~english
     * @brief Returns the Net-SNMP transport
     *
     * @return transport
     * @~
     */
    void* transport() const { return transport_; }
  };
} // namespace snmpmgr

#endif
