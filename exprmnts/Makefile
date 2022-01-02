# For Windows at home:
# CXX = /c/opt/Qt/Tools/mingw730_64/bin/g++ -O0
# Command line: /c/opt/Qt/Tools/mingw730_64/bin/mingw32-make.exe
#
# Eventually: PATH=/c/opt/Qt/Tools/mingw730_64/bin:$PATH; export PATH
DEFS =
CXX = g++-10.2.0 -O0
# CXX = /c/opt/Qt/Tools/mingw810_64/bin/g++ -O0
DEBUG = -g
INCLUDE = -I../include
WARNINGS = -Wpedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Werror
CXXFLAGS = $(DEBUG) $(INCLUDE) -std=c++17 $(WARNINGS) $(DEFS)
CPPFLAGS =
LDFLAGS =
LOADLIBES =
LDLIBS =
%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
%.o : ../src/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all top clean spotless

all: tree_test

run: all
	./tree_test

test: all
	valgrind --tool=memcheck --leak-check=yes tree_test

tree_test: tree_test.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

tree_test.o: tree_test.cc ../include/shg/tree.h ../include/shg/tree-inl.h

clean:
	rm -rf *.o

spotless: clean
	rm -rf tree_test
