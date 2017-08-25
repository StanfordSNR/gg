#!/usr/bin/env python3

import os
import sys
import hashlib

import gg_pb2

from ggpaths import GGPaths

MAGIC_CODE = b'##GGTHUNK##'

def read_thunk(thunk_hash):
    with open(GGPaths.blob_path(thunk_hash), "rb") as fin:
        magic = fin.read(len(MAGIC_CODE))

        if magic != MAGIC_CODE:
            raise Exception("not a thunk: " + thunk_hash)

        thunk = gg_pb2.Thunk()
        thunk.ParseFromString(fin.read())

        return thunk

def executable_hash(thunk):
    hashes = []
    for infile in thunk.infiles:
        if infile.type == gg_pb2.InFile.Type.Value('EXECUTABLE'):
            hashes += [infile.hash]

    hashes.sort()
    return hashlib.sha256("".join(hashes).encode('ascii')).hexdigest()
