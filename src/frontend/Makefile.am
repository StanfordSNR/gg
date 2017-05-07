AM_CPPFLAGS = -I$(srcdir)/../util -I$(srcdir)/../$(OS)/$(ARCH)/ \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../util/libggutil.a ../trace/libggtrace.a  ../sandbox/libggsandbox.a \
             ../thunk/libthunk.a

bin_PROGRAMS = gg-trace gg-sandbox gg-gcc

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)

gg_gcc_SOURCES = gg-gcc.cc
gg_gcc_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_sandbox_SOURCES = gg-sandbox.cc
gg_sandbox_LDADD = $(BASE_LDADD)
