# Makefile: description file for make in shg/src directory

include ../make.inc

OBJECTS = bdstest.o date.o drbnwtsn.o except.o hmm.o ieee.o laplace.o	\
     mathprog.o mstat.o mzt.o normmix.o ols.o permut.o rng.o runs.o	\
     smc.o specfunc.o stock.o strdlr.o unicode.o utils.o version.o

VERSION = $(shell sed -n                                             \
	'/SHG_VERSION "/ s/^\([^0-9]*\)\([.0-9]*\)\(.*$$\)/\2/ p'    \
	../include/shg/version.h)
STOBJD = ../build/static
SHOBJD = ../build/shared
STOBJS = $(OBJECTS:%.o=$(STOBJD)/%.o)
SHOBJS = $(OBJECTS:%.o=$(SHOBJD)/%.o)

# To generate report, append this with 2>&1 | tee -a report.txt.
$(STOBJD)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
$(SHOBJD)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -fpic $< -o $@

.PHONY: all static shared clean spotless

all: static shared
static: ../lib/libshg.a
shared: ../lib/libshg.so

../lib/libshg.a: $(STOBJD) $(STOBJS) | ../lib
	$(AR) $(ARFLAGS) $@ $(STOBJS)
	$(RANLIB) $@
../lib/libshg.so: ../lib/libshg.so.$(VERSION)
	cd ../lib && ln -sf libshg.so.$(VERSION) libshg.so
../lib/libshg.so.$(VERSION): $(SHOBJD) $(SHOBJS) | ../lib
	$(CXX) -shared -Wl,-soname,libshg.so -o $@ $(SHOBJS)
$(STOBJD) $(SHOBJD) ../lib:
	mkdir -p $@
clean:
	rm -rf $(STOBJD) $(SHOBJD)
	if ! test "$$(ls -A ../build 2>/dev/null)"; \
	then                                        \
	    rm -rf ../build;                        \
	fi

spotless: clean
	rm -rf ../lib
-include $(OBJECTS:%.o=$(STOBJD)/%.d)
-include $(OBJECTS:%.o=$(SHOBJD)/%.d)
