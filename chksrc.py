#!/usr/bin/env python

import sys
import glob

all_source = (glob.glob("include/shg/*.h") +
              glob.glob("src/*.cc") +
              glob.glob("testing/*.h") +
              glob.glob("testing/*.cc") +
              glob.glob("tools/*.cc") +
              glob.glob("examples/*.cc"))
nerrors = 0

def check_file_as_binary(fname):
    global nerrors
    with open(fname, 'rb') as f:
        bs = f.read()
    for b in bs:
        if b > 127:
            nerrors += 1
            print("Non-ASCII characters found in file {}."
                  .format(fname))
            return

def check_file_as_text(fname):
    global nerrors
    with open(fname, encoding="utf-8") as f:
        ls = f.read().splitlines()
    for i, l in enumerate(ls, start=1):
        if len(l) > 70:
            nerrors += 1
            print("Line too long in file {}, line {}."
                  .format(fname, i))
        for c in l:
            if ord(c) > 127:
                nerrors += 1
                print("Non-ASCII character found in file {}, line {}."
                      .format(fname, i))



source = sys.argv[1:]
if len(source) == 0:
    source = all_source

for fname in source:
    check_file_as_binary(fname)
    check_file_as_text(fname)
