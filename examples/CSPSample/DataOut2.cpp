// -*- C++ -*-
/*!
 * @file DataOut2.cpp
 * @if jp
 * @brief CSP出力ポートからデータを送信するサンプルRTC。 実装。
 * @else
 * @brief Sample RTC sending data through a CSP output port. Implementation.
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

#include "DataOut2.h"
namespace csp_samples { namespace basic {
DataOut2::DataOut2(RTC::Manager* manager) : SampleComponent(manager), m_port("out", m_value) {}
DataOut2::~DataOut2() = default;
RTC::ReturnCode_t DataOut2::onInitialize() {
  configureSample();
  addOutPort("out", m_port);
  return RTC::RTC_OK;
}
RTC::ReturnCode_t DataOut2::onExecute(RTC::UniqueId) {
  m_value.data = 1;
  if (m_port.write()) sent(0, m_value.data);
  return RTC::RTC_OK;
}
void DataOut2::closePorts() {
  m_port.close();
}
} }
/*!
 * @if jp
 * @brief `DataOut2`のFactoryを登録する。
 * @else
 * @brief Register the factory for DataOut2.
 * @endif
 */
extern "C" void DataOut2Init(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::basic::DataOut2>(*manager, "DataOut2", "CSP output sample");
}
