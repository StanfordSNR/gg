BUILT_SOURCES = gg_pb2.py
CLEANFILES = gg_pb2.py

if BUILD_STATIC_BINS
BUILT_SOURCES += ggfunction.zip
CLEANFILES += ggfunction.zip
endif

DIST_EXTRA = gg.py s3.py create-function.py lambda/packages.zip lambda/function.py

gg_pb2.py: $(srcdir)/../protobufs/gg.proto
	$(AM_V_GEN)$(PROTOC) --python_out=. -I$(srcdir)/../protobufs $(srcdir)/../protobufs/gg.proto

ggfunction.zip: $(srcdir)/create-function.py $(srcdir)/lambda/function.py $(srcdir)/lambda/packages.zip $(builddir)/../frontend/gg-execute-static
	$(AM_V_GEN)$(srcdir)/create-function.py --function-file=ggfunction.zip
