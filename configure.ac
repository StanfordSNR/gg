
AC_PREREQ([2.68])
AC_INIT([gg], [0.01], [gg@cs.stanford.edu])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/frontend/gg-trace.cc])
AC_CONFIG_HEADERS([config.h])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_SUBST(ARFLAGS, [cr])

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
  [EXTRA_CXXFLAGS="-fsanitize=address -fsanitize=undefined -fuse-ld=gold"],
  [sanitize=false])

AM_CONDITIONAL([BUILD_STATIC_BINS], [test x$sanitize = xfalse])

AC_SUBST(EXTRA_CXXFLAGS)

# Checks for libraries.
PKG_CHECK_MODULES([CRYPTO],[libcrypto++])
PKG_CHECK_MODULES([SSL],[libssl libcrypto])
PKG_CHECK_MODULES([PROTOBUF], [protobuf])
PKG_CHECK_MODULES([HIREDIS], [hiredis])

AX_BOOST_BASE([1.54.0], [], [AC_MSG_ERROR([Missing boost (may need to install libboost-dev)])])

# Protobuf compiler
AC_PATH_PROG([PROTOC], [protoc], [])
AS_IF([test x"$PROTOC" = x],
  [AC_MSG_ERROR([cannot find protoc, the Protocol Buffers compiler])])

# Checks for header files.
AC_CHECK_HEADERS([sys/capability.h], [], [AC_MSG_ERROR([Missing libcap header file])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T

abs_srcdir=$(readlink -f $srcdir)

# gg-toolchain path
AC_ARG_VAR([TOOLCHAIN_PATH], [Path to gg toolchain.])
AS_IF([test "x$TOOLCHAIN_PATH" = x], [TOOLCHAIN_PATH="$abs_srcdir/toolchain/bin"])
AC_CHECK_FILE($TOOLCHAIN_PATH/gcc, [], [AC_MSG_ERROR([Could not find gcc in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/cc1, [], [AC_MSG_ERROR([Could not find cc1 in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/as, [], [AC_MSG_ERROR([Could not find as in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/g++, [], [AC_MSG_ERROR([Could not find g++ in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/cc1plus, [], [AC_MSG_ERROR([Could not find cc1plus in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/collect2, [], [AC_MSG_ERROR([Could not find collect2 in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/ld, [], [AC_MSG_ERROR([Could not find ld in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/strip, [], [AC_MSG_ERROR([Could not find strip in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/ar, [], [AC_MSG_ERROR([Could not find ar in toolchain dir.])])
AC_CHECK_FILE($TOOLCHAIN_PATH/ranlib, [], [AC_MSG_ERROR([Could not find ranlib in toolchain dir.])])

# Used for loading the correct syscall table
OS="$(uname | tr '[[:upper:]]' '[[:lower:]]')"
ARCH="$(uname -m)"
AC_SUBST([OS])
AC_SUBST([ARCH])

# Checks for library functions.
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/util/Makefile
                 src/tui/Makefile
                 src/net/Makefile
                 src/protobufs/Makefile
                 src/thunk/Makefile
                 src/trace/Makefile
                 src/sandbox/Makefile
                 src/models/Makefile
                 src/storage/Makefile
                 src/execution/Makefile
                 src/frontend/Makefile
                 src/remote/Makefile
                 examples/Makefile
                 examples/fibonacci/Makefile
                 examples/gtest/Makefile
                 examples/viddec/Makefile
                 tests/Makefile])

AC_OUTPUT
