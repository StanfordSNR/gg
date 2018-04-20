#!/bin/bash

textdir="origtext/"

cp ../../src/gg_sdk.py .

mkdir -p ${textdir}

# Download 10 books from Project Gutenberg
wget http://www.gutenberg.org/files/1342/1342-0.txt
wget http://www.gutenberg.org/files/219/219-0.txt
wget http://www.gutenberg.org/files/84/84-0.txt
wget http://www.gutenberg.org/ebooks/844.txt.utf-8
wget http://www.gutenberg.org/files/98/98-0.txt
wget http://www.gutenberg.org/files/11/11-0.txt
wget http://www.gutenberg.org/ebooks/345.txt.utf-8
wget http://www.gutenberg.org/files/2701/2701-0.txt
wget http://www.gutenberg.org/files/76/76-0.txt
wget http://www.gutenberg.org/ebooks/6130.txt.utf-8

mv *.txt *.utf-8 ${textdir}

# Create input file
cat ${textdir}* >> inp_text.txt

# Split into 10K chunks
split -C 10k inp_text.txt
mv inp_text.txt ${textdir}

# Make
make -j$(nproc)

