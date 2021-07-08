# Makefile: description file for make in shg/doc directory

.PHONY: all clean spotless

PDFDOC = shg.pdf

%.pdf : %.tex
	pdflatex $*; pdflatex $*

all: html

top: shg.pdf

shg.pdf: shg.tex drbnwtsn.tex nhmm.tex bdstest.tex gps.tex 1.pdf 2.pdf
	pdflatex shg; bibtex shg; pdflatex shg; pdflatex shg

1.pdf: shg.mp
	mpost shg
	epstopdf shg.1 --outfile=1.pdf
2.pdf: shg.mp
	mpost shg
	epstopdf shg.2 --outfile=2.pdf

html: $(PDFDOC) ../src/*.cc ../include/shg/*.h ../testing/*.cc	\
	../testing/*.h ../tools/*.cc mainpage.doc
	doxygen
	@if [ -s doxwarn.txt ]; \
	then \
        echo "\n==================\nThere were errors."; \
	fi

clean:
	rm -rf *.aux *.log *.bbl *.blg *.toc *.out *.mpx shg.[1-9]* \
	[1-9]*.pdf doxwarn.txt

spotless: clean
	rm -rf $(PDFDOC) html
