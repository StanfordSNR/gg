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

# Now we can import gg stuff...
from ggpaths import GGPaths, GGCache
from common import is_executable, make_executable, run_command

def handler(event, context):
    # XXX incomplete
    coordinator_address = event['coordinator']
    pass
