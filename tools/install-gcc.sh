#!/bin/sh
# install-gcc.sh: install gcc${VERSION} on Debian Linux 9 (stretch)
#
# Change VERSION below to what is needed.
#
# If g++-6-multilib package is not already installed,
# --disable-multilib should be added to configure.
#
# Stanislaw Galus
# Sun Apr 21 19:57:41 CEST 2019

VERSION=-8.3.0

wget ftp://ftp.gnu.org/gnu/gcc/gcc${VERSION}/gcc${VERSION}.tar.xz
tar xvf gcc${VERSION}.tar.xz
cd gcc${VERSION}
./contrib/download_prerequisites
cd ..
mkdir objdir
cd objdir
../gcc${VERSION}/configure \
      --prefix=/usr/local/gcc${VERSION} \
      --program-suffix=${VERSION}
# This make takes about 163 min.
make
# This make takes about 165 min.
make -k check
make html
# These must be done as root.
make install
mkdir -p /usr/local/gcc${VERSION}/share/doc/html
mv gcc/HTML/gcc${VERSION}/* /usr/local/gcc${VERSION}/share/doc/html

# Create the file /etc/ld.so.conf.d/gcc${VERSION}.conf with lines:
# /usr/local/gcc-4.8.3/lib
# /usr/local/gcc-4.8.3/lib32
# /usr/local/gcc-4.8.3/lib64
# and run ldconfig.

# Create the file /etc/profile.d/gcc${VERSION}.sh with a line:
# export PATH="/usr/local/gcc${VERSION}/bin:$PATH"
# This also automatically adjusts paths for man.

