AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              -I$(srcdir)/../../toolchain/include

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BUILT_SOURCES = toolchain.hh

BASE_LDADD = ../thunk/libthunk.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(CRYPTO_LIBS) $(PROTOBUF_LIBS) \
             $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

bin_PROGRAMS = model-gcc model-ar model-ranlib

model_gcc_SOURCES = model-gcc.cc utils.hh
model_gcc_LDADD = $(BASE_LDADD)

model_ar_SOURCES = model-ar.cc
model_ar_LDADD = $(BASE_LDADD)

model_ranlib_SOURCES = model-ranlib.cc
model_ranlib_LDADD = $(BASE_LDADD)

EXTRA_DIST = generate-toolchain-header.py
CLEANFILES = toolchain.hh

toolchain.hh: $(TOOLCHAIN_PATH)/*
	$(srcdir)/generate-toolchain-header.py $(TOOLCHAIN_PATH) >toolchain.hh
