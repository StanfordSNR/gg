#!/usr/bin/env python3.6

import hmac
import hashlib
import datetime
import boto3
import requests

from base64 import b64encode
from email.utils import formatdate

def sign(key, msg):
    return b64encode(hmac.new(key, msg.encode('utf-8'), hashlib.sha1).digest()).decode('ascii')

class S3AuthGeneator:
    def __init__(self, aws_access_key, aws_secret_access_key):
        self.aws_access_key = aws_access_key.encode('ascii')
        self.aws_secret_access_key = aws_secret_access_key.encode('ascii')
        self.service_name = 's3'

    def __call__(self, method, bucket_name, object_name, headers):
        current_time = datetime.datetime.utcnow()
        timestamp = formatdate(timeval=None, localtime=False, usegmt=True)

        canonical_uri = '/{bucket_name}/{object_name}'.format(
            bucket_name=bucket_name, object_name=object_name)

        string_to_sign = method + '\n' + \
            '\n' + \
            'application/octet-stream' + '\n' + \
            timestamp + '\n'

        candidate_headers = []
        for hk, hv in headers.items():
            if hk.lower().startswith("x-amz-"):
                candidate_headers += [hk]
        candidate_headers.sort()

        for hk in candidate_headers:
            string_to_sign += hk.lower() + ':' + headers[hk] + '\n'

        string_to_sign += canonical_uri

        headers['Content-Type'] = 'application/octet-stream'
        headers['Date'] = timestamp
        headers['Authorization'] = "AWS {}:{}".format(self.aws_access_key.decode('ascii'),
            sign(self.aws_secret_access_key, string_to_sign))

def get_object_url(bucket_name, object_name):
    return 'https://{}.s3.amazonaws.com/{}'.format(bucket_name, object_name)

if __name__ == '__main__':
    data = b'random data'
    creds = boto3.Session().get_credentials()
    auth_gen = S3AuthGeneator(creds.access_key, creds.secret_key)

    bucket_name = 'gg-us-west-2'
    object_name = 'test'
    url = get_object_url(bucket_name, object_name)

    headers = {
        'x-amz-acl': 'public-read',
        'x-amz-tagging': 'gg=file'
    }

    auth_gen('PUT', bucket_name, object_name, headers)
    response = requests.put(url, data=data, headers=headers)
    print(response.status_code)
