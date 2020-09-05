#!/usr/bin/env python3.7
# ARGS: add 0 20
# RESULT: 20
# Copied
import pygg
from typing import List

gg = pygg.init()


@gg.thunk_fn()
def add(n: int, m: int) -> pygg.Output:
    return gg.thunk(add_, gg.str_value(str(n)), m)


def add_anon(_n: pygg.Value, m: int) -> int:
    return m

@gg.thunk_fn()
def add_(n: pygg.Value, m: int) -> pygg.Output:
    for _ in range(m):
        n = gg.thunk(inc, n) #type: ignore
    return n


@gg.thunk_fn()
def inc(n: pygg.Value) -> pygg.Value:
    i = int(n.as_str())
    return gg.str_value(str(i + 1))


gg.main()
