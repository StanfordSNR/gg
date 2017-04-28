AM_CPPFLAGS = -I$(srcdir)/../util $(CXX11_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(NODEBUG_CXXFLAGS)

noinst_LIBRARIES = libggtrace.a

libggtrace_a_SOURCES = syscall.hh syscall.cc traced_process.hh \
                       traced_process.cc
