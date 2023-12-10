#!/usr/bin/env python3

"This module checks source code."

import sys
import glob
import re
import os

MAX_LINE_LENGTH = 70

os.chdir('..')

ALL_SOURCE = glob.glob("include" + os.path.sep + "shg" +
                       os.path.sep + ".h")
ALL_SOURCE += glob.glob("src" + os.path.sep + "*.cc")
ALL_SOURCE += glob.glob("tests" + os.path.sep + "*.h")
ALL_SOURCE += glob.glob("tests" + os.path.sep + "*.cc")
ALL_SOURCE += glob.glob("tools" + os.path.sep + "*.cc")
ALL_SOURCE += glob.glob("examples" + os.path.sep + "*.cc")

N_ERRORS = 0

def print_error(fname, line, message):
    "Increases error count by 1 and prints error message."
    global N_ERRORS
    N_ERRORS += 1
    if not fname:
        s = f'{message}'
    elif line == 0:
        s = f'{fname}: {message}'
    else:
        s = f'{fname}:{line}: {message}'
    print(s)

def check_file_as_binary(fname):
    "Checks if all bytes of a file are less than 128."
    with open(fname, 'rb') as f:
        byte_array = f.read()
    for byte in byte_array:
        if byte > 127:
            print_error(fname, 0, "non-ASCII character")

def check_file_as_text(fname):
    "Checks file as UTF-8 text file."
    with open(fname, encoding="utf-8") as f:
        all_lines = f.read().splitlines()
    is_previous_line_empty = False
    for i, line in enumerate(all_lines, start=1):
        if len(line) > MAX_LINE_LENGTH:
            print_error(fname, i, "line too long")
        if re.search(r'\s$', line):
            print_error(fname, i, "line ends with white space")
        for c in line:
            if ord(c) > 127:
                print_error(fname, i, "non-ASCII character")
        if not line:
            if is_previous_line_empty:
                print_error(fname, i, "consecutive blank lines")
            else:
                is_previous_line_empty = True
        else:
            is_previous_line_empty = False
        if '\t' in line:
            print_error(fname, i, "tab found")
        if re.search(r'\\cite$', line):
            print_error(fname, i, "badly formatted \\cite command")
        if re.search(r'#\s+define', line):
            print_error(fname, i, "#define contains spaces")
        if re.search(r'#\s+include', line):
            print_error(fname, i, "#include contains spaces")
        if re.search(r'\bfabs\b', line):
            print_error(fname, i, "found fabs()")
        if re.search(r'\basserts\b', line):
            print_error(fname, i, "found assert()")
        if re.search(r'\bpragma\b', line):
            print_error(fname, i, "found pragma")

def run():
    """Runs the script. If no arguments, check all source and version
    numbers."""
    source = sys.argv[1:]
    if not source:
        source = ALL_SOURCE
    fname = None
    for fname in source:
        check_file_as_text(fname)
        check_file_as_binary(fname)
    command = "cpplint --quiet"
    command += ' ' + ' '.join(source)
    errors = os.system(command)
    if N_ERRORS > 0 or errors != 0:
        print("There were errors")
        return 1
    return 0

if __name__ == '__main__':
    if run() != 0:
        sys.exit(1)
