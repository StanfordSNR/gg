#!/usr/bin/env python3.6

import os
import sys

curdir = os.path.dirname(__file__)
sys.path.append(os.path.join(curdir, 'packages'))
os.environ['PATH'] = "{}:{}".format(curdir, os.environ.get('PATH', ''))

import stat
import subprocess as sub
import shutil
import asyncio
import aiohttp
import boto3

from contextlib import closing

CHUNK_SIZE = 32 * 1024 # 32 KB
s3_client = boto3.client('s3')

class GGInfo:
    root = "/tmp/_gg"
    s3_bucket = None
    thunk_hash = None
    infiles = []

def blob_path(blob_hash):
    return os.path.join(GGInfo.root, blob_hash)

def reduction_path(blob_hash):
    return os.path.join(GGInfo.root, "reductions", blob_hash)

def check_reduction_cache(blob_hash):
    rpath = reduction_path(blob_hash)

    if not os.path.islink(rpath):
        return None

    return os.readlink(rpath)

def blob_url(blob_hash):
    return "https://{bucket}.s3.amazonaws.com/{hash}".format(bucket=GGInfo.s3_bucket, hash=blob_hash)

def coroutine(func):
    def start(*args, **kwargs):
        cr = func(*args, **kwargs)
        next(cr)
        return cr
    return start

@coroutine
def chunk_writer(target_file):
    while True:
        chunk = yield
        assert chunk
        target_file.write(chunk)

async def download_file(session: aiohttp.ClientSession, url: str, sink):
    async with session.get(url) as response:
        if response.status != 200:
            raise Exception("Download failed: {}".format(url))

        while True:
            chunk = await response.content.read(CHUNK_SIZE)
            if not chunk:
                break
            sink.send(chunk)

@asyncio.coroutine
def download_multiple(download_data):
    with aiohttp.ClientSession() as session:
        download_futures = [download_file(session, d['url'], chunk_writer(d['file']))
                            for d in download_data]

        for download_future in asyncio.as_completed(download_futures):
            result = yield from download_future

def fetch_dependencies(infiles):
    try:
        download_data = []

        for infile in infiles:
            bpath = blob_path(infile['hash'])
            if os.path.exists(bpath) and os.path.getsize(bpath) == infile['size']:
                continue

            download_data += [{
                'url': blob_url(infile['hash']),
                'file': open(bpath, "wb")
            }]

        with closing(asyncio.new_event_loop()) as loop:
           loop.run_until_complete(download_multiple(download_data))
    finally:
        for d in download_data:
            d['file'].close()

def make_executable(path):
    st = os.stat(path)
    os.chmod(path, st.st_mode | stat.S_IEXEC)

def is_executable(path):
    st = os.stat(path)
    return ( st.st_mode & stat.S_IEXEC ) != 0

def run_command(command):
    res_code = sub.run(command).returncode

    if res_code:
        raise Exception("command failed: {}".format(" ".join(command)))

def handler(event, context):
    GGInfo.thunk_hash = event['thunk_hash']
    GGInfo.s3_bucket = event['s3_bucket']
    GGInfo.infiles = event['infiles']

    #shutil.rmtree(GGInfo.root, ignore_errors=True)
    os.makedirs(GGInfo.root, exist_ok=True)
    os.environ['GG_DIR'] = GGInfo.root

    fetch_dependencies(GGInfo.infiles)

    for infile in GGInfo.infiles:
        if infile['executable']:
            make_executable(blob_path(infile['hash']))

    run_command(["gg-execute-static", GGInfo.thunk_hash])

    result = check_reduction_cache(GGInfo.thunk_hash)

    if not result:
        raise Exception("thunk reduction failed")

    s3_client.upload_file(blob_path(result), GGInfo.s3_bucket, result)
    s3_client.put_object_acl(ACL='public-read', Bucket=GGInfo.s3_bucket, Key=result)


    return {
        'thunk_hash': GGInfo.thunk_hash,
        'output_hash': result,
        'output_size': os.path.getsize(blob_path(result)),
        'executable_output': is_executable(blob_path(result))
    }
