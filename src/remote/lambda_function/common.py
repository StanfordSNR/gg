import os
import stat
import subprocess as sub
import base64
import hashlib

def sizeof_fmt(num, suffix='B'):
    for unit in ['','K','M','G','T','P','E','Z']:
        if abs(num) < 1024.0:
            return "%3.1f %s%s" % (num, unit, suffix)
        num /= 1024.0
    return "%.1f %s%s" % (num, 'Y', suffix)

def is_executable(path):
    st = os.stat(path)
    return ( st.st_mode & stat.S_IEXEC ) != 0

def make_executable(path):
    st = os.stat(path)
    os.chmod(path, st.st_mode | stat.S_IEXEC)

def run_command(command):
    try:
        output = sub.check_output(command, stderr=sub.STDOUT)
        return 0, output.decode('utf-8')
    except sub.CalledProcessError as exc:
        return exc.returncode, exc.output.decode('utf-8')
