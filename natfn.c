#include "natfn.h"
#include "types.h"

struct hlisp_atom *fn_add(struct hlisp_list *args)
{
	struct hlisp_list *head = args->next;
	double result = 0.0;
	unsigned char is_double = 0;
	while (head != NULL && head->value != NULL)
	{
		if(head->value->type == HLISP_ATOM_FLOAT)
		{
			is_double = 1;
			result += head->value->value.real;
		}
		else if(head->value->type == HLISP_ATOM_INTERGER)
			result += head->value->value.integer;
		head = head->next;
	}
	return is_double ? make_atom_float(result) : make_atom_integer((long) result);
}

struct hlisp_atom *fn_sub(struct hlisp_list *args)
{
	struct hlisp_list *head = args->next;
	double result = 0.0;
	unsigned char is_double = 0;
	while (head != NULL && head->value != NULL)
	{
		if(head->value->type == HLISP_ATOM_FLOAT)
		{
			is_double = 1;
			result -= head->value->value.real;
		}
		else if(head->value->type == HLISP_ATOM_INTERGER)
			result -= head->value->value.integer;
		head = head->next;
	}
	return is_double ? make_atom_float(result) : make_atom_integer((long) result);
}

struct hlisp_atom *fn_mul(struct hlisp_list *args)
{
	struct hlisp_list *head = args->next;
	double result = 0.0;
	unsigned char is_double = 0;
	while (head != NULL && head->value != NULL)
	{
		if(head->value->type == HLISP_ATOM_FLOAT)
		{
			is_double = 1;
			result *= head->value->value.real;
		}
		else if(head->value->type == HLISP_ATOM_INTERGER)
			result *= head->value->value.integer;
		head = head->next;
	}
	return is_double ? make_atom_float(result) : make_atom_integer((long) result);
}

struct hlisp_atom *fn_div(struct hlisp_list *args)
{
	struct hlisp_list *head = args->next;
	double result = 0.0;
	while (head != NULL && head->value != NULL)
	{
		if(head->value->type == HLISP_ATOM_FLOAT)
			result /= head->value->value.real;
		else if(head->value->type == HLISP_ATOM_INTERGER)
			result /= head->value->value.integer;
		head = head->next;
	}
	return make_atom_integer((long) result);
}
