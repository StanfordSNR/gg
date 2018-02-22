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
    thunk_hash = event['thunk_hash']
    os.environ['GG_STORAGE_URI'] = event['storage_backend']

    # Write thunk to disk
    thunk_data = b64decode(event['thunk_data'])
    with open(GGPaths.blob_path(thunk_hash), "wb") as fout:
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
         "--get-dependencies", "--put-output", "--cleanup", thunk_hash])

    result = GGCache.check(thunk_hash)

    if return_code or not result:
        return {
            'returnCode': return_code,
            'output': output
        }

    executable = is_executable(GGPaths.blob_path(result))

    return {
        'thunk_hash': thunk_hash,
        'output_hash': result,
        'output_size': os.path.getsize(GGPaths.blob_path(result)),
        'executable_output': executable
    }
