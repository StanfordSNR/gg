import sys
import os
import stat
import shutil
import subprocess as sp
import json
import hashlib
import base64
import multiprocessing as mp # For getting number of cores
import magic # pip install python_magic

from threading import Thread
from concurrent.futures import Future
from timeit import default_timer as now

"""
Function used in conjunction with threaded for returning values
"""
def call_with_future(fn, future, args, kwargs):
    try:
        result = fn(*args, **kwargs)
        future.set_result(result)
    except Exception as exc:
        future.set_exception(exc)

"""
Function for multithreading thunk generation in force()
"""
def threaded(fn):
    def wrapper(*args, **kwargs):
        future = Future()
        Thread(target=call_with_future, args=(fn, future, args, kwargs)).start()
        return future
    return wrapper

"""
GGThunk class. Each function is represented through this IR.

If args_infiles=True, GGThunk will denote all arguments that are not flags
as infiles. Sometimes, this will lead to incorrect infiles (e.g. -o <out>
will see <out> as an infile). Thus, the user can tell GG not to do this,
and instead pass the infiles in manually. May be a point of optimization.
"""
class GGThunk(object):
    def __init__(self, exe, envars=[], outname=[], exe_args=[],
                 args_infiles=True):
        self.exe = exe
        self.thunk_hash = ''
        self.args = exe_args
        self.envars = envars
        self.order = 0
        self.outname = outname
        self.file_infiles = {}
        self.ggth_infiles = []

        # Function is also an infile
        self.add_infile([exe])

        if not isinstance(self.envars, list):
            self.envars = [self.envars]
        if not isinstance(self.args, list):
            self.args = [self.args]
        if not isinstance(self.outname, list):
            self.outname = [self.outname]

        if args_infiles:
            for ea in self.args:
                # Only add if a GGThunk or a string but not a flag
                if (isinstance(ea, GGThunk) or
                    (isinstance(ea, str) and
                    '--' not in ea and
                    '-o' not in ea)):
                    self.add_infile([ea])

    """
    Function to add an infile once the thunk is created
    """
    # if_type is optional: should be able to predict file type
    def add_infile(self, all_inf, if_type='INVALID'):
        if not isinstance(all_inf, list):
            all_inf = [all_inf]
        for new_inf in all_inf:
            new_inf_file_flag = False
            new_inf_tuple_flag = False
            if isinstance(new_inf, str):
                new_inf_file_flag = True
            elif isinstance(new_inf, tuple):
                    new_inf_tuple_flag = True
            elif isinstance(new_inf, GGThunk):
                if len(new_inf.get_all_outname()) > 1:
                    print("GGThunks with multiple infiles must specify which outfile to use")
                    sys.exit(1)
            else:
                self.__inv_file_print()
                sys.exit(1)

            _if_type = if_type

            if if_type != 'INVALID':
                if ( if_type != 'FILE' or
                     if_type != 'EXECUTABLE' or
                     if_type != 'GGTHUNK' ):
                    self.__inv_file_print()
                    sys.exit(1)
            else:
                if new_inf_file_flag:
                    # Check if file exists before attempting to predict
                    check_file = os.path.isfile(new_inf)
                    if not check_file:
                        print(new_inf + " not found")
                        sys.exit(1)

                    # If it is not an x86 ELF executable or a Python script,
                    # predict it to be a regular FILE
                    pred_type = magic.from_file(new_inf)
                    if "ELF" in pred_type:
                        if "statically linked" in pred_type:
                            _if_type = 'EXECUTABLE'
                        else:
                            print("Only statically linked binaries supported")
                            sys.exit(1)
                    else:
                        _if_type = 'FILE'
                else:
                    _if_type = 'GGTHUNK'

            assert _if_type != 'INVALID'

            if new_inf_file_flag:
                self.file_infiles[new_inf] = _if_type
            elif new_inf_tuple_flag:
                self.ggth_infiles.append(new_inf)
            else:
                self.ggth_infiles.append((new_inf, ''))

    """
    Function called by the GG class to generate and
    serialize the thunk
    """
    def generate_thunk(self, outnum):
        # Go through GGThunk infiles and recursively generate
        for inf in self.ggth_infiles:
            curr_thunk = inf[0]
            curr_search = inf[1]
            if curr_thunk.outname == []:
                curr_thunk.add_outname('output_' + str(outnum))
            curr_thunk.generate_thunk(outnum + 1)

            # Accounts for user passing in GGThunk as exe_arg
            if curr_thunk in self.args:
                self.args[self.args.index(curr_thunk)] = curr_thunk.get_outname(curr_search)

        self.__compute_order()

        # Special case if there are no ggth_infiles: the order
        # is then 1
        if not self.ggth_infiles:
            self.order = 1

        self.__create_ser_thunk(outnum == 0)

    """-------- Helper and accessor functions --------"""

    """
    Serialize thunk by building the gg-create-thunk call
    """
    def __create_ser_thunk(self, isPlaceholder):
        all_infiles = self.__comb_infiles()

        cmd = ['gg-create-thunk']

        # Add envars
        for ev in self.envars:
            cmd.extend(['-v', ev])

        # Add outputs
        for o in self.outname:
            cmd.extend(['-o', o])

        # Create placeholder if applicable
        # XXX: support multiple placeholder generation
        if isPlaceholder:
            if len(self.outname) > 1:
                print("gg currently only supports target thunks to have 1 outfile")
                sys.exit(1)

            cmd.extend(['-C', './' + self.outname[0]])

        # Add all Infiles
        for inf in all_infiles:
            inf_type = inf[1]
            if inf_type == 'FILE' or inf_type == 'DUMMY_DIRECTORY':
                cmd.extend(['-v', inf[0]])
            elif inf_type == 'EXECUTABLE':
                cmd.extend(['-e', inf[0]])
            elif inf_type == 'GGTHUNK':
                cmd.extend(['-t', inf[0]])

        # Append to avoid getopt from seeing anything past this as flags
        cmd.append('--')

        # Get function hash and function args
        func_hash = self.__file_hash(self.exe)
        cmd.append(func_hash)
        func_args = [self.exe] + self.args
        cmd.extend(func_args)

        in_proc = sp.Popen(cmd, stdout=sp.PIPE, stderr=sp.PIPE)
        out = in_proc.communicate()[1].strip().decode('utf-8')

        if 'gg-create-thunk' in out:
            print("Error: " + out)
            print("Tried to run: " + ' '.join(cmd))

            sys.exit(1)

        self.thunk_hash = out

    """
    Thunk hash accessor
    """
    def get_hash(self):
        if self.thunk_hash == '':
            self.thunk_hash = __file_hash(self.exe)

        return self.thunk_hash

    """
    Thunk outfile accessor for all names
    """
    def get_all_outname(self):
        return self.outname

    """
    Thunk outfile accessor by name
    """
    def get_outname(self, search):
        if search == '':
            assert len(self.outname) == 1
            return self.outname[0]
        else:
            if search in self.outname:
                return self.outname[self.outname.index(search)]
            else:
                print('%s not found in list of outnames' % search)
                sys.exit(1)

    """
    Thunk order accessor
    """
    def get_order(self):
        return self.order

    """
    Function to add to the Thunk's outfile name
    """
    def add_outname(self, new_name):
        self.outname.append(new_name)

    """
    Function to either look up hash from hash_cache, or
    generate hash and make a hash_cache entry.

    NOTE: Python does not have a timespec struct, so the
    comparisons are only done with seconds, not nanoseconds.
    The nanoseconds entries are INCORRECT, but are implemented
    to maintain valid file format. However, since ggSDK
    generates its own thunks, this should not be a problem.
    """
    def __file_hash(self, filename):
        f_fd = os.open(filename, os.O_RDONLY)
        info = os.fstat(f_fd)
        os.close(f_fd)

        hash_name = "%d-%d-%s" % (info.st_dev, info.st_ino, filename)
        hash_path = '.gg/hash_cache/' + hash_name

        if os.path.exists(hash_path):
            h_fd = open(hash_path, 'r')
            h_readlines = h_fd.readlines()
            while len(h_readlines) == 0:
                h_readlines = h_fd.readlines()
            h_file_cont = h_readlines[0].split()
            h_fd.close()

            if len(h_file_cont) != 6:
                print("Bad cache entry:", hash_path)
                sys.exit(1)

            if (h_file_cont[0] == str(int(info.st_size)) and
               h_file_cont[1] == str(int(info.st_mtime)) and
               h_file_cont[3] == str(int(info.st_ctime))):
                  return h_file_cont[5]

        # File not in cache, compute hash and add to hash_cache
        next_hash = self.__gen_hash_file(filename)
        outstr = "%d %d %d %d %d %s" % (info.st_size, info.st_mtime, 100, info.st_ctime, 101, next_hash)
        h_fd = open(hash_path, 'w')
        h_fd.write(outstr)
        h_fd.close()

        return next_hash

    """
    Function to merge infiles (since they can be a mix of
    external files and other GGThunks
    """
    def __comb_infiles(self):
        # Combine all infiles
        all_infiles = []
        for k, v in self.file_infiles.items():
            next_hash = self.__file_hash(k)
            if not os.path.exists('.gg/blobs/' + next_hash):
                shutil.copy(k, '.gg/blobs/' + next_hash)
            next_size = os.path.getsize('.gg/blobs/' + next_hash)
            next_tuple = (next_hash, v)
            all_infiles.append(next_tuple)

            # Also need to replace filename in args with hash
            if k in self.args:
                self.args[self.args.index(k)] = (
                        '@{GGHASH:' + next_hash + '}')
        for ig in self.ggth_infiles:
            curr_thunk = ig[0]
            curr_search = ig[1]
            if curr_search == '':
                hash_outname = curr_thunk.get_hash()
            else:
                hash_outname = curr_thunk.get_hash() + '#' + curr_search
            next_tuple = (hash_outname, 'GGTHUNK')
            all_infiles.append(next_tuple)

            # Also need to replace filename in args with hash
            if curr_thunk.get_outname(curr_search) in self.args:
                self.args[self.args.index(curr_thunk.get_outname(curr_search))] = (
                        '@{GGHASH:' + hash_outname + '}')

        return all_infiles

    """
    Function to compute Thunk's order
    """
    def __compute_order(self):
        self.order = 0
        for inf in self.ggth_infiles:
            curr_thunk = inf[0]
            self.order = max(curr_thunk.get_order(), self.order)
        self.order += 1

    """
    Infile usage function
    """
    def __inv_file_print(self):
        print("Invalid file type. Options:")
        print("\tFILE: file/library input")
        print("\tEXECUTABLE: x86 ELF binary")
        print("\tGGTHUNK: GGThunk object")

    """
    Generate hash from a file
    """
    def __gen_hash_file(self, h_file):
        in_proc = sp.Popen(['gg-hash', h_file], stdout=sp.PIPE)
        out = in_proc.communicate()[0].strip()

        return out.decode('utf-8')

"""
GG class. Interfaces with the GG platform, creates GGThunk placeholders,
and creates graph.
"""
class GG(object):
    def __init__(self, cleanenv=True):
        if cleanenv:
            self.clean_env()

        self.initialize()

    """
    Function to clean gg environment
    """
    def clean_env(self, deepClean=False):
        if deepClean:
            if os.path.exists('.gg'):
                shutil.rmtree('.gg')
        else:
            if os.path.exists('.gg/reductions'):
                shutil.rmtree('.gg/reductions')
            if os.path.exists('.gg/remote'):
                shutil.rmtree('.gg/remote')

    """
    Initialize gg directories
    """
    def initialize(self):
        # Make blobs and hash_cache directory as well
        if not os.path.exists('.gg/hash_cache'):
            os.makedirs('.gg/hash_cache')

        if not os.path.exists('.gg/blobs'):
            os.makedirs('.gg/blobs')
            print("Initialized gg directory at: " + os.getcwd() + "/.gg")

    """
    Infer build from make builds
    """
    def infer_build_make(self, np=-1):
        make_cmd = 'make -j'
        if nproc != -1:
            if nproc == 0:
                make_cmd += '1'
            else:
                make_cmd += str(np)
        in_proc = sp.Popen(['gg-infer', make_cmd], stdout=sp.PIPE)
        out = in_proc.communicate()[0]
        return out

    """
    Infer builds using model-gcc
    """
    def infer_build_mgcc(self, gcc_cmd):
        cmd = ['model-gcc'] + gcc_cmd.split()
        in_proc = sp.Popen(cmd, stdout=sp.PIPE)
        out = in_proc.communicate()[0]
        return out

    """
    Generate gg-force command
    """
    def __get_force_comm(self, inputs, showstatus, env, genfunc, numjobs):
        if env == 'lambda':
            os.environ['GG_LAMBDA'] = '1'
        elif env == 'remote':
            os.environ['GG_REMOTE'] = '1'

        if genfunc:
            os.environ['GG_GENERIC_FUNCTION'] = '1'

        nj_inp = ['--jobs', str(numjobs)]

        cmd_start = ['gg-force']
        if showstatus:
            cmd_start.append('--status')
        if env == 'lambda':
            cmd_start.append('--engine=lambda')
        elif env == 'remote':
            cmd_start.append('--engine=remote')
        elif env == 'local':
            cmd_start.append('--engine=local')
        elif env == 'meow':
            cmd_start.append('--engine=meow')


        cmd = cmd_start + nj_inp + inputs
        return cmd

    """
    Multi-threading function for creating placeholders in parallel
    """
    @threaded
    def __distr_thunk_gen(self, my_chunk):
        for c in my_chunk:
            c.generate_thunk(0)

        all_out = []
        for inf in my_chunk:
            all_out.extend(inf.get_all_outname())

        return all_out

    """
    Function called by user to create thunks.
    Function will first create placeholders if needed
    by first creating all thunks (i.e. generating graph).
    This function will NOT execute the thunks
    """
    def create_thunks(self, inputs):
        start = now()
        # Perform sanity checks
        if not inputs:
            print("List of inputs is empty!")
            return

        if not isinstance(inputs, list):
            inputs = [inputs]

        # Check for valid inputs
        # If input type is GGThunk, the actual thunks need to be created
        # along with a placeholder per input
        cmd_inp = []
        if isinstance(inputs[0], GGThunk):
            # Set the input name before generating...needed to be
            # consistent with placeholder
            out_index = 0
            for inp in inputs:
                if inp.get_all_outname() == []:
                    next_filename = 'my_output_' + str(out_index) + '.out'
                    inp.add_outname(next_filename)
                    out_index += 1

            # Multithread thunk generation
            all_threads = []
            num_cores = mp.cpu_count()
            if len(inputs) < num_cores:
                for inp in inputs:
                    all_threads.append(self.__distr_thunk_gen([inp]))
            else:
                batch_size = int(len(inputs) / num_cores)
                for i in range(num_cores):
                    if i < num_cores-1:
                        all_threads.append(self.__distr_thunk_gen(inputs[i*batch_size:i*batch_size+batch_size]))
                    else:
                        all_threads.append(self.__distr_thunk_gen(inputs[i*batch_size:]))

            for at in all_threads:
                cmd_inp.extend(at.result())

            if len(cmd_inp) != len(inputs):
                print("Error: cmd_inp != inputs")
                sys.exit(1)
        elif isinstance(inputs[0], str):
            print("Nothing to generate...")
            cmd_inp = inputs
        else:
            print("invalid input: must be a GGThunk object")
            sys.exit(1)

        end = now()
        delta = end - start
        print("Time to generate thunks: %.3f seconds" % delta)
        return cmd_inp
