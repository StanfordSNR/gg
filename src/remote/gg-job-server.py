#!/usr/bin/env python3

import os
os.environ['GG_RUNNER'] = '1'

import sys
import time
import json
import http.server
import socketserver
import hashlib
import base64
import shutil

import lambda_function.function as lambdafunc
from lambda_function.ggpaths import GGPaths

def gghash(filename, block_size=65536):
    sha256 = hashlib.sha256()
    size = 0

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            size += len(block)
            sha256.update(block)

    return "{}{:08x}".format(base64.urlsafe_b64encode(sha256.digest()).decode('ascii').replace('=','').replace('-', '.'), size)

class GGRequestHandler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        request_data = self.rfile.read(int(self.headers['content-length']))
        event = json.loads(request_data.decode('utf-8'))

        print(event['thunk_hash'])

        try:
            output = lambdafunc.handler(event, {})
        except Exception as ex:
            output = {
                'errorType': ex.__class__.__name__,
                'message': str(ex)
            }

        output_data = json.dumps(output).encode('utf-8')
        self.send_response(200)
        self.send_header('Content-Length', len(output_data))
        self.end_headers()

        print(output_data)

        self.wfile.write(output_data)

class GGServer(socketserver.ForkingMixIn, http.server.HTTPServer):
    pass

def usage(argv0):
    print("{} PORT TOOLCHAIN-PATH".format(argv0))

def main(port):
    print('Starting server, listening on :{}'.format(port))
    gg_server = GGServer(('', port), GGRequestHandler)
    gg_server.serve_forever()

def prepare_toolchain(toolchain_path):
    for tbin in os.listdir(toolchain_path):
        bin_path = os.path.join(toolchain_path, tbin)
        bin_hash = gghash(bin_path)
        shutil.copy(bin_path, GGPaths.blob_path(bin_hash))

if __name__ == '__main__':
    if len(sys.argv) == 0:
        os.abort()

    if len(sys.argv) != 3:
        usage(sys.argv[0])
        sys.exit(1)

    print('Preparing toolchain... ', end='')
    prepare_toolchain(sys.argv[2])
    print('done.')

    main(int(sys.argv[1]))
