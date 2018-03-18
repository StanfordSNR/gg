#!/usr/bin/env python3.6

import os
import sys
import time
import shutil
import subprocess as sub
from base64 import b64decode

# Set up environment variables necessary
curdir = os.path.dirname(__file__)
sys.path.append(curdir)
sys.path.append(os.path.join(curdir, 'packages'))
os.environ['PATH'] = "{}:{}".format(curdir, os.environ.get('PATH', ''))

if not os.environ.get('GG_DIR'):
    os.environ['GG_DIR'] = "/tmp/_gg"

# Now we can import gg stuff...
from ggpaths import GGPaths, GGCache
from common import is_executable, make_executable, run_command

def handler(event, context):
    os.environ['GG_STORAGE_URI'] = event['storageBackend']
    thunks = event['thunks']

    # Write thunks to disk
    for thunk_item in thunks:
        thunk_data = b64decode(thunk_item['data'])
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

    # Remove old thunk-execute directories
    os.system("rm -rf /tmp/thunk-execute.*")

    # Execute the thunk, and upload the result
    return_code, output = run_command(["gg-execute-static",
         "--get-dependencies", "--put-output", "--cleanup",] +
         [x['hash'] for x in thunks])

    result_hashes = []

    for thunk_item in thunks:
        result = GGCache.check(thunk_item['hash'])
        result_hashes += [result]

    if return_code or (None in result_hashes):
        return {
            'returnCode': return_code,
            'output': output
        }

    executed_thunks = []

    for thunk in thunks:
        outputs = []

        for output_tag in thunk['outputs']:
            output_hash = GGPaths.check(thunk['hash'], output)

            if not output_hash:
                # this shouldn't happen, gg-execute actually check for this
                # XXX Where do we go now? Where do we go now?
                pass

            outputs += [{
                'hash': result_hashes[i],
                'size': os.path.getsize(GGPaths.blob_path(output_hash)),
                'executable': is_executable(GGPaths.blob_path(output_hash))
            }]

        executed_thunks += [{
            'thunkHash': thunks[i]['hash'],
            'outputs': outputs
        }]

    return {
        'returnCode': 0,
        'output': '',
        'executedThunks': executed_thunks
    }
