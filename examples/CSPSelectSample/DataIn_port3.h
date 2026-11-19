// -*- C++ -*-
/*!
 * @file DataIn_port3.h
 * @if jp
 * @brief `DataIn_port3`の状態と操作をまとめる。 宣言。
 * @else
 * @brief Encapsulate the state and operations of DataIn_port3. Declarations.
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
namespace csp_samples { namespace selection {
/*!
 * @if jp
 * @brief `DataIn_port3`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of DataIn_port3.
 * @endif
 */
class DataIn_port3 : public SampleComponent {
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
  explicit DataIn_port3(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~DataIn_port3() override;
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
  std::array<RTC::TimedLong, 3> m_values{};
  RTC::CSPManager m_selector;
  std::array<std::unique_ptr<RTC::CSPInPort<RTC::TimedLong>>, 3> m_ports;
};
} }
/*!
 * @if jp
 * @brief `DataIn_port3`のFactoryを登録する。
 * @param manager 関連付けるManager。
 * @else
 * @brief Register the factory for DataIn_port3.
 * @param manager Manager to associate with the object.
 * @endif
 */
extern "C" { DLL_EXPORT void DataIn_port3Init(RTC::Manager* manager); }
