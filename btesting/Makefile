include ../make.inc

# "Using the GNU Compiler Collection (GCC)", chapter 3.15
# (https://gcc.gnu.org/onlinedocs/gcc-8.3.0/gcc/
# Directory-Options.html#Directory-Options) and "The C Preprocessor",
# chapter 2.8
# (https://gcc.gnu.org/onlinedocs/cpp/System-Headers.html#System-Headers).
# The option -isystem is used to suppress warnings from boost header
# files.

INCLUDE += -isystem/usr/local/boost_1_72_0/include
LOADLIBES += -L/usr/local/boost_1_72_0/lib
LDLIBS += -lboost_unit_test_framework

OBJECTS = testshg.o encoding.o csv.o

OBJDIR = ../build/btesting
OBJS = $(OBJECTS:%.o=$(OBJDIR)/%.o)

$(OBJDIR)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all FORCE check clean spotless

all: ../bin/btestshg

../bin/btestshg: $(OBJDIR) $(OBJS) FORCE | ../bin
	cd ../src && $(MAKE) static
	$(CXX) $(LDFLAGS) $(OBJS) $(LOADLIBES) $(LDLIBS) -o $@

FORCE:

bcheck: ../bin/btestshg
	../bin/btestshg

$(OBJDIR) ../build:
	mkdir -p $@
../bin:
	mkdir -p $@

clean:
	rm -rf $(OBJDIR)
	rm -f testshg.log mtrace
	if ! test "$$(ls -A ../build 2>/dev/null)"; \
	then                                        \
	    rm -rf ../build;                        \
	fi
spotless: clean
	rm -rf ../bin/btestshg
	if ! test "$$(ls -A ../bin 2>/dev/null)";   \
	then                                        \
	    rm -rf ../bin;                          \
	fi
-include $(OBJECTS:%.o=$(OBJDIR)/%.d)
