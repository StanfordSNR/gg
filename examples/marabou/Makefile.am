AM_CPPFLAGS = -I$(srcdir)/../../src/. -I$(builddir)/../../src/. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

BASE_LDADD =

bin_PROGRAMS = merge sleep

merge_SOURCES = merge.cc
merge_LDADD = $(BASE_LDADD)
merge_LDFLAGS = -s

sleep_SOURCES = sleep.cc
sleep_LDADD = $(BASE_LDADD)
sleep_LDFLAGS = -s

if BUILD_STATIC_BINS
  merge_LDFLAGS += -static
  sleep_LDFLAGS += -static
endif
