#!/bin/bash -ex

cd /home/user/gg

./fetch-toolchain.sh
./autogen.sh
./configure --enable-sanitize
make -j$(nproc) distcheck V=1 || (cat gg-0.01/_build/sub/tests/test-suite.log && exit 1)
