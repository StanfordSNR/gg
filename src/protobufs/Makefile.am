source = gg.proto

SUFFIXES = .proto .pb.cc

AM_CPPFLAGS = -I$(srcdir)/../util $(CXX11_FLAGS) $(PROTOBUF_CFLAGS)

.proto.pb.cc:
	$(AM_V_GEN)$(PROTOC) --cpp_out=. -I$(srcdir) $<

noinst_LIBRARIES = libggprotobufs.a

libggprotobufs_a_SOURCES = $(source) serialization.cc serialization.hh
nodist_libggprotobufs_a_SOURCES = $(source:.proto=.pb.cc) $(source:.proto=.pb.h)

BUILT_SOURCES = $(source:.proto=.pb.cc)
CLEANFILES = $(source:.proto=.pb.cc) $(source:.proto=.pb.h)
