#!/bin/bash -e

USAGE="$0 <N> <JOBS-COUNT>"

N=${1?$USAGE}
JOBS_COUNT=${2?$USAGE}

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/../

printf "1. Clear workspace\n"
./bin/clear.sh

printf "2. Initialize gg\n"
gg init

printf "3. Create thunks for number %s\n" "$N"
./create-thunk.sh $N ./fib ./add

printf "4. Run calculation\n"
gg force --jobs=$JOBS_COUNT --engine=lambda "fib${N}_output"

printf "5. Result: %s\n" $(cat fib${N}_output)
