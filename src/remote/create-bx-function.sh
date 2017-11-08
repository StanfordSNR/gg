#!/bin/bash

# Requirements:
#   - Bluemix CLI with Cloud Functions plugin, installed and configured.

# step 1: create zip package

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <gg-execute-static>"
    exit 1
fi

GG_EXECUTE=$(readlink -f $1)

pushd wsk_function/
cp ${GG_EXECUTE} gg-execute-static
zip -r ../wskfunction.zip .
rm gg-execute-static
popd

pushd kkv_download/
zip -r ../kkvdownload.zip .
popd

pushd kkv_upload/
zip -r ../kkvupload.zip .
popd

# step 2: installing the function
bx wsk action create gg-function wskfunction.zip --kind nodejs:6 --logsize 10 \
                                                 --memory 512 --timeout 300000

bx wsk action create gg-kkv-download kkvdownload.zip --kind nodejs:6 --logsize 10 \
                                                     --memory 512 --timeout 300000

bx wsk action create gg-kkv-upload kkvupload.zip --kind nodejs:6 --logsize 10 \
                                                 --memory 512 --timeout 300000

rm wskfunction.zip
rm kkvdownload.zip
rm kkvupload.zip
