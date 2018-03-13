AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

GG_LDADD = ../execution/libggexecution.a ../storage/libggstorage.a \
           ../thunk/libthunk.a \
           ../sandbox/libggsandbox.a \
           ../trace/libggtrace.a ../trace/libggsyscalltable.a \
           ../protobufs/libggprotobufs.a ../net/libggnet.a ../tui/libggtui.a \
           ../util/libggutil.a

BASE_LDADD = $(GG_LDADD) $(PROTOBUF_LIBS)

bin_PROGRAMS = gg-trace gg-describe gg-force-and-run gg-force gg-mock \
               gg-execute gg-infer gg-thunksummary gg-s3-upload \
               gg-s3-download gg-init lambda-invoker

if BUILD_STATIC_BINS
  bin_PROGRAMS += gg-execute-static
endif

gg_trace_SOURCES = gg-trace.cc
gg_trace_LDADD = $(BASE_LDADD)

gg_describe_SOURCES = gg-describe.cc
gg_describe_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_thunksummary_SOURCES = gg-thunksummary.cc
gg_thunksummary_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_force_and_run_SOURCES = gg-force-and-run.cc
gg_force_and_run_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_execute_SOURCES = gg-execute.cc
gg_execute_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

gg_execute_static_SOURCES = gg-execute.cc
gg_execute_static_CXXFLAGS = $(PICKY_CXXFLAGS)
gg_execute_static_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS) -ldl -lz
gg_execute_static_LDFLAGS = -static -s -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

gg_force_SOURCES = gg-force.cc
gg_force_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

gg_mock_SOURCES = gg-mock.cc
gg_mock_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_infer_SOURCES = gg-infer.cc
gg_infer_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_init_SOURCES = gg-init.cc
gg_init_LDADD = $(BASE_LDADD)

gg_s3_upload_SOURCES = gg-s3-upload.cc
gg_s3_upload_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

gg_s3_download_SOURCES = gg-s3-download.cc
gg_s3_download_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)

lambda_invoker_SOURCES = lambda-invoker.cc
lambda_invoker_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(SSL_LIBS)
