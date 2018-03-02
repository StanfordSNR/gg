AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(CRYPTO_CFLAGS) \
              -I$(srcdir)/../../toolchain/include

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BUILT_SOURCES = toolchain.hh toolchain.cc

BASE_LDADD = ../thunk/libthunk.a \
             ../sandbox/libggsandbox.a \
             ../trace/libggtrace.a ../trace/libggsyscalltable.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(CRYPTO_LIBS) $(PROTOBUF_LIBS)

bin_PROGRAMS = model-gcc model-ar model-ranlib model-strip model-ld \
               model-generic

COMMON_TOOLCHAIN_SOURCES = cli_description.hh cli_description.cc \
                           toolchain.hh toolchain.cc model-gcc.hh \
                           manifest.hh manifest.cc

model_gcc_SOURCES = model-gcc.cc model-gcc-base.cc  model-preprocessor.cc \
                    model-linker.cc model-linker-base.cc gcc-args.cc \
                    $(COMMON_TOOLCHAIN_SOURCES)
model_gcc_LDADD = $(BASE_LDADD)

model_ar_SOURCES = model-ar.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ar_LDADD = $(BASE_LDADD)

model_ranlib_SOURCES = model-ranlib.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ranlib_LDADD = $(BASE_LDADD)

model_strip_SOURCES = model-strip.cc $(COMMON_TOOLCHAIN_SOURCES)
model_strip_LDADD = $(BASE_LDADD)

model_ld_SOURCES = model-ld.cc model-linker-base.cc model-gcc-base.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ld_LDADD = $(BASE_LDADD)

model_generic_SOURCES = model-generic.cc $(COMMON_TOOLCHAIN_SOURCES)
model_generic_LDADD = $(BASE_LDADD)

EXTRA_DIST = generate-toolchain-header.py wrappers
CLEANFILES = toolchain.hh toolchain.cc

toolchain.cc toolchain.hh: generate-toolchain-header.py $(TOOLCHAIN_PATH)/*
	$(srcdir)/generate-toolchain-header.py $(TOOLCHAIN_PATH)
