/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "syscall.hh"
#include "traced_process.hh"

using namespace std;

SystemCallEntry SystemCall::get_syscall( size_t syscall_num )
{
  return syscall_table[ syscall_num ];
}

/* Arugment */

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

    arguments_.initialize();

    for ( size_t i = 0; i < sig.args().size(); i++ ) {
      auto & arg_info = sig.args().at( i );
      arguments_.get().emplace_back( arg_info, tp.get_syscall_arg<long>( tcb, i ) );

      auto & last_arg = arguments_.get().back();

      if ( arg_info.type != ARGUMENT_TYPE_CHARSTAR ) {
        last_arg.set_value( last_arg.raw_value() );
      }
      else {
        last_arg.set_value( tp.get_syscall_arg<string>( tcb, i ) );
      }
    }
  }
}
