/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SYSCALL_HH
#define SYSCALL_HH

#include <cstdlib>
#include <sys/syscall.h>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "optional.hh"
#include "sysent.hh"

static constexpr SystemCallEntry syscall_table[] = {
#include "syscallent.h"
};

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

class SystemCall
{
public:
  static SystemCallEntry get_syscall( size_t syscall_num );
};

enum ArgumentType
{
  ARGUMENT_TYPE_INT,
  ARGUMENT_TYPE_CHARSTAR,
};

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

  std::vector<ArgumentInfo> args() const { return args_; }
};

static std::map<long, SystemCallSignature> syscall_signatures =
{
  {
    SYS_open,
    {
      SYS_open, "open",
      {
        { ARGUMENT_TYPE_CHARSTAR, ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { ARGUMENT_TYPE_INT,      ARGUMENT_DIR_IN, 0 }, /* flags */
      }
    }
  }
};

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
  Optional<std::vector<Argument>> arguments_;

public:
  SystemCallInvocation( const TracedProcess & tp,
                        const TraceControlBlock & tcb,
                        const long syscall_no );
};

#endif /* SYSCALL_HH */
