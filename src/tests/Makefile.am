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
dist_check_SCRIPTS = fetch-toolchain.test model-preprocess.test model-compile.test \
                     model-assemble.test model-ar.test

thunk_roundtrip_SOURCES = thunk-roundtrip.cc
sandbox_test_SOURCES = sandbox-test.cc

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)

model-preprocess.log: fetch-toolchain.log
model-compile.log: model-preprocess.log
model-assemble.log: model-compile.log
model-ar.log: model-assemble.log

clean-local:
	-rm -rf toolchain/
	-rm -rf .gg/
	-rm -f *.i *.i.gold *.s *.s.gold *.o *.o.gold archive.a archive.a.gold
	-rm -f f1.txt f2.txt f3.txt
