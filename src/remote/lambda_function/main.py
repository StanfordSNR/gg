#!/usr/bin/env python3.6

import os
import sys
import time
import errno
import shutil
import subprocess as sub
from base64 import b64decode, b64encode

# Set up environment variables necessary
curdir = os.path.dirname(__file__)
sys.path.append(curdir)
sys.path.append(os.path.join(curdir, 'packages'))
os.environ['PATH'] = "{}:{}".format(curdir, os.environ.get('PATH', ''))

if not os.environ.get('GG_DIR'):
    os.environ['GG_DIR'] = "/tmp/_gg"

if not os.environ.get('GG_CACHE_DIR'):
    os.environ['GG_CACHE_DIR'] = "/tmp/_gg/_cache"

# Now we can import gg stuff...
from ggpaths import GGPaths, GGCache, make_gg_dirs
from common import is_executable, make_executable, run_command

def is_hash_for_thunk(hash):
    return len(hash) > 0 and hash[0] == 'T'

def handler(event, context):
    os.environ['GG_STORAGE_URI'] = event['storageBackend']
    thunks = event['thunks']
    timelog = event.get('timelog')

    # Remove old thunk-execute directories
    os.system("rm -rf /tmp/thunk-execute.*")

    # Write thunks to disk

    tried_once = False

    while True:
        try:
            for thunk_item in thunks:
                thunk_data = b64decode(thunk_item['data'])
                if os.path.exists(GGPaths.blob_path(thunk_item['hash'])):
                    os.remove(GGPaths.blob_path(thunk_item['hash']))
                with open(GGPaths.blob_path(thunk_item['hash']), "wb") as fout:
                    fout.write(thunk_data)

            # Move executables from Lambda package to .gg directory
            executables_dir = os.path.join(curdir, 'executables')
            if os.path.exists(executables_dir):
                for exe in os.listdir(executables_dir):
                    blob_path = GGPaths.blob_path(exe)
                    exe_path = os.path.join(executables_dir, exe)

                    if not os.path.exists(blob_path):
                        shutil.copy(exe_path, blob_path)
                        make_executable(blob_path)

            break

        except OSError as ex:
            if not tried_once and ex.errno == errno.ENOSPC:
                # there's no space left; let's get rid of GG_DIR and try again
                tried_once = True
                os.system("rm -rf '{}'".format(GGPaths.blobs))
                os.system("rm -rf '{}'".format(GGPaths.reductions))
                make_gg_dirs()
                continue
            else:
                raise

    # Execute the thunk, and upload the result
    command = ["gg-execute-static",
               "--get-dependencies",
               "--put-output",
               "--cleanup"]

    if timelog:
        command += ["--timelog"]

    return_code, stdout = run_command(command +
         [x['hash'] for x in thunks])

    executed_thunks = []

    for thunk in thunks:
        outputs = []

        for output_tag in thunk['outputs']:
            output_hash = GGCache.check(thunk['hash'], output_tag)

            if not output_hash:
                return {
                    'returnCode': return_code,
                    'stdout': stdout
                }

            data = None
            if is_hash_for_thunk(output_hash):
                with open(GGPaths.blob_path(output_hash), 'rb') as tin:
                    data = b64encode(tin.read()).decode('ascii')

            outputs += [{
                'tag': output_tag,
                'hash': output_hash,
                'size': os.path.getsize(GGPaths.blob_path(output_hash)),
                'executable': is_executable(GGPaths.blob_path(output_hash)),
                'data': data
            }]

        executed_thunks += [{
            'thunkHash': thunk['hash'],
            'outputs': outputs
        }]

    return {
        'returnCode': 0,
        'stdout': '',
        'executedThunks': executed_thunks
    }
