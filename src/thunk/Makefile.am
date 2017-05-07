OS=$(shell uname | tr '[:upper:]' '[:lower:]')
ARCH=$(shell uname -m)

AM_CPPFLAGS = -I$(srcdir)/../$(OS)/$(ARCH)/  $(CXX14_FLAGS) $(JSON_CFLAGS) 
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(NODEBUG_CXXFLAGS)

noinst_LIBRARIES = libthunk.a

libthunk_a_SOURCES = infile_desc.cc infile_desc.hh thunk.cc thunk.hh \
                thunk_func.cc thunk_func.hh thunk_writer.cc thunk_writer.hh

BASE_LDADD = libthunk.a 

bin_PROGRAMS = thunk-writer-test

thunk_writer_test_LDADD = $(BASE_LDADD) $(JSON_LIBS) $(CRYPTO_LIBS)
thunk_writer_test_SOURCES = thunk_writer_test.cc

