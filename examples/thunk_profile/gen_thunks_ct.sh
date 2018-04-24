#!/bin/bash -e

USAGE="$0 <num-thunks>"

N=${1?$USAGE}

gg-init

for i in `seq 1 ${N}`;
do
  gg-create-thunk --value "DUMMY_VALUE${i}" \
                  --executable "DUMMY_EXECUTABLE${i}" \
                  --output "DUMMY_OUTPUT${i}" \
                  --placeholder "OUTPUT${i}" \
                  "DUMMY_EXECUTABLE${i}" "my_func" "inp${i}"
done

