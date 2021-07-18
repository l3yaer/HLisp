#include <stdio.h>
#include <stdlib.h>

#define READ_BUFFER_SIZE 4098

char *read(char *buffer)
{
	return buffer;
}

char *eval(char *buffer)
{
	return buffer;
}

char *print(char *buffer)
{
	return buffer;
}

void rep(char *buffer)
{
	char *p = print(read(eval(buffer)));
	printf("%s", p);
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
