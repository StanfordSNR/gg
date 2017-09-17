#!/usr/bin/env python3

import http.server
import socketserver
import time

from lambda_function import handler

class GGRequestHandler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        request_data = self.rfile.read(self.headers.getheader('content-length'))
        event = json.loads(input_data)

        try:
            output = handler(event, {})
        except Exception as ex:
            output = {
                'errorType': ex.__class__.__name__,
                'message': ex.message
            }

        output_data = json.dumps(output).encode('utf-8')
        self.send_response(200)
        self.send_header('Content-Length', len(output_data))
        self.end_headers()

        self.wfile.write(output_data)

class GGServer(socketserver.ForkingMixIn, http.server.HTTPServer):
    pass

def usage(argv0):
    print("{} PORT".format(argv0))

def main(port):
    gg_server = GGServer(('', ), GGRequestHandler)
    gg_server.serve_forever()

if __name__ == '__main__':
    if len(sys.argv) == 0:
        os.abort()

    if len(sys.argv) != 2:
        usage(sys.argv[0])
        sys.exit(1)

    main()
