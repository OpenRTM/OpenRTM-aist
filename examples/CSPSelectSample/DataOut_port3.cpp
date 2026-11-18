// -*- C++ -*-
/*!
 * @file DataOut_port3.cpp
 * @if jp
 * @brief `DataOut_port3`の状態と操作をまとめる。 実装。
 * @else
 * @brief Encapsulate the state and operations of DataOut_port3. Implementation.
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

#include "DataOut_port3.h"
namespace csp_samples { namespace selection {
DataOut_port3::DataOut_port3(RTC::Manager* manager) : SampleComponent(manager) {
  for (size_t i = 0; i < m_ports.size(); ++i) {
    const std::string port = "out" + std::to_string(i + 1);
    m_ports[i].reset(new RTC::CSPOutPort<RTC::TimedLong>(port.c_str(), m_values[i]));
  }
}
DataOut_port3::~DataOut_port3() = default;
RTC::ReturnCode_t DataOut_port3::onInitialize() {
  configureSample();
  for (size_t i = 0; i < m_ports.size(); ++i) {
    const std::string port = "out" + std::to_string(i + 1);
    addOutPort(port.c_str(), *m_ports[i]);
  }
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataOut_port3::onExecute(RTC::UniqueId) {
  for (size_t i = 0; i < m_values.size(); ++i) { m_values[i].data = static_cast<CORBA::Long>(i + 1); ::setTimestamp(m_values[i]); }
  SelectionScope selection(m_selector);
  for (auto& port : m_ports) port->setManager(&m_selector);
  auto selected = m_selector.select(m_selectTimeout);
  for (size_t i = 0; i < m_ports.size(); ++i)
    if (selected.outport == m_ports[i].get() && m_ports[i]->writeData()) sent(i, m_values[i].data);
  return RTC::RTC_OK;
}
void DataOut_port3::closePorts() {
  for (auto& port : m_ports) port->close();
}
} }
/*!
 * @if jp
 * @brief `DataOut_port3`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataOut_port3.
 * @endif
 */
extern "C" void DataOut_port3Init(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::selection::DataOut_port3>(*manager, "DataOut_port3", "CSP output sample");
}
