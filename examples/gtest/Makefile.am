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

bin_PROGRAMS = gtest-wrapper gtest-create-thunks

gtest_wrapper_SOURCES = wrapper.cc
gtest_wrapper_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)
gtest_wrapper_LDFLAGS = -s

gtest_create_thunks_SOURCES = create-thunks.cc
gtest_create_thunks_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS)

if BUILD_STATIC_BINS
  gtest_wrapper_LDFLAGS += -static
endif
