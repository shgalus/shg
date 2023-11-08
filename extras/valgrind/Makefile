include ../../make.inc

INCLUDE = -I../../include
LOADLIBES =
LDLIBS =

OBJECTS = tree.o

.PHONY: all run clean spotless

all: tree

run: all
	valgrind --tool=memcheck --leak-check=yes tree

tree: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -rf *.d *.o

spotless: clean
	rm -rf tree

-include $(OBJECTS:%.o=%.d)
