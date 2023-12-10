SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .cc .o

ifndef clang
CXX = g++-13.2.0 -O2
WARNINGS = -Wpedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Werror
else
CXX = clang++-17 -O2 --gcc-toolchain=/usr/local/gcc-13.2.0
WARNINGS = -pedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Werror
endif

# Things you might add to DEFS:
# -DMCHECK              To debug memory allocation in GCC.
# -D_GLIBCXX_DEBUG      Use the libstdc++ debug mode.
# -DSHG_IEEE_EXCEPTIONS Enable traps for IEEE exceptions.

DEFS =

DEBUG = -g
INCLUDE = -I./include -isystem/usr/local/boost_1_83_0/include	\
-isystem/usr/local/include/eigen3
CXXFLAGS = -MMD $(DEBUG) $(INCLUDE) -std=c++20 $(WARNINGS) $(DEFS)
CPPFLAGS =
LDFLAGS = -static
LOADLIBES = -L./lib -L/usr/local/boost_1_83_0/lib
LDLIBS = -lshg -lgsl -lgslcblas -lboost_unit_test_framework	\
-lboost_iostreams -lboost_log -lboost_thread -lboost_filesystem	\
-lpugixml -lpthread -lz

AR = gcc-ar-13.2.0
ARFLAGS = rc
RANLIB = gcc-ranlib-13.2.0

%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

% : %.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
