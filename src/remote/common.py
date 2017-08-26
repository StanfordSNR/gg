import os
import stat
import subprocess as sub

def is_executable(path):
    st = os.stat(path)
    return ( st.st_mode & stat.S_IEXEC ) != 0

def make_executable(path):
    st = os.stat(path)
    os.chmod(path, st.st_mode | stat.S_IEXEC)

def run_command(command):
    res_code = sub.run(command).returncode

    if res_code:
        raise Exception("command failed: {}".format(" ".join(command)))
