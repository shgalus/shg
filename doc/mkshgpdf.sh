#!/bin/sh

# Build shg.pdf on Windows.

mpost shg
epstopdf shg.1 --outfile=1.pdf
epstopdf shg.2 --outfile=2.pdf
pdflatex shg
bibtex shg
pdflatex shg
pdflatex shg
