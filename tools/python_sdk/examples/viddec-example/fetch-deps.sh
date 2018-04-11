#!/bin/bash

cp ../../src/gg_sdk.py .

s3bucket="https://s3-us-west-2.amazonaws.com/gg-generic/vid_dec_small.zip"

wget $s3bucket
unzip -j vid_dec_small.zip
chmod +x li-static

wget https://johnvansickle.com/ffmpeg/releases/ffmpeg-release-64bit-static.tar.xz
tar xvf ffmpeg-release-64bit-static.tar.xz --wildcards ffmpeg-*/ffmpeg
mv ffmpeg-*/ffmpeg .

rm -rf vid_dec_small.zip ffmpeg-* ._* .DS_Store

