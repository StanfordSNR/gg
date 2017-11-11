#!/bin/bash -ex

# Requirements:
#   - Bluemix CLI with Cloud Functions plugin, installed and configured.

# step 1: create zip package

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <gg-execute-static>"
    exit 1
fi

GG_EXECUTE=$(readlink -f $1)

pushd wsk_function/
rm -rf node_modules/ && npm install
cp ${GG_EXECUTE} gg-execute-static
zip -r ../gg-function-unsecured.zip .
rm gg-execute-static
popd

pushd kkv_download/
rm -rf node_modules/ && npm install
zip -r ../gg-kkv-download-unsecured.zip .
popd

pushd kkv_upload/
rm -rf node_modules/ && npm install
zip -r ../gg-kkv-upload-unsecured.zip .
popd

pushd wsk_function_secure/
rm -rf node_modules/ && npm install
cp ${GG_EXECUTE} gg-execute-static
zip -r ../gg-function.zip .
rm gg-execute-static
popd

pushd kkv_download_secure/
rm -rf node_modules/ && npm install
zip -r ../gg-kkv-download.zip .
popd

pushd kkv_upload_secure/
rm -rf node_modules/ && npm install
zip -r ../gg-kkv-upload.zip .
popd

# step 2: installing the function

for func in gg-function-unsecured gg-kkv-download-unsecured gg-kkv-upload-unsecured gg-function gg-kkv-download gg-kkv-upload
do
  echo ">> ${func}..."
  bx wsk action delete ${func} || exit 0
  bx wsk action create ${func} ${func}.zip --kind nodejs:6 --logsize 10 --memory 512 --timeout 300000
  rm ${func}.zip
done
