.PHONY: ggfunctions

ggfunctions: $(srcdir)/*.py $(srcdir)/lambda_function/*.py $(srcdir)/common/*.py \
             $(srcdir)/lambda_function/packages.zip \
             $(builddir)/../frontend/gg-execute-static \
             $(TOOLCHAIN_PATH)/*
	$(AM_V_GEN)$(srcdir)/create-function.py --delete --gg-execute-static=$(builddir)/../frontend/gg-execute-static --toolchain-path=$(TOOLCHAIN_PATH) --gg-meow-worker=$(builddir)/../frontend/gg-meow-worker-static
