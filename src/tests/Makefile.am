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
	export DATADIR=$(srcdir);

EXTRA_DIST = examples/remake.i examples/remake.s

check_PROGRAMS = thunk-roundtrip sandbox-test
dist_check_SCRIPTS = compile-model.test download_binaries.test assemble-model.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)

compile-model-test.log: download_binaries.log
compile-model-test-python.log: compile-model-test.log
assemble-model.log: compile-model-test-python.log

clean-local:
	-rm -rf .gg/
