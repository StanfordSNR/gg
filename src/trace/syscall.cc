/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "syscall.hh"

SystemCallEntry SystemCall::get_syscall( size_t syscall_num )
{
  return syscall_table[ syscall_num ];
}
