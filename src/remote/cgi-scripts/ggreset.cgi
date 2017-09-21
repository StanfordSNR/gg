#!/usr/bin/env python3

import os
os.environ['GG_RUNNER'] = '1'

import cgi
import sys
import time
import json
import base64
import shutil

import function as lambdafunc
from ggpaths import GGPaths, GG_DIR, make_gg_dirs

print('Content-Type: text/plain')
print()
print(GG_DIR)

shutil.rmtree(GG_DIR, ignore_errors=True)
make_gg_dirs()
