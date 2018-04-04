#! /usr/bin/python

import sys
import os
import glob
import numpy as np
import subprocess as sp
from gg_sdk import GG, GGThunk

CMD = "ffmpeg -loglevel panic -i {video} -ss 00:{min}:{sec} -frames:v 1 {ofile}"
CMD_IMREC = "li-static {myimage} inception_v3_2016_08_28_frozen.pb imagenet_slim_labels.txt {myoutput}"
SUFFIX_TO_CLEAR = ['out']
VID_TO_PROC = "4kvid_chunk.webm"

def clear_chunks():
    to_delete = []
    for stc in SUFFIX_TO_CLEAR:
        to_delete.extend(glob.glob('*.' + stc))

    for todel in to_delete:
        if os.path.exists(todel):
            os.remove(todel)

def get_duration(myvid):
    cmd = './ffmpeg -i %s 2>&1 | grep -e "fps" -e "Duration"' % myvid
    process = sp.Popen(cmd, shell=True, stdout=sp.PIPE, stderr=sp.PIPE)
    out, _ = process.communicate()
    out_split = out.decode('utf-8').split()
    duration = out_split[out_split.index('Duration:') + 1].strip(',')
    duration_split = duration.split(':')
    tot_seconds = float(duration_split[0])*3600.0 + float(duration_split[1])*60.0 + float(duration_split[2])

    print("Total duration: %s (%.3f seconds)" % (duration, tot_seconds))
    return tot_seconds

def main():
    vid_dur = get_duration(VID_TO_PROC)

    gg = GG()
    all_thunks = []

    for ind, i in enumerate(np.arange(0, vid_dur - 0.5, 0.5)):
        next_min = '%02d' % int(i / 60)
        next_sec = '%02d' % (i % 60)
        next_outname = 'frameout%02d.jpg' % ind
        next_cmd = CMD.format(video=VID_TO_PROC, min=next_min, sec=next_sec, ofile=next_outname)
        next_cmd_split = next_cmd.split()
        gen_jpg_thunk = GGThunk(exe=next_cmd_split[0], outname=next_outname,
            exe_args=next_cmd_split[1:], args_infiles=False)
        gen_jpg_thunk.add_infile(VID_TO_PROC)

        pic_out = 'frameout%02d_lab.out' % ind
        last_cmd = CMD_IMREC.format(myimage=next_outname, myoutput=pic_out)
        last_cmd_split = last_cmd.split()
        last_thunk = GGThunk(exe=last_cmd_split[0], outname=pic_out,
            exe_args=last_cmd_split[1:], args_infiles=False)
        last_thunk.add_infile(['inception_v3_2016_08_28_frozen.pb',
            'imagenet_slim_labels.txt', gen_jpg_thunk])

        all_thunks.append(last_thunk)

    gg.create_thunks(all_thunks)

if __name__ == '__main__':
    clear_chunks()
    main()

