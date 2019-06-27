#!/bin/bash
# install-gcc-4.8.3.sh: install gcc-4.8.3 on Debian Squeeze

# See also https://gcc.gnu.org/wiki/InstallingGCC.

INSTALL_DIR=/usr/local/gcc-4.8.3

# Download prerequisites.
wget -c -i- <<EOF
ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-4.8.3/gcc-4.8.3.tar.bz2
https://ftp.gnu.org/gnu/gmp/gmp-6.0.0a.tar.bz2
http://www.mpfr.org/mpfr-current/mpfr-3.1.2.tar.bz2
http://www.mpfr.org/mpfr-current/allpatches
ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.2.tar.gz
ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.12.2.tar.bz2
ftp://gcc.gnu.org/pub/gcc/infrastructure/cloog-0.18.1.tar.gz
http://bugseng.com/products/ppl/download/ftp/releases/1.1/ppl-1.1.tar.bz2
EOF

# This is for gmp only according to hints in PPL configure --help.
CPPFLAGS=-fexceptions
export CPPFLAGS

tar jxvf gmp-6.0.0a.tar.bz2
cd gmp-6.0.0
./configure --prefix=$INSTALL_DIR --enable-cxx
make
make check
make install
cd ..
rm -rf gmp-6.0.0

unset CPPFLAGS

tar jxvf mpfr-3.1.2.tar.bz2
cd mpfr-3.1.2
patch -N -Z -p1 < ../allpatches
./configure --prefix=$INSTALL_DIR --with-gmp=$INSTALL_DIR
make
make check
make install
cd ..
rm -rf mpfr-3.1.2

tar zxvf mpc-1.0.2.tar.gz
cd mpc-1.0.2
./configure --prefix=$INSTALL_DIR --with-gmp=$INSTALL_DIR --with-mpfr=$INSTALL_DIR
make
make check
make install
cd ..
rm -rf mpc-1.0.2

tar jxvf isl-0.12.2.tar.bz2
cd isl-0.12.2
./configure --prefix=$INSTALL_DIR --with-gmp=$INSTALL_DIR
make
make install
cd ..
rm -rf isl-0.12.2

tar zxvf cloog-0.18.1.tar.gz
cd cloog-0.18.1
./configure --prefix=$INSTALL_DIR --with-gmp=$INSTALL_DIR --with-bits=gmp --with-isl=INSTALL_DIR --enable-cloog-backend=isl
make
make install
cd ..
rm -rf cloog-0.18.1

tar jxvf ppl-1.1.tar.bz2
cd ppl-1.1
./configure --prefix=$INSTALL_DIR --with-gmp=$INSTALL_DIR
make
make install
make check
cd ..
rm -rf ppl-1.1

tar jxvf gcc-4.8.3.tar.bz2
mkdir objdir
cd objdir
../gcc-4.8.3/configure \
    --prefix=$INSTALL_DIR \
    --with-gmp=$INSTALL_DIR \
    --with-mpfr=$INSTALL_DIR \
    --with-mpc=$INSTALL_DIR \
    --program-suffix=-4.8.3 \
    --enable-cloog-backend=isl \
    --with-ppl=$INSTALL_DIR \
    --with-cloog=$INSTALL_DIR
export LD_LIBRARY_PATH=$INSTALL_DIR/lib/
make
make install

# Do not forget to add
#
# LD_LIBRARY_PATH="/usr/local/gcc-4.8.3/lib:$LD_LIBRARY_PATH"
#
# in ~/.bashrc
#
# or add a file in /etc/ld.so.conf.d.
