#!/bin/sh

# Call this script from directory containing include directory!

INCLUDE="-I./include -isystem/usr/local/boost_1_87_0/include -isystem/usr/local/include/eigen3"

for var in $@
do
    clang-tidy-19 -extra-arg-before=-xc++ $var -- -std=c++20 $INCLUDE
done
