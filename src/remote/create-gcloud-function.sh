#!/bin/bash -e

FUNCTION_NAME=gg-gcloud-function
FUNCTION_PATH=gcloud_function
REGION=us-central1
MEMORY=2048
TIMEOUT=300

cp common/{common,ggpaths}.py ${FUNCTION_PATH}
cp $(which gg-execute-static) ${FUNCTION_PATH}

gcloud beta functions deploy --region=${REGION} --memory ${MEMORY} ${FUNCTION_NAME} --trigger-http --quiet --runtime=python37 --source=${FUNCTION_PATH} --timeout=${TIMEOUT} --format=json

rm ${FUNCTION_PATH}/{common,ggpaths}.py
rm ${FUNCTION_PATH}/gg-execute-static

echo
echo "export GG_GCLOUD_FUNCTION=[https-trigger-url]"
