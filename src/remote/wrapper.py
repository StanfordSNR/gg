#!/usr/bin/env python3

from lambda_function import handler

import json

def main():
    input_data = sys.stdin.read()
    event = json.loads(input_data)

    try:
        output = handler(event, {})
    except Exception as ex:
        output = {
            'errorType': ex.__class__.__name__,
            'message': ex.message
        }

    sys.stdout.write(json.dumps(output))
    return 0

if __name__ == '__main__':
    sys.exit(main())
