#!/bin/bash
# checksrc.sh: check source files

ALLSOURCE="include/shg/*.h src/*.cc testing/*.h testing/*.cc tools/*.cc"\
" examples/*.cc"

# Things you might add to FILTER:
# legal/copyright       no copyright message found
# build/header_guard    header guard has wrong style
# runtime/references    use only const references
# whitespace/indent     indent public/protected/private
# build/c++11           rvalue references are unapproved
# build/include         includes are not in alphabetical order
# readability/streams   streams are discouraged
# runtime/int           do not use long, short
# runtime/printf        use snprintf instead of sprintf
# runtime/arrays        do not use char[maxn + 1]

FILTER="-legal/copyright,"\
"-build/header_guard,"\
"-runtime/references,"\
"-whitespace/indent,"\
"-build/c++11,"\
"-build/include,"\
"-readability/streams,"\
"-runtime/int,"\
"-runtime/printf,"\
"-runtime/arrays"

if [ $# -eq 0 ]
then
    SOURCE=$ALLSOURCE
else
    SOURCE=$*
fi

echo "checking consistency of version numbers"
SUBST='s/^\([^0-9]*\)\([.0-9]*\)\(.*$\)/\2/'
V1=`sed -n "/SHG_VERSION \"/ $SUBST p" include/shg/version.h`
V2=`sed -n "/^PROJECT_NUMBER  *= / $SUBST p" doc/Doxyfile`
V3=`sed -n "/^This directory contains the / $SUBST p" README`
if [ -z $V1 ] || [ -z $V2 ] || [ -z $V3 ] || [ $V1 != $V2 ] ||\
 [ $V1 != $V3 ]
then
    echo "inconsistent version numbers"
fi

echo "checking for non-ASCII characters"
grep -P '[\x80-\xff]' $SOURCE

echo "checking for lines longer than 72 characters"
awk 'length($0) > 72 {print FILENAME, FNR}' $SOURCE

echo "checking for white space at end of lines"
grep '[[:space:]]$' $SOURCE

echo "checking for consecutive blank lines"
awk '
BEGINFILE {nb = 0}
NF == 0 {if (++nb > 1) print "Consecutive blank lines:", FILENAME, FNR}
NF > 0 {nb = 0}
' $SOURCE

echo "checking for badly formatted cite commands"
grep '\\cite$' $SOURCE

echo "checking for #define and #include containing spaces"
grep '#[[:space:]][[:space:]]*define' $SOURCE
grep '#[[:space:]][[:space:]]*include' $SOURCE

echo "checking for fabs, assert, pragma"
grep '\<fabs\>' $SOURCE
grep '\<assert\>' $SOURCE
grep '\<pragma\>' $SOURCE

echo "checking for tabs"
grep '	' $SOURCE

cpplint --linelength=72 --filter=$FILTER $SOURCE
