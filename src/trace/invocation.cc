/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "invocation.hh"

#include <sstream>
#include <string>
#include <sys/ptrace.h>
#include <climits>

#include "util/exception.hh"

using namespace std;

template<typename T>
T SystemCallInvocation::get_syscall_arg( const uint8_t argnum ) const
{
  return reinterpret_cast<T>( ptrace( PTRACE_PEEKUSER, pid_,
                                      sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], NULL ) );
}

template<>
string SystemCallInvocation::get_syscall_arg( const uint8_t argnum ) const
{
  string result;

  char * str_addr = get_syscall_arg<char *>( argnum );
  size_t i = 0;

  do {
    errno = 0;

    int val = ptrace( PTRACE_PEEKTEXT, pid_, str_addr, NULL );

    if ( errno and val < 0 ) {
      throw unix_error( "ptrace(PEEKTEXT)" );
    }

    str_addr += sizeof( int );

    char * p = reinterpret_cast<char *>( &val );
    char c;

    for ( i = 0; i < sizeof( int ); i++ ) {
      c = *p++;
      if ( c == '\0' ) break;

      result += c;
    }
  } while( i == sizeof( int ) );

  return result;
}

template<typename T>
void SystemCallInvocation::set_syscall_arg( const uint8_t argnum, const T & value ) const
{
  ptrace( PTRACE_POKEUSER, pid_,
          sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], &value );
}

template<>
void SystemCallInvocation::set_syscall_arg( const uint8_t argnum, const string & value ) const
{
  if ( value.length() >= PATH_MAX ) {
    throw runtime_error( "maximum string length for set_syscall_arg is PATH_MAX" );
  }

  char * stack_addr = ( char * )ptrace( PTRACE_PEEKUSER, pid_, sizeof( long ) * RSP, 0 );
  stack_addr -= 128 + PATH_MAX * ( argnum + 1 );

  char * const str_addr = stack_addr;

  size_t i = 0;
  long val = 0;
  char * val_ptr = reinterpret_cast<char *>( &val );

  for ( const char * c = value.c_str(); ; c++ ) {
    val_ptr[ i++ ] = *c;

    if ( i == sizeof( long ) or *c == '\0' ) {
      ptrace( PTRACE_POKETEXT, pid_, stack_addr, val );

      stack_addr += i;
      i = 0;
      val = 0;
    }

    if ( *c == '\0' ) {
      break;
    }
  }

  ptrace( PTRACE_POKEUSER, pid_, sizeof( long ) * SYSCALL_ARG_REGS[ argnum ], str_addr );
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
template<> int    Argument::value() const { return ( int )value_.long_val.get(); }

Argument::Argument( ArgumentInfo info, const long raw_value )
  : info_( info ), raw_value_( raw_value ), value_()
{}

/* SystemCallInvocation */

SystemCallInvocation::SystemCallInvocation( const pid_t pid,
                                            const long syscall_no )
  : pid_( pid ), syscall_( syscall_no ), signature_(), arguments_(),
    return_value_()
{
  if ( syscall_signature( syscall_no ).complete() ) {
    const SystemCallSignature & sig = syscall_signature( syscall_no );
    signature_.reset( sig );
  }
}

void SystemCallInvocation::fetch_arguments()
{
  const SystemCallSignature & sig = syscall_signature( syscall_ );
  vector<ArgumentInfo> args = sig.arguments();

  arguments_.reset();

  for ( size_t i = 0; i < args.size(); i++ ) {
    const ArgumentInfo & arg_info = args.at( i );

    arguments_->emplace_back( arg_info, get_syscall_arg<long>( i ) );

    Argument & last_arg = arguments_->back();

    if ( arg_info.is_readable_string() ) {
      last_arg.set_value( get_syscall_arg<string>( i ) );
    }

    // let's set the raw value anyway
    last_arg.set_value( last_arg.raw_value() );
  }
}

std::string SystemCallInvocation::name()
{
  if ( signature_.initialized() ) {
    return signature_->name();
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
    out << signature()->name() << "(";

    size_t i = 0;
    if ( arguments_.initialized() ) {
      for ( auto & arg : *arguments_ ) {
        i++;

        if ( arg.info().is_readable_string() ) {
          out << '"' << arg.value<string>() << '"';
        }
        else {
          out << arg.value<long>();
        }

        if ( i != arguments_->size() ) {
          out << ", ";
        }
      }
    }
    else {
      out << "?";
    }

    out << ")";
  }
  else {
    out << "sc-" << syscall_no() << "(?)";
  }

  return out.str();
}

template<>
void SystemCallInvocation::set_argument( uint8_t argnum, const string value )
{
  set_syscall_arg( argnum, value );

  if ( arguments_.initialized() ) {
    arguments_->at( argnum ).set_value( value ); /* XXX what about the long_val? */
  }
}

template<>
void SystemCallInvocation::set_argument( uint8_t argnum, const char * value )
{
  string value_string( value );
  set_argument( argnum, value_string );
}

template void SystemCallInvocation::set_argument( uint8_t argnum, const string value );
