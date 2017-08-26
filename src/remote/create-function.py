#!/usr/bin/env python3

import os
import sys
from zipfile import ZipFile
import tempfile
import shutil
import argparse
import hashlib
import base64
import boto3

BASE_FILE = "lambda_function/packages.zip"
PACKAGE_GG_DIR = "_gg"
GG_EXECUTE_STATIC = shutil.which("gg-execute-static")

if not GG_EXECUTE_STATIC:
    raise Exception("Cannot find gg-execute-static")

PACKAGE_FILES = {
    "gg-execute-static": GG_EXECUTE_STATIC,
    "function.py": "lambda_function/function.py",
    "ggpaths.py": "ggpaths.py",
    "downloader.py": "downloader.py",
    "common.py": "common.py"
}

def sha256_checksum(filename, block_size=65536):
    sha256 = hashlib.sha256()

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            sha256.update(block)

    return base64.urlsafe_b64encode(sha256.digest())

def create_lambda_package(output):
    shutil.copy(BASE_FILE, output)

    with ZipFile(output, 'a') as funczip:
        for fn, fp in PACKAGE_FILES.items():
            funczip.write(fp, fn)

def install_lambda_package(package_file, function_name, role, region, delete=False):
    with open(package_file, 'rb') as pfin:
        package_data = pfin.read()

    client = boto3.client('lambda', region_name=region)

    if delete:
        try:
            client.delete_function(FunctionName=function_name)
        except:
            pass

    response = client.create_function(
        FunctionName=function_name,
        Runtime='python3.6',
        Role=role,
        Handler='function.handler',
        Code={
            'ZipFile': package_data
        },
        Timeout=300,
        MemorySize=1536,
        Tags={
            'gg': 'generic',
        }
    )

    print(response['FunctionArn'])

def main():
    parser = argparse.ArgumentParser(description="Generate and install Lambda function.")
    parser.add_argument('--install', dest='install', action='store_true', default=False)
    parser.add_argument('--delete', dest='delete', action='store_true', default=False)
    parser.add_argument('--function-file', dest='function_file', action='store', default="ggfunction.zip")
    parser.add_argument('--function-name', dest='function_name', action='store', default="ggfunction")
    parser.add_argument('--role', dest='role', action='store')
    parser.add_argument('--region', dest='region', default='us-west-2', action='store')

    args = parser.parse_args()
    if not args.install:
        create_lambda_package(args.function_file)
    else:
        if not args.function_name or not args.role:
            raise Exception("Please provide function name, role.")

        install_lambda_package(args.function_file, args.function_name, args.role, args.region, delete=args.delete)

if __name__ == '__main__':
    main()
