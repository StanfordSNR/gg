AM_CPPFLAGS = -I$(srcdir)/../util $(CXX14_FLAGS) -fno-var-tracking
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggtrace.a

libggtrace_a_SOURCES = syscall.hh invocation.hh invocation.cc \
                       traced_process.hh traced_process.cc \
                       @OS@/@ARCH@/syscalltbl.cc

EXTRA_DIST = linux/generate_table.py
