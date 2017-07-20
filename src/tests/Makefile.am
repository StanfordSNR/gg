AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

LDADD = ../trace/libggtrace.a ../trace/libggsyscalltable.a \
        ../sandbox/libggsandbox.a \
        ../thunk/libthunk.a \
        ../protobufs/libggprotobufs.a ../util/libggutil.a \
        $(PROTOBUF_LIBS) $(CRYPTO_LIBS) \
        $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

AM_TESTS_ENVIRONMENT = \
  export abs_builddir=$(abs_builddir); \
	export DATADIR=$(abs_srcdir)/test_vectors; \
  export TOOLCHAIN_PATH=$(TOOLCHAIN_PATH); \
  export TEST_TMPDIR=$(abs_builddir)/temp; \
  mkdir -p $(abs_builddir)/temp;

EXTRA_DIST = test_vectors/server.c test_vectors/remake.i test_vectors/remake.s \
             test_vectors/libnet.a

check_PROGRAMS = thunk-roundtrip sandbox-test
dist_check_SCRIPTS = model-preprocess.test model-compile.test \
                     model-assemble.test model-link.test model-ar.test \
                     model-ranlib.test cleanup.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)
XFAIL_TESTS = model-link.test

cleanup.log: model-preprocess.log model-compile.log model-assemble.log \
             model-link.log model-ar.log model-ranlib.log

clean-local:
	-rm -rf $(abs_builddir)/tmp
