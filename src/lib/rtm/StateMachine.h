// -*- C++ -*-
/*!
 * @file StateMachine.h
 * @brief State machine template class
 * @date $Date: 2007-09-20 11:21:12 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef RTC_STATEMACHINE_H
#define RTC_STATEMACHINE_H

#include <coil/Mutex.h>
#include <coil/Guard.h>

#include <cassert>
#include <utility>
#include <vector>

namespace RTC_Utils
{
  /*!
   * @if jp
   * @class StateHolder
   * @brief 状態保持用クラス
   *
   * 状態を保持するためのホルダークラス。
   * 現在の状態と、１つ前の状態、遷移予定の状態を保持する。
   *
   * @param State 保持する状態の型
   *
   * @since 0.4.0
   *
   * @else
   * @class StateHolder
   * @brief State holder class
   *
   * This is a holder class to hold states.
   * Hold current state, the previous state and the
   * state to be expected to transfer.
   *
   * @param State Type of state for holding
   *
   * @since 0.4.0
   *
   * @endif
   */
  template <class State>
  struct StateHolder
  {
    State curr;
    State prev;
    State next;
  };

  /*!
   * @if jp
   *
   * @class StateMachine
   *
   * @brief 状態マシンクラス
   *
   * StateMachine クラスは状態マシンを実現するクラスである。
   *
   * 例: ActiveObjectは状態マシンを持つアクティブオブジェクトであるとする。
   * 状態は3状態 INACTIVE, ACTIVE, ERROR があり、各状態でのEntryやExit動作を
   * 定義したいとすると、以下のように実現される。
   * <pre>
   * class ActiveObject
   * {
   * public:
   *   enum MyState { INACTIVE, ACTIVE, ERROR };
   *   typedef States<MyState> MyStates;
   *
   *   ActiveObject()
   *     : m_sm(3)
   *   {
   *     m_sm.setNOP(&ActiveObject::nullAction);
   *     m_sm.setListener(this);
   *
   *     m_sm.setExitAction(INACTIVE, &ActiveObject::inactiveExit);
   *       :
   *     m_sm.setPostDoAction(ERROR, &ActiveObject::errorPostDo);
   *     m_sm.setTransitionAction(&ActiveObject:transition);
   *   };
   *
   *   bool nullAction(MyStates st) {};
   *   bool inactiveExit(MyStates st) {};
   *     :
   *   bool errorPostDo(MyStates st) {};
   *   bool transition(MyStates st) {};
   *
   * private:
   *   StateMachine<MyState, bool, ActiveObject> m_sm;
   * };
   * </pre>
   * 状態を持たせたいクラスは以下の条件を満たすように実装しなければならない。
   * <ol>
   * <li> enum で状態を定義
   * <li> StateMachine のテンプレート引数は、<br>
   *   <状態の型(MyState), リスナーオブジェクト, 状態ホルダー，コールバック関数>
   * <li> StateMachine のコンストラクタ引数は状態の数
   * <li> 以下のアクション関数を(Return _function_name_(States)) の関数として設定
   * <ol>
   *  <li> 何もしない関数を必ず定義し、setNOP で与えなければならない
   *  <li> 各状態毎に, set(Entry|PreDo|Do|PostDo|Exit)Action でアクションを設定
   *  <li> 状態遷移時のアクションを setTransitionAction() で設定。
   * </ol>
   * <li> 遷移時のアクションは、与えられた現在状態、次状態、前状態を元に、
   *   ユーザが実装しなければならない。
   * <li> 状態の変更は goTo() で、状態のチェックは isIn(state) で行う。
   * <li> goTo()は次状態を強制的にセットする関数であり、遷移の可否は、
   *   ユーザが現在状態を取得し判断するロジックを実装しなければならない。
   * </ol>
   *
   * このクラスは、一つの状態に対して、
   * <ul>
   * <li> Entry action
   * <li> PreDo action
   * <li> Do action
   * <li> PostDo action
   * <li> Exit action
   * </ul>
   * 5つのアクションが定義することができる。
   * Transition action はあらゆる状態間遷移で呼び出されるアクションで、
   * その振る舞いはユーザが定義しなければならない。
   *
   * このクラスは以下のようなタイミングで各アクションが実行される。
   *
   * <ul>
   * <li> 状態が変更され(A->B)状態が遷移する場合 <br>
   * (A:Exit)->|(状態更新:A->B)->(B:Entry)->(B:PreDo)->(B:Do)->(B:PostDo)
   *
   * <li> 状態が変更されず、B状態を維持する場合 (|はステップの区切りを表す)<br>
   * (B(n-1):PostDo)->|(B(n):PreDo)->(B(n):Do)->(B(n):PostDo)->|(B(n+1):PreDo)<br>
   * PreDo, Do, PostDo が繰り返し実行される。
   *
   * <li> 自己遷移する場合 <br>
   * (B(n-1):PostDo)->(B(n-1):Exit)->|(B(n):Entry)->(B(n):PreDo) <br>
   * 一旦 Exit が呼ばれた後、Entry が実行され、以降は前項と同じ動作をする。
   * </ul>
   *
   * @param State 状態の型
   * @param Listener アクション用リスナーオブジェクト
   * @param States 状態ホルダー
   * @param Callback アクション用コールバック関数
   *
   * @since 0.4.0
   *
   * @else
   *
   * @class StateMachine
   *
   * @brief State machine class
   *
   * StateMachine class is a class to realize a state machine.
   *
   * Example: ActiveObject assumes to be an active object that has
   * the state machine.
   * There are three states such as INACTIVE, ACTIVE and ERROR state,
   * and if you want to define Entry or Exit action, this class will realize
   * as follows:
   * <pre>
   * class ActiveObject
   * {
   * public:
   *   enum MyState { INACTIVE, ACTIVE, ERROR };
   *   typedef States<MyState> MyStates;
   *
   *   ActiveObject()
   *     : m_sm(3)
   *   {
   *     m_sm.setNOP(&ActiveObject::nullAction);
   *     m_sm.setListener(this);
   *
   *     m_sm.setExitAction(INACTIVE, &ActiveObject::inactiveExit);
   *       :
   *     m_sm.setPostDoAction(ERROR, &ActiveObject::errorPostDo);
   *     m_sm.setTransitionAction(&ActiveObject:transition);
   *   };
   *
   *   bool nullAction(MyStates st) {};
   *   bool inactiveExit(MyStates st) {};
   *     :
   *   bool errorPostDo(MyStates st) {};
   *   bool transition(MyStates st) {};
   *
   * private:
   *   StateMachine<MyState, bool, ActiveObject> m_sm;
   * };
   * </pre>
   * If you want to give a class to some states, you must implement the class to
   * satisfy the following conditions:
   * <ol>
   * <li> You must define states by enum.
   * <li> Template arguments of StateMachine must be
   *   <type of state(MyState), listener object, state holder，callback function>
   * <li> Constructor arguments of StateMachine must be the number of the states.
   * <li> You must set the following action functions as a function of
   *      (Return _function_name_(States))
   * <ol>
   *  <li> You must define a function that does not do anything and give with
   *       setNOP.
   *  <li> You must set actions to each state
   *       by set(Entry|PreDo|Do|PostDo|Exit)Action.
   *  <li> You should set actions at the state transition by setTransitionAction().
   * </ol>
   * <li> You must implement action at the transition based on given states, such
   *  as current state, next state and previous state.
   * <li> You should change states by goTo() and check the state by isIn(state).
   * <li> goTo() is a function that sets next state forcibly, therefore,
   *      to determine the next state, you must get current state and
   *      implement that logic.
   * </ol>
   *
   * In this class, you can define the following five actions for one state:
   * <ul>
   * <li> Entry action
   * <li> PreDo action
   * <li> Do action
   * <li> PostDo action
   * <li> Exit action
   * </ul>
   * Transition action is an action invoked at the transition between any states,
   * and you must define its behavior.
   *
   * This class executes each action according to the following timing.
   *
   * <ul>
   * <li> If the state is changed and transits(A->B) state,<br>
   * (A:Exit)->|(state update:A->B)->(B:Entry)->(B:PreDo)->(B:Do)->(B:PostDo)
   *
   * <li> If the state is not changed and remains B state, (| shows a step's break)
   * (B(n-1):PostDo)->|(B(n):PreDo)->(B(n):Do)->(B(n):PostDo)->|(B(n+1):PreDo)
   * PreDo, Do and PostDo are executed over and over again.
   *
   * <li> If the state transits to itself<br>
   * (B(n-1):PostDo)->(B(n-1):Exit)->|(B(n):Entry)->(B(n):PreDo) <br>
   * Once Exit is invoked, Entry is executed, and then the same operation described
   * above will be done from here on.
   * </ul>
   *
   * @param State Type of the state
   * @param Listener Listener object for action
   * @param States State holder
   * @param Callback Callback function for action
   *
   * @since 0.4.0
   *
   * @endif
   */
  template <class State,
            class Listener,
            class States = StateHolder<State>,
            class Callback = void (Listener::*)(const States& states)
            >
  class StateMachine
  {
    typedef coil::Mutex Mutex;
    typedef coil::Guard<Mutex> Guard;
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param num_of_state ステートマシン中の状態数
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @param num_of_state Number of states in the state machine
     *
     * @endif
     */
    explicit StateMachine(int num_of_state)
      : m_num(num_of_state),
        m_listener(nullptr),
        m_entry(m_num, (Callback)NULL),
        m_predo(m_num, (Callback)NULL),
        m_do(m_num, (Callback)NULL),
        m_postdo(m_num, (Callback)NULL),
        m_exit(m_num, (Callback)NULL),
        m_transit(NULL),
        m_selftrans(false)
    {
    }

    virtual ~StateMachine()
    {
    }

    StateMachine(const StateMachine& other)
      : m_num(other.m_num),
        m_listener(other.m_listener),
        m_entry (other.m_entry),
        m_predo (other.m_predo),
        m_do    (other.m_do),
        m_postdo(other.m_postdo),
        m_exit  (other.m_exit),
        m_transit(other.m_transit),
        m_states(other.m_states),
        m_selftrans(other.m_selftrans)
    {
    }

    StateMachine& operator=(const StateMachine& other)
    {
      StateMachine temp(other);
      swap(temp);
      return *this;
    }

    void swap(StateMachine& other)
    {
      std::swap(m_num,       other.m_num);
      std::swap(m_listener,  other.m_listener);
      std::swap(m_entry,     other.m_entry);
      std::swap(m_predo,     other.m_predo);
      std::swap(m_do,        other.m_do);
      std::swap(m_postdo,    other.m_postdo);
      std::swap(m_exit,      other.m_exit);
      std::swap(m_transit,   other.m_transit);
      std::swap(m_states,    other.m_states);
      std::swap(m_selftrans, other.m_selftrans);
    }
    /*!
     * @if jp
     * @brief NOP関数を登録する
     *
     * NOP関数(何もしない関数)を登録する。
     *
     * @param call_back コールバック関数
     *
     * @else
     * @brief Set NOP function
     *
     * Set NOP function that does not do anything
     *
     * @param call_back Callback function
     *
     * @endif
     */
    void setNOP(Callback call_back)
    {
      setNullFunc(m_entry,  call_back);
      setNullFunc(m_do,     call_back);
      setNullFunc(m_exit,   call_back);
      setNullFunc(m_predo,  call_back);
      setNullFunc(m_postdo, call_back);
      m_transit = call_back;
    }

    /*!
     * @if jp
     * @brief Listener オブジェクトを登録する
     *
     * 各種アクション実行時に呼び出される Listener オブジェクトを登録する。
     *
     * @param listener Listener オブジェクト
     *
     * @else
     * @brief Set Listener Object
     *
     * Set Listener Object invoked when various actions are executed.
     *
     * @param listener Listener object
     *
     * @endif
     */
    void setListener(Listener* listener)
    {
      assert(listener != nullptr);
      m_listener = listener;
    }

    /*!
     * @if jp
     * @brief Entry action 関数を登録する
     *
     * 各状態に入った際に実行される Entry action 用コールバック関数を登録する。
     *
     * @param state 登録対象状態
     * @param call_back Entry action 用コールバック関数
     *
     * @return アクション実行結果
     *
     * @else
     * @brief Set Entry action function
     *
     * Set callback function for Entry action that is executed when entering in
     * each state.
     *
     * @param state Target state for the set
     * @param call_back Callback function for Entry action
     *
     * @return Action execution result
     *
     * @endif
     */
    bool setEntryAction(State state, Callback call_back)
    {
     try
        {
          m_entry.at(state) = call_back;
        }
      catch (...)
        {
          assert(false);
          return false;
        }
      return true;
    }

    /*!
     * @if jp
     * @brief PreDo action 関数を登録する
     *
     * 各状態内で実行される PreDo action 用コールバック関数を登録する。
     *
     * @param state 登録対象状態
     * @param call_back PreDo action 用コールバック関数
     *
     * @return アクション実行結果
     *
     * @else
     * @brief Set PreDo action function
     *
     * Set callback function for PreDo action that is executed in each state.
     *
     * @param state Target state for the set
     * @param call_back Callback function for PreDo action
     *
     * @return Action execution result
     *
     * @endif
     */
    bool setPreDoAction(State state, Callback call_back)
    {
      try
        {
          m_predo.at(state) = call_back;
        }
      catch (...)
        {
          assert(false);
          return false;
        }
      return true;
    }

    /*!
     * @if jp
     * @brief Do action 関数を登録する
     *
     * 各状態内で実行される Do action 用コールバック関数を登録する。
     *
     * @param state 登録対象状態
     * @param call_back Do action 用コールバック関数
     *
     * @return アクション実行結果
     *
     * @else
     * @brief Set Do action function
     *
     * Set callback function for Do action that is executed in each state.
     *
     * @param state Target state for the set
     * @param call_back Callback function for Do action
     *
     * @return Action execution result
     *
     * @endif
     */
    bool setDoAction(State state, Callback call_back)
    {
      try
        {
          m_do.at(state) = call_back;
        }
      catch (...)
        {
          assert(false);
          return false;
        }
      return true;
    }

    /*!
     * @if jp
     * @brief PostDo action 関数を登録する
     *
     * 各状態内で実行される PostDo action 用コールバック関数を登録する。
     *
     * @param state 登録対象状態
     * @param call_back PostDo action 用コールバック関数
     *
     * @return アクション実行結果
     *
     * @else
     * @brief Set PostDo action function
     *
     * Set callback function for PostDo action that is executed in each state.
     *
     * @param state Target state for the set
     * @param call_back Callback function for PostDo action
     *
     * @return Action execution result
     *
     * @endif
     */
    bool setPostDoAction(State state, Callback call_back)
    {
      try
        {
          m_postdo.at(state) = call_back;
        }
      catch (...)
        {
          assert(false);
          return false;
        }
      return true;
    }

    /*!
     * @if jp
     * @brief Exit action 関数を登録する
     *
     * 各状態内で実行される Exit action 用コールバック関数を登録する。
     *
     * @param state 登録対象状態
     * @param call_back Exit action 用コールバック関数
     *
     * @return アクション実行結果
     *
     * @else
     * @brief Set Exit action function
     *
     * Set callback function for Exit action that is executed in each state.
     *
     * @param state Target state for the set
     * @param call_back Callback function for Exit action
     *
     * @return Action execution result
     *
     * @endif
     */
    bool setExitAction(State state, Callback call_back)
    {
      try
        {
          m_exit.at(state) = call_back;
        }
      catch (...)
        {
          assert(false);
          return false;
        }
      return true;
    }

    /*!
     * @if jp
     * @brief State transition action 関数を登録する
     *
     * 状態遷移時に実行される State transition action 用コールバック関数を
     * 登録する。
     *
     * @param call_back State transition 用コールバック関数
     *
     * @return アクション実行結果
     *
     * @else
     * @brief Set state transition action function
     *
     * Set callback function for State transition action that is executed
     * when transiting to the state.
     *
     * @param call_back Callback function for State transition
     *
     * @return Action execution result
     *
     * @endif
     */
    bool setTransitionAction(Callback call_back)
    {
      m_transit = call_back;
      return true;
    }

    /*!
     * @if jp
     * @brief 初期状態をセットする
     *
     * ステートマシンの初期状態を設定する。
     *
     * @param states 初期状態
     *
     * @else
     * @brief Set the initial state
     *
     * Set the initial state of the state machine.
     *
     * @param states Initial state
     *
     * @endif
     */
    void setStartState(States states)
    {
      m_states.curr = states.curr;
      m_states.prev = states.prev;
      m_states.next = states.next;
    }

    /*!
     * @if jp
     * @brief 状態を取得する
     *
     * 状態情報を取得する。
     * 現在の状態、１つ前の状態、遷移予定の状態を取得することができる。
     *
     * @return 状態情報
     *
     * @else
     * @brief Get states
     *
     * Get state information.
     * Get the current state, the previous state and the next state to
     * be expected to transfer.
     *
     * @return State information
     *
     * @endif
     */
    States getStates()
    {
      Guard guard(m_mutex);
      return m_states;
    }

    /*!
     * @if jp
     * @brief 現在の状態を取得する
     *
     * 現在の状態を取得する。
     *
     * @return 現在の状態
     *
     * @else
     * @brief Get current state
     *
     * Get current state.
     *
     * @return Current state
     *
     * @endif
     */
    State getState()
    {
      Guard guard(m_mutex);
      return m_states.curr;
    }

    /*!
     * @if jp
     * @brief 現在状態を確認
     *
     * 現在の状態が、引数で指定した状態と一致するか確認する。
     *
     * @param state 確認対象状態
     *
     * @return 状態確認結果
     *
     * @else
     * @brief Check current state
     *
     * Check whether current state matches the state specified by argument.
     *
     * @param state Target state for the check
     *
     * @return Check state result
     *
     * @endif
     */
    bool isIn(State state)
    {
      Guard guard(m_mutex);
      return m_states.curr == state ? true : false;
    }

    /*!
     * @if jp
     * @brief 状態を遷移
     *
     * 指定した状態に状態を遷移する。
     * 本関数は次状態を強制的にセットする関数である。
     * このため、遷移の可否は、ユーザが現在状態を取得し判断するロジックを
     * 実装しなければならない。
     * 遷移先が現在の状態と同じ場合には、自己遷移フラグをセットする。
     *
     * @param state 遷移先状態
     *
     * @else
     * @brief Transit State
     *
     * Transit to the specified state.
     * This function sets the next state forcibly.
     * Therefore, to determine the next state, users must get current state
     * and implement that logic.
     * If transit destination is the same as the current state, flag of
     * self-transition will be set.
     *
     * @param state State of the transition destination
     *
     * @endif
     */
    void goTo(State state)
    {
      Guard guard(m_mutex);
      m_states.next = state;
      if (m_states.curr == state)
        {
          m_selftrans  = true;
        }
    }


    /*!
     * @if jp
     * @brief 駆動関数
     *
     * ステートマシンの駆動関数。
     * 実際の状態遷移および状態遷移発生時の各アクションの呼びだしを実行する。
     *
     * @else
     * @brief Worker function
     *
     * This is a worker function of the state machine.
     * Execute the invocation of each action at actual state transition and the
     * state transition occurrence.
     *
     * @endif
     */
    void worker()
    {
      States state;

      sync(state);

      if (state.curr == state.next)
        {
          // pre-do
          if (m_predo[state.curr] != NULL)
            (m_listener->*m_predo[state.curr])(state);

          if (need_trans()) return;

          // do
          if (m_do[state.curr] != NULL)
            (m_listener->*m_do[state.curr])(state);

          if (need_trans()) return;

          // post-do
          if (m_postdo[state.curr] != NULL)
            (m_listener->*m_postdo[state.curr])(state);
        }
      else
        {
          if (m_exit[state.curr] != NULL)
            (m_listener->*m_exit[state.curr])(state);

          sync(state);

          if (state.curr != state.next)
            {
              state.curr = state.next;
              if (m_entry[state.curr] != NULL)
                (m_listener->*m_entry[state.curr])(state);
              update_curr(state.curr);
            }
        }
    }

    //============================================================
    // divided worker functions
    // The following divided worker functions have to be used together.
    // - worker_pre()
    // - worker_do()
    // - worker_post()
    //
    void worker_pre()
    {
      States state;
      sync(state);
      if (state.curr == state.next)
        {
          if (m_predo[state.curr] != nullptr)
            {
              (m_listener->*m_predo[state.curr])(state);
            }
          return;
        }

      // State changed
      if (m_exit[state.curr] != nullptr)
        {
          (m_listener->*m_exit[state.curr])(state);
        }
      sync(state);
      if (state.curr != state.next)
        {
          state.curr = state.next;
          if (m_entry[state.curr] != nullptr)
            {
              (m_listener->*m_entry[state.curr])(state);
            }
          update_curr(state.curr);
        }
    }

    void worker_do()
    {
      States state;
      sync(state);
      if (m_do[state.curr] != nullptr)
        {
          (m_listener->*m_do[state.curr])(state);
        }
    }

    void worker_post()
    {
      States state;
      sync(state);
      if (m_postdo[state.curr] != nullptr)
        {
          (m_listener->*m_postdo[state.curr])(state);
        }
    }

  protected:
    /*!
     * @if jp
     * @brief NOP関数を設定
     *
     * NOP関数(何もしない関数)を登録する。
     *
     * @param s コールバック関数設定先
     * @param nullfunc コールバック関数(NOP関数)
     *
     * @else
     * @brief Set NOP function
     *
     * Set NOP function (function to do nothing).
     *
     * @param s Callback function for setting
     * @param nullfunc Callback function (NOP function)
     *
     * @endif
     */
    void setNullFunc(std::vector<Callback>& s, Callback nullfunc)
    {
      s.clear();
      //      assert((size_t)m_num == s.size());
      for (size_t i(0); i < m_num; ++i)
        {
          s.push_back(nullfunc);
        }
    }

    /*!
     * @if jp
     * @brief 状態数
     * @else
     * @brief Number of state
     * @endif
     */
    int m_num;

    /*!
     * @if jp
     * @brief コールバック関数用リスナー
     * @else
     * @brief Callback function for listener
     * @endif
     */
    Listener* m_listener;

    /*!
     * @if jp
     * @brief Entry action 用コールバック関数
     * @else
     * @brief Callback function for Entry action
     * @endif
     */
    std::vector<Callback> m_entry;

    /*!
     * @if jp
     * @brief PreDo action 用コールバック関数
     * @else
     * @brief Callback function for PreDo action
     * @endif
     */
    std::vector<Callback> m_predo;

    /*!
     * @if jp
     * @brief Do action 用コールバック関数
     * @else
     * @brief Callback function for Do action
     * @endif
     */
    std::vector<Callback> m_do;

    /*!
     * @if jp
     * @brief PostDo action 用コールバック関数
     * @else
     * @brief Callback function for PostDo action
     * @endif
     */
    std::vector<Callback> m_postdo;

    /*!
     * @if jp
     * @brief Exit action 用コールバック関数
     * @else
     * @brief Callback function for Exit action
     * @endif
     */
    std::vector<Callback> m_exit;

    /*!
     * @if jp
     * @brief State transition action 用コールバック関数
     * @else
     * @brief Callback function for State transition action
     * @endif
     */
    Callback  m_transit;

    /*!
     * @if jp
     * @brief 現在の状態情報
     * @else
     * @brief Current state information
     * @endif
     */
    States m_states;
    bool m_selftrans;
    Mutex m_mutex;

  private:
    inline void sync(States& st)
    {
      Guard guard(m_mutex);
      st = m_states;
    }

    inline bool need_trans()
    {
      Guard guard(m_mutex);
      return (m_states.curr != m_states.next);
    }

    inline void update_curr(const State curr)
    {
      Guard guard(m_mutex);
      m_states.curr = curr;
    }
  };
} // namespace RTC_Utils

#endif  // RTC_STATEMACHINE_H
