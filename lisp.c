#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"
#include "printer.h"
#include "types.h"
#include "env.h"
#include "natfn.h"

#define READ_BUFFER_SIZE 4098
#define TOKEN_BUFFER_SIZE 512

hlisp_atom_t *read(char *buffer)
{
	return read_str(buffer, READ_BUFFER_SIZE, TOKEN_BUFFER_SIZE);
}

hlisp_atom_t *eval(hlisp_atom_t *value, struct env *env)
{
	(void)env;
	return value;
}

void print(hlisp_atom_t *value)
{
	pr_str(value);
	atom_free(value);
}

void rep(char *buffer, struct env *env)
{
	print(eval(read(buffer), env));
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	struct env *global = env_make(NULL);
	env_set_C(global, "+", FN_C(add));
	env_set_C(global, "-", FN_C(sub));
	env_set_C(global, "*", FN_C(mul));
	env_set_C(global, "/", FN_C(div));
	for (;;) {
		char buffer[READ_BUFFER_SIZE];
		printf("user> ");
		if (fgets(buffer, READ_BUFFER_SIZE, stdin) == NULL)
			break;

		rep(buffer, global);
	}

	env_free(global);
	return 0;
}
