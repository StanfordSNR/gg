AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util -I$(srcdir)/../trace \
              -I$(srcdir)/../sandbox -I$(srcdir)/../net \
              -I$(srcdir)/../thunk $(CXX14_FLAGS) $(PROTOBUF_CFLAGS) \
              $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggexecution.a

libggexecution_a_SOURCES = remote_response.hh remote_response.cc \
                           connection_context.hh connection_context.cc \
                           remote_lambda.hh remote_lambda.cc \
                           remote_gg.hh remote_gg.cc \
                           loop.hh loop.cc
