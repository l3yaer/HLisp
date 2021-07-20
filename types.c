#include "types.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

LL_IMPL_FN(hlisp_list, struct hlisp_atom)

void free_list(hlisp_atom_t *list);
void free_symbol(hlisp_atom_t *symbol);
void free_string(hlisp_atom_t *string);


static void to_upper_str(char *str, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		str[i] = toupper(str[i]);
}

hlisp_atom_t *make_atom_symbol(const char * symbol, size_t len)
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->value.symbol = (char *) malloc(len + 1);
	memcpy(a->value.symbol, symbol, len);
	a->value.symbol[len] = '\0';
	to_upper_str(a->value.symbol, len);
	a->type = HLISP_ATOM_SYMBOL;
	return a;
}

hlisp_atom_t *make_atom_integer(const long value)
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->value.integer = value;
	a->type = HLISP_ATOM_INTERGER;
	return a;
}

hlisp_atom_t *make_atom_float(const double value)
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->value.real = value;
	a->type = HLISP_ATOM_FLOAT;
	return a;
}

hlisp_atom_t *make_atom_string(const char *string, size_t len)
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->value.string = (char *) malloc(len + 1);
	memcpy(a->value.string, string, len);
	a->value.string[len] = '\0';
	a->type = HLISP_ATOM_STRING;
	return a;
}

hlisp_atom_t *make_atom_bool(const unsigned char value)
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->value.boolean = value;
	a->type = HLISP_ATOM_BOOL;
	return a;
}

hlisp_atom_t *make_atom_list()
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->value.list = hlisp_list_make(NULL);
	a->type = HLISP_ATOM_LIST;
	return a;
}

hlisp_atom_t *make_atom_nil()
{
	hlisp_atom_t *a = (hlisp_atom_t *) malloc(sizeof(hlisp_atom_t));
	a->type = HLISP_ATOM_NIL;
	return a;
}

void free_list(hlisp_atom_t *list)
{
	struct hlisp_list *node = list->value.list;
	while (node != NULL) {
		atom_free(node->value);
		node = node->next;
	}

	free(list->value.list);
}

void free_symbol(hlisp_atom_t *symbol)
{
	free(symbol->value.symbol);
}

void free_string(hlisp_atom_t *string)
{
	free(string->value.string);
}

void atom_free(hlisp_atom_t *atom)
{
	if(atom == NULL)
		return;
	switch (atom->type) {
	case HLISP_ATOM_NIL:
	case HLISP_ATOM_BOOL:
	case HLISP_ATOM_INTERGER:
	case HLISP_ATOM_FLOAT:
		break;
	case HLISP_ATOM_LIST:
		free_list(atom);
		break;
	case HLISP_ATOM_SYMBOL:
		free_symbol(atom);
		break;
	case HLISP_ATOM_STRING:
		free_string(atom);
		break;
	}
	free(atom);
}
