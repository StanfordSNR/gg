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
int main() {
    pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;
    struct user_regs_struct uregs;
    child = fork();
    if (child == 0) {
        printf("pid = %d, ppid = %d\n", getpid(), getppid());
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("./child", "child", NULL);

    } else {
        while (1) {
            wait(&status);
            if (WIFEXITED(status)) {
                std::cout << "exiting for some reason" << std::endl;
                break;
            }
            orig_eax = ptrace(PTRACE_PEEKUSER,
                       child, 8 * ORIG_RAX, NULL);

            ptrace(PTRACE_GETREGS, child, 0, &uregs);
            //std::cout << "syscall: " << uregs.orig_rax << std::endl;
            if (orig_eax == -1){
                break;
            }

            orig_eax = uregs.orig_rax;
            if (orig_eax == SYS_getpid) {
                if (insyscall == 0) {
                    /* Syscall entry */
                    insyscall = 1;
                    std::cout << "FOUND GETPID" << std::endl;
                    uregs.orig_rax = SYS_getppid;
                    ptrace(PTRACE_SETREGS, child, 0, &uregs);
                    //ptrace(PTRACE_CONT, child, 0, 0);
                } else { /* Syscall exit */
                    std::cout << "Syscall Exited " << std::endl;
                    eax = ptrace(PTRACE_PEEKUSER,
                                 child, 8 * RAX, NULL);
                    printf("Write returned "
                                   "with %ld\n", eax);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }
    return 0;
}
