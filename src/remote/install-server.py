#!/usr/bin/env python3

import os
import sys
import time
import base64
import hashlib
import shutil

SERVER_GG_DIR = '/var/www/_gg'
os.environ['GG_DIR'] = SERVER_GG_DIR

from lambda_function.ggpaths import GGPaths, GG_DIR, make_gg_dirs

toolchain_path = None

def gghash(filename, block_size=65536):
    sha256 = hashlib.sha256()
    size = 0

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            size += len(block)
            sha256.update(block)

    return "{}{:08x}".format(base64.urlsafe_b64encode(sha256.digest()).decode('ascii').replace('=','').replace('-', '.'), size)

def prepare_toolchain(tpath):
    for tbin in os.listdir(tpath):
        bin_path = os.path.join(tpath, tbin)
        bin_hash = gghash(bin_path)
        shutil.copy(bin_path, GGPaths.blob_path(bin_hash))

def usage(argv0):
    print("{} CGI-BIN TOOLCHAIN-PATH GG-S3-DOWNLOAD GG-EXECUTE-STATIC".format(argv0))

if __name__ == '__main__':
    if len(sys.argv) == 0:
        os.abort()

    if len(sys.argv) != 5:
        usage(sys.argv[0])
        sys.exit(1)

    cgi_bin_path = sys.argv[1]
    toolchain_path = sys.argv[2]
    downloader_path = sys.argv[3]
    gg_execute_path = sys.argv[4]

    print('Preparing toolchain... ', end='')
    prepare_toolchain(toolchain_path)
    print('done.')

    lambda_root = os.path.join(cgi_bin_path, 'gg')

    print('Copying Lambda package to cgi-bin... ', end='')
    shutil.copytree('lambda_function/', lambda_root)
    print('done.')

    print('Copying gg-s3-download and gg-execute binaries... ', end='')
    shutil.copy(downloader_path, os.path.join(lambda_root, 'gg-s3-download'))
    shutil.copy(gg_execute_path, os.path.join(lambda_root, 'gg-execute-static'))
    print('done.')

    print('Copy cgi scripts... ', end='')
    shutil.copy('cgi-scripts/ggexecute.cgi', os.path.join(lambda_root, 'execute.cgi'))
    shutil.copy('cgi-scripts/ggreset.cgi',   os.path.join(lambda_root, 'reset.cgi'))
    print('done.')

    print('Fixing up permissions for GG_DIR... ', end='')
    os.system('chown -R www-data:www-data {}'.format(SERVER_GG_DIR))
    print('done.')

    print('Creating environment file... ', end='')
    with open(os.path.join(lambda_root, 'config'), "w") as fout:
        for key in ['AWS_SECRET_ACCESS_KEY', 'AWS_ACCESS_KEY_ID']:
            fout.write('%s=%s\n' % (key, os.environ[key]))
        fout.write('TOOLCHAIN_PATH=%s' % toolchain_path)
    print('done.')
