AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              -I$(srcdir)/../../toolchain/include

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BUILT_SOURCES = toolchain.hh toolchain.cc

BASE_LDADD = ../thunk/libthunk.a \
             ../sandbox/libggsandbox.a \
             ../trace/libggtrace.a ../trace/libggsyscalltable.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(CRYPTO_LIBS) $(PROTOBUF_LIBS)

bin_PROGRAMS = model-gcc model-ar model-ranlib model-strip

COMMON_TOOLCHAIN_SOURCES = toolchain.hh toolchain.cc model-gcc.hh

model_gcc_SOURCES = model-gcc.cc model-gcc-base.cc  model-preprocessor.cc \
                    model-linker.cc gcc-args.cc \
                    $(COMMON_TOOLCHAIN_SOURCES)
model_gcc_LDADD = $(BASE_LDADD)

model_ar_SOURCES = model-ar.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ar_LDADD = $(BASE_LDADD)

model_ranlib_SOURCES = model-ranlib.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ranlib_LDADD = $(BASE_LDADD)

model_strip_SOURCES = model-strip.cc $(COMMON_TOOLCHAIN_SOURCES)
model_strip_LDADD = $(BASE_LDADD)

EXTRA_DIST = generate-toolchain-header.py
CLEANFILES = toolchain.hh toolchain.cc

toolchain.cc toolchain.hh: generate-toolchain-header.py $(TOOLCHAIN_PATH)/*
	$(srcdir)/generate-toolchain-header.py $(TOOLCHAIN_PATH)
