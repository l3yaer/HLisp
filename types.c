#include "types.h"

const hlisp_atom nil = { HLISP_ATOM_NIL };

void free_list(hlisp_atom *list);
void free_symbol(hlisp_atom *symbol);
void free_string(hlisp_atom *string);

hlisp_list *make_list(const hlisp_atom value)
{
	hlisp_list *l = (hlisp_list *)malloc(sizeof(hlisp_list));
	l->value = value;
	l->next = NULL;
	return l;
}

void list_append(hlisp_list *list, const hlisp_atom value)
{
	hlisp_list *node = list;
	while (!NILP(node->value))
		node = node->next;

	node->value = value;
	node->next = make_list(nil);
}

void free_list(hlisp_atom *list)
{
	hlisp_list *node = list->value.list;
	while (node != NULL) {
		atom_free(&node->value);
		node = node->next;
	}

	free(list->value.list);
}

void free_symbol(hlisp_atom *symbol)
{
	free(symbol->value.symbol);
}

void free_string(hlisp_atom *string)
{
	free(string->value.string);
}

void atom_free(hlisp_atom *atom)
{
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
}
