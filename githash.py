"This module creates the file githash.h with git hash."

import subprocess

RESULT = subprocess.run(["git", "describe", "--tags"],
                        stdout=subprocess.PIPE)
s = RESULT.stdout.decode("utf-8")
with open("githash.h", "w") as f:
    s = """#ifndef SHG_GIT_HASH
#define SHG_GIT_HASH "{}"
#endif""".format(s.rstrip())
    f.write(s)
