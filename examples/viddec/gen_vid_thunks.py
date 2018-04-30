#! /usr/bin/python

import sys
import os
import stat
import glob
import argparse
import subprocess as sp
from timeit import default_timer as now

SUFFIX_TO_CLEAR = ['jpg', 'out']

out_script = 'gen_thunks.sh'

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--video', '-v', type=str, required=False,
            dest='vidToProcess', default='all_vid.txt', help='file with all vid hashes')
    parser.add_argument('--libin', '-l', type=str, required=False,
            dest='listaticbin', default='li25-static', help='Label Image Binary (li25-static or li-static)')
    return parser.parse_args()

def get_hash(filename):
  hash_proc = sp.Popen(['gg-hash', filename], stdout=sp.PIPE)
  hash_out = hash_proc.communicate()[0].strip().decode('utf-8')
  if hash_out == '':
    print("Error getting hash of %s. Now exiting..." % filename)
    sys.exit(1)

  return hash_out

def collect_files(all_files):
  cf_cmd = ['gg-collect'] + all_files
  cf_proc = sp.Popen(cf_cmd, stdout=sp.PIPE, stderr=sp.PIPE)
  cf_out = cf_proc.communicate()[1].strip().decode('utf-8')
  if cf_out == '':
    print("Error calling gg-collect...")
    sys.exit(1)


def clear_chunks():
    to_delete = []
    for stc in SUFFIX_TO_CLEAR:
        to_delete.extend(glob.glob('*.' + stc))

    for todel in to_delete:
        if os.path.exists(todel):
            os.remove(todel)

def main(args):
    all_hashes = args.vidToProcess
    listatic = args.listaticbin
    all_vid_raw = open(all_hashes, 'r').readlines()

    # Get hashes of all files
    ffmpeg_hash = get_hash('ffmpeg')
    listatic_hash = get_hash(listatic)
    incept_hash = get_hash('inception_v3_2016_08_28_frozen.pb')
    inet_hash = get_hash('imagenet_slim_labels.txt')

    # Collect all files
    all_files_to_col = ['ffmpeg', listatic, 'inception_v3_2016_08_28_frozen.pb', \
                       'imagenet_slim_labels.txt']
    collect_files(all_files_to_col)

    # Build command
    cmd = []
    if listatic == 'li-static':
      cmd.append('./gen_vid_thunks')
    else:
      cmd.append('./gen_vid_thunks25')

    cmd.extend([ffmpeg_hash, listatic_hash, incept_hash, inet_hash])

    all_chunks = 60*(all_vid_raw[4:8])
    for ac in all_chunks:
        cmd.append(ac.strip())

    # Create output script
    fd = open(out_script, 'w')
    fd.write('#!/bin/bash -e\n')
    fd.write(' '.join(cmd))
    fd.close()

    # Make executable
    st = os.stat(out_script)
    os.chmod(out_script, st.st_mode | stat.S_IEXEC)

if __name__ == '__main__':
    clear_chunks()
    parsed_args = get_args()
    main(parsed_args)

