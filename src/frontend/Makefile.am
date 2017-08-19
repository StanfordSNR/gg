AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox -I$(srcdir)/../models \
              $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../thunk/libthunk.a \
             ../sandbox/libggsandbox.a \
             ../trace/libggtrace.a ../trace/libggsyscalltable.a \
             ../protobufs/libggprotobufs.a ../util/libggutil.a \
             $(PROTOBUF_LIBS)

bin_PROGRAMS = gg-trace gg-describe gg-reduce-and-run gg-reduce gg-mock \
               gg-execute

if BUILD_STATIC_BINS
  bin_PROGRAMS += gg-execute-static
endif

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)

gg_describe_SOURCES = gg-describe.cc
gg_describe_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_reduce_and_run_SOURCES = gg-reduce-and-run.cc
gg_reduce_and_run_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_SOURCES = gg-execute.cc
gg_execute_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_static_SOURCES = gg-execute.cc
gg_execute_static_CXXFLAGS = $(PICKY_CXXFLAGS) -static
gg_execute_static_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) -ldl -lz
gg_execute_static_LDFLAGS = -static

gg_reduce_SOURCES = gg-reduce.cc
gg_reduce_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_mock_SOURCES = gg-mock.cc
gg_mock_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)
