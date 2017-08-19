AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

LDADD = ../thunk/libthunk.a \
        ../sandbox/libggsandbox.a \
        ../trace/libggtrace.a ../trace/libggsyscalltable.a \
        ../protobufs/libggprotobufs.a ../util/libggutil.a \
        $(PROTOBUF_LIBS) $(CRYPTO_LIBS)

AM_TESTS_ENVIRONMENT = \
  export TMPDIR_ROOT=$(abs_builddir)/test_temp; \
  mkdir -p $$TMPDIR_ROOT; \
  export abs_builddir=$(abs_builddir); \
	export DATADIR=$(abs_builddir)/test_vectors; \
  export TOOLCHAIN_PATH=$(TOOLCHAIN_PATH); \
  export TEST_TMPDIR=`mktemp -d $$TMPDIR_ROOT/test.XXXXXX`; \
  export GG_DIR=$$TEST_TMPDIR/__gg_data__;

check_PROGRAMS = thunk-roundtrip sandbox-test path-test
dist_check_SCRIPTS = fetch-vectors.test \
                     model-preprocess.test model-preprocess-makedep.test \
                     model-compile.test model-assemble.test model-link.test \
                     model-ar.test model-ranlib.test model-strip.test \
                     gnu-hello.test mosh.test \
                     cleanup.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc
path_test_SOURCES = path-test.cc

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)

model-preprocess.log: fetch-vectors.log
model-preprocess-makedep.log: fetch-vectors.log
model-compile.log: fetch-vectors.log
model-assemble.log: fetch-vectors.log
model-link.log: fetch-vectors.log
model-ar.log: fetch-vectors.log
model-ranlib.log: fetch-vectors.log
model-strip.log: fetch-vectors.log
gnu-hello.log: fetch-vectors.log
mosh.log: fetch-vectors.log

cleanup.log: model-preprocess.log model-preprocess-makedep.log \
             model-compile.log model-assemble.log model-link.log model-ar.log \
             model-ranlib.log model-strip.log gnu-hello.log mosh.log

clean-local:
	-rm -rf $(abs_builddir)/test_temp
	-rm -rf $(abs_builddir)/test_vectors
