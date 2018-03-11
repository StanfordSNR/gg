AM_CPPFLAGS = -I$(srcdir)/. -I$(builddir)/.. -I$(srcdir)/.. $(CXX14_FLAGS) \
              $(CRYPTO_CFLAGS)

AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libthunk.a

libthunk_a_SOURCES = thunk.hh function.cc thunk.cc \
                     thunk_writer.cc thunk_writer.hh \
                     thunk_reader.cc thunk_reader.hh \
                     placeholder.cc placeholder.hh \
                     manifest.cc manifest.hh \
                     ggutils.cc ggutils.hh \
                     graph.cc graph.hh \
                     factory.cc factory.hh
