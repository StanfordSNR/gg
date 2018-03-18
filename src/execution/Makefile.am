AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggexecution.a

libggexecution_a_SOURCES = response.hh response.cc \
                           connection_context.hh connection_context.cc \
                           loop.hh loop.cc \
                           engine.hh \
                           engine_local.hh engine_local.cc \
                           engine_lambda.hh engine_lambda.cc \
                           engine_gg.hh engine_gg.cc \
                           reductor.hh reductor.cc
