#!/bin/bash -e

USAGE="$0 <AVI-PATH>"

AVI_PATH="$(readlink -f ${1?$USAGE})"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/../

# delete prev chunks
./bin/clear.sh

# build from avi source
ffmpeg -i $AVI_PATH -pix_fmt yuv420p input.y4m
ffmpeg -i input.y4m -f segment -segment_time 1 -pix_fmt yuv420p 0000%4d.y4m
