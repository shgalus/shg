#!/usr/bin/env python3

"This module adds a new C++ module."

import sys
import bisect

HEADER_FILE = """/**
 * \\file include/shg/{low}.h
 * ==== Insert description. ====
 */

#ifndef SHG_{upp}_H
#define SHG_{upp}_H

namespace SHG {{

}}  // namespace SHG

#endif"""

SOURCE_FILE = """/**
 * \\file src/{low}.cc
 * ==== Insert description. ====
 */

#include <shg/{low}.h>

namespace SHG {{

}}  // namespace SHG"""

TEST_FILE = """#include <shg/{low}.h>
#include "testing.h"

namespace SHG::Testing {{

BOOST_AUTO_TEST_SUITE({low}_test)

BOOST_AUTO_TEST_CASE(basic_test) {{
     BOOST_CHECK(true);
}}

BOOST_AUTO_TEST_SUITE_END()

}}  // namespace SHG::Testing"""

LIBRARY_HEADER_FILE_BEGIN = """/**
 * \\file include/shg/shg.h
 * SHG library header file.
 */

#ifndef SHG_SHG_H
#define SHG_SHG_H

"""

LIBRARY_HEADER_FILE_END = """

#endif
"""

def add_module(module_name):
    """Adds a module."""
    low = module_name.lower()
    upp = module_name.upper()

    s = HEADER_FILE.format(low=low, upp=upp)
    with open("include/shg/{low}.h".format(low=low), 'w') as f:
        f.write(s)

    s = SOURCE_FILE.format(low=low)
    with open("src/{low}.cc".format(low=low), 'w') as f:
        f.write(s)

    s = TEST_FILE.format(low=low)
    with open("testing/{low}_test.cc".format(low=low), 'w') as f:
        f.write(s)

    with open("include/shg/shg.h", encoding="utf-8") as f:
        all_lines = f.read().splitlines()
    include_lines = [s for s in all_lines if s.startswith("#include")]
    include_lines.sort()
    new_line = '#include <shg/{low}.h>'.format(low=low)
    bisect.insort(include_lines, new_line)
    with open("include/shg/shg.h", 'w') as f:
        f.write(LIBRARY_HEADER_FILE_BEGIN)
        f.write("\n".join(include_lines))
        f.write(LIBRARY_HEADER_FILE_END)


def run():
    """Runs the script."""
    module_name = None
    for module_name in sys.argv[1:]:
        add_module(module_name)

if __name__ == "__main__":
    run()
