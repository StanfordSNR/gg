EXTRA_DIST = create-thunk.sh

AM_CPPFLAGS = -I$(srcdir)/../../src/. -I$(builddir)/../../src/. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD = ../../src/thunk/libthunk.a \
             ../../src/sandbox/libggsandbox.a \
             ../../src/trace/libggtrace.a \
             ../../src/trace/libggsyscalltable.a \
             ../../src/protobufs/libggprotobufs.a \
             ../../src/util/libggutil.a \
             $(CRYPTO_LIBS) $(PROTOBUF_LIBS)

bin_PROGRAMS = fib add

fib_SOURCES = fib.cc
fib_LDADD = $(BASE_LDADD)
fib_LDFLAGS = -s

add_SOURCES = add.cc
add_LDFLAGS = -s

if BUILD_STATIC_BINS
  fib_LDFLAGS += -static
  add_LDFLAGS += -static
endif
