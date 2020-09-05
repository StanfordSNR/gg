#! /usr/bin/env python3.7

from typing import (
    Union,
    List,
    Optional,
    Callable,
    NamedTuple,
    Dict,
    Mapping,
    Iterable,
    BinaryIO,
    TypeVar,
    Sequence,
    NoReturn,
    Tuple,
)
import subprocess as sub
import shutil as sh
import hashlib
import base64
import sys
import pprint
import os
import inspect
import functools as ft
import pathlib
import tempfile
import itertools as it
import traceback as tb
import enum
import re


SHEBANG_RE = "^#! */usr/bin/env +python3.7"

MODULE_NAME = "pygg"
IMPORT_WRAPPER_HASH_ENVVAR = "IMPORT_WRAPPER_HASH"
SCRIPT_NAME_ENVVAR = "SCRIPT_NAME_ENVVAR"
DEFAULT_OUT = "out"
REQUIRED_BINS = ["gg-create-thunk", "gg-hash"]
GG_OUTPUT_DIR = "anonymous_outputs"

Hash = str

script_path = os.path.realpath(sys.argv[0])
lib_path = os.path.realpath(__file__)


class PyggError(Exception):
    pass


def _err(s: str) -> NoReturn:
    raise PyggError(s)


class IE(PyggError):
    """ Internal Error """

    def __init__(self, msg: str) -> None:
        super().__init__(f"Internal Error: {msg}")


T = TypeVar("T")


def _unreach() -> NoReturn:
    raise IE("This location should be unreachable")


def _which(cmd: str) -> str:
    t = sh.which(cmd)
    if t is None:
        _err(f"'{cmd}' is not present/executable")
    return t


def _gg_hash(data: bytes, tag: str) -> Hash:
    sha = hashlib.sha256()
    sha.update(data)
    h = (
        base64.urlsafe_b64encode(sha.digest())
        .decode("ascii")
        .replace("-", ".")
        .rstrip("=")
    )
    return f"{tag}{h}{len(data):08x}"

def _contains_thunk(path: str) -> bool:
    b = os.path.basename(path)
    if b[0] == "T" and len(b) == len("TI8oBoUKMAy5zKE6eLKutVStu.8U4krg.fclymPkZBgs00000322"):
        return True
    with open(path, "rb") as f:
        return f.readline(20).startswith(b"##GGTHUNK##")


if SCRIPT_NAME_ENVVAR not in os.environ:
    os.environ[SCRIPT_NAME_ENVVAR] = os.path.basename(script_path)

if IMPORT_WRAPPER_HASH_ENVVAR in os.environ:
    # In a worker
    IMPORT_WRAPPER_HASH = os.environ[IMPORT_WRAPPER_HASH_ENVVAR]
else:
    # Local
    import import_wrapper  # type: ignore

    IMPORT_WRAPPER_HASH = _gg_hash(open(import_wrapper.__file__, "rb").read(), "V")


class Value:
    _gg: "GG"
    _path: Optional[str]
    _hash: Optional[Hash]
    _bytes: Optional[bytes]
    saved: bool

    def __init__(
        self,
        gg: "GG",
        path: Optional[str],
        hash_: Optional[str],
        bytes_: Optional[bytes],
        saved: bool,
    ):
        self._gg = gg
        self._path = path
        self._hash = hash_
        self._bytes = bytes_
        self.saved = saved

    def as_bytes(self) -> bytes:
        """Get the content of the value, as bytes"""
        if self._bytes is None:
            assert self._path is not None, "No bytes nor path for this value..."
            with open(self._path, "rb") as f:
                self._bytes = f.read()
        return self._bytes

    def as_str(self) -> str:
        """Get the content of the value, as a string"""
        return self.as_bytes().decode()

    def path(self) -> str:
        """Get the path of the value's file, as a string.
        Raises a PyggError if the value does not have a path (e.g. if it had
        just been created using str_value)."""
        if self._path is None:
            _err(f"Value {self} has no path: it has not been written to the filesystem")
        return self._path

    def hash(self) -> str:
        """Get the hash of the value"""
        if self._hash is None:
            if self._path is not None:
                self._hash = self._gg._hash_file(self._path)
            else:
                assert (
                    self._bytes is not None
                ), "No bytes nor hash nor path for this value..."
                self._hash = _gg_hash(self._bytes, "V")
        return self._hash


Prim = Union[str, int, float, bool]
GG_PRIM_TYS = [str, int, float, bool]

FormalArg = Union[Prim, Value, Optional[Value]]
FORMAL_ARG_TYS = [str, int, float, bool, Value, Optional[Value]]

ActualArg = Union[Prim, Value, Optional[Value], "Thunk", "ThunkOutput"]
ACTUAL_ARG_TYS = [str, int, float, bool, Value, Optional[Value], "Thunk", "ThunkOutput"]

Output = Union[Value, "Thunk"]
OUTPUT_TYS = [Value, "Thunk"]
OutputDict = Mapping[str, Output]
MultiOutput = Union[Value, "Thunk", OutputDict]


class ThunkFn(NamedTuple):
    f: Callable[..., MultiOutput]
    outputs: Optional[Callable[..., List[str]]]

    def _sig(self) -> inspect.FullArgSpec:
        return inspect.getfullargspec(self.f)

    def named_outputs(self, gg: "GG", args: List[ActualArg]) -> Optional[List[str]]:
        if self.outputs is not None:
            op = self.outputs(*args)
            if len(op) == 0:
                _err(
                    f"The output profile {self.outputs.__name__} returned an empty list. Thunks must have at least one output.\nReturn:\n\t{op}"
                )
            return op
        return None

    def _check_output_sig(self) -> None:
        """ Check signature agreement """

        def ty_sig(s: inspect.FullArgSpec) -> List[type]:
            return [s.annotations[fa] for fa in s.args]

        f_sig = ty_sig(self._sig())
        if self.outputs is None:
            return
        o_sig = inspect.getfullargspec(self.outputs)
        o_args = ty_sig(o_sig)
        if f_sig != o_args:
            _err(
                f"The functions {self.f.__name__} and {self.outputs.__name__} should take the same arguments, since the latter is an output profile function for the former, but\n\t{f_sig}\nis not equal to\n\t{o_args}\n"
            )
        if (
            "return" not in o_sig.annotations
            or o_sig.annotations["return"] != List[str]
        ):
            _err(f"The output profile, {self.outputs.__name__} must return a List[str]")

    def __call__(self, *args):  # type: ignore
        return self.f(*args)


def _arg_decode(gg: "GG", arg: str, ex_type: type) -> FormalArg:
    """ Interprets the thunk argument `arg` as `ex_type`.
    For primitives, this is just a parse.
    For a value, this interprets `arg` as a path """
    if ex_type in GG_PRIM_TYS:
        return ex_type(arg)
    elif ex_type == Optional[Value]:
        if _contains_thunk(arg):
            return None
        else:
            gg._collect(arg)
            return Value(gg, arg, None, None, True)
    elif ex_type == Value:
        gg._collect(arg)
        return Value(gg, arg, None, None, True)
    else:
        _err(
            f"prim_dec: Unacceptable type {ex_type}. Acceptable types: {FORMAL_ARG_TYS}"
        )


class Thunk:
    gg: "GG"
    f: ThunkFn
    args: List[ActualArg]
    executable: bool

    @classmethod
    def _from_pgm_args(cls, gg: "GG", f: ThunkFn, str_args: List[str]) -> "Thunk":
        tys = f.f.__annotations__
        fargs = f._sig().args
        args = []
        for farg, str_arg in zip(fargs, str_args):
            ex_type = tys[farg]
            args.append(_arg_decode(gg, str_arg, ex_type))
        return cls(f, args, gg)

    def __init__(self, f: ThunkFn, args: Sequence[ActualArg], gg: "GG"):
        self.f = f  # type: ignore
        self.args = []
        self.executable = True
        self.gg = gg
        n = f.f.__name__

        def e(msg: str, note: Optional[str] = None) -> NoReturn:
            inv = f"{n}({', '.join(str(a) for a in args)})"
            m = f"Since\n\t{msg}\n, the thunk invocation\n\t{inv}\nis invalid\n"
            if note is not None:
                m += f"\nNote: {note}\n"
            _err(m)

        if n not in gg.thunk_functions:
            e(f"{n} is not a registered thunk function")
        if not isinstance(args, list):
            e(f"{args} is not a list")
        fargs = f._sig().args
        tys = f.f.__annotations__
        if len(fargs) != len(args):
            e(f"The number of arguments is incorrect")
        for farg, arg in zip(fargs, args):
            ex_type = tys[farg]
            if ex_type == Value or ex_type == Optional[Value]:
                if isinstance(arg, Thunk) or isinstance(arg, ThunkOutput):
                    self.executable = False
                elif isinstance(arg, Value):
                    pass
                elif ex_type == Optional[Value] and arg == None:
                    pass
                else:
                    e(
                        f"The actual argument {arg} should have type {tys[farg]} but has type {type(arg)}"
                    )
            elif not isinstance(arg, ex_type):
                if (
                ) and ex_type == Value:
                    self.executable = False
                else:
                    e(
                        f"The actual argument {arg} should have type {tys[farg]} but has type {type(arg)}"
                    )
            self.args.append(arg)

    def _exec(self) -> MultiOutput:
        assert self.executable
        r = self.f.f(*self.args)
        n = self.f.f.__name__
        ann = self.f.f.__annotations__["return"]
        if ann in OUTPUT_TYS and isinstance(r, ann):
            return r
        elif ann == OutputDict and isinstance(r, dict):
            outputs = self.f.named_outputs(self.gg, self.args)
            if outputs is None:
                raise IE("Missing outputs function")
            if set(r.keys()) != set(outputs):
                _err(
                    f"The thunk {n} returned outputs {set(r.keys())}, but its 'outputs' function says that it should return outputs {set(outputs)}."
                )
            return r
        elif ann == Output and (isinstance(r, Value) or isinstance(r, Thunk)):  # type: ignore
            return r
        else:
            _err(
                f"The thunk function {n} returned a {type(r)}, which is not a {ann}, as annotated"
            )

    def __repr__(self) -> str:
        return f"Thunk {self.f.f.__name__}({', '.join(str(a) for a in self.args)})"

    def _default_output(self) -> "ThunkOutput":
        return ThunkOutput(thunk=self, filename=None)

    def __getitem__(self, filename: str) -> "ThunkOutput":
        op = self.f.named_outputs(self.gg, self.args)
        if op is not None and op[0] == filename:
            return ThunkOutput(thunk=self, filename=None)
        else:
            return ThunkOutput(thunk=self, filename=filename)


class ThunkOutput(NamedTuple):
    thunk: Thunk
    # If there is no filename, this is the default output
    filename: Optional[str]


def _hash_deref(h: Hash) -> str:
    return "@{GGHASH:%s}" % h


def _hash_tag(h: Hash, filename: Optional[str]) -> str:
    return h if filename is None else f"{h}#{filename}"


def _prim_enc(prim: Prim) -> str:
    t = type(prim)
    if t not in GG_PRIM_TYS:
        _err(f"prim_end: Unacceptable type {t}. Acceptable types: {GG_PRIM_TYS}")
    return str(prim)


def _is_dynamic(path: str) -> bool:
    ldd_output = sub.check_output(["ldd", path]).decode()
    return "not a dynamic executable" in ldd_output


class GG:
    lib: Value
    script: Value
    import_wrapper: Value
    bins: Dict[str, Value]
    bin_order: List[Value]
    thunk_functions: Dict[str, ThunkFn]
    args: List[str]
    in_thunk: bool

    def __init__(
        self, lib: Value, script: Value, import_wrapper_: Value, args: List[str]
    ):
        self.lib = lib
        self.script = script
        self.import_wrapper = import_wrapper_
        self.bins = {}
        self.bin_order = []
        self.thunk_functions = {}
        self.args = args
        self.in_thunk = False

    def _collect(self, path: str) -> Hash:
        return ""

    def _hash_file(self, path: str) -> Hash:
        return (
            sub.check_output([self.bin("gg-hash").path(), path]).decode().strip()
        )

    def _assert_thunk_fn(self, fn_name: str, in_thunk_fn: bool) -> None:
        """ Checks that in_thunk_fn is the current state """
        if in_thunk_fn != self.in_thunk:
            place = "inside" if in_thunk_fn else "outside"
            _err(f"GG.{fn_name} can only be called {place} of a thunk function")

    def str_value(self, string: str) -> Value:
        """Create a value from a string"""
        try:
            self._assert_thunk_fn("str_value", True)
            return self.bytes_value(string.encode())
        except PyggError as e:
            _print_exit(e)

    def bytes_value(self, bytes_: bytes) -> Value:
        """Create a value from a byte sequence"""
        try:
            self._assert_thunk_fn("bytes_value", True)
            return Value(self, None, None, bytes_, False)
        except PyggError as e:
            _print_exit(e)

    def file_value(self, path: str, saved: bool = False) -> Value:
        """Create a value from a file. Takes ownership of the file, so it
        should not be manually removed after calling this."""
        try:
            self._assert_thunk_fn("file_value", True)
            return Value(self, path, None, None, saved)
        except PyggError as e:
            _print_exit(e)

    def thunk(self, f: ThunkFn, *args: ActualArg) -> Thunk:
        """Create a thunk from a function and its arguments.
        The arguments should be of the right type, save that Thunks and
        ThunkOutputs are substitutable for values."""
        try:
            self._assert_thunk_fn("thunk", True)
            return Thunk(f, list(args), self)
        except PyggError as e:
            _print_exit(e)

    def this(self) -> Output:
        """Returns exactly this thunk, used to indicate that this form is
        (non-recursively) irreducible"""
        this_hash = os.environ['__GG_THUNK_HASH__']
        return Value(self, None, this_hash, None, True)

    def _save_output(
        self, output: MultiOutput, dest_path: Optional[str] = None
    ) -> None:
        if isinstance(output, Mapping):
            for name, t in output.items():
                if not isinstance(name, str):
                    _err(f"The key {name} of {output} is not a string")
                self._save(t, name)
        else:
            self._save(output, dest_path)

    def _save(self, term: ActualArg, dest_path: Optional[str] = None) -> Hash:
        if isinstance(term, Value):
            p = term._path
            if term.saved:
                h = term.hash()
                if dest_path is not None and p != dest_path:
                    gg_dir = os.environ['__GG_DIR__']
                    sh.copy(f"{gg_dir}/{h}", dest_path)
                return h
            if p is None:
                ret = self._save_bytes(term.as_bytes(), dest_path)
            else:
                new_path = self._save_path(p, dest_path)
                term._path = new_path
                ret = term.hash()
            term.saved = True
            return ret
        elif isinstance(term, Thunk):
            return self._save_thunk(term, dest_path)
        elif isinstance(term, ThunkOutput):
            return _hash_tag(self._save_thunk(term.thunk, dest_path), term.filename)
        else:
            _err(f"Unknown type {type(term)}")

    def bin(self, name: str) -> Value:
        """Returns the value corresponding to the installed binary, `name`
        This name can be
           * The exact path that the binary was installed using
           * Just the basename"""
        try:
            if name not in self.bins:
                _err(f"Unknown bin: {name}")
            return self.bins[name]
        except PyggError as e:
            _print_exit(e)

    def install(self, cmd: str) -> None:
        """Find a runnable binary, and installs it, making it accessible inside
        thunks"""
        self._assert_thunk_fn("install", False)
        raise Exception("abstract")

    def _install_value(self, bin_: Value, names: List[str]) -> None:
        path = bin_._path
        if path is None:
            raise IE("Installed binaries must have paths!")
        elif not os.path.exists(path):
            _err(f"There is no file at '{path}', so it cannot be installed")
        elif not os.access(path, os.X_OK):
            _err(f"The file at '{path}' is not executable, so it cannot be installed")
        for name in names:
            if name in self.bins:
                _err(f"There is already a binary installed of name {name}")
            self.bins[name] = bin_
        self.bin_order.append(bin_)

    def _save_bytes(self, data: bytes, dest_path: Optional[str]) -> Hash:
        raise Exception("abstract")

    def _save_path(self, path: str, dest_path: Optional[str]) -> str:
        raise Exception("abstract")

    def _thunk_location_args(self, dest_path: Optional[str]) -> List[str]:
        raise Exception("abstract")

    def _save_thunk(self, t: Thunk, dest_path: Optional[str]) -> Hash:
        name = t.f.f.__name__
        bin_hashes = [bin_v.hash() for bin_v in self.bin_order]
        cmd = list(
            it.chain(
                [
                    "import_wrapper.py",
                    "--module",
                    MODULE_NAME,
                    _hash_deref(self.lib.hash()),
                    _hash_deref(self.script.hash()),
                    "exec",
                ],
                map(_hash_deref, bin_hashes),
                [name],
            )
        )
        executables = [self.import_wrapper.hash(), self.script.hash()] + bin_hashes
        thunks = []
        futures = []
        values = [self.lib.hash()]
        fparams = t.f._sig().args
        if len(t.args) != len(fparams):
            raise IE("The number of formal and actual params are not equal")
        for fp, ap in zip(fparams, t.args):
            ex_type = t.f.f.__annotations__[fp]
            if ex_type in GG_PRIM_TYS:
                cmd.append(_prim_enc(ap))  # type: ignore
            elif ex_type == Value or ex_type == Optional[Value]:
                h = self._save(ap)
                cmd.append(_hash_deref(h))
                if isinstance(ap, Value):
                    values.append(h)
                elif isinstance(ap, Thunk) or isinstance(ap, ThunkOutput):
                    if ex_type == Value:
                        thunks.append(h)
                    else:
                        futures.append(h)
                else:
                    _unreach()
            else:
                _unreach()
        outputs = []
        op = t.f.named_outputs(self, t.args)
        if op is None:
            outputs.append(DEFAULT_OUT)
        else:
            outputs.extend(op)
        value_args = it.chain.from_iterable(["--value", v] for v in values)
        thunk_args = it.chain.from_iterable(["--thunk", v] for v in thunks)
        future_args = it.chain.from_iterable(["--future", v] for v in futures)
        output_args = it.chain.from_iterable(["--output", v] for v in outputs)
        exec_args = it.chain.from_iterable(["--executable", v] for v in executables)
        env_additions = {
            "PYTHONDONTWRITEBYTECODE": "1",
            IMPORT_WRAPPER_HASH_ENVVAR: IMPORT_WRAPPER_HASH,
            SCRIPT_NAME_ENVVAR: os.environ[SCRIPT_NAME_ENVVAR],
        }
        if "PYGG_FULL_TRACE" in os.environ:
            env_additions["PYGG_FULL_TRACE"] = "1"
        env_args = it.chain.from_iterable(
            ["--envar", f"{k}={v}"] for k, v in env_additions.items()
        )
        loc_args = self._thunk_location_args(dest_path)
        cmd_args = list(
            it.chain(
                [self.bin("gg-create-thunk").path()],
                ["--output-dir", GG_OUTPUT_DIR],
                value_args,
                thunk_args,
                future_args,
                output_args,
                exec_args,
                loc_args,
                env_args,
                ["--", self.import_wrapper.hash()],
                cmd,
            )
        )
        result = sub.run(cmd_args, stderr=sub.PIPE, stdout=sub.PIPE, check=True)
        return result.stderr.decode().strip()

    def thunk_fn(
        self,
        outputs: Optional[Callable[..., List[str]]] = None,
    ) -> Callable[[Callable], ThunkFn]:
        f"""Decorator for turning a function into a thunk function.
        The function must:
            * Take primitives {GG_PRIM_TYS} or Values
            * Return a Output (Thunk or Value), or OutputDict (map from strings
              to Outputs)
        If it returns an OutputDict, the decorator must be passed an "outputs"
        function, which takes the same arguments, does not read the values, and
        returns a list of the output names (the keys in the OutputDict).
        """
        self._assert_thunk_fn("thunk_fn", False)

        def decorator_thunk_fn(func: Callable) -> ThunkFn:
            def e(msg: str, note: Optional[str] = None) -> NoReturn:
                m = f"In function `{func.__name__}`,\n\t{msg}\n, so `{func.__name__}` cannot be a thunk."
                if note is not None:
                    m += f"\n\nNote: {note}"
                _err(m)

            try:
                if "return" not in func.__annotations__:
                    e("there is no annotated return")
                ret = func.__annotations__["return"]
                if ret not in [OutputDict, Output, Value, Thunk]:  # type: ignore
                    e(
                        "the return is not annotated as a value, thunk, or output dictionary"
                    )
                if ret == OutputDict and outputs is None:  # type: ignore
                    e("the return is a MultiOutput, but there is no outputs")
                tf = ThunkFn(f=func, outputs=outputs)
                argspec = tf._sig()
                if argspec.varargs is not None:
                    e("there are varargs")
                if argspec.varkw is not None:
                    e("there are keyword args")
                if argspec.defaults is not None:
                    e("there are default arg values")
                params = argspec.args
                for p in params:
                    if p not in func.__annotations__:
                        e(f"the parameter `{p}` is not annotated")
                    if func.__annotations__[p] not in FORMAL_ARG_TYS:
                        e(
                            f"the parameter `{p}` has unacceptable type",
                            f"the acceptable types are: {FORMAL_ARG_TYS}",
                        )
                name = func.__name__
                assert name not in self.thunk_functions
                self.thunk_functions[name] = tf
                tf._check_output_sig()
                return tf
            except PyggError as ee:
                _print_exit(ee)

        return decorator_thunk_fn

    def main(self) -> None:
        """Yield control flow to pygg.
        Looks at the first argument to the program.
        If that argument is "init", parses
           <thunk_fn_name> [<thunk_fn_arguments> ...]
        and creates a thunk for that invocation in the "out" file.

        If that argument is "exec", parses
           <thunk_fn_name> [<thunk_fn_arguments> ...]
        and executes that thunk
        """
        raise Exception("abstract")


class GGWorker(GG):
    nextOutput: int

    def __init__(self, args: List[str]) -> None:
        script = Value(self, script_path, None, None, True)
        lib = Value(self, lib_path, None, None, True)
        iw = Value(self, None, IMPORT_WRAPPER_HASH, None, True)
        super().__init__(lib, script, iw, args)
        self.nextOutput = 0

    def _next_output_file(self) -> str:
        self.nextOutput += 1
        return f"{GG_OUTPUT_DIR}/{self.nextOutput - 1:03d}"

    def _save_bytes(self, data: bytes, dest_path: Optional[str]) -> Hash:
        if dest_path is None:
            dest_path = self._next_output_file()
        f = open(dest_path, "wb")
        f.write(data)
        f.close()
        return self._hash_file(dest_path)

    def _save_path(self, path: str, dest_path: Optional[str]) -> str:
        if dest_path is None:
            dest_path = self._next_output_file()
        sh.move(path, dest_path)
        return dest_path

    def _thunk_location_args(self, dest_path: Optional[str]) -> List[str]:
        if dest_path is None:
            dest_path = self._next_output_file()
        return ["--output-path", dest_path]

    def install(self, bin_: str) -> None:
        next_bin_idx = 1
        path = self.args[next_bin_idx]
        del self.args[next_bin_idx]
        if not os.access(path, os.X_OK):
            raise IE(
                f"another binary ({bin_}) is installed, but the next CLI arg ({path}) is not executable!"
            )
        v = Value(self, path, None, None, True)
        self._install_value(v, bin_names(bin_))

    def main(self) -> None:
        try:
            t_name = self.args[1]
            t_args = self.args[2:]
            f = self.thunk_functions[t_name]
            t = Thunk._from_pgm_args(self, f, t_args)
            if self.in_thunk:
                raise IE("recursive thunk exec?!")
            self.in_thunk = True
            os.mkdir(GG_OUTPUT_DIR)
            result = t._exec()
            self.in_thunk = False
            self._save_output(result, DEFAULT_OUT)
        except PyggError as e:
            _print_exit(e)


tty = os.isatty(sys.stderr.fileno())


class Ansi:
    BLACK = "\u001b[30m" if tty else ""
    RED = "\u001b[31m" if tty else ""
    GREEN = "\u001b[32m" if tty else ""
    YELLOW = "\u001b[33m" if tty else ""
    BLUE = "\u001b[34m" if tty else ""
    MAGENTA = "\u001b[35m" if tty else ""
    CYAN = "\u001b[36m" if tty else ""
    WHITE = "\u001b[37m" if tty else ""
    RESET = "\u001b[0m" if tty else ""
    BOLD = "\u001b[1m" if tty else ""
    UNDERLINE = "\u001b[4m" if tty else ""
    REVERSED = "\u001b[7m" if tty else ""


def _print_exit(e: Exception) -> NoReturn:
    def _print_span(path: str, lineno: int, ctx: int, prefix: str) -> None:
        i = lineno - 1
        is_ = list(range(i - ctx, i + ctx + 1))
        width = max(len(str(l + 1)) for l in is_)
        with open(path) as f:
            lines = list(it.islice(f, i + ctx + 1))[i - ctx : i + ctx + 1]
        for (j, line) in zip(is_, lines):
            print(prefix, file=sys.stderr, end="")
            print(
                "{}{{:{}d}}{}".format(Ansi.BLUE, width, Ansi.RESET).format(j + 1),
                file=sys.stderr,
                end="",
            )
            if i == j:
                print(f"|{Ansi.BOLD}{line}{Ansi.RESET}", file=sys.stderr, end="")
            else:
                print(f"|{line}", file=sys.stderr, end="")

    MODULE = "<module>"
    top = tb.extract_stack()
    bot = tb.extract_tb(e.__traceback__)
    whole = [f for f in it.chain(top, bot) if f.filename != __file__]
    print(f"\n{Ansi.BOLD}Traceback{Ansi.RESET}:", file=sys.stderr)
    last = False
    for f in it.chain(top, bot):
        if f.filename != __file__ or "PYGG_FULL_TRACE" in os.environ:
            print(
                f"  in file '{os.path.basename(f.filename)}', in function '{f.name}':",
                file=sys.stderr,
            )
            _print_span(f.filename, f.lineno, 1, "    ")
        elif last:
            print(f"  ... frames in {os.path.basename(__file__)}", file=sys.stderr)
        last = f.filename != __file__
    print(
        f"\n{Ansi.BOLD}{Ansi.RED}Error{Ansi.RESET}{Ansi.BOLD}:\n  {e}{Ansi.RESET}\n",
        file=sys.stderr,
    )
    sys.exit(1)


def bin_names(s: str) -> List[str]:
    return list({s, os.path.basename(s)})


class GGCoordinator(GG):
    def __init__(self, args: List[str]) -> None:
        script = Value(self, script_path, None, None, True)
        lib = Value(self, lib_path, None, None, True)
        iw = Value(self, None, IMPORT_WRAPPER_HASH, None, True)
        self.init()
        self._collect(script.path())
        self._collect(lib.path())
        self._collect(import_wrapper.__file__)
        super().__init__(lib, script, iw, args)

    def _collect(self, path: str) -> Hash:
        return sub.check_output([_which("gg-collect"), path]).decode().strip()

    def init(self) -> None:
        sub.check_call(["rm -rf .gg",], shell=True)
        sub.check_call([_which("gg-init")])

    def _save_bytes(self, data: bytes, dest_path: Optional[str]) -> Hash:
        if dest_path is None:
            f = tempfile.NamedTemporaryFile(mode="wb")
        else:
            f = open(dest_path, "wb")
        f.write(data)
        a = self._collect(f.name)
        f.close()
        return a

    def _save_path(self, path: str, dest_path: Optional[str]) -> str:
        if dest_path is not None:
            sh.copy(path, dest_path)
            path = dest_path
            self._collect(path)
        return path

    def _thunk_location_args(self, dest_path: Optional[str]) -> List[str]:
        if dest_path is None:
            return []
        else:
            return ["--placeholder", dest_path]

    def install(self, bin_: str) -> None:
        path = sh.which(bin_)
        if path is None:
            _err(f"Cannot find the binary '{bin_}', so cannot install it")
        v = Value(self, path, None, None, True)
        self._collect(path)
        self._install_value(v, bin_names(path))

    def main(self) -> None:
        try:
            t_name = self.args[1]
            t_args = self.args[2:]
            if t_name not in self.thunk_functions:
                _err(f"Unknown thunk name: '{t_name}'")
            f = self.thunk_functions[t_name]
            t = Thunk._from_pgm_args(self, f, t_args)
            self._save(t, DEFAULT_OUT)
        except PyggError as e:
            _print_exit(e)


def init() -> GG:
    """
    Initializes the GG environment.
    Looks at the first argument to the program.
    If that argument is "init", then it initilizes a "coordinator" GG
    environment, which will keep track of the computation, and has a .gg
    directory.
    If that argument is "exec", then initializes a "worker" GG environment,
    which allows a single step of the computation to be executed.
    """
    args = [a for a in sys.argv]
    if len(args) < 2:
        _err("There must be at least one argument: (init or run)")
    mode = args[1]
    del args[1]
    gg: GG
    if mode == "init":
        if not os.access(args[0], os.X_OK):
            _err(f"The script {args[0]} is not executable. It must be.")
        if not re.match(SHEBANG_RE, open(args[0], "r").read(100), re.MULTILINE):
            _err(
                f"The script {args[0]} must contain the shebang '#! /usr/bin/env python3.7'.\nThis is needed to run the script on AWS lambda."
            )
        gg = GGCoordinator(args)
    elif mode == "exec":
        gg = GGWorker(args)
    else:
        _err(f"The first argument must be (init|run), not {mode}")
    for b in REQUIRED_BINS:
        gg.install(b)
    return gg
