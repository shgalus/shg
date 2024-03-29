# Makefile: description file for make in shg/doc directory

CURRENT_DIR = $(shell pwd)

.PHONY: all cvs clean spotless

PDFDOC = shg.pdf

%.pdf : %.tex
	pdflatex $*; pdflatex $*

all: html

top: shg.pdf

shg.pdf: shg.tex shg.bib drbnwtsn.tex nhmm.tex bdstest.tex gps.tex	\
        neuralnet.tex 1.pdf 2.pdf
	pdflatex shg; bibtex shg; pdflatex shg; pdflatex shg

1.pdf: shg.mp
	mpost shg
	epstopdf shg.1 --outfile=1.pdf
2.pdf: shg.mp
	mpost shg
	epstopdf shg.2 --outfile=2.pdf

shgutf8.bib : shg.bib bibtoutf8.py
	./bibtoutf8.py

html: shgutf8.bib $(PDFDOC) ../src/*.cc ../include/shg/*.h	\
	../tests/*.cc ../tests/*.h ../tools/*.cc mainpage.dox	\
	notes.dox
	doxygen
	touch html
	@if [ -s doxwarn.txt ]; \
	then \
	echo ""; \
	echo "======================"; \
	echo "= There were errors. ="; \
	echo "======================"; \
	echo ""; \
	fi

# Prepares CVSROOT for old CVS repository in shg-cvs.tar.bz2.
cvs:
	tar jxvf shg-cvs.tar.bz2
	rm -rf cvs
	mkdir cvs
	cd cvs && cvs -d $(CURRENT_DIR)/shg-cvs $ co shg
	cd cvs && cvs -d $(CURRENT_DIR)/shg-cvs $ co metashg

clean:
	rm -rf shgutf8.bib *.aux *.log *.bbl *.blg *.toc *.out *.mpx	\
	shg.[1-9]* [1-9]*.pdf doxwarn.txt

spotless: clean
	rm -rf $(PDFDOC) html shg-cvs cvs
