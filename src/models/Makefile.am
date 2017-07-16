AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              -I$(srcdir)/../../toolchain/include

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BUILT_SOURCES = toolchain.hh

BASE_LDADD = ../thunk/libthunk.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(PROTOBUF_LIBS) \
             $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

bin_PROGRAMS = model-gcc

model_gcc_SOURCES = model-gcc.cc
model_gcc_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

noinst_SCRIPTS = generate-toolchain-header.py
CLEANFILES = toolchain.hh

toolchain.hh: generate-toolchain-header.py $(TOOLCHAIN_PATH)/*
	./generate-toolchain-header.py $(TOOLCHAIN_PATH) >toolchain.hh
