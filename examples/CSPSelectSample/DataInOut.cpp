// -*- C++ -*-
/*!
 * @file DataInOut.cpp
 * @if jp
 * @brief CSP入力と出力を選択して実行するサンプルRTC。 実装。
 * @else
 * @brief Sample RTC selecting between CSP input and output. Implementation.
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

#include "DataInOut.h"
namespace csp_samples { namespace selection {
DataInOut::DataInOut(RTC::Manager* manager) : SampleComponent(manager) {
  for (size_t i = 0; i < m_inPorts.size(); ++i) {
    const std::string suffix = std::to_string(i + 1);
    m_inPorts[i].reset(new RTC::CSPInPort<RTC::TimedLong>(("in" + suffix).c_str(), m_inputs[i]));
    m_outPorts[i].reset(new RTC::CSPOutPort<RTC::TimedLong>(("out" + suffix).c_str(), m_outputs[i]));
  }
}
DataInOut::~DataInOut() = default;
RTC::ReturnCode_t DataInOut::onInitialize() {
  configureSample();
  for (size_t i = 0; i < m_inPorts.size(); ++i) {
    const std::string suffix = std::to_string(i + 1);
    addInPort(("in" + suffix).c_str(), *m_inPorts[i]);
    addOutPort(("out" + suffix).c_str(), *m_outPorts[i]);
  }
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataInOut::onExecute(RTC::UniqueId) {
  for (auto& value : m_outputs) { value.data = 1; }
  SelectionScope selection(m_selector);
  for (auto& port : m_inPorts) port->setManager(&m_selector);
  for (auto& port : m_outPorts) port->setManager(&m_selector);
  auto selected = m_selector.select(m_selectTimeout);
  for (size_t i = 0; i < m_inPorts.size(); ++i) {
    if (selected.inport == m_inPorts[i].get() && m_inPorts[i]->readData()) received(i, m_inputs[i].data);
    else if (selected.outport == m_outPorts[i].get() && m_outPorts[i]->writeData()) sent(i, m_outputs[i].data);
  }
  return RTC::RTC_OK;
}
void DataInOut::closePorts() {
  for (auto& port : m_inPorts) port->close();
  for (auto& port : m_outPorts) port->close();
}
} }
/*!
 * @if jp
 * @brief `DataInOut`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataInOut.
 * @endif
 */
extern "C" void DataInOutInit(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::selection::DataInOut>(*manager, "DataInOut", "CSP three-input/three-output selection sample");
}
