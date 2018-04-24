#!/bin/bash -e

USAGE="$0 <num-thunks>"

N=${1?$USAGE}

for i in `seq 1 ${N}`;
do
  ./test_thunk_gen 1
done

