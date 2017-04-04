/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SYSCALL_HH
#define SYSCALL_HH

#include <cstdlib>
#include <sys/syscall.h>
#include "sysent.hh"

#define SEN(x) 0

static constexpr SystemCallEntry syscall_table[] = {
#include "syscallent.h"
};

class SystemCall
{
public:
  static SystemCallEntry get_syscall( size_t syscall_num );
};

#endif /* SYSCALL_HH */
