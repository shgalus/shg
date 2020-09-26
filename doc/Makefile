# Makefile: description file for make in shg/doc directory

.PHONY: all clean spotless

PDFDOC = shg.pdf

%.pdf : %.tex
	pdflatex $*; pdflatex $*

all: html

top: shg.pdf

shg.pdf: shg.tex drbnwtsn.tex nhmm.tex bdstest.tex
	pdflatex shg; bibtex shg; pdflatex shg; pdflatex shg

html: $(PDFDOC) ../src/*.cc ../include/shg/*.h ../testing/*.cc	\
	../testing/*.h ../tools/*.cc mainpage.doc
	doxygen
	@if [ -s doxwarn.txt ]; \
	then \
        echo "\n==================\nThere were errors."; \
	fi

clean:
	rm -rf *.aux *.log *.bbl *.blg *.toc *.out doxwarn.txt

spotless: clean
	rm -rf $(PDFDOC) html
