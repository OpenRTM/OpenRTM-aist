// -*- C++ -*-
/*!
 * @file DataInOut.h
 * @if jp
 * @brief CSP入力と出力を選択して実行するサンプルRTC。 宣言。
 * @else
 * @brief Sample RTC selecting between CSP input and output. Declarations.
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
#include <rtm/CSPOutPort.h>
#include <memory>
namespace csp_samples { namespace selection {
/*!
 * @if jp
 * @brief CSP入力と出力を選択して実行するサンプルRTC。
 * @else
 * @brief Sample RTC selecting between CSP input and output.
 * @endif
 */
class DataInOut : public SampleComponent {
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
  explicit DataInOut(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~DataInOut() override;
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
  std::array<RTC::TimedLong, 3> m_inputs{}, m_outputs{};
  RTC::CSPManager m_selector;
  std::array<std::unique_ptr<RTC::CSPInPort<RTC::TimedLong>>, 3> m_inPorts;
  std::array<std::unique_ptr<RTC::CSPOutPort<RTC::TimedLong>>, 3> m_outPorts;
};
} }
/*!
 * @if jp
 * @brief `DataInOut`のFactoryを登録する。
 * @param manager 関連付けるManager。
 * @else
 * @brief Register the factory for DataInOut.
 * @param manager Manager to associate with the object.
 * @endif
 */
extern "C" { DLL_EXPORT void DataInOutInit(RTC::Manager* manager); }
