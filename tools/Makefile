include ../make.inc

INCLUDE = -I../include
LOADLIBES = -L../lib -L/usr/local/boost_1_83_0/lib
GMP = -lgmpxx -lgmp

TARGET = ksone gmconsts octal genbuchb

all: $(TARGET)

specfunc: specfunc.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
ksone: ksone.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
gmconsts: gmconsts.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) $(GMP) -o $@
genbuchb: genbuchb.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) -lcocoa $(LDLIBS) $(GMP) -o $@

clean:
	rm -f $(TARGET:%=%.d)
spotless: clean
	rm -f $(TARGET)
