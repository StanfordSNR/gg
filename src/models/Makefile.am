AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util \
              -I$(srcdir)/../trace -I$(srcdir)/../thunk \
              -I$(srcdir)/../sandbox $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../thunk/libthunk.a \
             ../protobufs/libggprotobufs.a \
             ../util/libggutil.a \
             $(PROTOBUF_LIBS) \
             $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

EXTRA_DIST = model-compile.py

bin_PROGRAMS = model-gcc

model_gcc_SOURCES = model-gcc.cc utils.hh
model_gcc_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)
