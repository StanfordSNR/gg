#!/bin/bash -e

USAGE="$0 <JOBS-COUNT>"

JOBS_COUNT=${1?$USAGE}
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/../

./gen_makefile.py 0 6 16 1 > Makefile
gg init
gg-infer make -j$(nproc)
gg force --engine=lambda --jobs=$JOBS_COUNT *.ivf
ls *-vpxenc.ivf | while read each; do echo "file '$each'" >> mylist.txt; done
ffmpeg -f concat -i mylist.txt -codec copy output.avi
