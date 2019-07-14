#!/usr/bin/env python3

# This scripts takes in an strace's syscallent.h file and produces a syscall
# table that can be used with gg.

# Generate Ctags for linux kernel by running:
# ctags --fields=afmikKlnsStz --c-kinds=+pc -R

# Tested **only** with Linux 4.10.0

from __future__ import print_function

import os
import sys
import re
import ctags

from ctags import CTags, TagEntry

item_template = """    // sys_{name}({signature_str}) [{file_info}]
    {{
      {num}, "{name}",
      {{
{args}
      }},
      {flags}, {complete}
    }},

"""

arg_template = "        {{ typeid( {type} ), {dir}, {flags} }}, /* {name} */"

sysent_pattern = r"\[(.+)\] = {(.+)},"

includes = [
    "#include <sys/types.h>",
    "#include <sys/stat.h>",
    "#include <sys/capability.h>",
    "#include <linux/types.h>",
    "#include <linux/aio_abi.h>",
    "#include <signal.h>",
    "#include <poll.h>",
    "#include <mqueue.h>"
]

type_fixes = {
    'umode_t': 'mode_t',
    'qid_t': 'int',
    'u32': '__u32',
    'u32 *': '__u32 *',
    'u64': '__u64',
}

out_start = '''#include "syscall.hh"

{includes}

// from <keyutils.h>
typedef int32_t key_serial_t;

const SystemCallSignature & syscall_signature( const size_t syscall_no )
{{
  static const SystemCallSignature syscall_signatures[] =
  {{
'''
out_end = '''\
  };

  return syscall_signatures[ syscall_no ];
}
'''

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

def process_signature(sig):
    if sig == "(void)":
        return []

    sig = sig.strip('()').split(',')
    sig = [x.strip().replace(" __user", "") for x in sig]
    sig = [re.sub(r'\*(\S+)', r'* \1', x) for x in sig]

    signature = []

    for arg_str in sig:
        if arg_str.endswith('*') or arg_str.replace("struct ", "").count(" ") == 0:
            arg_str += ' unnamed'

        idx = arg_str.rfind(' ')
        arg = {
            'type': arg_str[:idx],
            'name': arg_str[idx+1:],
            'dir': 'ARGUMENT_DIR_IN',
            'flags': 0
        }

        if arg['type'] in type_fixes:
            arg['type'] = type_fixes[arg['type']]

        if arg['name'] in ['filename', 'pathname', 'path', 'specialfile', 'oldname', 'newname', 'old', 'new', 'library', 'new_root', 'put_old', 'dev_name', 'dir_name', 'special', ]:
            arg['flags'] = 'ARGUMENT_F_PATHNAME'

        if arg['type'] in ['char *', 'void *'] and arg['name'] == 'buf':
            arg['dir'] = 'ARGUMENT_DIR_OUT'

        signature += [arg]

    return signature

def sig_to_str(signature):
    return ", ".join(["{} {}".format(x['type'], x['name']) for x in signature])

def generate(sysent_path, ctags_path, output_path):
    syscall_data = {}

    print(">>> Processing syscallent.h file...", file=sys.stderr, end=' ')
    with open(sysent_path, "r") as fin:
        for line in fin:
            line = line.strip()
            m = re.match(sysent_pattern, line)
            syscall = {}
            if m:
                syscall['num'] = int(m.group(1).strip())
                data = re.split(r'[",\t ]+', m.group(2).strip())
                syscall['name'] = data[3]
                syscall['flags'] = fix_flags(data[1])
                syscall['complete'] = False

                syscall_data[syscall['num']] = syscall

    print("done.", file=sys.stderr)

    print(">>> Processing kernel ctags to get the function signatures...", file=sys.stderr, end=' ')

    tags = CTags(ctags_path)
    entry = TagEntry()
    i = 0
    for k, syscall in syscall_data.items():
        if tags.find(entry, "sys_%s" % syscall['name'], ctags.TAG_FULLMATCH | ctags.TAG_OBSERVECASE):
            while entry['kind'] != 'prototype':
                if not tags.findNext(entry):
                    print("Could not find signature for {}".format(syscall['name']), file=sys.stderr)
                    break

            sig = process_signature(entry['signature'])
            syscall['signature_str'] = sig_to_str(sig)
            syscall['args'] = "\n".join([arg_template.format(**x) for x in sig])
            syscall['file_info'] = "{}:{}".format(entry['file'], entry['lineNumber'])
            syscall['complete'] = 'true'
        else:
            print("Could not find signature for {}".format(syscall['name']), file=sys.stderr)
            syscall['signature_str'] = "?"
            syscall['args'] = ''
            syscall['file_info'] = '?'
            syscall['complete'] = 'false'

    print("done.", file=sys.stderr)

    with open(output_path, "w") as fout:
        fout.write(out_start.format(includes="\n".join(includes)))
        i = 0
        for syscall in sorted(syscall_data.values(), key=lambda x: x['num']):
            if syscall['num'] != i:
                raise Exception("There's a gap in syscall table.")

            fout.write(item_template.format(**syscall))
            i += 1

        fout.write(out_end)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("usage: generate_table.py [syscallent.h file] [kernel ctags file]")
        sys.exit(1)

    generate(sys.argv[1], sys.argv[2], "/dev/stdout")
