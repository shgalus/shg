SHELL = /bin/sh

# Things you might add to DEFS:
# -DMCHECK              To debug memory allocation in GCC.
# -D_GLIBCXX_DEBUG      Use the libstdc++ debug mode.
# -DSHG_IEEE_EXCEPTIONS Enable traps for IEEE exceptions.

DEFS = -DMCHECK -DSHG_IEEE_EXCEPTIONS

CXX = g++-11.2.0 -O2
DEBUG = -g
INCLUDE = -I../include -isystem/usr/local/boost_1_78_0/include
WARNINGS = -Wpedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Werror
CXXFLAGS = -MMD $(DEBUG) $(INCLUDE) -std=c++17 $(WARNINGS) $(DEFS)
CPPFLAGS =
LDFLAGS = -static
LOADLIBES = -L../lib -L/usr/local/boost_1_78_0/lib
LDLIBS = -lshg -lgsl -lgslcblas -lboost_unit_test_framework	\
-lboost_iostreams -lpugixml -lz

AR = gcc-ar-11.2.0
ARFLAGS = rc
RANLIB = gcc-ranlib-11.2.0

%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

% : %.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
