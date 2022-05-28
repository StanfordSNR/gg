# pygg

A python library for writing `gg` thunks as python functions.

## Installation

Add this directory to your `PYTHONPATH`. `pygg.py` and `import_wrapper.py`
must be importable.

## Testing

Run `make` in `/test` first, to build the C++ program used by one test.

Run `python3.7 -m unittest` from the root of this python package.

Run `REMOTE_TEST=1 python3.7 -m unittest` in order to runt the tests on AWS
Lambda.

## Introduction

`pygg` is a Python library and runtime for defining `Thunk`s and manipulating
persistent `Value`s in python.

It allows you to write programs that look a lot like standard python scripts,
that use recursive functions, but that actually defer this recursion to new
computations.

Let's start with an annotated example:

```
#!/usr/bin/env python3.7                # Make this portably executable
                                        # AWS Lambda requires python3.7

import pygg                             # Import the library
gg = pygg.init()                        # Initilize the runtime

@gg.thunk_fn()                          # Make this function thunk-able
def fib(n: int) -> pygg.Output:         # Fn signature. Must be annotated.
    if n < 2:
        return gg.str_value(str(n))     # Return a persistent value.
    else:
        a = gg.thunk(fib, n - 1)        # Create a (recursive) thunk
        b = gg.thunk(fib, n - 2)        # Another one
        return gg.thunk(add_str, a, b)  # A thunk that depends on the others

@gg.thunk_fn()
def add_str(a: pygg.Value, b: pygg.Value) -> pygg.Output:
    ai = int(a.as_str())                # Get the contents of this value
    bi = int(b.as_str())
    return gg.str_value(str(ai + bi))

gg.main()                               # Yield control to the runtime
```

When the above script is run in "initialize" mode (`./example.py init fib 5`),
it creates a file (`out`) containing a thunk for the `fib` function executed
with `5` as an argument. That thunk can be forced (`gg-force out`) to evaluate
it.

## GG Types & Methods

### Top Level Functions

* `pygg.init()` initializes the runtime. It must be called first, and returns an
   object of type `GG`.
* `GG.thunk_fn()` is a decorator for making a function thunk-able. The
   function must be fully annotated, take only primitive (int, float, str)
   or `pygg.Value` arguments, and return a `pygg.Output` or `pygg.OutputDict`.
   See its documentation for details and optional arguments.
* `GG.install(cmd)` "installs" `cmd` in this script, making it accessible
   within thunks.
* `GG.main()` yields control to the gg runtime. Call it at the end.

### In-Thunk Functions

* `GG.thunk(thunk_fn, *args)` creates a thunk for running `thunk_fn` on
   `args`. The arguments can contain `Thunk`s or `ThunkOutput`s in place of
   values. Thunks are a subtype of `pygg.Output`, and are returnable from
   thunk functions.
* `GG.bin(cmd)` returns the value of an installed binary (then call
   `Value.path()` to get the path for `subprocess`). `cmd` can be the basename
   of the executable, or the exact path it was installed using.
* `GG.str_value(s)`, `GG.bytes_value(b)`, and `GG.path_value(p)` create
  `Value`s from strings, bytes, and files respectively. `Value`s are subtypes
  of `pygg.Output`, and are returnable from thunk functions.
  `GG.path_value(path)` takes ownership of the file. It should not be manually
  removed.

### Value Methods

* `Value.as_str()`, `Value.as_bytes()` read the value's contents as a string
   and bytes respectively.
* `Value.hash()` gives the hash of the value.
* `Value.path()` gives the path of a value, if it exits; values that have been
  created but not returned, generally speaking, do not have paths yet.

### Thunk Methods

* `Thunk[ name ]` for a string, `name`, gives the output of the thunk called
   "name". The result is of type `ThunkOutput`.

## Where GG Types are Acceptable

### Thunk Function Returns

A thunk function may be annotated as returning:

* A `Value`
* A `Thunk`
* An `Output`: a union of the two previous options
* An `OutputDict`: a dictionary from strings (output names) to their
   respective `Output`s.

### Thunk Function Arguments

A thunk function must have its arguments annotated. Their type may be:

* A "primitive" (`float`, `int`, `bool`, `str`)
* A `Value`

If a thunk is initialized using `script.py init <thunk_fn> <args...>`, then
these values will be parsed from `<args...>`, with values being taken as
files.

### Thunk invocation arguments

When a thunk function is invoked as a thunk (`gg.thunk(thunk_fn, args...)`),
`Thunk`s and `ThunkOutput`s can be provided in place of a `Value`. If a
`Thunk` is provided, the default (first) output is used.

## TODOs

   [ ] Implement `install` using the new thunk "links"
   [ ] Implement `save` non-recursively to avoid overflow.
