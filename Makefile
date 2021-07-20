CS=lisp.c types.c reader.c lexxer.c parser.c printer.c ht.c
CC=gcc
CFLAGS=-Wall -Wextra -Werror -ggdb -pedantic
DFLAGS=-DHT_DEBUG -DDEBUG

.PHONY: all
	all: HLisp

HLisp:
	$(CC) $(CFLAGS) $(CS) -o HLisp

HLisp-debug:
	$(CC) $(CFLAGS) $(DFLAGS) $(CS) -o HLisp

clear:
	rm HLisp
