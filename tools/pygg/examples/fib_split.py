#!/usr/bin/env python3.7
# ARGS: fib 5
# RESULT: 5
import pygg
from typing import List

gg = pygg.init()


@gg.thunk_fn()
def fib(n: int) -> pygg.Output:
    return gg.thunk(fib_, gg.str_value(str(n)))


@gg.thunk_fn()
def fib_(n: pygg.Value) -> pygg.Output:
    i = int(n.as_str())
    if i < 2:
        return gg.str_value(str(i))
    else:
        s = gg.thunk(split, n)
        a = gg.thunk(fib_, s)
        b = gg.thunk(fib_, s["n2"])
        return gg.thunk(add_str, a, b)

def split_outputs(_n: pygg.Value) -> List[str]:
    return ["n1", "n2"]


@gg.thunk_fn(outputs=split_outputs)
def split(n: pygg.Value) -> pygg.OutputDict:
    i = int(n.as_str())
    return {
        "n1": gg.str_value(str(i - 1)),
        "n2": gg.str_value(str(i - 2)),
    }


@gg.thunk_fn()
def add_str(a: pygg.Value, b: pygg.Value) -> pygg.Output:
    ai = int(a.as_str())
    bi = int(b.as_str())
    return gg.str_value(str(ai + bi))


gg.main()
