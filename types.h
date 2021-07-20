#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdlib.h>

struct hlisp_list;

typedef enum {
	HLISP_ATOM_NIL,
	HLISP_ATOM_BOOL,
	HLISP_ATOM_SYMBOL,
	HLISP_ATOM_INTERGER,
	HLISP_ATOM_FLOAT,
	HLISP_ATOM_LIST,
	HLISP_ATOM_STRING,
} hlisp_atom_type_t;

typedef struct hlisp_atom {
	hlisp_atom_type_t type;

	union {
		char *symbol;
		long integer;
		double real;
		char *string;
		unsigned char boolean;
		struct hlisp_list *list;
	} value;
} hlisp_atom_t;

typedef struct hlisp_list {
	struct hlisp_atom *value;
	struct hlisp_list *next;
} hlisp_list_t;

#define NILP(x) ((x)->type == HLISP_ATOM_NIL)

struct hlisp_list *make_list(struct hlisp_atom *value);
void list_append(struct hlisp_list *list,struct hlisp_atom *value);

struct hlisp_atom *make_atom_symbol(const char *, size_t);
struct hlisp_atom *make_atom_integer(const long);
struct hlisp_atom *make_atom_float(const double);
struct hlisp_atom *make_atom_string(const char *, size_t);
struct hlisp_atom *make_atom_bool(const unsigned char);
struct hlisp_atom *make_atom_list();
struct hlisp_atom *make_atom_nil();

void atom_free(struct hlisp_atom *atom);

#endif //_TYPES_H_
