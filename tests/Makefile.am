AM_CPPFLAGS = -I$(srcdir)/../src -I$(builddir)/../src $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

LDADD = ../src/thunk/libthunk.a \
        ../src/sandbox/libggsandbox.a \
        ../src/trace/libggtrace.a \
        ../src/trace/libggsyscalltable.a \
        ../src/protobufs/libggprotobufs.a \
        ../src/util/libggutil.a \
        $(PROTOBUF_LIBS) $(CRYPTO_LIBS)

AM_TESTS_ENVIRONMENT = \
  export TMPDIR_ROOT=$(abs_builddir)/test_temp; \
  mkdir -p $$TMPDIR_ROOT; \
  export abs_builddir=$(abs_builddir); \
  export abs_srcdir=$(abs_srcdir); \
	export DATADIR=$(abs_builddir)/test_vectors; \
  export TOOLCHAIN_PATH=$(TOOLCHAIN_PATH); \
  export TEST_TMPDIR=`mktemp -d $$TMPDIR_ROOT/test.XXXXXX`; \
  export GG_DIR=$$TEST_TMPDIR/__gg_data__; \
  unset GG_LAMBDA; \
  unset GG_REMOTE;

check_PROGRAMS = thunk-roundtrip sandbox-test path-test
dist_check_SCRIPTS = fibonacci.test cleanup.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc
path_test_SOURCES = path-test.cc

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)

cleanup.log: fibonacci.log

clean-local:
	-rm -rf $(abs_builddir)/test_temp
	-rm -rf $(abs_builddir)/test_vectors
