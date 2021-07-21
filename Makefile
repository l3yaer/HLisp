CS=lisp.c types.c reader.c printer.c htree.c env.c token.c parser.c
CC=gcc
CFLAGS=-Wall -Wextra -Werror -ggdb -pedantic
DFLAGS=-DHTREE_DEBUG -DDEBUG

.PHONY: all
	all: HLisp

HLisp:
	$(CC) $(CFLAGS) $(CS) -o HLisp

HLisp-debug:
	$(CC) $(CFLAGS) $(DFLAGS) $(CS) -o HLisp

clear:
	rm HLisp
