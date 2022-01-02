#!/usr/bin/env python3

"Converts shg.bib to UTF-8."

import re

with open('shg.bib', 'r', encoding='utf-8') as f, \
     open('shgutf8.bib', 'w', encoding='utf-8') as g:
    for line in f:
        line = re.sub(r"{\\k{a}}", 'ą', line)
        line = re.sub(r"{\\'{c}}", 'ć', line)
        line = re.sub(r"{\\k{e}}", 'ę', line)
        line = re.sub(r"{\\l}", 'ł', line)
        line = re.sub(r"{\\'{n}}", 'ń', line)
        line = re.sub(r"{\\'{o}}", 'ó', line)
        line = re.sub(r"{\\'{s}}", 'ś', line)
        line = re.sub(r"{\\'{z}}", 'ź', line)
        line = re.sub(r"{\\.{z}}", 'ż', line)
        line = re.sub(r"{\\k{A}}", 'Ą', line)
        line = re.sub(r"{\\'{C}}", 'Ć', line)
        line = re.sub(r"{\\k{E}}", 'Ę', line)
        line = re.sub(r"{\\L}", 'Ł', line)
        line = re.sub(r"{\\'{N}}", 'Ń', line)
        line = re.sub(r"{\\'{O}}", 'Ó', line)
        line = re.sub(r"{\\'{S}}", 'Ś', line)
        line = re.sub(r"{\\'{Z}}", 'Ź', line)
        line = re.sub(r"{\\.{Z}}", 'Ż', line)

        line = re.sub(r'{\\"{a}}', 'ä', line)
        line = re.sub(r'{\\"{o}}', 'ö', line)
        line = re.sub(r'{\\"{u}}', 'ü', line)
        line = re.sub(r'{\\"{A}}', 'Ä', line)
        line = re.sub(r'{\\"{O}}', 'Ö', line)
        line = re.sub(r'{\\"{U}}', 'Ü', line)

        line = re.sub(r"{\\'{e}}", 'é', line)
        line = re.sub(r"{\\'{E}}", 'É', line)
        line = re.sub(r"{\\'{i}}", 'í', line)
        line = re.sub(r"{\\'{I}}", 'Í', line)
        g.write(line)
