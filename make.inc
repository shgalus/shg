SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .cc .o

ifndef clang
CXX = g++-16.2.0 -O2
WARNINGS = -Wpedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Werror
else
CXX = clang++-22 -O2 --gcc-toolchain=/usr/local/gcc-16.2.0
WARNINGS = -pedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Werror -Wno-error=character-conversion -Wno-c2y-extensions
endif

# -Wno-c2y-extensions above was added on 26 August 2026 due to an
# issue in clang++-22.

ifdef ubsan
UBSAN = -fsanitize=undefined
endif

# Things you might add to DEFS:
# -DMCHECK              To debug memory allocation in GCC.
# -D_GLIBCXX_DEBUG      Use the libstdc++ debug mode.
# -DSHG_IEEE_EXCEPTIONS Enable traps for IEEE exceptions.

DEFS =

DEBUG = -g $(UBSAN)
INCLUDE = -I./include -isystem/usr/local/boost_1_92_0/include	\
-isystem/usr/local/include/eigen3
CXXFLAGS = -MMD $(DEBUG) $(INCLUDE) -std=c++20 $(WARNINGS) $(DEFS)
CPPFLAGS =
LDFLAGS = -static
LOADLIBES = -L./lib -L/usr/local/boost_1_92_0/lib
LDLIBS = -lshg -lgsl -lgslcblas -lboost_unit_test_framework	\
-lboost_iostreams -lboost_log -lboost_thread -lboost_filesystem	\
-lpugixml -lpthread -lz

ifdef ubsan
LDLIBS += -lubsan
endif

AR = gcc-ar-16.2.0
ARFLAGS = rc
RANLIB = gcc-ranlib-16.2.0

%.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

% : %.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
