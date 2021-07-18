#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "printer.h"

#define READ_BUFFER_SIZE 4098
#define TOKEN_BUFFER_SIZE 512

hlisp_atom read(char *buffer)
{
	hlisp_reader r = read_str(buffer, READ_BUFFER_SIZE, TOKEN_BUFFER_SIZE);
	return r.root;
}

hlisp_atom eval(hlisp_atom value)
{
	return value;
}

void print(hlisp_atom value)
{
	pr_str(&value);
	free_atom(&value);
}

void rep(char *buffer)
{
	print(read(eval(buffer)));
}

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	for(;;)
	{
		char buffer[READ_BUFFER_SIZE];
		printf("user> ");
		fgets(buffer, READ_BUFFER_SIZE, stdin);

		if(buffer[0] == EOF)
			break;

		rep(buffer);
	}
	return 0;
}
