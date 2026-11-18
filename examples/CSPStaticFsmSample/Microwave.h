// -*- C++ -*-
/*!
 * @file Microwave.h
 * @if jp
 * @brief CSPイベントで電子レンジFSMを駆動するサンプルRTC。 宣言。
 * @else
 * @brief Sample RTC driving a microwave FSM with CSP events. Declarations.
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

#pragma once
#include "SampleSupport.h"
#include "MicrowaveFsm.h"
#include <rtm/CSPMachine.h>
#include <rtm/CSPEventPort.h>
#include <memory>
namespace csp_samples { namespace fsm {
/*!
 * @if jp
 * @brief CSPイベントで電子レンジFSMを駆動するサンプルRTC。
 * @else
 * @brief Sample RTC driving a microwave FSM with CSP events.
 * @endif
 */
class Microwave : public SampleComponent {
public:
  enum class State { DoorOpen, Idle, Programmed, Cooking };
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @param manager 関連付けるManager。
   * @else
   * @brief Initialize the instance and its internal state.
   * @param manager Manager to associate with the object.
   * @endif
   */
  explicit Microwave(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~Microwave() override;
  /*!
   * @if jp
   * @brief RTCのポート・設定・FSMを初期化する。
   * @else
   * @brief Initialize RTC ports, configuration and FSM state.
   * @endif
   */
  RTC::ReturnCode_t onInitialize() override;
  /*!
   * @if jp
   * @brief 周期実行に対応する通信・制御処理を行う。
   * @param ec 実行コンテキストの識別子。
   * @else
   * @brief Perform communication and control for a periodic execution callback.
   * @param ec Execution context identifier.
   * @endif
   */
  RTC::ReturnCode_t onExecute(RTC::UniqueId ec) override;
  /*!
   * @if jp
   * @brief `state`に対応する処理を行う。
   * @else
   * @brief Perform the state operation.
   * @endif
   */
  State state() const { return m_state.load(); }
  /*!
   * @if jp
   * @brief `remaining`に対応する処理を行う。
   * @else
   * @brief Perform the remaining operation.
   * @endif
   */
  int remaining() const { return m_remaining.load(); }
protected:
  /*!
   * @if jp
   * @brief サンプルが保持するCSPポートを終了する。
   * @else
   * @brief Close the CSP ports owned by the sample.
   * @endif
   */
  void closePorts() override;
private:
  /*!
   * @if jp
   * @brief `update Snapshot`に対応する処理を行う。
   * @else
   * @brief Perform the update snapshot operation.
   * @endif
   */
  void updateSnapshot();
  RTC::CSPManager m_selector;
  std::unique_ptr<RTC::CSPMachine<CspMicrowaveFsm::Top>> m_fsm;
  RTC::CSPEventPort<> m_eventIn;
  std::atomic<State> m_state{State::Idle};
  std::atomic<int> m_remaining{0};
};
} }
/*!
 * @if jp
 * @brief `Microwave`のFactoryを登録する。
 * @param manager 関連付けるManager。
 * @else
 * @brief Register the factory for Microwave.
 * @param manager Manager to associate with the object.
 * @endif
 */
extern "C" { DLL_EXPORT void MicrowaveInit(RTC::Manager* manager); }
