CS=lisp.c
CC=gcc
CFLAGS=-Wall -ggdb -Wextra

.PHONY: all
	all: HLisp

HLisp:
	$(CC) $(CFLAGS) $(CS) -o HLisp

clear:
	rm HLisp
