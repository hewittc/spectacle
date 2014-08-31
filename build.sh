#!/bin/sh

clang -std=c11 -o spectacle -g -I/usr/local/include -lhackrf -lliquid *.c
