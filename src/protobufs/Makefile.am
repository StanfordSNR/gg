source = thunk.proto gg.proto meow.proto

SUFFIXES = .proto .pb.cc

AM_CPPFLAGS = $(CXX14_FLAGS) $(PROTOBUF_CFLAGS)

.proto.pb.cc:
	$(AM_V_GEN)$(PROTOC) --cpp_out=. -I$(srcdir) $<

noinst_LIBRARIES = libggprotobufs.a

libggprotobufs_a_SOURCES = util.hh util.cc $(source)
nodist_libggprotobufs_a_SOURCES = $(source:.proto=.pb.cc) $(source:.proto=.pb.h)

BUILT_SOURCES = $(source:.proto=.pb.cc)
CLEANFILES = $(source:.proto=.pb.cc) $(source:.proto=.pb.h)
