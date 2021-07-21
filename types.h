#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdlib.h>
#include "llist.h"

struct hlisp_atom;

LIST_DECL(hlisp_list, struct hlisp_atom)

typedef struct hlisp_atom * (*c_fn)(struct hlisp_list*);

struct hlisp_list;

typedef enum {
	HLISP_ATOM_NIL,
	HLISP_ATOM_BOOL,
	HLISP_ATOM_SYMBOL,
	HLISP_ATOM_INTERGER,
	HLISP_ATOM_FLOAT,
	HLISP_ATOM_LIST,
	HLISP_ATOM_STRING,
	HLISP_ATOM_NATIVE_FN
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
		c_fn nat_fn;
	} value;
} hlisp_atom_t;

LIST_IMPL_LIST(hlisp_list, struct hlisp_atom)

#define NILP(x) ((x) == NULL || (x)->type == HLISP_ATOM_NIL)

struct hlisp_atom *make_atom_symbol(const char *, size_t);
struct hlisp_atom *make_atom_integer(const long);
struct hlisp_atom *make_atom_float(const double);
struct hlisp_atom *make_atom_string(const char *, size_t);
struct hlisp_atom *make_atom_bool(const unsigned char);
struct hlisp_atom *make_atom_list();
struct hlisp_atom *make_atom_nil();
struct hlisp_atom *make_atom_fn_C(c_fn);

void atom_free(struct hlisp_atom *atom);

#endif //_TYPES_H_
