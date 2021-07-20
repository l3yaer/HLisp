#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "printer.h"
#include "types.h"
#include <string.h>

#define READ_BUFFER_SIZE 4098
#define TOKEN_BUFFER_SIZE 512

hlisp_atom_t *read(char *buffer)
{
	return read_str(buffer, READ_BUFFER_SIZE, TOKEN_BUFFER_SIZE);
}

hlisp_atom_t *eval(hlisp_atom_t *value)
{
	return value;
}

void print(hlisp_atom_t *value)
{
	pr_str(value);
	atom_free(value);
}

void rep(char *buffer)
{
	print(eval(read(buffer)));
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	for (;;) {
		char buffer[READ_BUFFER_SIZE];
		printf("user> ");
		if (fgets(buffer, READ_BUFFER_SIZE, stdin) == NULL)
			return 0;

		rep(buffer);
	}
	return 0;
}
