#!/usr/bin/env python3.7
# ARGS: fib 5
# RESULT: 5
import pygg

gg = pygg.init()


@gg.thunk_fn()
def fib(n: int) -> pygg.Output:
    if n < 2:
        return gg.str_value(str(n))
    else:
        a = gg.thunk(fib, n - 1)
        b = gg.thunk(fib, n - 2)
        return gg.thunk(add_str, a, b)


@gg.thunk_fn()
def add_str(a: pygg.Value, b: pygg.Value) -> pygg.Output:
    ai = int(a.as_str())
    bi = int(b.as_str())
    path = 'random'
    with open(path, 'w') as f:
        f.write(str(ai + bi))
    return gg.file_value(path)


gg.main()
