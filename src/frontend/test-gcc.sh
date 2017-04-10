#!/bin/bash -e
set -e

g++ gg-gcc.cc
cd ../../exploration/targets/make-4.2/
rm remake.o || true
../../../src/frontend/a.out gcc -DLOCALEDIR=\"/usr/local/share/locale\" -DLIBDIR=\"/usr/local/lib\" -DINCLUDEDIR=\"/usr/local/include\" -DHAVE_CONFIG_H -I.      -g -O2 -MT remake.o -MD -MP -MF .deps/remake.Tpo -c -o remake.o remake.c
DIFF=$(diff remake.o remake.o.bak)
if [ "$DIFF" == "" ]
then
    echo "GCC TEST PASSED";
else
    echo "~~~~~~GCC TEST FAILED!!!!";
fi
cd - > /dev/null