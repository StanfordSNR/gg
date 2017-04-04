#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([gg], [0.01], [gg@cs.stanford.edu])
AM_INIT_AUTOMAKE([foreign])
#AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Add picky CXXFLAGS
CXX14_FLAGS="-std=c++14 -pthread"
PICKY_CXXFLAGS="-pedantic -Wall -Wextra -Weffc++ -Werror"
AC_SUBST([CXX11_FLAGS])
AC_SUBST([PICKY_CXXFLAGS])
AC_LANG_PUSH(C++)

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug@<:@=no/asserts/sanitize@:>@],
     [Turn on asserts or santizers])],
  [case "$enableval" in
     no)
       NODEBUG_CXXFLAGS="-DNDEBUG"
       ;;
     '' | yes | sanitize)
       NODEBUG_CXXFLAGS="-fsanitize=address -fsanitize=undefined -fuse-ld=bfd"
       ;;
     asserts)
       NODEBUG_CXXFLAGS=""
       ;;
     *)
       AC_MSG_ERROR([Unknown argument '$enableval' to --enable-debug])
       ;;
   esac],
  [NODEBUG_CXXFLAGS="-DNDEBUG"])

AC_SUBST(NODEBUG_CXXFLAGS)

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/linux/Makefile
                 src/util/Makefile
                 src/frontend/Makefile
                 src/tests/Makefile])

AC_OUTPUT
