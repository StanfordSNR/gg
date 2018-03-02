DIST_EXTRA = gg.py s3.py create-function.py lambda_function/packages.zip \
             lambda_function/function.py gg-lambda-execute.py

.PHONY: ggfunctions

ggfunctions: $(srcdir)/*.py $(srcdir)/lambda_function/*.py \
             $(srcdir)/lambda_function/packages.zip \
             $(builddir)/../frontend/gg-execute-static \
             $(builddir)/../frontend/gg-s3-download \
             $(TOOLCHAIN_PATH)/*
	$(AM_V_GEN)$(srcdir)/create-function.py --delete --gg-execute-static=$(builddir)/../frontend/gg-execute-static --toolchain-path=$(TOOLCHAIN_PATH)
