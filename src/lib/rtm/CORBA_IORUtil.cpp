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

#include <rtm/CORBA_IORUtil.h>

#include <iostream>
#include <sstream>

#define POA_NAME_SEP            '\xff'
#define TRANSIENT_SUFFIX_SEP    '\xfe'
#define TRANSIENT_SUFFIX_SIZE   8


namespace CORBA_IORUtil
{

#ifdef ORB_IS_OMNIORB
  typedef _CORBA_Unbounded_Sequence_Octet OctetUSequence;
  typedef _CORBA_Unbounded_Sequence_String StringUSequence;
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
#if !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
  bool toIOR(const char* iorstr, IOP::IOR& ior)
  {
#ifndef ORB_IS_RTORB
    if (iorstr == nullptr) { return false; }
    size_t size = strlen(iorstr);

    if (size < 4)
      {
        return false;
      }

    const char* p = iorstr;

    if (p[0] != 'I' || p[1] != 'O' || p[2] != 'R' || p[3] != ':')
      {
        return false;
      }

    // IOR:xxyyzz......
    // "IOR:" occupies 4 digits.
    // two digits express one byte, and all byte sequence express IOR profile
    size = (size - 4) / 2;  // how many octets are there in the string
    p += 4;

    cdrMemoryStream buf(static_cast<CORBA::ULong>(size), false);
    for (int i(0); i < static_cast<int>(size); ++i)
      {
        CORBA::Octet v;
        // upper digit
        int j(i * 2);
        if (p[j] >= '0' && p[j] <= '9')
          {
            v = ((p[j] - '0') << 4);
          }
        else if (p[j] >= 'a' && p[j] <= 'f')
          {
            v = ((p[j] - 'a' + 10) << 4);
          }
        else if (p[j] >= 'A' && p[j] <= 'F')
          {
            v = ((p[j] - 'A' + 10) << 4);
          }
        else
          {
            return false;
          }
        // lower digit
        int k(j + 1);
        if (p[k] >= '0' && p[k] <= '9')
          {
            v += (p[k] - '0');
          }
        else if (p[k] >= 'a' && p[k] <= 'f')
          {
            v += (p[k] - 'a' + 10);
          }
        else if (p[k] >= 'A' && p[k] <= 'F')
          {
            v += (p[k] - 'A' + 10);
          }
        else
          {
            return false;
          }
        // push_back to buffer
        buf.marshalOctet(v);
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
#if !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
  bool toString(IOP::IOR& ior, std::string& iorstr)
  {
#ifndef ORB_IS_RTORB
    cdrMemoryStream buf(CORBA::ULong(0), CORBA::Boolean(1));
    buf.marshalBoolean(omni::myByteOrder != 0u);
    buf.marshalRawString(ior.type_id);
    ior.profiles >>= buf;

    // turn the encapsulation into a hex string with "IOR:" prepended
    buf.rewindInputPtr();
    size_t s = buf.bufSize();
    CORBA::Char* data = static_cast<CORBA::Char *>(buf.bufPtr());

    char *result = new char[4 + s * 2 + 1];
    result[4 + s * 2] = '\0';
    result[0] = 'I';
    result[1] = 'O';
    result[2] = 'R';
    result[3] = ':';

    for (int i(0); i < static_cast<int>(s); ++i)
      {
        int j = 4 + i * 2;
        int v = (data[i] & 0xf0);

        v = v >> 4;
        if (v < 10)
          {
            result[j] = '0' + static_cast<char>(v);
          }
        else
          {
            result[j] = 'a' + (static_cast<char>(v) - 10);
          }
        v = ((data[i] & 0xf));
        if (v < 10)
          {
            result[j+1] = '0' + static_cast<char>(v);
          }
        else
          {
            result[j+1] = 'a' + (static_cast<char>(v) - 10);
          }
      }
    iorstr = std::string(result);
    delete[] result;
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
#else // ORB_IS_RTORB
    retstr << "RtORB and ORBexpress does't support formatIORinfo() function." << std::endl;
#endif // ORB_IS_RTORB
    return retstr.str();
  }

#if !defined(ORB_IS_RTORB) && !defined(ORB_IS_ORBEXPRESS) && !defined(ORB_IS_TAO)
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
            addr.push_back(pBody.address);
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
#else  // ORB_IS_RTORB
    retstr << "RtORB does't support formatIORinfo() function." << std::endl;
#endif  // ORB_IS_RTORB
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
            addr.push_back(v);
          }
      }
#else // ORB_IS_RTORB
#endif // ORB_IS_RTORB
    return;
  }
#endif // !ORB_IS_RTORB, !ORB_IS_ORBEXPRESS, !ORB_IS_TAO

#ifndef ORB_IS_RTORB
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
    const char* k = reinterpret_cast<const char*>(key.NP_data());
    size_t len = static_cast<size_t>(key.length());
    const char* kend = k + len;

    poas_out.length(1);
    poas_out[0] = CORBA::string_dup("root");

    if (*k != TRANSIENT_SUFFIX_SEP && *k != POA_NAME_SEP) { return 0; }

    while (k < kend && *k == POA_NAME_SEP)
      {
        ++k;
        const char* name = k;

        while (k < kend && (*k != 0) && *k != POA_NAME_SEP
              && *k != TRANSIENT_SUFFIX_SEP)
          {
            ++k;
          }
        if (k == kend)  { return 0; }

        char* nm = new char[k - name + 1];
        memcpy(nm, name, k - name);
        nm[k - name] = '\0';
        poas_out.length(poas_out.length() + 1);
        poas_out[poas_out.length() - 1] = nm;
      }
    if (k == kend)  { return 0; }

    transient_out = 0;
    if (*k == TRANSIENT_SUFFIX_SEP)
      {
        transient_out = 1;
        k += TRANSIENT_SUFFIX_SIZE + 1;
      }
    if (k >= kend || (*k != 0))  { return 0; }
    k++;

    id_out.length(static_cast<CORBA::ULong>(kend - k));
    memcpy(id_out.NP_data(), k, kend - k);

    return 1;
  }
#endif

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
#endif  // ORB_IS_RTORB
} // namespace CORBA_IORUtil

