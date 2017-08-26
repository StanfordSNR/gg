import os
import sys
import asyncio
import aiohttp

from contextlib import closing

CHUNK_SIZE = 16 * 1024 # 16 KB

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
def download_multiple(download_list):
    with aiohttp.ClientSession() as session:
        download_futures = [download_file(session, d['url'], chunk_writer(d['file']))
                            for d in download_list]

        for download_future in asyncio.as_completed(download_futures):
            result = yield from download_future

def download_files(download_list):
    try:
        for d in download_list:
            d['file'] = open(d['filename'], "wb")

        with closing(asyncio.new_event_loop()) as loop:
            loop.run_until_complete(download_multiple(download_list))
    finally:
        for d in download_list:
            d['file'].close()
