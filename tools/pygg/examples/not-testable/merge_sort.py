#! /usr/bin/env python3.7

import pygg

gg = pygg.init()

@gg.thunk_fn()
def merge(a: pygg.Value, b: pygg.Value) -> pygg.Value:
    al = [int(l.strip()) for l in open(a.path(), 'r').readlines()]
    bl = [int(l.strip()) for l in open(b.path(), 'r').readlines()]
    ai = 0
    bi = 0
    with open('c', 'w') as f:
        while ai < len(al) and bi < len(bl):
            if al[ai] < bl[bi]:
                f.writelines([str(al[ai]), "\n"])
                ai += 1
            else:
                f.writelines([str(bl[bi]), "\n"])
                bi += 1
        f.writelines(str(x) + "\n" for x in al[ai:])
        f.writelines(str(x) + "\n" for x in bl[bi:])
    return gg.file_value('c')

@gg.thunk_fn()
def sort(a: pygg.Value) -> pygg.Output:
    al = open(a.path(), 'r').readlines()
    if len(al) < 2:
        with open("out", "w") as f:
            f.writelines(al)
        return gg.file_value("out")
    else:
        with open("1", "w") as f:
            f.writelines(al[:len(al)//2])
        with open("2", "w") as f:
            f.writelines(al[len(al)//2:])
        return gg.thunk(merge,
                gg.thunk(sort, gg.file_value("1")),
                gg.thunk(sort, gg.file_value("2")))

gg.main()
