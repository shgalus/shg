include ../make.inc

INCLUDE += -isystem/usr/local/boost_1_73_0/include
LOADLIBES += -L/usr/local/boost_1_73_0/lib
LDLIBS += -lboost_unit_test_framework

SOURCES := $(wildcard *.cc)
OBJECTS := $(SOURCES:%.cc=%.o)

OBJDIR = ../build/testing
OBJS = $(OBJECTS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all FORCE check clean run spotless

all: ../bin/testing

run: all
	../bin/testing --log_level=message --run_test=packellp_test/*

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
