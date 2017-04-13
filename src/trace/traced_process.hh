/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TRACED_PROCESS_HH
#define TRACED_PROCESS_HH

#include <functional>
#include <unistd.h>
#include <cassert>
#include <csignal>

#include "syscall.hh"

enum ProcessState
{
  NOT_STARTED,
  RUNNING,
  RUNNING_SYSCALL,
  STOPPED,
  STOPPED_FOR_SYSCALL,
  TERMINATED
};

class TracedProcess
{
private:
  pid_t pid_;

  ProcessState process_state_;

  int exit_status_;
  int graceful_termination_signal_;

  bool died_on_signal_;
  bool moved_away_;

public:
  TracedProcess( char * args[],
                 const int termination_signal = SIGHUP );

  bool wait_for_syscall( std::function<void( SystemCallEntry )> before_entry,
                         std::function<void( SystemCallEntry, long int )> after_exit );

  void resume( void );

  template<typename T>
  T get_syscall_arg( uint8_t argnum );

  template<typename T>
  void set_syscall_arg( uint8_t argnum, T value );

  pid_t pid( void ) const { assert( not moved_away_ ); return pid_; }
  ProcessState process_state( void ) const { assert( not moved_away_ ); return process_state_; }

  /* Return exit status or signal that killed process */
  int exit_status( void ) const { assert( not moved_away_ ); assert( process_state_ == TERMINATED ); return exit_status_; }

  ~TracedProcess();

  /* ban copying */
  TracedProcess( const TracedProcess & other ) = delete;
  TracedProcess & operator=( const TracedProcess & other ) = delete;

  /* allow move constructor */
  TracedProcess( TracedProcess && other );

  /* ... but not move assignment operator */
  TracedProcess & operator=( TracedProcess && other ) = delete;
};
#endif /* TRACED_PROCESS_HH */
