/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TRACER_THREAD_HH
#define TRACER_THREAD_HH

#include <map>
#include <functional>

#include "syscall.hh"
#include "invocation.hh"

class TracedThreadInfo
{
private:
  bool detached_ { false };

public:
  pid_t pid;

  Optional<SystemCallInvocation> syscall_invocation {};

  void detach() { detached_ = true; }
  bool detached() const { return detached_; }

  TracedThreadInfo( pid_t pid )
    : pid( pid )
  {}

  std::string to_string() const;
};

class TracerFlock;

class Tracer
{
private:
  pid_t tracee_pid_;

  bool options_set_ = false;
  void set_ptrace_options() const;
  
  typedef std::function<void( TracedThreadInfo & )> entry_type;
  typedef std::function<void( const TracedThreadInfo & )> exit_type;

  entry_type before_entry_function_;
  exit_type after_exit_function_;

public:
  Tracer( const pid_t tracee_pid,
          const entry_type & before_entry_function,
          const exit_type & after_exit_function );

  /* handle one event from the tracee.
     positive return value => no longer interested in tracee, so
     Tracer object ready to be destructed */
  bool handle_one_event( TracerFlock & flock );
  pid_t tracee_pid() const { return tracee_pid_; }
};

class TracerFlock
{
private:
  std::map<pid_t, Tracer> tracers_ {};

public:
  void insert( Tracer && tracer );
  void remove( const pid_t tracee_pid );
  
  void loop_until_all_done();
};

#endif /* TRACER_THREAD_HH */
