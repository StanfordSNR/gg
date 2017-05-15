#!/usr/bin/env python3

# This scripts takes in an strace's syscallent.h file, and produces a syscall
# table that can be used with gg.

import os
import sys
import re

item_template = """  {{
    {num},
    {{
      {num}, "{name}",
      {{}},
      {flags}
    }}
  }},
"""

sysent_pattern = r"\[(.+)\] = {(.+)},"

out_start = '#include "syscall.hh"\n\nconst std::map<long, SystemCallSignature> syscall_signatures =\n{\n'
out_end = "};\n"

strace_flags = {
    "TD"  : "TRACE_DESC",
    "TF"  : "TRACE_FILE",
    "TI"  : "TRACE_IPC",
    "TN"  : "TRACE_NETWORK",
    "TP"  : "TRACE_PROCESS",
    "TS"  : "TRACE_SIGNAL",
    "TM"  : "TRACE_MEMORY",
    "TSF" : "TRACE_STATFS",
    "NF"  : "SYSCALL_NEVER_FAILS",
    "MA"  : "MAX_ARGS",
    "SI"  : "STACKTRACE_INVALIDATE_CACHE",
    "SE"  : "STACKTRACE_CAPTURE_ON_ENTER",
    "CST" : "COMPAT_SYSCALL_TYPES",
}

def fix_flags(flags):
    if flags == "0": return "0"

    flags = flags.split("|")
    new_flags = [strace_flags[x] for x in flags]
    return " | ".join(new_flags)

def generate(input_path, output_path):
    syscall_data = []
    with open(input_path) as fin:
        for line in fin:
            line = line.strip()
            m = re.match(sysent_pattern, line)
            syscall = {}
            if m:
                syscall['num'] = int(m.group(1).strip())
                data = re.split(r'[",\t ]+', m.group(2).strip())
                syscall['name'] = data[3]
                syscall['flags'] = fix_flags(data[1])

                syscall_data += [syscall]

    with open(output_path, "w") as fout:
        fout.write(out_start)
        for syscall in syscall_data:
            fout.write(item_template.format(**syscall))
        fout.write(out_end)

if __name__ == '__main__':
    generate(sys.argv[1], "/dev/stdout")
