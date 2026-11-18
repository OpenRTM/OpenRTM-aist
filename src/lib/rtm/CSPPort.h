// -*- C++ -*-
/*!
 * @file CSPPort.h
 * @if jp
 * @brief `CSPPort`の状態と操作をまとめる。 宣言。
 * @else
 * @brief Encapsulate the state and operations of CSPPort. Declarations.
 * @endif
 * @date $Date: 2026-11-13 15:32:20 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 */

// CSP transport shared by the typed input and output ports.
#ifndef RTC_CSPPORT_H
#define RTC_CSPPORT_H
#include <rtm/CSPManager.h>
#include <rtm/Manager.h>
#include <rtm/NVUtil.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/CORBA_CdrMemoryStream.h>
#include <rtm/Typename.h>
#include <rtm/ConnectorListener.h>
#include <rtm/idl/CSPPortSkel.h>
#include <functional>
#include <deque>
#include <map>
#include <cstring>
#include <cmath>

namespace RTC { namespace csp_detail {
/*!
 * @if jp
 * @brief `EventEnvelope`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of EventEnvelope.
 * @endif
 */
struct EventEnvelope { OctetSeq bytes; bool little{true}; };
/*!
 * @if jp
 * @brief `Finally`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of Finally.
 * @endif
 */
class Finally {
  std::function<void()> m_callback;
public:
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @param callback `callback`の値。
   * @else
   * @brief Initialize the instance and its internal state.
   * @param callback Value of callback.
   * @endif
   */
  explicit Finally(std::function<void()> callback) : m_callback(std::move(callback)) {}
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~Finally() { m_callback(); }
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @else
   * @brief Initialize the instance and its internal state.
   * @endif
   */
  Finally(const Finally&) = delete;
  /*!
   * @if jp
   * @brief `operator=`に対応する処理を行う。
   * @else
   * @brief Perform the operator= operation.
   * @endif
   */
  Finally& operator=(const Finally&) = delete;
};
/*!
 * @if jp
 * @brief `repository Id`に対応する処理を行う。
 * @else
 * @brief Perform the repository id operation.
 * @endif
 */
template<class T> const char* repositoryId() { return CORBA_Util::toRepositoryId<T>(); }
/*!
 * @if jp
 * @brief `make Listeners`に対応する処理を行う。
 * @else
 * @brief Perform the make listeners operation.
 * @endif
 */
template<> inline const char* repositoryId<EventEnvelope>() { return "any"; }
template<class T> ConnectorListenersBase* makeListeners()
{
  CdrMemoryStreamInit<T>();
  return new ConnectorListenersT<T>();
}
/*!
 * @if jp
 * @brief `make Listeners`に対応する処理を行う。
 * @else
 * @brief Perform the make listeners operation.
 * @endif
 */
template<> inline ConnectorListenersBase* makeListeners<EventEnvelope>() { return new ConnectorListeners; }
std::string property(const SDOPackage::NVList& props, const char* key,
                            const char* fallback = "");
void setProperty(SDOPackage::NVList& props, const char* key, const char* value);
ReturnCode_t validate(const ConnectorProfile& profile);
bool littleEndian(const ConnectorProfile& profile);
/*!
 * @if jp
 * @brief `reference`に対応する処理を行う。
 * @param props 初期化に使用するプロパティ。
 * @param key `key`の値。
 * @else
 * @brief Perform the reference operation.
 * @param props Properties used for initialization.
 * @param key Value of key.
 * @endif
 */
template<class Interface>
typename Interface::_var_type reference(const SDOPackage::NVList& props, const char* key)
{
  auto ior = property(props, key);
  if (ior.empty()) return Interface::_nil();
  CORBA::Object_var obj = Manager::instance().getORB()->string_to_object(ior.c_str());
  return Interface::_narrow(obj);
}
void publish(SDOPackage::NVList& props, const char* key, CORBA::Object_ptr ref);
/*!
 * @if jp
 * @brief `deactivate`に対応する処理を行う。
 * @param servant `servant`の値。
 * @else
 * @brief Perform the deactivate operation.
 * @param servant Value of servant.
 * @endif
 */
template<class Servant>
void deactivate(Servant* servant)
{
  try
  {
    PortableServer::POA_var poa = servant->_default_POA();
    PortableServer::ObjectId_var id = poa->servant_to_id(servant);
    poa->deactivate_object(id);
  }
  catch (const CORBA::Exception&) {}
}
/*!
 * @if jp
 * @brief 型付きデータを指定エンディアンのCDR列へ変換する。
 * @param value 設定または受け渡す値。
 * @param bytes `bytes`の値。
 * @param little `little`の値。
 * @else
 * @brief Encode typed data into a CDR sequence with the requested endianness.
 * @param value Value to configure or transfer.
 * @param bytes Value of bytes.
 * @param little Value of little.
 * @endif
 */
template<class T> bool encode(const T& value, OctetSeq& bytes, bool little)
{
  CORBA_CdrMemoryStream stream;
  stream.setEndian(little);
  if (!stream.serializeCDR(value)) return false;
  bytes.length(stream.getCdrDataLength());
  if (bytes.length()) std::memcpy(bytes.get_buffer(), stream.getBuffer(), bytes.length());
  return true;
}
/*!
 * @if jp
 * @brief CDR列を指定エンディアンで型付きデータへ復元する。
 * @param bytes `bytes`の値。
 * @param value 設定または受け渡す値。
 * @param little `little`の値。
 * @else
 * @brief Decode a CDR sequence into typed data with the requested endianness.
 * @param bytes Value of bytes.
 * @param value Value to configure or transfer.
 * @param little Value of little.
 * @endif
 */
template<class T> bool decode(const OctetSeq& bytes, T& value, bool little)
{
  CORBA_CdrMemoryStream stream;
  stream.setEndian(little);
  stream.writeCdrData(bytes.get_buffer(), bytes.length());
  return stream.deserializeCDR(value);
}
/*!
 * @if jp
 * @brief CDR列を指定エンディアンで型付きデータへ復元する。
 * @param bytes `bytes`の値。
 * @param value 設定または受け渡す値。
 * @param little `little`の値。
 * @else
 * @brief Decode a CDR sequence into typed data with the requested endianness.
 * @param bytes Value of bytes.
 * @param value Value to configure or transfer.
 * @param little Value of little.
 * @endif
 */
template<> inline bool decode<EventEnvelope>(const OctetSeq& bytes, EventEnvelope& value, bool little)
{ value.bytes = bytes; value.little = little; return true; }
/*!
 * @if jp
 * @brief 型付きデータを指定エンディアンのCDR列へ変換する。
 * @param value 設定または受け渡す値。
 * @param bytes `bytes`の値。
 * @else
 * @brief Encode typed data into a CDR sequence with the requested endianness.
 * @param value Value to configure or transfer.
 * @param bytes Value of bytes.
 * @endif
 */
template<> inline bool encode<EventEnvelope>(const EventEnvelope& value, OctetSeq& bytes, bool)
{ bytes = value.bytes; return true; }
double timeout(const coil::Properties& props);
} }
#endif
