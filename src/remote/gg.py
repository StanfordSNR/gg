#!/usr/bin/env python3

import os
import sys
import hashlib

import gg_pb2

MAGIC_CODE = b'##GGTHUNK##'

if 'GG_DIR' not in os.environ:
    raise Exception("GG_DIR not set.")

GG_DIR = os.path.abspath(os.environ['GG_DIR'])

class Path:
    blobs = GG_DIR
    reductions = os.path.join(GG_DIR, "reductions")

    @classmethod
    def blob_path(cls, blob_hash):
        return os.path.join(cls.blobs, blob_hash)

def blob_path(blob_hash):
    return os.path.join(GG_DIR, blob_hash)

def reduction_path(blob_hash):
    return os.path.join(GG_DIR, "reductions", blob_hash)

def read_thunk(thunk_hash):
    with open(Path.blob_path(thunk_hash), "rb") as fin:
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

if __name__ == '__main__':
    thunk = read_thunk("e1c4371ef825d5e7414b1b09cca2b991727ed585c107b8d4ed49419cb04888f1")
    print(executable_hash(thunk))
