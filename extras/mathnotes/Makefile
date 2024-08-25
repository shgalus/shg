SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .tex .pdf

BIBINPUTS = ../bib
export BIBINPUTS

CHAPTERS = commalg.tex ds.tex oldnotes.tex

%.pdf : %.tex
	pdflatex $*; bibtex $*; pdflatex $*; pdflatex $*

.PHONY: all clean spotless

all: mathnotes.pdf

mathnotes.pdf: mathnotes.tex $(CHAPTERS) | $(BIBINPUTS)

$(BIBINPUTS):
	if [ ! -d "${BIBINPUTS}" ]; then \
	     git clone git@github.com:shgalus/bib.git ${BIBINPUTS}; \
	fi

clean:
	rm -f *.aux *.bbl *.blg *.log *.out *.toc

spotless: clean
	rm -f mathnotes.pdf
