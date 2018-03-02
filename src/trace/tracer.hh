/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TRACER_HH
#define TRACER_HH

#include <map>
#include <functional>

#include "syscall.hh"
#include "invocation.hh"
#include "util/child_process.hh"

struct TracedThreadInfo
{
  pid_t pid;

  Optional<SystemCallInvocation> syscall_invocation {};

  bool detach = false;
  bool pause = false;
};

class TracerFlock;

class ProcessTracer
{
public:
  typedef std::function<void( TracedThreadInfo &, TracerFlock & )> entry_type;
  typedef std::function<void( const TracedThreadInfo & )> exit_type;

private:
  pid_t tracee_pid_;

  bool options_set_ = false;
  void set_ptrace_options() const;

  TracedThreadInfo info_ { tracee_pid_ };

public:
  ProcessTracer( const pid_t tracee_pid );
  ~ProcessTracer();

  /* handle one event from the tracee.
     positive return value => no longer interested in tracee, so
     ProcessTracer object ready to be destructed */
  bool handle_one_event( TracerFlock & flock,
                         const entry_type & before_entry_function,
                         const exit_type & after_exit_function );

  pid_t tracee_pid() const { return tracee_pid_; }
  bool is_paused() const { return info_.pause; }
  void resume( const int signal );

  ProcessTracer( const ProcessTracer & ) = delete;
  ProcessTracer & operator=( const ProcessTracer & ) = delete;

  ProcessTracer( ProcessTracer && pt );
};

class TracerFlock
{
private:
  ProcessTracer::entry_type before_entry_function_;
  ProcessTracer::exit_type after_exit_function_;

  std::map<pid_t, ChildProcess> children_ {};
  std::map<pid_t, ProcessTracer> tracers_ {};
  std::map<pid_t, pid_t> tracers_waiting_for_children_ {};

public:
  TracerFlock( const ProcessTracer::entry_type & before_entry_function,
               const ProcessTracer::exit_type & after_exit_function );

  void insert( const pid_t tracee_pid );
  void remove( const pid_t tracee_pid );

  void add_child_process( ChildProcess && child_process );
  void resume_after_termination( const pid_t child_to_wait_for,
                                 const pid_t tracee_to_resume );

  void loop_until_all_done();
};

class Tracer
{
private:
  TracerFlock flock_;

public:
  Tracer( const std::string & name,
          std::function<int()> && child_procedure,
          const ProcessTracer::entry_type & before_entry_function,
          const ProcessTracer::exit_type & after_exit_function,
          std::function<void()> && preparation_procedure = [](){} );

  void loop_until_done() { flock_.loop_until_all_done(); }
};

#endif /* TRACER_HH */
