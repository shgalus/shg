include ../make.inc

SOURCES := $(wildcard *.cc)
OBJECTS := $(SOURCES:%.cc=%.o)

OBJDIR = ../build/testing
OBJS = $(OBJECTS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all FORCE check clean run spotless

all: ../bin/testing
	cp ../data/basic.swf /tmp
	cp ../data/solaris.txt /tmp
	cp ../data/chlopcy.txt /tmp

run: all
	../bin/testing --log_level=message			\
--run_test=lexan_test/*

../bin/testing: $(OBJDIR) $(OBJS) FORCE | ../bin
	cd ../src && $(MAKE) static
	$(CXX) $(LDFLAGS) $(OBJS) $(LOADLIBES) $(LDLIBS) -o $@

FORCE:

check: ../bin/testing
	../bin/testing --log_level=message

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
	rm -rf ../bin/testing
	if ! test "$$(ls -A ../bin 2>/dev/null)";   \
	then                                        \
	    rm -rf ../bin;                          \
	fi
-include $(OBJECTS:%.o=$(OBJDIR)/%.d)
