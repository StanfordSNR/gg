/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <sstream>
#include <string>

#include "invocation.hh"
#include "traced_process.hh"

using namespace std;

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
template<> int    Argument::value() const { return ( int )value_.long_val.get(); }

Argument::Argument( ArgumentInfo info, const long raw_value )
  : info_( info ), raw_value_( raw_value ), value_()
{}

/* SystemCallInvocation */

SystemCallInvocation::SystemCallInvocation( const pid_t pid,
                                            const long syscall_no,
                                            bool fetch_arguments )
  : syscall_( syscall_no ), signature_(), arguments_()
{
  if ( syscall_signatures.count( syscall_no ) ) {
    const SystemCallSignature & sig = syscall_signatures.at( syscall_no );
    signature_.reset( sig );

    if ( not fetch_arguments ) {
      return;
    }

    vector<ArgumentInfo> args = sig.arguments();

    for ( size_t i = 0; i < args.size(); i++ ) {
      const ArgumentInfo & arg_info = args.at( i );

      arguments_.emplace_back( arg_info, TracedProcess::get_syscall_arg<long>( pid, i ) );

      Argument & last_arg = arguments_.back();

      if ( arg_info.is_readable_string() ) {
        last_arg.set_value( TracedProcess::get_syscall_arg<string>( pid, i ) );
      }

      // let's set the raw value anyway
      last_arg.set_value( last_arg.raw_value() );
    }
  }
}

std::string SystemCallInvocation::name()
{
  if ( signature_.initialized() ) {
    return signature_.get().name();
  }
  else {
    ostringstream out;
    out << "sc-" << syscall_;
    return out.str();
  }
}

std::string SystemCallInvocation::to_string() const
{
  ostringstream out;

  if ( signature().initialized() ) {
    out << signature().get().name() << "(";

    size_t i = 0;
    for ( auto & arg : arguments() ) {
      i++;

      if ( arg.info().is_readable_string() ) {
        out << '"' << arg.value<string>() << '"';
      }
      else {
        out << arg.value<long>();
      }

      if ( i != arguments().size() ) {
        out << ", ";
      }
    }

    out << ")";
  }
  else {
    out << "sc-" << syscall_no() << "(?)";
  }

  return out.str();
}
