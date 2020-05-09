#!/usr/bin/env python

"This module checks source code."

import sys
import glob
import re
import os

FILTER = "-legal/copyright,"\
         "-build/header_guard,"\
         "-runtime/references,"\
         "-whitespace/indent,"\
         "-build/c++11,"\
         "-build/include,"\
         "-readability/streams,"\
         "-runtime/int,"\
         "-runtime/printf,"\
         "-runtime/arrays"

ALL_SOURCE = glob.glob("include" + os.path.sep + "shg" +
                       os.path.sep + ".h")
ALL_SOURCE += glob.glob("src" + os.path.sep + "*.cc")
ALL_SOURCE += glob.glob("testing" + os.path.sep + "*.h")
ALL_SOURCE += glob.glob("testing" + os.path.sep + "*.cc")
ALL_SOURCE += glob.glob("tools" + os.path.sep + "*.cc")
ALL_SOURCE += glob.glob("examples" + os.path.sep + "*.cc")

N_ERRORS = 0

def print_error(fname, line, message):
    "Increases error count by 1 and prints error message."
    global N_ERRORS
    N_ERRORS += 1
    if not fname:
        s = '{}'.format(message)
    elif line == 0:
        s = '{}: {}'.format(fname, message)
    else:
        s = '{}:{}: {}'.format(fname, line, message)
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
        if len(line) > 70:
            print_error(fname, i, "line too long")
        if re.search(r'\s$', line):
            print_error(fname, i, "line ends with white space")
        for c in line:
            if ord(c) > 127:
                print_error(fname, i, "non-ASCII character")
        if len(line) == 0:
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

def check_version_numbers():
    "Checks if version numbers in three places are the same."
    version1 = version2 = version3 = ""
    with open('include/shg/version.h', encoding="utf-8") as f:
        all_lines = f.read().splitlines()
    for line in all_lines:
        match = re.search(r'#define +SHG_VERSION +"([0-9.]+)'\
                          r'"\s*$', line)
        if match:
            version1 = match.group(1)
            break
    with open('doc/Doxyfile', encoding="utf-8") as f:
        all_lines = f.read().splitlines()
    for line in all_lines:
        match = re.search(r'PROJECT_NUMBER += +([0-9.]+)\s*$', line)
        if match:
            version2 = match.group(1)
            break
    with open('README', encoding="utf-8") as f:
        all_lines = f.read().splitlines()
    for line in all_lines:
        match = re.search(r'This directory contains the ([0-9.]+)'\
                          r' release of the library.', line)
        if match:
            version3 = match.group(1)
            break
    if version1 and version1 == version2 and version2 == version3:
        return
    print_error("", 0, "inconsistent version numbers")

def run():
    """Runs the script. If no arguments, check all source and version
    numbers."""
    source = sys.argv[1:]
    if len(source) == 0:
        source = ALL_SOURCE
        check_version_numbers()
    fname = None
    for fname in source:
        check_file_as_text(fname)
        check_file_as_binary(fname)
    command = "cpplint --quiet --linelength=70 --filter="
    command += FILTER
    command += ' ' + ' '.join(source)
    errors = os.system(command)
    if N_ERRORS > 0 or errors != 0:
        print("There were errors")
        return 1
    else:
        return 0

if __name__ == '__main__':
    if run() != 0:
        sys.exit(1)
