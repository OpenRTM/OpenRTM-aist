/*
 * Copyright 2005 by Eduard Hiti.
*/
#ifndef HRTM_STATECHART_H_
#define HRTM_STATECHART_H_

#define SC_TRACE

#include <cassert>
#include <iostream>  // NOLINT
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include "hrtm/defs.h"
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4512 4251)
#elif defined(__GNUC__) && (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
////////////////////////////////////////////////////////////////////////////////
// Various macros for state and history declaration

// Use this macro to define your top state class.
#define TOPSTATE(TOP) \
  struct TOP : public hrtm::sc::Link< TOP, hrtm::sc::TopState< TOP > >

// Use this macro for all your other state classes.
#define SUBSTATE(STATE, SUPERSTATE) \
  struct STATE : public hrtm::sc::Link< STATE, SUPERSTATE >

// Use this macro in your class definition to give it state functionality
// (mandatory). If you have a state data declare it BEFORE macro invocation!
#define STATE(S) \
  public: \
  /* Constructor and destructor already defined: */ \
  /* you can't (and shouldn't) have your own! */ \
  /* For the user a state class "constructor" and */ \
  /* "destructor" are its entry and exit method! */ \
  S(hrtm::sc::StateInfo & info) : hrtm::sc::Link<S, SUPER>(info) { \
          /* Compile time check: S must derive directly from Link<S, SUPER> */ \
          hrtm::sc::Link<S, SUPER> * p = 0; \
          LinkType * p2 = 0; \
          p = p2; \
          p2 = p; \
  } \
  ~S() {} \
  static const char * state_name() { return #S; } \
  /* Get to your Data with this method: */ \
  Data & data() { return *static_cast<Data *>(local_data()); } \

// Use this macro to select deep history strategy.
#define DEEPHISTORY() \
  private: \
  /* If no superstate has history, SUPER::set_history_super is a NOOP */ \
  void save_history(hrtm::sc::StateInfo & self, \
      hrtm::sc::StateInfo & /* shallow */, hrtm::sc::StateInfo & deep) { \
    self.set_history(&deep); \
    SUPER::set_history_super(self, deep); } \
  protected: \
  /* Substates may use set_history_super to bubble up history */ \
  void set_history_super(\
      hrtm::sc::StateInfo & self, hrtm::sc::StateInfo & deep) \
  { self.set_history_super(deep); } \
 public:
// Use this macro to select shallow history strategy.
#define HISTORY() \
  private: \
  /* If no superstate has history, SUPER::set_history_super is a NOOP */ \
  void save_history(hrtm::sc::StateInfo & self, hrtm::sc::StateInfo & shallow, \
      hrtm::sc::StateInfo & deep) { \
    self.set_history(&shallow); \
    SUPER::set_history_super(self, deep); } \
  protected: \
  /* Substates may use set_history_super to bubble up history */ \
  void set_history_super(\
      hrtm::sc::StateInfo & self, hrtm::sc::StateInfo & deep) \
  { self.set_history_super(deep); } \
 public:
// Use this macro to have dataes survive state transitions
#define PERSISTENT() \
  private:  \
  void delete_data(hrtm::sc::StateInfo & info) {} \
 public:
namespace hrtm {
namespace sc {

using Key = unsigned int;

class MachineBase;

class EventBase;

template<class T>
class Machine;

template<class T>
class EventParamBase;

class StateInfo;

template<class T>
class TopState;

template<class S>
class State;

////////////////////////////////////////////////////////////////////////////////
// Data for states which don't declare own Data class.
class EmptyData {
 public:
  EmptyData() = default;
};

////////////////////////////////////////////////////////////////////////////////
// Each state machine has its own key generator. The generator returns unique
// identifiers. The identifiers are consecutive integers starting from zero,
// which allows use as index into a vector for fast access.
// Root always has zero as key.
template<class TOP>
struct KeyInit {
  KeyInit() : key(Machine<TOP>::the_state_count_++) {}
  Key key;
};

////////////////////////////////////////////////////////////////////////////////
// Base class for all state classes.
// Also serves as 'Root' state. By entering this state we trigger entry
// and exit actions of user's top state.
class EXPORT_DLL StateBase {
 public:
  virtual ~StateBase() = default;

  // Get unique name of state.
  static Key key() {
    return 0;
  }

 protected:
  StateBase(StateInfo & info)  // NOLINT
     : state_info_(info) {}

  // Restore from snapshot event: set current state.
  // Default implementation: Does not trigger entry actions!
  virtual void restore(StateInfo & current);
  // Shutdown machine event: exit all states.
  // Default implementation: Triggers exit actions!
  // Override empty to omit calling exit actions.
  virtual void shutdown();
  // This is the method to bubble up history information
  // for states whose superstates have no history (so does nothing).
  virtual void set_history_super(
      StateInfo & /* self */, StateInfo & /* deep */) {}

 protected:
  // State entry. Not allowed to initiate state change.
  virtual void on_entry() {}
  // Special kind of state entry: Upon transition to a new state,
  // entry methods of that state and its superstates are called;
  // 'init' however is called only on the one state the transition
  // actually goes to.
  // Is allowed to change state.
  virtual void on_init() {}
  // State exit. Not allowed to initiate state change.
  virtual void on_exit() {}

 protected:
  // C++ needs something like package visibility
  friend class StateInfo;
  friend class RootStateInfo;
  friend class MachineBase;

  // Create StateInfo object of state.
  static StateInfo & get_info(MachineBase & machine);
  virtual void delete_data(StateInfo & /* info */) {}
  // Default history strategy (no history).
  virtual void save_history(StateInfo & /* self */, StateInfo & /* shallow */,
      StateInfo & /* deep */) {}

 protected:
  StateInfo & state_info_;
};


////////////////////////////////////////////////////////////////////////////////
// StateInfo describes a state. Keeps history, data and state object for state.
// StateInfo object is created the first time state is entered.
// There is at most one StateInfo object per state per machine instance.
class EXPORT_DLL StateInfo {
public:
    StateInfo(MachineBase & machine, StateInfo * parent);
    virtual ~StateInfo();
    // Perform entry actions.
    // 'first' is true on very first call.
    void on_entry(StateInfo & previous, bool first = true);
    // Perform exit actions.
    void on_exit(StateInfo & next);
    // Perform init action.
    void on_init(bool history);
    void save_history(StateInfo & shallow, StateInfo & deep) {
        // Check state's history strategy.
        instance_->save_history(*this, shallow, deep);
    }
    // Update superstate's history information:
    void set_history_super(StateInfo & deep) {
        if (parent_ != nullptr) {
            // Let it choose between deep or shallow history.
            parent_->save_history(*this, deep);
        }
    }
    // Data has been created explicitly.
    void set_data(void * data) {
        assert(!data_);

        if (data_place != nullptr) {
            // Free cached memory of previously used data.
            ::operator delete(data_place);
            data_place = nullptr;
        }
        data_ = data;
    }
    // Copy state of another StateInfo object.
    void copy(StateInfo & original);
    // Create a clone of StateInfo object for another machine.
    StateInfo * clone(MachineBase & new_machine);
    virtual void clone_data(void * data) = 0;
    void shutdown() {
        instance_->shutdown();
    }
    void restore(StateInfo & info) {
        instance_->restore(info);
    }
    virtual Key key() = 0;
    // 'Virtual constructor' needed for cloning.
    virtual StateInfo * create(MachineBase & machine, StateInfo * parent) = 0;
    virtual void create_data() = 0;
    virtual void delete_data() = 0;
    virtual const char * name() const = 0;
    // Is 'state' a superstate?
    bool is_child(StateInfo & state) const {
        return this == &state || ((parent_ != nullptr) && parent_->is_child(state));
    }
    StateBase & instance() {
        assert(instance_);
        return *instance_;
    }
    void * data() {
        assert(data_);
        return data_;
    }
    MachineBase & machine() {
        return machine_;
    }
    void set_history(StateInfo * history) {
        history_ = history;
    }

protected:
    MachineBase & machine_;
    StateBase * instance_;   // Instance of state class
    StateInfo * history_;
    StateInfo * parent_;
    void * data_;
    void * data_place;      // Reused data memory
};


////////////////////////////////////////////////////////////////////////////////
// Base class for user defined top state (and indirectly all other states).
template<class T>
class TopState : public StateBase {
 public:
  // This typedef is an alias for user defined top state in all (sub)states.
  using TOP = T;

 protected:
  TopState(StateInfo & info) : StateBase(info) {}  // NOLINT

  void dispatch(EventParamBase<TOP> * event);

  void defer(EventParamBase<TOP> * event, const std::string& name="event");

  // Returns current state machine instance.
  Machine<TOP> & machine() {
    return *static_cast<Machine<TOP> *>(&this->state_info_.machine());
  }
};

////////////////////////////////////////////////////////////////////////////////
// Helper classes to allow parametrization of state transitions with 'set_state'
// (also for starting machines).

// Members of do-it-yourself virtual function table.
using GetInfoFn = StateInfo &(*)(MachineBase &);
using DestroyFn = void (*)(void*);
using CloneFn = void* (*)(void*);

// DIY virtual function table.
struct StateCharacter {
  GetInfoFn get_info;
  DestroyFn destroy;
  CloneFn clone;
};

// This is the base class for state aliases. A state alias represents a
// state of a machine. A transition to that state can be initiated by
// giving the alias object to the 'set_state' method.
//
// State aliases are created by the template class 'State' further below.
//
// Objects of this types are copyable; however, on copying the copy
// actually takes over the data instance of the original object.
// This also happens when the alias object is used for a state transition:
// the alias' data instance is used up by the reached state; the next time
// the same alias is used for a state transition no data is provided
// anymore!
//
// To keep the dataes of alias objects you need to clone the object with the
// 'clone' method: for this to work however dataes need to be copyable (have
// a copy constructor).
// Define MACRO_DATA_COPY when compiling if this is the case!
//
// This class has no virtual functions but something similar in form of
// pointers to static functions (static functions suffice since the
// operations depend only on template parameters).      This crazy stuff is
// done here because this way allocating memory on the heap when cloning
// state aliases can be avoided, which we would have to do with ordinary
// virtual methods. SPEED!!!
class StateAlias {
 public:
  // Takes over data from other object!
  // If you do not want this use 'clone'!
  StateAlias(const StateAlias & other)
    : data_(other.take_data())
    , character_(other.character_) {}

  // Takes over data from other object!
  // If you do not want this use 'clone'!
  StateAlias & operator=(const StateAlias & other) {
    (character_->destroy)(data_);
    character_ = other.character_;
    data_ = other.take_data();
    return *this;
  }
  ~StateAlias() {
    (character_->destroy)(data_);
  }
  // Clones object.
  // Will call copy constructor of data.
  StateAlias clone() {
    return StateAlias(character_, data_ != nullptr ? (character_->clone)(data_) : nullptr);
  }
  StateInfo & get_info(MachineBase & machine) const {
    return (character_->get_info)(machine);
  }
  // Hand over data object (to state instances or other state aliases).
  void * take_data() const {
    void * data = data_;
    data_ = nullptr;
    return data;
  }

 protected:
  // Protected constructor: User creates alias with the 'State' class below.
  StateAlias(StateCharacter * character, void * data)
    : data_(data)
    , character_(character) {}

 private:
  mutable void * data_;
  // DIY virtual function table.
  StateCharacter * character_;
};

////////////////////////////////////////////////////////////////////////////////
// Base class for Machine objects.
class EXPORT_DLL MachineBase {
 public:
  // Transition to new state (with optional preinitialized data).
  void set_state(StateInfo & info, bool history, void * data);
  // Transition to new state specified by state alias.
  void set_state(const StateAlias & state, bool history);
  // Prepare transition to new state (performed on call to "perform_pending").
  // There can be only one state transition pending (asserts otherwise)!
  // "data" is an optional preinitialized state data for the new state.
  void set_pending_state(StateInfo & info, bool history, void * data) {
    assert((!pending_state_ || pending_state_ == &info) &&
        "There is already a state transition pending!");

    pending_state_ = &info;
    pending_data_ = data;
    pending_history_ = history;
  }
  void set_pending_event(EventBase * event) {
    assert(event);
    assert(!pending_event_ && "There is already an event pending!");

    pending_event_ = event;
  }
  void add_deferred_event(EventBase * event, const std::string& name);
  // Performs pending state transition.
  void perform_pending();
  // Performs pending state transition.
  void perform_pending_event();
  // Performs deferred events.
  void perform_deferred_events();
  // Get StateInfo object for key.
  StateInfo * & get_info(Key name) {
    return states_[name];
  }
  bool is_current(StateInfo & info) {
    return current_state_->is_child(info);
  }
  bool is_current_direct(StateInfo & info) const {
    return current_state_ == &info;
  }

 protected:
  MachineBase();
  ~MachineBase();
  // Starts the machine. Will make it go into top state.
  // Optional parameter "data" is a preinitialized state data for the top
  // state.
  void start(StateInfo & info, void * data);
  // Shuts machine down. Will exit any states and free all allocated
  // resources.
  void shutdown();
  // Allocate space for pointers to StateInfo objects.
  void allocate(unsigned int count);
  // Free all StateInfo objects.
  void free(unsigned int count);
  // Create a copy of another machines StateInfo objects (includes dataes).
  void copy(const std::vector<StateInfo*>& other, unsigned int count);
  // Create a copy of another machines StateInfo object.
  StateInfo * create_clone(Key key, StateInfo * original);

 protected:
  using EventQueue = std::map<std::string, EventBase*>;
  using EventNames = std::vector<std::string>;

  // C++ needs something like package visibility
  // for set_pending_state
  friend class StateInfo;
  friend class StateBase;

  // Current state of Machine object.
  StateInfo * current_state_{nullptr};
  // Information about pending state transition.
  StateInfo * pending_state_{nullptr};
  void * pending_data_{nullptr};
  bool pending_history_{false};
  EventBase * pending_event_{nullptr};
  EventQueue deferred_events_;
  EventNames deferred_names_;
  // Array of StateInfo objects.
  std::vector<StateInfo*> states_;
};

////////////////////////////////////////////////////////////////////////////////
// This class links substates to superstates by deriving from the superstate
// and being derived from by the substate.
// Substates inherit event handlers from superstates for reuse or redefinition
// this way.
template<class C, class P>
class Link : public P {
 public:
  // Alias for superstate.
  using SUPER = P;
  // Alias for topstate.
  using TOP = typename P::TOP;
  // Default data type.
  using Data = EmptyData;

  // Get unique key of state.
  static Key key() {
    return the_key_.key;
  }
  // Is machine m in this state?
  static bool is_current(Machine<TOP> & m) {
    return m.is_current(get_info(m));
  }
  // Is machine m in exactly this state?
  static bool is_current_direct(Machine<TOP> & m) {
    return m.is_current_direct(get_info(m));
  }
  static void clear_history(Machine<TOP> & m) { get_info(m).set_history(0); }
  static void clear_history_deep(Machine<TOP> & m) {
    m.clear_history_deep(get_info(m)); }
  static void set_state(Machine<TOP> & machine, void * data = nullptr) {
    StateInfo & info = get_info(machine);
    machine.set_pending_state(info, true, data);
  }
  static void set_state_direct(Machine<TOP> & machine, void * data = nullptr) {
    StateInfo & info = get_info(machine);
    machine.set_pending_state(info, false, data);
  }
  // Initiate transition to a new state.
  // Parameter state is the new state to enter.
  // See above and class State for more information.
  void set_state(const StateAlias & state) {
    StateInfo & info = state.get_info(P::machine());
    P::machine().set_pending_state(info, true, state.take_data());
  }
  void set_state_direct(const StateAlias & state) {
    StateInfo & info = state.get_info(P::machine());
    P::machine().set_pending_state(info, true, state.take_data());
  }
  // to be used with restore
  void set_state(StateInfo & current, void * data = nullptr) {
    this->state_info_.machine().set_pending_state(current, true, data);
  }

 protected:
  // Needed to perform compile time checks.
  using LinkType = Link<C, P>;

  Link(StateInfo & info);  // NOLINT
  // These definitions seem redundant but they are not!
  // They override parent definitions so each substate gets either
  // this default or their own, but never its parents definitions.
  virtual void on_entry() {}
  virtual void on_init() {}
  virtual void on_exit() {}
  // Create StateInfo object of state.
  static StateInfo & get_info(MachineBase & machine);
  // Data is by default not persistent. Not redundant!
  void delete_data(StateInfo & info);
  // Default history strategy (no history). Not redundant!
  void save_history(
      StateInfo & self, StateInfo & /* shallow */, StateInfo & deep) {
    // Bubble up history. If no superstate has history,
    // set_history_super will do nothing.
    this->set_history_super(self, deep);
  }
  // This method keeps 'state_info_' attribute private.
  void * local_data();

 protected:
  // for get_info
  friend class StateBase;
  // for get_info
  friend class Machine<TOP>;
  friend class State<C>;

 protected:
  StateInfo & state_info_;
  static KeyInit<TOP> the_key_;
};

template<class C, class P> KeyInit<typename P::TOP> Link<C, P>::the_key_;


////////////////////////////////////////////////////////////////////////////////
// StateInfo for Root state.
class RootStateInfo : public StateInfo {
 public:
  RootStateInfo(MachineBase & machine, StateInfo * parent)
    : StateInfo(machine, parent) {
    instance_ = new StateBase(*this);
  }
  Key key() override {
    return 0;
  }

  void create_data() override {}
  void delete_data() override {}
  void clone_data(void * /* data */) override {}
  const char * name() const override { return "Root"; }
  // 'Virtual constructor' needed for cloning.
  StateInfo * create(MachineBase & machine, StateInfo * parent) override {
    return new RootStateInfo(machine, parent);
  }
};

////////////////////////////////////////////////////////////////////////////////
// StateInfo for substates (including Top ;-)
// Has methods to create state specific objects.
template<class S>
class SubStateInfo : public StateInfo {
 public:
  using Data = typename S::Data;

  SubStateInfo(MachineBase & machine, StateInfo * parent)
    : StateInfo(machine, parent) {
    assert(parent);
    this->instance_ = new S(*this);
  }
  ~SubStateInfo() override {
    if (this->data_)
      delete_data();
  }
  const char * name() const override { return S::state_name(); }
  Key key() override {
    return S::key();
  }
  // 'Virtual constructor' needed for cloning.
  StateInfo * create(MachineBase & machine, StateInfo * parent) override {
    return new SubStateInfo<S>(machine, parent);
  }
  void clone_data(void * data) override {
    assert(!data_);
    assert(!data_place);
    // Needs copy constructor in ALL data types.
    data_ = new Data(*static_cast<Data *>(data));
  }
  void create_data() override {
    this->data_ = new Data();
  }
  void delete_data() override {
    delete static_cast<Data *>(this->data_);
    data_ = 0;
  }
};

////////////////////////////////////////////////////////////////////////////////
// Definitions for queuable event types

// Generic interface for event objects (available only to MachineBase)
class EventBase {
 public:
  virtual ~EventBase() = default;
  virtual void dispatch(StateInfo &) = 0;
};

// Interface for event objects (bound to a top state)
template<class TOP>
class EventParamBase : protected EventBase {
  friend class Machine<TOP>;
  friend class TopState<TOP>;
};

// Event with four parameters
template<class TOP, class ROOT, class P1, class P2, class P3, class P4>
class EventWith4Params : public EventParamBase<TOP> {
  using Signature = ROOT (TOP::*)(P1, P2, P3, P4);

 public:
  EventWith4Params(Signature handler, P1 p1, P2 p2, P3 p3, P4 p4)
    : handler_(handler), param1_(p1), param2_(p2),
    param3_(p3) , param4_(p4) {
    assert(handler);
  }

 protected:
  void dispatch(StateInfo & info) {
    TOP & state = static_cast<TOP &>(info.instance());
    (state.*handler_)(param1_, param2_, param3_, param4_);
  }

  Signature handler_;
  P1 param1_;
  P2 param2_;
  P3 param3_;
  P4 param4_;
};

// Event with three parameters
template<class TOP, class ROOT, class P1, class P2, class P3>
class EventWith3Params : public EventParamBase<TOP> {
  using Signature = ROOT (TOP::*)(P1, P2, P3);

 public:
  EventWith3Params(Signature handler, P1 p1, P2 p2, P3 p3)
    : handler_(handler), param1_(p1), param2_(p2), param3_(p3) {
    assert(handler);
  }

 protected:
  void dispatch(StateInfo & info) {
    TOP & state = static_cast<TOP &>(info.instance());
    (state.*handler_)(param1_, param2_, param3_);
  }

  Signature handler_;
  P1 param1_;
  P2 param2_;
  P3 param3_;
};

// Event with two parameters
template<class TOP, class ROOT, class P1, class P2>
class EventWith2Params : public EventParamBase<TOP> {
  using Signature = ROOT (TOP::*)(P1, P2);

 public:
  EventWith2Params(ROOT (TOP::*handler)(P1, P2), P1 p1, P2 p2)
    : handler_(handler), param1_(p1), param2_(p2) {
    assert(handler);
  }

 protected:
  void dispatch(StateInfo & info) {
    TOP & state = static_cast<TOP &>(info.instance());
    (state.*handler_)(param1_, param2_);
  }

  Signature handler_;
  P1 param1_;
  P2 param2_;
};

// Event with one parameter
template<class TOP, class ROOT, class P1>
class EventWith1Params : public EventParamBase<TOP> {
  using Signature = ROOT (TOP::*)(P1);

 public:
  EventWith1Params(ROOT (TOP::*handler)(P1), P1 p1)
    : handler_(handler), param1_(p1) {
    assert(handler);
  }

 protected:
  void dispatch(StateInfo & info) {
    TOP & state = static_cast<TOP &>(info.instance());
    (state.*handler_)(param1_);
  }

  Signature handler_;
  P1 param1_;
};

// Event with no parameters
template<class TOP, class ROOT>
class EventWithoutParams : public EventParamBase<TOP> {
  using Signature = ROOT (TOP::*)();

 public:
  EventWithoutParams(ROOT (TOP::*handler)())
    : handler_(handler) {
    assert(handler);
  }

 protected:
  void dispatch(StateInfo & info) {
    TOP & state = static_cast<TOP &>(info.instance());
    (state.*handler_)();
  }

  Signature handler_;
};

// Event creating functions using type inference
template<class TOP, class ROOT, class P1, class P2, class P3, class P4>
inline EventParamBase<TOP> * Event(
    ROOT (TOP::*handler)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) {
  return new EventWith4Params<TOP, ROOT, P1, P2, P3, P4>(
      handler, p1, p2, p3, p4);
}

template<class TOP, class ROOT, class P1, class P2, class P3>
inline EventParamBase<TOP> * Event(ROOT(TOP::*handler)(
      P1, P2, P3), P1 p1, P2 p2, P3 p3) {
  return new EventWith3Params<TOP, ROOT, P1, P2, P3>(handler, p1, p2, p3);
}

template<class TOP, class ROOT, class P1, class P2>
inline EventParamBase<TOP> * Event(ROOT (TOP::*handler)(P1, P2), P1 p1, P2 p2) {
  return new EventWith2Params<TOP, ROOT, P1, P2>(handler, p1, p2);
}

template<class TOP, class ROOT, class P1>
inline EventParamBase<TOP> * Event(ROOT (TOP::*handler)(P1), P1 p1) {
  return new EventWith1Params<TOP, ROOT, P1>(handler, p1);
}

template<class TOP, class ROOT>
inline EventParamBase<TOP> * Event(ROOT (TOP::*handler)()) {
  return new EventWithoutParams<TOP, ROOT>(handler);
}


////////////////////////////////////////////////////////////////////////////////
// Implementation for TopState
template<class T>
inline void TopState<T>::dispatch(EventParamBase<TOP> * event) {
  assert(event);
  state_info_.machine().set_pending_event(event);
}

template<class T>
inline void TopState<T>::defer(EventParamBase<TOP> * event,
    const std::string& name) {
  assert(event);
  state_info_.machine().add_deferred_event(event, name);
}

////////////////////////////////////////////////////////////////////////////////
// Implementation for Link
template<class C, class P>
inline Link<C, P>::Link(StateInfo & info)
: P(P::get_info(info.machine()))
  // Can't initialize state_info_ with get_info,
  // because this would result in an endless loop (at least for first call)
  , state_info_(info) {}

// Create StateInfo object of state.
template<class C, class P>
inline StateInfo & Link<C, P>::get_info(MachineBase & machine) {
  // Look first in machine for existing StateInfo.
  StateInfo * & info = machine.get_info(key());
  if (info == nullptr) {
    // Will create parent StateInfo object if not already created.
    info = new SubStateInfo<C>(machine, &P::get_info(machine));
  }
  return *info;
}

// This method keeps 'state_info_' attribute private.
template<class C, class P>
inline void * Link<C, P>::local_data() {
  return state_info_.data();
}

// Default behaviour: free data on exit.
template<class C, class P>
inline void Link<C, P>::delete_data(StateInfo & info) {
  info.delete_data();
}


// Objects of this class represent states in user code.  They are used to
// create aliases to be used as parameters to the set_state method to
// initiate transitions to the specified state.
// If the object is created with a data instance for the state, that data
// instance is used to initialize the state on transition:
//
// Example:
//              StateAlias a = State<StateA>(new StateA::Data(42));
//              ...
//              set_state(a);
template<class S>
class State : public StateAlias {
 public:
  // Can be given a data to be used when transitioning to template
  // parameter state.
  explicit State(typename S::Data * data = 0)
    : StateAlias(&the_character_, data) {}

 private:
  static StateInfo & get_info(MachineBase & machine) {
    return S::get_info(machine);
  }
  static void destroy(void * data) {
    delete static_cast<typename S::Data *>(data);
  }
  static void * clone(void * data) {
    using Data = typename S::Data;
    return new Data(* static_cast<Data *>(data));
  }

 private:
  static StateCharacter the_character_;
};

////////////////////////////////////////////////////////////////////////////////
// Snapshot of a machine object.
// Saves the state of a machine object at a specific point in time to be
// restored later (can be used to achieve something like backtracking).
// Includes dataes of current state and persistent dataes.
// Assign a snapshot to a machine (operator=) to restore state.
// Note that no exit/entry actions of the overwritten machine state are
// performed! Data destructors however are executed!
template<class TOP>
class Snapshot : public MachineBase {
 public:
  Snapshot(const Machine<TOP> & machine);  // NOLINT
  ~Snapshot() {
    free(Machine<TOP>::the_state_count_);
  }

 private:
  friend class Machine<TOP>;

  Snapshot(const Snapshot<TOP> & other) = delete;
  Snapshot & operator=(const Snapshot<TOP> & other) = delete;
};

////////////////////////////////////////////////////////////////////////////////
// A Machine object maintains a current state.
// The state can be any substate of template parameter TOP.
// TOP is the Machine's top most and inital state. TOP must be defined by
// the macro TOPSTATE. Event processing is done by calling methods (event
// handlers) on the current state.
// This is realized by defining an arrow ("->") operator on Machine,
// forwarding to the interface of TOP.
// Every possible event handler to be called must therefore appear in the
// interface of TOP. Events are dispatched by using this operator on a
// Machine object (e.g. "machine->event()").
template<class TOP>
class Machine : public MachineBase {
 public:
  //////////////////////////////////////////////////////////////////////////////
  // This class performs an action in its destructor after an event
  // handler has finished. Comparable to an After Advice in AOP.
  struct AfterAdvice {
    AfterAdvice(Machine<TOP> & m) : machine_(m) {}  // NOLINT

    // Event handler has finished execution. Execute pending transitions now.
    ~AfterAdvice() { machine_.perform_pending(); }

    // this arrow operator finally dispatches to TOP interface.
    TOP * operator->() {
      return static_cast<TOP *>(& (machine_.current_state_->instance()) );
    }

   private:
     Machine<TOP> & machine_;
  };
  friend struct AfterAdvice;

  // Constructor takes an optional preinitialized Data parameter for its
  // initial (top) state.
  Machine(typename TOP::Data * data = 0) {  // NOLINT
    // Compile time check: TOP must directly derive from TopState<TOP>
    TopState<TOP> * p = 0;
    typename TOP::SUPER * p2 = 0;
    p2 = p;
    allocate(the_state_count_);
    start(TOP::get_info(*this), data);
  }
  // Initialize with a state alias object to have machine go to a state
  // other than TOP on startup.
  Machine(const StateAlias & state) {  // NOLINT
    // Compile time check: TOP must directly derive from TopState<TOP>
    TopState<TOP> * p = 0;
    typename TOP::SUPER * p2 = 0;
    p2 = p;
    allocate(the_state_count_);
    start(state.get_info(*this), state.take_data());
  }
  // Create machine from a snapshot.
  Machine(const Snapshot<TOP> & snapshot) {  // NOLINT
    allocate(the_state_count_);
    copy(snapshot.states_, the_state_count_);
  }
  // Overwrite current machine state by snapshot.
  Machine<TOP> & operator=(const Snapshot<TOP> & snapshot) {
    assert(!pending_state_);
    assert(!pending_event_);

    current_state_->shutdown();
    free(the_state_count_);
    copy(snapshot.states_, the_state_count_);
    // Go to Root state first
    current_state_ = get_info(0);
    // Then set previous current state
    StateInfo * current = get_info(snapshot.current_state_->key());
    current->restore(*current);
    perform_pending();
    return *this;
  }
  ~Machine() {
    current_state_->shutdown();
    free(the_state_count_);
  }
  // Don't return pointer to interface right now: we need to know when the
  // event handler has finished; return an AfterAdvice object instead:
  // it allows us to perform actions after access.
  AfterAdvice operator->() {
    assert(current_state_);
    assert(!pending_state_);

    // We need to know when the event handler has finished.
    return AfterAdvice(*this);
  }
  // Dispatch an event object to machine.
  void dispatch(EventParamBase<TOP> * event, bool destroy = true) {
    assert(event);

    event->dispatch(*current_state_);
    if (destroy) delete event;
    perform_pending();
  }
  // Allow (const) access to top state's data (for state data extraction).
  const typename TOP::Data & data() const {
    assert(current_state_);
    return static_cast<TOP &>(current_state_->instance()).TOP::data();
  }
  // Clear history of state and children.
  void clear_history_deep(StateInfo & state) {
    for (unsigned int i = 0; i < the_state_count_; ++i) {
      StateInfo * s = states_[i];
      if ((s != nullptr) && s->is_child(state))
        s->set_history(nullptr);
    }
  }

  // Next free identifier for StateInfo objects.
  static Key the_state_count_;

 private:
  Machine(const Machine<TOP> & other) = delete;
  Machine<TOP> & operator=(const Machine<TOP> & other) = delete;

  friend class Snapshot<TOP>;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation for Snapshot
template<class TOP>
Snapshot<TOP>::Snapshot(const Machine<TOP> & machine) {
  assert(!machine.pending_state_);
  assert(!machine.pending_event_);
  assert(machine.current_state_);

  allocate(Machine<TOP>::the_state_count_);
  copy(machine.states_, Machine<TOP>::the_state_count_);
  current_state_ = get_info(machine.current_state_->key());
}

// Root is always there and has key 0, so start from 1
template<class TOP> Key Machine<TOP>::the_state_count_ = 1;

// DIY virtual function table of class 'State'.
template<class S> StateCharacter State<S>::the_character_ = {
  State<S>::get_info,
  State<S>::destroy,
  State<S>::clone
};

} // namespace sc
} // namespace hrtm
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif  // HRTM_STATECHART_H_
