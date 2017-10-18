AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util -I$(srcdir)/../tui \
              -I$(srcdir)/../net -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox -I$(srcdir)/../models \
              -I$(srcdir)/../execution \
              $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) $(SSL_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

GG_LDADD = ../execution/libggexecution.a ../thunk/libthunk.a \
           ../sandbox/libggsandbox.a \
           ../trace/libggtrace.a ../trace/libggsyscalltable.a \
           ../protobufs/libggprotobufs.a ../net/libggnet.a ../tui/libggtui.a \
           ../util/libggutil.a

BASE_LDADD = $(GG_LDADD) $(PROTOBUF_LIBS)

bin_PROGRAMS = gg-trace gg-describe gg-force-and-run gg-force gg-mock \
               gg-execute gg-infer gg-thunkgraph gg-thunksummary gg-s3-upload \
               gg-s3-download gg-init lambda-invoker

if BUILD_STATIC_BINS
  bin_PROGRAMS += gg-execute-static gg-s3-download-static
endif

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)

gg_describe_SOURCES = gg-describe.cc
gg_describe_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_thunkgraph_SOURCES = gg-thunkgraph.cc
gg_thunkgraph_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_thunksummary_SOURCES = gg-thunksummary.cc
gg_thunksummary_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_force_and_run_SOURCES = gg-force-and-run.cc
gg_force_and_run_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_SOURCES = gg-execute.cc
gg_execute_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_static_SOURCES = gg-execute.cc
gg_execute_static_CXXFLAGS = $(PICKY_CXXFLAGS) -static
gg_execute_static_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) -ldl -lz
gg_execute_static_LDFLAGS = -static -s

gg_force_SOURCES = gg-force.cc
gg_force_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

gg_mock_SOURCES = gg-mock.cc
gg_mock_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_infer_SOURCES = gg-infer.cc
gg_infer_LDADD = $(BASE_LDADD)

gg_init_SOURCES = gg-init.cc
gg_init_LDADD = $(BASE_LDADD)

gg_s3_upload_SOURCES = gg-s3-upload.cc
gg_s3_upload_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

gg_s3_download_SOURCES = gg-s3-download.cc
gg_s3_download_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

gg_s3_download_static_SOURCES = gg-s3-download.cc
gg_s3_download_static_CXXFLAGS = $(PICKY_CXXFLAGS)
gg_s3_download_static_LDADD = $(GG_LDADD) $(SSL_LIBS) -ldl -lz
gg_s3_download_static_LDFLAGS = -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

lambda_invoker_SOURCES = lambda-invoker.cc
lambda_invoker_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)
