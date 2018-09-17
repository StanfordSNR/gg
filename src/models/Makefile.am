AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(CRYPTO_CFLAGS) \
              -I$(srcdir)/../../toolchain/include

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BUILT_SOURCES = toolchain.hh toolchain.cc

BASE_LDADD = ../thunk/libthunk.a \
             ../sandbox/libggsandbox.a \
             ../trace/libggtrace.a \
             ../trace/libggsyscalltable.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(CRYPTO_LIBS) $(PROTOBUF_LIBS)

bin_PROGRAMS = model-gcc model-ar model-ranlib model-strip model-ld \
               model-generic model-nm model-readelf generate-deps dir-watcher

COMMON_TOOLCHAIN_SOURCES = cli_description.hh cli_description.cc \
                           toolchain.hh toolchain.cc gcc.hh timeouts.hh

model_gcc_SOURCES = gcc.cc gcc-base.cc preprocessor.cc \
                    linker.cc linker-base.cc gcc-args.cc \
                    $(COMMON_TOOLCHAIN_SOURCES)
model_gcc_LDADD = $(BASE_LDADD)

model_ar_SOURCES = ar.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ar_LDADD = $(BASE_LDADD)

model_ranlib_SOURCES = ranlib.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ranlib_LDADD = $(BASE_LDADD)

model_strip_SOURCES = strip.cc $(COMMON_TOOLCHAIN_SOURCES)
model_strip_LDADD = $(BASE_LDADD)

model_ld_SOURCES = ld.cc linker-base.cc gcc-base.cc $(COMMON_TOOLCHAIN_SOURCES)
model_ld_LDADD = $(BASE_LDADD)

model_generic_SOURCES = generic.cc $(COMMON_TOOLCHAIN_SOURCES)
model_generic_LDADD = $(BASE_LDADD)

model_nm_SOURCES = nm.cc $(COMMON_TOOLCHAIN_SOURCES)
model_nm_LDADD = $(BASE_LDADD)

model_readelf_SOURCES = readelf.cc $(COMMON_TOOLCHAIN_SOURCES)
model_readelf_LDADD = $(BASE_LDADD)

generate_deps_SOURCES = generate-deps.cc gcc-base.cc gcc-args.cc preprocessor.cc \
                        $(COMMON_TOOLCHAIN_SOURCES)
generate_deps_LDADD = $(BASE_LDADD)
generate_deps_LDFLAGS = -static -s

dir_watcher_SOURCES = dir-watcher.cc
dir_watcher_LDADD = $(BASE_LDADD)

EXTRA_DIST = generate-toolchain-header.py wrappers
CLEANFILES = toolchain.hh toolchain.cc

toolchain.cc toolchain.hh: generate-toolchain-header.py $(TOOLCHAIN_PATH)/*
	$(srcdir)/generate-toolchain-header.py $(TOOLCHAIN_PATH)
