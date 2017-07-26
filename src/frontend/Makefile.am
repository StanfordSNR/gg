AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../trace/libggtrace.a ../trace/libggsyscalltable.a \
             ../sandbox/libggsandbox.a \
             ../thunk/libthunk.a \
             ../protobufs/libggprotobufs.a ../util/libggutil.a \
             $(PROTOBUF_LIBS)

bin_PROGRAMS = gg-trace gg-describe gg-execute gg-mock

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)

gg_describe_SOURCES = gg-describe.cc
gg_describe_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_SOURCES = gg-execute.cc
gg_execute_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_mock_SOURCES = gg-mock.cc
gg_mock_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)
