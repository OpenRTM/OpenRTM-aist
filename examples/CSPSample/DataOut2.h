// -*- C++ -*-
/*!
 * @file DataOut2.h
 * @if jp
 * @brief CSP出力ポートからデータを送信するサンプルRTC。 宣言。
 * @else
 * @brief Sample RTC sending data through a CSP output port. Declarations.
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
namespace csp_samples { namespace basic {
/*!
 * @if jp
 * @brief CSP出力ポートからデータを送信するサンプルRTC。
 * @else
 * @brief Sample RTC sending data through a CSP output port.
 * @endif
 */
class DataOut2 : public SampleComponent {
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
  explicit DataOut2(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~DataOut2() override;
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
  RTC::CSPOutPort<RTC::TimedLong> m_port;
};
} }
/*!
 * @if jp
 * @brief `DataOut2`のFactoryを登録する。
 * @param manager 関連付けるManager。
 * @else
 * @brief Register the factory for DataOut2.
 * @param manager Manager to associate with the object.
 * @endif
 */
extern "C" { DLL_EXPORT void DataOut2Init(RTC::Manager* manager); }
