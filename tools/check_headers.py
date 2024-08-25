#!/usr/bin/env python3

'This module tries to check if header files are self-sufficient.'

import os

def gcc(fname):
    '''Returns gcc command to compile file fname.'''
    cxx = 'g++-13.2.0 -O2 -MMD -g'
    cxx += ' -I../include'
    cxx += ' -isystem/usr/local/boost_1_84_0/include'
    cxx += ' -isystem/usr/local/include/eigen3'
    cxx += ' -std=c++20'
    cxx += ' -Wpedantic -Wall -Wextra'
    cxx += ' -Weffc++ -Wold-style-cast -Werror'
    cxx += ' ' + fname + ' -o' + os.path.splitext(fname)[0]
    return cxx

for filename in os.listdir('../include/shg'):
    if filename.endswith('-inl.h'):
        continue
    f = os.path.splitext(filename)[0]
    e = f
    cc = f + '.cc'
    d = f + '.d'
    with open(cc, 'w', encoding='utf-8') as f:
        s = f'#include <shg/{filename}>\nint main() {{}}\n'
        f.write(s)
    command = gcc(cc)
    status = os.system(command)
    if status != 0:
        print(f'error: {filename}')
    os.remove(e)
    os.remove(cc)
    os.remove(d)
