// -*- C++ -*-
/*!
 * @file Inputbutton.cpp
 * @if jp
 * @brief 電子レンジFSMへの入力イベントを送信するサンプルRTC。 実装。
 * @else
 * @brief Sample RTC sending input events to the microwave FSM. Implementation.
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

#include "Inputbutton.h"
#include <algorithm>
#include <iostream>
#include <sstream>
namespace csp_samples { namespace fsm {
namespace { const std::array<const char*, 6> names{{"open", "close", "minute", "start", "stop", "tick"}}; }
Inputbutton::Inputbutton(RTC::Manager* manager) : SampleComponent(manager) {
  for (size_t i = 0; i < m_ports.size(); ++i)
    m_ports[i].reset(new RTC::CSPOutPort<RTC::TimedLong>(names[i], m_values[i]));
}
Inputbutton::~Inputbutton() = default;
RTC::ReturnCode_t Inputbutton::onInitialize() {
  configureSample();
  for (size_t i = 0; i < m_ports.size(); ++i) addOutPort(names[i], *m_ports[i]);
  return RTC::RTC_OK;
}
RTC::ReturnCode_t Inputbutton::onExecute(RTC::UniqueId) {
  std::cout << "\nPlease select action!!\nCommands:\n"
               "  open         : Open the microwave's door.\n"
               "  close        : Close the microwave's door.\n"
               "  minute <int> : Increment timer.\n"
               "  start        : Start the microwave.\n"
               "  stop         : Stop the microwave.\n"
               "  tick         : Proceed time.\n"
               "  -> others are interpreted as tick command.\n>> " << std::flush;
  std::string line;
  if (!std::getline(std::cin, line)) return RTC::RTC_OK;
  std::istringstream input(line);
  std::string name; input >> name;
  auto found = std::find(names.begin(), names.end(), name);
  const size_t index = found == names.end() ? 5 : static_cast<size_t>(found - names.begin());
  CORBA::Long value = 0;
  if (index == 2) {
    std::string argument; input >> argument;
    std::istringstream number(argument);
    char extra;
    if (!(number >> value) || (number >> extra)) {
      std::cout << "minute command needs an integer arg." << std::endl;
      return RTC::RTC_OK;
    }
  }
  m_values[index].data = value;
  if (m_ports[index]->write()) sent(index, value);
  return RTC::RTC_OK;
}
void Inputbutton::closePorts() { for (auto& port : m_ports) port->close(); }
} }
/*!
 * @if jp
 * @brief `Inputbutton`のFactoryを登録する。
 * @else
 * @brief Register the factory for Inputbutton.
 * @endif
 */
extern "C" void InputbuttonInit(RTC::Manager* manager) {
  csp_samples::registerComponent<csp_samples::fsm::Inputbutton>(*manager, "Inputbutton", "CSP microwave console commands");
}
