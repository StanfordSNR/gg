AM_CPPFLAGS = -I$(srcdir)/../protobufs -I$(srcdir)/../util $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libthunk.a

libthunk_a_SOURCES = thunk.hh infile.cc function.cc thunk.cc \
                     thunk_writer.cc thunk_writer.hh \
                     thunk_reader.cc thunk_reader.hh

BASE_LDADD = libthunk.a ../protobufs/libggprotobufs.a

bin_PROGRAMS = thunk-writer-test

thunk_writer_test_LDADD = $(BASE_LDADD) $(CRYPTO_LIBS) $(PROTOBUF_LIBS)
thunk_writer_test_SOURCES = thunk_writer_test.cc
