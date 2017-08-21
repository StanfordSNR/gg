#!/usr/bin/env python3

import os
import sys
from zipfile import ZipFile
import tempfile
import shutil
import argparse
import hashlib
import boto3

BASE_FILE = "lambda/lambda_function_packages.zip"
PACKAGE_GG_DIR = "_gg"
GG_EXECUTE_STATIC = shutil.which("gg-execute-static")

if not GG_EXECUTE_STATIC:
    raise Exception("Cannot find gg-execute-static")

PACKAGE_FILES = {
    "gg-execute-static": GG_EXECUTE_STATIC,
    "infile.js": "lambda/infile.js",
    "function.js": "lambda/function.js",
    "thunk.js": "lambda/thunk.js",
    "lambda_function.js": "lambda/lambda_function.js"
}

def sha256_checksum(filename, block_size=65536):
    sha256 = hashlib.sha256()

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            sha256.update(block)

    return sha256.hexdigest()

def create_lambda_package(toolchain_path, output):
    shutil.copy(BASE_FILE, output)

    with ZipFile(output, 'w') as funczip:
        for fn, fp in PACKAGE_FILES.items():
            funczip.write(fp, fn)

        for exe in os.listdir(toolchain_path):
            exe_path = os.path.join(toolchain_path, exe)
            exe_hash = sha256_checksum(exe_path)
            funczip.write(exe_path, os.path.join(PACKAGE_GG_DIR, exe_hash))

def install_lambda_package(package_file, function_name, role, s3_bucket):
    with open(package_file, 'rb') as pfin:
        package_data = pfin.read()

    client = boto3.client('lambda')
    client.create_function(
        FunctionName=function_name,
        Runtime='python3.6',
        Role='lambdarole',
        Handler='lambda_function.handler',
        Code={
            'ZipFile': package_data,
            'S3Bucket': s3_bucket,
            'S3Key': function_name
        },
        Timeout=300,
        MemorySize=1536
    )

def main():
    parser = argparse.ArgumentParser(description="Generate and install Lambda function.")
    parser.add_argument('--install', dest='install', action='store_true', default=False)
    parser.add_argument('--function-file', dest='function_file', action='store', default="ggfunction.zip")
    parser.add_argument('--toolchain-path', dest='toolchain_path', action='store')
    parser.add_argument('--function-name', dest='function_name', action='store', default="ggfunction")
    parser.add_argument('--role', dest='role', action='store')
    parser.add_argument('--s3-bucket', dest='s3_bucket', action='store')

    args = parser.parse_args()
    if not args.install:
        create_lambda_package(args.toolchain_path, args.function_file)
    else:
        if not args.function_name or not args.role or not args.s3_bucket:
            raise Exception("Please provide function name, role and S3 bucket.")

        install_lambda_package(args.function_file, args.function_name, args.role, args.s3_bucket)

if __name__ == '__main__':
    main()
