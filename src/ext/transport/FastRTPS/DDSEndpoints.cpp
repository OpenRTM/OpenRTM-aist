// -*- C++ -*-
/*!
 * @file  DDSEndpoints.cpp
 * @brief DDS endpoint registry for FastRTPS
 */

#include "DDSEndpoints.h"

#include <map>
#include <mutex>

namespace openrtm_fastrtps {
namespace {
std::mutex mutex;
std::map<Guid, std::pair<Kind, std::string>> endpoints;
Observer observer = nullptr;
}

void observe(Observer callback)
{
  std::lock_guard<std::mutex> lock(mutex);
  observer = callback;
  if (observer) {
    for (const auto& endpoint : endpoints) {
      observer(Event::added, endpoint.second.first, endpoint.first, endpoint.second.second);
    }
  }
}

void added(Kind kind, const Guid& guid, const std::string& marshaling)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (endpoints.emplace(guid, std::make_pair(kind, marshaling)).second && observer) {
    observer(Event::added, kind, guid, marshaling);
  }
}

void removed(const Guid& guid)
{
  std::lock_guard<std::mutex> lock(mutex);
  const auto endpoint = endpoints.find(guid);
  if (endpoint == endpoints.end()) {
    return;
  }
  if (observer) {
    observer(Event::removed, endpoint->second.first, guid, endpoint->second.second);
  }
  endpoints.erase(endpoint);
}

void shutdown()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (observer) {
    observer(Event::shutdown, Kind::reader, Guid{}, {});
    observer = nullptr;
  }
  endpoints.clear();
}

}  // namespace openrtm_fastrtps
