/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SYSCALL_HH
#define SYSCALL_HH

#include <cstdlib>
#include <sys/syscall.h>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <vector>
#include <map> /* needed by syscalltbl */

using ArgumentType = std::type_index;

#define TRACE_FILE		001	/* Trace file-related syscalls. */
#define TRACE_IPC		002	/* Trace IPC-related syscalls. */
#define TRACE_NETWORK		004	/* Trace network-related syscalls. */
#define TRACE_PROCESS		010	/* Trace process-related syscalls. */
#define TRACE_SIGNAL		020	/* Trace signal-related syscalls. */
#define TRACE_DESC		040	/* Trace file descriptor-related syscalls. */
#define TRACE_MEMORY		0100	/* Trace memory mapping-related syscalls. */
#define SYSCALL_NEVER_FAILS	0200	/* Syscall is always successful. */
#define STACKTRACE_INVALIDATE_CACHE 0400  /* Trigger proc/maps cache updating */
#define STACKTRACE_CAPTURE_ON_ENTER 01000 /* Capture stacktrace on "entering" stage */
#define TRACE_INDIRECT_SUBCALL	02000	/* Syscall is an indirect socket/ipc subcall. */
#define COMPAT_SYSCALL_TYPES	04000	/* A compat syscall that uses compat types. */
#define TRACE_STATFS		040000	/* Trace statfs, statfs64, and statvfs syscalls. */

enum ArgumentDirection
{
  ARGUMENT_DIR_IN,
  ARGUMENT_DIR_OUT,
  ARGUMENT_DIR_INOUT,
};

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
  int flags_;

public:
  SystemCallSignature( const long number, const std::string & name,
                       const std::vector<ArgumentInfo> & args, int flags = 0 )
    : number_( number ), name_( name ), args_( args ), flags_( flags )
  {}

  long number() const { return number_; }
  std::string name() const { return name_; }
  std::vector<ArgumentInfo> args() const { return args_; }
  int flags() const { return flags_; }
};

#include "syscalltbl.hh"

#endif /* SYSCALL_HH */
