#!/usr/bin/env python3.6

import os
import sys

curdir = os.path.dirname(__file__)
sys.path.append(os.path.join(curdir, 'packages'))
os.environ['PATH'] = "{}:{}".format(curdir, os.environ.get('PATH', ''))

if not os.environ.get('GG_DIR'):
    os.environ['GG_DIR'] = "/tmp/_gg"

GG_DIR = os.environ['GG_DIR']

import stat
import subprocess as sub
import shutil
import asyncio
import aiohttp
import boto3

from ggpaths import GGPaths, GGCache
from downloader import download_files
from common import is_executable, make_executable, run_command

s3_client = boto3.client('s3')

class GGInfo:
    s3_bucket = None
    thunk_hash = None
    infiles = []

def fetch_dependencies(infiles):
    download_list = []

    for infile in infiles:
        bpath = GGPaths.blob_path(infile['hash'])
        if os.path.exists(bpath) and os.path.getsize(bpath) == infile['size']:
            continue

        download_list += [{
            'url': GGPaths.object_url(GGInfo.s3_bucket, infile['hash']),
            'filename': bpath,
        }]

    download_files(download_list)

def handler(event, context):
    GGInfo.thunk_hash = event['thunk_hash']
    GGInfo.s3_bucket = event['s3_bucket']
    GGInfo.infiles = event['infiles']

    fetch_dependencies(GGInfo.infiles)

    for infile in GGInfo.infiles:
        if infile['executable']:
            make_executable(GGPaths.blob_path(infile['hash']))

    run_command(["gg-execute-static", GGInfo.thunk_hash])

    result = GGCache.check(GGInfo.thunk_hash)

    if not result:
        raise Exception("thunk reduction failed")

    executable = is_executable(GGPaths.blob_path(result))

    s3_client.upload_file(GGPaths.blob_path(result), GGInfo.s3_bucket, result)

    s3_client.put_object_acl(
        ACL='public-read',
        Bucket=GGInfo.s3_bucket,
        Key=result
    )

    s3_client.put_object_tagging(
        Bucket=GGInfo.s3_bucket,
        Key=result,
        Tagging={
            'TagSet': [
                { 'Key': 'gg:reduced_from', 'Value': GGInfo.thunk_hash },
                { 'Key': 'gg:executable', 'Value': 'true' if executable else 'false' }
            ]
        }
    )


    return {
        'thunk_hash': GGInfo.thunk_hash,
        'output_hash': result,
        'output_size': os.path.getsize(GGPaths.blob_path(result)),
        'executable_output': executable
    }
