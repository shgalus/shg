include ../make.inc

DEFS += -DSHG_DATADIR="\"../data/\""

LDLIBS += -lboost_program_options

SOURCES := $(wildcard *.cc)
OBJECTS := $(SOURCES:%.cc=%.o)

OBJDIR = ../build/plp
OBJS = $(OBJECTS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all FORCE check clean run spotless

all: ../bin/plp

../bin/plp: $(OBJDIR) $(OBJS) FORCE | ../bin
	cd ../src && $(MAKE) static
	$(CXX) $(LDFLAGS) $(OBJS) $(LOADLIBES) $(LDLIBS) -o $@

FORCE:

$(OBJDIR) ../build:
	mkdir -p $@
../bin:
	mkdir -p $@

clean:
	rm -rf $(OBJDIR)
	if ! test "$$(ls -A ../build 2>/dev/null)"; \
	then                                        \
	    rm -rf ../build;                        \
	fi
spotless: clean
	rm -rf ../bin/plp
	if ! test "$$(ls -A ../bin 2>/dev/null)";   \
	then                                        \
	    rm -rf ../bin;                          \
	fi
-include $(OBJECTS:%.o=$(OBJDIR)/%.d)
