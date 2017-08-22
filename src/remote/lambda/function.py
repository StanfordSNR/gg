#!/usr/bin/env python3.6

import os
import sys

curdir = os.path.dirname(__file__)
sys.path.append(os.path.join(curdir, 'packages'))
os.environ['PATH'] = "{}:{}".format(curdir, os.environ.get('PATH', ''))

import stat
import subprocess as sub
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
        if not chunk:
            target_file.close()
        target_file.write(chunk)

async def download_file(session: aiohttp.ClientSession, url: str, sink):
    async with session.get(url) as response:
        assert response.status == 200

        while True:
            chunk = await response.content.read(CHUNK_SIZE)
            sink.send(chunk)
            if not chunk:
                break

@asyncio.coroutine
def download_multiple(session: aiohttp.ClientSession, download_data):
    download_futures = [download_file(session, d['url'], chunk_writer(d['file'])
                        for d in download_data]

    for download_future in asyncio.as_completed(download_futures):
        result = yield from download_future

def fetch_dependencies(dep_hashes):
    try:
        download_data = []

        for dep_hash in dep_hashes:
            if os.path.exists(blob_path(dep_hash)):
                pass

            download_data += [{
                'url': blob_url(dep_hash),
                'file': open(blob_path(dep_hash), "wb")
            }]

        with closing(asyncio.get_event_loop()) as loop:
           with aiohttp.ClientSession() as session:
               loop.run_until_complete(download_multiple(session, download_data))
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
    res_code = sub.call(command)

    if res_code:
        raise Exception("command failed: {}".format(" ".join(command)))

def handler(event, context):
    GGInfo.thunk_hash = event['thunk_hash']
    GGInfo.s3_bucket = event['s3_bucket']
    GGInfo.infiles = event['infiles']

    os.mkdirs(path=GGInfo.root)
    os.environ['GG_DIR'] = GGInfo.root

    download_list = [{'hash': GGInfo.thunk_hash, 'executable': false}]
    download_list += [i['hash'] for i in GGInfo.infiles]

    fetch_dependencies(download_list)

    for infile in GGInfo.infiles:
        if infile['executable']:
            make_executable(blob_path(infile['hash']))

    run_command(["gg-execute-static", GGInfo.thunk_hash])

    result = check_reduction_cache(GGInfo.thunk_hash)

    if not result:
        raise Exception("thunk reduction failed")

    s3_client.upload_file(blob_path(result), GGInfo.s3_bucket, result)

    return {
        'thunk_hash': GGInfo.thunk_hash,
        'output_hash': result,
        'executable_output': is_executable(blob_hash(result))
    }
