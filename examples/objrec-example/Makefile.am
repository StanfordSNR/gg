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

bin_PROGRAMS = ormap has_wall do_merge

ormap_SOURCES = ormap.cc
ormap_LDADD = $(BASE_LDADD)
ormap_LDFLAGS = -s

has_wall_SOURCES = has_wall.cc
has_wall_LDFLAGS = -s

do_merge_SOURCES = do_merge.cc
do_merge_LDFLAGS = -s

if BUILD_STATIC_BINS
  ormap_LDFLAGS += -static
  has_wall_LDFLAGS += -static
  do_merge_LDFLAGS += -static
endif

