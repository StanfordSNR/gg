AM_CPPFLAGS = -I$(srcdir)/../util $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(NODEBUG_CXXFLAGS)

BASE_LDADD = ../util/libggutil.a

bin_PROGRAMS = gg-trace

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)
