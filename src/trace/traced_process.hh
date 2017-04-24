/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TRACED_PROCESS_HH
#define TRACED_PROCESS_HH

#include <functional>
#include <unistd.h>
#include <cassert>
#include <csignal>
#include <map>

#include "syscall.hh"

struct TraceControlBlock
{
  pid_t pid;
  bool in_syscall { false };
  bool initialized { false };

  TraceControlBlock( pid_t pid )
    : pid( pid )
  {}
};

class TracedProcess
{
private:
  pid_t pid_;

  int exit_status_;
  int graceful_termination_signal_;

  bool died_on_signal_;
  bool moved_away_;

  std::map<pid_t, TraceControlBlock> processes_;

  bool ptrace_syscall( pid_t & out_pid );

public:
  TracedProcess( char * args[],
                 const int termination_signal = SIGHUP );

  bool wait_for_syscall( std::function<void( TraceControlBlock, long, SystemCallEntry )> before_entry,
                         std::function<void( TraceControlBlock, long, SystemCallEntry, long )> after_exit );

  void resume( void );

  template<typename T>
  T get_syscall_arg( TraceControlBlock tcb, uint8_t argnum );

  template<typename T>
  void set_syscall_arg( TraceControlBlock tcb, uint8_t argnum, T value );

  pid_t pid( void ) const { assert( not moved_away_ ); return pid_; }

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
