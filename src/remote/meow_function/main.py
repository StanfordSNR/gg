#!/usr/bin/env python3.6

import os
import sys
import time
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
from common import run_command

def handler(event, context):
    os.environ['GG_STORAGE_URI'] = event['storageBackend']
    coordinator_address = event['coordinator']
    coordinator_host, coordinator_port = event['coordinator'].split(':')

    if event.get('timelog'):
        os.environ['GG_EXECUTE_TIMELOG'] = '1'

    return_code, stdout = run_command(["gg-meow-worker",
        coordinator_host, coordinator_port])

    print(stdout)
    print(return_code)

    return {
        'returnCode': return_code,
        'stdout': stdout,
    }
