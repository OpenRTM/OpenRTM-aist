// -*- C++ -*-
/*!
 * @file SampleSupport.cpp
 * @if jp
 * @brief `SampleSupport`の状態と操作をまとめる。 実装。
 * @else
 * @brief Encapsulate the state and operations of SampleSupport. Implementation.
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

#include "SampleSupport.h"
#include <iostream>
#include <mutex>

namespace csp_samples {
namespace {
std::mutex outputMutex;

}
SampleComponent::SampleComponent(RTC::Manager* manager)
  : RTC::DataFlowComponentBase(manager), m_sampleManager(manager) {
  for (size_t i = 0; i < m_reads.size(); ++i) { m_reads[i] = 0; m_writes[i] = 0; m_values[i] = 0; }
}
SampleComponent::~SampleComponent() = default;
RTC::ReturnCode_t SampleComponent::onFinalize() { closePorts(); return RTC::RTC_OK; }
void SampleComponent::configureSample(double selectTimeout) {
  const std::string timeout = std::to_string(selectTimeout);
  bindParameter("select_timeout", m_selectTimeout, timeout.c_str());
  m_quiet = coil::toBool(m_sampleManager->getConfig().getProperty("sample.quiet", "NO"), "YES", "NO", false);
}
unsigned long SampleComponent::reads(size_t index) const { return m_reads.at(index).load(); }
unsigned long SampleComponent::writes(size_t index) const { return m_writes.at(index).load(); }
CORBA::Long SampleComponent::lastValue(size_t index) const { return m_values.at(index).load(); }
void SampleComponent::received(size_t index, CORBA::Long value) {
  m_values.at(index) = value; ++m_reads.at(index);
  if (!m_quiet) { std::lock_guard<std::mutex> lock(outputMutex); std::cout << getInstanceName() << " read[" << index + 1 << "]=" << value << std::endl; }
}
void SampleComponent::sent(size_t index, CORBA::Long value) {
  ++m_writes.at(index);
  if (!m_quiet) { std::lock_guard<std::mutex> lock(outputMutex); std::cout << getInstanceName() << " write[" << index + 1 << "]=" << value << std::endl; }
}
}
