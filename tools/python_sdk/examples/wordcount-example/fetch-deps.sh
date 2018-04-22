#!/bin/bash

textdir="origtext/"

cp ../../src/gg_sdk.py .

mkdir -p ${textdir}

# Download books from Project Gutenberg
s3bucket="https://s3-us-west-2.amazonaws.com/gg-generic/Gutenberg.zip"

wget $s3bucket
unzip -j Gutenberg.zip

rm .fuse_hidden* Gutenberg.zip .DS_Store ._*

mv *.txt ${textdir}

# Create input file
cat ${textdir}* >> inp_text.txt

# Split into 100K chunks
split -C 100k inp_text.txt
mv inp_text.txt ${textdir}

# Make
make -j$(nproc)

