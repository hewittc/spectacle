#!/bin/sh

clang -o spectacle -I/usr/local/include/libhackrf -lhackrf *.c
