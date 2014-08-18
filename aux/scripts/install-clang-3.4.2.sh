#!/bin/bash
# install-clang-3.4.2.sh: install clang 3.4.2

# Download prerequisites.
wget -c -i- <<EOF
http://llvm.org/releases/3.4.2/cfe-3.4.2.src.tar.gz
http://llvm.org/releases/3.4.2/llvm-3.4.2.src.tar.gz
http://llvm.org/releases/3.4/compiler-rt-3.4.src.tar.gz
http://llvm.org/releases/3.4/dragonegg-3.4.src.tar.gz
http://llvm.org/releases/3.4.2/libcxx-3.4.2.src.tar.gz
http://llvm.org/releases/3.4/lldb-3.4.src.tar.gz
http://llvm.org/releases/3.4/lld-3.4.src.tar.gz
http://llvm.org/releases/3.4/polly-3.4.src.tar.gz
http://llvm.org/releases/3.4/clang-tools-extra-3.4.src.tar.gz
http://llvm.org/releases/3.4/test-suite-3.4.src.tar.gz
EOF

tar zxvf llvm-3.4.2.src.tar.gz
tar zxvf cfe-3.4.2.src.tar.gz
tar zxvf compiler-rt-3.4.src.tar.gz
tar zxvf clang-tools-extra-3.4.src.tar.gz
mv llvm-3.4.2.src llvm
mv cfe-3.4.2.src llvm/tools/clang
mv compiler-rt-3.4 llvm/projects/compiler-rt
mv clang-tools-extra-3.4 llvm/tools/clang/tools/extra
mkdir build
cd build
../llvm/configure --prefix=/usr/local/clang \
    --with-gcc-toolchain=/usr/local/gcc-4.8.3
make
make install
