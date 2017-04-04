#include "syscall.hh"

SystemCallEntry SystemCall::get_syscall( size_t syscall_num )
{
  return syscall_table[ syscall_num ];
}
