// -*- C++ -*-
/*!
 * @file Microwave.cpp
 * @if jp
 * @brief CSPイベントで電子レンジFSMを駆動するサンプルRTC。 実装。
 * @else
 * @brief Sample RTC driving a microwave FSM with CSP events. Implementation.
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

#include "Microwave.h"
namespace csp_samples { namespace fsm {
Microwave::Microwave(RTC::Manager* manager) : SampleComponent(manager), m_eventIn("event") {}
Microwave::~Microwave() = default;
RTC::ReturnCode_t Microwave::onInitialize() {
  configureSample(10.0);
  m_fsm.reset(new RTC::CSPMachine<CspMicrowaveFsm::Top>(this, m_selector));
  m_eventIn.bindEvent("open", *m_fsm, &CspMicrowaveFsm::Top::open);
  m_eventIn.bindEvent("close", *m_fsm, &CspMicrowaveFsm::Top::close);
  m_eventIn.bindEvent("minute", *m_fsm, &CspMicrowaveFsm::Top::minute);
  m_eventIn.bindEvent("start", *m_fsm, &CspMicrowaveFsm::Top::start);
  m_eventIn.bindEvent("stop", *m_fsm, &CspMicrowaveFsm::Top::stop);
  m_eventIn.bindEvent("tick", *m_fsm, &CspMicrowaveFsm::Top::tick);
  addInPort("event", m_eventIn);
  updateSnapshot();
  return RTC::RTC_OK;
}
RTC::ReturnCode_t Microwave::onExecute(RTC::UniqueId) {
  SelectionScope selection(m_selector);
  m_eventIn.setManager(&m_selector);
  if (m_fsm->runEvent(m_selectTimeout)) { updateSnapshot(); received(0, static_cast<CORBA::Long>(m_remaining.load())); }
  return RTC::RTC_OK;
}
void Microwave::updateSnapshot() {
  m_remaining = m_fsm->box().getRemainingTime();
  if (CspMicrowaveFsm::Disabled::isCurrent(*m_fsm)) m_state = State::DoorOpen;
  else if (CspMicrowaveFsm::Cooking::isCurrent(*m_fsm)) m_state = State::Cooking;
  else if (CspMicrowaveFsm::Programmed::isCurrent(*m_fsm)) m_state = State::Programmed;
  else m_state = State::Idle;
}
void Microwave::closePorts() { m_eventIn.close(); m_fsm.reset(); }
} }
/*!
 * @if jp
 * @brief `Microwave`のFactoryを登録する。
 * @else
 * @brief Register the factory for Microwave.
 * @endif
 */
extern "C" void MicrowaveInit(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::fsm::Microwave>(*manager, "Microwave", "CSP StaticFSM microwave");
}
