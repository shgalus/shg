#!/bin/bash
# addmod.sh: add a new module to the shg library

if [ $# -ne 1 ]
then
    echo "This command adds a new module to the shg library."
    echo -n "It requires exactly one parameter: "
    echo "a new module name."
    exit 1;
fi

a=$1
A=`echo $a | tr "[:lower:]" "[:upper:]"`

#######################################################################
# Create the header file.
#
echo "Creating include/shg/$a.h."
cat <<EOF > include/shg/$a.h
/* $a.h: xxxxx */

/**
 * \file include/shg/$a.h
 * Xxxxx.
 * Created on @insert_date@.
 */

#ifndef SHG_${A}_H
#define SHG_${A}_H

namespace SHG {

}       // namespace SHG

#endif
EOF

#######################################################################
# Create the source file.
#
echo "Creating src/$a.cc."
cat <<EOF > src/$a.cc
/* $a.cc: xxxxx */

/**
 * \file src/$a.cc
 * Xxxxx.
 * Created on @insert_date@.
 */

#include "shg/$a.h"

namespace SHG {

}       // namespace SHG
EOF

#######################################################################
# Create the source file for testing.
#
echo "Creating testing/$a.cc."
cat <<EOF > testing/$a.cc
/* date.cc: testing xxxxx */

/**
 * \file testing/$a.cc
 * Testing xxxxx.
 * Created on @insert_date@.
 */

#include "shg/$a.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

}       // anonymous namespace

void test_$a() {

}

}       // namespace Testing
}       // namespace SHG
EOF

sed -i "s/@insert_date@/`date '+%e %B %Y'`/" \
include/shg/$a.h src/$a.cc testing/$a.cc

echo "Add these keeping alphabetic order:"
echo "$a.h to include/shg/shg.h."
echo "$a.o to OBJECTS in src/Makefile."
echo "$a.o to OBJECTS in testing/Makefile."
echo "void test_$a(); in testing/testshg.h."
echo "register_test(test_$a, \"$a\"); in testing/testshg.cc."
