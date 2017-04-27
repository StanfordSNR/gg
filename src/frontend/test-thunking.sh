#!/bin/bash -e
set -e

make
cd ../../../exploration/targets/make-4.2/
rm remake.o || true
../../../gg/src/frontend/gg-gcc gcc -DLOCALEDIR=\"/usr/local/share/locale\" -DLIBDIR=\"/usr/local/lib\" -DINCLUDEDIR=\"/usr/local/include\" -DHAVE_CONFIG_H -I.      -g -O2 -MT remake.o -MD -MP -MF .deps/remake.Tpo -c -o remake.o remake.c
cd - > /dev/null
