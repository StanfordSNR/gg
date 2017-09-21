#!/usr/bin/env python3

import os

with open("config") as fin:
    for line in fin:
        line = line.strip()
        line = line.split('=')
        os.environ[line[0]] = line[1]

toolchain_path = os.environ['TOOLCHAIN_PATH']
os.environ['GG_DIR'] = os.environ['SERVER_GG_DIR']
os.environ['GG_RUNNER'] = '1'

import cgi
import sys
import time
import json
import base64
import shutil
import hashlib

import function as lambdafunc
from ggpaths import GGPaths, GG_DIR, make_gg_dirs

def gghash(filename, block_size=65536):
    sha256 = hashlib.sha256()
    size = 0

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            size += len(block)
            sha256.update(block)

    return "{}{:08x}".format(base64.urlsafe_b64encode(sha256.digest()).decode('ascii').replace('=','').replace('-', '.'), size)

def prepare_toolchain(tpath):
    for tbin in os.listdir(tpath):
        bin_path = os.path.join(tpath, tbin)
        bin_hash = gghash(bin_path)
        shutil.copy(bin_path, GGPaths.blob_path(bin_hash))

shutil.rmtree(GG_DIR, ignore_errors=True)
make_gg_dirs()
prepare_toolchain(toolchain_path)

print('Content-Type: text/plain')
print()
print(GG_DIR)
