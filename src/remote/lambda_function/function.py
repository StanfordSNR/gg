#!/usr/bin/env python3.6

import os
import sys

curdir = os.path.dirname(__file__)
sys.path.append(curdir)
sys.path.append(os.path.join(curdir, 'packages'))
os.environ['PATH'] = "{}:{}".format(curdir, os.environ.get('PATH', ''))

if not os.environ.get('GG_DIR'):
    os.environ['GG_DIR'] = "/tmp/_gg"

GG_DIR = os.environ['GG_DIR']
GG_RUNNER = 'GG_RUNNER' in os.environ

import stat
import subprocess as sub
import shutil
import asyncio
import boto3
import time
import socket

from base64 import b64decode

from ggpaths import GGPaths, GGCache
from common import is_executable, make_executable, run_command

class GGInfo:
    def __init__(self):
        self.s3_bucket = None
        self.s3_region = None
        self.thunk_hash = None
        self.infiles = []

def fetch_dependencies(gginfo, infiles, cleanup_first=True):
    download_list = []

    blob_path = GGPaths.blobs
    infile_hashes = {x['hash'] for x in infiles}
    infile_hashes.add(gginfo.thunk_hash)

    if cleanup_first:
        # remove temp execution directories
        os.system("rm -rf /tmp/thunk-execute.*")

        for x in os.listdir(blob_path):
            if x not in infile_hashes:
                os.remove(os.path.join(blob_path, x))

    for infile in infiles:
        bpath = GGPaths.blob_path(infile['hash'])
        if os.path.exists(bpath) and os.path.getsize(bpath) == infile['size']:
            continue

        download_list += [infile['hash']]

    s3_ip = socket.gethostbyname('{}.s3.amazonaws.com'.format(gginfo.s3_bucket))
    p = sub.Popen(['gg-s3-download', gginfo.s3_region, gginfo.s3_bucket, s3_ip], stdout=sub.PIPE, stdin=sub.PIPE, stderr=sub.PIPE)
    out, err = p.communicate(input="\n".join(download_list).encode('ascii'))

    if p.returncode != 0:
        return False

    return True

class TimeLog:
    def __init__(self, enabled=True):
        self.enabled = enabled
        self.start = time.time()
        self.prev = self.start
        self.points = []

    def add_point(self, title):
        if not self.enabled:
            return

        now = time.time()
        self.points += [(title, now - self.prev)]
        self.prev = now

def handler(event, context):
    gginfo = GGInfo()

    gginfo.thunk_hash = event['thunk_hash']
    gginfo.s3_bucket = event['s3_bucket']
    gginfo.s3_region = event['s3_region']
    gginfo.infiles = event['infiles']

    enable_timelog = event.get('timelog', False)
    timelogger = TimeLog(enabled=enable_timelog)

    thunk_data = b64decode(event['thunk_data'])

    with open(GGPaths.blob_path(gginfo.thunk_hash), "wb") as fout:
        fout.write(thunk_data)

    timelogger.add_point("write thunk to disk")

    executables_dir = os.path.join(curdir, 'executables')

    if os.path.exists(executables_dir):
        for exe in os.listdir(executables_dir):
            blob_path = GGPaths.blob_path(exe)
            exe_path = os.path.join(executables_dir, exe)

            if not os.path.exists(blob_path):
                shutil.copy(exe_path, blob_path)
                make_executable(blob_path)

    timelogger.add_point("copy executables to ggdir")

    # only clean up the gg directory if running on Lambda.
    if not fetch_dependencies(gginfo, gginfo.infiles, not GG_RUNNER):
        return {
            'errorType': 'GG-FetchDependenciesFailed'
        }

    for infile in gginfo.infiles:
        if infile['executable']:
            make_executable(GGPaths.blob_path(infile['hash']))

    timelogger.add_point("fetching the dependencies")

    return_code, output = run_command(["gg-execute-static", gginfo.thunk_hash])

    if return_code:
        return {
            'output': output,
            'errorType': 'GG-ExecutionFailed',
        }

    timelogger.add_point("gg-execute")

    result = GGCache.check(gginfo.thunk_hash)

    if not result:
        return {
            'output': output,
            'errorType': 'GG-ExecutionFailed'
        }

    executable = is_executable(GGPaths.blob_path(result))

    timelogger.add_point("check the outfile")

    s3_client = boto3.client('s3')
    s3_client.upload_file(GGPaths.blob_path(result), gginfo.s3_bucket, result)

    s3_client.put_object_acl(
        ACL='public-read',
        Bucket=gginfo.s3_bucket,
        Key=result
    )

    s3_client.put_object_tagging(
        Bucket=gginfo.s3_bucket,
        Key=result,
        Tagging={
            'TagSet': [
                { 'Key': 'gg:reduced_from', 'Value': gginfo.thunk_hash },
                { 'Key': 'gg:executable', 'Value': 'true' if executable else 'false' }
            ]
        }
    )

    timelogger.add_point("upload outfile to s3")

    if enable_timelog:
        s3_client.put_object(
            ACL='public-read',
            Bucket=gginfo.s3_bucket,
            Key="runlogs/{}".format(gginfo.thunk_hash),
            Body=str({'output_hash': result,
                      'started': timelogger.start,
                      'timelog': timelogger.points}).encode('utf-8')
        )

    return {
        'thunk_hash': gginfo.thunk_hash,
        'output_hash': result,
        'output_size': os.path.getsize(GGPaths.blob_path(result)),
        'executable_output': executable
    }
