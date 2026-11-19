// -*- C++ -*-
/*!
 * @file SampleSupport.h
 * @if jp
 * @brief `SampleSupport`の状態と操作をまとめる。 宣言。
 * @else
 * @brief Encapsulate the state and operations of SampleSupport. Declarations.
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

#ifndef OPENRTM_CSP_SAMPLE_SUPPORT_H
#define OPENRTM_CSP_SAMPLE_SUPPORT_H
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/Manager.h>
#include <rtm/Timestamp.h>
#include <rtm/CSPManager.h>
#include <array>
#include <atomic>
#include <functional>
#include <string>
#include <vector>

namespace csp_samples {
/// @brief 選択済み通信の処理後に候補を必ず解除する。 / Always detach candidates after handling the selected communication.
class SelectionScope {
public:
  /// @brief 今回の選択器を保持する。 / Retain the selector for this operation.
  explicit SelectionScope(RTC::CSPManager& manager) : m_manager(manager) {}
  /// @brief 通常終了・例外の両方で関連付けを解除する。 / Detach on normal completion and exceptions.
  ~SelectionScope() { m_manager.reset(); }
  SelectionScope(const SelectionScope&) = delete;
  SelectionScope& operator=(const SelectionScope&) = delete;
private:
  RTC::CSPManager& m_manager;
};
// Observable counters let the demos verify real execution-context callbacks.
/*!
 * @if jp
 * @brief `SampleComponent`の状態と操作をまとめる。
 * @else
 * @brief Encapsulate the state and operations of SampleComponent.
 * @endif
 */
class SampleComponent : public RTC::DataFlowComponentBase {
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
  explicit SampleComponent(RTC::Manager* manager);
  /*!
   * @if jp
   * @brief 保持している資源を解放する。
   * @else
   * @brief Release the resources owned by the object.
   * @endif
   */
  ~SampleComponent() override;
  /*!
   * @if jp
   * @brief RTCの終了処理を行う。
   * @else
   * @brief Finalize the RTC resources.
   * @endif
   */
  RTC::ReturnCode_t onFinalize() override;
  /*!
   * @if jp
   * @brief `reads`に対応する処理を行う。
   * @param index `index`の値。
   * @else
   * @brief Perform the reads operation.
   * @param index Value of index.
   * @endif
   */
  unsigned long reads(size_t index = 0) const;
  /*!
   * @if jp
   * @brief `writes`に対応する処理を行う。
   * @param index `index`の値。
   * @else
   * @brief Perform the writes operation.
   * @param index Value of index.
   * @endif
   */
  unsigned long writes(size_t index = 0) const;
  /*!
   * @if jp
   * @brief `last Value`に対応する処理を行う。
   * @param index `index`の値。
   * @else
   * @brief Perform the last value operation.
   * @param index Value of index.
   * @endif
   */
  CORBA::Long lastValue(size_t index = 0) const;
protected:
  /*!
   * @if jp
   * @brief `configure Sample`に対応する処理を行う。
   * @param selectTimeout `selectTimeout`の値。
   * @else
   * @brief Perform the configure sample operation.
   * @param selectTimeout Value of selectTimeout.
   * @endif
   */
  void configureSample(double selectTimeout = 100.0);
  /*!
   * @if jp
   * @brief `received`に対応する処理を行う。
   * @param index `index`の値。
   * @param value 設定または受け渡す値。
   * @else
   * @brief Perform the received operation.
   * @param index Value of index.
   * @param value Value to configure or transfer.
   * @endif
   */
  void received(size_t index, CORBA::Long value);
  /*!
   * @if jp
   * @brief `sent`に対応する処理を行う。
   * @param index `index`の値。
   * @param value 設定または受け渡す値。
   * @else
   * @brief Perform the sent operation.
   * @param index Value of index.
   * @param value Value to configure or transfer.
   * @endif
   */
  void sent(size_t index, CORBA::Long value);
  /*!
   * @if jp
   * @brief サンプルが保持するCSPポートを終了する。
   * @else
   * @brief Close the CSP ports owned by the sample.
   * @endif
   */
  virtual void closePorts() = 0;
  double m_selectTimeout{100.0};
private:
  RTC::Manager* m_sampleManager;
  bool m_quiet{false};
  std::array<std::atomic<unsigned long>, 6> m_reads, m_writes;
  std::array<std::atomic<CORBA::Long>, 6> m_values;
};
/*!
 * @if jp
 * @brief `register Component`に対応する処理を行う。
 * @param manager 関連付けるManager。
 * @param name 識別に使用する名前。
 * @param description `description`の値。
 * @else
 * @brief Perform the register component operation.
 * @param manager Manager to associate with the object.
 * @param name Name identifying the object.
 * @param description Value of description.
 * @endif
 */
template<class Component>
void registerComponent(RTC::Manager& manager, const char* name, const char* description) {
  const char* profile[] = {
    "implementation_id", name, "type_name", name, "description", description,
    "version", "1.0", "vendor", "OpenRTM", "category", "example",
    "activity_type", "DataFlowComponent", "kind", "DataFlowComponent", "max_instance", "10",
    "language", "C++", "lang_type", "compile",
    ""
  };
  coil::Properties properties(profile);
  manager.registerFactory(properties, RTC::Create<Component>, RTC::Delete<Component>);
}

}
#endif
