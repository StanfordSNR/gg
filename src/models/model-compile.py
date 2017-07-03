#!/usr/bin/env python2

import sys
import os

sys.path.append("../protobufs/")
import gg_pb2 as gg

import hashlib
import argparse
from shutil import copyfile, copymode

GG_DIR = "GG_DIR"
OUTFLAG = "-o"
SRCFLAG = "-c"
GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc"
CC1=".gg/exe/bin/cc1"
MAGIC_NUMBER = "##GGTHUNK##"


def print_error():
  print "GG_DIR environment variable must be set."
  exit(-1)

def hash_file(filename):
  sha256 = hashlib.sha256()
  block_size = 65536
  with open(filename, 'rb') as f:
    for block in iter(lambda: f.read(block_size), b''):
      sha256.update(block)
  return sha256.hexdigest()

def set_function(thunk):
  thunk.function.exe = GCC_COMPILER
  thunk.function.args.extend(sys.argv[1:])
  if not "random-seed" in "".join(sys.argv):
    thunk.function.args.extend(["-frandom-seed=winstein"])
  thunk.function.hash = hash_file(GCC_COMPILER)
  return

def set_default_infiles(thunk):
  for f in [GCC_COMPILER, CC1]:
    infile = gg.InFile()
    infile.order = 0
    infile.filename = f
    infile.hash = hash_file(f)
    thunk.infiles.extend([infile])
  return

def parse_args():
  parser = argparse.ArgumentParser()
  parser.add_argument("src_file")
  parser.add_argument("-S", action='store_true')
  parser.add_argument("-c", action='store_true')
  parser.add_argument("-g", action='store_true')
  parser.add_argument("-O2", action='store_true')
  parser.add_argument("-frandom-seed")
  parser.add_argument("-o", "--outfile", nargs=1)
  args = parser.parse_args()
  return args.src_file, args.outfile[0]

def get_source_file():
  return parse_args()[0]

def set_infiles(thunk):
  set_default_infiles(thunk)
  src_file = get_source_file()
  infile = gg.InFile()
  infile.order = 0
  infile.filename = src_file
  infile.hash = hash_file(src_file)
  thunk.infiles.extend([infile])
  return

def set_outfile(thunk):
  thunk.outfile = parse_args()[1]
  return

def put_to_gg(infile):
  #TODO : put back thunk order
  #new_name = os.path.join(gg_dir, str(infile.order) + ":" + infile.hash)
  new_name = os.path.join(gg_dir, infile.hash)
  copyfile(infile.filename, new_name)
  copymode(infile.filename, new_name)
  return

def build_thunk():
  thunk = gg.Thunk()
  set_outfile(thunk)
  set_infiles(thunk)
  # write infiles to .gg
  for infile in thunk.infiles:
    put_to_gg(infile)
  set_function(thunk)
  return thunk

def write_thunk(thunk):
  with open(thunk.outfile, "wb") as out:
    out.write(MAGIC_NUMBER)
    out.write(thunk.SerializeToString())
  return

if __name__=="__main__":
  try:
    gg_dir = os.environ[GG_DIR]
  except KeyError:
    print_error()
  if gg_dir is None:
    print_error()

  thunk = build_thunk()
  write_thunk(thunk)
