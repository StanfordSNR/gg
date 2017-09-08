AM_CPPFLAGS = -I$(srcdir)/../util $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggtrace.a libggsyscalltable.a

libggtrace_a_SOURCES = syscall.hh invocation.hh invocation.cc \
                       traced_process.hh traced_process.cc \
		       tracer_thread.hh tracer_thread.cc

libggsyscalltable_a_SOURCES = @OS@/@ARCH@/syscalltbl.cc
libggsyscalltable_a_CPPFLAGS = $(AM_CPPFLAGS) -fno-var-tracking

EXTRA_DIST = linux/generate_table.py
