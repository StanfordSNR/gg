AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util -I$(srcdir)/../trace \
              -I$(srcdir)/../sandbox -I$(srcdir)/../net \
              -I$(srcdir)/../thunk $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggremote.a

libggremote_a_SOURCES = remote.hh remote.cc

BUILT_SOURCES = gg_pb2.py
CLEANFILES = gg_pb2.py

DIST_EXTRA = gg.py s3.py create-function.py lambda_function/packages.zip \
             lambda_function/function.py gg-lambda-execute.py

gg_pb2.py: $(srcdir)/../protobufs/gg.proto
	$(AM_V_GEN)$(PROTOC) --python_out=. -I$(srcdir)/../protobufs $(srcdir)/../protobufs/gg.proto

.PHONY: ggfunctions

ggfunctions: $(srcdir)/*.py $(srcdir)/lambda_function/*.py \
             $(srcdir)/lambda_function/packages.zip \
             $(builddir)/../frontend/gg-execute-static \
             $(TOOLCHAIN_PATH)/*
	$(AM_V_GEN)$(srcdir)/create-function.py --delete --gg-execute-static=$(builddir)/../frontend/gg-execute-static --toolchain-path=$(TOOLCHAIN_PATH)
