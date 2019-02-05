#include "syscall.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/capability.h>
#include <linux/types.h>
#include <linux/aio_abi.h>
#include <signal.h>
#include <poll.h>
#include <mqueue.h>

// from <keyutils.h>
typedef int32_t key_serial_t;

const SystemCallSignature & syscall_signature( const size_t syscall_no )
{
  static const SystemCallSignature syscall_signatures[] =
  {
    // sys_read(unsigned int fd, char * buf, size_t count) [include/linux/syscalls.h:562]
    {
      0, "read",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC, true
    },

    // sys_write(unsigned int fd, const char * buf, size_t count) [include/linux/syscalls.h:567]
    {
      1, "write",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* buf */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC, true
    },

    // sys_open(const char * filename, int flags, mode_t mode) [include/linux/syscalls.h:517]
    {
      2, "open",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_close(unsigned int fd) [include/linux/syscalls.h:519]
    {
      3, "close",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
      },
      TRACE_DESC, true
    },

    // sys_stat(const char * filename, struct __old_kernel_stat * statbuf) [include/linux/syscalls.h:394]
    {
      4, "stat",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct __old_kernel_stat * ), ARGUMENT_DIR_IN, 0 }, /* statbuf */
      },
      TRACE_FILE, true
    },

    // sys_fstat(unsigned int fd, struct __old_kernel_stat * statbuf) [include/linux/syscalls.h:405]
    {
      5, "fstat",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct __old_kernel_stat * ), ARGUMENT_DIR_IN, 0 }, /* statbuf */
      },
      TRACE_DESC, true
    },

    // sys_lstat(const char * filename, struct __old_kernel_stat * statbuf) [include/linux/syscalls.h:403]
    {
      6, "lstat",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct __old_kernel_stat * ), ARGUMENT_DIR_IN, 0 }, /* statbuf */
      },
      TRACE_FILE, true
    },

    // sys_poll(struct pollfd * ufds, unsigned int nfds, int timeout) [include/linux/syscalls.h:628]
    {
      7, "poll",
      {
        { typeid( struct pollfd * ), ARGUMENT_DIR_IN, 0 }, /* ufds */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* nfds */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* timeout */
      },
      TRACE_DESC, true
    },

    // sys_lseek(unsigned int fd, off_t offset, unsigned int whence) [include/linux/syscalls.h:557]
    {
      8, "lseek",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( off_t ), ARGUMENT_DIR_IN, 0 }, /* offset */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* whence */
      },
      TRACE_DESC, true
    },

    // sys_mmap(unsigned long addr, unsigned long len, int prot, int flags, int fd, long off) [arch/ia64/include/asm/unistd.h:38]
    {
      9, "mmap",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* addr */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* prot */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* off */
      },
      TRACE_DESC | TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_mprotect(unsigned long start, size_t len, unsigned long prot) [include/linux/syscalls.h:451]
    {
      10, "mprotect",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* prot */
      },
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_munmap(unsigned long addr, size_t len) [include/linux/syscalls.h:462]
    {
      11, "munmap",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* addr */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_brk(unsigned long brk) [include/linux/syscalls.h:450]
    {
      12, "brk",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* brk */
      },
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_rt_sigaction(int unnamed, const struct sigaction * unnamed, struct sigaction * unnamed, size_t unnamed) [include/linux/syscalls.h:360]
    {
      13, "rt_sigaction",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( const struct sigaction * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sigaction * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_SIGNAL, true
    },

    // sys_rt_sigprocmask(int how, sigset_t * set, sigset_t * oset, size_t sigsetsize) [include/linux/syscalls.h:365]
    {
      14, "rt_sigprocmask",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* how */
        { typeid( sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* set */
        { typeid( sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* oset */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sigsetsize */
      },
      TRACE_SIGNAL, true
    },

    // sys_rt_sigreturn() [arch/h8300/kernel/syscalls.c:10]
    {
      15, "rt_sigreturn",
      {

      },
      TRACE_SIGNAL, true
    },

    // sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg) [include/linux/syscalls.h:496]
    {
      16, "ioctl",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg */
      },
      TRACE_DESC, true
    },

    // sys_pread64(unsigned int fd, char * buf, size_t count, loff_t pos) [include/linux/syscalls.h:572]
    {
      17, "pread64",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* pos */
      },
      TRACE_DESC, true
    },

    // sys_pwrite64(unsigned int fd, const char * buf, size_t count, loff_t pos) [include/linux/syscalls.h:574]
    {
      18, "pwrite64",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* buf */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* pos */
      },
      TRACE_DESC, true
    },

    // sys_readv(unsigned long fd, const struct iovec * vec, unsigned long vlen) [include/linux/syscalls.h:564]
    {
      19, "readv",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* vec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* vlen */
      },
      TRACE_DESC, true
    },

    // sys_writev(unsigned long fd, const struct iovec * vec, unsigned long vlen) [include/linux/syscalls.h:569]
    {
      20, "writev",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* vec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* vlen */
      },
      TRACE_DESC, true
    },

    // sys_access(const char * filename, int mode) [include/linux/syscalls.h:520]
    {
      21, "access",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_FILE, true
    },

    // sys_pipe(int * fildes) [include/linux/syscalls.h:490]
    {
      22, "pipe",
      {
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* fildes */
      },
      TRACE_DESC, true
    },

    // sys_select(int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timeval * tvp) [include/linux/syscalls.h:630]
    {
      23, "select",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* n */
        { typeid( fd_set * ), ARGUMENT_DIR_IN, 0 }, /* inp */
        { typeid( fd_set * ), ARGUMENT_DIR_IN, 0 }, /* outp */
        { typeid( fd_set * ), ARGUMENT_DIR_IN, 0 }, /* exp */
        { typeid( struct timeval * ), ARGUMENT_DIR_IN, 0 }, /* tvp */
      },
      TRACE_DESC, true
    },

    // sys_sched_yield() [include/linux/syscalls.h:306]
    {
      24, "sched_yield",
      {

      },
      0, true
    },

    // sys_mremap(unsigned long addr, unsigned long old_len, unsigned long new_len, unsigned long flags, unsigned long new_addr) [include/linux/syscalls.h:453]
    {
      25, "mremap",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* addr */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* old_len */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* new_len */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* new_addr */
      },
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_msync(unsigned long start, size_t len, int flags) [include/linux/syscalls.h:459]
    {
      26, "msync",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY, true
    },

    // sys_mincore(unsigned long start, size_t len, unsigned char * vec) [include/linux/syscalls.h:468]
    {
      27, "mincore",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned char * ), ARGUMENT_DIR_IN, 0 }, /* vec */
      },
      TRACE_MEMORY, true
    },

    // sys_madvise(unsigned long start, size_t len, int behavior) [include/linux/syscalls.h:467]
    {
      28, "madvise",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* behavior */
      },
      TRACE_MEMORY, true
    },

    // sys_shmget(key_t key, size_t size, int flag) [include/linux/syscalls.h:678]
    {
      29, "shmget",
      {
        { typeid( key_t ), ARGUMENT_DIR_IN, 0 }, /* key */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flag */
      },
      TRACE_IPC, true
    },

    // sys_shmat(int shmid, char * shmaddr, int shmflg) [include/linux/syscalls.h:677]
    {
      30, "shmat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* shmid */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* shmaddr */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* shmflg */
      },
      TRACE_IPC | TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_shmctl(int shmid, int cmd, struct shmid_ds * buf) [include/linux/syscalls.h:680]
    {
      31, "shmctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* shmid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( struct shmid_ds * ), ARGUMENT_DIR_IN, 0 }, /* buf */
      },
      TRACE_IPC, true
    },

    // sys_dup(unsigned int fildes) [include/linux/syscalls.h:492]
    {
      32, "dup",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fildes */
      },
      TRACE_DESC, true
    },

    // sys_dup2(unsigned int oldfd, unsigned int newfd) [include/linux/syscalls.h:493]
    {
      33, "dup2",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* oldfd */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* newfd */
      },
      TRACE_DESC, true
    },

    // sys_pause() [include/linux/syscalls.h:381]
    {
      34, "pause",
      {

      },
      TRACE_SIGNAL, true
    },

    // sys_nanosleep(struct timespec * rqtp, struct timespec * rmtp) [include/linux/syscalls.h:221]
    {
      35, "nanosleep",
      {
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* rqtp */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* rmtp */
      },
      0, true
    },

    // sys_getitimer(int which, struct itimerval * value) [include/linux/syscalls.h:261]
    {
      36, "getitimer",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( struct itimerval * ), ARGUMENT_DIR_IN, 0 }, /* value */
      },
      0, true
    },

    // sys_alarm(unsigned int seconds) [include/linux/syscalls.h:222]
    {
      37, "alarm",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* seconds */
      },
      0, true
    },

    // sys_setitimer(int which, struct itimerval * value, struct itimerval * ovalue) [include/linux/syscalls.h:262]
    {
      38, "setitimer",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( struct itimerval * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( struct itimerval * ), ARGUMENT_DIR_IN, 0 }, /* ovalue */
      },
      0, true
    },

    // sys_getpid() [include/linux/syscalls.h:223]
    {
      39, "getpid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_sendfile(int out_fd, int in_fd, off_t * offset, size_t count) [include/linux/syscalls.h:510]
    {
      40, "sendfile",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* out_fd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* in_fd */
        { typeid( off_t * ), ARGUMENT_DIR_IN, 0 }, /* offset */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC | TRACE_NETWORK, true
    },

    // sys_socket(int unnamed, int unnamed, int unnamed) [include/linux/syscalls.h:624]
    {
      41, "socket",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_connect(int unnamed, struct sockaddr * unnamed, int unnamed) [include/linux/syscalls.h:606]
    {
      42, "connect",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_accept(int unnamed, struct sockaddr * unnamed, int * unnamed) [include/linux/syscalls.h:607]
    {
      43, "accept",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_sendto(int unnamed, void * unnamed, size_t unnamed, unsigned unnamed, struct sockaddr * unnamed, int unnamed) [include/linux/syscalls.h:612]
    {
      44, "sendto",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_recvfrom(int unnamed, void * unnamed, size_t unnamed, unsigned unnamed, struct sockaddr * unnamed, int * unnamed) [include/linux/syscalls.h:618]
    {
      45, "recvfrom",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_sendmsg(int fd, struct user_msghdr * msg, unsigned flags) [include/linux/syscalls.h:614]
    {
      46, "sendmsg",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct user_msghdr * ), ARGUMENT_DIR_IN, 0 }, /* msg */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_NETWORK, true
    },

    // sys_recvmsg(int fd, struct user_msghdr * msg, unsigned flags) [include/linux/syscalls.h:620]
    {
      47, "recvmsg",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct user_msghdr * ), ARGUMENT_DIR_IN, 0 }, /* msg */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_NETWORK, true
    },

    // sys_shutdown(int unnamed, int unnamed) [include/linux/syscalls.h:314]
    {
      48, "shutdown",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_bind(int unnamed, struct sockaddr * unnamed, int unnamed) [include/linux/syscalls.h:605]
    {
      49, "bind",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_listen(int unnamed, int unnamed) [include/linux/syscalls.h:627]
    {
      50, "listen",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_getsockname(int unnamed, struct sockaddr * unnamed, int * unnamed) [include/linux/syscalls.h:609]
    {
      51, "getsockname",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_getpeername(int unnamed, struct sockaddr * unnamed, int * unnamed) [include/linux/syscalls.h:610]
    {
      52, "getpeername",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_socketpair(int unnamed, int unnamed, int unnamed, int * unnamed) [include/linux/syscalls.h:625]
    {
      53, "socketpair",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_setsockopt(int fd, int level, int optname, char * optval, int optlen) [include/linux/syscalls.h:601]
    {
      54, "setsockopt",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* level */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* optname */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* optval */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* optlen */
      },
      TRACE_NETWORK, true
    },

    // sys_getsockopt(int fd, int level, int optname, char * optval, int * optlen) [include/linux/syscalls.h:603]
    {
      55, "getsockopt",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* level */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* optname */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* optval */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* optlen */
      },
      TRACE_NETWORK, true
    },

    // sys_clone(unsigned long, unsigned long, int * unnamed, unsigned long, int * unnamed) [include/linux/syscalls.h:837]
    {
      56, "clone",
      {
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_PROCESS, true
    },

    // sys_fork() [include/linux/syscalls.h:834]
    {
      57, "fork",
      {

      },
      TRACE_PROCESS, true
    },

    // sys_vfork() [include/linux/syscalls.h:835]
    {
      58, "vfork",
      {

      },
      TRACE_PROCESS, true
    },

    // sys_execve(const char * filename, const char * const * argv, const char * const * envp) [include/linux/syscalls.h:849]
    {
      59, "execve",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( const char * const * ), ARGUMENT_DIR_IN, 0 }, /* argv */
        { typeid( const char * const * ), ARGUMENT_DIR_IN, 0 }, /* envp */
      },
      TRACE_FILE | TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_exit(int error_code) [include/linux/syscalls.h:326]
    {
      60, "exit",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* error_code */
      },
      TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER, true
    },

    // sys_wait4(pid_t pid, int * stat_addr, int options, struct rusage * ru) [include/linux/syscalls.h:328]
    {
      61, "wait4",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* stat_addr */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* options */
        { typeid( struct rusage * ), ARGUMENT_DIR_IN, 0 }, /* ru */
      },
      TRACE_PROCESS, true
    },

    // sys_kill(pid_t pid, int sig) [include/linux/syscalls.h:374]
    {
      62, "kill",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* sig */
      },
      TRACE_SIGNAL, true
    },

    // sys_uname(struct old_utsname * unnamed) [include/linux/syscalls.h:647]
    {
      63, "uname",
      {
        { typeid( struct old_utsname * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      0, true
    },

    // sys_semget(key_t key, int nsems, int semflg) [include/linux/syscalls.h:670]
    {
      64, "semget",
      {
        { typeid( key_t ), ARGUMENT_DIR_IN, 0 }, /* key */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* nsems */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* semflg */
      },
      TRACE_IPC, true
    },

    // sys_semop(int semid, struct sembuf * sops, unsigned nsops) [include/linux/syscalls.h:671]
    {
      65, "semop",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* semid */
        { typeid( struct sembuf * ), ARGUMENT_DIR_IN, 0 }, /* sops */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* nsops */
      },
      TRACE_IPC, true
    },

    // sys_semctl(int semid, int semnum, int cmd, unsigned long arg) [include/linux/syscalls.h:673]
    {
      66, "semctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* semid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* semnum */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg */
      },
      TRACE_IPC, true
    },

    // sys_shmdt(char * shmaddr) [include/linux/syscalls.h:679]
    {
      67, "shmdt",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* shmaddr */
      },
      TRACE_IPC | TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_msgget(key_t key, int msgflg) [include/linux/syscalls.h:663]
    {
      68, "msgget",
      {
        { typeid( key_t ), ARGUMENT_DIR_IN, 0 }, /* key */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* msgflg */
      },
      TRACE_IPC, true
    },

    // sys_msgsnd(int msqid, struct msgbuf * msgp, size_t msgsz, int msgflg) [include/linux/syscalls.h:664]
    {
      69, "msgsnd",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* msqid */
        { typeid( struct msgbuf * ), ARGUMENT_DIR_IN, 0 }, /* msgp */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* msgsz */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* msgflg */
      },
      TRACE_IPC, true
    },

    // sys_msgrcv(int msqid, struct msgbuf * msgp, size_t msgsz, long msgtyp, int msgflg) [include/linux/syscalls.h:666]
    {
      70, "msgrcv",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* msqid */
        { typeid( struct msgbuf * ), ARGUMENT_DIR_IN, 0 }, /* msgp */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* msgsz */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* msgtyp */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* msgflg */
      },
      TRACE_IPC, true
    },

    // sys_msgctl(int msqid, int cmd, struct msqid_ds * buf) [include/linux/syscalls.h:668]
    {
      71, "msgctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* msqid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( struct msqid_ds * ), ARGUMENT_DIR_IN, 0 }, /* buf */
      },
      TRACE_IPC, true
    },

    // sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg) [include/linux/syscalls.h:485]
    {
      72, "fcntl",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg */
      },
      TRACE_DESC, true
    },

    // sys_flock(unsigned int fd, unsigned int cmd) [include/linux/syscalls.h:498]
    {
      73, "flock",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
      },
      TRACE_DESC, true
    },

    // sys_fsync(unsigned int fd) [include/linux/syscalls.h:384]
    {
      74, "fsync",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
      },
      TRACE_DESC, true
    },

    // sys_fdatasync(unsigned int fd) [include/linux/syscalls.h:385]
    {
      75, "fdatasync",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
      },
      TRACE_DESC, true
    },

    // sys_truncate(const char * path, long length) [include/linux/syscalls.h:392]
    {
      76, "truncate",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* length */
      },
      TRACE_FILE, true
    },

    // sys_ftruncate(unsigned int fd, unsigned long length) [include/linux/syscalls.h:393]
    {
      77, "ftruncate",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* length */
      },
      TRACE_DESC, true
    },

    // sys_getdents(unsigned int fd, struct linux_dirent * dirent, unsigned int count) [include/linux/syscalls.h:594]
    {
      78, "getdents",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct linux_dirent * ), ARGUMENT_DIR_IN, 0 }, /* dirent */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC, true
    },

    // sys_getcwd(char * buf, unsigned long size) [include/linux/syscalls.h:586]
    {
      79, "getcwd",
      {
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_FILE, true
    },

    // sys_chdir(const char * filename) [include/linux/syscalls.h:588]
    {
      80, "chdir",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
      },
      TRACE_FILE, true
    },

    // sys_fchdir(unsigned int fd) [include/linux/syscalls.h:589]
    {
      81, "fchdir",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
      },
      TRACE_DESC, true
    },

    // sys_rename(const char * oldname, const char * newname) [include/linux/syscalls.h:480]
    {
      82, "rename",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* oldname */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* newname */
      },
      TRACE_FILE, true
    },

    // sys_mkdir(const char * pathname, mode_t mode) [include/linux/syscalls.h:587]
    {
      83, "mkdir",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_FILE, true
    },

    // sys_rmdir(const char * pathname) [include/linux/syscalls.h:590]
    {
      84, "rmdir",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
      },
      TRACE_FILE, true
    },

    // sys_creat(const char * pathname, mode_t mode) [include/linux/syscalls.h:516]
    {
      85, "creat",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_link(const char * oldname, const char * newname) [include/linux/syscalls.h:476]
    {
      86, "link",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* oldname */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* newname */
      },
      TRACE_FILE, true
    },

    // sys_unlink(const char * pathname) [include/linux/syscalls.h:479]
    {
      87, "unlink",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
      },
      TRACE_FILE, true
    },

    // sys_symlink(const char * old, const char * new) [include/linux/syscalls.h:478]
    {
      88, "symlink",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* old */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* new */
      },
      TRACE_FILE, true
    },

    // sys_readlink(const char * path, char * buf, int bufsiz) [include/linux/syscalls.h:514]
    {
      89, "readlink",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* bufsiz */
      },
      TRACE_FILE, true
    },

    // sys_chmod(const char * filename, mode_t mode) [include/linux/syscalls.h:482]
    {
      90, "chmod",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_FILE, true
    },

    // sys_fchmod(unsigned int fd, mode_t mode) [include/linux/syscalls.h:483]
    {
      91, "fchmod",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC, true
    },

    // sys_chown(const char * filename, uid_t user, gid_t group) [include/linux/syscalls.h:522]
    {
      92, "chown",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* user */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* group */
      },
      TRACE_FILE, true
    },

    // sys_fchown(unsigned int fd, uid_t user, gid_t group) [include/linux/syscalls.h:526]
    {
      93, "fchown",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* user */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* group */
      },
      TRACE_DESC, true
    },

    // sys_lchown(const char * filename, uid_t user, gid_t group) [include/linux/syscalls.h:524]
    {
      94, "lchown",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* user */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* group */
      },
      TRACE_FILE, true
    },

    // sys_umask(int mask) [include/linux/syscalls.h:661]
    {
      95, "umask",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* mask */
      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_gettimeofday(struct timeval * tv, struct timezone * tz) [include/linux/syscalls.h:212]
    {
      96, "gettimeofday",
      {
        { typeid( struct timeval * ), ARGUMENT_DIR_IN, 0 }, /* tv */
        { typeid( struct timezone * ), ARGUMENT_DIR_IN, 0 }, /* tz */
      },
      0, true
    },

    // sys_getrlimit(unsigned int resource, struct rlimit * rlim) [include/linux/syscalls.h:650]
    {
      97, "getrlimit",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* resource */
        { typeid( struct rlimit * ), ARGUMENT_DIR_IN, 0 }, /* rlim */
      },
      0, true
    },

    // sys_getrusage(int who, struct rusage * ru) [include/linux/syscalls.h:660]
    {
      98, "getrusage",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* who */
        { typeid( struct rusage * ), ARGUMENT_DIR_IN, 0 }, /* ru */
      },
      0, true
    },

    // sys_sysinfo(struct sysinfo * info) [include/linux/syscalls.h:704]
    {
      99, "sysinfo",
      {
        { typeid( struct sysinfo * ), ARGUMENT_DIR_IN, 0 }, /* info */
      },
      0, true
    },

    // sys_times(struct tms * tbuf) [include/linux/syscalls.h:218]
    {
      100, "times",
      {
        { typeid( struct tms * ), ARGUMENT_DIR_IN, 0 }, /* tbuf */
      },
      0, true
    },

    // sys_ptrace(long request, long pid, unsigned long addr, unsigned long data) [include/linux/syscalls.h:710]
    {
      101, "ptrace",
      {
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* request */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* addr */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* data */
      },
      0, true
    },

    // sys_getuid() [include/linux/syscalls.h:225]
    {
      102, "getuid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_syslog(int type, char * buf, int len) [include/linux/syscalls.h:707]
    {
      103, "syslog",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* type */
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      0, true
    },

    // sys_getgid() [include/linux/syscalls.h:227]
    {
      104, "getgid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_setuid(uid_t uid) [include/linux/syscalls.h:239]
    {
      105, "setuid",
      {
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* uid */
      },
      0, true
    },

    // sys_setgid(gid_t gid) [include/linux/syscalls.h:237]
    {
      106, "setgid",
      {
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* gid */
      },
      0, true
    },

    // sys_geteuid() [include/linux/syscalls.h:226]
    {
      107, "geteuid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_getegid() [include/linux/syscalls.h:228]
    {
      108, "getegid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_setpgid(pid_t pid, pid_t pgid) [include/linux/syscalls.h:244]
    {
      109, "setpgid",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pgid */
      },
      0, true
    },

    // sys_getppid() [include/linux/syscalls.h:224]
    {
      110, "getppid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_getpgrp() [include/linux/syscalls.h:232]
    {
      111, "getpgrp",
      {

      },
      0, true
    },

    // sys_setsid() [include/linux/syscalls.h:245]
    {
      112, "setsid",
      {

      },
      0, true
    },

    // sys_setreuid(uid_t ruid, uid_t euid) [include/linux/syscalls.h:238]
    {
      113, "setreuid",
      {
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* ruid */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* euid */
      },
      0, true
    },

    // sys_setregid(gid_t rgid, gid_t egid) [include/linux/syscalls.h:236]
    {
      114, "setregid",
      {
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* rgid */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* egid */
      },
      0, true
    },

    // sys_getgroups(int gidsetsize, gid_t * grouplist) [include/linux/syscalls.h:234]
    {
      115, "getgroups",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* gidsetsize */
        { typeid( gid_t * ), ARGUMENT_DIR_IN, 0 }, /* grouplist */
      },
      0, true
    },

    // sys_setgroups(int gidsetsize, gid_t * grouplist) [include/linux/syscalls.h:246]
    {
      116, "setgroups",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* gidsetsize */
        { typeid( gid_t * ), ARGUMENT_DIR_IN, 0 }, /* grouplist */
      },
      0, true
    },

    // sys_setresuid(uid_t ruid, uid_t euid, uid_t suid) [include/linux/syscalls.h:240]
    {
      117, "setresuid",
      {
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* ruid */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* euid */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* suid */
      },
      0, true
    },

    // sys_getresuid(uid_t * ruid, uid_t * euid, uid_t * suid) [include/linux/syscalls.h:229]
    {
      118, "getresuid",
      {
        { typeid( uid_t * ), ARGUMENT_DIR_IN, 0 }, /* ruid */
        { typeid( uid_t * ), ARGUMENT_DIR_IN, 0 }, /* euid */
        { typeid( uid_t * ), ARGUMENT_DIR_IN, 0 }, /* suid */
      },
      0, true
    },

    // sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid) [include/linux/syscalls.h:241]
    {
      119, "setresgid",
      {
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* rgid */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* egid */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* sgid */
      },
      0, true
    },

    // sys_getresgid(gid_t * rgid, gid_t * egid, gid_t * sgid) [include/linux/syscalls.h:230]
    {
      120, "getresgid",
      {
        { typeid( gid_t * ), ARGUMENT_DIR_IN, 0 }, /* rgid */
        { typeid( gid_t * ), ARGUMENT_DIR_IN, 0 }, /* egid */
        { typeid( gid_t * ), ARGUMENT_DIR_IN, 0 }, /* sgid */
      },
      0, true
    },

    // sys_getpgid(pid_t pid) [include/linux/syscalls.h:231]
    {
      121, "getpgid",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
      },
      0, true
    },

    // sys_setfsuid(uid_t uid) [include/linux/syscalls.h:242]
    {
      122, "setfsuid",
      {
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* uid */
      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_setfsgid(gid_t gid) [include/linux/syscalls.h:243]
    {
      123, "setfsgid",
      {
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* gid */
      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_getsid(pid_t pid) [include/linux/syscalls.h:233]
    {
      124, "getsid",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
      },
      0, true
    },

    // sys_capget(cap_user_header_t header, cap_user_data_t dataptr) [include/linux/syscalls.h:249]
    {
      125, "capget",
      {
        { typeid( cap_user_header_t ), ARGUMENT_DIR_IN, 0 }, /* header */
        { typeid( cap_user_data_t ), ARGUMENT_DIR_IN, 0 }, /* dataptr */
      },
      0, true
    },

    // sys_capset(cap_user_header_t header, const cap_user_data_t data) [include/linux/syscalls.h:251]
    {
      126, "capset",
      {
        { typeid( cap_user_header_t ), ARGUMENT_DIR_IN, 0 }, /* header */
        { typeid( const cap_user_data_t ), ARGUMENT_DIR_IN, 0 }, /* data */
      },
      0, true
    },

    // sys_rt_sigpending(sigset_t * set, size_t sigsetsize) [include/linux/syscalls.h:367]
    {
      127, "rt_sigpending",
      {
        { typeid( sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* set */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sigsetsize */
      },
      TRACE_SIGNAL, true
    },

    // sys_rt_sigtimedwait(const sigset_t * uthese, siginfo_t * uinfo, const struct timespec * uts, size_t sigsetsize) [include/linux/syscalls.h:368]
    {
      128, "rt_sigtimedwait",
      {
        { typeid( const sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* uthese */
        { typeid( siginfo_t * ), ARGUMENT_DIR_IN, 0 }, /* uinfo */
        { typeid( const struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* uts */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sigsetsize */
      },
      TRACE_SIGNAL, true
    },

    // sys_rt_sigqueueinfo(pid_t pid, int sig, siginfo_t * uinfo) [include/linux/syscalls.h:377]
    {
      129, "rt_sigqueueinfo",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* sig */
        { typeid( siginfo_t * ), ARGUMENT_DIR_IN, 0 }, /* uinfo */
      },
      TRACE_SIGNAL, true
    },

    // sys_rt_sigsuspend(sigset_t * unewset, size_t sigsetsize) [include/linux/syscalls.h:352]
    {
      130, "rt_sigsuspend",
      {
        { typeid( sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* unewset */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sigsetsize */
      },
      TRACE_SIGNAL, true
    },

    // sys_sigaltstack(const struct sigaltstack * uss, struct sigaltstack * uoss) [include/linux/syscalls.h:258]
    {
      131, "sigaltstack",
      {
        { typeid( const struct sigaltstack * ), ARGUMENT_DIR_IN, 0 }, /* uss */
        { typeid( struct sigaltstack * ), ARGUMENT_DIR_IN, 0 }, /* uoss */
      },
      TRACE_SIGNAL, true
    },

    // sys_utime(char * filename, struct utimbuf * times) [include/linux/syscalls.h:553]
    {
      132, "utime",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct utimbuf * ), ARGUMENT_DIR_IN, 0 }, /* times */
      },
      TRACE_FILE, true
    },

    // sys_mknod(const char * filename, mode_t mode, unsigned dev) [include/linux/syscalls.h:474]
    {
      133, "mknod",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* dev */
      },
      TRACE_FILE, true
    },

    // sys_uselib(const char * library) [include/linux/syscalls.h:708]
    {
      134, "uselib",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* library */
      },
      TRACE_FILE, true
    },

    // sys_personality(unsigned int personality) [include/linux/syscalls.h:253]
    {
      135, "personality",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* personality */
      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_ustat(unsigned dev, struct ustat * ubuf) [include/linux/syscalls.h:412]
    {
      136, "ustat",
      {
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* dev */
        { typeid( struct ustat * ), ARGUMENT_DIR_IN, 0 }, /* ubuf */
      },
      0, true
    },

    // sys_statfs(const char * path, struct statfs * buf) [include/linux/syscalls.h:396]
    {
      137, "statfs",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( struct statfs * ), ARGUMENT_DIR_IN, 0 }, /* buf */
      },
      TRACE_FILE | TRACE_STATFS, true
    },

    // sys_fstatfs(unsigned int fd, struct statfs * buf) [include/linux/syscalls.h:400]
    {
      138, "fstatfs",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct statfs * ), ARGUMENT_DIR_IN, 0 }, /* buf */
      },
      TRACE_DESC, true
    },

    // sys_sysfs(int option, unsigned long arg1, unsigned long arg2) [include/linux/syscalls.h:705]
    {
      139, "sysfs",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* option */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg1 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg2 */
      },
      0, true
    },

    // sys_getpriority(int which, int who) [include/linux/syscalls.h:312]
    {
      140, "getpriority",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* who */
      },
      0, true
    },

    // sys_setpriority(int which, int who, int niceval) [include/linux/syscalls.h:311]
    {
      141, "setpriority",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* who */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* niceval */
      },
      0, true
    },

    // sys_sched_setparam(pid_t pid, struct sched_param * param) [include/linux/syscalls.h:290]
    {
      142, "sched_setparam",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct sched_param * ), ARGUMENT_DIR_IN, 0 }, /* param */
      },
      0, true
    },

    // sys_sched_getparam(pid_t pid, struct sched_param * param) [include/linux/syscalls.h:296]
    {
      143, "sched_getparam",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct sched_param * ), ARGUMENT_DIR_IN, 0 }, /* param */
      },
      0, true
    },

    // sys_sched_setscheduler(pid_t pid, int policy, struct sched_param * param) [include/linux/syscalls.h:288]
    {
      144, "sched_setscheduler",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* policy */
        { typeid( struct sched_param * ), ARGUMENT_DIR_IN, 0 }, /* param */
      },
      0, true
    },

    // sys_sched_getscheduler(pid_t pid) [include/linux/syscalls.h:295]
    {
      145, "sched_getscheduler",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
      },
      0, true
    },

    // sys_sched_get_priority_max(int policy) [include/linux/syscalls.h:307]
    {
      146, "sched_get_priority_max",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* policy */
      },
      0, true
    },

    // sys_sched_get_priority_min(int policy) [include/linux/syscalls.h:308]
    {
      147, "sched_get_priority_min",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* policy */
      },
      0, true
    },

    // sys_sched_rr_get_interval(pid_t pid, struct timespec * interval) [include/linux/syscalls.h:309]
    {
      148, "sched_rr_get_interval",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* interval */
      },
      0, true
    },

    // sys_mlock(unsigned long start, size_t len) [include/linux/syscalls.h:463]
    {
      149, "mlock",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      TRACE_MEMORY, true
    },

    // sys_munlock(unsigned long start, size_t len) [include/linux/syscalls.h:464]
    {
      150, "munlock",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      TRACE_MEMORY, true
    },

    // sys_mlockall(int flags) [include/linux/syscalls.h:465]
    {
      151, "mlockall",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY, true
    },

    // sys_munlockall() [include/linux/syscalls.h:466]
    {
      152, "munlockall",
      {

      },
      TRACE_MEMORY, true
    },

    // sys_vhangup() [include/linux/syscalls.h:521]
    {
      153, "vhangup",
      {

      },
      0, true
    },

    // sys_modify_ldt(int unnamed, void * unnamed, unsigned long) [arch/x86/include/asm/syscalls.h:24]
    {
      154, "modify_ldt",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
      },
      0, true
    },

    // sys_pivot_root(const char * new_root, const char * put_old) [include/linux/syscalls.h:471]
    {
      155, "pivot_root",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* new_root */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* put_old */
      },
      TRACE_FILE, true
    },

    // sys__sysctl(?) [?]
    {
      156, "_sysctl",
      {

      },
      0, false
    },

    // sys_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) [include/linux/syscalls.h:699]
    {
      157, "prctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* option */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg2 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg3 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg4 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg5 */
      },
      0, true
    },

    // sys_arch_prctl(int unnamed, unsigned long) [arch/x86/include/asm/syscalls.h:48]
    {
      158, "arch_prctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
      },
      TRACE_PROCESS, true
    },

    // sys_adjtimex(struct timex * txc_p) [include/linux/syscalls.h:216]
    {
      159, "adjtimex",
      {
        { typeid( struct timex * ), ARGUMENT_DIR_IN, 0 }, /* txc_p */
      },
      0, true
    },

    // sys_setrlimit(unsigned int resource, struct rlimit * rlim) [include/linux/syscalls.h:655]
    {
      160, "setrlimit",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* resource */
        { typeid( struct rlimit * ), ARGUMENT_DIR_IN, 0 }, /* rlim */
      },
      0, true
    },

    // sys_chroot(const char * filename) [include/linux/syscalls.h:473]
    {
      161, "chroot",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
      },
      TRACE_FILE, true
    },

    // sys_sync() [include/linux/syscalls.h:383]
    {
      162, "sync",
      {

      },
      0, true
    },

    // sys_acct(const char * name) [include/linux/syscalls.h:248]
    {
      163, "acct",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
      },
      TRACE_FILE, true
    },

    // sys_settimeofday(struct timeval * tv, struct timezone * tz) [include/linux/syscalls.h:214]
    {
      164, "settimeofday",
      {
        { typeid( struct timeval * ), ARGUMENT_DIR_IN, 0 }, /* tv */
        { typeid( struct timezone * ), ARGUMENT_DIR_IN, 0 }, /* tz */
      },
      0, true
    },

    // sys_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data) [include/linux/syscalls.h:387]
    {
      165, "mount",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* dev_name */
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* dir_name */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* type */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* data */
      },
      TRACE_FILE, true
    },

    // sys_umount2(?) [?]
    {
      166, "umount2",
      {

      },
      TRACE_FILE, false
    },

    // sys_swapon(const char * specialfile, int swap_flags) [include/linux/syscalls.h:701]
    {
      167, "swapon",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* specialfile */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* swap_flags */
      },
      TRACE_FILE, true
    },

    // sys_swapoff(const char * specialfile) [include/linux/syscalls.h:702]
    {
      168, "swapoff",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* specialfile */
      },
      TRACE_FILE, true
    },

    // sys_reboot(int magic1, int magic2, unsigned int cmd, void * arg) [include/linux/syscalls.h:315]
    {
      169, "reboot",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* magic1 */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* magic2 */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* arg */
      },
      0, true
    },

    // sys_sethostname(char * name, int len) [include/linux/syscalls.h:644]
    {
      170, "sethostname",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      0, true
    },

    // sys_setdomainname(char * name, int len) [include/linux/syscalls.h:645]
    {
      171, "setdomainname",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      0, true
    },

    // sys_iopl(unsigned int) [arch/x86/include/asm/syscalls.h:21]
    {
      172, "iopl",
      {
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* int */
      },
      0, true
    },

    // sys_ioperm(unsigned long, unsigned long, int unnamed) [arch/x86/include/asm/syscalls.h:20]
    {
      173, "ioperm",
      {
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* long */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      0, true
    },

    // sys_create_module(?) [?]
    {
      174, "create_module",
      {

      },
      0, false
    },

    // sys_init_module(void * umod, unsigned long len, const char * uargs) [include/linux/syscalls.h:339]
    {
      175, "init_module",
      {
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* umod */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* uargs */
      },
      0, true
    },

    // sys_delete_module(const char * name_user, unsigned int flags) [include/linux/syscalls.h:341]
    {
      176, "delete_module",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name_user */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_get_kernel_syms(?) [?]
    {
      177, "get_kernel_syms",
      {

      },
      0, false
    },

    // sys_query_module(?) [?]
    {
      178, "query_module",
      {

      },
      0, false
    },

    // sys_quotactl(unsigned int cmd, const char * special, int id, void * addr) [include/linux/syscalls.h:592]
    {
      179, "quotactl",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* special */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* id */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* addr */
      },
      TRACE_FILE, true
    },

    // sys_nfsservctl(?) [?]
    {
      180, "nfsservctl",
      {

      },
      0, false
    },

    // sys_getpmsg(?) [?]
    {
      181, "getpmsg",
      {

      },
      TRACE_NETWORK, false
    },

    // sys_putpmsg(?) [?]
    {
      182, "putpmsg",
      {

      },
      TRACE_NETWORK, false
    },

    // sys_afs_syscall(?) [?]
    {
      183, "afs_syscall",
      {

      },
      0, false
    },

    // sys_tuxcall(?) [?]
    {
      184, "tuxcall",
      {

      },
      0, false
    },

    // sys_security(?) [?]
    {
      185, "security",
      {

      },
      0, false
    },

    // sys_gettid() [include/linux/syscalls.h:220]
    {
      186, "gettid",
      {

      },
      SYSCALL_NEVER_FAILS, true
    },

    // sys_readahead(int fd, loff_t offset, size_t count) [include/linux/syscalls.h:563]
    {
      187, "readahead",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* offset */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC, true
    },

    // sys_setxattr(const char * path, const char * name, const void * value, size_t size, int flags) [include/linux/syscalls.h:427]
    {
      188, "setxattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( const void * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_FILE, true
    },

    // sys_lsetxattr(const char * path, const char * name, const void * value, size_t size, int flags) [include/linux/syscalls.h:429]
    {
      189, "lsetxattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( const void * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_FILE, true
    },

    // sys_fsetxattr(int fd, const char * name, const void * value, size_t size, int flags) [include/linux/syscalls.h:431]
    {
      190, "fsetxattr",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( const void * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_getxattr(const char * path, const char * name, void * value, size_t size) [include/linux/syscalls.h:433]
    {
      191, "getxattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_FILE, true
    },

    // sys_lgetxattr(const char * path, const char * name, void * value, size_t size) [include/linux/syscalls.h:435]
    {
      192, "lgetxattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_FILE, true
    },

    // sys_fgetxattr(int fd, const char * name, void * value, size_t size) [include/linux/syscalls.h:437]
    {
      193, "fgetxattr",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* value */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_DESC, true
    },

    // sys_listxattr(const char * path, char * list, size_t size) [include/linux/syscalls.h:439]
    {
      194, "listxattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* list */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_FILE, true
    },

    // sys_llistxattr(const char * path, char * list, size_t size) [include/linux/syscalls.h:441]
    {
      195, "llistxattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* list */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_FILE, true
    },

    // sys_flistxattr(int fd, char * list, size_t size) [include/linux/syscalls.h:443]
    {
      196, "flistxattr",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* list */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_DESC, true
    },

    // sys_removexattr(const char * path, const char * name) [include/linux/syscalls.h:444]
    {
      197, "removexattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
      },
      TRACE_FILE, true
    },

    // sys_lremovexattr(const char * path, const char * name) [include/linux/syscalls.h:446]
    {
      198, "lremovexattr",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
      },
      TRACE_FILE, true
    },

    // sys_fremovexattr(int fd, const char * name) [include/linux/syscalls.h:448]
    {
      199, "fremovexattr",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
      },
      TRACE_DESC, true
    },

    // sys_tkill(pid_t pid, int sig) [include/linux/syscalls.h:376]
    {
      200, "tkill",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* sig */
      },
      TRACE_SIGNAL, true
    },

    // sys_time(time_t * tloc) [include/linux/syscalls.h:210]
    {
      201, "time",
      {
        { typeid( time_t * ), ARGUMENT_DIR_IN, 0 }, /* tloc */
      },
      0, true
    },

    // sys_futex(__u32 * uaddr, int op, __u32 val, struct timespec * utime, __u32 * uaddr2, __u32 val3) [include/linux/syscalls.h:335]
    {
      202, "futex",
      {
        { typeid( __u32 * ), ARGUMENT_DIR_IN, 0 }, /* uaddr */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* op */
        { typeid( __u32 ), ARGUMENT_DIR_IN, 0 }, /* val */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* utime */
        { typeid( __u32 * ), ARGUMENT_DIR_IN, 0 }, /* uaddr2 */
        { typeid( __u32 ), ARGUMENT_DIR_IN, 0 }, /* val3 */
      },
      0, true
    },

    // sys_sched_setaffinity(pid_t pid, unsigned int len, unsigned long * user_mask_ptr) [include/linux/syscalls.h:302]
    {
      203, "sched_setaffinity",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* user_mask_ptr */
      },
      0, true
    },

    // sys_sched_getaffinity(pid_t pid, unsigned int len, unsigned long * user_mask_ptr) [include/linux/syscalls.h:304]
    {
      204, "sched_getaffinity",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* user_mask_ptr */
      },
      0, true
    },

    // sys_set_thread_area(struct user_desc * unnamed) [arch/x86/include/asm/syscalls.h:30]
    {
      205, "set_thread_area",
      {
        { typeid( struct user_desc * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      0, true
    },

    // sys_io_setup(unsigned nr_reqs, aio_context_t * ctx) [include/linux/syscalls.h:499]
    {
      206, "io_setup",
      {
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* nr_reqs */
        { typeid( aio_context_t * ), ARGUMENT_DIR_IN, 0 }, /* ctx */
      },
      TRACE_MEMORY, true
    },

    // sys_io_destroy(aio_context_t ctx) [include/linux/syscalls.h:500]
    {
      207, "io_destroy",
      {
        { typeid( aio_context_t ), ARGUMENT_DIR_IN, 0 }, /* ctx */
      },
      TRACE_MEMORY, true
    },

    // sys_io_getevents(aio_context_t ctx_id, long min_nr, long nr, struct io_event * events, struct timespec * timeout) [include/linux/syscalls.h:501]
    {
      208, "io_getevents",
      {
        { typeid( aio_context_t ), ARGUMENT_DIR_IN, 0 }, /* ctx_id */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* min_nr */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* nr */
        { typeid( struct io_event * ), ARGUMENT_DIR_IN, 0 }, /* events */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* timeout */
      },
      0, true
    },

    // sys_io_submit(aio_context_t unnamed, long unnamed, struct iocb * * unnamed) [include/linux/syscalls.h:506]
    {
      209, "io_submit",
      {
        { typeid( aio_context_t ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( long ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct iocb * * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      0, true
    },

    // sys_io_cancel(aio_context_t ctx_id, struct iocb * iocb, struct io_event * result) [include/linux/syscalls.h:508]
    {
      210, "io_cancel",
      {
        { typeid( aio_context_t ), ARGUMENT_DIR_IN, 0 }, /* ctx_id */
        { typeid( struct iocb * ), ARGUMENT_DIR_IN, 0 }, /* iocb */
        { typeid( struct io_event * ), ARGUMENT_DIR_IN, 0 }, /* result */
      },
      0, true
    },

    // sys_get_thread_area(struct user_desc * unnamed) [arch/x86/include/asm/syscalls.h:31]
    {
      211, "get_thread_area",
      {
        { typeid( struct user_desc * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      0, true
    },

    // sys_lookup_dcookie(__u64 cookie64, char * buf, size_t len) [include/linux/syscalls.h:591]
    {
      212, "lookup_dcookie",
      {
        { typeid( __u64 ), ARGUMENT_DIR_IN, 0 }, /* cookie64 */
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      0, true
    },

    // sys_epoll_create(int size) [include/linux/syscalls.h:633]
    {
      213, "epoll_create",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_DESC, true
    },

    // sys_epoll_ctl_old(?) [?]
    {
      214, "epoll_ctl_old",
      {

      },
      0, false
    },

    // sys_epoll_wait_old(?) [?]
    {
      215, "epoll_wait_old",
      {

      },
      0, false
    },

    // sys_remap_file_pages(unsigned long start, unsigned long size, unsigned long prot, unsigned long pgoff, unsigned long flags) [include/linux/syscalls.h:456]
    {
      216, "remap_file_pages",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* size */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* prot */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pgoff */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_getdents64(unsigned int fd, struct linux_dirent64 * dirent, unsigned int count) [include/linux/syscalls.h:597]
    {
      217, "getdents64",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct linux_dirent64 * ), ARGUMENT_DIR_IN, 0 }, /* dirent */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC, true
    },

    // sys_set_tid_address(int * tidptr) [include/linux/syscalls.h:334]
    {
      218, "set_tid_address",
      {
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* tidptr */
      },
      0, true
    },

    // sys_restart_syscall() [include/linux/syscalls.h:317]
    {
      219, "restart_syscall",
      {

      },
      0, true
    },

    // sys_semtimedop(int semid, struct sembuf * sops, unsigned nsops, const struct timespec * timeout) [include/linux/syscalls.h:674]
    {
      220, "semtimedop",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* semid */
        { typeid( struct sembuf * ), ARGUMENT_DIR_IN, 0 }, /* sops */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* nsops */
        { typeid( const struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* timeout */
      },
      TRACE_IPC, true
    },

    // sys_fadvise64(int fd, loff_t offset, size_t len, int advice) [include/linux/syscalls.h:460]
    {
      221, "fadvise64",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* offset */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* advice */
      },
      TRACE_DESC, true
    },

    // sys_timer_create(clockid_t which_clock, struct sigevent * timer_event_spec, timer_t * created_timer_id) [include/linux/syscalls.h:265]
    {
      222, "timer_create",
      {
        { typeid( clockid_t ), ARGUMENT_DIR_IN, 0 }, /* which_clock */
        { typeid( struct sigevent * ), ARGUMENT_DIR_IN, 0 }, /* timer_event_spec */
        { typeid( timer_t * ), ARGUMENT_DIR_IN, 0 }, /* created_timer_id */
      },
      0, true
    },

    // sys_timer_settime(timer_t timer_id, int flags, const struct itimerspec * new_setting, struct itimerspec * old_setting) [include/linux/syscalls.h:271]
    {
      223, "timer_settime",
      {
        { typeid( timer_t ), ARGUMENT_DIR_IN, 0 }, /* timer_id */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( const struct itimerspec * ), ARGUMENT_DIR_IN, 0 }, /* new_setting */
        { typeid( struct itimerspec * ), ARGUMENT_DIR_IN, 0 }, /* old_setting */
      },
      0, true
    },

    // sys_timer_gettime(timer_t timer_id, struct itimerspec * setting) [include/linux/syscalls.h:268]
    {
      224, "timer_gettime",
      {
        { typeid( timer_t ), ARGUMENT_DIR_IN, 0 }, /* timer_id */
        { typeid( struct itimerspec * ), ARGUMENT_DIR_IN, 0 }, /* setting */
      },
      0, true
    },

    // sys_timer_getoverrun(timer_t timer_id) [include/linux/syscalls.h:270]
    {
      225, "timer_getoverrun",
      {
        { typeid( timer_t ), ARGUMENT_DIR_IN, 0 }, /* timer_id */
      },
      0, true
    },

    // sys_timer_delete(timer_t timer_id) [include/linux/syscalls.h:274]
    {
      226, "timer_delete",
      {
        { typeid( timer_t ), ARGUMENT_DIR_IN, 0 }, /* timer_id */
      },
      0, true
    },

    // sys_clock_settime(clockid_t which_clock, const struct timespec * tp) [include/linux/syscalls.h:275]
    {
      227, "clock_settime",
      {
        { typeid( clockid_t ), ARGUMENT_DIR_IN, 0 }, /* which_clock */
        { typeid( const struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* tp */
      },
      0, true
    },

    // sys_clock_gettime(clockid_t which_clock, struct timespec * tp) [include/linux/syscalls.h:277]
    {
      228, "clock_gettime",
      {
        { typeid( clockid_t ), ARGUMENT_DIR_IN, 0 }, /* which_clock */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* tp */
      },
      0, true
    },

    // sys_clock_getres(clockid_t which_clock, struct timespec * tp) [include/linux/syscalls.h:281]
    {
      229, "clock_getres",
      {
        { typeid( clockid_t ), ARGUMENT_DIR_IN, 0 }, /* which_clock */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* tp */
      },
      0, true
    },

    // sys_clock_nanosleep(clockid_t which_clock, int flags, const struct timespec * rqtp, struct timespec * rmtp) [include/linux/syscalls.h:283]
    {
      230, "clock_nanosleep",
      {
        { typeid( clockid_t ), ARGUMENT_DIR_IN, 0 }, /* which_clock */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( const struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* rqtp */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* rmtp */
      },
      0, true
    },

    // sys_exit_group(int error_code) [include/linux/syscalls.h:327]
    {
      231, "exit_group",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* error_code */
      },
      TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER, true
    },

    // sys_epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout) [include/linux/syscalls.h:637]
    {
      232, "epoll_wait",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* epfd */
        { typeid( struct epoll_event * ), ARGUMENT_DIR_IN, 0 }, /* events */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* maxevents */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* timeout */
      },
      TRACE_DESC, true
    },

    // sys_epoll_ctl(int epfd, int op, int fd, struct epoll_event * event) [include/linux/syscalls.h:635]
    {
      233, "epoll_ctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* epfd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* op */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct epoll_event * ), ARGUMENT_DIR_IN, 0 }, /* event */
      },
      TRACE_DESC, true
    },

    // sys_tgkill(pid_t tgid, pid_t pid, int sig) [include/linux/syscalls.h:375]
    {
      234, "tgkill",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* tgid */
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* sig */
      },
      TRACE_SIGNAL, true
    },

    // sys_utimes(char * filename, struct timeval * utimes) [include/linux/syscalls.h:555]
    {
      235, "utimes",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct timeval * ), ARGUMENT_DIR_IN, 0 }, /* utimes */
      },
      TRACE_FILE, true
    },

    // sys_vserver(?) [?]
    {
      236, "vserver",
      {

      },
      0, false
    },

    // sys_mbind(unsigned long start, unsigned long len, unsigned long mode, const unsigned long * nmask, unsigned long maxnode, unsigned flags) [include/linux/syscalls.h:739]
    {
      237, "mbind",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* mode */
        { typeid( const unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* nmask */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* maxnode */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY, true
    },

    // sys_set_mempolicy(int mode, const unsigned long * nmask, unsigned long maxnode) [include/linux/syscalls.h:729]
    {
      238, "set_mempolicy",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* mode */
        { typeid( const unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* nmask */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* maxnode */
      },
      TRACE_MEMORY, true
    },

    // sys_get_mempolicy(int * policy, unsigned long * nmask, unsigned long maxnode, unsigned long addr, unsigned long flags) [include/linux/syscalls.h:744]
    {
      239, "get_mempolicy",
      {
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* policy */
        { typeid( unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* nmask */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* maxnode */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* addr */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY, true
    },

    // sys_mq_open(const char * name, int oflag, mode_t mode, struct mq_attr * attr) [include/linux/syscalls.h:684]
    {
      240, "mq_open",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* oflag */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
        { typeid( struct mq_attr * ), ARGUMENT_DIR_IN, 0 }, /* attr */
      },
      0, true
    },

    // sys_mq_unlink(const char * name) [include/linux/syscalls.h:685]
    {
      241, "mq_unlink",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
      },
      0, true
    },

    // sys_mq_timedsend(mqd_t mqdes, const char * msg_ptr, size_t msg_len, unsigned int msg_prio, const struct timespec * abs_timeout) [include/linux/syscalls.h:686]
    {
      242, "mq_timedsend",
      {
        { typeid( mqd_t ), ARGUMENT_DIR_IN, 0 }, /* mqdes */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* msg_ptr */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* msg_len */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* msg_prio */
        { typeid( const struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* abs_timeout */
      },
      0, true
    },

    // sys_mq_timedreceive(mqd_t mqdes, char * msg_ptr, size_t msg_len, unsigned int * msg_prio, const struct timespec * abs_timeout) [include/linux/syscalls.h:687]
    {
      243, "mq_timedreceive",
      {
        { typeid( mqd_t ), ARGUMENT_DIR_IN, 0 }, /* mqdes */
        { typeid( char * ), ARGUMENT_DIR_IN, 0 }, /* msg_ptr */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* msg_len */
        { typeid( unsigned int * ), ARGUMENT_DIR_IN, 0 }, /* msg_prio */
        { typeid( const struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* abs_timeout */
      },
      0, true
    },

    // sys_mq_notify(mqd_t mqdes, const struct sigevent * notification) [include/linux/syscalls.h:688]
    {
      244, "mq_notify",
      {
        { typeid( mqd_t ), ARGUMENT_DIR_IN, 0 }, /* mqdes */
        { typeid( const struct sigevent * ), ARGUMENT_DIR_IN, 0 }, /* notification */
      },
      0, true
    },

    // sys_mq_getsetattr(mqd_t mqdes, const struct mq_attr * mqstat, struct mq_attr * omqstat) [include/linux/syscalls.h:689]
    {
      245, "mq_getsetattr",
      {
        { typeid( mqd_t ), ARGUMENT_DIR_IN, 0 }, /* mqdes */
        { typeid( const struct mq_attr * ), ARGUMENT_DIR_IN, 0 }, /* mqstat */
        { typeid( struct mq_attr * ), ARGUMENT_DIR_IN, 0 }, /* omqstat */
      },
      0, true
    },

    // sys_kexec_load(unsigned long entry, unsigned long nr_segments, struct kexec_segment * segments, unsigned long flags) [include/linux/kexec.h:239]
    {
      246, "kexec_load",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* entry */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* nr_segments */
        { typeid( struct kexec_segment * ), ARGUMENT_DIR_IN, 0 }, /* segments */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_waitid(int which, pid_t pid, struct siginfo * infop, int options, struct rusage * ru) [include/linux/syscalls.h:330]
    {
      247, "waitid",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct siginfo * ), ARGUMENT_DIR_IN, 0 }, /* infop */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* options */
        { typeid( struct rusage * ), ARGUMENT_DIR_IN, 0 }, /* ru */
      },
      TRACE_PROCESS, true
    },

    // sys_add_key(const char * _type, const char * _description, const void * _payload, size_t plen, key_serial_t destringid) [include/linux/syscalls.h:713]
    {
      248, "add_key",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* _type */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* _description */
        { typeid( const void * ), ARGUMENT_DIR_IN, 0 }, /* _payload */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* plen */
        { typeid( key_serial_t ), ARGUMENT_DIR_IN, 0 }, /* destringid */
      },
      0, true
    },

    // sys_request_key(const char * _type, const char * _description, const char * _callout_info, key_serial_t destringid) [include/linux/syscalls.h:719]
    {
      249, "request_key",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* _type */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* _description */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* _callout_info */
        { typeid( key_serial_t ), ARGUMENT_DIR_IN, 0 }, /* destringid */
      },
      0, true
    },

    // sys_keyctl(int cmd, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) [include/linux/syscalls.h:724]
    {
      250, "keyctl",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg2 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg3 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg4 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* arg5 */
      },
      0, true
    },

    // sys_ioprio_set(int which, int who, int ioprio) [include/linux/syscalls.h:727]
    {
      251, "ioprio_set",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* who */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* ioprio */
      },
      0, true
    },

    // sys_ioprio_get(int which, int who) [include/linux/syscalls.h:728]
    {
      252, "ioprio_get",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* which */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* who */
      },
      0, true
    },

    // sys_inotify_init() [include/linux/syscalls.h:749]
    {
      253, "inotify_init",
      {

      },
      TRACE_DESC, true
    },

    // sys_inotify_add_watch(int fd, const char * path, __u32 mask) [include/linux/syscalls.h:751]
    {
      254, "inotify_add_watch",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( __u32 ), ARGUMENT_DIR_IN, 0 }, /* mask */
      },
      TRACE_DESC, true
    },

    // sys_inotify_rm_watch(int fd, __s32 wd) [include/linux/syscalls.h:753]
    {
      255, "inotify_rm_watch",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( __s32 ), ARGUMENT_DIR_IN, 0 }, /* wd */
      },
      TRACE_DESC, true
    },

    // sys_migrate_pages(pid_t pid, unsigned long maxnode, const unsigned long * from, const unsigned long * to) [include/linux/syscalls.h:731]
    {
      256, "migrate_pages",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* maxnode */
        { typeid( const unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* from */
        { typeid( const unsigned long * ), ARGUMENT_DIR_IN, 0 }, /* to */
      },
      TRACE_MEMORY, true
    },

    // sys_openat(int dfd, const char * filename, int flags, mode_t mode) [include/linux/syscalls.h:780]
    {
      257, "openat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_mkdirat(int dfd, const char * pathname, mode_t mode) [include/linux/syscalls.h:762]
    {
      258, "mkdirat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_mknodat(int dfd, const char * filename, mode_t mode, unsigned dev) [include/linux/syscalls.h:760]
    {
      259, "mknodat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* dev */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_fchownat(int dfd, const char * filename, uid_t user, gid_t group, int flag) [include/linux/syscalls.h:778]
    {
      260, "fchownat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( uid_t ), ARGUMENT_DIR_IN, 0 }, /* user */
        { typeid( gid_t ), ARGUMENT_DIR_IN, 0 }, /* group */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flag */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_futimesat(int dfd, const char * filename, struct timeval * utimes) [include/linux/syscalls.h:773]
    {
      261, "futimesat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct timeval * ), ARGUMENT_DIR_IN, 0 }, /* utimes */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_newfstatat(int dfd, const char * filename, struct stat * statbuf, int flag) [include/linux/syscalls.h:782]
    {
      262, "newfstatat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct stat * ), ARGUMENT_DIR_IN, 0 }, /* statbuf */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flag */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_unlinkat(int dfd, const char * pathname, int flag) [include/linux/syscalls.h:763]
    {
      263, "unlinkat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flag */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_renameat(int olddfd, const char * oldname, int newdfd, const char * newname) [include/linux/syscalls.h:768]
    {
      264, "renameat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* olddfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* oldname */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* newdfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* newname */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_linkat(int olddfd, const char * oldname, int newdfd, const char * newname, int flags) [include/linux/syscalls.h:766]
    {
      265, "linkat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* olddfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* oldname */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* newdfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* newname */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_symlinkat(const char * oldname, int newdfd, const char * newname) [include/linux/syscalls.h:764]
    {
      266, "symlinkat",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* oldname */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* newdfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* newname */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_readlinkat(int dfd, const char * path, char * buf, int bufsiz) [include/linux/syscalls.h:784]
    {
      267, "readlinkat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* path */
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* bufsiz */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_fchmodat(int dfd, const char * filename, mode_t mode) [include/linux/syscalls.h:776]
    {
      268, "fchmodat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( mode_t ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_faccessat(int dfd, const char * filename, int mode) [include/linux/syscalls.h:775]
    {
      269, "faccessat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* mode */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_pselect6(int n, fd_set * inp, fd_set * outp, fd_set * exp, struct timespec * tsp, void * sig) [arch/xtensa/include/asm/syscall.h:21]
    {
      270, "pselect6",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* n */
        { typeid( fd_set * ), ARGUMENT_DIR_IN, 0 }, /* inp */
        { typeid( fd_set * ), ARGUMENT_DIR_IN, 0 }, /* outp */
        { typeid( fd_set * ), ARGUMENT_DIR_IN, 0 }, /* exp */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* tsp */
        { typeid( void * ), ARGUMENT_DIR_IN, 0 }, /* sig */
      },
      TRACE_DESC, true
    },

    // sys_ppoll(struct pollfd * ufds, unsigned int nfds, struct timespec * tsp, const sigset_t * sigmask, size_t sigsetsize) [arch/xtensa/include/asm/syscall.h:24]
    {
      271, "ppoll",
      {
        { typeid( struct pollfd * ), ARGUMENT_DIR_IN, 0 }, /* ufds */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* nfds */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* tsp */
        { typeid( const sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* sigmask */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sigsetsize */
      },
      TRACE_DESC, true
    },

    // sys_unshare(unsigned long unshare_flags) [include/linux/syscalls.h:788]
    {
      272, "unshare",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* unshare_flags */
      },
      TRACE_PROCESS, true
    },

    // sys_set_robust_list(struct robust_list_head * head, size_t len) [include/linux/syscalls.h:806]
    {
      273, "set_robust_list",
      {
        { typeid( struct robust_list_head * ), ARGUMENT_DIR_IN, 0 }, /* head */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      0, true
    },

    // sys_get_robust_list(int pid, struct robust_list_head * * head_ptr, size_t * len_ptr) [include/linux/syscalls.h:803]
    {
      274, "get_robust_list",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct robust_list_head * * ), ARGUMENT_DIR_IN, 0 }, /* head_ptr */
        { typeid( size_t * ), ARGUMENT_DIR_IN, 0 }, /* len_ptr */
      },
      0, true
    },

    // sys_splice(int fd_in, loff_t * off_in, int fd_out, loff_t * off_out, size_t len, unsigned int flags) [include/linux/syscalls.h:790]
    {
      275, "splice",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd_in */
        { typeid( loff_t * ), ARGUMENT_DIR_IN, 0 }, /* off_in */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd_out */
        { typeid( loff_t * ), ARGUMENT_DIR_IN, 0 }, /* off_out */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_tee(int fdin, int fdout, size_t len, unsigned int flags) [include/linux/syscalls.h:797]
    {
      276, "tee",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fdin */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fdout */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_sync_file_range(int fd, loff_t offset, loff_t nbytes, unsigned int flags) [include/linux/syscalls.h:799]
    {
      277, "sync_file_range",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* offset */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* nbytes */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_vmsplice(int fd, const struct iovec * iov, unsigned long nr_segs, unsigned int flags) [include/linux/syscalls.h:794]
    {
      278, "vmsplice",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* iov */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* nr_segs */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_move_pages(pid_t pid, unsigned long nr_pages, const void * * pages, const int * nodes, int * status, int flags) [include/linux/syscalls.h:734]
    {
      279, "move_pages",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* nr_pages */
        { typeid( const void * * ), ARGUMENT_DIR_IN, 0 }, /* pages */
        { typeid( const int * ), ARGUMENT_DIR_IN, 0 }, /* nodes */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* status */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY, true
    },

    // sys_utimensat(int dfd, const char * filename, struct timespec * utimes, int flags) [include/linux/syscalls.h:786]
    {
      280, "utimensat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* utimes */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_epoll_pwait(int epfd, struct epoll_event * events, int maxevents, int timeout, const sigset_t * sigmask, size_t sigsetsize) [include/linux/syscalls.h:639]
    {
      281, "epoll_pwait",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* epfd */
        { typeid( struct epoll_event * ), ARGUMENT_DIR_IN, 0 }, /* events */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* maxevents */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* timeout */
        { typeid( const sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* sigmask */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sigsetsize */
      },
      TRACE_DESC, true
    },

    // sys_signalfd(int ufd, sigset_t * user_mask, size_t sizemask) [include/linux/syscalls.h:809]
    {
      282, "signalfd",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* ufd */
        { typeid( sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* user_mask */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sizemask */
      },
      TRACE_DESC | TRACE_SIGNAL, true
    },

    // sys_timerfd_create(int clockid, int flags) [include/linux/syscalls.h:811]
    {
      283, "timerfd_create",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* clockid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_eventfd(unsigned int count) [include/linux/syscalls.h:816]
    {
      284, "eventfd",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* count */
      },
      TRACE_DESC, true
    },

    // sys_fallocate(int fd, int mode, loff_t offset, loff_t len) [include/linux/syscalls.h:820]
    {
      285, "fallocate",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* mode */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* offset */
        { typeid( loff_t ), ARGUMENT_DIR_IN, 0 }, /* len */
      },
      TRACE_DESC, true
    },

    // sys_timerfd_settime(int ufd, int flags, const struct itimerspec * utmr, struct itimerspec * otmr) [include/linux/syscalls.h:812]
    {
      286, "timerfd_settime",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* ufd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( const struct itimerspec * ), ARGUMENT_DIR_IN, 0 }, /* utmr */
        { typeid( struct itimerspec * ), ARGUMENT_DIR_IN, 0 }, /* otmr */
      },
      TRACE_DESC, true
    },

    // sys_timerfd_gettime(int ufd, struct itimerspec * otmr) [include/linux/syscalls.h:815]
    {
      287, "timerfd_gettime",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* ufd */
        { typeid( struct itimerspec * ), ARGUMENT_DIR_IN, 0 }, /* otmr */
      },
      TRACE_DESC, true
    },

    // sys_accept4(int unnamed, struct sockaddr * unnamed, int * unnamed, int unnamed) [include/linux/syscalls.h:608]
    {
      288, "accept4",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( struct sockaddr * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* unnamed */
      },
      TRACE_NETWORK, true
    },

    // sys_signalfd4(int ufd, sigset_t * user_mask, size_t sizemask, int flags) [include/linux/syscalls.h:810]
    {
      289, "signalfd4",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* ufd */
        { typeid( sigset_t * ), ARGUMENT_DIR_IN, 0 }, /* user_mask */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* sizemask */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC | TRACE_SIGNAL, true
    },

    // sys_eventfd2(unsigned int count, int flags) [include/linux/syscalls.h:817]
    {
      290, "eventfd2",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* count */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_epoll_create1(int flags) [include/linux/syscalls.h:634]
    {
      291, "epoll_create1",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_dup3(unsigned int oldfd, unsigned int newfd, int flags) [include/linux/syscalls.h:494]
    {
      292, "dup3",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* oldfd */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* newfd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_pipe2(int * fildes, int flags) [include/linux/syscalls.h:491]
    {
      293, "pipe2",
      {
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* fildes */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_inotify_init1(int flags) [include/linux/syscalls.h:750]
    {
      294, "inotify_init1",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_preadv(unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h) [include/linux/syscalls.h:576]
    {
      295, "preadv",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* vec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* vlen */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_l */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_h */
      },
      TRACE_DESC, true
    },

    // sys_pwritev(unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h) [include/linux/syscalls.h:581]
    {
      296, "pwritev",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* vec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* vlen */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_l */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_h */
      },
      TRACE_DESC, true
    },

    // sys_rt_tgsigqueueinfo(pid_t tgid, pid_t pid, int sig, siginfo_t * uinfo) [include/linux/syscalls.h:372]
    {
      297, "rt_tgsigqueueinfo",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* tgid */
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* sig */
        { typeid( siginfo_t * ), ARGUMENT_DIR_IN, 0 }, /* uinfo */
      },
      TRACE_PROCESS | TRACE_SIGNAL, true
    },

    // sys_perf_event_open(struct perf_event_attr * attr_uptr, pid_t pid, int cpu, int group_fd, unsigned long flags) [include/linux/syscalls.h:853]
    {
      298, "perf_event_open",
      {
        { typeid( struct perf_event_attr * ), ARGUMENT_DIR_IN, 0 }, /* attr_uptr */
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cpu */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* group_fd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_recvmmsg(int fd, struct mmsghdr * msg, unsigned int vlen, unsigned flags, struct timespec * timeout) [include/linux/syscalls.h:621]
    {
      299, "recvmmsg",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct mmsghdr * ), ARGUMENT_DIR_IN, 0 }, /* msg */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* vlen */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( struct timespec * ), ARGUMENT_DIR_IN, 0 }, /* timeout */
      },
      TRACE_NETWORK, true
    },

    // sys_fanotify_init(unsigned int flags, unsigned int event_f_flags) [include/linux/syscalls.h:828]
    {
      300, "fanotify_init",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* event_f_flags */
      },
      TRACE_DESC, true
    },

    // sys_fanotify_mark(int fanotify_fd, unsigned int flags, __u64 mask, int fd, const char * pathname) [include/linux/syscalls.h:829]
    {
      301, "fanotify_mark",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fanotify_fd */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( __u64 ), ARGUMENT_DIR_IN, 0 }, /* mask */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_prlimit64(pid_t pid, unsigned int resource, const struct rlimit64 * new_rlim, struct rlimit64 * old_rlim) [include/linux/syscalls.h:657]
    {
      302, "prlimit64",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* resource */
        { typeid( const struct rlimit64 * ), ARGUMENT_DIR_IN, 0 }, /* new_rlim */
        { typeid( struct rlimit64 * ), ARGUMENT_DIR_IN, 0 }, /* old_rlim */
      },
      0, true
    },

    // sys_name_to_handle_at(int dfd, const char * name, struct file_handle * handle, int * mnt_id, int flag) [include/linux/syscalls.h:861]
    {
      303, "name_to_handle_at",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* name */
        { typeid( struct file_handle * ), ARGUMENT_DIR_IN, 0 }, /* handle */
        { typeid( int * ), ARGUMENT_DIR_IN, 0 }, /* mnt_id */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flag */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_open_by_handle_at(int mountdirfd, struct file_handle * handle, int flags) [include/linux/syscalls.h:864]
    {
      304, "open_by_handle_at",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* mountdirfd */
        { typeid( struct file_handle * ), ARGUMENT_DIR_IN, 0 }, /* handle */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_clock_adjtime(clockid_t which_clock, struct timex * tx) [include/linux/syscalls.h:279]
    {
      305, "clock_adjtime",
      {
        { typeid( clockid_t ), ARGUMENT_DIR_IN, 0 }, /* which_clock */
        { typeid( struct timex * ), ARGUMENT_DIR_IN, 0 }, /* tx */
      },
      0, true
    },

    // sys_syncfs(int fd) [include/linux/syscalls.h:832]
    {
      306, "syncfs",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
      },
      TRACE_DESC, true
    },

    // sys_sendmmsg(int fd, struct mmsghdr * msg, unsigned int vlen, unsigned flags) [include/linux/syscalls.h:615]
    {
      307, "sendmmsg",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( struct mmsghdr * ), ARGUMENT_DIR_IN, 0 }, /* msg */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* vlen */
        { typeid( unsigned ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_NETWORK, true
    },

    // sys_setns(int fd, int nstype) [include/linux/syscalls.h:867]
    {
      308, "setns",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* nstype */
      },
      TRACE_DESC, true
    },

    // sys_getcpu(unsigned * cpu, unsigned * node, struct getcpu_cache * cache) [include/linux/syscalls.h:808]
    {
      309, "getcpu",
      {
        { typeid( unsigned * ), ARGUMENT_DIR_IN, 0 }, /* cpu */
        { typeid( unsigned * ), ARGUMENT_DIR_IN, 0 }, /* node */
        { typeid( struct getcpu_cache * ), ARGUMENT_DIR_IN, 0 }, /* cache */
      },
      0, true
    },

    // sys_process_vm_readv(pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags) [include/linux/syscalls.h:868]
    {
      310, "process_vm_readv",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* lvec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* liovcnt */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* rvec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* riovcnt */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_process_vm_writev(pid_t pid, const struct iovec * lvec, unsigned long liovcnt, const struct iovec * rvec, unsigned long riovcnt, unsigned long flags) [include/linux/syscalls.h:874]
    {
      311, "process_vm_writev",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* lvec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* liovcnt */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* rvec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* riovcnt */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_kcmp(pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2) [include/linux/syscalls.h:881]
    {
      312, "kcmp",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid1 */
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid2 */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* type */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* idx1 */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* idx2 */
      },
      0, true
    },

    // sys_finit_module(int fd, const char * uargs, int flags) [include/linux/syscalls.h:883]
    {
      313, "finit_module",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* uargs */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_sched_setattr(pid_t pid, struct sched_attr * attr, unsigned int flags) [include/linux/syscalls.h:292]
    {
      314, "sched_setattr",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct sched_attr * ), ARGUMENT_DIR_IN, 0 }, /* attr */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_sched_getattr(pid_t pid, struct sched_attr * attr, unsigned int size, unsigned int flags) [include/linux/syscalls.h:298]
    {
      315, "sched_getattr",
      {
        { typeid( pid_t ), ARGUMENT_DIR_IN, 0 }, /* pid */
        { typeid( struct sched_attr * ), ARGUMENT_DIR_IN, 0 }, /* attr */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* size */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_renameat2(int olddfd, const char * oldname, int newdfd, const char * newname, unsigned int flags) [include/linux/syscalls.h:770]
    {
      316, "renameat2",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* olddfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* oldname */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* newdfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* newname */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC | TRACE_FILE, true
    },

    // sys_seccomp(unsigned int op, unsigned int flags, const char * uargs) [include/linux/syscalls.h:884]
    {
      317, "seccomp",
      {
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* op */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* uargs */
      },
      0, true
    },

    // sys_getrandom(char * buf, size_t count, unsigned int flags) [include/linux/syscalls.h:886]
    {
      318, "getrandom",
      {
        { typeid( char * ), ARGUMENT_DIR_OUT, 0 }, /* buf */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* count */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_memfd_create(const char * uname_ptr, unsigned int flags) [include/linux/syscalls.h:818]
    {
      319, "memfd_create",
      {
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* uname_ptr */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_kexec_file_load(int kernel_fd, int initrd_fd, unsigned long cmdline_len, const char * cmdline_ptr, unsigned long flags) [include/linux/syscalls.h:321]
    {
      320, "kexec_file_load",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* kernel_fd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* initrd_fd */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* cmdline_len */
        { typeid( const char * ), ARGUMENT_DIR_IN, 0 }, /* cmdline_ptr */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_bpf(int cmd, union bpf_attr * attr, unsigned int size) [include/linux/syscalls.h:888]
    {
      321, "bpf",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( union bpf_attr * ), ARGUMENT_DIR_IN, 0 }, /* attr */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* size */
      },
      TRACE_DESC, true
    },

    // sys_execveat(int dfd, const char * filename, const char * const * argv, const char * const * envp, int flags) [include/linux/syscalls.h:890]
    {
      322, "execveat",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* dfd */
        { typeid( const char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        { typeid( const char * const * ), ARGUMENT_DIR_IN, 0 }, /* argv */
        { typeid( const char * const * ), ARGUMENT_DIR_IN, 0 }, /* envp */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC | TRACE_FILE | TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_userfaultfd(int flags) [include/linux/syscalls.h:819]
    {
      323, "userfaultfd",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_membarrier(int cmd, int flags) [include/linux/syscalls.h:894]
    {
      324, "membarrier",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* cmd */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      0, true
    },

    // sys_mlock2(unsigned long start, size_t len, int flags) [include/linux/syscalls.h:899]
    {
      325, "mlock2",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_MEMORY, true
    },

    // sys_copy_file_range(int fd_in, loff_t * off_in, int fd_out, loff_t * off_out, size_t len, unsigned int flags) [include/linux/syscalls.h:895]
    {
      326, "copy_file_range",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd_in */
        { typeid( loff_t * ), ARGUMENT_DIR_IN, 0 }, /* off_in */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* fd_out */
        { typeid( loff_t * ), ARGUMENT_DIR_IN, 0 }, /* off_out */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_preadv2(unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h, int flags) [include/linux/syscalls.h:578]
    {
      327, "preadv2",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* vec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* vlen */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_l */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_h */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_pwritev2(unsigned long fd, const struct iovec * vec, unsigned long vlen, unsigned long pos_l, unsigned long pos_h, int flags) [include/linux/syscalls.h:583]
    {
      328, "pwritev2",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* fd */
        { typeid( const struct iovec * ), ARGUMENT_DIR_IN, 0 }, /* vec */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* vlen */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_l */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* pos_h */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC, true
    },

    // sys_pkey_mprotect(unsigned long start, size_t len, unsigned long prot, int pkey) [include/linux/syscalls.h:901]
    {
      329, "pkey_mprotect",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* start */
        { typeid( size_t ), ARGUMENT_DIR_IN, 0 }, /* len */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* prot */
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* pkey */
      },
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE, true
    },

    // sys_pkey_alloc(unsigned long flags, unsigned long init_val) [include/linux/syscalls.h:903]
    {
      330, "pkey_alloc",
      {
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* flags */
        { typeid( unsigned long ), ARGUMENT_DIR_IN, 0 }, /* init_val */
      },
      0, true
    },

    // sys_pkey_free(int pkey) [include/linux/syscalls.h:904]
    {
      331, "pkey_free",
      {
        { typeid( int ), ARGUMENT_DIR_IN, 0 }, /* pkey */
      },
      0, true
    },

    // sys_statx(?) [?]
    {
      332, "statx",
      {

      },
      TRACE_DESC | TRACE_FILE, false
    },

  };

  return syscall_signatures[ syscall_no ];
}
