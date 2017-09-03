#!/usr/bin/env python3

import os
import sys
import base64
import boto3

import gg_pb2

from ggpaths import GGPaths
from common import make_executable

MAGIC_CODE = b'##GGTHUNK##'
s3_client = boto3.client('s3')

class NotAThunkException(Exception):
    pass

def read_thunk(thunk_hash, s3_bucket=None):
    tpath = GGPaths.blob_path(thunk_hash)

    if not os.path.exists(tpath) and not s3_bucket:
        raise Exception("thunk is not locally available")
    elif not os.path.exists(tpath):
        try:
            s3_client.download_file(Bucket=s3_bucket, Key=thunk_hash, Filename=tpath)
            tags = s3_client.get_object_tagging(Bucket=s3_bucket, Key=thunk_hash)
            tags = tags.get('TagSet', [])

            for tag in tags:
                if tag['Key'] == 'gg:executable' and tag['Value'] == 'true':
                    make_executable(tpath)
                    break
        except:
            Exception("thunk is not available: {}".format(thunk_hash))

    with open(tpath, "rb") as fin:
        magic = fin.read(len(MAGIC_CODE))

        if magic != MAGIC_CODE:
            raise NotAThunkException("not a thunk: {}".format(thunk_hash))

        thunk = gg_pb2.Thunk()
        thunk.ParseFromString(fin.read())

        return thunk

def executable_hash(hashes):
    hashes.sort()
    str_to_hash = "".join(hashes)
    return "{}".format(base64.urlsafe_b64encode(hashlib.sha256(str_to_hash).digest()).replace('=',''))

def thunk_executable_hash(thunk):
    hashes = []
    for infile in thunk.infiles:
        if infile.type == gg_pb2.InFile.Type.Value('EXECUTABLE'):
            hashes += [infile.hash]

    return executable_hash(hashes)
