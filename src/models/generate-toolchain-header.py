#!/usr/bin/env python2

from __future__ import print_function

import re
import os
import sys
import hashlib
import subprocess as sub

BINDIR = sys.argv[1]

def sha256_checksum(filename, block_size=65536):
    sha256 = hashlib.sha256()

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            sha256.update(block)

    return sha256.hexdigest()

def get_include_path():
    command = "gcc -E -Wp,-v - < /dev/null >/dev/null"
    output = sub.check_output(command, stderr=sub.STDOUT, shell=True)

    include_dirs = []
    include_dirs_started = False

    for line in output.split("\n"):
        if line.startswith("#include <...> search"):
            include_dirs_started = True
            continue
        elif line.startswith("End of search list"):
            break
        elif not include_dirs_started:
            continue

        include_dirs += [line.strip()]

    return include_dirs

def get_library_path():
    command = "gcc -Wl,--verbose 2>/dev/null || exit 0"
    output = sub.check_output(command, shell=True)

    library_dirs = []

    for d in re.findall(r'SEARCH_DIR\("=([^;]+)"\);', output):
        library_dirs += [d]

    return library_dirs

c_include_path = get_include_path()
c_library_path = get_library_path()

print("""\
#ifndef TOOLCHAIN_HH
#define TOOLCHAIN_HH
""")

print("#include <unordered_map>")
print("#include <string>")

print()
print('#define TOOLCHAIN_PATH "{}"'.format(BINDIR))
print()

print("""\
const std::string & program_hash( const std::string & name )
{
  static const std::unordered_map<std::string, std::string> programs = {""")

for exe in os.listdir(BINDIR):
    exe_path = os.path.join(BINDIR, exe)
    exe_hash = sha256_checksum(exe_path)
    print(" " * 4, end='')
    print('{{ "{exe}", "{hash}" }},'.format(exe=exe, hash=exe_hash))

print("  };\n")
print("  return programs.at( name );")
print("}\n")

print("static const std::vector<std::string> c_include_path = {")
for path in c_include_path:
    print('  "{}",'.format(path))
print("};\n")

print("static const std::vector<std::string> c_library_path = {")
for path in c_library_path:
    print('  "{}",'.format(path))
print("};\n")

print("#endif /* TOOLCHAIN_HH */")
