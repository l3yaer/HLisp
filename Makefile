CS=lisp.cpp
CC=g++
CFLAGS=-Wall -ggdb -std=c++0x

.PHONY: all
	all: HLisp

HLisp:
	$(CC) $(CFLAGS) $(CS) -o HLisp

clear:
	rm HLisp
