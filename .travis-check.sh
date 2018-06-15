#!/bin/bash -ex

git clone --depth=3 https://github.com/stanfordsnr/gg /home/user/gg
cd /home/user/gg
git checkout $COMMIT

./fetch-toolchain.sh
./autogen.sh
./configure --enable-sanitize
make -j$(nproc) distcheck V=1 || (cat gg-0.01/_build/sub/tests/test-suite.log && exit 1)
