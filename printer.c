#include "printer.h"
#include <stdio.h>

static void print_atom(hlisp_atom *atom);

static void print_symbol(hlisp_atom *atom)
{
	printf("%s", atom->value.symbol);
}

static void print_string(hlisp_atom *atom)
{
	printf("%s", atom->value.string);
}

static void print_list(hlisp_atom *atom)
{
	printf("(");
	hlisp_list *node = atom->value.list;
	while (!NILP(node->value)) {
		print_atom(&node->value);
		if (!NILP(node->next->value))
			printf(" ");
		node = node->next;
	}
	printf(")");
}

static void print_integer(hlisp_atom *atom)
{
	printf("%ld", atom->value.integer);
}

static void print_float(hlisp_atom *atom)
{
	printf("%lf", atom->value.real);
}

static void print_bool(hlisp_atom *atom)
{
	if (atom->value.boolean == 0)
		printf("false");
	else
		printf("true");
}

static void print_nil(hlisp_atom *atom)
{
	(void)atom;
	printf("nil");
}

static void print_atom(hlisp_atom *atom)
{
	switch (atom->type) {
	case HLISP_ATOM_NIL:
		print_nil(atom);
		break;
	case HLISP_ATOM_BOOL:
		print_bool(atom);
		break;
	case HLISP_ATOM_SYMBOL:
		print_symbol(atom);
		break;
	case HLISP_ATOM_INTERGER:
		print_integer(atom);
		break;
	case HLISP_ATOM_FLOAT:
		print_float(atom);
		break;
	case HLISP_ATOM_LIST:
		print_list(atom);
		break;
	case HLISP_ATOM_STRING:
		print_string(atom);
		break;
	default:
		break;
	}
}

void pr_str(hlisp_atom *atom)
{
	print_atom(atom);
	printf("\n");
}