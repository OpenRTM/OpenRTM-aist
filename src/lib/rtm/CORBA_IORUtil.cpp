// -*- C++ -*-
/*!
 * @file CORBA_IORUtil.h
 * @brief CORBA IOR manipulation utility functions
 * @date $Date: 2007-12-31 03:06:24 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2010-2016
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <limits>
#include <iterator>
#include <rtm/CORBA_IORUtil.h>

#include <iostream>
#include <sstream>

#define POA_NAME_SEP            '\xff'
#define TRANSIENT_SUFFIX_SEP    '\xfe'
#define TRANSIENT_SUFFIX_SIZE   8


namespace CORBA_IORUtil
{

#ifdef ORB_IS_OMNIORB
  using OctetUSequence = _CORBA_Unbounded_Sequence_Octet;
  using StringUSequence = _CORBA_Unbounded_Sequence_String;
#endif

#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
  // prototype of static functions
  static void print_key(std::stringstream& sstr, OctetUSequence& key);

  static void print_omni_key(std::stringstream& sstr, OctetUSequence& key);

  static int get_poa_info(OctetUSequence& key, StringUSequence& poas_out,
                          int& transient_out, OctetUSequence& id_out);
  static void print_tagged_components(std::stringstream& sstr,
                                      IOP::MultipleComponentProfile& components);
#endif  // ORB_IS_RTORB


  /*!
   * @if jp
   * @brief IOR 文字列をIOR構造体へ変換する
   * @else
   * @brief Convert from IOR string to IOR structure
   * @endif
   */
#if !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO) && !defined(ORB_IS_RTORB)
  bool toIOR(const char* iorstr, IOP::IOR& ior)
  {
#ifndef ORB_IS_RTORB
    if (iorstr == nullptr) { return false; }
    const std::string text(iorstr);
    if (text.compare(0, 4, "IOR:") != 0 || text.size() <= 4 ||
        (text.size() - 4) % 2 != 0)
      {
        return false;
      }
    const auto size = (text.size() - 4) / 2;
    if (size > std::numeric_limits<CORBA::ULong>::max()) { return false; }
    const std::string digits("0123456789abcdefABCDEF");
    cdrMemoryStream buf(static_cast<CORBA::ULong>(size), false);
    for (std::size_t i = 4; i < text.size(); i += 2)
      {
        auto upper = digits.find(text[i]);
        auto lower = digits.find(text[i + 1]);
        if (upper == std::string::npos || lower == std::string::npos) { return false; }
        if (upper >= 16) { upper -= 6; }
        if (lower >= 16) { lower -= 6; }
        buf.marshalOctet(static_cast<CORBA::Octet>(upper * 16 + lower));
      }

    buf.rewindInputPtr();
    CORBA::Boolean b = buf.unmarshalBoolean();
    buf.setByteSwapFlag(b);

    ior.type_id = IOP::IOR::unmarshaltype_id(buf);
    ior.profiles <<= buf;
    return true;
#else  // ORB_IS_RTORB
    // RtORB does not supports this function
    return false;
#endif  // ORB_IS_RTORB
  }
#endif //ORB_IS_ORBEXPRESS

  /*!
   * @if jp
   * @brief IOR構造体をIOR文字列へ変換する
   * @else
   * @brief Convert from IOR structure to IOR string
   * @endif
   */
#if !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO) && !defined(ORB_IS_RTORB)
  bool toString(IOP::IOR& ior, std::string& iorstr)
  {
#ifndef ORB_IS_RTORB
    cdrMemoryStream buf(CORBA::ULong(0), CORBA::Boolean(1));
    buf.marshalBoolean(omni::myByteOrder != 0U);
    buf.marshalRawString(ior.type_id);
    ior.profiles >>= buf;

    // turn the encapsulation into a hex string with "IOR:" prepended
    buf.rewindInputPtr();
    const auto size = buf.bufSize();
    const auto* data = static_cast<const unsigned char*>(buf.bufPtr());
    const std::string digits("0123456789abcdef");
    std::string result("IOR:");
    if (size > (result.max_size() - result.size()) / 2) { return false; }
    result.reserve(result.size() + size * 2);
    for (std::size_t i = 0; i < size; ++i)
      {
        const auto byte = *std::next(data, i);
        result.push_back(digits[byte >> 4]);
        result.push_back(digits[byte & 0xf]);
      }
    iorstr = std::move(result);
    return true;
#else  // ORB_IS_RTORB
    // RtORB does not this function.
    return false;
#endif  // ORB_IS_RTORB
  }
#endif //ORB_IS_ORBEXPRESS

  /*!
   * @if jp
   * @brief IOR内のエンドポイントを置換する
   * @else
   * @brief Replace endpoint address in IOR entry
   * @endif
   */
  bool replaceEndpoint(std::string& iorstr, const std::string& endpoint)
  {
#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
    try
      {
        IOP::IOR ior;
        toIOR(iorstr.c_str(), ior);

        for (unsigned long count(0); count < ior.profiles.length(); ++count)
          {

            if (ior.profiles[count].tag == IOP::TAG_INTERNET_IOP)
              {
                IIOP::ProfileBody pBody;
                IIOP::unmarshalProfile(ior.profiles[count], pBody);
                pBody.address.host = endpoint.c_str();

                IOP::TaggedProfile profile;
                // profile_data is cctet sequence
                IIOP::encodeProfile(pBody, profile);
                CORBA::ULong max = profile.profile_data.maximum();
                CORBA::ULong len = profile.profile_data.length();
                CORBA::Octet* buf = profile.profile_data.get_buffer(true);
                // replace is not standard function
                ior.profiles[count].profile_data.replace(max, len, buf, true);
              }
          }
        return toString(ior, iorstr);

      }
    catch(...)
      {
        return false;
      }
#else
    (void)iorstr;
    (void)endpoint;
    return false;
#endif  // ORB_IS_RTORB
  }

  /*!
   * @if jp
   * @brief IOR文字列から情報を抽出しフォーマット済みの文字列として返す
   * @else
   * @brief Extracts information from IOR string and returns formatted string
   * @endif
   */
  std::string formatIORinfo(const char* iorstr)
  {
    std::stringstream retstr("");
#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
    IOP::IOR ior;
    toIOR(iorstr, ior);

    if (ior.profiles.length() == 0 && strlen(ior.type_id) == 0)
      {
        retstr << "IOR is a nil object reference." << std::endl;
        if (iorstr != nullptr) { retstr << iorstr << std::endl; }
        return retstr.str();
      }

    retstr << "IOR information" << std::endl;
    retstr << "  Type ID: \"" << (const char*) ior.type_id
           << "\"" << std::endl;
    retstr << "  Profiles:" << std::endl;
    for (CORBA::ULong count(0); count < ior.profiles.length(); ++count)
      {
        retstr << "    " << count + 1 << ". ";
        if (ior.profiles[count].tag == IOP::TAG_INTERNET_IOP)
          {
            IIOP::ProfileBody pBody;
            IIOP::unmarshalProfile(ior.profiles[count], pBody);

            retstr << "IIOP " << static_cast<int>(pBody.version.major) << "."
                   << static_cast<int>(pBody.version.minor) << " ";
            retstr << (const char*) pBody.address.host
                   << " " << pBody.address.port << std::endl;

            print_omni_key(retstr, pBody.object_key);
            print_key(retstr, pBody.object_key);
            print_tagged_components(retstr, pBody.components);

            retstr << std::endl;
          }
        else if (ior.profiles[count].tag == IOP::TAG_MULTIPLE_COMPONENTS)
          {
            retstr << "Multiple Component Profile ";
            IIOP::ProfileBody pBody;
            IIOP::unmarshalMultiComponentProfile(ior.profiles[count],
                    pBody.components);
            print_tagged_components(retstr, pBody.components);

            retstr << std::endl;
          }
        else
          {
            retstr << "Unrecognised profile tag: 0x"
                   << std::hex
                   << static_cast<unsigned>(ior.profiles[count].tag)
                   << std::dec
                   << std::endl;
          }
      }
#else
    (void)iorstr;
    retstr << "RtORB and ORBexpress does't support formatIORinfo() function." << std::endl;
#endif
    return retstr.str();
  }

#if !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO) && !defined(ORB_IS_RTORB)
  std::vector<IIOP::Address> getEndpoints(IOP::IOR& ior)
  {
    std::vector<IIOP::Address> addr;
#ifndef ORB_IS_RTORB
    if (ior.profiles.length() == 0 && strlen(ior.type_id) == 0)
      {
        std::cerr << "IOR is a nil object reference." << std::endl;
        return addr;
      }

    for (CORBA::ULong i(0); i < ior.profiles.length(); ++i)
      {
        if (ior.profiles[i].tag == IOP::TAG_INTERNET_IOP)
          {
            IIOP::ProfileBody pBody;
            IIOP::unmarshalProfile(ior.profiles[i], pBody);
            addr.emplace_back(pBody.address);
            extractAddrs(pBody.components, addr);
          }
        else if (ior.profiles[i].tag == IOP::TAG_MULTIPLE_COMPONENTS)
          {
            IIOP::ProfileBody pBody;
            IIOP::unmarshalMultiComponentProfile(ior.profiles[i],
                                                 pBody.components);
            extractAddrs(pBody.components, addr);
          }
        else
          {
            std::cerr << "Unrecognised profile tag: 0x"
                     << std::hex << static_cast<unsigned>(ior.profiles[i].tag)
                     << std::dec << std::endl;
          }
      }
#else  // !ORB_IS_RTORB
    (void)ior;
    retstr << "RtORB does't support formatIORinfo() function." << std::endl;
#endif  // !ORB_IS_RTORB
    return addr;
  }

  void extractAddrs(IOP::MultipleComponentProfile& comp,
                    std::vector<IIOP::Address>& addr)
  {
#ifndef ORB_IS_RTORB
    for (CORBA::ULong i(0); i < comp.length(); ++i)
      {
        if (comp[i].tag == IOP::TAG_ALTERNATE_IIOP_ADDRESS)
          {
            cdrEncapsulationStream e(comp[i].component_data.get_buffer(),
                                     comp[i].component_data.length(), true);
            IIOP::Address v;
            v.host = e.unmarshalRawString();
            v.port <<= e;
            addr.emplace_back(v);
          }
      }
#else // !ORB_IS_RTORB
#endif // !ORB_IS_RTORB
    return;
  }
#endif // !ORB_IS_RTORB, !ORB_IS_ORBEXPRESS, !ORB_IS_TAO

  //------------------------------------------------------------
  // static functions
#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
  static void print_key(std::stringstream& sstr, OctetUSequence& key)
  {
    // Output key as text
    sstr << "       Object Key: \"";
    for (CORBA::ULong j(0); j < key.length(); ++j)
      {
        if (static_cast<char>(key[j]) >= ' '
         && static_cast<char>(key[j]) <= '~')
          {
            sstr << static_cast<char>(key[j]);
          }
        else
          {
            sstr << ".";
          }
      }
    sstr << "\"";

    // Output key in hexadecimal form.
    sstr << " = 0x";
    for (unsigned int j(0); j < key.length(); ++j)
      {
        int v = (key[j] & 0xf0) >> 4;
        if (v < 10)
          {
            sstr << static_cast<char>('0' + v);
          }
        else
          {
            sstr << static_cast<char>('a' + (v - 10));
          }
        v = key[j] & 0xf;
        if (v < 10)
          {
            sstr << static_cast<char>('0' + v);
          }
        else
          {
            sstr << static_cast<char>('a' + (v - 10));
          }
      }
    sstr << "  (" << key.length() << " bytes)" << std::endl;
  }

  static void print_omni_key(std::stringstream& sstr, OctetUSequence& key)
  {
    StringUSequence poas;
    int is_transient;
    OctetUSequence id;

    if (get_poa_info(key, poas, is_transient, id) != 0)
      {
        sstr << "       POA(" << (char*)poas[0];
        for (unsigned i(1); i < poas.length(); ++i)
          {
            sstr << '/' << (char*)poas[i];
          }
        sstr << ") ";
      }
    else
      {
        if (key.length() != static_cast<CORBA::ULong>(sizeof(omniOrbBoaKey)))
          {
            return;
          }
        sstr << "BOA ";
      }
    print_key(sstr, id);
  }

  static int get_poa_info(OctetUSequence& key, StringUSequence& poas_out,
                          int& transient_out, OctetUSequence& id_out)
  {
    if (key.length() == 0) { return 0; }
    const std::string bytes(reinterpret_cast<const char*>(key.NP_data()), key.length());
    std::size_t cursor = 0;

    poas_out.length(1);
    poas_out[0] = CORBA::string_dup("root");
    if (bytes.front() != TRANSIENT_SUFFIX_SEP && bytes.front() != POA_NAME_SEP) { return 0; }

    while (cursor < bytes.size() && bytes[cursor] == POA_NAME_SEP)
      {
        const auto start = ++cursor;
        while (cursor < bytes.size() && bytes[cursor] != 0 &&
              bytes[cursor] != POA_NAME_SEP && bytes[cursor] != TRANSIENT_SUFFIX_SEP)
          {
            ++cursor;
          }
        if (cursor == bytes.size()) { return 0; }

        const auto name = bytes.substr(start, cursor - start);
        poas_out.length(poas_out.length() + 1);
        poas_out[poas_out.length() - 1] = CORBA::string_dup(name.c_str());
      }
    if (cursor == bytes.size()) { return 0; }

    transient_out = 0;
    if (bytes[cursor] == TRANSIENT_SUFFIX_SEP)
      {
        transient_out = 1;
        if (bytes.size() - cursor <= TRANSIENT_SUFFIX_SIZE + 1) { return 0; }
        cursor += TRANSIENT_SUFFIX_SIZE + 1;
      }
    if (bytes[cursor] != 0) { return 0; }
    ++cursor;

    id_out.length(static_cast<CORBA::ULong>(bytes.size() - cursor));
    if (id_out.length() != 0)
      {
        bytes.copy(reinterpret_cast<char*>(id_out.NP_data()), id_out.length(), cursor);
      }

    return 1;
  }
#endif // !ORB_IS_RTORB, !ORB_IS_ORBEXPRESS, !ORB_IS_TAO

#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
  static void print_tagged_components(std::stringstream& sstr,
                                      IOP::MultipleComponentProfile& components)
  {
#if defined(RTM_OMNIORB_40) || defined(RTM_OMNIORB_41)
    CORBA::ULong total(components.length());

    for (CORBA::ULong index(0); index < total; ++index)
      {
        try
          {
            CORBA::String_var content;
            content = IOP::dumpComponent(components[index]);
            char* p = content;
            char* q;
            do
              {
                q = strchr(p, '\n');
                if (q)
                  {
                    *q++ = '\0';
                  }
                sstr << "       " << (const char*) p << std::endl;
                p = q;
              } while (q);
          }
        catch (CORBA::MARSHAL&)
          {
            sstr << "       Broken component" << std::endl;
          }
      }
#endif  // defined(RTM_OMNIORB_40) || defined(RTM_OMNIORB_41)
    (void)sstr;
    (void)components;
  }
#endif // !ORB_IS_RTORB, !ORB_IS_ORBEXPRESS, !ORB_IS_TAO
} // namespace CORBA_IORUtil

