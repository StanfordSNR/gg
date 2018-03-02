AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggtrace.a libggsyscalltable.a

libggtrace_a_SOURCES = syscall.hh invocation.hh invocation.cc \
                       tracer.hh tracer.cc

libggsyscalltable_a_SOURCES = @OS@/@ARCH@/syscalltbl.cc
libggsyscalltable_a_CPPFLAGS = $(AM_CPPFLAGS) -fno-var-tracking

EXTRA_DIST = linux/generate_table.py
