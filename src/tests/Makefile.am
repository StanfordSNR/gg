AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS) 

LDADD = ../trace/libggtrace.a ../trace/libggsyscalltable.a \
        ../sandbox/libggsandbox.a \
        ../thunk/libthunk.a \
        ../protobufs/libggprotobufs.a ../util/libggutil.a \
        $(PROTOBUF_LIBS) $(CRYPTO_LIBS)

AM_TESTS_ENVIRONMENT = \
	export DATADIR=$(srcdir);

	

EXTRA_DIST = examples/remake.i

check_PROGRAMS = thunk-roundtrip sandbox-test
dist_check_SCRIPTS = compile-model-test.test compile-model-test-python.test download_binaries.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc

TESTS = download_binaries.test thunk-roundtrip sandbox-test compile-model-test.test compile-model-test-python.test

compile-model-test.log: download_binaries.log
compile-model-test-python.log: compile-model-test.log #download_binaries.log

clean-local:
	-rm -rf .gg/
