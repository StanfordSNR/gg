#! /usr/bin/env python3.7

import argparse as ap
import sys
import pprint
import os
import subprocess as sub
import shutil as sh
from typing import NoReturn


def error(msg: str) -> NoReturn:
    print(f"import-wrapper.py ERROR: {msg}", file=sys.stderr)
    sys.exit(1)

SCRIPT_TEMP = "SCRIPT_TEMP" if "SCRIPT_NAME_ENVVAR" not in os.environ else os.environ["SCRIPT_NAME_ENVVAR"]

def main() -> None:
    p = ap.ArgumentParser(description="Run a python script with importable files")
    p.add_argument(
        "-m",
        "--module",
        action="append",
        nargs=2,
        metavar=("NAME", "PATH"),
        help="A name-file pair indicating the module name by which a file should be importable",
    )
    p.add_argument("args", nargs=ap.REMAINDER)
    r = p.parse_args()
    to_remove = []
    for name, path in r.module:
        if not name.isidentifier():
            error(f"The module name '{name}' is not a legal identifier")
        if not os.path.exists(path):
            error(f"The file '{name}' does not exist")
        dest = f"{name}.py"
        sh.copy(path, dest)
        to_remove.append(dest)
    sh.copy(r.args[0], SCRIPT_TEMP)
    to_remove.append(SCRIPT_TEMP)
    args = [f"./{SCRIPT_TEMP}"] + r.args[1:]
    cproc = sub.run(args)
    for path in to_remove:
        os.remove(path)
    sys.exit(cproc.returncode)


if __name__ == "__main__":
    main()
