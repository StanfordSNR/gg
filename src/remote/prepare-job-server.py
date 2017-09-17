#!/usr/bin/env python3

import os
import sys
import hashlib
import base64
import shutil

def gghash(filename, block_size=65536):
    sha256 = hashlib.sha256()
    size = 0

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            size += len(block)
            sha256.update(block)

    return "{}{:08x}".format(base64.urlsafe_b64encode(sha256.digest()).decode('ascii').replace('=','').replace('-', '.'), size)


toolchain_path = sys.argv[1]
target_path = sys.argv[2]

os.makedirs(target_path, exist_ok=True)

for tbin in os.listdir(toolchain_path):
    bin_path = os.path.join(toolchain_path, tbin)
    bin_hash = gghash(bin_path)
    shutil.copy(bin_path, os.path.join(target_path, bin_hash))
