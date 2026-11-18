// -*- C++ -*-
/*!
 * @file CSPTransport.cpp
 * @if jp
 * @brief CORBAを介したCSPポートの準備確認とデータ転送を実装する。 実装。
 * @else
 * @brief Implement CSP readiness queries and data transfer over CORBA. Implementation.
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

#include <rtm/CSPTransport.h>
#include <limits>
#ifdef ORB_IS_OMNIORB
#include <omniORB4/omniORB.h>
#endif

namespace RTC { namespace csp_detail {
/*!
 * @if jp
 * @brief `property`に対応する処理を行う。
 * @param props 初期化に使用するプロパティ。
 * @param key `key`の値。
 * @param fallback `fallback`の値。
 * @else
 * @brief Perform the property operation.
 * @param props Properties used for initialization.
 * @param key Value of key.
 * @param fallback Value of fallback.
 * @endif
 */
std::string property(const SDOPackage::NVList& props, const char* key,
                            const char* fallback)
{
  const auto index = NVUtil::find_index(props, key);
  const char* value = nullptr;
  return index >= 0 && (props[index].value >>= value) ? value : fallback;
}
/*!
 * @if jp
 * @brief `Property`を設定する。
 * @param props 初期化に使用するプロパティ。
 * @param key `key`の値。
 * @param value 設定または受け渡す値。
 * @else
 * @brief Set property.
 * @param props Properties used for initialization.
 * @param key Value of key.
 * @param value Value to configure or transfer.
 * @endif
 */
void setProperty(SDOPackage::NVList& props, const char* key, const char* value)
{
  const auto index = NVUtil::find_index(props, key);
  if (index >= 0) props[index].value <<= value;
  else CORBA_SeqUtil::push_back(props, NVUtil::newNV(key, value));
}
/*!
 * @if jp
 * @brief `validate`に対応する処理を行う。
 * @param profile 接続プロファイル。
 * @else
 * @brief Perform the validate operation.
 * @param profile Connector profile.
 * @endif
 */
ReturnCode_t validate(const ConnectorProfile& profile)
{
  if (profile.ports.length() != 2) return BAD_PARAMETER;
  if (property(profile.properties, "dataport.interface_type", "csp_channel") != "csp_channel") return BAD_PARAMETER;
  if (property(profile.properties, "dataport.dataflow_type", "duplex") != "duplex") return BAD_PARAMETER;
  if (property(profile.properties, "dataport.marshaling_type", "cdr") != "cdr") return BAD_PARAMETER;
  const auto endian = coil::split(property(profile.properties, "serializer.cdr.endian", property(profile.properties, "dataport.serializer.cdr.endian", "little").c_str()), ",").front();
  if (endian != "little" && endian != "big") return BAD_PARAMETER;
  return RTC_OK;
}
/*!
 * @if jp
 * @brief `little Endian`に対応する処理を行う。
 * @param profile 接続プロファイル。
 * @else
 * @brief Perform the little endian operation.
 * @param profile Connector profile.
 * @endif
 */
bool littleEndian(const ConnectorProfile& profile)
{ return coil::split(property(profile.properties, "serializer.cdr.endian", property(profile.properties, "dataport.serializer.cdr.endian", "little").c_str()), ",").front() == "little"; }
/*!
 * @if jp
 * @brief `publish`に対応する処理を行う。
 * @param props 初期化に使用するプロパティ。
 * @param key `key`の値。
 * @param ref `ref`の値。
 * @else
 * @brief Perform the publish operation.
 * @param props Properties used for initialization.
 * @param key Value of key.
 * @param ref Value of ref.
 * @endif
 */
void publish(SDOPackage::NVList& props, const char* key, CORBA::Object_ptr ref)
{
  CORBA::String_var ior = Manager::instance().getORB()->object_to_string(ref);
  setProperty(props, key, ior);
}

/*!
 * @if jp
 * @brief `to Bytes`に対応する処理を行う。
 * @param data 送受信するデータ。
 * @param little `little`の値。
 * @else
 * @brief Perform the to bytes operation.
 * @param data Data to transmit or receive.
 * @param little Value of little.
 * @endif
 */
ByteData toBytes(const OctetSeq& data, bool little)
{
  ByteData bytes;
  bytes.isLittleEndian(little);
  bytes.writeData(data.get_buffer(), data.length());
  return bytes;
}
/*!
 * @if jp
 * @brief `to Octets`に対応する処理を行う。
 * @param bytes `bytes`の値。
 * @else
 * @brief Perform the to octets operation.
 * @param bytes Value of bytes.
 * @endif
 */
OctetSeq toOctets(const ByteData& bytes)
{
  OctetSeq result;
  result.length(bytes.getDataLength());
  if (result.length()) bytes.readData(result.get_buffer(), result.length());
  return result;
}
/*!
 * @if jp
 * @brief `wire Status`に対応する処理を行う。
 * @param status `status`の値。
 * @else
 * @brief Perform the wire status operation.
 * @param status Value of status.
 * @endif
 */
PortStatus wireStatus(BufferStatus status)
{
  switch (status) {
  case BufferStatus::OK: return PORT_OK;
  case BufferStatus::FULL: return BUFFER_FULL;
  case BufferStatus::EMPTY: return BUFFER_EMPTY;
  case BufferStatus::TIMEOUT: return BUFFER_TIMEOUT;
  case BufferStatus::BUFFER_ERROR:
  case BufferStatus::NOT_SUPPORTED:
  case BufferStatus::PRECONDITION_NOT_MET: return PORT_ERROR;
  default: return PORT_ERROR;
  }
}
/*!
 * @if jp
 * @brief `port Status`に対応する処理を行う。
 * @param status `status`の値。
 * @else
 * @brief Perform the port status operation.
 * @param status Value of status.
 * @endif
 */
DataPortStatus portStatus(PortStatus status)
{
  switch (status) {
  case PORT_OK: return DataPortStatus::PORT_OK;
  case BUFFER_FULL: return DataPortStatus::BUFFER_FULL;
  case BUFFER_EMPTY: return DataPortStatus::BUFFER_EMPTY;
  case BUFFER_TIMEOUT: return DataPortStatus::BUFFER_TIMEOUT;
  case PORT_ERROR: return DataPortStatus::PORT_ERROR;
  case UNKNOWN_ERROR: return DataPortStatus::UNKNOWN_ERROR;
  default: return DataPortStatus::UNKNOWN_ERROR;
  }
}
/*!
 * @if jp
 * @brief `timeout`に対応する処理を行う。
 * @param props 初期化に使用するプロパティ。
 * @else
 * @brief Perform the timeout operation.
 * @param props Properties used for initialization.
 * @endif
 */
double timeout(const coil::Properties& props)
{
  double seconds = 10.0;
  if (!coil::stringTo(seconds, props.getProperty("channel_timeout", "10").c_str()) || !std::isfinite(seconds) || seconds < 0)
    throw std::invalid_argument("Invalid CSP channel_timeout");
  return seconds;
}
/*!
 * @if jp
 * @brief `connector Info`に対応する処理を行う。
 * @param profile 接続プロファイル。
 * @else
 * @brief Perform the connector info operation.
 * @param profile Connector profile.
 * @endif
 */
ConnectorInfo connectorInfo(const ConnectorProfile& profile)
{
  coil::Properties props;
  NVUtil::copyToProperties(props, profile.properties);
  auto node = props.getNode("dataport");
  // Accept the historical top-level endian/event keys as well.
  node << props.getNode("serializer");
  node.setProperty("serializer.cdr.endian", property(profile.properties, "serializer.cdr.endian",
                   property(profile.properties, "dataport.serializer.cdr.endian", "little").c_str()));
  return ConnectorInfo(profile.name, profile.connector_id, CORBA_SeqUtil::refToVstring(profile.ports), node);
}
/*!
 * @if jp
 * @brief `Call Timeout`を設定する。
 * @param ref `ref`の値。
 * @param seconds `seconds`の値。
 * @else
 * @brief Set call timeout.
 * @param ref Value of ref.
 * @param seconds Value of seconds.
 * @endif
 */
static void setCallTimeout(CORBA::Object_ptr ref, double seconds)
{
#ifdef ORB_IS_OMNIORB
  const double limit = static_cast<double>(std::numeric_limits<CORBA::ULong>::max());
  omniORB::setClientCallTimeout(ref, static_cast<CORBA::ULong>(std::min(limit, std::max(1.0, std::ceil(seconds * 1000)))));
#else
  (void)ref; (void)seconds;
#endif
}
/*!
 * @if jp
 * @brief `endpoint`に対応する処理を行う。
 * @param props 初期化に使用するプロパティ。
 * @param key `key`の値。
 * @param refkey `refkey`の値。
 * @else
 * @brief Perform the endpoint operation.
 * @param props Properties used for initialization.
 * @param key Value of key.
 * @param refkey Value of refkey.
 * @endif
 */
template<class Interface>
typename Interface::_var_type endpoint(const SDOPackage::NVList& props, const char* key, const char* refkey)
{
  auto ref = reference<Interface>(props, key);
  if (!CORBA::is_nil(ref)) return ref;
  const auto index = NVUtil::find_index(props, refkey);
  CORBA::Object_var object;
  if (index >= 0 && (props[index].value >>= CORBA::Any::to_object(object.out()))) return Interface::_narrow(object);
  return Interface::_nil();
}
} }

namespace RTC {
InPortCSPProvider::InPortCSPProvider()
{
  setInterfaceType("csp_channel");
  setDataFlowType("duplex");
  m_ref = _this();
  csp_detail::publish(m_properties, "dataport.csp_channel.inport_ior", m_ref);
  CORBA_SeqUtil::push_back(m_properties, NVUtil::newNV("dataport.csp_channel.inport_ref", m_ref));
}
InPortCSPProvider::~InPortCSPProvider() { close(); }
void InPortCSPProvider::close() { m_gate.close(); if (!m_deactivated.exchange(true)) csp_detail::deactivate(this); }
void InPortCSPProvider::setListener(ConnectorInfo& info, ConnectorListenersBase* listeners)
{ m_info = info; m_listeners = listeners; }
void InPortCSPProvider::setConnector(InPortConnector* connector)
{ m_connector = dynamic_cast<InPortDuplexConnector*>(connector); }
PortStatus InPortCSPProvider::put(const OctetSeq& data)
{
  csp_detail::Call call(m_gate);
  if (!call || !m_connector) return PORT_ERROR;
  try {
    auto bytes = csp_detail::toBytes(data, m_connector->isLittleEndian());
    // Admission precedes listeners so rejected or duplicate puts have no side effects.
    return csp_detail::wireStatus(m_connector->write(bytes));
  } catch (...) { return UNKNOWN_ERROR; }
}
CSP::Readiness InPortCSPProvider::is_writable(CORBA::Boolean retry, const CSP::NVList& option)
{
  csp_detail::Call call(m_gate);
  try {
    if (!call || !m_connector) return CSP::NOT_READY;
    return m_connector->writableReadiness(retry, option);
  } catch (...) { return CSP::NOT_READY; }
}
CORBA::Boolean InPortCSPProvider::match_condition(const CSP::NVList& option)
{
  csp_detail::Call call(m_gate);
  try { return call && m_connector && m_connector->matchCondition(option); }
  catch (...) { return false; }
}
OutPortCSPProvider::OutPortCSPProvider()
{
  setInterfaceType("csp_channel");
  setDataFlowType("duplex");
  m_ref = _this();
  csp_detail::publish(m_properties, "dataport.csp_channel.outport_ior", m_ref);
  CORBA_SeqUtil::push_back(m_properties, NVUtil::newNV("dataport.csp_channel.outport_ref", m_ref));
}
OutPortCSPProvider::~OutPortCSPProvider() { close(); }
void OutPortCSPProvider::close() { m_gate.close(); if (!m_deactivated.exchange(true)) csp_detail::deactivate(this); }
void OutPortCSPProvider::setListener(ConnectorInfo& info, ConnectorListenersBase* listeners)
{ m_info = info; m_listeners = listeners; }
void OutPortCSPProvider::setConnector(OutPortConnector* connector)
{ m_connector = dynamic_cast<OutPortDuplexConnector*>(connector); }
PortStatus OutPortCSPProvider::get(OctetSeq_out data)
{
  data = new OctetSeq;
  csp_detail::Call call(m_gate);
  if (!call || !m_connector) return PORT_ERROR;
  try {
    ByteData bytes;
    const auto status = m_connector->read(bytes);
    if (status == BufferStatus::OK) *data.ptr() = csp_detail::toOctets(bytes);
    return csp_detail::wireStatus(status);
  } catch (...) { return UNKNOWN_ERROR; }
}
CSP::Readiness OutPortCSPProvider::is_readable(CORBA::Boolean retry, CSP::InPortCsp_ptr provider)
{
  csp_detail::Call call(m_gate);
  try {
    if (!call || !m_connector) return CSP::NOT_READY;
    return m_connector->readableReadiness(retry, provider);
  } catch (...) { return CSP::NOT_READY; }
}
void InPortCSPConsumer::init(coil::Properties& prop) {
  m_timeout = csp_detail::timeout(prop);
  if (!CORBA::is_nil(m_ref)) csp_detail::setCallTimeout(m_ref, m_timeout);
}
bool InPortCSPConsumer::subscribeInterface(const SDOPackage::NVList& prop)
{
  try {
    m_ref = csp_detail::endpoint<CSP::InPortCsp>(prop, "dataport.csp_channel.inport_ior", "dataport.csp_channel.inport_ref");
    if (CORBA::is_nil(m_ref)) return false;
    csp_detail::setCallTimeout(m_ref, m_timeout);
    return true;
  } catch (...) { return false; }
}
DataPortStatus InPortCSPConsumer::put(ByteData& data)
{
  if (CORBA::is_nil(m_ref)) return DataPortStatus::PRECONDITION_NOT_MET;
  try {
    auto status = m_ref->put(csp_detail::toOctets(data));
    if (status == BUFFER_FULL) return DataPortStatus::SEND_FULL;
    if (status == BUFFER_TIMEOUT) return DataPortStatus::SEND_TIMEOUT;
    return csp_detail::portStatus(status);
  }
  catch (...) { return DataPortStatus::CONNECTION_LOST; }
}
bool InPortCSPConsumer::isWritable(bool retry, const CSP::NVList& option)
{ return writableReadiness(retry, option) == CSP::READY; }
CSP::Readiness InPortCSPConsumer::writableReadiness(bool retry, const CSP::NVList& option)
{
  try { return !CORBA::is_nil(m_ref) ? m_ref->is_writable(retry, option) : CSP::NOT_READY; }
  catch (...) { return CSP::NOT_READY; }
}
bool InPortCSPConsumer::matchCondition(const CSP::NVList& option)
{
  try { return !CORBA::is_nil(m_ref) && m_ref->match_condition(option); }
  catch (...) { return false; }
}
void OutPortCSPConsumer::init(coil::Properties& prop) {
  m_timeout = csp_detail::timeout(prop);
  if (!CORBA::is_nil(m_ref)) csp_detail::setCallTimeout(m_ref, m_timeout);
}
bool OutPortCSPConsumer::subscribeInterface(const SDOPackage::NVList& prop)
{
  try {
    m_ref = csp_detail::endpoint<CSP::OutPortCsp>(prop, "dataport.csp_channel.outport_ior", "dataport.csp_channel.outport_ref");
    if (CORBA::is_nil(m_ref)) return false;
    csp_detail::setCallTimeout(m_ref, m_timeout);
    return true;
  } catch (...) { return false; }
}
DataPortStatus OutPortCSPConsumer::get(ByteData& data)
{
  if (CORBA::is_nil(m_ref)) return DataPortStatus::PRECONDITION_NOT_MET;
  try {
    OctetSeq_var bytes;
    auto status = m_ref->get(bytes.out());
    if (status == PORT_OK) data.writeData(bytes->get_buffer(), bytes->length());
    return csp_detail::portStatus(status);
  } catch (...) { return DataPortStatus::CONNECTION_LOST; }
}
bool OutPortCSPConsumer::isReadable(bool retry, CSP::InPortCsp_ptr provider)
{ return readableReadiness(retry, provider) == CSP::READY; }
CSP::Readiness OutPortCSPConsumer::readableReadiness(bool retry, CSP::InPortCsp_ptr provider)
{
  try { return !CORBA::is_nil(m_ref) ? m_ref->is_readable(retry, provider) : CSP::NOT_READY; }
  catch (...) { return CSP::NOT_READY; }
}

InPortDuplexConnector::InPortDuplexConnector(ConnectorInfo& info, InPortCSPProvider* provider,
                                           ConnectorListenersBase* listeners)
  : InPortConnector(info, listeners, nullptr), m_provider(provider), m_storage(8)
{
  m_buffer = &m_storage;
  m_timeout = csp_detail::timeout(info.properties);
  auto props = info.properties.getNode("buffer");
  props.setProperty("write.full_policy", "do_nothing");
  props.setProperty("read.empty_policy", "do_nothing");
  m_storage.init(props);
  m_provider->setConnector(this);
  m_provider->setListener(m_profile, listeners);
}
InPortDuplexConnector::~InPortDuplexConnector()
{
  disconnect();
  InPortProvider* base = m_provider; InPortProviderFactory::instance().deleteObject(base); m_provider = nullptr;
}
DataPortStatus InPortDuplexConnector::disconnect()
{
  bool notify;
  { std::lock_guard<std::recursive_mutex> l(m_mutex); notify = !m_closed; m_closed = true; m_reserved = false; }
  if (m_provider) m_provider->close();
  if (notify && m_listeners) m_listeners->notify(ConnectorListenerType::ON_DISCONNECT, m_profile);
  return DataPortStatus::PORT_OK;
}
bool InPortDuplexConnector::matchCondition(const CSP::NVList& option)
{ return !m_condition || m_condition(option); }
bool InPortDuplexConnector::isWritable(bool retry, const CSP::NVList& option)
{ return writableReadiness(retry, option) == CSP::READY; }
CSP::Readiness InPortDuplexConnector::writableReadiness(bool retry, const CSP::NVList& option)
{
  if (!matchCondition(option)) return readinessState();
  // Do not retain a connector lock while user readiness calls into a manager.
  const auto result = m_writableState ? m_writableState(retry, option) :
    ((m_writable ? m_writable(retry, option) : !m_storage.full()) ? CSP::READY : readinessState());
  std::lock_guard<std::recursive_mutex> l(m_mutex);
  if (m_closed) return CSP::NOT_READY;
  if (result != CSP::READY) return result;
  m_reserved = true;
  m_grantExpires = csp_detail::Clock::now() + std::chrono::duration_cast<csp_detail::Clock::duration>(std::chrono::duration<double>(m_timeout));
  return CSP::READY;
}
BufferStatus InPortDuplexConnector::write(ByteData& data)
{
  {
    std::lock_guard<std::recursive_mutex> l(m_mutex);
    if (m_closed || !m_reserved || csp_detail::Clock::now() >= m_grantExpires) return BufferStatus::PRECONDITION_NOT_MET;
    m_reserved = false;
  }
  if (m_write) return m_write(data); // The port performs atomic admission and listeners.
  if (m_listeners) m_listeners->notifyIn(ConnectorDataListenerType::ON_RECEIVED, m_profile, data);
  auto status = m_storage.write(data);
  if (m_listeners) {
    if (status == BufferStatus::OK) m_listeners->notifyIn(ConnectorDataListenerType::ON_BUFFER_WRITE, m_profile, data);
    else m_listeners->notifyIn(ConnectorDataListenerType::ON_RECEIVER_ERROR, m_profile, data);
  }
  return status;
}
bool InPortDuplexConnector::isReadable(bool retry)
{
  if (!m_consumer) return false;
  CSP::InPortCsp_var ref = m_provider->object();
  return m_consumer->isReadable(retry, ref);
}
DataPortStatus InPortDuplexConnector::read(ByteDataStreamBase* data)
{
  ByteData bytes;
  DataPortStatus result;
  if (m_read) result = m_read(bytes);
  else if (!m_storage.empty()) result = csp_detail::portStatus(csp_detail::wireStatus(m_storage.read(bytes)));
  else result = isReadable() ? m_consumer->get(bytes) : DataPortStatus::BUFFER_EMPTY;
  if (result == DataPortStatus::PORT_OK) {
    bytes.isLittleEndian(m_littleEndian);
    if (m_listeners && !m_read) m_listeners->notifyIn(ConnectorDataListenerType::ON_BUFFER_READ, m_profile, bytes);
    data->writeData(bytes.getBuffer(), bytes.getDataLength());
  }
  return result;
}

OutPortDuplexConnector::OutPortDuplexConnector(ConnectorInfo& info, OutPortCSPProvider* provider,
                                             ConnectorListenersBase* listeners)
  : OutPortConnector(info, listeners), m_provider(provider), m_storage(1), m_timeout(csp_detail::timeout(info.properties))
{
  coil::Properties props;
  props.setProperty("length", "1"); props.setProperty("write.full_policy", "do_nothing");
  props.setProperty("read.empty_policy", "do_nothing");
  m_storage.init(props);
  m_provider->setConnector(this);
  m_provider->setListener(m_profile, listeners);
}
OutPortDuplexConnector::~OutPortDuplexConnector()
{
  disconnect();
  OutPortProvider* base = m_provider; OutPortProviderFactory::instance().deleteObject(base); m_provider = nullptr;
}
DataPortStatus OutPortDuplexConnector::disconnect()
{
  bool notify;
  { std::lock_guard<std::recursive_mutex> l(m_mutex); notify = !m_closed; m_closed = true; m_changed.notify_all(); }
  if (m_provider) m_provider->close();
  if (notify && m_listeners) m_listeners->notify(ConnectorListenerType::ON_DISCONNECT, m_profile);
  return DataPortStatus::PORT_OK;
}
bool OutPortDuplexConnector::isReadable(bool retry, CSP::InPortCsp_ptr provider)
{ return readableReadiness(retry, provider) == CSP::READY; }
CSP::Readiness OutPortDuplexConnector::readableReadiness(bool retry, CSP::InPortCsp_ptr provider)
{
  if (m_readableState) return m_readableState(retry, provider);
  if (m_readable) return m_readable(retry, provider) ? CSP::READY : readinessState();
  if (!m_consumer || CORBA::is_nil(provider)) return CSP::NOT_READY;
  CSP::InPortCsp_var remote = m_consumer->object();
  if (CORBA::is_nil(remote) || !remote->_is_equivalent(provider)) return CSP::NOT_READY;
  std::lock_guard<std::recursive_mutex> l(m_mutex);
  return !m_closed && !m_storage.empty() ? CSP::READY : CSP::NOT_READY;
}
bool OutPortDuplexConnector::isWritable(bool retry, const CSP::NVList& option)
{ return m_consumer && m_consumer->isWritable(retry, option); }
DataPortStatus OutPortDuplexConnector::send(ByteData& data, bool buffered)
{
  if (!m_consumer) return DataPortStatus::PRECONDITION_NOT_MET;
  if (buffered && m_listeners) m_listeners->notifyOut(ConnectorDataListenerType::ON_BUFFER_WRITE, m_profile, data);
  if (m_listeners) m_listeners->notifyOut(ConnectorDataListenerType::ON_SEND, m_profile, data);
  return m_consumer->put(data);
}
DataPortStatus OutPortDuplexConnector::write(ByteDataStreamBase* data)
{
  ByteData bytes;
  bytes.isLittleEndian(m_littleEndian);
  bytes.setDataLength(data->getDataLength());
  data->readData(bytes.getBuffer(), bytes.getDataLength());
  if (m_write) return m_write(bytes);
  if (m_listeners) m_listeners->notifyOut(ConnectorDataListenerType::ON_BUFFER_WRITE, m_profile, bytes);
  if (isWritable()) return send(bytes, false);
  std::unique_lock<std::recursive_mutex> l(m_mutex);
  if (m_closed) return DataPortStatus::CONNECTION_LOST;
  if (m_storage.write(bytes) != BufferStatus::OK) return DataPortStatus::BUFFER_FULL;
  if (!m_changed.wait_for(l, std::chrono::duration<double>(m_timeout), [this] { return m_closed || m_storage.empty(); })) {
    m_storage.reset();
    return DataPortStatus::BUFFER_TIMEOUT;
  }
  return m_closed ? DataPortStatus::CONNECTION_LOST : DataPortStatus::PORT_OK;
}
BufferStatus OutPortDuplexConnector::read(ByteData& data)
{
  BufferStatus status;
  if (m_read) status = m_read(data);
  else {
    std::lock_guard<std::recursive_mutex> l(m_mutex);
    status = m_closed ? BufferStatus::PRECONDITION_NOT_MET : m_storage.read(data);
    m_changed.notify_all();
  }
  if (status == BufferStatus::OK && m_listeners) {
    data.isLittleEndian(m_littleEndian);
    if (m_read) m_listeners->notifyOut(ConnectorDataListenerType::ON_BUFFER_WRITE, m_profile, data);
    m_listeners->notifyOut(ConnectorDataListenerType::ON_BUFFER_READ, m_profile, data);
    m_listeners->notifyOut(ConnectorDataListenerType::ON_SEND, m_profile, data);
  }
  return status;
}
}
extern "C" {
/*!
 * @if jp
 * @brief `InPortCSPProvider`のFactoryを登録する。
 * @else
 * @brief Register the factory for InPortCSPProvider.
 * @endif
 */
void InPortCSPProviderInit() {
  RTC::InPortProviderFactory::instance().addFactory("csp_channel", coil::Creator<RTC::InPortProvider, RTC::InPortCSPProvider>,
    [](RTC::InPortProvider*& base) { auto* provider = dynamic_cast<RTC::InPortCSPProvider*>(base); if (provider) { provider->close(); provider->_remove_ref(); } base = nullptr; });
}
/*!
 * @if jp
 * @brief `OutPortCSPProvider`のFactoryを登録する。
 * @else
 * @brief Register the factory for OutPortCSPProvider.
 * @endif
 */
void OutPortCSPProviderInit() {
  RTC::OutPortProviderFactory::instance().addFactory("csp_channel", coil::Creator<RTC::OutPortProvider, RTC::OutPortCSPProvider>,
    [](RTC::OutPortProvider*& base) { auto* provider = dynamic_cast<RTC::OutPortCSPProvider*>(base); if (provider) { provider->close(); provider->_remove_ref(); } base = nullptr; });
}
/*!
 * @if jp
 * @brief `InPortCSPConsumer`のFactoryを登録する。
 * @else
 * @brief Register the factory for InPortCSPConsumer.
 * @endif
 */
void InPortCSPConsumerInit() { RTC::InPortConsumerFactory::instance().addFactory("csp_channel", coil::Creator<RTC::InPortConsumer, RTC::InPortCSPConsumer>, coil::Destructor<RTC::InPortConsumer, RTC::InPortCSPConsumer>); }
/*!
 * @if jp
 * @brief `OutPortCSPConsumer`のFactoryを登録する。
 * @else
 * @brief Register the factory for OutPortCSPConsumer.
 * @endif
 */
void OutPortCSPConsumerInit() { RTC::OutPortConsumerFactory::instance().addFactory("csp_channel", coil::Creator<RTC::OutPortConsumer, RTC::OutPortCSPConsumer>, coil::Destructor<RTC::OutPortConsumer, RTC::OutPortCSPConsumer>); }
}

namespace RTC {
CSP::Readiness InPortDuplexConnector::readinessState() const {
  return m_state ? m_state() : CSP::NOT_READY;
}
CSP::Readiness OutPortDuplexConnector::readinessState() const {
  return m_state ? m_state() : CSP::NOT_READY;
}
}
