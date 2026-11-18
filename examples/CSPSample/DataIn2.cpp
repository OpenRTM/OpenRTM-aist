// -*- C++ -*-
/*!
 * @file DataIn2.cpp
 * @if jp
 * @brief CSP入力ポートからデータを受信するサンプルRTC。 実装。
 * @else
 * @brief Sample RTC receiving data through a CSP input port. Implementation.
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

#include "DataIn2.h"
namespace csp_samples { namespace basic {
DataIn2::DataIn2(RTC::Manager* manager) : SampleComponent(manager), m_port("in", m_value) {}
DataIn2::~DataIn2() = default;
RTC::ReturnCode_t DataIn2::onInitialize() {
  configureSample();
  addInPort("in", m_port);
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataIn2::onExecute(RTC::UniqueId) {
  if (m_port.read()) received(0, m_value.data);
  return RTC::RTC_OK;
}
void DataIn2::closePorts() {
  m_port.close();
}
} }
/*!
 * @if jp
 * @brief `DataIn2`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataIn2.
 * @endif
 */
extern "C" void DataIn2Init(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::basic::DataIn2>(*manager, "DataIn2", "CSP input sample");
}
