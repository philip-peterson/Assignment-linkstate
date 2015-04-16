#!/bin/bash
g++ -g -std=c++11 linkstate.c && gdb ./a.out -ex 'r hello.txt'
