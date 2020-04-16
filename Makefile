# Makefile: description file for make in shg directory

prefix = /usr/local
libdir = $(prefix)/lib
includedir = $(prefix)/include/shg
docdir = $(prefix)/share/doc/shg
datadir = $(prefix)/share/shg/data

VERSION = $(shell sed -n                                              \
'/SHG_VERSION "/ s/^\([^0-9]*\)\([.0-9]*\)\(.*$$\)/\2/ p'             \
include/shg/version.h)

.PHONY: all lib testing btesting check bcheck doc examples lint	\
install uninstall tar dist clean almost-spotless spotless

all: lib testing check
ball: lib btesting bcheck
lib:
	cd src && $(MAKE)
testing:
	cd testing && $(MAKE)
check:
	cd testing && $(MAKE) check
btesting:
	cd btesting && $(MAKE)
bcheck:
	cd btesting && $(MAKE) bcheck
doc:
	cd doc && $(MAKE)
examples: lib
	cd examples && $(MAKE)
lint:
	./tools/scripts/checksrc.sh $(module)
addmod:
ifndef module
	@echo "ERROR! Say: make addmod module=<new module name>"
else
	./tools/scripts/addmod.sh $(module)
endif
TAGS:
	etags -lc++ include/shg/*.h src/*.cc
install: lib doc uninstall
	install -d $(includedir) $(libdir) $(docdir)/html/search
	install -m644 include/shg/*.h $(includedir)
	install -m644 lib/libshg.a lib/libshg.so.$(VERSION) $(libdir)
	install lib/libshg.so.$(VERSION) $(libdir)
	cd $(libdir) && ln -s libshg.so.$(VERSION) libshg.so
	install -m644 doc/html/*.* $(docdir)/html
	install -m644 doc/html/search/*.* $(docdir)/html/search
uninstall:
	rm -rf $(includedir)
	rm -f $(libdir)/libshg.*
	rm -rf $(docdir)
clean:
	cd tools && $(MAKE) clean
	cd examples && $(MAKE) clean
	cd src && $(MAKE) clean
	cd testing && $(MAKE) clean
	cd btesting && $(MAKE) clean
	cd doc && $(MAKE) clean
almost-spotless:
	rm -f ChangeLog TAGS shg[0-9][0-9]*.tar.gz shg-[0-9.]*.tar.gz
	cd tools && $(MAKE) spotless
	cd examples && $(MAKE) spotless
	cd src && $(MAKE) spotless
	cd testing && $(MAKE) spotless
	cd btesting && $(MAKE) spotless
spotless: almost-spotless
	cd doc && $(MAKE) spotless
cleandoc:
	cd doc && $(MAKE) spotless
