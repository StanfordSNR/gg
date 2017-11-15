AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util -I$(srcdir)/../tui \
              -I$(srcdir)/../trace -I$(srcdir)/../sandbox -I$(srcdir)/../net \
              -I$(srcdir)/../thunk $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggstorage.a

libggstorage_a_SOURCES = backend.hh \
                         backend_s3.hh backend_s3.cc
