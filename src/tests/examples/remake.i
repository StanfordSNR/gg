# 1 "remake.c"
# 1 "/home/daniter/workspace/gg-playground/targets/make-4.2//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "remake.c"
# 17 "remake.c"
# 1 "makeint.h" 1
# 20 "makeint.h"
# 1 "./config.h" 1
# 21 "makeint.h" 2
# 31 "makeint.h"
# 1 "/usr/include/alloca.h" 1 3 4
# 21 "/usr/include/alloca.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 374 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 385 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 386 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 375 "/usr/include/features.h" 2 3 4
# 398 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 399 "/usr/include/features.h" 2 3 4
# 22 "/usr/include/alloca.h" 2 3 4


# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 216 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 3 4

# 216 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 25 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__ , __leaf__));






# 32 "makeint.h" 2
# 57 "makeint.h"
# 1 "gnumake.h" 1
# 23 "gnumake.h"

# 23 "gnumake.h"
typedef struct
  {
    const char *filenm;
    unsigned long lineno;
  } gmk_floc;

typedef char *(*gmk_func_ptr)(const char *nm, unsigned int argc, char **argv);
# 42 "gnumake.h"
 void gmk_free (char *str);


 char *gmk_alloc (unsigned int len);


 void gmk_eval (const char *buffer, const gmk_floc *floc);



 char *gmk_expand (const char *str);
# 72 "gnumake.h"
 void gmk_add_function (const char *name, gmk_func_ptr func,
                                  unsigned int min_args, unsigned int max_args,
                                  unsigned int flags);
# 58 "makeint.h" 2
# 70 "makeint.h"
# 1 "/usr/include/x86_64-linux-gnu/sys/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4



# 30 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;







typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
# 121 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/typesizes.h" 1 3 4
# 122 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;
# 30 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;






typedef __ino64_t ino64_t;




typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;






typedef __off64_t off64_t;




typedef __pid_t pid_t;





typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 132 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 57 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



# 73 "/usr/include/time.h" 3 4


typedef __time_t time_t;



# 91 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 103 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 133 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef __useconds_t useconds_t;



typedef __suseconds_t suseconds_t;





# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 147 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 194 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 216 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 36 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 2 3 4
# 60 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 2 3 4






# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap-16.h" 1 3 4
# 36 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 2 3 4
# 44 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return __builtin_bswap32 (__bsx);
}
# 108 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  return __builtin_bswap64 (__bsx);
}
# 61 "/usr/include/endian.h" 2 3 4
# 217 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/select.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/select.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/select.h" 2 3 4
# 31 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 34 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/usr/include/time.h" 1 3 4
# 120 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    __syscall_slong_t tv_nsec;
  };
# 44 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 46 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4
# 54 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef long int __fd_mask;
# 64 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef struct
  {



    __fd_mask fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];





  } fd_set;






typedef __fd_mask fd_mask;
# 96 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 106 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 118 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);
# 131 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 220 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/sysmacros.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/sys/sysmacros.h" 3 4


__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
            unsigned int __minor)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


__extension__ extern __inline __attribute__ ((__gnu_inline__)) __attribute__ ((__const__)) unsigned int
__attribute__ ((__nothrow__ , __leaf__)) gnu_dev_major (unsigned long long int __dev)
{
  return ((__dev >> 8) & 0xfff) | ((unsigned int) (__dev >> 32) & ~0xfff);
}

__extension__ extern __inline __attribute__ ((__gnu_inline__)) __attribute__ ((__const__)) unsigned int
__attribute__ ((__nothrow__ , __leaf__)) gnu_dev_minor (unsigned long long int __dev)
{
  return (__dev & 0xff) | ((unsigned int) (__dev >> 12) & ~0xff);
}

__extension__ extern __inline __attribute__ ((__gnu_inline__)) __attribute__ ((__const__)) unsigned long long int
__attribute__ ((__nothrow__ , __leaf__)) gnu_dev_makedev (unsigned int __major, unsigned int __minor)
{
  return ((__minor & 0xff) | ((__major & 0xfff) << 8)
   | (((unsigned long long int) (__minor & ~0xff)) << 12)
   | (((unsigned long long int) (__major & ~0xfff)) << 32));
}


# 223 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 262 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __blkcnt64_t blkcnt64_t;
typedef __fsblkcnt64_t fsblkcnt64_t;
typedef __fsfilcnt64_t fsfilcnt64_t;





# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 1 3 4
# 21 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 2 3 4
# 60 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


union pthread_attr_t
{
  char __size[56];
  long int __align;
};

typedef union pthread_attr_t pthread_attr_t;





typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
# 90 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;

    unsigned int __nusers;



    int __kind;

    short __spins;
    short __elision;
    __pthread_list_t __list;
# 124 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
  } __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;





typedef union
{

  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
    int __writer;
    int __shared;
    unsigned long int __pad1;
    unsigned long int __pad2;


    unsigned int __flags;

  } __data;
# 211 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 271 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



# 71 "makeint.h" 2
# 1 "/usr/include/x86_64-linux-gnu/sys/stat.h" 1 3 4
# 37 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/sys/stat.h" 2 3 4
# 103 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/stat.h" 1 3 4
# 46 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
struct stat
  {
    __dev_t st_dev;




    __ino_t st_ino;







    __nlink_t st_nlink;
    __mode_t st_mode;

    __uid_t st_uid;
    __gid_t st_gid;

    int __pad0;

    __dev_t st_rdev;




    __off_t st_size;



    __blksize_t st_blksize;

    __blkcnt_t st_blocks;
# 91 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 106 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
    __syscall_slong_t __glibc_reserved[3];
# 115 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
  };



struct stat64
  {
    __dev_t st_dev;

    __ino64_t st_ino;
    __nlink_t st_nlink;
    __mode_t st_mode;






    __uid_t st_uid;
    __gid_t st_gid;

    int __pad0;
    __dev_t st_rdev;
    __off_t st_size;





    __blksize_t st_blksize;
    __blkcnt64_t st_blocks;







    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 164 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
    __syscall_slong_t __glibc_reserved[3];



  };
# 106 "/usr/include/x86_64-linux-gnu/sys/stat.h" 2 3 4
# 209 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int stat (const char *__restrict __file,
   struct stat *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int fstat (int __fd, struct stat *__buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
# 228 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int stat64 (const char *__restrict __file,
     struct stat64 *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int fstat64 (int __fd, struct stat64 *__buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));







extern int fstatat (int __fd, const char *__restrict __file,
      struct stat *__restrict __buf, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
# 253 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int fstatat64 (int __fd, const char *__restrict __file,
        struct stat64 *__restrict __buf, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));







extern int lstat (const char *__restrict __file,
    struct stat *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 276 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int lstat64 (const char *__restrict __file,
      struct stat64 *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern int chmod (const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int lchmod (const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




extern int fchmod (int __fd, __mode_t __mode) __attribute__ ((__nothrow__ , __leaf__));





extern int fchmodat (int __fd, const char *__file, __mode_t __mode,
       int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;






extern __mode_t umask (__mode_t __mask) __attribute__ ((__nothrow__ , __leaf__));




extern __mode_t getumask (void) __attribute__ ((__nothrow__ , __leaf__));



extern int mkdir (const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int mkdirat (int __fd, const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));






extern int mknod (const char *__path, __mode_t __mode, __dev_t __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int mknodat (int __fd, const char *__path, __mode_t __mode,
      __dev_t __dev) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));





extern int mkfifo (const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int mkfifoat (int __fd, const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));





extern int utimensat (int __fd, const char *__path,
        const struct timespec __times[2],
        int __flags)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));




extern int futimens (int __fd, const struct timespec __times[2]) __attribute__ ((__nothrow__ , __leaf__));
# 399 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int __fxstat (int __ver, int __fildes, struct stat *__stat_buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3)));
extern int __xstat (int __ver, const char *__filename,
      struct stat *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat (int __ver, const char *__filename,
       struct stat *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat (int __ver, int __fildes, const char *__filename,
         struct stat *__stat_buf, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4)));
# 432 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int __fxstat64 (int __ver, int __fildes, struct stat64 *__stat_buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3)));
extern int __xstat64 (int __ver, const char *__filename,
        struct stat64 *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat64 (int __ver, const char *__filename,
         struct stat64 *__stat_buf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat64 (int __ver, int __fildes, const char *__filename,
    struct stat64 *__stat_buf, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4)));

extern int __xmknod (int __ver, const char *__path, __mode_t __mode,
       __dev_t *__dev) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));

extern int __xmknodat (int __ver, int __fd, const char *__path,
         __mode_t __mode, __dev_t *__dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 5)));




extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) stat (const char *__path, struct stat *__statbuf)
{
  return __xstat (1, __path, __statbuf);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) lstat (const char *__path, struct stat *__statbuf)
{
  return __lxstat (1, __path, __statbuf);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) fstat (int __fd, struct stat *__statbuf)
{
  return __fxstat (1, __fd, __statbuf);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) fstatat (int __fd, const char *__filename, struct stat *__statbuf, int __flag)

{
  return __fxstatat (1, __fd, __filename, __statbuf, __flag);
}



extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) mknod (const char *__path, __mode_t __mode, __dev_t __dev)
{
  return __xmknod (0, __path, __mode, &__dev);
}



extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) mknodat (int __fd, const char *__path, __mode_t __mode, __dev_t __dev)

{
  return __xmknodat (0, __fd, __path, __mode, &__dev);
}





extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) stat64 (const char *__path, struct stat64 *__statbuf)
{
  return __xstat64 (1, __path, __statbuf);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) lstat64 (const char *__path, struct stat64 *__statbuf)
{
  return __lxstat64 (1, __path, __statbuf);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) fstat64 (int __fd, struct stat64 *__statbuf)
{
  return __fxstat64 (1, __fd, __statbuf);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) fstatat64 (int __fd, const char *__filename, struct stat64 *__statbuf, int __flag)

{
  return __fxstatat64 (1, __fd, __filename, __statbuf, __flag);
}







# 72 "makeint.h" 2
# 1 "/usr/include/signal.h" 1 3 4
# 30 "/usr/include/signal.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 1 3 4
# 102 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 3 4
extern int __sigismember (const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);
# 116 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) int __sigismember (const __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return (__set->__val[__word] & __mask) ? 1 : 0; }
extern __inline __attribute__ ((__gnu_inline__)) int __sigaddset ( __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return ((__set->__val[__word] |= __mask), 0); }
extern __inline __attribute__ ((__gnu_inline__)) int __sigdelset ( __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return ((__set->__val[__word] &= ~__mask), 0); }
# 33 "/usr/include/signal.h" 2 3 4







typedef __sig_atomic_t sig_atomic_t;

# 57 "/usr/include/signal.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/signum.h" 1 3 4
# 58 "/usr/include/signal.h" 2 3 4
# 75 "/usr/include/signal.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 76 "/usr/include/signal.h" 2 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/siginfo.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/siginfo.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/siginfo.h" 2 3 4







typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;
# 58 "/usr/include/x86_64-linux-gnu/bits/siginfo.h" 3 4
typedef __clock_t __sigchld_clock_t;



typedef struct
  {
    int si_signo;
    int si_errno;

    int si_code;

    union
      {
 int _pad[((128 / sizeof (int)) - 4)];


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
   } _kill;


 struct
   {
     int si_tid;
     int si_overrun;
     sigval_t si_sigval;
   } _timer;


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
     sigval_t si_sigval;
   } _rt;


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
     int si_status;
     __sigchld_clock_t si_utime;
     __sigchld_clock_t si_stime;
   } _sigchld;


 struct
   {
     void *si_addr;
     short int si_addr_lsb;
   } _sigfault;


 struct
   {
     long int si_band;
     int si_fd;
   } _sigpoll;


 struct
   {
     void *_call_addr;
     int _syscall;
     unsigned int _arch;
   } _sigsys;
      } _sifields;
  } siginfo_t ;
# 153 "/usr/include/x86_64-linux-gnu/bits/siginfo.h" 3 4
enum
{
  SI_ASYNCNL = -60,

  SI_TKILL = -6,

  SI_SIGIO,

  SI_ASYNCIO,

  SI_MESGQ,

  SI_TIMER,

  SI_QUEUE,

  SI_USER,

  SI_KERNEL = 0x80

};



enum
{
  ILL_ILLOPC = 1,

  ILL_ILLOPN,

  ILL_ILLADR,

  ILL_ILLTRP,

  ILL_PRVOPC,

  ILL_PRVREG,

  ILL_COPROC,

  ILL_BADSTK

};


enum
{
  FPE_INTDIV = 1,

  FPE_INTOVF,

  FPE_FLTDIV,

  FPE_FLTOVF,

  FPE_FLTUND,

  FPE_FLTRES,

  FPE_FLTINV,

  FPE_FLTSUB

};


enum
{
  SEGV_MAPERR = 1,

  SEGV_ACCERR

};


enum
{
  BUS_ADRALN = 1,

  BUS_ADRERR,

  BUS_OBJERR,

  BUS_MCEERR_AR,

  BUS_MCEERR_AO

};


enum
{
  TRAP_BRKPT = 1,

  TRAP_TRACE

};


enum
{
  CLD_EXITED = 1,

  CLD_KILLED,

  CLD_DUMPED,

  CLD_TRAPPED,

  CLD_STOPPED,

  CLD_CONTINUED

};


enum
{
  POLL_IN = 1,

  POLL_OUT,

  POLL_MSG,

  POLL_ERR,

  POLL_PRI,

  POLL_HUP

};
# 307 "/usr/include/x86_64-linux-gnu/bits/siginfo.h" 3 4
typedef struct sigevent
  {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;

    union
      {
 int _pad[((64 / sizeof (int)) - 4)];



 __pid_t _tid;

 struct
   {
     void (*_function) (sigval_t);
     pthread_attr_t *_attribute;
   } _sigev_thread;
      } _sigev_un;
  } sigevent_t;






enum
{
  SIGEV_SIGNAL = 0,

  SIGEV_NONE,

  SIGEV_THREAD,


  SIGEV_THREAD_ID = 4

};
# 81 "/usr/include/signal.h" 2 3 4




typedef void (*__sighandler_t) (int);




extern __sighandler_t __sysv_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__ , __leaf__));

extern __sighandler_t sysv_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__ , __leaf__));







extern __sighandler_t signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__ , __leaf__));
# 114 "/usr/include/signal.h" 3 4





extern __sighandler_t bsd_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__ , __leaf__));






extern int kill (__pid_t __pid, int __sig) __attribute__ ((__nothrow__ , __leaf__));






extern int killpg (__pid_t __pgrp, int __sig) __attribute__ ((__nothrow__ , __leaf__));




extern int raise (int __sig) __attribute__ ((__nothrow__ , __leaf__));




extern __sighandler_t ssignal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__ , __leaf__));
extern int gsignal (int __sig) __attribute__ ((__nothrow__ , __leaf__));




extern void psignal (int __sig, const char *__s);




extern void psiginfo (const siginfo_t *__pinfo, const char *__s);
# 167 "/usr/include/signal.h" 3 4
extern int __sigpause (int __sig_or_mask, int __is_sig);



extern int sigpause (int __sig) __asm__ ("__xpg_sigpause");
# 189 "/usr/include/signal.h" 3 4
extern int sigblock (int __mask) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__deprecated__));


extern int sigsetmask (int __mask) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__deprecated__));


extern int siggetmask (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__deprecated__));
# 204 "/usr/include/signal.h" 3 4
typedef __sighandler_t sighandler_t;




typedef __sighandler_t sig_t;





extern int sigemptyset (sigset_t *__set) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int sigfillset (sigset_t *__set) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int sigaddset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int sigdelset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int sigismember (const sigset_t *__set, int __signo)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern int sigisemptyset (const sigset_t *__set) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int sigandset (sigset_t *__set, const sigset_t *__left,
        const sigset_t *__right) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3)));


extern int sigorset (sigset_t *__set, const sigset_t *__left,
       const sigset_t *__right) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3)));




# 1 "/usr/include/x86_64-linux-gnu/bits/sigaction.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/sigaction.h" 3 4
struct sigaction
  {


    union
      {

 __sighandler_t sa_handler;

 void (*sa_sigaction) (int, siginfo_t *, void *);
      }
    __sigaction_handler;







    __sigset_t sa_mask;


    int sa_flags;


    void (*sa_restorer) (void);
  };
# 246 "/usr/include/signal.h" 2 3 4


extern int sigprocmask (int __how, const sigset_t *__restrict __set,
   sigset_t *__restrict __oset) __attribute__ ((__nothrow__ , __leaf__));






extern int sigsuspend (const sigset_t *__set) __attribute__ ((__nonnull__ (1)));


extern int sigaction (int __sig, const struct sigaction *__restrict __act,
        struct sigaction *__restrict __oact) __attribute__ ((__nothrow__ , __leaf__));


extern int sigpending (sigset_t *__set) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int sigwait (const sigset_t *__restrict __set, int *__restrict __sig)
     __attribute__ ((__nonnull__ (1, 2)));






extern int sigwaitinfo (const sigset_t *__restrict __set,
   siginfo_t *__restrict __info) __attribute__ ((__nonnull__ (1)));






extern int sigtimedwait (const sigset_t *__restrict __set,
    siginfo_t *__restrict __info,
    const struct timespec *__restrict __timeout)
     __attribute__ ((__nonnull__ (1)));



extern int sigqueue (__pid_t __pid, int __sig, const union sigval __val)
     __attribute__ ((__nothrow__ , __leaf__));
# 303 "/usr/include/signal.h" 3 4
extern const char *const _sys_siglist[65];
extern const char *const sys_siglist[65];


struct sigvec
  {
    __sighandler_t sv_handler;
    int sv_mask;

    int sv_flags;

  };
# 327 "/usr/include/signal.h" 3 4
extern int sigvec (int __sig, const struct sigvec *__vec,
     struct sigvec *__ovec) __attribute__ ((__nothrow__ , __leaf__));



# 1 "/usr/include/x86_64-linux-gnu/bits/sigcontext.h" 1 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/sigcontext.h" 3 4
struct _fpx_sw_bytes
{
  __uint32_t magic1;
  __uint32_t extended_size;
  __uint64_t xstate_bv;
  __uint32_t xstate_size;
  __uint32_t padding[7];
};

struct _fpreg
{
  unsigned short significand[4];
  unsigned short exponent;
};

struct _fpxreg
{
  unsigned short significand[4];
  unsigned short exponent;
  unsigned short padding[3];
};

struct _xmmreg
{
  __uint32_t element[4];
};
# 121 "/usr/include/x86_64-linux-gnu/bits/sigcontext.h" 3 4
struct _fpstate
{

  __uint16_t cwd;
  __uint16_t swd;
  __uint16_t ftw;
  __uint16_t fop;
  __uint64_t rip;
  __uint64_t rdp;
  __uint32_t mxcsr;
  __uint32_t mxcr_mask;
  struct _fpxreg _st[8];
  struct _xmmreg _xmm[16];
  __uint32_t padding[24];
};

struct sigcontext
{
  __uint64_t r8;
  __uint64_t r9;
  __uint64_t r10;
  __uint64_t r11;
  __uint64_t r12;
  __uint64_t r13;
  __uint64_t r14;
  __uint64_t r15;
  __uint64_t rdi;
  __uint64_t rsi;
  __uint64_t rbp;
  __uint64_t rbx;
  __uint64_t rdx;
  __uint64_t rax;
  __uint64_t rcx;
  __uint64_t rsp;
  __uint64_t rip;
  __uint64_t eflags;
  unsigned short cs;
  unsigned short gs;
  unsigned short fs;
  unsigned short __pad0;
  __uint64_t err;
  __uint64_t trapno;
  __uint64_t oldmask;
  __uint64_t cr2;
  __extension__ union
    {
      struct _fpstate * fpstate;
      __uint64_t __fpstate_word;
    };
  __uint64_t __reserved1 [8];
};



struct _xsave_hdr
{
  __uint64_t xstate_bv;
  __uint64_t reserved1[2];
  __uint64_t reserved2[5];
};

struct _ymmh_state
{
  __uint32_t ymmh_space[64];
};

struct _xstate
{
  struct _fpstate fpstate;
  struct _xsave_hdr xstate_hdr;
  struct _ymmh_state ymmh;
};
# 333 "/usr/include/signal.h" 2 3 4


extern int sigreturn (struct sigcontext *__scp) __attribute__ ((__nothrow__ , __leaf__));






# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 343 "/usr/include/signal.h" 2 3 4




extern int siginterrupt (int __sig, int __interrupt) __attribute__ ((__nothrow__ , __leaf__));

# 1 "/usr/include/x86_64-linux-gnu/bits/sigstack.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/sigstack.h" 3 4
struct sigstack
  {
    void *ss_sp;
    int ss_onstack;
  };



enum
{
  SS_ONSTACK = 1,

  SS_DISABLE

};
# 49 "/usr/include/x86_64-linux-gnu/bits/sigstack.h" 3 4
typedef struct sigaltstack
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;
# 350 "/usr/include/signal.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/ucontext.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/sys/ucontext.h" 3 4
# 1 "/usr/include/signal.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/sys/ucontext.h" 2 3 4
# 31 "/usr/include/x86_64-linux-gnu/sys/ucontext.h" 3 4
__extension__ typedef long long int greg_t;





typedef greg_t gregset_t[23];



enum
{
  REG_R8 = 0,

  REG_R9,

  REG_R10,

  REG_R11,

  REG_R12,

  REG_R13,

  REG_R14,

  REG_R15,

  REG_RDI,

  REG_RSI,

  REG_RBP,

  REG_RBX,

  REG_RDX,

  REG_RAX,

  REG_RCX,

  REG_RSP,

  REG_RIP,

  REG_EFL,

  REG_CSGSFS,

  REG_ERR,

  REG_TRAPNO,

  REG_OLDMASK,

  REG_CR2

};


struct _libc_fpxreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int padding[3];
};

struct _libc_xmmreg
{
  __uint32_t element[4];
};

struct _libc_fpstate
{

  __uint16_t cwd;
  __uint16_t swd;
  __uint16_t ftw;
  __uint16_t fop;
  __uint64_t rip;
  __uint64_t rdp;
  __uint32_t mxcsr;
  __uint32_t mxcr_mask;
  struct _libc_fpxreg _st[8];
  struct _libc_xmmreg _xmm[16];
  __uint32_t padding[24];
};


typedef struct _libc_fpstate *fpregset_t;


typedef struct
  {
    gregset_t gregs;

    fpregset_t fpregs;
    __extension__ unsigned long long __reserved1 [8];
} mcontext_t;


typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
    struct _libc_fpstate __fpregs_mem;
  } ucontext_t;
# 353 "/usr/include/signal.h" 2 3 4





extern int sigstack (struct sigstack *__ss, struct sigstack *__oss)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__deprecated__));



extern int sigaltstack (const struct sigaltstack *__restrict __ss,
   struct sigaltstack *__restrict __oss) __attribute__ ((__nothrow__ , __leaf__));







extern int sighold (int __sig) __attribute__ ((__nothrow__ , __leaf__));


extern int sigrelse (int __sig) __attribute__ ((__nothrow__ , __leaf__));


extern int sigignore (int __sig) __attribute__ ((__nothrow__ , __leaf__));


extern __sighandler_t sigset (int __sig, __sighandler_t __disp) __attribute__ ((__nothrow__ , __leaf__));






# 1 "/usr/include/x86_64-linux-gnu/bits/sigthread.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/bits/sigthread.h" 3 4
extern int pthread_sigmask (int __how,
       const __sigset_t *__restrict __newmask,
       __sigset_t *__restrict __oldmask)__attribute__ ((__nothrow__ , __leaf__));


extern int pthread_kill (pthread_t __threadid, int __signo) __attribute__ ((__nothrow__ , __leaf__));



extern int pthread_sigqueue (pthread_t __threadid, int __signo,
        const union sigval __value) __attribute__ ((__nothrow__ , __leaf__));
# 389 "/usr/include/signal.h" 2 3 4






extern int __libc_current_sigrtmin (void) __attribute__ ((__nothrow__ , __leaf__));

extern int __libc_current_sigrtmax (void) __attribute__ ((__nothrow__ , __leaf__));




# 73 "makeint.h" 2
# 1 "/usr/include/stdio.h" 1 3 4
# 29 "/usr/include/stdio.h" 3 4




# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 34 "/usr/include/stdio.h" 2 3 4
# 44 "/usr/include/stdio.h" 3 4
struct _IO_FILE;



typedef struct _IO_FILE FILE;





# 64 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 74 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 31 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 15 "/usr/include/_G_config.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 16 "/usr/include/_G_config.h" 2 3 4




# 1 "/usr/include/wchar.h" 1 3 4
# 82 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {

    unsigned int __wch;



    char __wchb[4];
  } __value;
} __mbstate_t;
# 21 "/usr/include/_G_config.h" 2 3 4
typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 32 "/usr/include/libio.h" 2 3 4
# 49 "/usr/include/libio.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stdarg.h" 1 3 4
# 40 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 50 "/usr/include/libio.h" 2 3 4
# 144 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 154 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 177 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 245 "/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 293 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 302 "/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 338 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);




typedef __io_read_fn cookie_read_function_t;
typedef __io_write_fn cookie_write_function_t;
typedef __io_seek_fn cookie_seek_function_t;
typedef __io_close_fn cookie_close_function_t;


typedef struct
{
  __io_read_fn *read;
  __io_write_fn *write;
  __io_seek_fn *seek;
  __io_close_fn *close;
} _IO_cookie_io_functions_t;
typedef _IO_cookie_io_functions_t cookie_io_functions_t;

struct _IO_cookie_file;


extern void _IO_cookie_init (struct _IO_cookie_file *__cfile, int __read_write,
        void *__cookie, _IO_cookie_io_functions_t __fns);







extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
# 434 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__ , __leaf__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
# 464 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
# 75 "/usr/include/stdio.h" 2 3 4




typedef __gnuc_va_list va_list;
# 108 "/usr/include/stdio.h" 3 4


typedef _G_fpos_t fpos_t;





typedef _G_fpos64_t fpos64_t;
# 164 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdio_lim.h" 1 3 4
# 165 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (const char *__filename) __attribute__ ((__nothrow__ , __leaf__));

extern int rename (const char *__old, const char *__new) __attribute__ ((__nothrow__ , __leaf__));




extern int renameat (int __oldfd, const char *__old, int __newfd,
       const char *__new) __attribute__ ((__nothrow__ , __leaf__));








extern FILE *tmpfile (void) ;
# 205 "/usr/include/stdio.h" 3 4
extern FILE *tmpfile64 (void) ;



extern char *tmpnam (char *__s) __attribute__ ((__nothrow__ , __leaf__)) ;





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__ , __leaf__)) ;
# 227 "/usr/include/stdio.h" 3 4
extern char *tempnam (const char *__dir, const char *__pfx)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 252 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 262 "/usr/include/stdio.h" 3 4
extern int fcloseall (void);









extern FILE *fopen (const char *__restrict __filename,
      const char *__restrict __modes) ;




extern FILE *freopen (const char *__restrict __filename,
        const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 295 "/usr/include/stdio.h" 3 4


extern FILE *fopen64 (const char *__restrict __filename,
        const char *__restrict __modes) ;
extern FILE *freopen64 (const char *__restrict __filename,
   const char *__restrict __modes,
   FILE *__restrict __stream) ;




extern FILE *fdopen (int __fd, const char *__modes) __attribute__ ((__nothrow__ , __leaf__)) ;





extern FILE *fopencookie (void *__restrict __magic_cookie,
     const char *__restrict __modes,
     _IO_cookie_io_functions_t __io_funcs) __attribute__ ((__nothrow__ , __leaf__)) ;




extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  __attribute__ ((__nothrow__ , __leaf__)) ;




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__ , __leaf__)) ;






extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__ , __leaf__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__ , __leaf__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));








extern int fprintf (FILE *__restrict __stream,
      const char *__restrict __format, ...);




extern int printf (const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));






extern int vasprintf (char **__restrict __ptr, const char *__restrict __f,
        __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 0))) ;
extern int __asprintf (char **__restrict __ptr,
         const char *__restrict __fmt, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3))) ;
extern int asprintf (char **__restrict __ptr,
       const char *__restrict __fmt, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3))) ;




extern int vdprintf (int __fd, const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));








extern int fscanf (FILE *__restrict __stream,
     const char *__restrict __format, ...) ;




extern int scanf (const char *__restrict __format, ...) ;

extern int sscanf (const char *__restrict __s,
     const char *__restrict __format, ...) __attribute__ ((__nothrow__ , __leaf__));
# 463 "/usr/include/stdio.h" 3 4








extern int vfscanf (FILE *__restrict __s, const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (const char *__restrict __s,
      const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
# 522 "/usr/include/stdio.h" 3 4









extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 550 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 561 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 594 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;
# 640 "/usr/include/stdio.h" 3 4

# 649 "/usr/include/stdio.h" 3 4
extern char *fgets_unlocked (char *__restrict __s, int __n,
        FILE *__restrict __stream) ;
# 665 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
          size_t *__restrict __n, int __delimiter,
          FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
        size_t *__restrict __n, int __delimiter,
        FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
       size_t *__restrict __n,
       FILE *__restrict __stream) ;








extern int fputs (const char *__restrict __s, FILE *__restrict __stream);





extern int puts (const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);

# 726 "/usr/include/stdio.h" 3 4
extern int fputs_unlocked (const char *__restrict __s,
      FILE *__restrict __stream);
# 737 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 773 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 792 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, const fpos_t *__pos);
# 815 "/usr/include/stdio.h" 3 4



extern int fseeko64 (FILE *__stream, __off64_t __off, int __whence);
extern __off64_t ftello64 (FILE *__stream) ;
extern int fgetpos64 (FILE *__restrict __stream, fpos64_t *__restrict __pos);
extern int fsetpos64 (FILE *__stream, const fpos64_t *__pos);




extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;








extern void perror (const char *__s);






# 1 "/usr/include/x86_64-linux-gnu/bits/sys_errlist.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern const char *const sys_errlist[];


extern int _sys_nerr;
extern const char *const _sys_errlist[];
# 854 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
# 873 "/usr/include/stdio.h" 3 4
extern FILE *popen (const char *__command, const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__ , __leaf__));





extern char *cuserid (char *__s);




struct obstack;


extern int obstack_printf (struct obstack *__restrict __obstack,
      const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3)));
extern int obstack_vprintf (struct obstack *__restrict __obstack,
       const char *__restrict __format,
       __gnuc_va_list __args)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 0)));







extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
# 934 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdio.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/stdio.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) int
vprintf (const char *__restrict __fmt, __gnuc_va_list __arg)
{
  return vfprintf (stdout, __fmt, __arg);
}



extern __inline __attribute__ ((__gnu_inline__)) int
getchar (void)
{
  return _IO_getc (stdin);
}




extern __inline __attribute__ ((__gnu_inline__)) int
fgetc_unlocked (FILE *__fp)
{
  return (__builtin_expect (((__fp)->_IO_read_ptr >= (__fp)->_IO_read_end), 0) ? __uflow (__fp) : *(unsigned char *) (__fp)->_IO_read_ptr++);
}





extern __inline __attribute__ ((__gnu_inline__)) int
getc_unlocked (FILE *__fp)
{
  return (__builtin_expect (((__fp)->_IO_read_ptr >= (__fp)->_IO_read_end), 0) ? __uflow (__fp) : *(unsigned char *) (__fp)->_IO_read_ptr++);
}


extern __inline __attribute__ ((__gnu_inline__)) int
getchar_unlocked (void)
{
  return (__builtin_expect (((stdin)->_IO_read_ptr >= (stdin)->_IO_read_end), 0) ? __uflow (stdin) : *(unsigned char *) (stdin)->_IO_read_ptr++);
}




extern __inline __attribute__ ((__gnu_inline__)) int
putchar (int __c)
{
  return _IO_putc (__c, stdout);
}




extern __inline __attribute__ ((__gnu_inline__)) int
fputc_unlocked (int __c, FILE *__stream)
{
  return (__builtin_expect (((__stream)->_IO_write_ptr >= (__stream)->_IO_write_end), 0) ? __overflow (__stream, (unsigned char) (__c)) : (unsigned char) (*(__stream)->_IO_write_ptr++ = (__c)));
}





extern __inline __attribute__ ((__gnu_inline__)) int
putc_unlocked (int __c, FILE *__stream)
{
  return (__builtin_expect (((__stream)->_IO_write_ptr >= (__stream)->_IO_write_end), 0) ? __overflow (__stream, (unsigned char) (__c)) : (unsigned char) (*(__stream)->_IO_write_ptr++ = (__c)));
}


extern __inline __attribute__ ((__gnu_inline__)) int
putchar_unlocked (int __c)
{
  return (__builtin_expect (((stdout)->_IO_write_ptr >= (stdout)->_IO_write_end), 0) ? __overflow (stdout, (unsigned char) (__c)) : (unsigned char) (*(stdout)->_IO_write_ptr++ = (__c)));
}





extern __inline __attribute__ ((__gnu_inline__)) __ssize_t
getline (char **__lineptr, size_t *__n, FILE *__stream)
{
  return __getdelim (__lineptr, __n, '\n', __stream);
}





extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) feof_unlocked (FILE *__stream)
{
  return (((__stream)->_flags & 0x10) != 0);
}


extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) ferror_unlocked (FILE *__stream)
{
  return (((__stream)->_flags & 0x20) != 0);
}
# 935 "/usr/include/stdio.h" 2 3 4
# 943 "/usr/include/stdio.h" 3 4

# 74 "makeint.h" 2
# 1 "/usr/include/ctype.h" 1 3 4
# 28 "/usr/include/ctype.h" 3 4

# 46 "/usr/include/ctype.h" 3 4
enum
{
  _ISupper = ((0) < 8 ? ((1 << (0)) << 8) : ((1 << (0)) >> 8)),
  _ISlower = ((1) < 8 ? ((1 << (1)) << 8) : ((1 << (1)) >> 8)),
  _ISalpha = ((2) < 8 ? ((1 << (2)) << 8) : ((1 << (2)) >> 8)),
  _ISdigit = ((3) < 8 ? ((1 << (3)) << 8) : ((1 << (3)) >> 8)),
  _ISxdigit = ((4) < 8 ? ((1 << (4)) << 8) : ((1 << (4)) >> 8)),
  _ISspace = ((5) < 8 ? ((1 << (5)) << 8) : ((1 << (5)) >> 8)),
  _ISprint = ((6) < 8 ? ((1 << (6)) << 8) : ((1 << (6)) >> 8)),
  _ISgraph = ((7) < 8 ? ((1 << (7)) << 8) : ((1 << (7)) >> 8)),
  _ISblank = ((8) < 8 ? ((1 << (8)) << 8) : ((1 << (8)) >> 8)),
  _IScntrl = ((9) < 8 ? ((1 << (9)) << 8) : ((1 << (9)) >> 8)),
  _ISpunct = ((10) < 8 ? ((1 << (10)) << 8) : ((1 << (10)) >> 8)),
  _ISalnum = ((11) < 8 ? ((1 << (11)) << 8) : ((1 << (11)) >> 8))
};
# 79 "/usr/include/ctype.h" 3 4
extern const unsigned short int **__ctype_b_loc (void)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern const __int32_t **__ctype_tolower_loc (void)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern const __int32_t **__ctype_toupper_loc (void)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 104 "/usr/include/ctype.h" 3 4






extern int isalnum (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isalpha (int) __attribute__ ((__nothrow__ , __leaf__));
extern int iscntrl (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isdigit (int) __attribute__ ((__nothrow__ , __leaf__));
extern int islower (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isgraph (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isprint (int) __attribute__ ((__nothrow__ , __leaf__));
extern int ispunct (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isspace (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isupper (int) __attribute__ ((__nothrow__ , __leaf__));
extern int isxdigit (int) __attribute__ ((__nothrow__ , __leaf__));



extern int tolower (int __c) __attribute__ ((__nothrow__ , __leaf__));


extern int toupper (int __c) __attribute__ ((__nothrow__ , __leaf__));








extern int isblank (int) __attribute__ ((__nothrow__ , __leaf__));






extern int isctype (int __c, int __mask) __attribute__ ((__nothrow__ , __leaf__));






extern int isascii (int __c) __attribute__ ((__nothrow__ , __leaf__));



extern int toascii (int __c) __attribute__ ((__nothrow__ , __leaf__));



extern int _toupper (int) __attribute__ ((__nothrow__ , __leaf__));
extern int _tolower (int) __attribute__ ((__nothrow__ , __leaf__));
# 214 "/usr/include/ctype.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) tolower (int __c)
{
  return __c >= -128 && __c < 256 ? (*__ctype_tolower_loc ())[__c] : __c;
}

extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) toupper (int __c)
{
  return __c >= -128 && __c < 256 ? (*__ctype_toupper_loc ())[__c] : __c;
}
# 257 "/usr/include/ctype.h" 3 4
# 1 "/usr/include/xlocale.h" 1 3 4
# 27 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 258 "/usr/include/ctype.h" 2 3 4
# 271 "/usr/include/ctype.h" 3 4
extern int isalnum_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isalpha_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int iscntrl_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isdigit_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int islower_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isgraph_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isprint_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int ispunct_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isspace_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isupper_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));
extern int isxdigit_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));

extern int isblank_l (int, __locale_t) __attribute__ ((__nothrow__ , __leaf__));



extern int __tolower_l (int __c, __locale_t __l) __attribute__ ((__nothrow__ , __leaf__));
extern int tolower_l (int __c, __locale_t __l) __attribute__ ((__nothrow__ , __leaf__));


extern int __toupper_l (int __c, __locale_t __l) __attribute__ ((__nothrow__ , __leaf__));
extern int toupper_l (int __c, __locale_t __l) __attribute__ ((__nothrow__ , __leaf__));
# 347 "/usr/include/ctype.h" 3 4

# 75 "makeint.h" 2




# 1 "/usr/include/x86_64-linux-gnu/sys/timeb.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/sys/timeb.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/sys/timeb.h" 2 3 4






struct timeb
  {
    time_t time;
    unsigned short int millitm;
    short int timezone;
    short int dstflag;
  };



extern int ftime (struct timeb *__timebuf);


# 80 "makeint.h" 2


# 1 "/usr/include/x86_64-linux-gnu/sys/time.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/sys/time.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/sys/time.h" 2 3 4
# 37 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4

# 55 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4
struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };

typedef struct timezone *__restrict __timezone_ptr_t;
# 71 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4
extern int gettimeofday (struct timeval *__restrict __tv,
    __timezone_ptr_t __tz) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




extern int settimeofday (const struct timeval *__tv,
    const struct timezone *__tz)
     __attribute__ ((__nothrow__ , __leaf__));





extern int adjtime (const struct timeval *__delta,
      struct timeval *__olddelta) __attribute__ ((__nothrow__ , __leaf__));




enum __itimer_which
  {

    ITIMER_REAL = 0,


    ITIMER_VIRTUAL = 1,



    ITIMER_PROF = 2

  };



struct itimerval
  {

    struct timeval it_interval;

    struct timeval it_value;
  };




typedef enum __itimer_which __itimer_which_t;






extern int getitimer (__itimer_which_t __which,
        struct itimerval *__value) __attribute__ ((__nothrow__ , __leaf__));




extern int setitimer (__itimer_which_t __which,
        const struct itimerval *__restrict __new,
        struct itimerval *__restrict __old) __attribute__ ((__nothrow__ , __leaf__));




extern int utimes (const char *__file, const struct timeval __tvp[2])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern int lutimes (const char *__file, const struct timeval __tvp[2])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int futimes (int __fd, const struct timeval __tvp[2]) __attribute__ ((__nothrow__ , __leaf__));






extern int futimesat (int __fd, const char *__file,
        const struct timeval __tvp[2]) __attribute__ ((__nothrow__ , __leaf__));
# 189 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4

# 83 "makeint.h" 2
# 1 "/usr/include/time.h" 1 3 4
# 29 "/usr/include/time.h" 3 4








# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 38 "/usr/include/time.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 86 "/usr/include/x86_64-linux-gnu/bits/time.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/timex.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/timex.h" 3 4
struct timex
{
  unsigned int modes;
  __syscall_slong_t offset;
  __syscall_slong_t freq;
  __syscall_slong_t maxerror;
  __syscall_slong_t esterror;
  int status;
  __syscall_slong_t constant;
  __syscall_slong_t precision;
  __syscall_slong_t tolerance;
  struct timeval time;
  __syscall_slong_t tick;
  __syscall_slong_t ppsfreq;
  __syscall_slong_t jitter;
  int shift;
  __syscall_slong_t stabil;
  __syscall_slong_t jitcnt;
  __syscall_slong_t calcnt;
  __syscall_slong_t errcnt;
  __syscall_slong_t stbcnt;

  int tai;


  int :32; int :32; int :32; int :32;
  int :32; int :32; int :32; int :32;
  int :32; int :32; int :32;
};
# 87 "/usr/include/x86_64-linux-gnu/bits/time.h" 2 3 4




extern int clock_adjtime (__clockid_t __clock_id, struct timex *__utx) __attribute__ ((__nothrow__ , __leaf__));


# 42 "/usr/include/time.h" 2 3 4
# 131 "/usr/include/time.h" 3 4


struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;


  long int tm_gmtoff;
  const char *tm_zone;




};








struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };


struct sigevent;
# 186 "/usr/include/time.h" 3 4



extern clock_t clock (void) __attribute__ ((__nothrow__ , __leaf__));


extern time_t time (time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));


extern double difftime (time_t __time1, time_t __time0)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern time_t mktime (struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));





extern size_t strftime (char *__restrict __s, size_t __maxsize,
   const char *__restrict __format,
   const struct tm *__restrict __tp) __attribute__ ((__nothrow__ , __leaf__));





extern char *strptime (const char *__restrict __s,
         const char *__restrict __fmt, struct tm *__tp)
     __attribute__ ((__nothrow__ , __leaf__));







extern size_t strftime_l (char *__restrict __s, size_t __maxsize,
     const char *__restrict __format,
     const struct tm *__restrict __tp,
     __locale_t __loc) __attribute__ ((__nothrow__ , __leaf__));



extern char *strptime_l (const char *__restrict __s,
    const char *__restrict __fmt, struct tm *__tp,
    __locale_t __loc) __attribute__ ((__nothrow__ , __leaf__));






extern struct tm *gmtime (const time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));



extern struct tm *localtime (const time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));





extern struct tm *gmtime_r (const time_t *__restrict __timer,
       struct tm *__restrict __tp) __attribute__ ((__nothrow__ , __leaf__));



extern struct tm *localtime_r (const time_t *__restrict __timer,
          struct tm *__restrict __tp) __attribute__ ((__nothrow__ , __leaf__));





extern char *asctime (const struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern char *ctime (const time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));







extern char *asctime_r (const struct tm *__restrict __tp,
   char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));


extern char *ctime_r (const time_t *__restrict __timer,
        char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));




extern char *__tzname[2];
extern int __daylight;
extern long int __timezone;




extern char *tzname[2];



extern void tzset (void) __attribute__ ((__nothrow__ , __leaf__));



extern int daylight;
extern long int timezone;





extern int stime (const time_t *__when) __attribute__ ((__nothrow__ , __leaf__));
# 319 "/usr/include/time.h" 3 4
extern time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern int dysize (int __year) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 334 "/usr/include/time.h" 3 4
extern int nanosleep (const struct timespec *__requested_time,
        struct timespec *__remaining);



extern int clock_getres (clockid_t __clock_id, struct timespec *__res) __attribute__ ((__nothrow__ , __leaf__));


extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern int clock_settime (clockid_t __clock_id, const struct timespec *__tp)
     __attribute__ ((__nothrow__ , __leaf__));






extern int clock_nanosleep (clockid_t __clock_id, int __flags,
       const struct timespec *__req,
       struct timespec *__rem);


extern int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) __attribute__ ((__nothrow__ , __leaf__));




extern int timer_create (clockid_t __clock_id,
    struct sigevent *__restrict __evp,
    timer_t *__restrict __timerid) __attribute__ ((__nothrow__ , __leaf__));


extern int timer_delete (timer_t __timerid) __attribute__ ((__nothrow__ , __leaf__));


extern int timer_settime (timer_t __timerid, int __flags,
     const struct itimerspec *__restrict __value,
     struct itimerspec *__restrict __ovalue) __attribute__ ((__nothrow__ , __leaf__));


extern int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     __attribute__ ((__nothrow__ , __leaf__));


extern int timer_getoverrun (timer_t __timerid) __attribute__ ((__nothrow__ , __leaf__));





extern int timespec_get (struct timespec *__ts, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 403 "/usr/include/time.h" 3 4
extern int getdate_err;
# 412 "/usr/include/time.h" 3 4
extern struct tm *getdate (const char *__string);
# 426 "/usr/include/time.h" 3 4
extern int getdate_r (const char *__restrict __string,
        struct tm *__restrict __resbufp);



# 84 "makeint.h" 2
# 92 "makeint.h"
# 1 "/usr/include/errno.h" 1 3 4
# 31 "/usr/include/errno.h" 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/errno.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
# 1 "/usr/include/linux/errno.h" 1 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 1 3 4
# 1 "/usr/include/asm-generic/errno.h" 1 3 4



# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 5 "/usr/include/asm-generic/errno.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 2 3 4
# 1 "/usr/include/linux/errno.h" 2 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/errno.h" 2 3 4
# 50 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 36 "/usr/include/errno.h" 2 3 4
# 54 "/usr/include/errno.h" 3 4
extern char *program_invocation_name, *program_invocation_short_name;




# 68 "/usr/include/errno.h" 3 4
typedef int error_t;
# 93 "makeint.h" 2
# 108 "makeint.h"
# 1 "/usr/include/unistd.h" 1 3 4
# 27 "/usr/include/unistd.h" 3 4

# 202 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix_opt.h" 1 3 4
# 203 "/usr/include/unistd.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/environments.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/environments.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/environments.h" 2 3 4
# 207 "/usr/include/unistd.h" 2 3 4
# 226 "/usr/include/unistd.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 227 "/usr/include/unistd.h" 2 3 4
# 267 "/usr/include/unistd.h" 3 4
typedef __intptr_t intptr_t;






typedef __socklen_t socklen_t;
# 287 "/usr/include/unistd.h" 3 4
extern int access (const char *__name, int __type) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




extern int euidaccess (const char *__name, int __type)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int eaccess (const char *__name, int __type)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int faccessat (int __fd, const char *__file, int __type, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;
# 334 "/usr/include/unistd.h" 3 4
extern __off_t lseek (int __fd, __off_t __offset, int __whence) __attribute__ ((__nothrow__ , __leaf__));
# 345 "/usr/include/unistd.h" 3 4
extern __off64_t lseek64 (int __fd, __off64_t __offset, int __whence)
     __attribute__ ((__nothrow__ , __leaf__));






extern int close (int __fd);






extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;





extern ssize_t write (int __fd, const void *__buf, size_t __n) ;
# 376 "/usr/include/unistd.h" 3 4
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes,
        __off_t __offset) ;






extern ssize_t pwrite (int __fd, const void *__buf, size_t __n,
         __off_t __offset) ;
# 404 "/usr/include/unistd.h" 3 4
extern ssize_t pread64 (int __fd, void *__buf, size_t __nbytes,
   __off64_t __offset) ;


extern ssize_t pwrite64 (int __fd, const void *__buf, size_t __n,
    __off64_t __offset) ;







extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int pipe2 (int __pipedes[2], int __flags) __attribute__ ((__nothrow__ , __leaf__)) ;
# 432 "/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__ , __leaf__));
# 444 "/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);







extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__ , __leaf__));






extern int usleep (__useconds_t __useconds);
# 469 "/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int lchown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;






extern int fchownat (int __fd, const char *__file, __uid_t __owner,
       __gid_t __group, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;



extern int chdir (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;
# 511 "/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__ , __leaf__)) ;





extern char *get_current_dir_name (void) __attribute__ ((__nothrow__ , __leaf__));







extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;




extern int dup (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__ , __leaf__));




extern int dup3 (int __fd, int __fd2, int __flags) __attribute__ ((__nothrow__ , __leaf__));



extern char **__environ;

extern char **environ;





extern int execve (const char *__path, char *const __argv[],
     char *const __envp[]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int fexecve (int __fd, char *const __argv[], char *const __envp[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));




extern int execv (const char *__path, char *const __argv[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execle (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execl (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execvp (const char *__file, char *const __argv[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int execlp (const char *__file, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int execvpe (const char *__file, char *const __argv[],
      char *const __envp[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern int nice (int __inc) __attribute__ ((__nothrow__ , __leaf__)) ;




extern void _exit (int __status) __attribute__ ((__noreturn__));





# 1 "/usr/include/x86_64-linux-gnu/bits/confname.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/confname.h" 3 4
enum
  {
    _PC_LINK_MAX,

    _PC_MAX_CANON,

    _PC_MAX_INPUT,

    _PC_NAME_MAX,

    _PC_PATH_MAX,

    _PC_PIPE_BUF,

    _PC_CHOWN_RESTRICTED,

    _PC_NO_TRUNC,

    _PC_VDISABLE,

    _PC_SYNC_IO,

    _PC_ASYNC_IO,

    _PC_PRIO_IO,

    _PC_SOCK_MAXBUF,

    _PC_FILESIZEBITS,

    _PC_REC_INCR_XFER_SIZE,

    _PC_REC_MAX_XFER_SIZE,

    _PC_REC_MIN_XFER_SIZE,

    _PC_REC_XFER_ALIGN,

    _PC_ALLOC_SIZE_MIN,

    _PC_SYMLINK_MAX,

    _PC_2_SYMLINKS

  };


enum
  {
    _SC_ARG_MAX,

    _SC_CHILD_MAX,

    _SC_CLK_TCK,

    _SC_NGROUPS_MAX,

    _SC_OPEN_MAX,

    _SC_STREAM_MAX,

    _SC_TZNAME_MAX,

    _SC_JOB_CONTROL,

    _SC_SAVED_IDS,

    _SC_REALTIME_SIGNALS,

    _SC_PRIORITY_SCHEDULING,

    _SC_TIMERS,

    _SC_ASYNCHRONOUS_IO,

    _SC_PRIORITIZED_IO,

    _SC_SYNCHRONIZED_IO,

    _SC_FSYNC,

    _SC_MAPPED_FILES,

    _SC_MEMLOCK,

    _SC_MEMLOCK_RANGE,

    _SC_MEMORY_PROTECTION,

    _SC_MESSAGE_PASSING,

    _SC_SEMAPHORES,

    _SC_SHARED_MEMORY_OBJECTS,

    _SC_AIO_LISTIO_MAX,

    _SC_AIO_MAX,

    _SC_AIO_PRIO_DELTA_MAX,

    _SC_DELAYTIMER_MAX,

    _SC_MQ_OPEN_MAX,

    _SC_MQ_PRIO_MAX,

    _SC_VERSION,

    _SC_PAGESIZE,


    _SC_RTSIG_MAX,

    _SC_SEM_NSEMS_MAX,

    _SC_SEM_VALUE_MAX,

    _SC_SIGQUEUE_MAX,

    _SC_TIMER_MAX,




    _SC_BC_BASE_MAX,

    _SC_BC_DIM_MAX,

    _SC_BC_SCALE_MAX,

    _SC_BC_STRING_MAX,

    _SC_COLL_WEIGHTS_MAX,

    _SC_EQUIV_CLASS_MAX,

    _SC_EXPR_NEST_MAX,

    _SC_LINE_MAX,

    _SC_RE_DUP_MAX,

    _SC_CHARCLASS_NAME_MAX,


    _SC_2_VERSION,

    _SC_2_C_BIND,

    _SC_2_C_DEV,

    _SC_2_FORT_DEV,

    _SC_2_FORT_RUN,

    _SC_2_SW_DEV,

    _SC_2_LOCALEDEF,


    _SC_PII,

    _SC_PII_XTI,

    _SC_PII_SOCKET,

    _SC_PII_INTERNET,

    _SC_PII_OSI,

    _SC_POLL,

    _SC_SELECT,

    _SC_UIO_MAXIOV,

    _SC_IOV_MAX = _SC_UIO_MAXIOV,

    _SC_PII_INTERNET_STREAM,

    _SC_PII_INTERNET_DGRAM,

    _SC_PII_OSI_COTS,

    _SC_PII_OSI_CLTS,

    _SC_PII_OSI_M,

    _SC_T_IOV_MAX,



    _SC_THREADS,

    _SC_THREAD_SAFE_FUNCTIONS,

    _SC_GETGR_R_SIZE_MAX,

    _SC_GETPW_R_SIZE_MAX,

    _SC_LOGIN_NAME_MAX,

    _SC_TTY_NAME_MAX,

    _SC_THREAD_DESTRUCTOR_ITERATIONS,

    _SC_THREAD_KEYS_MAX,

    _SC_THREAD_STACK_MIN,

    _SC_THREAD_THREADS_MAX,

    _SC_THREAD_ATTR_STACKADDR,

    _SC_THREAD_ATTR_STACKSIZE,

    _SC_THREAD_PRIORITY_SCHEDULING,

    _SC_THREAD_PRIO_INHERIT,

    _SC_THREAD_PRIO_PROTECT,

    _SC_THREAD_PROCESS_SHARED,


    _SC_NPROCESSORS_CONF,

    _SC_NPROCESSORS_ONLN,

    _SC_PHYS_PAGES,

    _SC_AVPHYS_PAGES,

    _SC_ATEXIT_MAX,

    _SC_PASS_MAX,


    _SC_XOPEN_VERSION,

    _SC_XOPEN_XCU_VERSION,

    _SC_XOPEN_UNIX,

    _SC_XOPEN_CRYPT,

    _SC_XOPEN_ENH_I18N,

    _SC_XOPEN_SHM,


    _SC_2_CHAR_TERM,

    _SC_2_C_VERSION,

    _SC_2_UPE,


    _SC_XOPEN_XPG2,

    _SC_XOPEN_XPG3,

    _SC_XOPEN_XPG4,


    _SC_CHAR_BIT,

    _SC_CHAR_MAX,

    _SC_CHAR_MIN,

    _SC_INT_MAX,

    _SC_INT_MIN,

    _SC_LONG_BIT,

    _SC_WORD_BIT,

    _SC_MB_LEN_MAX,

    _SC_NZERO,

    _SC_SSIZE_MAX,

    _SC_SCHAR_MAX,

    _SC_SCHAR_MIN,

    _SC_SHRT_MAX,

    _SC_SHRT_MIN,

    _SC_UCHAR_MAX,

    _SC_UINT_MAX,

    _SC_ULONG_MAX,

    _SC_USHRT_MAX,


    _SC_NL_ARGMAX,

    _SC_NL_LANGMAX,

    _SC_NL_MSGMAX,

    _SC_NL_NMAX,

    _SC_NL_SETMAX,

    _SC_NL_TEXTMAX,


    _SC_XBS5_ILP32_OFF32,

    _SC_XBS5_ILP32_OFFBIG,

    _SC_XBS5_LP64_OFF64,

    _SC_XBS5_LPBIG_OFFBIG,


    _SC_XOPEN_LEGACY,

    _SC_XOPEN_REALTIME,

    _SC_XOPEN_REALTIME_THREADS,


    _SC_ADVISORY_INFO,

    _SC_BARRIERS,

    _SC_BASE,

    _SC_C_LANG_SUPPORT,

    _SC_C_LANG_SUPPORT_R,

    _SC_CLOCK_SELECTION,

    _SC_CPUTIME,

    _SC_THREAD_CPUTIME,

    _SC_DEVICE_IO,

    _SC_DEVICE_SPECIFIC,

    _SC_DEVICE_SPECIFIC_R,

    _SC_FD_MGMT,

    _SC_FIFO,

    _SC_PIPE,

    _SC_FILE_ATTRIBUTES,

    _SC_FILE_LOCKING,

    _SC_FILE_SYSTEM,

    _SC_MONOTONIC_CLOCK,

    _SC_MULTI_PROCESS,

    _SC_SINGLE_PROCESS,

    _SC_NETWORKING,

    _SC_READER_WRITER_LOCKS,

    _SC_SPIN_LOCKS,

    _SC_REGEXP,

    _SC_REGEX_VERSION,

    _SC_SHELL,

    _SC_SIGNALS,

    _SC_SPAWN,

    _SC_SPORADIC_SERVER,

    _SC_THREAD_SPORADIC_SERVER,

    _SC_SYSTEM_DATABASE,

    _SC_SYSTEM_DATABASE_R,

    _SC_TIMEOUTS,

    _SC_TYPED_MEMORY_OBJECTS,

    _SC_USER_GROUPS,

    _SC_USER_GROUPS_R,

    _SC_2_PBS,

    _SC_2_PBS_ACCOUNTING,

    _SC_2_PBS_LOCATE,

    _SC_2_PBS_MESSAGE,

    _SC_2_PBS_TRACK,

    _SC_SYMLOOP_MAX,

    _SC_STREAMS,

    _SC_2_PBS_CHECKPOINT,


    _SC_V6_ILP32_OFF32,

    _SC_V6_ILP32_OFFBIG,

    _SC_V6_LP64_OFF64,

    _SC_V6_LPBIG_OFFBIG,


    _SC_HOST_NAME_MAX,

    _SC_TRACE,

    _SC_TRACE_EVENT_FILTER,

    _SC_TRACE_INHERIT,

    _SC_TRACE_LOG,


    _SC_LEVEL1_ICACHE_SIZE,

    _SC_LEVEL1_ICACHE_ASSOC,

    _SC_LEVEL1_ICACHE_LINESIZE,

    _SC_LEVEL1_DCACHE_SIZE,

    _SC_LEVEL1_DCACHE_ASSOC,

    _SC_LEVEL1_DCACHE_LINESIZE,

    _SC_LEVEL2_CACHE_SIZE,

    _SC_LEVEL2_CACHE_ASSOC,

    _SC_LEVEL2_CACHE_LINESIZE,

    _SC_LEVEL3_CACHE_SIZE,

    _SC_LEVEL3_CACHE_ASSOC,

    _SC_LEVEL3_CACHE_LINESIZE,

    _SC_LEVEL4_CACHE_SIZE,

    _SC_LEVEL4_CACHE_ASSOC,

    _SC_LEVEL4_CACHE_LINESIZE,



    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,

    _SC_RAW_SOCKETS,


    _SC_V7_ILP32_OFF32,

    _SC_V7_ILP32_OFFBIG,

    _SC_V7_LP64_OFF64,

    _SC_V7_LPBIG_OFFBIG,


    _SC_SS_REPL_MAX,


    _SC_TRACE_EVENT_NAME_MAX,

    _SC_TRACE_NAME_MAX,

    _SC_TRACE_SYS_MAX,

    _SC_TRACE_USER_EVENT_MAX,


    _SC_XOPEN_STREAMS,


    _SC_THREAD_ROBUST_PRIO_INHERIT,

    _SC_THREAD_ROBUST_PRIO_PROTECT

  };


enum
  {
    _CS_PATH,


    _CS_V6_WIDTH_RESTRICTED_ENVS,



    _CS_GNU_LIBC_VERSION,

    _CS_GNU_LIBPTHREAD_VERSION,


    _CS_V5_WIDTH_RESTRICTED_ENVS,



    _CS_V7_WIDTH_RESTRICTED_ENVS,



    _CS_LFS_CFLAGS = 1000,

    _CS_LFS_LDFLAGS,

    _CS_LFS_LIBS,

    _CS_LFS_LINTFLAGS,

    _CS_LFS64_CFLAGS,

    _CS_LFS64_LDFLAGS,

    _CS_LFS64_LIBS,

    _CS_LFS64_LINTFLAGS,


    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,

    _CS_XBS5_ILP32_OFF32_LDFLAGS,

    _CS_XBS5_ILP32_OFF32_LIBS,

    _CS_XBS5_ILP32_OFF32_LINTFLAGS,

    _CS_XBS5_ILP32_OFFBIG_CFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LIBS,

    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,

    _CS_XBS5_LP64_OFF64_CFLAGS,

    _CS_XBS5_LP64_OFF64_LDFLAGS,

    _CS_XBS5_LP64_OFF64_LIBS,

    _CS_XBS5_LP64_OFF64_LINTFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LIBS,

    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LIBS,

    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_LP64_OFF64_CFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LIBS,

    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V7_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LIBS,

    _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V7_LP64_OFF64_CFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LIBS,

    _CS_POSIX_V7_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS,


    _CS_V6_ENV,

    _CS_V7_ENV

  };
# 610 "/usr/include/unistd.h" 2 3 4


extern long int pathconf (const char *__path, int __name)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__ , __leaf__));


extern long int sysconf (int __name) __attribute__ ((__nothrow__ , __leaf__));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__ , __leaf__));




extern __pid_t getpid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t getppid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t getpgrp (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));

extern __pid_t getpgid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));






extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__ , __leaf__));
# 660 "/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__ , __leaf__));






extern __pid_t setsid (void) __attribute__ ((__nothrow__ , __leaf__));



extern __pid_t getsid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));



extern __uid_t getuid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __gid_t getgid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __gid_t getegid (void) __attribute__ ((__nothrow__ , __leaf__));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__ , __leaf__)) ;



extern int group_member (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__));






extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__)) ;





extern int getresuid (__uid_t *__ruid, __uid_t *__euid, __uid_t *__suid)
     __attribute__ ((__nothrow__ , __leaf__));



extern int getresgid (__gid_t *__rgid, __gid_t *__egid, __gid_t *__sgid)
     __attribute__ ((__nothrow__ , __leaf__));



extern int setresuid (__uid_t __ruid, __uid_t __euid, __uid_t __suid)
     __attribute__ ((__nothrow__ , __leaf__)) ;



extern int setresgid (__gid_t __rgid, __gid_t __egid, __gid_t __sgid)
     __attribute__ ((__nothrow__ , __leaf__)) ;






extern __pid_t fork (void) __attribute__ ((__nothrow__));







extern __pid_t vfork (void) __attribute__ ((__nothrow__ , __leaf__));





extern char *ttyname (int __fd) __attribute__ ((__nothrow__ , __leaf__));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;



extern int isatty (int __fd) __attribute__ ((__nothrow__ , __leaf__));





extern int ttyslot (void) __attribute__ ((__nothrow__ , __leaf__));




extern int link (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int linkat (int __fromfd, const char *__from, int __tofd,
     const char *__to, int __flags)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4))) ;




extern int symlink (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int symlinkat (const char *__from, int __tofd,
        const char *__to) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 3))) ;


extern ssize_t readlinkat (int __fd, const char *__restrict __path,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3))) ;



extern int unlink (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern int unlinkat (int __fd, const char *__name, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern int rmdir (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__ , __leaf__));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__ , __leaf__));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));




extern int setlogin (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 871 "/usr/include/unistd.h" 3 4
# 1 "./getopt.h" 1 3 4
# 32 "./getopt.h" 3 4
extern char *optarg;
# 46 "./getopt.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
# 78 "./getopt.h" 3 4
struct option
{

  const char *name;





  int has_arg;
  int *flag;
  int val;
};
# 103 "./getopt.h" 3 4
extern int getopt (int argc, char *const *argv, const char *shortopts);



extern int getopt_long (int argc, char *const *argv, const char *shortopts,
          const struct option *longopts, int *longind);
extern int getopt_long_only (int argc, char *const *argv,
        const char *shortopts,
               const struct option *longopts, int *longind);


extern int _getopt_internal (int argc, char *const *argv,
        const char *shortopts,
               const struct option *longopts, int *longind,
        int long_only);
# 872 "/usr/include/unistd.h" 2 3 4







extern int gethostname (char *__name, size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int sethostname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int sethostid (long int __id) __attribute__ ((__nothrow__ , __leaf__)) ;





extern int getdomainname (char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
extern int setdomainname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;





extern int vhangup (void) __attribute__ ((__nothrow__ , __leaf__));


extern int revoke (const char *__file) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;







extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int acct (const char *__name) __attribute__ ((__nothrow__ , __leaf__));



extern char *getusershell (void) __attribute__ ((__nothrow__ , __leaf__));
extern void endusershell (void) __attribute__ ((__nothrow__ , __leaf__));
extern void setusershell (void) __attribute__ ((__nothrow__ , __leaf__));





extern int daemon (int __nochdir, int __noclose) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int chroot (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern char *getpass (const char *__prompt) __attribute__ ((__nonnull__ (1)));







extern int fsync (int __fd);





extern int syncfs (int __fd) __attribute__ ((__nothrow__ , __leaf__));






extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__ , __leaf__));





extern int getpagesize (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__ , __leaf__));
# 993 "/usr/include/unistd.h" 3 4
extern int truncate (const char *__file, __off_t __length)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 1005 "/usr/include/unistd.h" 3 4
extern int truncate64 (const char *__file, __off64_t __length)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 1016 "/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off_t __length) __attribute__ ((__nothrow__ , __leaf__)) ;
# 1026 "/usr/include/unistd.h" 3 4
extern int ftruncate64 (int __fd, __off64_t __length) __attribute__ ((__nothrow__ , __leaf__)) ;
# 1037 "/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__ , __leaf__)) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__ , __leaf__));
# 1058 "/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__ , __leaf__));
# 1081 "/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off_t __len) ;
# 1091 "/usr/include/unistd.h" 3 4
extern int lockf64 (int __fd, int __cmd, __off64_t __len) ;
# 1112 "/usr/include/unistd.h" 3 4
extern int fdatasync (int __fildes);







extern char *crypt (const char *__key, const char *__salt)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern void encrypt (char *__glibc_block, int __edflag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern void swab (const void *__restrict __from, void *__restrict __to,
    ssize_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 1151 "/usr/include/unistd.h" 3 4

# 109 "makeint.h" 2
# 142 "makeint.h"
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 1 3 4
# 34 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/syslimits.h" 1 3 4






# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 1 3 4
# 168 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 143 "/usr/include/limits.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 1 3 4
# 160 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 3 4
# 1 "/usr/include/linux/limits.h" 1 3 4
# 39 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 2 3 4
# 161 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 2 3 4
# 144 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/posix2_lim.h" 1 3 4
# 148 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/xopen_lim.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/bits/xopen_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdio_lim.h" 1 3 4
# 34 "/usr/include/x86_64-linux-gnu/bits/xopen_lim.h" 2 3 4
# 152 "/usr/include/limits.h" 2 3 4
# 169 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 2 3 4
# 8 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/syslimits.h" 2 3 4
# 35 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 2 3 4
# 143 "makeint.h" 2


# 1 "/usr/include/x86_64-linux-gnu/sys/param.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/sys/param.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/sys/param.h" 2 3 4


# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include-fixed/limits.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/sys/param.h" 2 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/param.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/param.h" 3 4
# 1 "/usr/include/linux/param.h" 1 3 4



# 1 "/usr/include/x86_64-linux-gnu/asm/param.h" 1 3 4
# 1 "/usr/include/asm-generic/param.h" 1 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/param.h" 2 3 4
# 5 "/usr/include/linux/param.h" 2 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/param.h" 2 3 4
# 32 "/usr/include/x86_64-linux-gnu/sys/param.h" 2 3 4
# 146 "makeint.h" 2
# 237 "makeint.h"
# 1 "/usr/include/stdlib.h" 1 3 4
# 32 "/usr/include/stdlib.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 328 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 3 4
typedef int wchar_t;
# 33 "/usr/include/stdlib.h" 2 3 4








# 1 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 1 3 4
# 50 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 3 4
typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
# 42 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 1 3 4
# 66 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 3 4
union wait
  {
    int w_status;
    struct
      {

 unsigned int __w_termsig:7;
 unsigned int __w_coredump:1;
 unsigned int __w_retcode:8;
 unsigned int:16;







      } __wait_terminated;
    struct
      {

 unsigned int __w_stopval:8;
 unsigned int __w_stopsig:8;
 unsigned int:16;






      } __wait_stopped;
  };
# 43 "/usr/include/stdlib.h" 2 3 4
# 67 "/usr/include/stdlib.h" 3 4
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));
# 95 "/usr/include/stdlib.h" 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;







__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;


# 139 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ , __leaf__)) ;




extern double atof (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern float strtof (const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern long double strtold (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern unsigned long int strtoul (const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




__extension__
extern long long int strtoq (const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtouq (const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





__extension__
extern long long int strtoll (const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtoull (const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

# 239 "/usr/include/stdlib.h" 3 4
extern long int strtol_l (const char *__restrict __nptr,
     char **__restrict __endptr, int __base,
     __locale_t __loc) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 4)));

extern unsigned long int strtoul_l (const char *__restrict __nptr,
        char **__restrict __endptr,
        int __base, __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 4)));

__extension__
extern long long int strtoll_l (const char *__restrict __nptr,
    char **__restrict __endptr, int __base,
    __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 4)));

__extension__
extern unsigned long long int strtoull_l (const char *__restrict __nptr,
       char **__restrict __endptr,
       int __base, __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 4)));

extern double strtod_l (const char *__restrict __nptr,
   char **__restrict __endptr, __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 3)));

extern float strtof_l (const char *__restrict __nptr,
         char **__restrict __endptr, __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 3)));

extern long double strtold_l (const char *__restrict __nptr,
         char **__restrict __endptr,
         __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 3)));





extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) atoi (const char *__nptr)
{
  return (int) strtol (__nptr, (char **) ((void *)0), 10);
}
extern __inline __attribute__ ((__gnu_inline__)) long int
__attribute__ ((__nothrow__ , __leaf__)) atol (const char *__nptr)
{
  return strtol (__nptr, (char **) ((void *)0), 10);
}




__extension__ extern __inline __attribute__ ((__gnu_inline__)) long long int
__attribute__ ((__nothrow__ , __leaf__)) atoll (const char *__nptr)
{
  return strtoll (__nptr, (char **) ((void *)0), 10);
}

# 305 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__ , __leaf__)) ;


extern long int a64l (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
# 321 "/usr/include/stdlib.h" 3 4
extern long int random (void) __attribute__ ((__nothrow__ , __leaf__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));






extern int rand (void) __attribute__ ((__nothrow__ , __leaf__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));




extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__ , __leaf__));







extern double drand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__ , __leaf__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    __extension__ unsigned long long int __a;

  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));









extern void *malloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;










extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));




extern void cfree (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));
# 498 "/usr/include/stdlib.h" 3 4
extern void *valloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;




extern void *aligned_alloc (size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__alloc_size__ (2))) ;




extern void abort (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







extern int at_quick_exit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));





extern void quick_exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));







extern void _Exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));






extern char *getenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;





extern char *secure_getenv (const char *__name)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;






extern int putenv (char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (const char *__name, const char *__value, int __replace)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int clearenv (void) __attribute__ ((__nothrow__ , __leaf__));
# 606 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 620 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 630 "/usr/include/stdlib.h" 3 4
extern int mkstemp64 (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 642 "/usr/include/stdlib.h" 3 4
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
# 652 "/usr/include/stdlib.h" 3 4
extern int mkstemps64 (char *__template, int __suffixlen)
     __attribute__ ((__nonnull__ (1))) ;
# 663 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 674 "/usr/include/stdlib.h" 3 4
extern int mkostemp (char *__template, int __flags) __attribute__ ((__nonnull__ (1))) ;
# 684 "/usr/include/stdlib.h" 3 4
extern int mkostemp64 (char *__template, int __flags) __attribute__ ((__nonnull__ (1))) ;
# 694 "/usr/include/stdlib.h" 3 4
extern int mkostemps (char *__template, int __suffixlen, int __flags)
     __attribute__ ((__nonnull__ (1))) ;
# 706 "/usr/include/stdlib.h" 3 4
extern int mkostemps64 (char *__template, int __suffixlen, int __flags)
     __attribute__ ((__nonnull__ (1))) ;









extern int system (const char *__command) ;






extern char *canonicalize_file_name (const char *__name)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 734 "/usr/include/stdlib.h" 3 4
extern char *realpath (const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__ , __leaf__)) ;






typedef int (*__compar_fn_t) (const void *, const void *);


typedef __compar_fn_t comparison_fn_t;



typedef int (*__compar_d_fn_t) (const void *, const void *, void *);





extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;


# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h" 1 3 4
# 19 "/usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) void *
bsearch (const void *__key, const void *__base, size_t __nmemb, size_t __size,
  __compar_fn_t __compar)
{
  size_t __l, __u, __idx;
  const void *__p;
  int __comparison;

  __l = 0;
  __u = __nmemb;
  while (__l < __u)
    {
      __idx = (__l + __u) / 2;
      __p = (void *) (((const char *) __base) + (__idx * __size));
      __comparison = (*__compar) (__key, __p);
      if (__comparison < 0)
 __u = __idx;
      else if (__comparison > 0)
 __l = __idx + 1;
      else
 return (void *) __p;
    }

  return ((void *)0);
}
# 761 "/usr/include/stdlib.h" 2 3 4




extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));

extern void qsort_r (void *__base, size_t __nmemb, size_t __size,
       __compar_d_fn_t __compar, void *__arg)
  __attribute__ ((__nonnull__ (1, 4)));




extern int abs (int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;



__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;







extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;




__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;

# 812 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));






extern int mblen (const char *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int mbtowc (wchar_t *__restrict __pwc,
     const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__ , __leaf__));



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));

extern size_t wcstombs (char *__restrict __s,
   const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__));








extern int rpmatch (const char *__response) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 899 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3))) ;





extern void setkey (const char *__key) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







extern int posix_openpt (int __oflag) ;







extern int grantpt (int __fd) __attribute__ ((__nothrow__ , __leaf__));



extern int unlockpt (int __fd) __attribute__ ((__nothrow__ , __leaf__));




extern char *ptsname (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int ptsname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern int getpt (void);






extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 3 4

extern __inline __attribute__ ((__gnu_inline__)) double
__attribute__ ((__nothrow__ , __leaf__)) atof (const char *__nptr)
{
  return strtod (__nptr, (char **) ((void *)0));
}

# 956 "/usr/include/stdlib.h" 2 3 4
# 968 "/usr/include/stdlib.h" 3 4

# 238 "makeint.h" 2
# 1 "/usr/include/string.h" 1 3 4
# 27 "/usr/include/string.h" 3 4





# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 33 "/usr/include/string.h" 2 3 4
# 44 "/usr/include/string.h" 3 4


extern void *memcpy (void *__restrict __dest, const void *__restrict __src,
       size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, const void *__src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 96 "/usr/include/string.h" 3 4
extern void *memchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 110 "/usr/include/string.h" 3 4
extern void *rawmemchr (const void *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 121 "/usr/include/string.h" 3 4
extern void *memrchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));






extern char *strcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

# 166 "/usr/include/string.h" 3 4
extern int strcoll_l (const char *__s1, const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (const char *__string, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 211 "/usr/include/string.h" 3 4

# 236 "/usr/include/string.h" 3 4
extern char *strchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 263 "/usr/include/string.h" 3 4
extern char *strrchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 277 "/usr/include/string.h" 3 4
extern char *strchrnul (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));






extern size_t strcspn (const char *__s, const char *__reject)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 315 "/usr/include/string.h" 3 4
extern char *strpbrk (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 342 "/usr/include/string.h" 3 4
extern char *strstr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
# 373 "/usr/include/string.h" 3 4
extern char *strcasestr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));







extern void *memmem (const void *__haystack, size_t __haystacklen,
       const void *__needle, size_t __needlelen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 3)));



extern void *__mempcpy (void *__restrict __dest,
   const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern void *mempcpy (void *__restrict __dest,
        const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern size_t strlen (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) __attribute__ ((__nothrow__ , __leaf__));

# 438 "/usr/include/string.h" 3 4
extern char *strerror_r (int __errnum, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;





extern char *strerror_l (int __errnum, __locale_t __l) __attribute__ ((__nothrow__ , __leaf__));





extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 489 "/usr/include/string.h" 3 4
extern char *index (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 517 "/usr/include/string.h" 3 4
extern char *rindex (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern int ffs (int __i) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern int ffsl (long int __l) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__ extern int ffsll (long long int __ll)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int strcasecmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));





extern int strcasecmp_l (const char *__s1, const char *__s2,
    __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern int strncasecmp_l (const char *__s1, const char *__s2,
     size_t __n, __locale_t __loc)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 4)));





extern char *strsep (char **__restrict __stringp,
       const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__ , __leaf__));


extern char *__stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int strverscmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strfry (char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern void *memfrob (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 604 "/usr/include/string.h" 3 4
extern char *basename (const char *__filename) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 632 "/usr/include/string.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/string.h" 1 3 4
# 633 "/usr/include/string.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/string2.h" 1 3 4
# 393 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern void *__rawmemchr (const void *__s, int __c);
# 945 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) size_t __strcspn_c1 (const char *__s, int __reject);
extern __inline __attribute__ ((__gnu_inline__)) size_t
__strcspn_c1 (const char *__s, int __reject)
{
  size_t __result = 0;
  while (__s[__result] != '\0' && __s[__result] != __reject)
    ++__result;
  return __result;
}

extern __inline __attribute__ ((__gnu_inline__)) size_t __strcspn_c2 (const char *__s, int __reject1,
         int __reject2);
extern __inline __attribute__ ((__gnu_inline__)) size_t
__strcspn_c2 (const char *__s, int __reject1, int __reject2)
{
  size_t __result = 0;
  while (__s[__result] != '\0' && __s[__result] != __reject1
  && __s[__result] != __reject2)
    ++__result;
  return __result;
}

extern __inline __attribute__ ((__gnu_inline__)) size_t __strcspn_c3 (const char *__s, int __reject1,
         int __reject2, int __reject3);
extern __inline __attribute__ ((__gnu_inline__)) size_t
__strcspn_c3 (const char *__s, int __reject1, int __reject2,
       int __reject3)
{
  size_t __result = 0;
  while (__s[__result] != '\0' && __s[__result] != __reject1
  && __s[__result] != __reject2 && __s[__result] != __reject3)
    ++__result;
  return __result;
}
# 1021 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) size_t __strspn_c1 (const char *__s, int __accept);
extern __inline __attribute__ ((__gnu_inline__)) size_t
__strspn_c1 (const char *__s, int __accept)
{
  size_t __result = 0;

  while (__s[__result] == __accept)
    ++__result;
  return __result;
}

extern __inline __attribute__ ((__gnu_inline__)) size_t __strspn_c2 (const char *__s, int __accept1,
        int __accept2);
extern __inline __attribute__ ((__gnu_inline__)) size_t
__strspn_c2 (const char *__s, int __accept1, int __accept2)
{
  size_t __result = 0;

  while (__s[__result] == __accept1 || __s[__result] == __accept2)
    ++__result;
  return __result;
}

extern __inline __attribute__ ((__gnu_inline__)) size_t __strspn_c3 (const char *__s, int __accept1,
        int __accept2, int __accept3);
extern __inline __attribute__ ((__gnu_inline__)) size_t
__strspn_c3 (const char *__s, int __accept1, int __accept2, int __accept3)
{
  size_t __result = 0;

  while (__s[__result] == __accept1 || __s[__result] == __accept2
  || __s[__result] == __accept3)
    ++__result;
  return __result;
}
# 1097 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) char *__strpbrk_c2 (const char *__s, int __accept1,
        int __accept2);
extern __inline __attribute__ ((__gnu_inline__)) char *
__strpbrk_c2 (const char *__s, int __accept1, int __accept2)
{

  while (*__s != '\0' && *__s != __accept1 && *__s != __accept2)
    ++__s;
  return *__s == '\0' ? ((void *)0) : (char *) (size_t) __s;
}

extern __inline __attribute__ ((__gnu_inline__)) char *__strpbrk_c3 (const char *__s, int __accept1,
        int __accept2, int __accept3);
extern __inline __attribute__ ((__gnu_inline__)) char *
__strpbrk_c3 (const char *__s, int __accept1, int __accept2, int __accept3)
{

  while (*__s != '\0' && *__s != __accept1 && *__s != __accept2
  && *__s != __accept3)
    ++__s;
  return *__s == '\0' ? ((void *)0) : (char *) (size_t) __s;
}
# 1147 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) char *__strtok_r_1c (char *__s, char __sep, char **__nextp);
extern __inline __attribute__ ((__gnu_inline__)) char *
__strtok_r_1c (char *__s, char __sep, char **__nextp)
{
  char *__result;
  if (__s == ((void *)0))
    __s = *__nextp;
  while (*__s == __sep)
    ++__s;
  __result = ((void *)0);
  if (*__s != '\0')
    {
      __result = __s++;
      while (*__s != '\0')
 if (*__s++ == __sep)
   {
     __s[-1] = '\0';
     break;
   }
    }
  *__nextp = __s;
  return __result;
}
# 1179 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern char *__strsep_g (char **__stringp, const char *__delim);
# 1197 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) char *__strsep_1c (char **__s, char __reject);
extern __inline __attribute__ ((__gnu_inline__)) char *
__strsep_1c (char **__s, char __reject)
{
  char *__retval = *__s;
  if (__retval != ((void *)0) && (*__s = (__extension__ (__builtin_constant_p (__reject) && !__builtin_constant_p (__retval) && (__reject) == '\0' ? (char *) __rawmemchr (__retval, __reject) : __builtin_strchr (__retval, __reject)))) != ((void *)0))
    *(*__s)++ = '\0';
  return __retval;
}

extern __inline __attribute__ ((__gnu_inline__)) char *__strsep_2c (char **__s, char __reject1, char __reject2);
extern __inline __attribute__ ((__gnu_inline__)) char *
__strsep_2c (char **__s, char __reject1, char __reject2)
{
  char *__retval = *__s;
  if (__retval != ((void *)0))
    {
      char *__cp = __retval;
      while (1)
 {
   if (*__cp == '\0')
     {
       __cp = ((void *)0);
   break;
     }
   if (*__cp == __reject1 || *__cp == __reject2)
     {
       *__cp++ = '\0';
       break;
     }
   ++__cp;
 }
      *__s = __cp;
    }
  return __retval;
}

extern __inline __attribute__ ((__gnu_inline__)) char *__strsep_3c (char **__s, char __reject1, char __reject2,
       char __reject3);
extern __inline __attribute__ ((__gnu_inline__)) char *
__strsep_3c (char **__s, char __reject1, char __reject2, char __reject3)
{
  char *__retval = *__s;
  if (__retval != ((void *)0))
    {
      char *__cp = __retval;
      while (1)
 {
   if (*__cp == '\0')
     {
       __cp = ((void *)0);
   break;
     }
   if (*__cp == __reject1 || *__cp == __reject2 || *__cp == __reject3)
     {
       *__cp++ = '\0';
       break;
     }
   ++__cp;
 }
      *__s = __cp;
    }
  return __retval;
}
# 1278 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern char *__strdup (const char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__));
# 1297 "/usr/include/x86_64-linux-gnu/bits/string2.h" 3 4
extern char *__strndup (const char *__string, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__));
# 636 "/usr/include/string.h" 2 3 4
# 644 "/usr/include/string.h" 3 4

# 239 "makeint.h" 2
# 283 "makeint.h"
# 1 "/usr/include/inttypes.h" 1 3 4
# 27 "/usr/include/inttypes.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stdint.h" 1 3 4
# 9 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stdint.h" 3 4
# 1 "/usr/include/stdint.h" 1 3 4
# 26 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wchar.h" 1 3 4
# 27 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/stdint.h" 2 3 4
# 48 "/usr/include/stdint.h" 3 4
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;



typedef unsigned long int uint64_t;
# 65 "/usr/include/stdint.h" 3 4
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;

typedef long int int_least64_t;






typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;

typedef unsigned long int uint_least64_t;
# 90 "/usr/include/stdint.h" 3 4
typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
# 103 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
# 122 "/usr/include/stdint.h" 3 4
typedef unsigned long int uintptr_t;
# 134 "/usr/include/stdint.h" 3 4
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
# 10 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stdint.h" 2 3 4
# 28 "/usr/include/inttypes.h" 2 3 4






typedef int __gwchar_t;
# 266 "/usr/include/inttypes.h" 3 4





typedef struct
  {
    long int quot;
    long int rem;
  } imaxdiv_t;
# 290 "/usr/include/inttypes.h" 3 4
extern intmax_t imaxabs (intmax_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern imaxdiv_t imaxdiv (intmax_t __numer, intmax_t __denom)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern intmax_t strtoimax (const char *__restrict __nptr,
      char **__restrict __endptr, int __base) __attribute__ ((__nothrow__ , __leaf__));


extern uintmax_t strtoumax (const char *__restrict __nptr,
       char ** __restrict __endptr, int __base) __attribute__ ((__nothrow__ , __leaf__));


extern intmax_t wcstoimax (const __gwchar_t *__restrict __nptr,
      __gwchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__));


extern uintmax_t wcstoumax (const __gwchar_t *__restrict __nptr,
       __gwchar_t ** __restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__));





extern long int __strtol_internal (const char *__restrict __nptr,
       char **__restrict __endptr,
       int __base, int __group)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern __inline __attribute__ ((__gnu_inline__)) intmax_t
__attribute__ ((__nothrow__ , __leaf__)) strtoimax (const char *__restrict nptr, char **__restrict endptr, int base)

{
  return __strtol_internal (nptr, endptr, base, 0);
}

extern unsigned long int __strtoul_internal (const char *__restrict __nptr,
          char ** __restrict __endptr,
          int __base, int __group)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern __inline __attribute__ ((__gnu_inline__)) uintmax_t
__attribute__ ((__nothrow__ , __leaf__)) strtoumax (const char *__restrict nptr, char **__restrict endptr, int base)

{
  return __strtoul_internal (nptr, endptr, base, 0);
}

extern long int __wcstol_internal (const __gwchar_t * __restrict __nptr,
       __gwchar_t **__restrict __endptr,
       int __base, int __group)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern __inline __attribute__ ((__gnu_inline__)) intmax_t
__attribute__ ((__nothrow__ , __leaf__)) wcstoimax (const __gwchar_t *__restrict nptr, __gwchar_t **__restrict endptr, int base)

{
  return __wcstol_internal (nptr, endptr, base, 0);
}

extern unsigned long int __wcstoul_internal (const __gwchar_t *
          __restrict __nptr,
          __gwchar_t **
          __restrict __endptr,
          int __base, int __group)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

extern __inline __attribute__ ((__gnu_inline__)) uintmax_t
__attribute__ ((__nothrow__ , __leaf__)) wcstoumax (const __gwchar_t *__restrict nptr, __gwchar_t **__restrict endptr, int base)

{
  return __wcstoul_internal (nptr, endptr, base, 0);
}
# 432 "/usr/include/inttypes.h" 3 4

# 284 "makeint.h" 2
# 333 "makeint.h"
# 1 "/usr/include/locale.h" 1 3 4
# 28 "/usr/include/locale.h" 3 4
# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 29 "/usr/include/locale.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/locale.h" 1 3 4
# 30 "/usr/include/locale.h" 2 3 4


# 50 "/usr/include/locale.h" 3 4



struct lconv
{


  char *decimal_point;
  char *thousands_sep;





  char *grouping;





  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;

  char p_cs_precedes;

  char p_sep_by_space;

  char n_cs_precedes;

  char n_sep_by_space;






  char p_sign_posn;
  char n_sign_posn;


  char int_p_cs_precedes;

  char int_p_sep_by_space;

  char int_n_cs_precedes;

  char int_n_sep_by_space;






  char int_p_sign_posn;
  char int_n_sign_posn;
# 120 "/usr/include/locale.h" 3 4
};



extern char *setlocale (int __category, const char *__locale) __attribute__ ((__nothrow__ , __leaf__));


extern struct lconv *localeconv (void) __attribute__ ((__nothrow__ , __leaf__));


# 151 "/usr/include/locale.h" 3 4
extern __locale_t newlocale (int __category_mask, const char *__locale,
        __locale_t __base) __attribute__ ((__nothrow__ , __leaf__));
# 186 "/usr/include/locale.h" 3 4
extern __locale_t duplocale (__locale_t __dataset) __attribute__ ((__nothrow__ , __leaf__));



extern void freelocale (__locale_t __dataset) __attribute__ ((__nothrow__ , __leaf__));






extern __locale_t uselocale (__locale_t __dataset) __attribute__ ((__nothrow__ , __leaf__));








# 334 "makeint.h" 2




# 1 "./gettext.h" 1
# 24 "./gettext.h"
# 1 "/usr/include/libintl.h" 1 3 4
# 34 "/usr/include/libintl.h" 3 4





extern char *gettext (const char *__msgid)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (1)));



extern char *dgettext (const char *__domainname, const char *__msgid)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (2)));
extern char *__dgettext (const char *__domainname, const char *__msgid)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (2)));



extern char *dcgettext (const char *__domainname,
   const char *__msgid, int __category)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (2)));
extern char *__dcgettext (const char *__domainname,
     const char *__msgid, int __category)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (2)));




extern char *ngettext (const char *__msgid1, const char *__msgid2,
         unsigned long int __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (1))) __attribute__ ((__format_arg__ (2)));



extern char *dngettext (const char *__domainname, const char *__msgid1,
   const char *__msgid2, unsigned long int __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (2))) __attribute__ ((__format_arg__ (3)));



extern char *dcngettext (const char *__domainname, const char *__msgid1,
    const char *__msgid2, unsigned long int __n,
    int __category)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format_arg__ (2))) __attribute__ ((__format_arg__ (3)));





extern char *textdomain (const char *__domainname) __attribute__ ((__nothrow__ , __leaf__));



extern char *bindtextdomain (const char *__domainname,
        const char *__dirname) __attribute__ ((__nothrow__ , __leaf__));



extern char *bind_textdomain_codeset (const char *__domainname,
          const char *__codeset) __attribute__ ((__nothrow__ , __leaf__));







# 1 "/home/daniter/workspace/gg-playground/targets/gcc63build/install/lib/gcc/x86_64-pc-linux-gnu/6.3.0/include/stddef.h" 1 3 4
# 101 "/usr/include/libintl.h" 2 3 4
# 121 "/usr/include/libintl.h" 3 4

# 25 "./gettext.h" 2
# 339 "makeint.h" 2
# 449 "makeint.h"
# 1 "/usr/include/x86_64-linux-gnu/sys/resource.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/sys/resource.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/resource.h" 1 3 4
# 31 "/usr/include/x86_64-linux-gnu/bits/resource.h" 3 4
enum __rlimit_resource
{

  RLIMIT_CPU = 0,



  RLIMIT_FSIZE = 1,



  RLIMIT_DATA = 2,



  RLIMIT_STACK = 3,



  RLIMIT_CORE = 4,






  __RLIMIT_RSS = 5,



  RLIMIT_NOFILE = 7,
  __RLIMIT_OFILE = RLIMIT_NOFILE,




  RLIMIT_AS = 9,



  __RLIMIT_NPROC = 6,



  __RLIMIT_MEMLOCK = 8,



  __RLIMIT_LOCKS = 10,



  __RLIMIT_SIGPENDING = 11,



  __RLIMIT_MSGQUEUE = 12,





  __RLIMIT_NICE = 13,




  __RLIMIT_RTPRIO = 14,





  __RLIMIT_RTTIME = 15,


  __RLIMIT_NLIMITS = 16,
  __RLIM_NLIMITS = __RLIMIT_NLIMITS


};
# 131 "/usr/include/x86_64-linux-gnu/bits/resource.h" 3 4
typedef __rlim_t rlim_t;




typedef __rlim64_t rlim64_t;


struct rlimit
  {

    rlim_t rlim_cur;

    rlim_t rlim_max;
  };


struct rlimit64
  {

    rlim64_t rlim_cur;

    rlim64_t rlim_max;
 };



enum __rusage_who
{

  RUSAGE_SELF = 0,



  RUSAGE_CHILDREN = -1



  ,

  RUSAGE_THREAD = 1




};


# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 180 "/usr/include/x86_64-linux-gnu/bits/resource.h" 2 3 4







struct rusage
  {

    struct timeval ru_utime;

    struct timeval ru_stime;

    __extension__ union
      {
 long int ru_maxrss;
 __syscall_slong_t __ru_maxrss_word;
      };



    __extension__ union
      {
 long int ru_ixrss;
 __syscall_slong_t __ru_ixrss_word;
      };

    __extension__ union
      {
 long int ru_idrss;
 __syscall_slong_t __ru_idrss_word;
      };

    __extension__ union
      {
 long int ru_isrss;
  __syscall_slong_t __ru_isrss_word;
      };


    __extension__ union
      {
 long int ru_minflt;
 __syscall_slong_t __ru_minflt_word;
      };

    __extension__ union
      {
 long int ru_majflt;
 __syscall_slong_t __ru_majflt_word;
      };

    __extension__ union
      {
 long int ru_nswap;
 __syscall_slong_t __ru_nswap_word;
      };


    __extension__ union
      {
 long int ru_inblock;
 __syscall_slong_t __ru_inblock_word;
      };

    __extension__ union
      {
 long int ru_oublock;
 __syscall_slong_t __ru_oublock_word;
      };

    __extension__ union
      {
 long int ru_msgsnd;
 __syscall_slong_t __ru_msgsnd_word;
      };

    __extension__ union
      {
 long int ru_msgrcv;
 __syscall_slong_t __ru_msgrcv_word;
      };

    __extension__ union
      {
 long int ru_nsignals;
 __syscall_slong_t __ru_nsignals_word;
      };



    __extension__ union
      {
 long int ru_nvcsw;
 __syscall_slong_t __ru_nvcsw_word;
      };


    __extension__ union
      {
 long int ru_nivcsw;
 __syscall_slong_t __ru_nivcsw_word;
      };
  };







enum __priority_which
{
  PRIO_PROCESS = 0,

  PRIO_PGRP = 1,

  PRIO_USER = 2

};







extern int prlimit (__pid_t __pid, enum __rlimit_resource __resource,
      const struct rlimit *__new_limit,
      struct rlimit *__old_limit) __attribute__ ((__nothrow__ , __leaf__));
# 322 "/usr/include/x86_64-linux-gnu/bits/resource.h" 3 4
extern int prlimit64 (__pid_t __pid, enum __rlimit_resource __resource,
        const struct rlimit64 *__new_limit,
        struct rlimit64 *__old_limit) __attribute__ ((__nothrow__ , __leaf__));




# 25 "/usr/include/x86_64-linux-gnu/sys/resource.h" 2 3 4













typedef enum __rlimit_resource __rlimit_resource_t;
typedef enum __rusage_who __rusage_who_t;
typedef enum __priority_which __priority_which_t;
# 50 "/usr/include/x86_64-linux-gnu/sys/resource.h" 3 4
extern int getrlimit (__rlimit_resource_t __resource,
        struct rlimit *__rlimits) __attribute__ ((__nothrow__ , __leaf__));
# 61 "/usr/include/x86_64-linux-gnu/sys/resource.h" 3 4
extern int getrlimit64 (__rlimit_resource_t __resource,
   struct rlimit64 *__rlimits) __attribute__ ((__nothrow__ , __leaf__));






extern int setrlimit (__rlimit_resource_t __resource,
        const struct rlimit *__rlimits) __attribute__ ((__nothrow__ , __leaf__));
# 81 "/usr/include/x86_64-linux-gnu/sys/resource.h" 3 4
extern int setrlimit64 (__rlimit_resource_t __resource,
   const struct rlimit64 *__rlimits) __attribute__ ((__nothrow__ , __leaf__));




extern int getrusage (__rusage_who_t __who, struct rusage *__usage) __attribute__ ((__nothrow__ , __leaf__));





extern int getpriority (__priority_which_t __which, id_t __who) __attribute__ ((__nothrow__ , __leaf__));



extern int setpriority (__priority_which_t __which, id_t __who, int __prio)
     __attribute__ ((__nothrow__ , __leaf__));


# 450 "makeint.h" 2

# 450 "makeint.h"
extern struct rlimit stack_limit;


# 1 "/usr/include/glob.h" 1 3 4
# 23 "/usr/include/glob.h" 3 4

# 80 "/usr/include/glob.h" 3 4

# 80 "/usr/include/glob.h" 3 4
struct stat;

typedef struct
  {
    size_t gl_pathc;
    char **gl_pathv;
    size_t gl_offs;
    int gl_flags;



    void (*gl_closedir) (void *);

    struct dirent *(*gl_readdir) (void *);



    void *(*gl_opendir) (const char *);

    int (*gl_lstat) (const char *__restrict, struct stat *__restrict);
    int (*gl_stat) (const char *__restrict, struct stat *__restrict);




  } glob_t;



struct stat64;

typedef struct
  {
    size_t gl_pathc;
    char **gl_pathv;
    size_t gl_offs;
    int gl_flags;



    void (*gl_closedir) (void *);

    struct dirent64 *(*gl_readdir) (void *);



    void *(*gl_opendir) (const char *);

    int (*gl_lstat) (const char *__restrict, struct stat64 *__restrict);
    int (*gl_stat) (const char *__restrict, struct stat64 *__restrict);




  } glob64_t;
# 146 "/usr/include/glob.h" 3 4
extern int glob (const char *__restrict __pattern, int __flags,
   int (*__errfunc) (const char *, int),
   glob_t *__restrict __pglob) __attribute__ ((__nothrow__ , __leaf__));


extern void globfree (glob_t *__pglob) __attribute__ ((__nothrow__ , __leaf__));
# 162 "/usr/include/glob.h" 3 4
extern int glob64 (const char *__restrict __pattern, int __flags,
     int (*__errfunc) (const char *, int),
     glob64_t *__restrict __pglob) __attribute__ ((__nothrow__ , __leaf__));

extern void globfree64 (glob64_t *__pglob) __attribute__ ((__nothrow__ , __leaf__));
# 176 "/usr/include/glob.h" 3 4
extern int glob_pattern_p (const char *__pattern, int __quote) __attribute__ ((__nothrow__ , __leaf__));



# 454 "makeint.h" 2
# 473 "makeint.h"

# 473 "makeint.h"
typedef struct
  {
    const char *filenm;
    unsigned long lineno;
    unsigned long offset;
  } floc;

const char *concat (unsigned int, ...);
void message (int prefix, size_t length, const char *fmt, ...)
              __attribute__ ((__format__ (__printf__, 3, 4)));
void error (const floc *flocp, size_t length, const char *fmt, ...)
            __attribute__ ((__format__ (__printf__, 3, 4)));
void fatal (const floc *flocp, size_t length, const char *fmt, ...)
            __attribute__ ((noreturn, __format__ (__printf__, 3, 4)));
# 504 "makeint.h"
void die (int) __attribute__ ((noreturn));
void pfatal_with_name (const char *) __attribute__ ((noreturn));
void perror_with_name (const char *, const char *);

void *xmalloc (unsigned int);
void *xcalloc (unsigned int);
void *xrealloc (void *, unsigned int);
char *xstrdup (const char *);
char *xstrndup (const char *, unsigned int);
char *find_next_token (const char **, unsigned int *);
char *next_token (const char *);
char *end_of_token (const char *);
void collapse_continuations (char *);
char *lindex (const char *, const char *, int);
int alpha_compare (const void *, const void *);
void print_spaces (unsigned int);
char *find_percent (char *);
const char *find_percent_cached (const char **);


int ar_name (const char *);
void ar_parse_name (const char *, char **, char **);
int ar_touch (const char *);
time_t ar_member_date (const char *);

typedef long int (*ar_member_func_t) (int desc, const char *mem, int truncated,
                                      long int hdrpos, long int datapos,
                                      long int size, long int date, int uid,
                                      int gid, unsigned int mode,
                                      const void *arg);

long int ar_scan (const char *archive, ar_member_func_t function, const void *arg);
int ar_name_equal (const char *name, const char *mem, int truncated);

int ar_member_touch (const char *arname, const char *memname);



int dir_file_exists_p (const char *, const char *);
int file_exists_p (const char *);
int file_impossible_p (const char *);
void file_impossible (const char *);
const char *dir_name (const char *);
void print_dir_data_base (void);
void dir_setup_glob (glob_t *);
void hash_init_directories (void);

void define_default_variables (void);
void undefine_default_variables (void);
void set_default_suffixes (void);
void install_default_suffix_rules (void);
void install_default_implicit_rules (void);

void build_vpath_lists (void);
void construct_vpath_list (char *pattern, char *dirpath);
const char *vpath_search (const char *file, uintmax_t *mtime_ptr,
                          unsigned int* vpath_index, unsigned int* path_index);
int gpath_search (const char *file, unsigned int len);

void construct_include_path (const char **arg_dirs);

void user_access (void);
void make_access (void);
void child_access (void);

char *strip_whitespace (const char **begpp, const char **endpp);

void show_goal_error (void);


void strcache_init (void);
void strcache_print_stats (const char *prefix);
int strcache_iscached (const char *str);
const char *strcache_add (const char *str);
const char *strcache_add_len (const char *str, unsigned int len);


int guile_gmake_setup (const floc *flocp);


typedef int (*load_func_t)(const floc *flocp);
int load_file (const floc *flocp, const char **filename, int noerror);
void unload_file (const char *name);
# 637 "makeint.h"
extern char **environ;

extern const floc *reading_file;
extern const floc **expanding_var;

extern unsigned short stopchar_map[];

extern int just_print_flag, silent_flag, ignore_errors_flag, keep_going_flag;
extern int print_data_base_flag, question_flag, touch_flag, always_make_flag;
extern int env_overrides, no_builtin_rules_flag, no_builtin_variables_flag;
extern int print_version_flag, print_directory_flag, check_symlink_flag;
extern int warn_undefined_variables_flag, trace_flag, posix_pedantic;
extern int not_parallel, second_expansion, clock_skew_detected;
extern int rebuilding_makefiles, one_shell, output_sync, verify_flag;

extern const char *default_shell;


extern int batch_mode_shell;




extern char cmd_prefix;

extern unsigned int job_slots;

extern double max_load_average;







extern const char *program;
# 705 "makeint.h"
void remote_setup (void);
void remote_cleanup (void);
int start_remote_job_p (int);
int start_remote_job (char **, char **, int, int *, int *, int *);
int remote_status (int *, int *, int *, int);
void block_remote_children (void);
void unblock_remote_children (void);
int remote_kill (int id, int sig);
void print_variable_data_base (void);
void print_vpath_data_base (void);

extern char *starting_directory;
extern unsigned int makelevel;
extern char *version_string, *remote_description, *make_host;

extern unsigned int commands_started;

extern int handling_fatal_signal;
# 18 "remake.c" 2
# 1 "filedef.h" 1
# 22 "filedef.h"
# 1 "hash.h" 1
# 36 "hash.h"
typedef unsigned long (*hash_func_t) (void const *key);
typedef int (*hash_cmp_func_t) (void const *x, void const *y);
typedef void (*hash_map_func_t) (void const *item);
typedef void (*hash_map_arg_func_t) (void const *item, void *arg);

struct hash_table
{
  void **ht_vec;
  hash_func_t ht_hash_1;
  hash_func_t ht_hash_2;
  hash_cmp_func_t ht_compare;
  unsigned long ht_size;
  unsigned long ht_capacity;
  unsigned long ht_fill;
  unsigned long ht_empty_slots;
  unsigned long ht_collisions;
  unsigned long ht_lookups;
  unsigned int ht_rehashes;
};

typedef int (*qsort_cmp_t) (void const *, void const *);

void hash_init (struct hash_table *ht, unsigned long size, hash_func_t hash_1, hash_func_t hash_2, hash_cmp_func_t hash_cmp)
                                                                        ;
void hash_load (struct hash_table *ht, void *item_table, unsigned long cardinality, unsigned long size)
                                                     ;
void **hash_find_slot (struct hash_table *ht, void const *key);
void *hash_find_item (struct hash_table *ht, void const *key);
void *hash_insert (struct hash_table *ht, const void *item);
void *hash_insert_at (struct hash_table *ht, const void *item, void const *slot);
void *hash_delete (struct hash_table *ht, void const *item);
void *hash_delete_at (struct hash_table *ht, void const *slot);
void hash_delete_items (struct hash_table *ht);
void hash_free_items (struct hash_table *ht);
void hash_free (struct hash_table *ht, int free_items);
void hash_map (struct hash_table *ht, hash_map_func_t map);
void hash_map_arg (struct hash_table *ht, hash_map_arg_func_t map, void *arg);
void hash_print_stats (struct hash_table *ht, FILE *out_FILE);
void **hash_dump (struct hash_table *ht, void **vector_0, qsort_cmp_t compare);

extern void *hash_deleted_item;
# 23 "filedef.h" 2

struct file
  {
    const char *name;
    const char *hname;
    const char *vpath;
    struct dep *deps;
    struct commands *cmds;
    const char *stem;

    struct dep *also_make;
    struct file *prev;


    struct file *last;



    struct file *renamed;


    struct variable_set_list *variables;



    struct variable_set_list *pat_variables;



    struct file *parent;



    struct file *double_colon;

    uintmax_t last_mtime;
    uintmax_t mtime_before_update;

    int command_flags;
    enum update_status
      {
        us_success = 0,
        us_none,
        us_question,
        us_failed
      } update_status :2;
    enum cmd_state
      {
        cs_not_started = 0,
        cs_deps_running,
        cs_running,
        cs_finished
      } command_state :2;

    unsigned int builtin:1;
    unsigned int precious:1;
    unsigned int loaded:1;
    unsigned int low_resolution_time:1;

    unsigned int tried_implicit:1;


    unsigned int updating:1;
    unsigned int updated:1;
    unsigned int is_target:1;
    unsigned int cmd_target:1;
    unsigned int phony:1;

    unsigned int intermediate:1;
    unsigned int secondary:1;

    unsigned int dontcare:1;

    unsigned int ignore_vpath:1;
    unsigned int pat_searched:1;

    unsigned int considered:1;

    unsigned int no_diag:1;

  };


extern struct file *default_file;


struct file *lookup_file (const char *name);
struct file *enter_file (const char *name);
struct dep *split_prereqs (char *prereqstr);
struct dep *enter_prereqs (struct dep *prereqs, const char *stem);
void remove_intermediates (int sig);
void snap_deps (void);
void rename_file (struct file *file, const char *name);
void rehash_file (struct file *file, const char *name);
void set_command_state (struct file *file, enum cmd_state state);
void notice_finished_file (struct file *file);
void init_hash_files (void);
void verify_file_data_base (void);
char *build_target_list (char *old_list);
void print_prereqs (const struct dep *deps);
void print_file_data_base (void);
int try_implicit_rule (struct file *file, unsigned int depth);
int stemlen_compare (const void *v1, const void *v2);
# 166 "filedef.h"
uintmax_t file_timestamp_cons (char const *, time_t, long int);
uintmax_t file_timestamp_now (int *);
void file_timestamp_sprintf (char *p, uintmax_t ts);
# 178 "filedef.h"
uintmax_t f_mtime (struct file *file, int search);
# 212 "filedef.h"
extern int snapped_deps;
# 19 "remake.c" 2
# 1 "job.h" 1
# 17 "job.h"
# 1 "output.h" 1
# 17 "output.h"
struct output
  {
    int out;
    int err;
    unsigned int syncout:1;
 };

extern struct output *output_context;
extern unsigned int stdio_traced;







FILE *output_tmpfile (char **, const char *);



void output_init (struct output *out);
void output_close (struct output *out);



void output_start (void);


void outputs (int is_err, const char *msg);


int output_tmpfd (void);

void output_dump (struct output *out);
# 18 "job.h" 2


# 1 "/usr/include/fcntl.h" 1 3 4
# 28 "/usr/include/fcntl.h" 3 4







# 1 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 3 4

# 35 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 3 4
struct flock
  {
    short int l_type;
    short int l_whence;

    __off_t l_start;
    __off_t l_len;




    __pid_t l_pid;
  };


struct flock64
  {
    short int l_type;
    short int l_whence;
    __off64_t l_start;
    __off64_t l_len;
    __pid_t l_pid;
  };



# 1 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/uio.h" 1 3 4
# 43 "/usr/include/x86_64-linux-gnu/bits/uio.h" 3 4
struct iovec
  {
    void *iov_base;
    size_t iov_len;
  };
# 39 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 2 3 4
# 241 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
enum __pid_type
  {
    F_OWNER_TID = 0,
    F_OWNER_PID,
    F_OWNER_PGRP,
    F_OWNER_GID = F_OWNER_PGRP
  };


struct f_owner_ex
  {
    enum __pid_type type;
    __pid_t pid;
  };
# 311 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
struct file_handle
{
  unsigned int handle_bytes;
  int handle_type;

  unsigned char f_handle[0];
};
# 341 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4





extern ssize_t readahead (int __fd, __off64_t __offset, size_t __count)
    __attribute__ ((__nothrow__ , __leaf__));






extern int sync_file_range (int __fd, __off64_t __offset, __off64_t __count,
       unsigned int __flags);






extern ssize_t vmsplice (int __fdout, const struct iovec *__iov,
    size_t __count, unsigned int __flags);





extern ssize_t splice (int __fdin, __off64_t *__offin, int __fdout,
         __off64_t *__offout, size_t __len,
         unsigned int __flags);





extern ssize_t tee (int __fdin, int __fdout, size_t __len,
      unsigned int __flags);






extern int fallocate (int __fd, int __mode, __off_t __offset, __off_t __len);
# 396 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
extern int fallocate64 (int __fd, int __mode, __off64_t __offset,
   __off64_t __len);




extern int name_to_handle_at (int __dfd, const char *__name,
         struct file_handle *__handle, int *__mnt_id,
         int __flags) __attribute__ ((__nothrow__ , __leaf__));





extern int open_by_handle_at (int __mountdirfd, struct file_handle *__handle,
         int __flags);




# 61 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 2 3 4
# 36 "/usr/include/fcntl.h" 2 3 4
# 68 "/usr/include/fcntl.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stat.h" 1 3 4
# 69 "/usr/include/fcntl.h" 2 3 4
# 137 "/usr/include/fcntl.h" 3 4
extern int fcntl (int __fd, int __cmd, ...);
# 146 "/usr/include/fcntl.h" 3 4
extern int open (const char *__file, int __oflag, ...) __attribute__ ((__nonnull__ (1)));
# 156 "/usr/include/fcntl.h" 3 4
extern int open64 (const char *__file, int __oflag, ...) __attribute__ ((__nonnull__ (1)));
# 170 "/usr/include/fcntl.h" 3 4
extern int openat (int __fd, const char *__file, int __oflag, ...)
     __attribute__ ((__nonnull__ (2)));
# 181 "/usr/include/fcntl.h" 3 4
extern int openat64 (int __fd, const char *__file, int __oflag, ...)
     __attribute__ ((__nonnull__ (2)));
# 192 "/usr/include/fcntl.h" 3 4
extern int creat (const char *__file, mode_t __mode) __attribute__ ((__nonnull__ (1)));
# 202 "/usr/include/fcntl.h" 3 4
extern int creat64 (const char *__file, mode_t __mode) __attribute__ ((__nonnull__ (1)));
# 238 "/usr/include/fcntl.h" 3 4
extern int posix_fadvise (int __fd, off_t __offset, off_t __len,
     int __advise) __attribute__ ((__nothrow__ , __leaf__));
# 250 "/usr/include/fcntl.h" 3 4
extern int posix_fadvise64 (int __fd, off64_t __offset, off64_t __len,
       int __advise) __attribute__ ((__nothrow__ , __leaf__));
# 260 "/usr/include/fcntl.h" 3 4
extern int posix_fallocate (int __fd, off_t __offset, off_t __len);
# 271 "/usr/include/fcntl.h" 3 4
extern int posix_fallocate64 (int __fd, off64_t __offset, off64_t __len);
# 282 "/usr/include/fcntl.h" 3 4

# 21 "job.h" 2
# 78 "job.h"

# 78 "job.h"
typedef int sync_handle_t;
# 87 "job.h"
struct child
  {
    struct child *next;

    struct file *file;

    char **environment;
    char *sh_batch_file;
    char **command_lines;
    char *command_ptr;
# 105 "job.h"
    unsigned int command_line;
    struct output output;
    pid_t pid;
    unsigned int remote:1;
    unsigned int noerror:1;
    unsigned int good_stdin:1;
    unsigned int deleted:1;
    unsigned int recursive:1;
    unsigned int dontcare:1;
  };

extern struct child *children;


void child_handler (int sig);
int is_bourne_compatible_shell(const char *path);
void new_job (struct file *file);
void reap_children (int block, int err);
void start_waiting_jobs (void);

char **construct_command_argv (char *line, char **restp, struct file *file,
                               int cmd_flags, char** batch_file);







int child_execute_job (struct output *out, int good_stdin, char **argv, char **envp);







void exec_command (char **argv, char **envp) __attribute__ ((noreturn));


extern unsigned int job_slots_used;

void block_sigs (void);

void unblock_sigs (void);
# 159 "job.h"
extern unsigned int jobserver_tokens;
# 20 "remake.c" 2
# 1 "commands.h" 1
# 20 "commands.h"
struct commands
  {
    floc fileinfo;
    char *commands;
    char **command_lines;
    unsigned char *lines_flags;
    unsigned short ncommand_lines;
    char recipe_prefix;
    unsigned int any_recurse:1;

  };






void fatal_error_signal (int sig);
void execute_file_commands (struct file *file);
void print_commands (const struct commands *cmds);
void delete_child_targets (struct child *child);
void chop_commands (struct commands *cmds);
void set_file_variables (struct file *file);
# 21 "remake.c" 2
# 1 "dep.h" 1
# 24 "dep.h"
struct nameseq
  {
    struct nameseq *next; const char *name;
  };
# 53 "dep.h"
struct dep
  {
    struct dep *next; const char *name; struct file *file; const char *stem; unsigned short flags : 8; unsigned short changed : 1; unsigned short ignore_mtime : 1; unsigned short staticpattern : 1; unsigned short need_2nd_expansion : 1;
  };





struct goaldep
  {
    struct goaldep *next; const char *name; struct file *file; const char *stem; unsigned short flags : 8; unsigned short changed : 1; unsigned short ignore_mtime : 1; unsigned short staticpattern : 1; unsigned short need_2nd_expansion : 1;
    unsigned short error;
    floc floc;
  };
# 86 "dep.h"
void *parse_file_seq (char **stringp, unsigned int size,
                      int stopmap, const char *prefix, int flags);


char *tilde_expand (const char *name);


struct nameseq *ar_glob (const char *arname, const char *member_pattern, unsigned int size);





void free_ns_chain (struct nameseq *n);
# 127 "dep.h"
struct dep *copy_dep_chain (const struct dep *d);

struct goaldep *read_all_makefiles (const char **makefiles);
void eval_buffer (char *buffer, const floc *floc);
enum update_status update_goal_chain (struct goaldep *goals);
# 22 "remake.c" 2
# 1 "variable.h" 1
# 21 "variable.h"
enum variable_origin
  {
    o_default,
    o_env,
    o_file,
    o_env_override,
    o_command,
    o_override,
    o_automatic,
    o_invalid
  };

enum variable_flavor
  {
    f_bogus,
    f_simple,
    f_recursive,
    f_append,
    f_conditional,
    f_shell
  };
# 50 "variable.h"
struct variable
  {
    char *name;
    char *value;
    floc fileinfo;
    int length;
    unsigned int recursive:1;
    unsigned int append:1;

    unsigned int conditional:1;
    unsigned int per_target:1;
    unsigned int special:1;
    unsigned int exportable:1;

    unsigned int expanding:1;
    unsigned int private_var:1;

    unsigned int exp_count:15;


    enum variable_flavor
      flavor :3;
    enum variable_origin
      origin :3;
    enum variable_export
      {
        v_export,
        v_noexport,
        v_ifset,
        v_default
      } export :2;
  };



struct variable_set
  {
    struct hash_table table;
  };



struct variable_set_list
  {
    struct variable_set_list *next;
    struct variable_set *set;
    int next_is_parent;
  };



struct pattern_var
  {
    struct pattern_var *next;
    const char *suffix;
    const char *target;
    unsigned int len;
    struct variable variable;
  };

extern char *variable_buffer;
extern struct variable_set_list *current_variable_set_list;
extern struct variable *default_goal_var;
extern struct variable shell_var;


char *variable_buffer_output (char *ptr, const char *string, unsigned int length);
char *variable_expand (const char *line);
char *variable_expand_for_file (const char *line, struct file *file);
char *allocated_variable_expand_for_file (const char *line, struct file *file);


char *expand_argument (const char *str, const char *end);
char *variable_expand_string (char *line, const char *string, long length);
void install_variable_buffer (char **bufp, unsigned int *lenp);
void restore_variable_buffer (char *buf, unsigned int len);


int handle_function (char **op, const char **stringp);
int pattern_matches (const char *pattern, const char *percent, const char *str);
char *subst_expand (char *o, const char *text, const char *subst,
                    const char *replace, unsigned int slen, unsigned int rlen,
                    int by_word);
char *patsubst_expand_pat (char *o, const char *text, const char *pattern,
                           const char *replace, const char *pattern_percent,
                           const char *replace_percent);
char *patsubst_expand (char *o, const char *text, char *pattern, char *replace);
char *func_shell_base (char *o, char **argv, int trim_newlines);
void shell_completed (int exit_code, int exit_sig);


char *recursively_expand_for_file (struct variable *v, struct file *file);



struct variable_set_list *create_new_variable_set (void);
void free_variable_set (struct variable_set_list *);
struct variable_set_list *push_new_variable_scope (void);
void pop_variable_scope (void);
void define_automatic_variables (void);
void initialize_file_variables (struct file *file, int reading);
void print_file_variables (const struct file *file);
void print_target_variables (const struct file *file);
void merge_variable_set_lists (struct variable_set_list **to_list,
                               struct variable_set_list *from_list);
struct variable *do_variable_definition (const floc *flocp,
                                         const char *name, const char *value,
                                         enum variable_origin origin,
                                         enum variable_flavor flavor,
                                         int target_var);
char *parse_variable_definition (const char *line,
                                 struct variable *v);
struct variable *assign_variable_definition (struct variable *v, const char *line);
struct variable *try_variable_definition (const floc *flocp, const char *line,
                                          enum variable_origin origin,
                                          int target_var);
void init_hash_global_variable_set (void);
void hash_init_function_table (void);
void define_new_function(const floc *flocp, const char *name,
                         unsigned int min, unsigned int max, unsigned int flags,
                         gmk_func_ptr func);
struct variable *lookup_variable (const char *name, unsigned int length);
struct variable *lookup_variable_in_set (const char *name, unsigned int length,
                                         const struct variable_set *set);

struct variable *define_variable_in_set (const char *name, unsigned int length,
                                         const char *value,
                                         enum variable_origin origin,
                                         int recursive,
                                         struct variable_set *set,
                                         const floc *flocp);
# 210 "variable.h"
void undefine_variable_in_set (const char *name, unsigned int length,
                               enum variable_origin origin,
                               struct variable_set *set);
# 228 "variable.h"
char **target_environment (struct file *file);

struct pattern_var *create_pattern_var (const char *target,
                                        const char *suffix);

extern int export_all_variables;
# 23 "remake.c" 2
# 1 "debug.h" 1
# 26 "debug.h"
extern int db_level;
# 24 "remake.c" 2

# 1 "/usr/include/assert.h" 1 3 4
# 26 "remake.c" 2
# 54 "remake.c"
unsigned int commands_started = 0;


static struct goaldep *goal_list;
static struct dep *goal_dep;


static unsigned int considered;

static enum update_status update_file (struct file *file, unsigned int depth);
static enum update_status update_file_1 (struct file *file, unsigned int depth);
static enum update_status check_dep (struct file *file, unsigned int depth,
                                     uintmax_t this_mtime, int *must_make);
static enum update_status touch_file (struct file *file);
static void remake_file (struct file *file);
static uintmax_t name_mtime (const char *name);
static const char *library_search (const char *lib, uintmax_t *mtime_ptr);
# 81 "remake.c"
enum update_status
update_goal_chain (struct goaldep *goaldeps)
{
  int t = touch_flag, q = question_flag, n = just_print_flag;
  enum update_status status = us_none;



  struct dep *goals = copy_dep_chain ((struct dep *)goaldeps);

  goal_list = rebuilding_makefiles ? goaldeps : 
# 91 "remake.c" 3 4
                                               ((void *)0)
# 91 "remake.c"
                                                   ;


  considered = 1;






  while (goals != 0)
    {
      register struct dep *g, *lastgoal;



      start_waiting_jobs ();



      reap_children (1, 0);

      lastgoal = 0;
      g = goals;
      while (g != 0)
        {

          struct file *file;
          int stop = 0, any_not_updated = 0;

          goal_dep = g;

          for (file = g->file->double_colon ? g->file->double_colon : g->file;
               file != 
# 124 "remake.c" 3 4
                      ((void *)0)
# 124 "remake.c"
                          ;
               file = file->prev)
            {
              unsigned int ocommands_started;
              enum update_status fail;

              file->dontcare = (((g->flags)&((1 << 2))) != 0);

              while ((file)->renamed != 0) (file) = (file)->renamed;
              if (rebuilding_makefiles)
                {
                  if (file->cmd_target)
                    {
                      touch_flag = t;
                      question_flag = q;
                      just_print_flag = n;
                    }
                  else
                    touch_flag = question_flag = just_print_flag = 0;
                }




              ocommands_started = commands_started;

              fail = update_file (file, rebuilding_makefiles ? 1 : 0);
              while ((file)->renamed != 0) (file) = (file)->renamed;




              if (commands_started > ocommands_started)
                g->changed = 1;

              stop = 0;
              if ((fail || file->updated) && status < us_question)
                {


                  if (file->update_status)
                    {


                      status = file->update_status;



                      stop = (question_flag && !keep_going_flag
                              && !rebuilding_makefiles);
                    }
                  else
                    {
                      uintmax_t mtime = (rebuilding_makefiles ? (((file))->last_mtime == 0 ? f_mtime (((file)), 0) : ((file))->last_mtime) : (((file))->last_mtime == 0 ? f_mtime (((file)), 1) : ((file))->last_mtime));
                      while ((file)->renamed != 0) (file) = (file)->renamed;

                      if (file->updated && g->changed &&
                           mtime != file->mtime_before_update)
                        {






                          if (!rebuilding_makefiles
                              || (!just_print_flag && !question_flag))
                            status = us_success;
                          if (rebuilding_makefiles && file->dontcare)

                            stop = 1;
                        }
                    }
                }



              any_not_updated |= !file->updated;

              file->dontcare = 0;

              if (stop)
                break;
            }


          file = g->file;

          if (stop || !any_not_updated)
            {



              if (!rebuilding_makefiles



                  && file->update_status == us_success && !g->changed

                  && !silent_flag && !question_flag)
                message((1), strlen (file->name), (((file->phony || file->cmds == 0) ? 
# 224 "remake.c" 3 4
               dcgettext (((void *)0), 
# 224 "remake.c"
               "Nothing to be done for '%s'."
# 224 "remake.c" 3 4
               , 5) 
# 224 "remake.c"
               : 
# 224 "remake.c" 3 4
               dcgettext (((void *)0), 
# 224 "remake.c"
               "'%s' is up to date."
# 224 "remake.c" 3 4
               , 5)
# 224 "remake.c"
               )), (file->name))


                               ;


              if (lastgoal == 0)
                goals = g->next;
              else
                lastgoal->next = g->next;


              free (g);

              g = lastgoal == 0 ? goals : lastgoal->next;

              if (stop)
                break;
            }
          else
            {
              lastgoal = g;
              g = g->next;
            }
        }



      if (g == 0)
        considered = !considered;
    }

  if (rebuilding_makefiles)
    {
      touch_flag = t;
      question_flag = q;
      just_print_flag = n;
    }

  return status;
}




void
show_goal_error (void)
{
  struct goaldep *goal;

  if ((goal_dep->flags & ((1 << 1)|(1 << 2))) != (1 << 1))
    return;

  for (goal = goal_list; goal; goal = goal->next)
    if (goal_dep->file == goal->file)
      {
        if (goal->error)
          {
            error((&goal->floc), strlen (goal->file->name) + strlen (strerror ((int)goal->error)), ("%s: %s"), (goal->file->name), (strerror ((int)goal->error)))
                                                               ;
            goal->error = 0;
          }
        return;
      }
}
# 302 "remake.c"
static enum update_status
update_file (struct file *file, unsigned int depth)
{
  enum update_status status = us_success;
  struct file *f;

  f = file->double_colon ? file->double_colon : file;





  if (f->considered == considered)
    {



      if (!(f->updated && f->update_status > us_none
            && !f->dontcare && f->no_diag))
        {
          do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 322 "remake.c" 3 4
         dcgettext (((void *)0), 
# 322 "remake.c"
         "Pruning file '%s'.\n"
# 322 "remake.c" 3 4
         , 5)
# 322 "remake.c"
         , file->name); fflush (
# 322 "remake.c" 3 4
         stdout
# 322 "remake.c"
         );} }while(0);
          return f->command_state == cs_finished ? f->update_status : us_success;
        }
    }



  for (; f != 0; f = f->prev)
    {
      enum update_status new;

      f->considered = considered;

      new = update_file_1 (f, depth);
      while ((f)->renamed != 0) (f) = (f)->renamed;


      
# 339 "remake.c" 3 4
     __builtin_alloca (
# 339 "remake.c"
     0
# 339 "remake.c" 3 4
     )
# 339 "remake.c"
               ;


      if (new && !keep_going_flag)
        return new;

      if (f->command_state == cs_running
          || f->command_state == cs_deps_running)


        return us_success;

      if (new > status)
        status = new;
    }



  if (file->double_colon)
    for (; f != 0 ; f = f->prev)
      {
        struct dep *d;

        f->considered = considered;

        for (d = f->deps; d != 0; d = d->next)
          {
            enum update_status new = update_file (d->file, depth + 1);
            if (new > status)
              status = new;
          }
      }

  return status;
}



static void
complain (struct file *file)
{






  struct dep *d;

  for (d = file->deps; d != 0; d = d->next)
    {
      if (d->file->updated && d->file->update_status > us_none && file->no_diag)
        {
          complain (d->file);
          break;
        }
    }

  if (d == 0)
    {
      show_goal_error ();


      if (file->parent)
        {
          size_t l = strlen (file->name) + strlen (file->parent->name) + 4;
          const char *m = 
# 405 "remake.c" 3 4
                         dcgettext (((void *)0), 
# 405 "remake.c"
                         "%sNo rule to make target '%s', needed by '%s'%s"
# 405 "remake.c" 3 4
                         , 5)
# 405 "remake.c"
                                                                             ;

          if (!keep_going_flag)
            fatal (((floc *)0), l, m, "", file->name, file->parent->name, "");

          error (((floc *)0), l, m, "*** ", file->name, file->parent->name, ".");
        }
      else
        {
          size_t l = strlen (file->name) + 4;
          const char *m = 
# 415 "remake.c" 3 4
                         dcgettext (((void *)0), 
# 415 "remake.c"
                         "%sNo rule to make target '%s'%s"
# 415 "remake.c" 3 4
                         , 5)
# 415 "remake.c"
                                                             ;

          if (!keep_going_flag)
            fatal (((floc *)0), l, m, "", file->name, "");

          error (((floc *)0), l, m, "*** ", file->name, ".");
        }

      file->no_diag = 0;
    }
}




static enum update_status
update_file_1 (struct file *file, unsigned int depth)
{
  enum update_status dep_status = us_success;
  uintmax_t this_mtime;
  int noexist, must_make, deps_changed;
  struct file *ofile;
  struct dep *d, *ad;
  struct dep amake;
  int running = 0;

  do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 441 "remake.c" 3 4
 dcgettext (((void *)0), 
# 441 "remake.c"
 "Considering target file '%s'.\n"
# 441 "remake.c" 3 4
 , 5)
# 441 "remake.c"
 , file->name); fflush (
# 441 "remake.c" 3 4
 stdout
# 441 "remake.c"
 );} }while(0);

  if (file->updated)
    {
      if (file->update_status > us_none)
        {
          do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 447 "remake.c" 3 4
         dcgettext (((void *)0), 
# 447 "remake.c"
         "Recently tried and failed to update file '%s'.\n"
# 447 "remake.c" 3 4
         , 5)
# 447 "remake.c"
         , file->name); fflush (
# 447 "remake.c" 3 4
         stdout
# 447 "remake.c"
         );} }while(0)
                                                                     ;





          if (file->no_diag && !file->dontcare)
              complain (file);

          return file->update_status;
        }

      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 460 "remake.c" 3 4
     dcgettext (((void *)0), 
# 460 "remake.c"
     "File '%s' was considered already.\n"
# 460 "remake.c" 3 4
     , 5)
# 460 "remake.c"
     , file->name); fflush (
# 460 "remake.c" 3 4
     stdout
# 460 "remake.c"
     );} }while(0);
      return 0;
    }

  switch (file->command_state)
    {
    case cs_not_started:
    case cs_deps_running:
      break;
    case cs_running:
      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 470 "remake.c" 3 4
     dcgettext (((void *)0), 
# 470 "remake.c"
     "Still updating file '%s'.\n"
# 470 "remake.c" 3 4
     , 5)
# 470 "remake.c"
     , file->name); fflush (
# 470 "remake.c" 3 4
     stdout
# 470 "remake.c"
     );} }while(0);
      return 0;
    case cs_finished:
      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 473 "remake.c" 3 4
     dcgettext (((void *)0), 
# 473 "remake.c"
     "Finished updating file '%s'.\n"
# 473 "remake.c" 3 4
     , 5)
# 473 "remake.c"
     , file->name); fflush (
# 473 "remake.c" 3 4
     stdout
# 473 "remake.c"
     );} }while(0);
      return file->update_status;
    default:
      abort ();
    }



  file->no_diag = file->dontcare;

  ++depth;


  (((file)->double_colon ? (file)->double_colon : (file)) ->updating = 1);



  ofile = file;







  this_mtime = (((file))->last_mtime == 0 ? f_mtime (((file)), 1) : ((file))->last_mtime);
  while ((file)->renamed != 0) (file) = (file)->renamed;
  noexist = this_mtime == 1;
  if (noexist)
    do{ if((((0x001))&db_level)) {print_spaces (depth); printf (
# 502 "remake.c" 3 4
   dcgettext (((void *)0), 
# 502 "remake.c"
   "File '%s' does not exist.\n"
# 502 "remake.c" 3 4
   , 5)
# 502 "remake.c"
   , file->name); fflush (
# 502 "remake.c" 3 4
   stdout
# 502 "remake.c"
   );} }while(0);
  else if ((2 + 1) <= this_mtime && this_mtime <= ((((((~ (uintmax_t) 0 - (! ((uintmax_t) -1 <= 0) ? (uintmax_t) 0 : ~ (uintmax_t) 0 << (sizeof (uintmax_t) * 8 - 1)))) - (2 + 1)) >> (1 ? 30 : 0)) << (1 ? 30 : 0)) + (2 + 1) + (1 ? 1000000000 : 1) - 1)
           && file->low_resolution_time)
    {

      int ns = ((int) (((this_mtime) - (2 + 1)) & ((1 << (1 ? 30 : 0)) - 1)));
      if (ns != 0)
        error((((floc *)0)), strlen (file->name), (
# 509 "remake.c" 3 4
       dcgettext (((void *)0), 
# 509 "remake.c"
       "*** Warning: .LOW_RESOLUTION_TIME file '%s' has a high resolution time stamp"
# 509 "remake.c" 3 4
       , 5)
# 509 "remake.c"
       ), (file->name))

                       ;
      this_mtime += (1 ? 1000000000 : 1) - 1 - ns;
    }

  must_make = noexist;




  if (!file->phony && file->cmds == 0 && !file->tried_implicit)
    {
      if (try_implicit_rule (file, depth))
        do{ if((((0x008))&db_level)) {print_spaces (depth); printf (
# 523 "remake.c" 3 4
       dcgettext (((void *)0), 
# 523 "remake.c"
       "Found an implicit rule for '%s'.\n"
# 523 "remake.c" 3 4
       , 5)
# 523 "remake.c"
       , file->name); fflush (
# 523 "remake.c" 3 4
       stdout
# 523 "remake.c"
       );} }while(0);
      else
        do{ if((((0x008))&db_level)) {print_spaces (depth); printf (
# 525 "remake.c" 3 4
       dcgettext (((void *)0), 
# 525 "remake.c"
       "No implicit rule found for '%s'.\n"
# 525 "remake.c" 3 4
       , 5)
# 525 "remake.c"
       , file->name); fflush (
# 525 "remake.c" 3 4
       stdout
# 525 "remake.c"
       );} }while(0);
      file->tried_implicit = 1;
    }
  if (file->cmds == 0 && !file->is_target
      && default_file != 0 && default_file->cmds != 0)
    {
      do{ if((((0x008))&db_level)) {print_spaces (depth); printf (
# 531 "remake.c" 3 4
     dcgettext (((void *)0), 
# 531 "remake.c"
     "Using default recipe for '%s'.\n"
# 531 "remake.c" 3 4
     , 5)
# 531 "remake.c"
     , file->name); fflush (
# 531 "remake.c" 3 4
     stdout
# 531 "remake.c"
     );} }while(0);
      file->cmds = default_file->cmds;
    }






  amake.file = file;
  amake.next = file->also_make;
  ad = &amake;
  while (ad)
    {
      struct dep *lastd = 0;


      d = ad->file->deps;
      ad = ad->next;

      while (d)
        {
          enum update_status new;
          uintmax_t mtime;
          int maybe_make;
          int dontcare = 0;

          while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;

          mtime = (((d->file))->last_mtime == 0 ? f_mtime (((d->file)), 1) : ((d->file))->last_mtime);
          while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;

          if ((((d->file)->double_colon ? (d->file)->double_colon : (d->file)) ->updating))
            {
              error((((floc *)0)), strlen (file->name) + strlen (d->file->name), (
# 565 "remake.c" 3 4
             dcgettext (((void *)0), 
# 565 "remake.c"
             "Circular %s <- %s dependency dropped."
# 565 "remake.c" 3 4
             , 5)
# 565 "remake.c"
             ), (file->name), (d->file->name))
                                             ;



              if (lastd == 0)
                file->deps = d->next;
              else
                lastd->next = d->next;
              d = d->next;
              continue;
            }

          d->file->parent = file;
          maybe_make = must_make;


          if (rebuilding_makefiles)
            {
              dontcare = d->file->dontcare;
              d->file->dontcare = file->dontcare;
            }

          new = check_dep (d->file, depth, this_mtime, &maybe_make);
          if (new > dep_status)
            dep_status = new;


          if (rebuilding_makefiles)
            d->file->dontcare = dontcare;

          if (! d->ignore_mtime)
            must_make = maybe_make;

          while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;

          {
            register struct file *f = d->file;
            if (f->double_colon)
              f = f->double_colon;
            do
              {
                running |= (f->command_state == cs_running
                            || f->command_state == cs_deps_running);
                f = f->prev;
              }
            while (f != 0);
          }

          if (dep_status && !keep_going_flag)
            break;

          if (!running)


            d->changed = (((((d->file))->last_mtime == 0 ? f_mtime (((d->file)), 1) : ((d->file))->last_mtime) != mtime)
                          || (mtime == 1));

          lastd = d;
          d = d->next;
        }
    }




  if (must_make || always_make_flag)
    {
      for (d = file->deps; d != 0; d = d->next)
        if (d->file->intermediate)
          {
            enum update_status new;
            int dontcare = 0;

            uintmax_t mtime = (((d->file))->last_mtime == 0 ? f_mtime (((d->file)), 1) : ((d->file))->last_mtime);
            while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;
            d->file->parent = file;


            if (rebuilding_makefiles)
              {
                dontcare = d->file->dontcare;
                d->file->dontcare = file->dontcare;
              }




            d->file->considered = !considered;

            new = update_file (d->file, depth);
            if (new > dep_status)
              dep_status = new;


            if (rebuilding_makefiles)
              d->file->dontcare = dontcare;

            while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;

            {
              register struct file *f = d->file;
              if (f->double_colon)
                f = f->double_colon;
              do
                {
                  running |= (f->command_state == cs_running
                              || f->command_state == cs_deps_running);
                  f = f->prev;
                }
              while (f != 0);
            }

            if (dep_status && !keep_going_flag)
              break;

            if (!running)
              d->changed = ((file->phony && file->cmds != 0)
                            || (((d->file))->last_mtime == 0 ? f_mtime (((d->file)), 1) : ((d->file))->last_mtime) != mtime);
          }
    }

  (((file)->double_colon ? (file)->double_colon : (file)) ->updating = 0);
  (((ofile)->double_colon ? (ofile)->double_colon : (ofile)) ->updating = 0);

  do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 690 "remake.c" 3 4
 dcgettext (((void *)0), 
# 690 "remake.c"
 "Finished prerequisites of target file '%s'.\n"
# 690 "remake.c" 3 4
 , 5)
# 690 "remake.c"
 , file->name); fflush (
# 690 "remake.c" 3 4
 stdout
# 690 "remake.c"
 );} }while(0);

  if (running)
    {
      set_command_state (file, cs_deps_running);
      --depth;
      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 696 "remake.c" 3 4
     dcgettext (((void *)0), 
# 696 "remake.c"
     "The prerequisites of '%s' are being made.\n"
# 696 "remake.c" 3 4
     , 5)
# 696 "remake.c"
     , file->name); fflush (
# 696 "remake.c" 3 4
     stdout
# 696 "remake.c"
     );} }while(0);
      return 0;
    }



  if (dep_status)
    {

      file->update_status = dep_status == us_none ? us_failed : dep_status;
      notice_finished_file (file);

      --depth;

      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 710 "remake.c" 3 4
     dcgettext (((void *)0), 
# 710 "remake.c"
     "Giving up on target file '%s'.\n"
# 710 "remake.c" 3 4
     , 5)
# 710 "remake.c"
     , file->name); fflush (
# 710 "remake.c" 3 4
     stdout
# 710 "remake.c"
     );} }while(0);

      if (depth == 0 && keep_going_flag
          && !just_print_flag && !question_flag)
        error((((floc *)0)), strlen (file->name), (
# 714 "remake.c" 3 4
       dcgettext (((void *)0), 
# 714 "remake.c"
       "Target '%s' not remade because of errors."
# 714 "remake.c" 3 4
       , 5)
# 714 "remake.c"
       ), (file->name))
                                                                       ;

      return dep_status;
    }

  if (file->command_state == cs_deps_running)
# 729 "remake.c"
    set_command_state (file, cs_not_started);




  deps_changed = 0;
  for (d = file->deps; d != 0; d = d->next)
    {
      uintmax_t d_mtime = (((d->file))->last_mtime == 0 ? f_mtime (((d->file)), 1) : ((d->file))->last_mtime);
      while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;

      if (! d->ignore_mtime)
        {




          if (d_mtime == 1 && !d->file->intermediate)


            must_make = 1;



          deps_changed |= d->changed;
        }



      d->changed |= noexist || d_mtime > this_mtime;

      if (!noexist && (((0x001)|(0x002))&db_level))
        {
          const char *fmt = 0;

          if (d->ignore_mtime)
            {
              if ((((0x002))&db_level))
                fmt = 
# 767 "remake.c" 3 4
                     dcgettext (((void *)0), 
# 767 "remake.c"
                     "Prerequisite '%s' is order-only for target '%s'.\n"
# 767 "remake.c" 3 4
                     , 5)
# 767 "remake.c"
                                                                            ;
            }
          else if (d_mtime == 1)
            {
              if ((((0x001))&db_level))
                fmt = 
# 772 "remake.c" 3 4
                     dcgettext (((void *)0), 
# 772 "remake.c"
                     "Prerequisite '%s' of target '%s' does not exist.\n"
# 772 "remake.c" 3 4
                     , 5)
# 772 "remake.c"
                                                                            ;
            }
          else if (d->changed)
            {
              if ((((0x001))&db_level))
                fmt = 
# 777 "remake.c" 3 4
                     dcgettext (((void *)0), 
# 777 "remake.c"
                     "Prerequisite '%s' is newer than target '%s'.\n"
# 777 "remake.c" 3 4
                     , 5)
# 777 "remake.c"
                                                                        ;
            }
          else if ((((0x002))&db_level))
            fmt = 
# 780 "remake.c" 3 4
                 dcgettext (((void *)0), 
# 780 "remake.c"
                 "Prerequisite '%s' is older than target '%s'.\n"
# 780 "remake.c" 3 4
                 , 5)
# 780 "remake.c"
                                                                    ;

          if (fmt)
            {
              print_spaces (depth);
              printf (fmt, ((d)->name ? (d)->name : (d)->file->name), file->name);
              fflush (
# 786 "remake.c" 3 4
                     stdout
# 786 "remake.c"
                           );
            }
        }
    }


  depth--;

  if (file->double_colon && file->deps == 0)
    {
      must_make = 1;
      do{ if((((0x001))&db_level)) {print_spaces (depth); printf (
# 797 "remake.c" 3 4
     dcgettext (((void *)0), 
# 797 "remake.c"
     "Target '%s' is double-colon and has no prerequisites.\n"
# 797 "remake.c" 3 4
     , 5)
# 797 "remake.c"
     , file->name); fflush (
# 797 "remake.c" 3 4
     stdout
# 797 "remake.c"
     );} }while(0)
                                                                        ;
    }
  else if (!noexist && file->is_target && !deps_changed && file->cmds == 0
           && !always_make_flag)
    {
      must_make = 0;
      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 804 "remake.c" 3 4
     dcgettext (((void *)0), 
# 804 "remake.c"
     "No recipe for '%s' and no prerequisites actually changed.\n"
# 804 "remake.c" 3 4
     , 5)
# 804 "remake.c"
     , file->name); fflush (
# 804 "remake.c" 3 4
     stdout
# 804 "remake.c"
     );} }while(0)
                                                                            ;
    }
  else if (!must_make && file->cmds != 0 && always_make_flag)
    {
      must_make = 1;
      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 810 "remake.c" 3 4
     dcgettext (((void *)0), 
# 810 "remake.c"
     "Making '%s' due to always-make flag.\n"
# 810 "remake.c" 3 4
     , 5)
# 810 "remake.c"
     , file->name); fflush (
# 810 "remake.c" 3 4
     stdout
# 810 "remake.c"
     );} }while(0);
    }

  if (!must_make)
    {
      if ((((0x002))&db_level))
        {
          print_spaces (depth);
          printf (
# 818 "remake.c" 3 4
                 dcgettext (((void *)0), 
# 818 "remake.c"
                 "No need to remake target '%s'"
# 818 "remake.c" 3 4
                 , 5)
# 818 "remake.c"
                                                   , file->name);
          if (!((file->name) == (file->hname) || (*(file->name) == *(file->hname) && (*(file->name) == '\0' || !
# 819 "remake.c" 3 4
              __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) && __builtin_constant_p (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) && (__s1_len = __builtin_strlen (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ), __s2_len = __builtin_strlen (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ), (!((size_t)(const void *)((
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) + 1) - (size_t)(const void *)(
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) + 1) - (size_t)(const void *)(
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) == 1) || __s2_len >= 4)) ? __builtin_strcmp (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              , 
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) : (__builtin_constant_p (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) && ((size_t)(const void *)((
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) + 1) - (size_t)(const void *)(
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) == 1) && (__s1_len = __builtin_strlen (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ), __s1_len < 4) ? (__builtin_constant_p (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) && ((size_t)(const void *)((
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) + 1) - (size_t)(const void *)(
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) == 1) ? __builtin_strcmp (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              , 
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) : (__extension__ ({ const unsigned char *__s2 = (const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ); int __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) && ((size_t)(const void *)((
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) + 1) - (size_t)(const void *)(
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) == 1) && (__s2_len = __builtin_strlen (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ), __s2_len < 4) ? (__builtin_constant_p (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) && ((size_t)(const void *)((
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) + 1) - (size_t)(const void *)(
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ) == 1) ? __builtin_strcmp (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              , 
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ) : (- (__extension__ ({ const unsigned char *__s2 = (const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              ); int __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ))[0] - __s2[0]); if (__s2_len > 0 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ))[1] - __s2[1]); if (__s2_len > 1 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ))[2] - __s2[2]); if (__s2_len > 2 && __result == 0) __result = (((const unsigned char *) (const char *) (
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              ))[3] - __s2[3]); } } __result; })))) : __builtin_strcmp (
# 819 "remake.c"
              (file->name) + 1
# 819 "remake.c" 3 4
              , 
# 819 "remake.c"
              (file->hname) + 1
# 819 "remake.c" 3 4
              )))); })
# 819 "remake.c"
              ))))
              printf (
# 820 "remake.c" 3 4
                     dcgettext (((void *)0), 
# 820 "remake.c"
                     "; using VPATH name '%s'"
# 820 "remake.c" 3 4
                     , 5)
# 820 "remake.c"
                                                 , file->hname);
          puts (".");
          fflush (
# 822 "remake.c" 3 4
                 stdout
# 822 "remake.c"
                       );
        }

      notice_finished_file (file);





      while (file)
        {
          file->name = file->hname;
          file = file->prev;
        }

      return 0;
    }

  do{ if((((0x001))&db_level)) {print_spaces (depth); printf (
# 840 "remake.c" 3 4
 dcgettext (((void *)0), 
# 840 "remake.c"
 "Must remake target '%s'.\n"
# 840 "remake.c" 3 4
 , 5)
# 840 "remake.c"
 , file->name); fflush (
# 840 "remake.c" 3 4
 stdout
# 840 "remake.c"
 );} }while(0);



  if (!((file->name) == (file->hname) || (*(file->name) == *(file->hname) && (*(file->name) == '\0' || !
# 844 "remake.c" 3 4
      __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) && __builtin_constant_p (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) && (__s1_len = __builtin_strlen (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ), __s2_len = __builtin_strlen (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ), (!((size_t)(const void *)((
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) + 1) - (size_t)(const void *)(
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) + 1) - (size_t)(const void *)(
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) == 1) || __s2_len >= 4)) ? __builtin_strcmp (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      , 
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) : (__builtin_constant_p (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) && ((size_t)(const void *)((
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) + 1) - (size_t)(const void *)(
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) == 1) && (__s1_len = __builtin_strlen (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ), __s1_len < 4) ? (__builtin_constant_p (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) && ((size_t)(const void *)((
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) + 1) - (size_t)(const void *)(
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) == 1) ? __builtin_strcmp (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      , 
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) : (__extension__ ({ const unsigned char *__s2 = (const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ); int __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) && ((size_t)(const void *)((
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) + 1) - (size_t)(const void *)(
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) == 1) && (__s2_len = __builtin_strlen (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ), __s2_len < 4) ? (__builtin_constant_p (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) && ((size_t)(const void *)((
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) + 1) - (size_t)(const void *)(
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ) == 1) ? __builtin_strcmp (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      , 
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ) : (- (__extension__ ({ const unsigned char *__s2 = (const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      ); int __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ))[0] - __s2[0]); if (__s2_len > 0 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ))[1] - __s2[1]); if (__s2_len > 1 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ))[2] - __s2[2]); if (__s2_len > 2 && __result == 0) __result = (((const unsigned char *) (const char *) (
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      ))[3] - __s2[3]); } } __result; })))) : __builtin_strcmp (
# 844 "remake.c"
      (file->name) + 1
# 844 "remake.c" 3 4
      , 
# 844 "remake.c"
      (file->hname) + 1
# 844 "remake.c" 3 4
      )))); })
# 844 "remake.c"
      ))))
    {
      do{ if((((0x001))&db_level)) {printf (
# 846 "remake.c" 3 4
     dcgettext (((void *)0), 
# 846 "remake.c"
     "  Ignoring VPATH name '%s'.\n"
# 846 "remake.c" 3 4
     , 5)
# 846 "remake.c"
     , file->hname); fflush (
# 846 "remake.c" 3 4
     stdout
# 846 "remake.c"
     );} }while(0);
      file->ignore_vpath = 1;
    }


  remake_file (file);

  if (file->command_state != cs_finished)
    {
      do{ if((((0x002))&db_level)) {print_spaces (depth); printf (
# 855 "remake.c" 3 4
     dcgettext (((void *)0), 
# 855 "remake.c"
     "Recipe of '%s' is being run.\n"
# 855 "remake.c" 3 4
     , 5)
# 855 "remake.c"
     , file->name); fflush (
# 855 "remake.c" 3 4
     stdout
# 855 "remake.c"
     );} }while(0);
      return 0;
    }

  switch (file->update_status)
    {
    case us_failed:
      do{ if((((0x001))&db_level)) {print_spaces (depth); printf (
# 862 "remake.c" 3 4
     dcgettext (((void *)0), 
# 862 "remake.c"
     "Failed to remake target file '%s'.\n"
# 862 "remake.c" 3 4
     , 5)
# 862 "remake.c"
     , file->name); fflush (
# 862 "remake.c" 3 4
     stdout
# 862 "remake.c"
     );} }while(0);
      break;
    case us_success:
      do{ if((((0x001))&db_level)) {print_spaces (depth); printf (
# 865 "remake.c" 3 4
     dcgettext (((void *)0), 
# 865 "remake.c"
     "Successfully remade target file '%s'.\n"
# 865 "remake.c" 3 4
     , 5)
# 865 "remake.c"
     , file->name); fflush (
# 865 "remake.c" 3 4
     stdout
# 865 "remake.c"
     );} }while(0);
      break;
    case us_question:
      do{ if((((0x001))&db_level)) {print_spaces (depth); printf (
# 868 "remake.c" 3 4
     dcgettext (((void *)0), 
# 868 "remake.c"
     "Target file '%s' needs to be remade under -q.\n"
# 868 "remake.c" 3 4
     , 5)
# 868 "remake.c"
     , file->name); fflush (
# 868 "remake.c" 3 4
     stdout
# 868 "remake.c"
     );} }while(0);
      break;
    case us_none:
      break;
    }

  file->updated = 1;
  return file->update_status;
}







void
notice_finished_file (struct file *file)
{
  struct dep *d;
  int ran = file->command_state == cs_running;
  int touched = 0;

  file->command_state = cs_finished;
  file->updated = 1;

  if (touch_flag






      && file->update_status == us_success)
    {
      if (file->cmds != 0 && file->cmds->any_recurse)
        {


          unsigned int i;
          for (i = 0; i < file->cmds->ncommand_lines; ++i)
            if (!(file->cmds->lines_flags[i] & 1))
              goto have_nonrecursing;
        }
      else
        {
        have_nonrecursing:
          if (file->phony)
            file->update_status = us_success;

          else if (file->cmds != 0)
            {

              file->update_status = touch_file (file);



              commands_started++;





              touched = 1;
            }
        }
    }

  if (file->mtime_before_update == 0)
    file->mtime_before_update = file->last_mtime;

  if ((ran && !file->phony) || touched)
    {
      int i = 0;





      if ((question_flag || just_print_flag || touch_flag) && file->cmds)
        {
          for (i = file->cmds->ncommand_lines; i > 0; --i)
            if (! (file->cmds->lines_flags[i-1] & 1))
              break;
        }



      else if (file->is_target && file->cmds == 0)
        i = 1;

      file->last_mtime = i == 0 ? 0 : (~ (uintmax_t) 0 - (! ((uintmax_t) -1 <= 0) ? (uintmax_t) 0 : ~ (uintmax_t) 0 << (sizeof (uintmax_t) * 8 - 1)));
    }

  if (file->double_colon)
    {
# 974 "remake.c"
      struct file *f;
      uintmax_t max_mtime = file->last_mtime;




      for (f = file->double_colon; f != 0 && f->updated; f = f->prev)
        if (max_mtime != 0
            && (f->last_mtime == 0 || f->last_mtime > max_mtime))
          max_mtime = f->last_mtime;

      if (f == 0)
        for (f = file->double_colon; f != 0; f = f->prev)
          f->last_mtime = max_mtime;
    }

  if (ran && file->update_status != us_none)




    for (d = file->also_make; d != 0; d = d->next)
      {
        d->file->command_state = cs_finished;
        d->file->updated = 1;
        d->file->update_status = file->update_status;

        if (ran && !d->file->phony)




          f_mtime (d->file, 0);
      }
  else if (file->update_status == us_none)


    file->update_status = us_success;
}







static enum update_status
check_dep (struct file *file, unsigned int depth,
           uintmax_t this_mtime, int *must_make_ptr)
{
  struct file *ofile;
  struct dep *d;
  enum update_status dep_status = us_success;

  ++depth;
  (((file)->double_colon ? (file)->double_colon : (file)) ->updating = 1);



  ofile = file;

  if (file->phony || !file->intermediate)
    {


      uintmax_t mtime;
      dep_status = update_file (file, depth);
      while ((file)->renamed != 0) (file) = (file)->renamed;
      mtime = (((file))->last_mtime == 0 ? f_mtime (((file)), 1) : ((file))->last_mtime);
      while ((file)->renamed != 0) (file) = (file)->renamed;
      if (mtime == 1 || mtime > this_mtime)
        *must_make_ptr = 1;
    }
  else
    {

      uintmax_t mtime;

      if (!file->phony && file->cmds == 0 && !file->tried_implicit)
        {
          if (try_implicit_rule (file, depth))
            do{ if((((0x008))&db_level)) {print_spaces (depth); printf (
# 1055 "remake.c" 3 4
           dcgettext (((void *)0), 
# 1055 "remake.c"
           "Found an implicit rule for '%s'.\n"
# 1055 "remake.c" 3 4
           , 5)
# 1055 "remake.c"
           , file->name); fflush (
# 1055 "remake.c" 3 4
           stdout
# 1055 "remake.c"
           );} }while(0);
          else
            do{ if((((0x008))&db_level)) {print_spaces (depth); printf (
# 1057 "remake.c" 3 4
           dcgettext (((void *)0), 
# 1057 "remake.c"
           "No implicit rule found for '%s'.\n"
# 1057 "remake.c" 3 4
           , 5)
# 1057 "remake.c"
           , file->name); fflush (
# 1057 "remake.c" 3 4
           stdout
# 1057 "remake.c"
           );} }while(0);
          file->tried_implicit = 1;
        }
      if (file->cmds == 0 && !file->is_target
          && default_file != 0 && default_file->cmds != 0)
        {
          do{ if((((0x008))&db_level)) {print_spaces (depth); printf (
# 1063 "remake.c" 3 4
         dcgettext (((void *)0), 
# 1063 "remake.c"
         "Using default commands for '%s'.\n"
# 1063 "remake.c" 3 4
         , 5)
# 1063 "remake.c"
         , file->name); fflush (
# 1063 "remake.c" 3 4
         stdout
# 1063 "remake.c"
         );} }while(0);
          file->cmds = default_file->cmds;
        }

      while ((file)->renamed != 0) (file) = (file)->renamed;
      mtime = (((file))->last_mtime == 0 ? f_mtime (((file)), 1) : ((file))->last_mtime);
      while ((file)->renamed != 0) (file) = (file)->renamed;
      if (mtime != 1 && mtime > this_mtime)


        *must_make_ptr = 1;
      else
        {



          struct dep *ld;
          int deps_running = 0;




          if (file->command_state != cs_running)
            {


              if (file->command_state == cs_deps_running)
                file->considered = !considered;

              set_command_state (file, cs_not_started);
            }

          ld = 0;
          d = file->deps;
          while (d != 0)
            {
              enum update_status new;
              int maybe_make;

              if ((((d->file)->double_colon ? (d->file)->double_colon : (d->file)) ->updating))
                {
                  error((((floc *)0)), strlen (file->name) + strlen (d->file->name), (
# 1104 "remake.c" 3 4
                 dcgettext (((void *)0), 
# 1104 "remake.c"
                 "Circular %s <- %s dependency dropped."
# 1104 "remake.c" 3 4
                 , 5)
# 1104 "remake.c"
                 ), (file->name), (d->file->name))
                                                 ;
                  if (ld == 0)
                    {
                      file->deps = d->next;
                      free (d);
                      d = file->deps;
                    }
                  else
                    {
                      ld->next = d->next;
                      free (d);
                      d = ld->next;
                    }
                  continue;
                }

              d->file->parent = file;
              maybe_make = *must_make_ptr;
              new = check_dep (d->file, depth, this_mtime, &maybe_make);
              if (new > dep_status)
                dep_status = new;

              if (! d->ignore_mtime)
                *must_make_ptr = maybe_make;
              while ((d->file)->renamed != 0) (d->file) = (d->file)->renamed;
              if (dep_status && !keep_going_flag)
                break;

              if (d->file->command_state == cs_running
                  || d->file->command_state == cs_deps_running)
                deps_running = 1;

              ld = d;
              d = d->next;
            }

          if (deps_running)



            set_command_state (file, cs_deps_running);
        }
    }

  (((file)->double_colon ? (file)->double_colon : (file)) ->updating = 0);
  (((ofile)->double_colon ? (ofile)->double_colon : (ofile)) ->updating = 0);

  return dep_status;
}






static enum update_status
touch_file (struct file *file)
{
  if (!silent_flag)
    message((0), strlen (file->name), ("touch %s"), (file->name));


  if (just_print_flag)
    return us_success;


  if (ar_name (file->name))
    return ar_touch (file->name) ? us_failed : us_success;
  else

    {
      int fd;

      while (((fd)=open (file->name, 
# 1178 "remake.c" 3 4
     02 
# 1178 "remake.c"
     | 
# 1178 "remake.c" 3 4
     0100
# 1178 "remake.c"
     , 0666))==-1 && 
# 1178 "remake.c" 3 4
     (*__errno_location ())
# 1178 "remake.c"
     ==
# 1178 "remake.c" 3 4
     4
# 1178 "remake.c"
     );
      if (fd < 0)
        do{ perror_with_name (("touch: open: "), file->name); return us_failed; }while(0);
      else
        {
          struct stat statbuf;
          char buf = 'x';
          int e;

          while (((e)=fstat (fd, &statbuf))==-1 && 
# 1187 "remake.c" 3 4
         (*__errno_location ())
# 1187 "remake.c"
         ==
# 1187 "remake.c" 3 4
         4
# 1187 "remake.c"
         );
          if (e < 0)
            do{ perror_with_name (("touch: fstat: "), file->name); return us_failed; }while(0);

          while (((e)=read (fd, &buf, 1))==-1 && 
# 1191 "remake.c" 3 4
         (*__errno_location ())
# 1191 "remake.c"
         ==
# 1191 "remake.c" 3 4
         4
# 1191 "remake.c"
         );
          if (e < 0)
            do{ perror_with_name (("touch: read: "), file->name); return us_failed; }while(0);
          {
            off_t o;
            while (((o)=lseek (fd, 0L, 0))==-1 && 
# 1196 "remake.c" 3 4
           (*__errno_location ())
# 1196 "remake.c"
           ==
# 1196 "remake.c" 3 4
           4
# 1196 "remake.c"
           );
            if (o < 0L)
              do{ perror_with_name (("touch: lseek: "), file->name); return us_failed; }while(0);
          }
          while (((e)=write (fd, &buf, 1))==-1 && 
# 1200 "remake.c" 3 4
         (*__errno_location ())
# 1200 "remake.c"
         ==
# 1200 "remake.c" 3 4
         4
# 1200 "remake.c"
         );
          if (e < 0)
            do{ perror_with_name (("touch: write: "), file->name); return us_failed; }while(0);


          if (statbuf.st_size == 0)
            {
              (void) close (fd);
              while (((fd)=open (file->name, 
# 1208 "remake.c" 3 4
             02 
# 1208 "remake.c"
             | 
# 1208 "remake.c" 3 4
             01000
# 1208 "remake.c"
             , 0666))==-1 && 
# 1208 "remake.c" 3 4
             (*__errno_location ())
# 1208 "remake.c"
             ==
# 1208 "remake.c" 3 4
             4
# 1208 "remake.c"
             );
              if (fd < 0)
                do{ perror_with_name (("touch: open: "), file->name); return us_failed; }while(0);
            }
          (void) close (fd);
        }
    }

  return us_success;
}





static void
remake_file (struct file *file)
{
  if (file->cmds == 0)
    {
      if (file->phony)

        file->update_status = us_success;
      else if (file->is_target)


        file->update_status = us_success;
      else
        {

          if (!rebuilding_makefiles || !file->dontcare)
            complain (file);
          file->update_status = us_failed;
        }
    }
  else
    {
      chop_commands (file->cmds);


      if (!touch_flag || file->cmds->any_recurse)
        {
          execute_file_commands (file);
          return;
        }


      file->update_status = us_success;
    }


  notice_finished_file (file);
}
# 1270 "remake.c"
uintmax_t
f_mtime (struct file *file, int search)
{
  uintmax_t mtime;
  int propagate_timestamp;




  if (ar_name (file->name))
    {


      char *arname, *memname;
      struct file *arfile;
      time_t member_date;


      ar_parse_name (file->name, &arname, &memname);



      arfile = lookup_file (arname);
      if (arfile == 0)
        arfile = enter_file (strcache_add (arname));
      mtime = f_mtime (arfile, search);
      while ((arfile)->renamed != 0) (arfile) = (arfile)->renamed;
      if (search && 
# 1297 "remake.c" 3 4
                   __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) && __builtin_constant_p (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) && (__s1_len = __builtin_strlen (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ), __s2_len = __builtin_strlen (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ), (!((size_t)(const void *)((
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) + 1) - (size_t)(const void *)(
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) + 1) - (size_t)(const void *)(
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) == 1) || __s2_len >= 4)) ? __builtin_strcmp (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   , 
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) : (__builtin_constant_p (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) && ((size_t)(const void *)((
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) + 1) - (size_t)(const void *)(
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) == 1) && (__s1_len = __builtin_strlen (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ), __s1_len < 4) ? (__builtin_constant_p (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) && ((size_t)(const void *)((
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) + 1) - (size_t)(const void *)(
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) == 1) ? __builtin_strcmp (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   , 
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) : (__extension__ ({ const unsigned char *__s2 = (const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ); int __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) && ((size_t)(const void *)((
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) + 1) - (size_t)(const void *)(
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) == 1) && (__s2_len = __builtin_strlen (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ), __s2_len < 4) ? (__builtin_constant_p (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) && ((size_t)(const void *)((
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) + 1) - (size_t)(const void *)(
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ) == 1) ? __builtin_strcmp (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   , 
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ) : (- (__extension__ ({ const unsigned char *__s2 = (const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   ); int __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ))[0] - __s2[0]); if (__s2_len > 0 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ))[1] - __s2[1]); if (__s2_len > 1 && __result == 0) { __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ))[2] - __s2[2]); if (__s2_len > 2 && __result == 0) __result = (((const unsigned char *) (const char *) (
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   ))[3] - __s2[3]); } } __result; })))) : __builtin_strcmp (
# 1297 "remake.c"
                   arfile->hname
# 1297 "remake.c" 3 4
                   , 
# 1297 "remake.c"
                   arname
# 1297 "remake.c" 3 4
                   )))); })
# 1297 "remake.c"
                                                 )
        {



          char *name;
          unsigned int arlen, memlen;

          arlen = strlen (arfile->hname);
          memlen = strlen (memname);

          name = 
# 1308 "remake.c" 3 4
                __builtin_alloca (
# 1308 "remake.c"
                arlen + 1 + memlen + 2
# 1308 "remake.c" 3 4
                )
# 1308 "remake.c"
                                               ;
          memcpy (name, arfile->hname, arlen);
          name[arlen] = '(';
          memcpy (name + arlen + 1, memname, memlen);
          name[arlen + 1 + memlen] = ')';
          name[arlen + 1 + memlen + 1] = '\0';



          if (arfile->name == arfile->hname)
            rename_file (file, strcache_add (name));
          else
            rehash_file (file, strcache_add (name));
          while ((file)->renamed != 0) (file) = (file)->renamed;
        }

      free (arname);

      file->low_resolution_time = 1;

      if (mtime == 1)

        return 1;

      member_date = ar_member_date (file->hname);
      mtime = (member_date == (time_t) -1
               ? 1
               : file_timestamp_cons (file->hname, member_date, 0));
    }
  else

    {
      mtime = name_mtime (file->name);

      if (mtime == 1 && search && !file->ignore_vpath)
        {

          const char *name = vpath_search (file->name, &mtime, 
# 1345 "remake.c" 3 4
                                                              ((void *)0)
# 1345 "remake.c"
                                                                  , 
# 1345 "remake.c" 3 4
                                                                    ((void *)0)
# 1345 "remake.c"
                                                                        );
          if (name

              || (file->name[0] == '-' && file->name[1] == 'l'
                  && (name = library_search (file->name, &mtime)) != 0))
            {
              int name_len;

              if (mtime != 0)


                file->last_mtime = mtime;





              name_len = strlen (name) - strlen (file->name) - 1;





              if (gpath_search (name, name_len))
                {
                  rename_file (file, name);
                  while ((file)->renamed != 0) (file) = (file)->renamed;
                  return (((file))->last_mtime == 0 ? f_mtime (((file)), 1) : ((file))->last_mtime);
                }

              rehash_file (file, name);
              while ((file)->renamed != 0) (file) = (file)->renamed;


              if (mtime != 2 && mtime != (~ (uintmax_t) 0 - (! ((uintmax_t) -1 <= 0) ? (uintmax_t) 0 : ~ (uintmax_t) 0 << (sizeof (uintmax_t) * 8 - 1))))
                mtime = name_mtime (name);
            }
        }
    }







  if (!clock_skew_detected
      && mtime != 1 && mtime != (~ (uintmax_t) 0 - (! ((uintmax_t) -1 <= 0) ? (uintmax_t) 0 : ~ (uintmax_t) 0 << (sizeof (uintmax_t) * 8 - 1)))
      && !file->updated)
    {
      static uintmax_t adjusted_now;

      uintmax_t adjusted_mtime = mtime;
# 1420 "remake.c"
      if (adjusted_now < adjusted_mtime)
        {
          int resolution;
          uintmax_t now = file_timestamp_now (&resolution);
          adjusted_now = now + (resolution - 1);
          if (adjusted_now < adjusted_mtime)
            {





              double from_now =
                ((((mtime) - (2 + 1)) >> (1 ? 30 : 0)) - (((now) - (2 + 1)) >> (1 ? 30 : 0))
                 + ((((int) (((mtime) - (2 + 1)) & ((1 << (1 ? 30 : 0)) - 1))) - ((int) (((now) - (2 + 1)) & ((1 << (1 ? 30 : 0)) - 1))))
                    / 1e9));
              char from_now_string[100];

              if (from_now >= 99 && from_now <= 
# 1438 "remake.c" 3 4
                                               (0x7fffffffffffffffL * 2UL + 1UL)
# 1438 "remake.c"
                                                        )
                sprintf (from_now_string, "%lu", (unsigned long) from_now);
              else
                sprintf (from_now_string, "%.2g", from_now);
              error((((floc *)0)), strlen (file->name) + strlen (from_now_string), (
# 1442 "remake.c" 3 4
             dcgettext (((void *)0), 
# 1442 "remake.c"
             "Warning: File '%s' has modification time %s s in the future"
# 1442 "remake.c" 3 4
             , 5)
# 1442 "remake.c"
             ), (file->name), (from_now_string))

                                               ;

              clock_skew_detected = 1;
            }
        }
    }




  if (file->double_colon)
    file = file->double_colon;

  propagate_timestamp = file->updated;
  do
    {





      if (mtime != 1 && file->command_state == cs_not_started
          && !file->tried_implicit && file->intermediate)
        file->intermediate = 0;

      if (file->updated == propagate_timestamp)
        file->last_mtime = mtime;
      file = file->prev;
    }
  while (file != 0);

  return mtime;
}
# 1487 "remake.c"
static uintmax_t
name_mtime (const char *name)
{
  uintmax_t mtime;
  struct stat st;
  int e;

  while (((e)=stat (name, &st))==-1 && 
# 1494 "remake.c" 3 4
 (*__errno_location ())
# 1494 "remake.c"
 ==
# 1494 "remake.c" 3 4
 4
# 1494 "remake.c"
 );
  if (e == 0)
    mtime = file_timestamp_cons (name, (st).
# 1496 "remake.c" 3 4
           st_mtim.tv_sec
# 1496 "remake.c"
           , (st).st_mtim.tv_nsec);
  else if (
# 1497 "remake.c" 3 4
          (*__errno_location ()) 
# 1497 "remake.c"
                == 
# 1497 "remake.c" 3 4
                   2 
# 1497 "remake.c"
                          || 
# 1497 "remake.c" 3 4
                             (*__errno_location ()) 
# 1497 "remake.c"
                                   == 
# 1497 "remake.c" 3 4
                                      20
# 1497 "remake.c"
                                             )
    mtime = 1;
  else
    {
      perror_with_name ("stat: ", name);
      return 1;
    }
# 1512 "remake.c"
  if (check_symlink_flag)
    {
      char lpath[
# 1514 "remake.c" 3 4
     4096
# 1514 "remake.c"
     ];







      strcpy (lpath, name);
      while (1)
        {
          uintmax_t ltime;
          char lbuf[
# 1526 "remake.c" 3 4
         4096
# 1526 "remake.c"
         ];
          long llen;
          char *p;

          while (((e)=lstat (lpath, &st))==-1 && 
# 1530 "remake.c" 3 4
         (*__errno_location ())
# 1530 "remake.c"
         ==
# 1530 "remake.c" 3 4
         4
# 1530 "remake.c"
         );
          if (e)
            {

              if (
# 1534 "remake.c" 3 4
                 (*__errno_location ()) 
# 1534 "remake.c"
                       != 
# 1534 "remake.c" 3 4
                          2 
# 1534 "remake.c"
                                 && 
# 1534 "remake.c" 3 4
                                    (*__errno_location ()) 
# 1534 "remake.c"
                                          != 
# 1534 "remake.c" 3 4
                                             20
# 1534 "remake.c"
                                                    )
                perror_with_name ("lstat: ", lpath);
              break;
            }



          if (!
# 1541 "remake.c" 3 4
              ((((
# 1541 "remake.c"
              st.st_mode
# 1541 "remake.c" 3 4
              )) & 0170000) == (0120000))
# 1541 "remake.c"
                                  )
            break;


          ltime = file_timestamp_cons (lpath, (st).
# 1545 "remake.c" 3 4
                 st_mtim.tv_sec
# 1545 "remake.c"
                 , (st).st_mtim.tv_nsec);
          if (ltime > mtime)
            mtime = ltime;


          while (((llen)=readlink (lpath, lbuf, 
# 1550 "remake.c" 3 4
         4096
# 1550 "remake.c"
         ))==-1 && 
# 1550 "remake.c" 3 4
         (*__errno_location ())
# 1550 "remake.c"
         ==
# 1550 "remake.c" 3 4
         4
# 1550 "remake.c"
         );
          if (llen < 0)
            {

              perror_with_name ("readlink: ", lpath);
              break;
            }
          lbuf[llen] = '\0';




          if (lbuf[0] == '/' || (p = strrchr (lpath, '/')) == 
# 1562 "remake.c" 3 4
                                                             ((void *)0)
# 1562 "remake.c"
                                                                 )
            strcpy (lpath, lbuf);
          else if ((p - lpath) + llen + 2 > 
# 1564 "remake.c" 3 4
                                           4096
# 1564 "remake.c"
                                                       )

            break;
          else

            strcpy (p+1, lbuf);
        }
    }


  return mtime;
}






static const char *
library_search (const char *lib, uintmax_t *mtime_ptr)
{
  static const char *dirs[] =
    {

      "/lib",
      "/usr/lib",
# 1598 "remake.c"
      "/usr/local/lib",
      0
    };

  const char *file = 0;
  char *libpatterns;
  uintmax_t mtime;


  char *p;
  const char *p2;
  unsigned int len;
  unsigned int liblen;


  unsigned int best_vpath = 0, best_path = 0;

  const char **dp;

  libpatterns = xstrdup (variable_expand ("$(.LIBPATTERNS)"));


  lib += 2;
  liblen = strlen (lib);




  p2 = libpatterns;
  while ((p = find_next_token (&p2, &len)) != 0)
    {
      static char *buf = 
# 1629 "remake.c" 3 4
                        ((void *)0)
# 1629 "remake.c"
                            ;
      static unsigned int buflen = 0;
      static int libdir_maxlen = -1;
      static unsigned int std_dirs = 0;
      char *libbuf = variable_expand ("");


      {
        char c = p[len];
        char *p3, *p4;

        p[len] = '\0';
        p3 = find_percent (p);
        if (!p3)
          {

            error((((floc *)0)), strlen (p), (
# 1645 "remake.c" 3 4
           dcgettext (((void *)0), 
# 1645 "remake.c"
           ".LIBPATTERNS element '%s' is not a pattern"
# 1645 "remake.c" 3 4
           , 5)
# 1645 "remake.c"
           ), (p))
                                                                   ;
            p[len] = c;
            continue;
          }
        p4 = variable_buffer_output (libbuf, p, p3-p);
        p4 = variable_buffer_output (p4, lib, liblen);
        p4 = variable_buffer_output (p4, p3+1, len - (p3-p));
        p[len] = c;
      }


      mtime = name_mtime (libbuf);
      if (mtime != 1)
        {
          if (mtime_ptr != 0)
            *mtime_ptr = mtime;
          file = strcache_add (libbuf);

          break;
        }



      {
        unsigned int vpath_index, path_index;
        const char* f = vpath_search (libbuf, mtime_ptr ? &mtime : 
# 1671 "remake.c" 3 4
                                                                  ((void *)0)
# 1671 "remake.c"
                                                                      ,
                                      &vpath_index, &path_index);
        if (f)
          {

            if (file == 0 ||
                vpath_index < best_vpath ||
                (vpath_index == best_vpath && path_index < best_path))
              {
                file = f;
                best_vpath = vpath_index;
                best_path = path_index;

                if (mtime_ptr != 0)
                  *mtime_ptr = mtime;
              }
          }
      }



      if (!buflen)
        {
          for (dp = dirs; *dp != 0; ++dp)
            {
              int l = strlen (*dp);
              if (l > libdir_maxlen)
                libdir_maxlen = l;
              std_dirs++;
            }
          buflen = strlen (libbuf);
          buf = xmalloc (libdir_maxlen + buflen + 2);
        }
      else if (buflen < strlen (libbuf))
        {
          buflen = strlen (libbuf);
          buf = xrealloc (buf, libdir_maxlen + buflen + 2);
        }

      {


        unsigned int vpath_index = ~((unsigned int)0) - std_dirs;

        for (dp = dirs; *dp != 0; ++dp)
          {
            sprintf (buf, "%s/%s", *dp, libbuf);
            mtime = name_mtime (buf);
            if (mtime != 1)
              {
                if (file == 0 || vpath_index < best_vpath)
                  {
                    file = strcache_add (buf);
                    best_vpath = vpath_index;

                    if (mtime_ptr != 0)
                      *mtime_ptr = mtime;
                  }
              }

            vpath_index++;
          }
      }

    }

  free (libpatterns);
  return file;
}
