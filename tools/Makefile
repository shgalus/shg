# Makefile: description file for make in shg/tools directory

include ../make.inc

TARGET = ksone gmconsts octal

GMP = -lgmpxx -lgmp

all: $(TARGET)

specfunc: specfunc.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
ksone: ksone.cc
gmconsts: gmconsts.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) $(GMP) -o $@
clean:
	rm -f $(TARGET) $(TARGET:%=%.d)
spotless: clean
	rm -f ppnd7.cc gammad.cc probst.cc betain.cc cdffdist.cc
