OS=$(shell uname | tr '[:upper:]' '[:lower:]')
ARCH=$(shell uname -m)

AM_CPPFLAGS = -I$(srcdir)/../$(OS)/$(ARCH)/  -I$(srcdir)/../util $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(NODEBUG_CXXFLAGS)

noinst_LIBRARIES = libggtrace.a

libggtrace_a_SOURCES = sysent.hh syscall.hh syscall.cc
