#!/bin/bash -e

USAGE="$0 <JOBS-COUNT>"

JOBS_COUNT=${1?$USAGE}
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/../

printf "1. Generate Makefile\n"
./gen_makefile.py 0 6 16 1 > Makefile

printf "2. Initialize gg\n"
rm -rf .gg
gg init

printf "3. Execute 'make' to create thunks\n"
gg-infer make -j$(nproc)

printf "4. Run video processing jobs\n"
gg force --jobs=$JOBS_COUNT --engine=lambda *.ivf

printf "5. Build output.avi\n"
ls *-vpxenc.ivf | while read each; do echo "file '$each'" >> mylist.txt; done
ffmpeg -f concat -i mylist.txt -codec copy output.avi

printf "6. Result:\n"
file output.avi
