BUILT_SOURCES = gg_pb2.py
CLEANFILES = gg_pb2.py

gg_pb2.py: $(srcdir)/../protobufs/gg.proto
	$(AM_V_GEN)$(PROTOC) --python_out=. -I$(srcdir)/../protobufs $(srcdir)/../protobufs/gg.proto
