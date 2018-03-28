#!/usr/bin/env python3

import os

GG_DIR = os.environ.get('GG_DIR')

if not GG_DIR:
    raise Exception("GG_DIR environment variable not set")

if not os.path.isabs(GG_DIR):
    raise Exception("GG_DIR must be an absolute path")

class GGPaths:
    blobs = os.path.join(GG_DIR, "blobs")
    reductions = os.path.join(GG_DIR, "reductions")

    @classmethod
    def blob_path(cls, blob_hash):
        return os.path.join(cls.blobs, blob_hash)

    @classmethod
    def reduction_path(cls, blob_hash):
        return os.path.join(cls.reductions, blob_hash)

    @classmethod
    def object_url(cls, bucket, key):
        return "https://{bucket}.s3.amazonaws.com/{key}".format(bucket=bucket, key=key)

class GGCache:
    @classmethod
    def check(cls, thunk_hash, output_tag=None):
        key = thunk_hash
        if output_tag:
            key += ("#%s" % output_tag)

        rpath = GGPaths.reduction_path(key)

        if not os.path.exists(rpath):
            return None

        with open(rpath, "r") as fin:
            data = fin.read()
            data = data.split(" ")
            return data[0]

    @classmethod
    def insert(cls, old_hash, new_hash):
        rpath = GGPaths.reduction_path(old_hash)

        with open(rpath, "w") as fout:
            fout.write(new_hash)

def make_gg_dirs():
    os.makedirs(GGPaths.blobs, exist_ok=True)
    os.makedirs(GGPaths.reductions, exist_ok=True)

make_gg_dirs()
