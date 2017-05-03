/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "sandbox.hh"

using namespace std;

Sandbox::Sandbox( function<int()> && child_procedure,
                  const unordered_set<string> & allowed_reads,
                  const unordered_set<string> & allowed_writes )
  : process_( move( child_procedure ) ),
    allowed_reads_( allowed_reads ), allowed_writes_( allowed_writes )
{}

void Sandbox::execute()
{
  auto syscall_entry =
    [&]( const TraceControlBlock & tcb )
    {
      auto & syscall = tcb.syscall_invocation.get();

      switch ( syscall.syscall_no() ) {
      case SYS_open:
        return;

      default:
        throw SandboxViolation( "Unknown syscall" );
      }
    };

  auto syscall_exit =
    [&]( const TraceControlBlock &, long )
    {};

  while ( true ) {
    int waitres = process_.wait_for_syscall( syscall_entry, syscall_exit );
    if ( not waitres ) { break; }
  }
}
