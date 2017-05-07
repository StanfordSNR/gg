AM_CPPFLAGS = -I$(srcdir)/../$(OS)/$(ARCH) -I$(srcdir)/../util $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggtrace.a

libggtrace_a_SOURCES = syscall.hh invocation.hh invocation.cc \
                       traced_process.hh traced_process.cc
