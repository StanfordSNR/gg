/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef INVOCATION_HH
#define INVOCATION_HH

#include <string>
#include <vector>

#include "syscall.hh"
#include "util/optional.hh"

struct ArgumentValue
{
  Optional<std::string> string_val {};
  Optional<long> long_val {};
};

class Argument
{
private:
  ArgumentInfo info_;
  long raw_value_;

  ArgumentValue value_;

public:
  Argument( ArgumentInfo info, const long raw_value );

  long raw_value() const { return raw_value_; }

  template<typename T>
  void set_value( const T value );

  template<typename T>
  T value() const;

  ArgumentInfo info() const { return info_; }
};

class SystemCallInvocation
{
private:
  /* pid of the calling process */
  pid_t pid_;

  /* syscall number */
  long syscall_;

  /* signature of the invoked system call, if available */
  Optional<SystemCallSignature> signature_;

  /* arguments to this system call */
  Optional<std::vector<Argument>> arguments_;

  /* syscall return value.
     if this value is not set, it means that the syscall has not been invoked
     yet */
  Optional<long> return_value_;

  template<typename T>
  T get_syscall_arg( const uint8_t argnum ) const;

  template<typename T>
  void set_syscall_arg( const uint8_t argnum, const T & value ) const;

public:
  SystemCallInvocation( const pid_t pid, const long syscall_no );

  long syscall_no() const { return syscall_; }
  std::string name();
  Optional<SystemCallSignature> signature() const { return signature_; }
  const Optional<std::vector<Argument>> & arguments() const { return arguments_; }
  Optional<long> retval() const { return return_value_; }

  void fetch_arguments();

  void set_retval( const long return_value ) { return_value_.reset( return_value ); }

  template<typename T>
  void set_argument( uint8_t argnum, const T value );

  std::string to_string() const;
};

#endif /* INVOCATION_HH */
