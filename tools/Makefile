# Makefile: description file for make in shg/tools directory

include ../make.inc

TARGET = ksone gmconsts octal genbuchb

GMP = -lgmpxx -lgmp

all: $(TARGET)

specfunc: specfunc.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
ksone: ksone.cc
gmconsts: gmconsts.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) $(GMP) -o $@
genbuchb: genbuchb.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) -lcocoa $(LDLIBS) $(GMP) -o $@
clean:
	rm -f $(TARGET:%=%.d)
spotless: clean
	rm -f $(TARGET)
