/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TRACER_THREAD_HH
#define TRACER_THREAD_HH

#include <map>
#include <functional>

#include "syscall.hh"
#include "invocation.hh"

struct TracedThreadInfo
{
  Optional<SystemCallInvocation> syscall_invocation {};

  bool detach = false;
};

class TracerFlock;

class Tracer
{
public:
  typedef std::function<void( TracedThreadInfo & )> entry_type;
  typedef std::function<void( const TracedThreadInfo & )> exit_type;

private:
  pid_t tracee_pid_;

  bool options_set_ = false;
  void set_ptrace_options() const;

  TracedThreadInfo info_ {};
  
public:
  Tracer( const pid_t tracee_pid );

  /* handle one event from the tracee.
     positive return value => no longer interested in tracee, so
     Tracer object ready to be destructed */
  bool handle_one_event( TracerFlock & flock,
                         const entry_type & before_entry_function,
                         const exit_type & after_exit_function );

  pid_t tracee_pid() const { return tracee_pid_; }
};

class TracerFlock
{
private:
  Tracer::entry_type before_entry_function_;
  Tracer::exit_type after_exit_function_;

  std::map<pid_t, Tracer> tracers_ {};

public:
  TracerFlock( const Tracer::entry_type & before_entry_function,
               const Tracer::exit_type & after_exit_function );

  void insert( const pid_t tracee_pid );
  void remove( const pid_t tracee_pid );
  
  void loop_until_all_done();
};

#endif /* TRACER_THREAD_HH */
