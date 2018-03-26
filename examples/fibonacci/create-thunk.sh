#!/bin/bash -e

FIB_PATH=$(dirname $0)/fib
ADD_PATH=$(dirname $0)/add

FIB_HASH=$(gg-hash $FIB_PATH)
ADD_HASH=$(gg-hash $ADD_PATH)

N=${1?"Usage: $0 <N>"}

gg-create-thunk --envar FIB_FUNCTION_HASH=${FIB_HASH} \
                --envar ADD_FUNCTION_HASH=${ADD_HASH} \
                --executable ${FIB_HASH} \
                --output out \
                --output left \
                --output right \
                --placeholder fib${N}_output \
                ${FIB_HASH} fib ${N}

gg-collect $FIB_PATH $ADD_PATH
