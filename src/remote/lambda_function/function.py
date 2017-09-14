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
import time

from base64 import b64decode

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

    blob_path = GGPaths.blobs
    infile_hashes = {x['hash'] for x in infiles}
    infile_hashes.add(GGInfo.thunk_hash)

    for x in os.listdir(blob_path):
        if x not in infile_hashes:
            os.remove(os.path.join(blob_path, x))

    for infile in infiles:
        bpath = GGPaths.blob_path(infile['hash'])
        if os.path.exists(bpath) and os.path.getsize(bpath) == infile['size']:
            continue

        download_list += [{
            'url': GGPaths.object_url(GGInfo.s3_bucket, infile['hash']),
            'filename': bpath,
        }]

    download_files(download_list)

EXECUTABLES_DIR = os.path.join(curdir, 'executables')

class TimeLog:
    def __init__(self, enabled=True):
        self.enabled = enabled
        self.start = time.time()
        self.points = []

    def add_point(self, title):
        if not self.enabled:
            return

        now = time.time()
        self.points += [(title, now - self.start)]
        self.start = now

def handler(event, context):
    GGInfo.thunk_hash = event['thunk_hash']
    GGInfo.s3_bucket = event['s3_bucket']
    GGInfo.infiles = event['infiles']

    enable_timelog = event.get('timelog', False)
    timelogger = TimeLog(enabled=enable_timelog)

    thunk_data = b64decode(event['thunk_data'])

    with open(GGPaths.blob_path(GGInfo.thunk_hash), "wb") as fout:
        fout.write(thunk_data)

    timelogger.add_point("write thunk to disk")

    if os.path.exists(EXECUTABLES_DIR):
        for exe in os.listdir(EXECUTABLES_DIR):
            blob_path = GGPaths.blob_path(exe)
            exe_path = os.path.join(EXECUTABLES_DIR, exe)

            if not os.path.exists(blob_path):
                shutil.copy(exe_path, blob_path)
                make_executable(blob_path)

    timelogger.add_point("copy executables to ggdir")

    fetch_dependencies(GGInfo.infiles)

    for infile in GGInfo.infiles:
        if infile['executable']:
            make_executable(GGPaths.blob_path(infile['hash']))

    timelogger.add_point("fetching the dependencies")

    return_code, output = run_command(["gg-execute-static", GGInfo.thunk_hash])

    if return_code:
        return {
            'errorType': 'GG-ExecutionFailed'
        }

    timelogger.add_point("gg-execute")

    result = GGCache.check(GGInfo.thunk_hash)

    if not result:
        return {
            'errorType': 'GG-ExecutionFailed'
        }

    executable = is_executable(GGPaths.blob_path(result))

    timelogger.add_point("check the outfile")

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

    timelogger.add_point("upload outfile to s3")

    if enable_timelog:
        s3_client.put_object(
            ACL='public-read',
            Bucket=GGInfo.s3_bucket,
            Key="runlogs/{}".format(GGInfo.thunk_hash),
            Body=str({'output_hash': result, 'timelog': timelogger.points}).encode('utf-8')
        )

    return {
        'thunk_hash': GGInfo.thunk_hash,
        'output_hash': result,
        'output_size': os.path.getsize(GGPaths.blob_path(result)),
        'executable_output': executable
    }
