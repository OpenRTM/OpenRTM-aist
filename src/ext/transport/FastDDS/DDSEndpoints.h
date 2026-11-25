// -*- C++ -*-
/*!
 * @file  DDSEndpoints.h
 * @brief DDS endpoint lifecycle API for FastDDS
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

namespace openrtm_fastdds {

//! DDS GUID in wire byte order.
using Guid = std::array<std::uint8_t, 16>;
//! Direction of a DDS endpoint.
enum class Kind { reader, writer };
//! Events delivered by the endpoint registry.
enum class Event { added, removed, shutdown };

/*!
 * @if jp
 * @brief DDS の GUID をバイト配列に変換する
 * @tparam NativeGuid DDS ライブラリの GUID 型
 * @param guid 変換する GUID
 * @return 16 バイトの GUID
 * @else
 * @brief Convert a native DDS GUID to a byte array
 * @tparam NativeGuid GUID type provided by the DDS library
 * @param guid GUID to convert
 * @return 16-byte GUID
 * @endif
 */
template<class NativeGuid>
Guid guid_bytes(const NativeGuid& guid)
{
  Guid result{};
  std::copy_n(guid.guidPrefix.value, 12, result.begin());
  std::copy_n(guid.entityId.value, 4, result.begin() + 12);
  return result;
}

//! Callback receiving the event, direction, GUID, and marshaling type.
using Observer = void (*)(Event, Kind, const Guid&, const std::string&);

/*!
 * @if jp
 * @brief 端点通知のコールバックを設定する
 * @param callback 通知先。nullptr を指定すると通知を解除する
 * @details 登録済みの端点は直ちに通知される。コールバック内で本 API を呼び出さないこと。
 * @else
 * @brief Set the endpoint observer
 * @param callback Observer, or nullptr to disable notifications
 * @details Existing endpoints are reported immediately. The callback must not call this API.
 * @endif
 */
void observe(Observer callback);

/*!
 * @if jp
 * @brief DDS 端点を登録し、追加を通知する
 * @param kind 端点の方向
 * @param guid 端点の GUID
 * @param marshaling シリアライザ名
 * @else
 * @brief Register a DDS endpoint and notify the observer
 * @param kind Endpoint direction
 * @param guid Endpoint GUID
 * @param marshaling Serializer name
 * @endif
 */
void added(Kind kind, const Guid& guid, const std::string& marshaling);

/*!
 * @if jp
 * @brief DDS 端点の削除を通知し、登録を解除する
 * @param guid 端点の GUID
 * @else
 * @brief Notify the observer and remove a DDS endpoint
 * @param guid Endpoint GUID
 * @endif
 */
void removed(const Guid& guid);

/*!
 * @if jp
 * @brief 終了を通知し、コールバックと端点の登録を解除する
 * @else
 * @brief Notify the observer of shutdown and clear the observer and endpoint registry
 * @endif
 */
void shutdown();

}  // namespace openrtm_fastdds
