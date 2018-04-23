#! /usr/bin/python

import sys
import os
import stat
import glob
import argparse
import numpy as np
import subprocess as sp
from timeit import default_timer as now
from gg_sdk import GG

CMD = "ffmpeg -loglevel panic -i {video} -frames:v {numout} frameout%03d_{ofile}.jpg"
CMD_IMREC = "li-static {myimage} {incep_hash} {inet_hash} {myoutput}"
SUFFIX_TO_CLEAR = ['jpg', 'out']

num_out = 25

out_script = 'gen_thunks.sh'

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--video', '-v', type=str, required=False,
            dest='vidToProcess', default='4kvid', help='Video to process (Def: 4kvid)')
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

def get_dur_fps(myvid):
    cmd = 'ffmpeg -i %s 2>&1 | grep -e "fps" -e "Duration"' % myvid
    process = sp.Popen(cmd, shell=True, stdout=sp.PIPE, stderr=sp.PIPE)
    out, _ = process.communicate()
    out_split = out.decode('utf-8').split()
    duration = out_split[out_split.index('Duration:') + 1].strip(',')
    duration_split = duration.split(':')
    tot_seconds = float(duration_split[0])*3600.0 + float(duration_split[1])*60.0 + float(duration_split[2])

    print("Total duration: %s (%.3f seconds)" % (duration, tot_seconds))
    return tot_seconds

def main(args):
    vidStart = args.vidToProcess
    all_vid_raw = glob.glob(vidStart + '_chunk*')[21:25]

    # Initialize gg-dir
    GG()

    # Get hashes of all files
    ffmpeg_hash = get_hash('ffmpeg')
    listatic_hash = get_hash('li-static')
    incept_hash = get_hash('inception_v3_2016_08_28_frozen.pb')
    inet_hash = get_hash('imagenet_slim_labels.txt')

    # Collect all files
    all_files_to_col = ['ffmpeg', 'li-static', 'inception_v3_2016_08_28_frozen.pb', \
                       'imagenet_slim_labels.txt'] + all_vid_raw
    collect_files(all_files_to_col)

    # Create output script
    fd = open(out_script, 'w')
    fd.write('#!/bin/bash -e\n')

    all_chunks = []

    # Get all durations and hashes of all video files
    all_dur = {}
    for ac in all_vid_raw:
        ts = get_dur_fps(ac)
        if ts > 4:
          next_hash = get_hash(ac)
          all_chunks.append(next_hash)
          all_dur[next_hash] = ts


    start = now()
    all_count = 0
    for i, myvid in enumerate(all_chunks):
        if myvid not in all_dur:
          continue
        vid_dur = all_dur[myvid]
        for vd in np.arange(0, 1, 1): 
          all_outname = []
          out_size = num_out
          if (len(all_chunks)-i) <= 4:
            out_size = 4
          for j in range(out_size):
              all_outname.append('frameout%03d_%03d.jpg' % (j + 1, all_count))

          # Create gg-create-thunk command and add it to the script
          cmd = ['gg-create-thunk']

          # Outputs
          for o in all_outname:
            cmd.extend(['--output', o])

          # Values
          cmd.extend(['--value', myvid])

          # Executable
          cmd.extend(['--executable', ffmpeg_hash])

          cmd.append('--')
          
          # Function hash
          cmd.append(ffmpeg_hash)

          # Function call
          cmd.extend(CMD.format(video='@{GGHASH:'+myvid+'}', 
                numout=out_size, ofile='%03d' % all_count).split())

          # Add to script
          cmd_string = ' '.join(cmd)
          line_to_add = 'hash%d=$((%s) 2>&1)' % (i, cmd_string)
          fd.write(line_to_add + '\n\n')

          for j in range(out_size): 
              pic_out = 'frameout%03d_%03d_lab.out' % (j + 1, all_count)
              all_count += 1

              # Create gg-create-thunk command and add it to the script
              cmd = ['gg-create-thunk']

              # Outputs
              cmd.extend(['--output', pic_out])

              # Values
              cmd.extend(['--value', incept_hash])
              cmd.extend(['--value', inet_hash])

              # Thunk
              cmd.extend(['--thunk', '${hash%d}#%s' % (i, all_outname[j])])

              # Executable
              cmd.extend(['--executable', listatic_hash])

              # Placeholder
              cmd.extend(['--placeholder', pic_out])

              cmd.append('--')
              
              # Function hash
              cmd.append(listatic_hash)

              # Function call
              cmd.extend(CMD_IMREC.format(
                    myimage='@{GGHASH:'+'${hash%d}#%s' % (i, all_outname[j])+'}',
                    incep_hash='@{GGHASH:'+incept_hash+'}',
                    inet_hash='@{GGHASH:'+inet_hash+'}',
                    myoutput=pic_out).split())

              # Add to script
              cmd_string = ' '.join(cmd)
              line_to_add = cmd_string + ' &'
              fd.write(line_to_add + '\n\n')

          all_count += 1

    fd.close()
    # Make executable
    st = os.stat(out_script)
    os.chmod(out_script, st.st_mode | stat.S_IEXEC)

    end = now()
    delta = end - start
    print("Total time to create bash script: %.3f seconds" % delta)

if __name__ == '__main__':
    clear_chunks()
    parsed_args = get_args()
    start = now()
    main(parsed_args)
    end = now()
    delta = end - start
    print("Total runtime: %.3f seconds" % delta)

