#include "syscall.hh"

const std::map<long, SystemCallSignature> syscall_signatures =
{
  /*{
    0,
    {
      0, "read",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    1,
    {
      1, "write",
      {},
      TRACE_DESC
    }
  },*/
  {
    2,
    {
      2, "open",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { typeid( int ),    ARGUMENT_DIR_IN, 0 }, /* flags */
      },
      TRACE_DESC | TRACE_FILE
    }
  },
  /*{
    3,
    {
      3, "close",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    4,
    {
      4, "stat",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    5,
    {
      5, "fstat",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    6,
    {
      6, "lstat",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    7,
    {
      7, "poll",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    8,
    {
      8, "lseek",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    9,
    {
      9, "mmap",
      {},
      TRACE_DESC | TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    10,
    {
      10, "mprotect",
      {},
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    11,
    {
      11, "munmap",
      {},
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    12,
    {
      12, "brk",
      {},
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    13,
    {
      13, "rt_sigaction",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    14,
    {
      14, "rt_sigprocmask",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    15,
    {
      15, "rt_sigreturn",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    16,
    {
      16, "ioctl",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    17,
    {
      17, "pread64",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    18,
    {
      18, "pwrite64",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    19,
    {
      19, "readv",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    20,
    {
      20, "writev",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    21,
    {
      21, "access",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    22,
    {
      22, "pipe",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    23,
    {
      23, "select",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    24,
    {
      24, "sched_yield",
      {},
      0
    }
  },*/
  /*{
    25,
    {
      25, "mremap",
      {},
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    26,
    {
      26, "msync",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    27,
    {
      27, "mincore",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    28,
    {
      28, "madvise",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    29,
    {
      29, "shmget",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    30,
    {
      30, "shmat",
      {},
      TRACE_IPC | TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    31,
    {
      31, "shmctl",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    32,
    {
      32, "dup",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    33,
    {
      33, "dup2",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    34,
    {
      34, "pause",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    35,
    {
      35, "nanosleep",
      {},
      0
    }
  },*/
  /*{
    36,
    {
      36, "getitimer",
      {},
      0
    }
  },*/
  /*{
    37,
    {
      37, "alarm",
      {},
      0
    }
  },*/
  /*{
    38,
    {
      38, "setitimer",
      {},
      0
    }
  },*/
  /*{
    39,
    {
      39, "getpid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    40,
    {
      40, "sendfile",
      {},
      TRACE_DESC | TRACE_NETWORK
    }
  },*/
  /*{
    41,
    {
      41, "socket",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    42,
    {
      42, "connect",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    43,
    {
      43, "accept",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    44,
    {
      44, "sendto",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    45,
    {
      45, "recvfrom",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    46,
    {
      46, "sendmsg",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    47,
    {
      47, "recvmsg",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    48,
    {
      48, "shutdown",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    49,
    {
      49, "bind",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    50,
    {
      50, "listen",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    51,
    {
      51, "getsockname",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    52,
    {
      52, "getpeername",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    53,
    {
      53, "socketpair",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    54,
    {
      54, "setsockopt",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    55,
    {
      55, "getsockopt",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    56,
    {
      56, "clone",
      {},
      TRACE_PROCESS
    }
  },*/
  /*{
    57,
    {
      57, "fork",
      {},
      TRACE_PROCESS
    }
  },*/
  /*{
    58,
    {
      58, "vfork",
      {},
      TRACE_PROCESS
    }
  },*/
  {
    59,
    {
      59, "execve",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }
      },
      TRACE_FILE | TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER | STACKTRACE_INVALIDATE_CACHE
    }
  },
  /*{
    60,
    {
      60, "exit",
      {},
      TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER
    }
  },*/
  /*{
    61,
    {
      61, "wait4",
      {},
      TRACE_PROCESS
    }
  },*/
  /*{
    62,
    {
      62, "kill",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    63,
    {
      63, "uname",
      {},
      0
    }
  },*/
  /*{
    64,
    {
      64, "semget",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    65,
    {
      65, "semop",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    66,
    {
      66, "semctl",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    67,
    {
      67, "shmdt",
      {},
      TRACE_IPC | TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    68,
    {
      68, "msgget",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    69,
    {
      69, "msgsnd",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    70,
    {
      70, "msgrcv",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    71,
    {
      71, "msgctl",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    72,
    {
      72, "fcntl",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    73,
    {
      73, "flock",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    74,
    {
      74, "fsync",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    75,
    {
      75, "fdatasync",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    76,
    {
      76, "truncate",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    77,
    {
      77, "ftruncate",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    78,
    {
      78, "getdents",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    79,
    {
      79, "getcwd",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    80,
    {
      80, "chdir",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    81,
    {
      81, "fchdir",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    82,
    {
      82, "rename",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    83,
    {
      83, "mkdir",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    84,
    {
      84, "rmdir",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    85,
    {
      85, "creat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    86,
    {
      86, "link",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    87,
    {
      87, "unlink",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    88,
    {
      88, "symlink",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    89,
    {
      89, "readlink",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    90,
    {
      90, "chmod",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    91,
    {
      91, "fchmod",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    92,
    {
      92, "chown",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    93,
    {
      93, "fchown",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    94,
    {
      94, "lchown",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    95,
    {
      95, "umask",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    96,
    {
      96, "gettimeofday",
      {},
      0
    }
  },*/
  /*{
    97,
    {
      97, "getrlimit",
      {},
      0
    }
  },*/
  /*{
    98,
    {
      98, "getrusage",
      {},
      0
    }
  },*/
  /*{
    99,
    {
      99, "sysinfo",
      {},
      0
    }
  },*/
  /*{
    100,
    {
      100, "times",
      {},
      0
    }
  },*/
  /*{
    101,
    {
      101, "ptrace",
      {},
      0
    }
  },*/
  /*{
    102,
    {
      102, "getuid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    103,
    {
      103, "syslog",
      {},
      0
    }
  },*/
  /*{
    104,
    {
      104, "getgid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    105,
    {
      105, "setuid",
      {},
      0
    }
  },*/
  /*{
    106,
    {
      106, "setgid",
      {},
      0
    }
  },*/
  /*{
    107,
    {
      107, "geteuid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    108,
    {
      108, "getegid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    109,
    {
      109, "setpgid",
      {},
      0
    }
  },*/
  /*{
    110,
    {
      110, "getppid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    111,
    {
      111, "getpgrp",
      {},
      0
    }
  },*/
  /*{
    112,
    {
      112, "setsid",
      {},
      0
    }
  },*/
  /*{
    113,
    {
      113, "setreuid",
      {},
      0
    }
  },*/
  /*{
    114,
    {
      114, "setregid",
      {},
      0
    }
  },*/
  /*{
    115,
    {
      115, "getgroups",
      {},
      0
    }
  },*/
  /*{
    116,
    {
      116, "setgroups",
      {},
      0
    }
  },*/
  /*{
    117,
    {
      117, "setresuid",
      {},
      0
    }
  },*/
  /*{
    118,
    {
      118, "getresuid",
      {},
      0
    }
  },*/
  /*{
    119,
    {
      119, "setresgid",
      {},
      0
    }
  },*/
  /*{
    120,
    {
      120, "getresgid",
      {},
      0
    }
  },*/
  /*{
    121,
    {
      121, "getpgid",
      {},
      0
    }
  },*/
  /*{
    122,
    {
      122, "setfsuid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    123,
    {
      123, "setfsgid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    124,
    {
      124, "getsid",
      {},
      0
    }
  },*/
  /*{
    125,
    {
      125, "capget",
      {},
      0
    }
  },*/
  /*{
    126,
    {
      126, "capset",
      {},
      0
    }
  },*/
  /*{
    127,
    {
      127, "rt_sigpending",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    128,
    {
      128, "rt_sigtimedwait",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    129,
    {
      129, "rt_sigqueueinfo",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    130,
    {
      130, "rt_sigsuspend",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    131,
    {
      131, "sigaltstack",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    132,
    {
      132, "utime",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    133,
    {
      133, "mknod",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    134,
    {
      134, "uselib",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    135,
    {
      135, "personality",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    136,
    {
      136, "ustat",
      {},
      0
    }
  },*/
  /*{
    137,
    {
      137, "statfs",
      {},
      TRACE_FILE | TRACE_STATFS
    }
  },*/
  /*{
    138,
    {
      138, "fstatfs",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    139,
    {
      139, "sysfs",
      {},
      0
    }
  },*/
  /*{
    140,
    {
      140, "getpriority",
      {},
      0
    }
  },*/
  /*{
    141,
    {
      141, "setpriority",
      {},
      0
    }
  },*/
  /*{
    142,
    {
      142, "sched_setparam",
      {},
      0
    }
  },*/
  /*{
    143,
    {
      143, "sched_getparam",
      {},
      0
    }
  },*/
  /*{
    144,
    {
      144, "sched_setscheduler",
      {},
      0
    }
  },*/
  /*{
    145,
    {
      145, "sched_getscheduler",
      {},
      0
    }
  },*/
  /*{
    146,
    {
      146, "sched_get_priority_max",
      {},
      0
    }
  },*/
  /*{
    147,
    {
      147, "sched_get_priority_min",
      {},
      0
    }
  },*/
  /*{
    148,
    {
      148, "sched_rr_get_interval",
      {},
      0
    }
  },*/
  /*{
    149,
    {
      149, "mlock",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    150,
    {
      150, "munlock",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    151,
    {
      151, "mlockall",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    152,
    {
      152, "munlockall",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    153,
    {
      153, "vhangup",
      {},
      0
    }
  },*/
  /*{
    154,
    {
      154, "modify_ldt",
      {},
      0
    }
  },*/
  /*{
    155,
    {
      155, "pivot_root",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    156,
    {
      156, "_sysctl",
      {},
      0
    }
  },*/
  /*{
    157,
    {
      157, "prctl",
      {},
      0
    }
  },*/
  /*{
    158,
    {
      158, "arch_prctl",
      {},
      TRACE_PROCESS
    }
  },*/
  /*{
    159,
    {
      159, "adjtimex",
      {},
      0
    }
  },*/
  /*{
    160,
    {
      160, "setrlimit",
      {},
      0
    }
  },*/
  /*{
    161,
    {
      161, "chroot",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    162,
    {
      162, "sync",
      {},
      0
    }
  },*/
  /*{
    163,
    {
      163, "acct",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    164,
    {
      164, "settimeofday",
      {},
      0
    }
  },*/
  /*{
    165,
    {
      165, "mount",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    166,
    {
      166, "umount2",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    167,
    {
      167, "swapon",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    168,
    {
      168, "swapoff",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    169,
    {
      169, "reboot",
      {},
      0
    }
  },*/
  /*{
    170,
    {
      170, "sethostname",
      {},
      0
    }
  },*/
  /*{
    171,
    {
      171, "setdomainname",
      {},
      0
    }
  },*/
  /*{
    172,
    {
      172, "iopl",
      {},
      0
    }
  },*/
  /*{
    173,
    {
      173, "ioperm",
      {},
      0
    }
  },*/
  /*{
    174,
    {
      174, "create_module",
      {},
      0
    }
  },*/
  /*{
    175,
    {
      175, "init_module",
      {},
      0
    }
  },*/
  /*{
    176,
    {
      176, "delete_module",
      {},
      0
    }
  },*/
  /*{
    177,
    {
      177, "get_kernel_syms",
      {},
      0
    }
  },*/
  /*{
    178,
    {
      178, "query_module",
      {},
      0
    }
  },*/
  /*{
    179,
    {
      179, "quotactl",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    180,
    {
      180, "nfsservctl",
      {},
      0
    }
  },*/
  /*{
    181,
    {
      181, "getpmsg",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    182,
    {
      182, "putpmsg",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    183,
    {
      183, "afs_syscall",
      {},
      0
    }
  },*/
  /*{
    184,
    {
      184, "tuxcall",
      {},
      0
    }
  },*/
  /*{
    185,
    {
      185, "security",
      {},
      0
    }
  },*/
  /*{
    186,
    {
      186, "gettid",
      {},
      SYSCALL_NEVER_FAILS
    }
  },*/
  /*{
    187,
    {
      187, "readahead",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    188,
    {
      188, "setxattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    189,
    {
      189, "lsetxattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    190,
    {
      190, "fsetxattr",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    191,
    {
      191, "getxattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    192,
    {
      192, "lgetxattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    193,
    {
      193, "fgetxattr",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    194,
    {
      194, "listxattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    195,
    {
      195, "llistxattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    196,
    {
      196, "flistxattr",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    197,
    {
      197, "removexattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    198,
    {
      198, "lremovexattr",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    199,
    {
      199, "fremovexattr",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    200,
    {
      200, "tkill",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    201,
    {
      201, "time",
      {},
      0
    }
  },*/
  /*{
    202,
    {
      202, "futex",
      {},
      0
    }
  },*/
  /*{
    203,
    {
      203, "sched_setaffinity",
      {},
      0
    }
  },*/
  /*{
    204,
    {
      204, "sched_getaffinity",
      {},
      0
    }
  },*/
  /*{
    205,
    {
      205, "set_thread_area",
      {},
      0
    }
  },*/
  /*{
    206,
    {
      206, "io_setup",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    207,
    {
      207, "io_destroy",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    208,
    {
      208, "io_getevents",
      {},
      0
    }
  },*/
  /*{
    209,
    {
      209, "io_submit",
      {},
      0
    }
  },*/
  /*{
    210,
    {
      210, "io_cancel",
      {},
      0
    }
  },*/
  /*{
    211,
    {
      211, "get_thread_area",
      {},
      0
    }
  },*/
  /*{
    212,
    {
      212, "lookup_dcookie",
      {},
      0
    }
  },*/
  /*{
    213,
    {
      213, "epoll_create",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    214,
    {
      214, "epoll_ctl_old",
      {},
      0
    }
  },*/
  /*{
    215,
    {
      215, "epoll_wait_old",
      {},
      0
    }
  },*/
  /*{
    216,
    {
      216, "remap_file_pages",
      {},
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    217,
    {
      217, "getdents64",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    218,
    {
      218, "set_tid_address",
      {},
      0
    }
  },*/
  /*{
    219,
    {
      219, "restart_syscall",
      {},
      0
    }
  },*/
  /*{
    220,
    {
      220, "semtimedop",
      {},
      TRACE_IPC
    }
  },*/
  /*{
    221,
    {
      221, "fadvise64",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    222,
    {
      222, "timer_create",
      {},
      0
    }
  },*/
  /*{
    223,
    {
      223, "timer_settime",
      {},
      0
    }
  },*/
  /*{
    224,
    {
      224, "timer_gettime",
      {},
      0
    }
  },*/
  /*{
    225,
    {
      225, "timer_getoverrun",
      {},
      0
    }
  },*/
  /*{
    226,
    {
      226, "timer_delete",
      {},
      0
    }
  },*/
  /*{
    227,
    {
      227, "clock_settime",
      {},
      0
    }
  },*/
  /*{
    228,
    {
      228, "clock_gettime",
      {},
      0
    }
  },*/
  /*{
    229,
    {
      229, "clock_getres",
      {},
      0
    }
  },*/
  /*{
    230,
    {
      230, "clock_nanosleep",
      {},
      0
    }
  },*/
  /*{
    231,
    {
      231, "exit_group",
      {},
      TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER
    }
  },*/
  /*{
    232,
    {
      232, "epoll_wait",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    233,
    {
      233, "epoll_ctl",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    234,
    {
      234, "tgkill",
      {},
      TRACE_SIGNAL
    }
  },*/
  /*{
    235,
    {
      235, "utimes",
      {},
      TRACE_FILE
    }
  },*/
  /*{
    236,
    {
      236, "vserver",
      {},
      0
    }
  },*/
  /*{
    237,
    {
      237, "mbind",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    238,
    {
      238, "set_mempolicy",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    239,
    {
      239, "get_mempolicy",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    240,
    {
      240, "mq_open",
      {},
      0
    }
  },*/
  /*{
    241,
    {
      241, "mq_unlink",
      {},
      0
    }
  },*/
  /*{
    242,
    {
      242, "mq_timedsend",
      {},
      0
    }
  },*/
  /*{
    243,
    {
      243, "mq_timedreceive",
      {},
      0
    }
  },*/
  /*{
    244,
    {
      244, "mq_notify",
      {},
      0
    }
  },*/
  /*{
    245,
    {
      245, "mq_getsetattr",
      {},
      0
    }
  },*/
  /*{
    246,
    {
      246, "kexec_load",
      {},
      0
    }
  },*/
  /*{
    247,
    {
      247, "waitid",
      {},
      TRACE_PROCESS
    }
  },*/
  /*{
    248,
    {
      248, "add_key",
      {},
      0
    }
  },*/
  /*{
    249,
    {
      249, "request_key",
      {},
      0
    }
  },*/
  /*{
    250,
    {
      250, "keyctl",
      {},
      0
    }
  },*/
  /*{
    251,
    {
      251, "ioprio_set",
      {},
      0
    }
  },*/
  /*{
    252,
    {
      252, "ioprio_get",
      {},
      0
    }
  },*/
  /*{
    253,
    {
      253, "inotify_init",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    254,
    {
      254, "inotify_add_watch",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    255,
    {
      255, "inotify_rm_watch",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    256,
    {
      256, "migrate_pages",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    257,
    {
      257, "openat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    258,
    {
      258, "mkdirat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    259,
    {
      259, "mknodat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    260,
    {
      260, "fchownat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    261,
    {
      261, "futimesat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    262,
    {
      262, "newfstatat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    263,
    {
      263, "unlinkat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    264,
    {
      264, "renameat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    265,
    {
      265, "linkat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    266,
    {
      266, "symlinkat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    267,
    {
      267, "readlinkat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    268,
    {
      268, "fchmodat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    269,
    {
      269, "faccessat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    270,
    {
      270, "pselect6",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    271,
    {
      271, "ppoll",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    272,
    {
      272, "unshare",
      {},
      TRACE_PROCESS
    }
  },*/
  /*{
    273,
    {
      273, "set_robust_list",
      {},
      0
    }
  },*/
  /*{
    274,
    {
      274, "get_robust_list",
      {},
      0
    }
  },*/
  /*{
    275,
    {
      275, "splice",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    276,
    {
      276, "tee",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    277,
    {
      277, "sync_file_range",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    278,
    {
      278, "vmsplice",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    279,
    {
      279, "move_pages",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    280,
    {
      280, "utimensat",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    281,
    {
      281, "epoll_pwait",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    282,
    {
      282, "signalfd",
      {},
      TRACE_DESC | TRACE_SIGNAL
    }
  },*/
  /*{
    283,
    {
      283, "timerfd_create",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    284,
    {
      284, "eventfd",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    285,
    {
      285, "fallocate",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    286,
    {
      286, "timerfd_settime",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    287,
    {
      287, "timerfd_gettime",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    288,
    {
      288, "accept4",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    289,
    {
      289, "signalfd4",
      {},
      TRACE_DESC | TRACE_SIGNAL
    }
  },*/
  /*{
    290,
    {
      290, "eventfd2",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    291,
    {
      291, "epoll_create1",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    292,
    {
      292, "dup3",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    293,
    {
      293, "pipe2",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    294,
    {
      294, "inotify_init1",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    295,
    {
      295, "preadv",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    296,
    {
      296, "pwritev",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    297,
    {
      297, "rt_tgsigqueueinfo",
      {},
      TRACE_PROCESS | TRACE_SIGNAL
    }
  },*/
  /*{
    298,
    {
      298, "perf_event_open",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    299,
    {
      299, "recvmmsg",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    300,
    {
      300, "fanotify_init",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    301,
    {
      301, "fanotify_mark",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    302,
    {
      302, "prlimit64",
      {},
      0
    }
  },*/
  /*{
    303,
    {
      303, "name_to_handle_at",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    304,
    {
      304, "open_by_handle_at",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    305,
    {
      305, "clock_adjtime",
      {},
      0
    }
  },*/
  /*{
    306,
    {
      306, "syncfs",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    307,
    {
      307, "sendmmsg",
      {},
      TRACE_NETWORK
    }
  },*/
  /*{
    308,
    {
      308, "setns",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    309,
    {
      309, "getcpu",
      {},
      0
    }
  },*/
  /*{
    310,
    {
      310, "process_vm_readv",
      {},
      0
    }
  },*/
  /*{
    311,
    {
      311, "process_vm_writev",
      {},
      0
    }
  },*/
  /*{
    312,
    {
      312, "kcmp",
      {},
      0
    }
  },*/
  /*{
    313,
    {
      313, "finit_module",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    314,
    {
      314, "sched_setattr",
      {},
      0
    }
  },*/
  /*{
    315,
    {
      315, "sched_getattr",
      {},
      0
    }
  },*/
  /*{
    316,
    {
      316, "renameat2",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
  /*{
    317,
    {
      317, "seccomp",
      {},
      0
    }
  },*/
  /*{
    318,
    {
      318, "getrandom",
      {},
      0
    }
  },*/
  /*{
    319,
    {
      319, "memfd_create",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    320,
    {
      320, "kexec_file_load",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    321,
    {
      321, "bpf",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    322,
    {
      322, "execveat",
      {},
      TRACE_DESC | TRACE_FILE | TRACE_PROCESS | STACKTRACE_CAPTURE_ON_ENTER | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    323,
    {
      323, "userfaultfd",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    324,
    {
      324, "membarrier",
      {},
      0
    }
  },*/
  /*{
    325,
    {
      325, "mlock2",
      {},
      TRACE_MEMORY
    }
  },*/
  /*{
    326,
    {
      326, "copy_file_range",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    327,
    {
      327, "preadv2",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    328,
    {
      328, "pwritev2",
      {},
      TRACE_DESC
    }
  },*/
  /*{
    329,
    {
      329, "pkey_mprotect",
      {},
      TRACE_MEMORY | STACKTRACE_INVALIDATE_CACHE
    }
  },*/
  /*{
    330,
    {
      330, "pkey_alloc",
      {},
      0
    }
  },*/
  /*{
    331,
    {
      331, "pkey_free",
      {},
      0
    }
  },*/
  /*{
    332,
    {
      332, "statx",
      {},
      TRACE_DESC | TRACE_FILE
    }
  },*/
};
