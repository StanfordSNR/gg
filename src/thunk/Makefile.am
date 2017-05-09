AM_CPPFLAGS = -I../protobufs -I$(srcdir)/../util $(CXX14_FLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libthunk.a

libthunk_a_SOURCES = thunk.hh infile.cc function.cc thunk.cc \
                     thunk_writer.cc thunk_writer.hh \
                     thunk_reader.cc thunk_reader.hh
