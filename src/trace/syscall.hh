/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SYSCALL_HH
#define SYSCALL_HH

#include <cstdlib>
#include <sys/syscall.h>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "optional.hh"

struct TracedProcess;

struct TraceControlBlock
{
  pid_t pid;
  bool in_syscall { false };
  bool initialized { false };

  TraceControlBlock( pid_t pid )
    : pid( pid )
  {}
};

using ArgumentType = std::type_index;

enum ArgumentDirection
{
  ARGUMENT_DIR_IN,
  ARGUMENT_DIR_OUT,
  ARGUMENT_DIR_INOUT,
};

#define DIR_IN ARGUMENT_DIR_IN
#define DIR_OUT ARGUMENT_DIR_OUT
#define DIR_INOUT ARGUMENT_DIR_INOUT

enum ArgumentFlags
{
  ARGUMENT_F_PATHNAME = 0x1,
};

struct ArgumentInfo
{
  ArgumentType type;
  ArgumentDirection dir;
  int flags;
};

class SystemCallSignature
{
private:
  long number_;
  std::string name_;
  std::vector<ArgumentInfo> args_;

public:
  SystemCallSignature( const long number, const std::string & name,
                       const std::vector<ArgumentInfo> & args )
    : number_( number ), name_( name ), args_( args )
  {}

  long number() const { return number_; }
  std::string name() const { return name_; }
  std::vector<ArgumentInfo> args() const { return args_; }
};

#include "syscalltbl.hh"

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
  /* information about the process that invoked this system call */
  TraceControlBlock tcb_;

  /* basic information about the invoked system call */
  long syscall_;

  /* signature of the invoked system call, if available */
  Optional<SystemCallSignature> signature_;

  /* arguments to this system call */
  std::vector<Argument> arguments_;

public:
  SystemCallInvocation( const TracedProcess & tp,
                        const TraceControlBlock & tcb,
                        const long syscall_no, bool fetch_arguments = true );

  pid_t pid() const { return tcb_.pid; }
  long syscall_no() const { return syscall_; }
  Optional<SystemCallSignature> signature() const { return signature_; }
  std::vector<Argument> arguments() const { return arguments_; }
};

#endif /* SYSCALL_HH */
