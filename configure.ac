
AC_PREREQ([2.68])
AC_INIT([gg], [0.01], [gg@cs.stanford.edu])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/frontend/gg-trace.cc])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Add picky CXXFLAGS
CXX14_FLAGS="-std=c++14 -pthread"
PICKY_CXXFLAGS="-pedantic -Wall -Wextra -Weffc++ -Werror"
AC_SUBST([CXX14_FLAGS])
AC_SUBST([PICKY_CXXFLAGS])
AC_LANG_PUSH(C++)

AC_ARG_ENABLE([sanitize],
  [AS_HELP_STRING([--enable-sanitize],
     [build with address and undefined-behavior santizers])],
     [EXTRA_CXXFLAGS="-fsanitize=address -fsanitize=undefined -fuse-ld=bfd"])

AC_SUBST(EXTRA_CXXFLAGS)

# Checks for libraries.
PKG_CHECK_MODULES([CRYPTO],[libcrypto])
PKG_CHECK_MODULES([PROTOBUF], [protobuf])
PKG_CHECK_MODULES([LIBCAP], [libcap])

# Protobuf compiler
AC_PATH_PROG([PROTOC], [protoc], [])
AS_IF([test x"$PROTOC" = x],
  [AC_MSG_ERROR([cannot find protoc, the Protocol Buffers compiler])])

# Checks for header files.
AC_CHECK_HEADERS([keyutils.h], [], [AC_MSG_ERROR([Missing required header file])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T

# Used for loading the correct syscall table
OS="$(uname | tr '[[:upper:]]' '[[:lower:]]')"
ARCH="$(uname -m)"
AC_SUBST([OS])
AC_SUBST([ARCH])

# Checks for library functions.
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/util/Makefile
                 src/protobufs/Makefile
                 src/thunk/Makefile
                 src/trace/Makefile
                 src/sandbox/Makefile
                 src/frontend/Makefile
                 src/tests/Makefile])

AC_OUTPUT
