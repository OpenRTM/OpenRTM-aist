// -*- C++ -*-
/*!
 * @file MicrowaveFsm.h
 * @if jp
 * @brief `MicrowaveFsm`の状態と操作をまとめる。 宣言。
 * @else
 * @brief Encapsulate the state and operations of MicrowaveFsm. Declarations.
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

// A simple simulation of an microwave oven:
// Timer can be programmed in minutes, last mode will be resumed if opened.
//
// Compile like this:
// g++ Macho.cpp Microwave.cpp

#ifndef CSP_MICROWAVEFSM_H
#define CSP_MICROWAVEFSM_H

#include <iostream>
#include <rtm/StaticFSM.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/RTC.h>



// Simple microwave simulation
namespace CspMicrowaveFsm
{

  /*!
   * @if jp
   * @brief 電子レンジ状態マシンサンプル
   *
   * Macho に付属の Microwave FSM のサンプル
   *
   * @else
   * @brief Microwave state machine sample
   *
   * Microwave FSM example included in Macho
   *
   * @endif
   */

  /*!
   * @if jp
   * @class TOP状態
   *
   *
   * @else
   * @brief TOP state
   *
   * @endif
   */
  FSM_TOPSTATE(Top)
  {
    // Top state data (visible to all substates)
    /*!
     * @if jp
     * @brief `Box`の状態と操作をまとめる。
     * @else
     * @brief Encapsulate the state and operations of Box.
     * @endif
     */
    struct Box
    {
      /*!
       * @if jp
       * @brief インスタンスと内部状態を初期化する。
       * @else
       * @brief Initialize the instance and its internal state.
       * @endif
       */
      Box()  {}
      /*!
       * @if jp
       * @brief 残りの調理時間を表示する。
       * @else
       * @brief Print the remaining cooking time.
       * @endif
       */
      void printTimer()
      {
        std::cout << "  Timer set to ";
        std::cout << myCookingTime << " minutes" << std::endl;
      }
      /*!
       * @if jp
       * @brief 調理タイマーを一つ増やす。
       * @else
       * @brief Increment the cooking timer by one.
       * @endif
       */
      void incrementTimer() { ++myCookingTime; }
      /*!
       * @if jp
       * @brief 調理タイマーを一つ減らす。
       * @else
       * @brief Decrement the cooking timer by one.
       * @endif
       */
      void decrementTimer() { -- myCookingTime; }
      /*!
       * @if jp
       * @brief 調理タイマーをゼロへ戻す。
       * @else
       * @brief Reset the cooking timer to zero.
       * @endif
       */
      void resetTimer() { myCookingTime = 0; }
      /*!
       * @if jp
       * @brief 残りの調理時間を取得する。
       * @else
       * @brief Get the remaining cooking time.
       * @endif
       */
      int getRemainingTime() const { return myCookingTime; }
    private:
      int myCookingTime{0};
    };

    FSM_STATE(Top);

    // Machine's event protocol
    /*!
     * @if jp
     * @brief `open`に対応する処理を行う。
     * @else
     * @brief Perform the open operation.
     * @endif
     */
    virtual void open() {}
    /*!
     * @if jp
     * @brief 新規I/Oを停止し、待機解除と処理中操作の終了を行う。
     * @else
     * @brief Stop new I/O, wake waiters and finish in-flight operations.
     * @endif
     */
    virtual void close() {}
    /*!
     * @if jp
     * @brief `minute`に対応する処理を行う。
     * @else
     * @brief Perform the minute operation.
     * @endif
     */
    virtual void minute(RTC::TimedLong /* time */) {}
    /*!
     * @if jp
     * @brief `start`に対応する処理を行う。
     * @else
     * @brief Perform the start operation.
     * @endif
     */
    virtual void start() {}		// Start cooking
    /*!
     * @if jp
     * @brief `stop`に対応する処理を行う。
     * @else
     * @brief Perform the stop operation.
     * @endif
     */
    virtual void stop() {}		// Stop cooking
    /*!
     * @if jp
     * @brief `tick`に対応する処理を行う。
     * @else
     * @brief Perform the tick operation.
     * @endif
     */
    virtual void tick() {}		// Minute has passed

  private:
    /*!
     * @if jp
     * @brief FSM状態の初期遷移を実行する。
     * @else
     * @brief Perform the initial transition of the FSM state.
     * @endif
     */
    RTC::ReturnCode_t onInit() override;
    /*!
     * @if jp
     * @brief FSM状態への入場処理を実行する。
     * @else
     * @brief Perform the FSM state entry action.
     * @endif
     */
    RTC::ReturnCode_t onEntry() override;
    /*!
     * @if jp
     * @brief FSM状態からの退場処理を実行する。
     * @else
     * @brief Perform the FSM state exit action.
     * @endif
     */
    RTC::ReturnCode_t onExit() override;
  };

  //============================================================
  // Microwave has been opened
  /*!
   * @if jp
   * @brief `Disabled`に対応するFSM状態を表す。
   * @else
   * @brief Represent the Disabled FSM state.
   * @endif
   */
  FSM_SUBSTATE(Disabled, Top)
  {
    FSM_STATE(Disabled);

    // Event handler
    /*!
     * @if jp
     * @brief 新規I/Oを停止し、待機解除と処理中操作の終了を行う。
     * @else
     * @brief Stop new I/O, wake waiters and finish in-flight operations.
     * @endif
     */
    void close() override;

  private:
    /*!
     * @if jp
     * @brief FSM状態への入場処理を実行する。
     * @else
     * @brief Perform the FSM state entry action.
     * @endif
     */
    RTC::ReturnCode_t onEntry() override;
    /*!
     * @if jp
     * @brief FSM状態からの退場処理を実行する。
     * @else
     * @brief Perform the FSM state exit action.
     * @endif
     */
    RTC::ReturnCode_t onExit() override;
  };

  //============================================================
  // Microwave is ready
  /*!
   * @if jp
   * @brief `Operational`に対応するFSM状態を表す。
   * @else
   * @brief Represent the Operational FSM state.
   * @endif
   */
  FSM_SUBSTATE(Operational, Top)
  {
    FSM_STATE(Operational);

    // State has history enabled
    DEEPHISTORY();

    // Event handler
    /*!
     * @if jp
     * @brief `open`に対応する処理を行う。
     * @else
     * @brief Perform the open operation.
     * @endif
     */
    void open() override;
    /*!
     * @if jp
     * @brief `stop`に対応する処理を行う。
     * @else
     * @brief Perform the stop operation.
     * @endif
     */
    void stop() override;

  private:
    /*!
     * @if jp
     * @brief FSM状態の初期遷移を実行する。
     * @else
     * @brief Perform the initial transition of the FSM state.
     * @endif
     */
    RTC::ReturnCode_t onInit() override;
  };

  //============================================================
  // Microwave is idling
  /*!
   * @if jp
   * @brief `Idle`に対応するFSM状態を表す。
   * @else
   * @brief Represent the Idle FSM state.
   * @endif
   */
  FSM_SUBSTATE(Idle, Operational)
  {
    FSM_STATE(Idle);

    /*!
     * @if jp
     * @brief `minute`に対応する処理を行う。
     * @param time `time`の値。
     * @else
     * @brief Perform the minute operation.
     * @param time Value of time.
     * @endif
     */
    void minute(RTC::TimedLong time) override;

  private:
    /*!
     * @if jp
     * @brief FSM状態への入場処理を実行する。
     * @else
     * @brief Perform the FSM state entry action.
     * @endif
     */
    RTC::ReturnCode_t onEntry() override;
  };

  //============================================================
  // Microwave is being programmed
  /*!
   * @if jp
   * @brief `Programmed`に対応するFSM状態を表す。
   * @else
   * @brief Represent the Programmed FSM state.
   * @endif
   */
  FSM_SUBSTATE(Programmed, Operational)
  {
    FSM_STATE(Programmed);

    /*!
     * @if jp
     * @brief `minute`に対応する処理を行う。
     * @param time `time`の値。
     * @else
     * @brief Perform the minute operation.
     * @param time Value of time.
     * @endif
     */
    void minute(RTC::TimedLong time) override;
    /*!
     * @if jp
     * @brief `start`に対応する処理を行う。
     * @else
     * @brief Perform the start operation.
     * @endif
     */
    void start() override;

  private:
    /*!
     * @if jp
     * @brief FSM状態への入場処理を実行する。
     * @else
     * @brief Perform the FSM state entry action.
     * @endif
     */
    RTC::ReturnCode_t onEntry() override;
    /*!
     * @if jp
     * @brief FSM状態の初期遷移を実行する。
     * @else
     * @brief Perform the initial transition of the FSM state.
     * @endif
     */
    RTC::ReturnCode_t onInit() override;
    /*!
     * @if jp
     * @brief FSM状態からの退場処理を実行する。
     * @else
     * @brief Perform the FSM state exit action.
     * @endif
     */
    RTC::ReturnCode_t onExit() override;
  };

  //============================================================
  // Microwave is heating
  /*!
   * @if jp
   * @brief `Cooking`に対応するFSM状態を表す。
   * @else
   * @brief Represent the Cooking FSM state.
   * @endif
   */
  FSM_SUBSTATE(Cooking, Programmed)
  {
    FSM_STATE(Cooking);

    /*!
     * @if jp
     * @brief `tick`に対応する処理を行う。
     * @else
     * @brief Perform the tick operation.
     * @endif
     */
    void tick() override;

  private:
    /*!
     * @if jp
     * @brief FSM状態への入場処理を実行する。
     * @else
     * @brief Perform the FSM state entry action.
     * @endif
     */
    RTC::ReturnCode_t onEntry() override;
    /*!
     * @if jp
     * @brief FSM状態からの退場処理を実行する。
     * @else
     * @brief Perform the FSM state exit action.
     * @endif
     */
    RTC::ReturnCode_t onExit() override;
  };
} // namespace CspMicrowaveFsm


#endif // CSP_MICROWAVEFSM_H
