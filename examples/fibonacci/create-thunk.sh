#!/bin/bash -e

USAGE="$0 <N> <FIB-PATH> <ADD-PATH>"

N=${1?$USAGE}
FIB_PATH=${2?$USAGE}
ADD_PATH=${3?$USAGE}

FIB_HASH=$(gg-hash $FIB_PATH)
ADD_HASH=$(gg-hash $ADD_PATH)

gg-create-thunk --envar FIB_FUNCTION_HASH=${FIB_HASH} \
                --envar ADD_FUNCTION_HASH=${ADD_HASH} \
                --executable ${FIB_HASH} \
                --executable ${ADD_HASH} \
                --output out \
                --output left \
                --output right \
                --placeholder fib${N}_output \
                ${FIB_HASH} fib ${N}

gg-collect $FIB_PATH $ADD_PATH
