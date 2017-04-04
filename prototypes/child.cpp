#include <sys/ptrace.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <signal.h>
#include <sys/syscall.h>   /* For SYS_write etc */

int main(){
    printf("pid = %d, ppid = %d\n", getpid(), getppid());
}