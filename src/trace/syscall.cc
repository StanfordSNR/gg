/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "syscall.hh"
#include "traced_process.hh"

#include <iostream>

using namespace std;

SystemCallEntry SystemCall::get_syscall( size_t syscall_num )
{
  return syscall_table[ syscall_num ];
}

/* Argument */

template<>
void Argument::set_value( const string value )
{
  value_.string_val.reset( value );
}

template<>
void Argument::set_value( const long value )
{
  value_.long_val.reset( value );
}

template<> string Argument::value() const { return value_.string_val.get(); }
template<> long   Argument::value() const { return value_.long_val.get(); }

Argument::Argument( ArgumentInfo info, const long raw_value )
  : info_( info ), raw_value_( raw_value ), value_()
{}

/* SystemCallInvocation */

SystemCallInvocation::SystemCallInvocation( const TracedProcess & tp,
                                            const TraceControlBlock & tcb,
                                            const long syscall_no )
  : tcb_( tcb ), syscall_( syscall_no ), signature_(), arguments_()
{
  if ( syscall_signatures.count( syscall_no ) ) {
    const SystemCallSignature & sig = syscall_signatures.at( syscall_no );
    signature_.reset( sig );

    vector<ArgumentInfo> args = sig.args();

    for ( size_t i = 0; i < args.size(); i++ ) {
      const ArgumentInfo & arg_info = args.at( i );

      arguments_.emplace_back( arg_info, tp.get_syscall_arg<long>( tcb, i ) );

      Argument & last_arg = arguments_.back();

      if ( arg_info.type != typeid( char * ) ) {
        last_arg.set_value( last_arg.raw_value() );
      }
      else {
        last_arg.set_value( tp.get_syscall_arg<string>( tcb, i ) );
      }
    }
  }
}
