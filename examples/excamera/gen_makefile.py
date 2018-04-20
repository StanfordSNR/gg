#!/usr/bin/env python3

import sys
import math

VPXENC = 'vpxenc --ivf --codec=vp8 --good --cpu-used=0 --end-usage=cq --min-q=0 --max-q=63 --cq-level={quality} --buf-initial-sz=10000 --buf-optimal-sz=20000 --buf-sz=40000 --undershoot-pct=100 --passes=2 --auto-alt-ref=1 --threads=1 --token-parts=0 --tune=ssim --target-bitrate=4294967295 -o {output}.ivf {input}.y4m'
TERMINATE_CHUNK = "xc-terminate-chunk {input}.ivf {output}.ivf"
XC_DUMP_0 = 'xc-dump {input}.ivf {output}.state'
XC_ENC_FIRST_FRAME = 'xc-enc -W -w 0.75 -i y4m -o {output}.ivf -r -I {source_state}.state -p {input_pred}.ivf {extra} {input}.y4m'
XC_ENC_REBASE = 'xc-enc -W -w 0.75 -i y4m -o {output}.ivf -r -I {source_state}.state -p {input_pred}.ivf -S {pred_state}.state {extra} {input}.y4m'

def bname(i):
    return "{:08d}".format(i)

def make_command(cmd):
    return "\t{}".format(cmd)

def generate_batch(start, end, quality):
        print("### batch %d: [%s, %s] ###\n" % (batch_index, batch_start, batch_end))
        print("# stage 1: vpxenc\n")

        for i in range(start, end + 1):
            name = bname(i)

            print("## {}\n".format(name))

            print("{i}-vpxenc.ivf: {i}.y4m".format(i=name))
            print(make_command(VPXENC.format(quality=quality, input=name, output="%s-vpxenc" % name)))

            print()

            if i == start:
                print("{i}.ivf: {i}-vpxenc.ivf".format(i=name))
                print(make_command(TERMINATE_CHUNK.format(input="%s-vpxenc" % name,
                                                               output="%s" % name)))
            else:
                print("{i}-0.ivf: {i}-vpxenc.ivf".format(i=name))
                print(make_command(TERMINATE_CHUNK.format(input="%s-vpxenc" % name,
                                                               output="%s-0" % name)))

            print()

            if i == start:
                print("{i}-0.state: {i}.ivf".format(i=name))
                print(make_command(XC_DUMP_0.format(input=name, output="%s-0" % name)))
            else:
                print("{i}-0.state: {i}-0.ivf".format(i=name))
                print(make_command(XC_DUMP_0.format(input="%s-0" % name, output="%s-0" % name)))

            print()

        print("# stage 2: reencode-first-frame\n")

        for i in range(start + 1, end + 1):
            name = bname(i)
            prev_name = bname(i - 1)

            print("## {}\n".format(name))

            base_command = ''

            if i == (start + 1):
                print("{i}-1.state {i}.ivf: {i}.y4m {i}-0.ivf {j}-0.state".format(i=name, j=prev_name))
                base_command = XC_ENC_FIRST_FRAME.format(input=name, output=name,
                                                         source_state="%s-0" % prev_name,
                                                         extra="-O %s-1.state" % name,
                                                         input_pred="%s-0" % name)
            else:
                print("{i}-1.ivf: {i}.y4m {i}-0.ivf {j}-0.state".format(i=name, j=prev_name))
                base_command = XC_ENC_FIRST_FRAME.format(input=name, output="%s-1" % name,
                                                         source_state="%s-0" % prev_name,
                                                         extra='',
                                                         input_pred="%s-0" % name)

            print(make_command(base_command))

        print("# stage 3: rebase\n")

        for i in range(start + 2, end + 1):
            name = bname(i)
            prev_name = bname(i - 1)

            print("## {}\n".format(name))

            if i != end:
                print("{i}-1.state {i}.ivf: {i}.y4m {i}-1.ivf {j}-0.state {j}-1.state".format(i=name, j=prev_name))
            else:
                print("{i}.ivf: {i}.y4m {i}-1.ivf {j}-0.state {j}-1.state".format(i=name, j=prev_name))

            base_command = XC_ENC_REBASE.format(output=name, input=name,
                                                source_state="%s-1" % prev_name,
                                                extra=("-O %s-1.state" % name) if i != end else '',
                                                input_pred="%s-1" % name, pred_state="%s-0" % prev_name)

            print(make_command(base_command))

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print("Usage: gen_makefile.py <start> <end> <batch-size> <cq-level>", file=sys.stderr)
        sys.exit(1)

    start = int(sys.argv[1])
    end = int(sys.argv[2])
    batch_size = int(sys.argv[3])
    quality = int(sys.argv[4])

    print("all: {}\n".format(" ".join(["%s.ivf" % bname(i) for i in range(start, end + 1)])))

    print("clean:")
    print(make_command("rm -rf *.ivf *.state"))
    print()

    batch_start = start

    for batch_index in range(math.ceil((end - start + 1) / batch_size)):
        batch_end = min(batch_start + batch_size - 1, end)
        generate_batch(batch_start, batch_end, quality)
        batch_start = batch_end + 1
