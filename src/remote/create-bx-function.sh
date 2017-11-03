#!/bin/bash

# Requirements:
#   - Bluemix CLI with Cloud Functions plugin, installed and configured.

# step 1: create zip package

if [ "$#" -ne 2 ]; then
    echo "usage: $0 <gg-execute-static>"
fi

GG_EXECUTE=$(readlink -f $1)

pushd wsk_function/
cp ${GG_EXECUTE} gg-execute-static
zip -r ../wskfunction.zip .
popd

# step 2: isntalling the function
bx wsk action create gg-function wskfunction.zip --kind nodejs:6 --logsize 10 \
                                                 --memory 512 --timeout 300000

rm wskfunction.zip
