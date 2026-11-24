// -*- C++ -*-
/*!
 * @file Inputbutton.h
 * @if jp
 * @brief 電子レンジFSMへの入力イベントを送信するサンプルRTC。 宣言。
 * @else
 * @brief Sample RTC sending input events to the microwave FSM. Declarations.
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
#include <rtm/CSPOutPort.h>
#include <memory>
namespace csp_samples { namespace fsm {
/*!
 * @if jp
 * @brief 電子レンジFSMへの入力イベントを送信するサンプルRTC。
 * @else
 * @brief Sample RTC sending input events to the microwave FSM.
 * @endif
 */
class Inputbutton : public SampleComponent {
public:
  /*!
   * @if jp
   * @brief インスタンスと内部状態を初期化する。
   * @param manager 関連付けるManager。
   * @else
   * @brief Initialize the instance and its internal state.
   * @param manager Manager to associate with the object.
   * @endif
   */
  explicit Inputbutton(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~Inputbutton() override;
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
  std::array<RTC::TimedLong, 6> m_values{};
  std::array<std::unique_ptr<RTC::CSPOutPort<RTC::TimedLong>>, 6> m_ports;
};
} }
/*!
 * @if jp
 * @brief `Inputbutton`のFactoryを登録する。
 * @param manager 関連付けるManager。
 * @else
 * @brief Register the factory for Inputbutton.
 * @param manager Manager to associate with the object.
 * @endif
 */
extern "C" { DLL_EXPORT void InputbuttonInit(RTC::Manager* manager); }
