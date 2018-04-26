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

bin_PROGRAMS = gen_vid_thunks gen_vid_thunks25

gen_vid_thunks_SOURCES = gen_vid_thunks.cc
gen_vid_thunks_LDADD = $(BASE_LDADD)
gen_vid_thunks_LDFLAGS = -s

gen_vid_thunks25_SOURCES = gen_vid_thunks25.cc
gen_vid_thunks25_LDADD = $(BASE_LDADD)
gen_vid_thunks25_LDFLAGS = -s

if BUILD_STATIC_BINS
  gen_vid_thunks_LDFLAGS += -static
  gen_vid_thunks25_LDFLAGS += -static
endif
