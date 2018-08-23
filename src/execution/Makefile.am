AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(PROTOBUF_CFLAGS) $(SSL_CFLAGS) $(SSL_CFLAGS) $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggexecution.a

libggexecution_a_SOURCES = response.hh response.cc \
                           connection.hh \
                           loop.hh loop.cc \
                           engine.hh \
                           engine_local.hh engine_local.cc \
                           engine_lambda.hh engine_lambda.cc \
                           engine_gg.hh engine_gg.cc \
                           engine_gcloud.hh engine_gcloud.cc \
                           meow/message.hh meow/message.cc \
                           meow/util.hh meow/util.cc \
                           engine_meow.hh engine_meow.cc \
                           reductor.hh reductor.cc
