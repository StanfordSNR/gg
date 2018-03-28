#!/bin/bash

cp ../../src/gg_sdk.py .

s3bucket="https://s3-us-west-2.amazonaws.com/gg-generic/excam-examples.zip"

wget $s3bucket
unzip -j excam-examples.zip

rm excam-examples.zip ._* .DS_Store

