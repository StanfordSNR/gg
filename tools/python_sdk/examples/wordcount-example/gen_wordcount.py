#! /usr/bin/python

import sys
import os
import glob
import argparse
from timeit import default_timer as now
from gg_sdk import GG, GGThunk

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--num_reducers', '-r', type=int, required=False,
            dest='num_red', default=2, help='Number of reducers')
    return parser.parse_args()

def do_cleanup():
    to_delete = glob.glob('*.out')

    for todel in to_delete:
        if os.path.exists(todel):
            os.remove(todel)

def main(args):
    num_red = args.num_red

    all_text = glob.glob('x*')

    gg = GG()

    all_map_thunks = []
    all_map_outputs = []
    # Map step
    for itext in all_text:
        next_output = '%s-map.out' % itext
        map_cmd = 'do_map %s %s' % (itext, next_output)
        map_cmd_split = map_cmd.split()
        map_thunk = GGThunk(exe=map_cmd_split[0], outname=next_output,
            exe_args=map_cmd_split[1:], args_infiles=False)
        map_thunk.add_infile(itext)
        all_map_outputs.append(next_output)
        all_map_thunks.append(map_thunk)

    # Shuffle step
    shuf_out = ['shuffle_out%d.out' % x for x in range(num_red)]
    shuf_cmd = 'do_shuffle ' + ' '.join(all_map_outputs) + ' ' + str(num_red)
    shuf_cmd_split = shuf_cmd.split()
    shuf_thunk = GGThunk(exe=shuf_cmd_split[0],
                         outname=shuf_out,
                         exe_args=shuf_cmd_split[1:], args_infiles=False)
    shuf_thunk.add_infile(all_map_thunks)

    # Reduce step
    all_red_thunks = []
    all_red_outputs = []
    for i, nred in enumerate(shuf_out):
        next_output = 'red%d.out' % i
        red_cmd = 'do_reduce %s %s' % (nred, next_output)
        red_cmd_split = red_cmd.split()
        red_thunk = GGThunk(exe=red_cmd_split[0], outname=next_output,
            exe_args=red_cmd_split[1:], args_infiles=False)
        red_thunk.add_infile([(shuf_thunk, nred)])
        all_red_outputs.append(next_output)
        all_red_thunks.append(red_thunk)

    # Merge step
    merge_out = 'wordcount_merge.out'
    merge_cmd = 'do_merge ' + ' '.join(all_red_outputs) + ' ' + merge_out
    merge_cmd_split = merge_cmd.split()
    merge_thunk = GGThunk(exe=merge_cmd_split[0],
                         outname=merge_out,
                         exe_args=merge_cmd_split[1:], args_infiles=False)
    merge_thunk.add_infile(all_red_thunks)

    print("Thunks declared, now creating")
    gg.create_thunks(merge_thunk)

if __name__ == '__main__':
    do_cleanup()
    parsed_args = get_args()
    main(parsed_args)

