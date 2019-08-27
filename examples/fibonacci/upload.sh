#!/bin/bash -e

USAGE="$0 <FIB-PATH> <ADD-PATH>"

FIB_PATH=${1?$USAGE}
ADD_PATH=${2?$USAGE}

gg-put $FIB_PATH $ADD_PATH
