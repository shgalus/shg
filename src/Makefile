# Makefile: description file for make in shg/src directory

include ../make.inc

DEFS += -DSHG_DATADIR="\"../data/\""

SOURCES := $(wildcard *.cc)
OBJECTS := $(SOURCES:%.cc=%.o)
# Other possibilities:
# SOURCES = $(wildcard ../testing/*.cc)
# SOURCES := $(shell find . -name '*.cc')
# OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

VERSION = $(shell sed -n                                             \
	'/SHG_VERSION "/ s/^\([^0-9]*\)\([.0-9]*\)\(.*$$\)/\2/ p'    \
	../include/shg/version.h)
STOBJD = ../build/static
SHOBJD = ../build/shared
STOBJS = $(OBJECTS:%.o=$(STOBJD)/%.o)
SHOBJS = $(OBJECTS:%.o=$(SHOBJD)/%.o)

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
