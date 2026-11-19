// -*- C++ -*-
/*!
 * @file DataIn_port3.cpp
 * @if jp
 * @brief `DataIn_port3`の状態と操作をまとめる。 実装。
 * @else
 * @brief Encapsulate the state and operations of DataIn_port3. Implementation.
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

#include "DataIn_port3.h"
namespace csp_samples { namespace selection {
DataIn_port3::DataIn_port3(RTC::Manager* manager) : SampleComponent(manager) {
  for (size_t i = 0; i < m_ports.size(); ++i) {
    const std::string port = "in" + std::to_string(i + 1);
    m_ports[i].reset(new RTC::CSPInPort<RTC::TimedLong>(port.c_str(), m_values[i]));
  }
}
DataIn_port3::~DataIn_port3() = default;
RTC::ReturnCode_t DataIn_port3::onInitialize() {
  configureSample();
  for (size_t i = 0; i < m_ports.size(); ++i) {
    const std::string port = "in" + std::to_string(i + 1);
    addInPort(port.c_str(), *m_ports[i]);
  }
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataIn_port3::onExecute(RTC::UniqueId) {
  SelectionScope selection(m_selector);
  for (auto& port : m_ports) port->setManager(&m_selector);
  auto selected = m_selector.select(m_selectTimeout);
  for (size_t i = 0; i < m_ports.size(); ++i)
    if (selected.inport == m_ports[i].get() && m_ports[i]->readData()) received(i, m_values[i].data);
  return RTC::RTC_OK;
}
void DataIn_port3::closePorts() {
  for (auto& port : m_ports) port->close();
}
} }
/*!
 * @if jp
 * @brief `DataIn_port3`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataIn_port3.
 * @endif
 */
extern "C" void DataIn_port3Init(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::selection::DataIn_port3>(*manager, "DataIn_port3", "CSP input sample");
}
