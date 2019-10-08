#!/bin/bash -e

USAGE="$0 <SLEEP-PATH> <MERGE-PATH>"

SLEEP_PATH=${1?$USAGE}
MERGE_PATH=${2?$USAGE}

SLEEP_HASH=$(gg-hash $SLEEP_PATH)
MERGE_HASH=$(gg-hash $MERGE_PATH)

gg init
gg collect $SLEEP_PATH
gg collect $MERGE_PATH

HASHES=$(for i in 1 10 30 60
do
  gg create-thunk --executable $(gg hash $SLEEP_PATH)=sleep --output output \
                  $(gg hash $SLEEP_PATH) sleep ${i} "SLEEP(${i})" 2>&1
done)

gg create-thunk`for h in ${HASHES}; do echo -n ' --future' ${h}; done` \
                --executable $(gg hash $MERGE_PATH)=merge \
                --placeholder out --output output \
                $(gg hash $MERGE_PATH) merge \
                `for h in ${HASHES}; do echo -n ' '@{GGHASH:${h}}; done`
