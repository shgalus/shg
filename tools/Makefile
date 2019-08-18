# Makefile: description file for make in shg/tools directory

include ../make.inc

TARGET = specfunc ksone gmconsts gstrdlrd octal

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
