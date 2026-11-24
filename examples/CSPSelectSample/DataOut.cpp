// -*- C++ -*-
/*!
 * @file DataOut.cpp
 * @if jp
 * @brief CSP出力ポートからデータを送信するサンプルRTC。 実装。
 * @else
 * @brief Sample RTC sending data through CSP output ports. Implementation.
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

#include "DataOut.h"
namespace csp_samples { namespace selection {
DataOut::DataOut(RTC::Manager* manager) : SampleComponent(manager), m_port("out", m_value) {}
DataOut::~DataOut() = default;
RTC::ReturnCode_t DataOut::onInitialize() {
  configureSample();
  addOutPort("out", m_port);
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataOut::onExecute(RTC::UniqueId) {
  m_value.data = 1; ::setTimestamp(m_value);
  SelectionScope selection(m_selector);
  m_port.setManager(&m_selector);
  auto selected = m_selector.select(m_selectTimeout);
  if (selected.outport == &m_port && m_port.writeData()) sent(0, m_value.data);
  return RTC::RTC_OK;
}
void DataOut::closePorts() {
  m_port.close();
}
} }
/*!
 * @if jp
 * @brief `DataOut`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataOut.
 * @endif
 */
extern "C" void DataOutInit(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::selection::DataOut>(*manager, "DataOut", "CSP output sample");
}
