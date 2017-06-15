AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../thunk/libthunk.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(PROTOBUF_LIBS)

EXTRA_DIST = gg-model-compile.py

bin_PROGRAMS = gg-model-compile gg-model-assemble gg-model-gcc

gg_model_compile_SOURCES = gg-model-compile.cc gg-model-base.hh gg-model-base.cc
gg_model_compile_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_model_assemble_SOURCES = gg-model-assemble.cc gg-model-base.hh gg-model-base.cc
gg_model_assemble_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

gg_model_gcc_SOURCES = gg-model-gcc.cc
gg_model_gcc_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)
