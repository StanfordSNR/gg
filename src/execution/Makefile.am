AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util -I$(srcdir)/../trace \
              -I$(srcdir)/../sandbox -I$(srcdir)/../net \
              -I$(srcdir)/../thunk $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggexecution.a

libggexecution_a_SOURCES = remote.hh remote.cc
