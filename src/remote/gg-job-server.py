#!/usr/bin/env python3

import os
import sys
import time
import json
import http.server
import socketserver

import lambda_function.function

class GGRequestHandler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        request_data = self.rfile.read(int(self.headers['content-length']))
        event = json.loads(request_data.decode('utf-8'))

        print(event['thunk_hash'])

        try:
            output = lambda_function.function.handler(event, {})
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
    print("{} PORT".format(argv0))

def main(port):
    gg_server = GGServer(('', port), GGRequestHandler)
    gg_server.serve_forever()

if __name__ == '__main__':
    if len(sys.argv) == 0:
        os.abort()

    if len(sys.argv) != 2:
        usage(sys.argv[0])
        sys.exit(1)

    main(int(sys.argv[1]))
