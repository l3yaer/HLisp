CS=lisp.c types.c reader.c lexxer.c parser.c printer.c
CC=gcc
CFLAGS=-Wall -Wextra -Werror -ggdb -pedantic

.PHONY: all
	all: HLisp

HLisp:
	$(CC) $(CFLAGS) $(CS) -o HLisp

clear:
	rm HLisp
