/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef EVENT_LOOP_HH
#define EVENT_LOOP_HH

#include <vector>
#include <functional>

#include "poller.hh"
#include "file_descriptor.hh"
#include "signalfd.hh"
#include "child_process.hh"

class TemporarilyUnprivileged {
private:
  const uid_t orig_euid;
  const gid_t orig_egid;

public:
  TemporarilyUnprivileged();
  ~TemporarilyUnprivileged();
};

class EventLoop
{
private:
  SignalMask signals_;
  Poller poller_;
  std::vector<std::pair<int, ChildProcess>> child_processes_;

protected:
  void add_action( Poller::Action action ) { poller_.add_action( action ); }

  PollerShortNames::Result handle_signal( const signalfd_siginfo & sig );
  int internal_loop( const std::function<int(void)> & wait_time );

public:
  EventLoop();

  void add_simple_input_handler( FileDescriptor & fd, const Poller::Action::CallbackType & callback );

  template <typename... Targs>
  void add_child_process( Targs&&... Fargs )
  {
    child_processes_.emplace_back( -1,
                                   ChildProcess( std::forward<Targs>( Fargs )... ) );
  }

  template <typename... Targs>
  void add_special_child_process( const int continue_status,
                                  Targs&&... Fargs )
  {
    /* parent won't quit when this process quits */
    child_processes_.emplace_back( continue_status, ChildProcess( std::forward<Targs>( Fargs )... ) );
  }

  int loop( void ) { return internal_loop( [] () { return -1; } ); } /* no timeout */

  virtual ~EventLoop() {}
};

#endif /* EVENT_LOOP_HH */
