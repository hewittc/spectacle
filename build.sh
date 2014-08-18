#!/bin/sh

clang -o spectacle -g -I/usr/local/include/libhackrf -lhackrf *.c
