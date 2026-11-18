// -*- C++ -*-
/*!
 * @file CSPMachine.h
 * @if jp
 * @brief CSP通信とStaticFSMの実行を連携する。 宣言。
 * @else
 * @brief Integrate CSP communication with StaticFSM execution. Declarations.
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

#ifndef RTC_CSPMACHINE_H
#define RTC_CSPMACHINE_H
#include <rtm/StaticFSM.h>
#include <rtm/CSPManager.h>

namespace RTC {
/** StaticFSM machine driven by CSP event inputs and ready data outputs. */
/*!
 * @if jp
 * @brief CSP通信とStaticFSMの実行を連携する。
 * @else
 * @brief Integrate CSP communication with StaticFSM execution.
 * @endif
 */
template<class TOP>
class CSPMachine : public Machine<TOP>
{
public:
  /*!
   * @if jp
   * @brief `CSPMachine`に対応する処理を行う。
   * @param component `component`の値。
   * @param manager 関連付けるManager。
   * @else
   * @brief Perform the cspmachine operation.
   * @param component Value of component.
   * @param manager Manager to associate with the object.
   * @endif
   */
  CSPMachine(RTObject_impl* component, CSPManager& manager)
    : Machine<TOP>(component), m_component(component), m_manager(manager) {}
  /*!
   * @if jp
   * @brief FSMが保持するRTCを取得する。
   * @else
   * @brief Get the RTC owned by the FSM context.
   * @endif
   */
  RTObject_impl* getComp() const { return m_component; }
  /*!
   * @if jp
   * @brief 通信を選択して入出力またはFSMイベントを実行する。
   * @param timeout 待機期限。秒単位。
   * @else
   * @brief Select communication and perform I/O or execute the FSM event.
   * @param timeout Wait timeout in seconds.
   * @endif
   */
  CSPSelectable* runEvent(double timeout = 10.0)
  {
    auto result = m_manager.select(timeout);
    auto* port = result.inport ? result.inport : result.outport;
    return port && port->selectedIO() ? port : nullptr;
  }
  /*!
   * @if jp
   * @brief 通信を選択して入出力またはFSMイベントを実行する。
   * @param timeout 待機期限。秒単位。
   * @else
   * @brief Select communication and perform I/O or execute the FSM event.
   * @param timeout Wait timeout in seconds.
   * @endif
   */
  CSPSelectable* run_event(double timeout) { return runEvent(timeout); }
  /*!
   * @if jp
   * @brief 通信を選択して入出力またはFSMイベントを実行する。
   * @else
   * @brief Select communication and perform I/O or execute the FSM event.
   * @endif
   */
  void run_event() override { runEvent(); }
private:
  RTObject_impl* m_component;
  CSPManager& m_manager;
};
}
#endif
