#!/bin/bash

FUNCTION_NAME=gg-gcloud-function
FUNCTION_PATH=gcloud_function
REGION=us-central1
MEMORY=2048
TIMEOUT=300

cp common/{common,ggpaths}.py ${FUNCTION_PATH}

gcloud beta functions deploy --region=${REGION} --memory ${MEMORY} ${FUNCTION_NAME} --trigger-http --quiet --runtime=python37 --source=${FUNCTION_PATH} --timeout=${TIMEOUT}

rm ${FUNCTION_PATH}/{common,ggpaths}.py
