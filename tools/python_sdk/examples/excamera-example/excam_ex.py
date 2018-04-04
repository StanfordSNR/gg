#! /usr/bin/python3

from gg_sdk import GG, GGThunk
import sys
import math

VPXENC = 'vpxenc --ivf --codec=vp8 --good --cpu-used=0 --end-usage=cq --min-q=0 --max-q=63 --cq-level={quality} --buf-initial-sz=10000 --buf-optimal-sz=20000 --buf-sz=40000 --undershoot-pct=100 --passes=2 --auto-alt-ref=1 --threads=1 --token-parts=0 --tune=ssim --target-bitrate=4294967295 -o {output}.ivf {input}.y4m'
TERMINATE_CHUNK = "xc-terminate-chunk {input}.ivf {output}.ivf"
XC_DUMP_0 = 'xc-dump {input}.ivf {output}.state'
XC_DUMP_1 = 'xc-dump -S {input_state}.state {input}.ivf {output}.state'
XC_ENC_FIRST_FRAME = 'xc-enc -W -w 0.75 -i y4m -o {output}.ivf -r -I {source_state}.state -p {input_pred}.ivf {input}.y4m'
XC_ENC_REBASE = 'xc-enc -W -w 0.75 -i y4m -o {output}.ivf -r -I {source_state}.state -p {input_pred}.ivf -S {pred_state}.state {input}.y4m'

def bname(i):
    return "{:08d}".format(i)

def make_command(cmd):
    return "\t{}".format(cmd)

"""
For this pipeline, we must keep track of state. Therefore, as we create
the thunks, we add them to individual lists and index into them
accordingly.

In addition, since outputs are "recomputed", we should only return the
most recent GGThunk associated with a particular output. Hence we use
a dictionary with key=output, value=GGThunk.
"""
def generate_batch(start, end, quality):
        thunk_dict = {}
        vpxenc_list = []
        term_chunk_list = []
        xc_dump_list = []
        xc_enc_ff_list = []
        xc_dump1_list = []

        for i in range(start, end + 1):
            name = bname(i)

            vpxenc = make_command(VPXENC.format(quality=quality, 
                        input=name, output="%s-vpxenc" % name))
            vpxenc_split = vpxenc.split()
            vpxenc_thunk = GGThunk(exe=vpxenc_split[0], 
                    outname="%s-vpxenc.ivf" % name, exe_args=vpxenc_split[1:],
                    args_infiles=False)
            vpxenc_thunk.add_infile(name + '.y4m')

            vpxenc_list.append(vpxenc_thunk)
            thunk_dict["%s-vpxenc.ivf" % name] = vpxenc_thunk

            term_chunk_outname = ''
            if i == start:
                term_chunk = make_command(TERMINATE_CHUNK.format(input="%s-vpxenc" % name,
                                                               output="%s" % name))
                term_chunk_split = term_chunk.split()
                term_chunk_thunk = GGThunk(exe=term_chunk_split[0], 
                        outname="%s.ivf" % name, 
                        exe_args=term_chunk_split[1:],
                        args_infiles=False)
                term_chunk_outname = "%s.ivf" % name
            else:
                term_chunk = make_command(TERMINATE_CHUNK.format(input="%s-vpxenc" % name,
                                                               output="%s-0" % name))
                term_chunk_split = term_chunk.split()
                term_chunk_thunk = GGThunk(exe=term_chunk_split[0], 
                        outname="%s-0.ivf" % name,
                        exe_args=term_chunk_split[1:],
                        args_infiles=False)
                term_chunk_outname = "%s-0.ivf" % name

            term_chunk_thunk.add_infile(vpxenc_thunk)
            term_chunk_list.append(term_chunk_thunk)
            thunk_dict[term_chunk_outname] = term_chunk_thunk

            if i == start:
                xc_dump = make_command(XC_DUMP_0.format(input=name, output="%s-0" % name))
                xc_dump_split = xc_dump.split()
                xc_dump_thunk = GGThunk(exe=xc_dump_split[0], 
                        outname="%s-0.state" % name,
                        exe_args=xc_dump_split[1:],
                        args_infiles=False)
            else:
                xc_dump = make_command(XC_DUMP_0.format(input="%s-0" % name, 
                            output="%s-0" % name))
                xc_dump_split = xc_dump.split()
                xc_dump_thunk = GGThunk(exe=xc_dump_split[0], 
                        outname="%s-0.state" % name,
                        exe_args=xc_dump_split[1:],
                        args_infiles=False)

            xc_dump_thunk.add_infile(term_chunk_thunk)
            xc_dump_list.append(xc_dump_thunk)
            thunk_dict["%s-0.state" % name] = xc_dump_thunk

        for ind, i in enumerate(range(start + 1, end + 1)):
            name = bname(i)
            prev_name = bname(i - 1)

            x_enc_ff_outname = ''
            if i == (start + 1):
                xc_enc_ff = make_command(XC_ENC_FIRST_FRAME.format(input=name, output=name,
                                                          source_state="%s-0" % prev_name,
                                                          input_pred="%s-0" % name))
                xc_enc_ff_split = xc_enc_ff.split()
                xc_enc_ff_thunk = GGThunk(exe=xc_enc_ff_split[0], 
                        outname=name+'.ivf',
                        exe_args=xc_enc_ff_split[1:],
                        args_infiles=False)
                x_enc_ff_outname = name+'.ivf'
            else:
                xc_enc_ff = make_command(XC_ENC_FIRST_FRAME.format(input=name, 
                                                          output="%s-1" % name,
                                                          source_state="%s-0" % prev_name,
                                                          input_pred="%s-0" % name))
                xc_enc_ff_split = xc_enc_ff.split()
                xc_enc_ff_thunk = GGThunk(exe=xc_enc_ff_split[0], 
                        outname=name+'-1.ivf',
                        exe_args=xc_enc_ff_split[1:],
                        args_infiles=False)
                x_enc_ff_outname = name+'-1.ivf'

            xc_enc_ff_thunk.add_infile([xc_dump_list[ind], 
                    term_chunk_list[ind+1], name+'.y4m'])
            xc_enc_ff_list.append(xc_enc_ff_thunk)
            thunk_dict[x_enc_ff_outname] = xc_enc_ff_thunk

            if i == (start + 1):
                xc_dump1 = make_command(XC_DUMP_1.format(input=name, 
                                                    input_state="%s-0" % prev_name,
                                                     output="%s-1" % name))
                xc_dump1_split = xc_dump1.split()
                xc_dump1_thunk = GGThunk(exe=xc_dump1_split[0], 
                        outname="%s-1.state" % name,
                        exe_args=xc_dump1_split[1:],
                        args_infiles=False)

                xc_dump1_thunk.add_infile([xc_dump_list[ind], xc_enc_ff_thunk])
                xc_dump1_list.append(xc_dump1_thunk)
                thunk_dict['%s-1.state' % name] = xc_dump1_thunk
                
        for ind, i in enumerate(range(start + 2, end + 1)):
            name = bname(i)
            prev_name = bname(i - 1)

            xc_enc_rb = make_command(XC_ENC_REBASE.format(output=name, input=name, 
                                source_state="%s-1" % prev_name,
                                input_pred="%s-1" % name, pred_state="%s-0" % prev_name))
            xc_enc_rb_split = xc_enc_rb.split()
            xc_enc_rb_thunk = GGThunk(exe=xc_enc_rb_split[0], 
                    outname=name+'.ivf',
                    exe_args=xc_enc_rb_split[1:],
                    args_infiles=False)
            xc_enc_rb_thunk.add_infile([xc_dump_list[ind+1], xc_enc_ff_list[ind+1],
                    xc_dump1_list[ind], name+'.y4m'])
            thunk_dict[name+'.ivf'] = xc_enc_rb_thunk

            if i != end:
                xc_dump12 = make_command(XC_DUMP_1.format(input=name, 
                                input_state="%s-1" % prev_name, output="%s-1" % name))

                xc_dump12_split = xc_dump12.split()
                xc_dump12_thunk = GGThunk(exe=xc_dump12_split[0], 
                        outname="%s-1.state" % name,
                        exe_args=xc_dump12_split[1:],
                        args_infiles=False)
                xc_dump12_thunk.add_infile([xc_dump1_list[ind], xc_enc_rb_thunk])
                xc_dump1_list.append(xc_dump12_thunk)
                thunk_dict["%s-1.state" % name] = xc_dump12_thunk

        return list(thunk_dict.values())

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print("Usage: gen_makefile.py <start> <end> <batch-size> <cq-level>")
        sys.exit(1)

    start = int(sys.argv[1])
    end = int(sys.argv[2])
    batch_size = int(sys.argv[3])
    quality = int(sys.argv[4])

    gg = GG()

    batch_start = start

    out_thunks = []
    for batch_index in range(math.ceil((end - start + 1) / batch_size)):
        batch_end = min(batch_start + batch_size - 1, end)
        out_thunks.extend(generate_batch(batch_start, batch_end, quality))
        batch_start = batch_end + 1

    gg.create_thunks(out_thunks)

