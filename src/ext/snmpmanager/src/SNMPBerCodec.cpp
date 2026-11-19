// -*- C++ -*-
/*!
 * @file SNMPBerCodec.cpp
 * @brief Encoding and decoding of SNMPv2c messages (BER). See SNMPBerCodec.hpp
 *
 * @~japanese
 * SNMPv2c のメッセージの符号化・復号 (BER)。説明は SNMPBerCodec.hpp
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

#include "SNMPBerCodec.hpp"

#include <stdexcept>

namespace snmpmgr
{
  namespace ber
  {
    namespace
    {
      // ------------------------------------------------------------ 符号化
      /*!
       * @~japanese
       * @brief BER の長さを符号化する
       *
       * @param n 長さ
       * @return 符号化した長さ
       * @~english
       * @brief Encodes a BER length
       *
       * @param n length
       * @return encoded length
       * @~
       */
      std::string length(size_t n)
      {
        std::string out;
        if (n < 0x80) { out += static_cast<char>(n); return out; }
        std::string bytes;
        while (n > 0) { bytes.insert(bytes.begin(), static_cast<char>(n & 0xff)); n >>= 8; }
        out += static_cast<char>(0x80 | bytes.size());
        return out + bytes;
      }

      /*!
       * @~japanese
       * @brief タグ・長さ・値を符号化する
       *
       * @param tag タグ
       * @param body 値
       * @return 符号化した TLV
       * @~english
       * @brief Encodes tag, length and value
       *
       * @param tag tag
       * @param body value
       * @return encoded TLV
       * @~
       */
      std::string tlv(uint8_t tag, const std::string& body)
      {
        return std::string(1, static_cast<char>(tag)) + length(body.size()) + body;
      }

      /*!
       * @~japanese
       * @brief 符号付き整数の値を符号化する
       *
       * @param v 値
       * @return 符号化した値
       * @~english
       * @brief Encodes the value of a signed integer
       *
       * @param v value
       * @return encoded value
       * @~
       */
      std::string signed_int(long long v)
      {
        // 2 の補数の最短の表現
        std::string out;
        for (int i = 7; i >= 0; --i) { out += static_cast<char>((static_cast<unsigned long long>(v) >> (8 * i)) & 0xff); }
        while (out.size() > 1 &&
               ((static_cast<uint8_t>(out[0]) == 0x00 && !(static_cast<uint8_t>(out[1]) & 0x80)) ||
                (static_cast<uint8_t>(out[0]) == 0xff && (static_cast<uint8_t>(out[1]) & 0x80))))
          {
            out.erase(out.begin());
          }
        return out;
      }

      /*!
       * @~japanese
       * @brief 符号なし整数の値を符号化する
       *
       * @param v 値
       * @return 符号化した値
       * @~english
       * @brief Encodes the value of an unsigned integer
       *
       * @param v value
       * @return encoded value
       * @~
       */
      std::string unsigned_int(unsigned long long v)
      {
        std::string out;
        do { out.insert(out.begin(), static_cast<char>(v & 0xff)); v >>= 8; } while (v > 0);
        if (static_cast<uint8_t>(out[0]) & 0x80) { out.insert(out.begin(), '\0'); }   // 正の数のまま読めるように
        return out;
      }

      /*!
       * @~japanese
       * @brief OID の文字列を番号の並びにする
       *
       * @param text OID
       * @return 番号の並び
       * @~english
       * @brief Parses an OID string into a sequence of numbers
       *
       * @param text OID
       * @return sequence of numbers
       * @~
       */
      std::vector<unsigned long long> parse_oid(const std::string& text)
      {
        std::vector<unsigned long long> arcs;
        std::string t = (!text.empty() && text[0] == '.') ? text.substr(1) : text;
        size_t pos = 0;
        while (pos <= t.size())
          {
            size_t dot = t.find('.', pos);
            std::string part = t.substr(pos, dot == std::string::npos ? std::string::npos : dot - pos);
            if (part.empty()) { throw std::invalid_argument("bad OID: " + text); }
            arcs.push_back(std::stoull(part));
            if (dot == std::string::npos) { break; }
            pos = dot + 1;
          }
        if (arcs.size() < 2) { throw std::invalid_argument("bad OID: " + text); }
        return arcs;
      }

      /*!
       * @~japanese
       * @brief OID の番号を base128 で符号化する
       *
       * @param v 番号
       * @return 符号化した値
       * @~english
       * @brief Encodes an OID number in base128
       *
       * @param v number
       * @return encoded value
       * @~
       */
      std::string base128(unsigned long long v)
      {
        std::string out(1, static_cast<char>(v & 0x7f));
        v >>= 7;
        while (v > 0) { out.insert(out.begin(), static_cast<char>(0x80 | (v & 0x7f))); v >>= 7; }
        return out;
      }

      /*!
       * @~japanese
       * @brief OID の値を符号化する
       *
       * @param text OID
       * @return 符号化した値
       * @~english
       * @brief Encodes the value of an OID
       *
       * @param text OID
       * @return encoded value
       * @~
       */
      std::string oid_body(const std::string& text)
      {
        auto arcs = parse_oid(text);
        std::string out = base128(arcs[0] * 40 + arcs[1]);
        for (size_t i = 2; i < arcs.size(); ++i) { out += base128(arcs[i]); }
        return out;
      }

      /*!
       * @~japanese
       * @brief 変数のバインディングの値を符号化する
       *
       * @param v 変数のバインディング
       * @return 符号化した値 (TLV)
       * @~english
       * @brief Encodes the value of a variable binding
       *
       * @param v variable binding
       * @return encoded value (TLV)
       * @~
       */
      std::string value_body(const VarBind& v)
      {
        switch (v.type)
          {
          case INTEGER: return signed_int(std::stoll(v.value));
          case COUNTER32: case GAUGE32: case TIMETICKS: case COUNTER64: return unsigned_int(std::stoull(v.value));
          case OBJECT_ID: return oid_body(v.value);
          case IP_ADDRESS:
            {
              std::string out;
              size_t pos = 0;
              for (int i = 0; i < 4; ++i)
                {
                  size_t dot = v.value.find('.', pos);
                  out += static_cast<char>(std::stoi(v.value.substr(pos, dot - pos)));
                  pos = dot + 1;
                }
              return out;
            }
          case NULL_: case NO_SUCH_OBJECT: case NO_SUCH_INSTANCE: case END_OF_MIB_VIEW: return "";
          default: return v.value;      // OCTET STRING, Opaque
          }
      }

      // ------------------------------------------------------------ 復号
      /*!
       * @~japanese
       * @brief BER を読む位置 (データと範囲)
       * @~english
       * @brief Reader position of BER (data and range)
       * @~
       */
      struct Reader
      {
        const std::string& d;
        size_t pos;
        size_t end;

        /*!
         * @~japanese
         * @brief 1 バイト読む
         *
         * @return バイト
         * @~english
         * @brief Reads one byte
         *
         * @return byte
         * @~
         */
        uint8_t byte()
        {
          if (pos >= end) { throw std::out_of_range("truncated"); }
          return static_cast<uint8_t>(d[pos++]);
        }

        /*!
         * @~japanese
         * @brief 次の TLV のタグと値の範囲を読む
         *
         * @param tag タグ
         * @param start 値の始まり
         * @param len 値の長さ
         * @~english
         * @brief Reads the tag and value range of the next TLV
         *
         * @param tag tag
         * @param start start of the value
         * @param len length of the value
         * @~
         */
        void next(uint8_t& tag, size_t& start, size_t& len)
        {
          tag = byte();
          uint8_t l = byte();
          if (l & 0x80)
            {
              int n = l & 0x7f;
              if (n == 0 || n > 4) { throw std::out_of_range("bad length"); }
              len = 0;
              for (int i = 0; i < n; ++i) { len = (len << 8) | byte(); }
            }
          else { len = l; }
          if (pos + len > end) { throw std::out_of_range("truncated"); }
          start = pos;
          pos += len;
        }

        /*!
         * @~japanese
         * @brief 範囲の Reader を作る
         *
         * @param start 始まり
         * @param len 長さ
         * @return Reader
         * @~english
         * @brief Makes a Reader for a range
         *
         * @param start start
         * @param len length
         * @return Reader
         * @~
         */
        Reader sub(size_t start, size_t len) const { return Reader{d, start, start + len}; }
      };

      /*!
       * @~japanese
       * @brief 符号付き整数を読む
       *
       * @param d データ
       * @param start 始まり
       * @param len 長さ
       * @return 値
       * @~english
       * @brief Reads a signed integer
       *
       * @param d data
       * @param start start
       * @param len length
       * @return value
       * @~
       */
      long long read_signed(const std::string& d, size_t start, size_t len)
      {
        if (len == 0 || len > 8) { throw std::out_of_range("bad integer"); }
        long long v = (static_cast<uint8_t>(d[start]) & 0x80) ? -1 : 0;
        for (size_t i = 0; i < len; ++i) { v = static_cast<long long>((static_cast<unsigned long long>(v) << 8) | static_cast<uint8_t>(d[start + i])); }
        return v;
      }

      /*!
       * @~japanese
       * @brief 符号なし整数を読む
       *
       * @param d データ
       * @param start 始まり
       * @param len 長さ
       * @return 値
       * @~english
       * @brief Reads an unsigned integer
       *
       * @param d data
       * @param start start
       * @param len length
       * @return value
       * @~
       */
      unsigned long long read_unsigned(const std::string& d, size_t start, size_t len)
      {
        if (len > 9) { throw std::out_of_range("bad integer"); }
        unsigned long long v = 0;
        for (size_t i = 0; i < len; ++i) { v = (v << 8) | static_cast<uint8_t>(d[start + i]); }
        return v;
      }

      /*!
       * @~japanese
       * @brief OID を読む
       *
       * @param d データ
       * @param start 始まり
       * @param len 長さ
       * @return OID の文字列
       * @~english
       * @brief Reads an OID
       *
       * @param d data
       * @param start start
       * @param len length
       * @return OID string
       * @~
       */
      std::string read_oid(const std::string& d, size_t start, size_t len)
      {
        std::string out;
        unsigned long long v = 0;
        bool first = true;
        for (size_t i = 0; i < len; ++i)
          {
            uint8_t b = static_cast<uint8_t>(d[start + i]);
            v = (v << 7) | (b & 0x7f);
            if (b & 0x80) { continue; }
            if (first)
              {
                unsigned long long a = v < 40 ? 0 : (v < 80 ? 1 : 2);
                out = std::to_string(a) + "." + std::to_string(v - a * 40);
                first = false;
              }
            else { out += "." + std::to_string(v); }
            v = 0;
          }
        return out;
      }

      /*!
       * @~japanese
       * @brief 変数のバインディングの並びを読む
       *
       * @param r Reader
       * @param out 読んだ変数のバインディング
       * @~english
       * @brief Reads a sequence of variable bindings
       *
       * @param r Reader
       * @param out read variable bindings
       * @~
       */
      void read_varbinds(Reader r, std::vector<VarBind>& out)
      {
        while (r.pos < r.end)
          {
            uint8_t tag;
            size_t s, l;
            r.next(tag, s, l);
            if (tag != SEQUENCE) { throw std::out_of_range("bad varbind"); }
            Reader vb = r.sub(s, l);
            uint8_t t1, t2;
            size_t s1, l1, s2, l2;
            vb.next(t1, s1, l1);
            vb.next(t2, s2, l2);
            if (t1 != OBJECT_ID) { throw std::out_of_range("bad varbind name"); }
            VarBind v;
            v.oid = read_oid(r.d, s1, l1);
            v.type = t2;
            switch (t2)
              {
              case INTEGER: v.value = std::to_string(read_signed(r.d, s2, l2)); break;
              case COUNTER32: case GAUGE32: case TIMETICKS:
                v.value = std::to_string(read_unsigned(r.d, s2, l2) & 0xffffffffULL);
                break;
              case COUNTER64: v.value = std::to_string(read_unsigned(r.d, s2, l2)); break;
              case OBJECT_ID: v.value = read_oid(r.d, s2, l2); break;
              case IP_ADDRESS:
                if (l2 == 4)
                  {
                    for (size_t i = 0; i < 4; ++i)
                      {
                        v.value += (i ? "." : "") + std::to_string(static_cast<uint8_t>(r.d[s2 + i]));
                      }
                  }
                break;
              default: v.value = r.d.substr(s2, l2); break;
              }
            out.push_back(std::move(v));
          }
      }
    } // namespace

    std::string encode_message(const std::string& community, const Pdu& pdu)
    {
      std::string vbs;
      for (auto& v : pdu.vars) { vbs += tlv(SEQUENCE, tlv(OBJECT_ID, oid_body(v.oid)) + tlv(v.type, value_body(v))); }
      std::string body = tlv(INTEGER, signed_int(pdu.request_id)) + tlv(INTEGER, signed_int(pdu.error_status)) +
                         tlv(INTEGER, signed_int(pdu.error_index)) + tlv(SEQUENCE, vbs);
      return tlv(SEQUENCE, tlv(INTEGER, signed_int(1)) + tlv(OCTET_STRING, community) + tlv(pdu.type, body));
    }

    std::optional<Pdu> decode_message(const std::string& data, std::string* community)
    {
      try
        {
          Reader top{data, 0, data.size()};
          uint8_t tag;
          size_t s, l;
          top.next(tag, s, l);
          if (tag != SEQUENCE) { return std::nullopt; }
          Reader msg = top.sub(s, l);
          msg.next(tag, s, l);
          if (tag != INTEGER) { return std::nullopt; }
          long long version = read_signed(data, s, l);
          if (version != 0 && version != 1) { return std::nullopt; }     // v1 / v2c だけ
          msg.next(tag, s, l);
          if (tag != OCTET_STRING) { return std::nullopt; }
          if (community) { *community = data.substr(s, l); }
          msg.next(tag, s, l);
          Pdu pdu;
          pdu.type = tag;
          Reader p = msg.sub(s, l);
          if (tag == TRAP_V1)
            {
              // enterprise, agent-addr, generic-trap, specific-trap, time-stamp, varbinds
              for (int i = 0; i < 5; ++i) { p.next(tag, s, l); }
              p.next(tag, s, l);
              read_varbinds(p.sub(s, l), pdu.vars);
              return pdu;
            }
          p.next(tag, s, l);
          pdu.request_id = static_cast<int32_t>(read_signed(data, s, l));
          p.next(tag, s, l);
          pdu.error_status = static_cast<int32_t>(read_signed(data, s, l));
          p.next(tag, s, l);
          pdu.error_index = static_cast<int32_t>(read_signed(data, s, l));
          p.next(tag, s, l);
          if (tag != SEQUENCE) { return std::nullopt; }
          read_varbinds(p.sub(s, l), pdu.vars);
          return pdu;
        }
      catch (std::exception&) { return std::nullopt; }
    }

    std::optional<std::string> value_string(const VarBind& v)
    {
      switch (v.type)
        {
        case NULL_: case NO_SUCH_OBJECT: case NO_SUCH_INSTANCE: case END_OF_MIB_VIEW: return std::nullopt;
        case INTEGER: case COUNTER32: case GAUGE32: case TIMETICKS: case COUNTER64: case OBJECT_ID:
        case OCTET_STRING: case IP_ADDRESS:
          return v.value;
        default: return std::nullopt;
        }
    }

    bool oid_under(const std::string& oid, const std::string& root)
    {
      std::string r = (!root.empty() && root[0] == '.') ? root.substr(1) : root;
      return oid.size() > r.size() && oid.compare(0, r.size(), r) == 0 && oid[r.size()] == '.';
    }

    std::optional<long long> oid_last(const std::string& oid)
    {
      auto p = oid.rfind('.');
      try { return std::stoll(p == std::string::npos ? oid : oid.substr(p + 1)); }
      catch (...) { return std::nullopt; }
    }
  } // namespace ber
} // namespace snmpmgr
