#!/usr/bin/python3

import sys
import os
import glob
import subprocess as sp

list_path = 'all_list.txt'
do_merge_path = 'do_merge'
has_wall_path = 'has_wall'
ormap_path = 'ormap'

hash_start = '@{GGHASH:'
hash_end = '}'

# Make list
open(list_path, 'w').close()

# Get file and binary hashes
list_proc = sp.Popen(['gg-hash', list_path], stdout=sp.PIPE, stderr=sp.PIPE)
list_hash = list_proc.communicate()[0].strip().decode('utf-8')
if list_hash == '':
  print("Error getting list hash")
  sys.exit(1)

dmh_proc = sp.Popen(['gg-hash', do_merge_path], stdout=sp.PIPE, stderr=sp.PIPE)
do_merge_hash = dmh_proc.communicate()[0].strip().decode('utf-8')
if do_merge_hash == '':
  print("Error getting do_merge hash")
  sys.exit(1)

hwh_proc = sp.Popen(['gg-hash', has_wall_path], stdout=sp.PIPE, stderr=sp.PIPE)
has_wall_hash = hwh_proc.communicate()[0].strip().decode('utf-8')
if has_wall_hash == '':
  print("Error getting has_wall hash")
  sys.exit(1)

om_proc = sp.Popen(['gg-hash', ormap_path], stdout=sp.PIPE, stderr=sp.PIPE)
om_hash = om_proc.communicate()[0].strip().decode('utf-8')
if om_hash == '':
  print("Error getting om hash")
  sys.exit(1)

# Get input hashes
all_inputs = glob.glob('frame_objects/frameout*.out')
if not all_inputs:
  print("No inputs found")
  sys.exit(1)

all_inp_hashes = []
for inp in all_inputs:
  inp_proc = sp.Popen(['gg-hash', inp], stdout=sp.PIPE, stderr=sp.PIPE)
  inp_hash = inp_proc.communicate()[0].strip().decode('utf-8')
  if inp_hash == '':
    print("Error getting hash for " + inp)
    sys.exit(1)
  all_inp_hashes.append(inp_hash)

# Call gg-init
gg_init_proc = sp.Popen(['gg-init'], stdout=sp.PIPE, stderr=sp.PIPE)
gg_init_proc.communicate()

# Make gg-collect call
gg_coll_cmd = ['gg-collect'] + [list_path, do_merge_path, has_wall_path, ormap_path] + all_inputs
gg_coll_proc = sp.Popen(gg_coll_cmd, stdout=sp.PIPE, stderr=sp.PIPE)
gg_coll_out = gg_coll_proc.communicate()[1].strip().decode('utf-8')
if gg_coll_out == '':
  print("Error running gg-collect")
  sys.exit(1)

# Finally, make gg-create-thunk call
# Envars
envars = ['--envar', 'DO_MERGE_FUNCTION_HASH=' + do_merge_hash,
          '--envar', 'HAS_WALL_FUNCTION_HASH=' + has_wall_hash,
          '--envar', 'ORMAP_FUNCTION_HASH=' + om_hash]

# Values
values = []
for i in all_inp_hashes:
  values.extend(['--value', i])

# Executable
executable = ['--executable', om_hash]

# Outputs
outputs = ['--output', 'next', '--output', 'job', '--output', list_path]

# Placeholder
placeholder = ['--placeholder', 'all_found.out']

# Function
function = [om_hash, ormap_path]
for i in all_inp_hashes:
  function.append(hash_start + i + hash_end)
function.extend(['0', hash_start + list_hash + hash_end])

gg_ct_cmd = ['gg-create-thunk'] + envars + values + executable \
            + outputs + placeholder + ['--'] + function

gg_ct_proc = sp.Popen(gg_ct_cmd, stdout=sp.PIPE, stderr=sp.PIPE)
gg_ct_out = gg_ct_proc.communicate()[1].strip().decode('utf-8')
if gg_ct_out == '':
  print("Error running gg-create-thunk")
  sys.exit(1)




