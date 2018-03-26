#!/bin/bash -e

FIB_PATH=$(dirname $0)/fib
ADD_PATH=$(dirname $0)/add

gg-put $FIB_PATH $ADD_PATH
