// -*- C++ -*-
/*!
 * @file DataIn.cpp
 * @if jp
 * @brief 選択した入力ポートからデータを読み出すサンプルRTC。 実装。
 * @else
 * @brief Sample RTC receiving data from the selected input port. Implementation.
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

#include "DataIn.h"
namespace csp_samples { namespace selection {
DataIn::DataIn(RTC::Manager* manager) : SampleComponent(manager), m_port("in", m_value) {}
DataIn::~DataIn() = default;
RTC::ReturnCode_t DataIn::onInitialize() {
  configureSample();
  addInPort("in", m_port);
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataIn::onExecute(RTC::UniqueId) {
  SelectionScope selection(m_selector);
  m_port.setManager(&m_selector);
  auto selected = m_selector.select(m_selectTimeout);
  if (selected.inport == &m_port && m_port.readData()) received(0, m_value.data);
  return RTC::RTC_OK;
}
void DataIn::closePorts() {
  m_port.close();
}
} }
/*!
 * @if jp
 * @brief `DataIn`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataIn.
 * @endif
 */
extern "C" void DataInInit(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::selection::DataIn>(*manager, "DataIn", "CSP input sample");
}
