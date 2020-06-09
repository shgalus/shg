# Makefile: description file for make in shg/doc directory

.PHONY: all clean spotless

PDFDOC = bdstest.pdf drbnwtsn.pdf nhmm.pdf

%.pdf : %.tex
	pdflatex $*; pdflatex $*

all: html

html: $(PDFDOC) ../src/*.cc ../include/shg/*.h ../testing/*.cc	\
	../tools/*.cc mainpage.doc
	doxygen

clean:
	rm -rf *.aux *.log doxwarn.txt

spotless: clean
	rm -rf $(PDFDOC) html

