#!/usr/bin/env python3

import os

with open("config") as fin:
    for line in fin:
        line = line.strip()
        line = line.split('=')
        os.environ[line[0]] = line[1]

os.environ['GG_DIR'] = os.environ['SERVER_GG_DIR']
os.environ['GG_RUNNER'] = '1'

import cgi
import sys
import time
import json
import base64

import function as lambdafunc
from ggpaths import GGPaths, GG_DIR, make_gg_dirs

request_data = sys.stdin.read()
event = json.loads(request_data)

try:
    output = lambdafunc.handler(event, {})
except Exception as ex:
    output = {
        'errorType': ex.__class__.__name__,
        'message': str(ex)
    }

output_data = json.dumps(output)
sys.stdout.write('Content-Length: {}\r\n'.format(len(output_data)))
sys.stdout.write('\r\n')

sys.stdout.write(output_data)
