AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../trace/libggtrace.a \
             ../sandbox/libggsandbox.a \
             ../thunk/libthunk.a \
             ../protobufs/libggprotobufs.a ../util/libggutil.a \
             $(PROTOBUF_LIBS)

bin_PROGRAMS = gg-trace gg-sandbox gg-describe gg-execute gg-gcc

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)

gg_sandbox_SOURCES = gg-sandbox.cc
gg_sandbox_LDADD = $(BASE_LDADD)

gg_describe_SOURCES = gg-describe.cc
gg_describe_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_SOURCES = gg-execute.cc
gg_execute_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_gcc_SOURCES = gg-gcc.cc
gg_gcc_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)
