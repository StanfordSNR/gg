export TOOLCHAIN_PATH := $(TOOLCHAIN_PATH)

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

EXTRA_DIST = test_vectors/server.c test_vectors/remake.i test_vectors/remake.s

check_PROGRAMS = thunk-roundtrip sandbox-test
dist_check_SCRIPTS = fetch-toolchain.test preprocess-model.test compile-model.test assemble-model.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)
XFAIL_TESTS = preprocess-model.test

preprocess-model.log: fetch-toolchain.log
compile-model.log: preprocess-model.log
assemble-model.log: compile-model.log

clean-local:
	-rm -rf toolchain/
	-rm -rf .gg/
	-rm -rf *.i *.i.gold *.s *.s.gold *.o *.o.gold
