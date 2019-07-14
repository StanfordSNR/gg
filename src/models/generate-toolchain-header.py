#!/usr/bin/env python3

import re
import os
import sys
import hashlib
import base64
import subprocess as sub

BINDIR = sys.argv[1]

def sha256_checksum(filename, otype='V', block_size=65536):
    sha256 = hashlib.sha256()
    size = 0

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            size += len(block)
            sha256.update(block)
    hash = base64.urlsafe_b64encode(sha256.digest()).decode('UTF-8').replace('=','').replace('-', '.')
    return "{}{}{:08x}".format(otype, hash, size)

def get_include_path(language='c'):
    lang_switch = ''

    if language == 'c++':
        lang_switch = '-x c++ '

    command = "gcc-7 -E -Wp,-v {}- < /dev/null >/dev/null".format(lang_switch)
    output = sub.check_output(command, stderr=sub.STDOUT, shell=True, encoding='UTF-8')

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
    command = "gcc-7 -Wl,--verbose 2>/dev/null || exit 0"
    output = sub.check_output(command, shell=True, encoding='UTF-8')

    library_dirs = []

    for d in re.findall(r'SEARCH_DIR\("=([^;]+)"\);', output):
        library_dirs += [d]

    return library_dirs

def get_gcc_envars():
    command = "gcc-7 -print-search-dirs"
    output = sub.check_output(command, shell=True, encoding='UTF-8')

    INSTALL_PREFIX = ("install: ", "INSTALL_PATH")
    PROGRAMS_PREFIX = ("programs: =", "PROGRAMS_PATH")
    LIBRARIES_PREFIX = ("libraries: =", "LIBRARY_PATH")

    res = {}

    for line in output.split("\n"):
        for prefix in [INSTALL_PREFIX, PROGRAMS_PREFIX, LIBRARIES_PREFIX]:
            if line.startswith(prefix[0]):
                path = line[len(prefix[0]):]
                path = path.split(":")
                rpath = []

                for p in path:
                    if os.path.exists(p):
                        rpath += [os.path.abspath(p)]

                res[prefix[1]] = rpath

    return res

c_include_path = get_include_path()
cxx_include_path = get_include_path(language='c++')
ld_search_path = get_library_path()
gcc_envars = get_gcc_envars()

hh_file = open( "toolchain.hh", "w" );
cc_file = open( "toolchain.cc", "w" );

print_hh = lambda *args, **kwargs: print(*args, file=hh_file, **kwargs)
print_cc = lambda *args, **kwargs: print(*args, file=cc_file, **kwargs)

print_hh("""\
#ifndef TOOLCHAIN_HH
#define TOOLCHAIN_HH
""")

print_hh("""\
#include <sys/types.h>
#include <unordered_map>
#include <string>
#include <vector>

#include "thunk/thunk.hh"
#include "thunk/factory.hh"
#include "util/path.hh"
""")

print_hh('#define TOOLCHAIN_PATH "{}"'.format(BINDIR))
print_hh()
print_hh("""\
const std::string & program_hash( const std::string & name );
extern const std::vector<std::string> c_include_path;
extern const std::vector<std::string> cxx_include_path;
extern const std::vector<std::string> ld_search_path;
extern const std::vector<std::string> gcc_library_path;
extern const std::string gcc_install_path;

extern const std::string GCC;
extern const std::string GXX;
extern const std::string AS ;
extern const std::string CC1;
extern const std::string CC1PLUS;
extern const std::string COLLECT2;
extern const std::string LD;
extern const std::string AR;
extern const std::string RANLIB;
extern const std::string STRIP;
extern const std::string NM;
extern const std::string READELF;
extern const std::string GG_BIN_PREFIX;
extern const roost::path toolchain_path;

extern const std::unordered_map<std::string, ThunkFactory::Data> program_data;

#endif /* TOOLCHAIN_HH */""")

hh_file.close()

print_cc('#include "toolchain.hh"')
print_cc()
print_cc("using namespace std;")
print_cc()
print_cc("""\
#define PROGRAM(x) \\
  { x, \\
    { \\
      GG_BIN_PREFIX + "/" + x, \\
      ( toolchain_path / x ).string(), \\
      gg::ObjectType::Value, \\
      program_hash( x ), \\
    } \\
  }""")
print_cc()
print_cc("""\
const string & program_hash( const string & name )
{
  static const unordered_map<string, string> programs = {""")

for exe in os.listdir(BINDIR):
    exe_path = os.path.join(BINDIR, exe)
    exe_hash = sha256_checksum(exe_path, 'V')
    print_cc(" " * 4, end='')
    print_cc('{{ "{exe}", "{hash}" }},'.format(exe=exe, hash=exe_hash))

print_cc("  };\n")
print_cc("  return programs.at( name );")
print_cc("}\n")

print_cc("""\
const std::string GCC { "gcc" };
const std::string GXX { "g++" };
const std::string AS  { "as" };
const std::string CC1 { "cc1" };
const std::string CC1PLUS { "cc1plus" };
const std::string COLLECT2 { "collect2" };
const std::string LD { "ld" };
const std::string AR { "ar" };
const std::string RANLIB { "ranlib" };
const std::string STRIP { "strip" };
const std::string NM { "nm" };
const std::string READELF { "readelf" };
const std::string GG_BIN_PREFIX { "/__gg__" };
const roost::path toolchain_path { std::string( TOOLCHAIN_PATH ) };

const std::unordered_map<std::string, ThunkFactory::Data> program_data {
  PROGRAM( GCC ), PROGRAM( GXX ), PROGRAM( CC1 ), PROGRAM( CC1PLUS ),
  PROGRAM( AS ), PROGRAM( COLLECT2 ), PROGRAM( LD ), PROGRAM( AR ),
  PROGRAM( RANLIB ), PROGRAM( STRIP ), PROGRAM( NM ), PROGRAM( READELF )
};
""")

print_cc("const vector<string> c_include_path = {")
for path in c_include_path:
    print_cc('  "{}",'.format(path))
print_cc("};\n")

print_cc("const vector<string> cxx_include_path = {")
for path in cxx_include_path:
    print_cc('  "{}",'.format(path))
print_cc("};\n")

print_cc("const vector<string> ld_search_path = {")
for path in ld_search_path:
    print_cc('  "{}",'.format(path))
print_cc("};\n")

print_cc('const vector<string> gcc_library_path = {')
for path in gcc_envars.get('LIBRARY_PATH', []):
    print_cc('  "{}",'.format(path))
print_cc("};\n")

print_cc('const string gcc_install_path = "{}";\n'.format(gcc_envars.get('INSTALL_PATH',[''])[0]))

cc_file.close()
