// -*- C++ -*-
/*!
 * @file SNMPBerCodec.hpp
 * @brief Encoding and decoding of SNMPv2c messages (BER). Used by the built-in SNMP implementation (SNMPBuiltinClient.cpp). OS-independent
 *
 * @~japanese
 * SNMPv2c のメッセージの符号化・復号 (BER)。組み込みの SNMP の実装 (SNMPBuiltinClient.cpp) が使う。OS に依存しない
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

#ifndef SNMPMGR_SNMPBER_HPP
#define SNMPMGR_SNMPBER_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace snmpmgr
{
  namespace ber
  {
    /*!
     * @~japanese
     * @brief 値の型と PDU の種類 (BER のタグ)
     * @~english
     * @brief Value types and PDU kinds (BER tags)
     * @~
     */
    enum Tag : uint8_t
    {
      INTEGER = 0x02, OCTET_STRING = 0x04, NULL_ = 0x05, OBJECT_ID = 0x06, SEQUENCE = 0x30,
      IP_ADDRESS = 0x40, COUNTER32 = 0x41, GAUGE32 = 0x42, TIMETICKS = 0x43, OPAQUE = 0x44, COUNTER64 = 0x46,
      NO_SUCH_OBJECT = 0x80, NO_SUCH_INSTANCE = 0x81, END_OF_MIB_VIEW = 0x82,
      // PDU
      GET = 0xA0, GETNEXT = 0xA1, RESPONSE = 0xA2, SET = 0xA3, TRAP_V1 = 0xA4, GETBULK = 0xA5, INFORM = 0xA6,
      TRAP_V2 = 0xA7, REPORT = 0xA8,
    };

    /*!
     * @~japanese
     * @brief 変数のバインディング (OID、型、値)
     * @~english
     * @brief Variable binding (OID, type, value)
     * @~
     */
    struct VarBind
    {
      std::string oid;          // 数値 OID (先頭の '.' なし)
      uint8_t type{NULL_};
      std::string value;        // 符号化: INTEGER / 符号なし整数 / OID は 10 進の文字列、OCTET STRING はそのまま
    };

    /*!
     * @~japanese
     * @brief SNMP の PDU
     * @~english
     * @brief SNMP PDU
     * @~
     */
    struct Pdu
    {
      uint8_t type{GET};
      int32_t request_id{0};
      int32_t error_status{0};
      int32_t error_index{0};
      std::vector<VarBind> vars;
    };

    /*!
     * @~japanese
     * @brief version 1 (= SNMPv2c) のメッセージにする
     *
     * @param community コミュニティ名
     * @param pdu PDU
     * @return 符号化したメッセージ
     * @~english
     * @brief Encodes a version 1 (= SNMPv2c) message
     *
     * @param community community name
     * @param pdu PDU
     * @return encoded message
     * @~
     */
    std::string encode_message(const std::string& community, const Pdu& pdu);
    /*!
     * @~japanese
     * @brief メッセージを読む (SNMPv1 / v2c。v1 のトラップは varbind だけを読む)
     *
     * @param data メッセージ
     * @param community コミュニティ名を受ける (省略可)
     * @return PDU (読めなければ nullopt)
     * @~english
     * @brief Decodes a message (SNMPv1 / v2c; only the varbinds of v1 traps are read)
     *
     * @param data message
     * @param community receives the community name (optional)
     * @return PDU (nullopt if it cannot be decoded)
     * @~
     */
    std::optional<Pdu> decode_message(const std::string& data, std::string* community = nullptr);
    /*!
     * @~japanese
     * @brief 値を文字列にする (Net-SNMP の実装と同じ: 数値は 10 進、文字列は中身、IpAddress は a.b.c.d)
     *
     * @param v 変数のバインディング
     * @return 文字列 (noSuchObject などと NULL は nullopt)
     * @~english
     * @brief Converts a value into a string (same as the Net-SNMP implementation: decimal numbers, string contents, a.b.c.d for IpAddress)
     *
     * @param v variable binding
     * @return string (nullopt for noSuchObject etc. and NULL)
     * @~
     */
    std::optional<std::string> value_string(const VarBind& v);
    /*!
     * @~japanese
     * @brief OID が root の下にあるか (前方一致)
     *
     * @param oid OID
     * @param root 根の OID
     * @return 下にあれば true
     * @~english
     * @brief Whether an OID is under root (prefix match)
     *
     * @param oid OID
     * @param root root OID
     * @return true if under root
     * @~
     */
    bool oid_under(const std::string& oid, const std::string& root);
    /*!
     * @~japanese
     * @brief OID の最後の番号を返す
     *
     * @param oid OID
     * @return 番号 (なければ nullopt)
     * @~english
     * @brief Returns the last number of an OID
     *
     * @param oid OID
     * @return number (nullopt if none)
     * @~
     */
    std::optional<long long> oid_last(const std::string& oid);
  } // namespace ber
} // namespace snmpmgr

#endif
