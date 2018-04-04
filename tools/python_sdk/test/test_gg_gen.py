#!/usr/bin/env python3

from gg_sdk import GG, GGThunk

test_prog_bin = 'test_program'
test_lines = 'test_lines.txt'
num_test_lines = 10

def main():
    gg = GG()
    all_thunks = []
    for i in range(num_test_lines):
        next_thunk = GGThunk(exe=test_prog_bin, outname='test_%d.out' % i,
                exe_args=[test_lines, '%d' % i], args_infiles=False)
        next_thunk.add_infile(test_lines)
        all_thunks.append(next_thunk)

    gg.create_thunks(all_thunks)

if __name__ == '__main__':
    main()
