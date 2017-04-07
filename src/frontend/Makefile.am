OS=$(shell uname | tr '[:upper:]' '[:lower:]')
ARCH=$(shell uname -m)

AM_CPPFLAGS = -I$(srcdir)/../util -I$(srcdir)/../$(OS)/$(ARCH)/ -I$(srcdir)/../trace $(CXX11_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(NODEBUG_CXXFLAGS)

BASE_LDADD = ../util/libggutil.a ../trace/libggtrace.a

bin_PROGRAMS = gg-trace

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)
