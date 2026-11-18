// -*- C++ -*-
/*!
 * @file DataIn2.h
 * @if jp
 * @brief CSP入力ポートからデータを受信するサンプルRTC。 宣言。
 * @else
 * @brief Sample RTC receiving data through a CSP input port. Declarations.
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
#include <rtm/CSPInPort.h>
#include <memory>
namespace csp_samples { namespace basic {
/*!
 * @if jp
 * @brief CSP入力ポートからデータを受信するサンプルRTC。
 * @else
 * @brief Sample RTC receiving data through a CSP input port.
 * @endif
 */
class DataIn2 : public SampleComponent {
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
  explicit DataIn2(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~DataIn2() override;
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
  RTC::TimedLong m_value{};
  RTC::CSPInPort<RTC::TimedLong> m_port;
};
} }
/*!
 * @if jp
 * @brief `DataIn2`のFactoryを登録する。
 * @param manager 関連付けるManager。
 * @else
 * @brief Register the factory for DataIn2.
 * @param manager Manager to associate with the object.
 * @endif
 */
extern "C" { DLL_EXPORT void DataIn2Init(RTC::Manager* manager); }
