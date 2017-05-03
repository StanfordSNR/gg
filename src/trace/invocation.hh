/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef INVOCATION_HH
#define INVOCATION_HH

#include <string>
#include <vector>

#include "optional.hh"
#include "syscall.hh"

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
  /* syscall number */
  long syscall_;

  /* signature of the invoked system call, if available */
  Optional<SystemCallSignature> signature_;

  /* arguments to this system call */
  std::vector<Argument> arguments_;

public:
  SystemCallInvocation( const pid_t pid, const long syscall_no,
                        bool fetch_arguments = true );

  long syscall_no() const { return syscall_; }
  std::string name();
  Optional<SystemCallSignature> signature() const { return signature_; }
  std::vector<Argument> arguments() const { return arguments_; }

  std::string to_string() const;
};

#endif /* INVOCATION_HH */
