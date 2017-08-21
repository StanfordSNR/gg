BUILT_SOURCES = gg_pb2.py lambda/infile.js lambda/function.js lambda/thunk.js
CLEANFILES = gg_pb2.py lambda/infile.js lambda/function.js lambda/thunk.js

if BUILD_STATIC_BINS
	BUILT_SOURCES += ggfunction.zip
	CLEANFILES += ggfunction.zip
endif

lambda/infile.js lambda/function.js lambda/thunk.js gg_pb2.py: $(srcdir)/../protobufs/gg.proto
	$(AM_V_GEN)$(PROTOC) --python_out=. --js_out=./lambda/ -I$(srcdir)/../protobufs $(srcdir)/../protobufs/gg.proto

ggfunction.zip: $(srcdir)/create-function.py $(TOOLCHAIN_PATH)/* lambda/*
	$(srcdir)/create-function.py --toolchain-path=$(TOOLCHAIN_PATH) --function-file=ggfunction.zip
