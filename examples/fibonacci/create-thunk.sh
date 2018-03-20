#!/bin/bash

FIB_HASH=$(gg-hash fib)
ADD_HASH=$(gg-hash add)

N=${1?"Usage: $0 <N>"}

gg-create-thunk --envar FIB_FUNCTION_HASH=${FIB_HASH} \
                --envar ADD_FUNCTION_HASH=${ADD_HASH} \
                --executable ${FIB_HASH} \
                --output out \
                --output left \
                --output right \
                --placeholder fib${N}_output \
                ${FIB_HASH} fib ${N}

gg-collect fib add
