#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdlib.h>

struct _hlisp_pair;

struct _hlisp_list;

typedef enum {
    HLISP_ATOM_NIL,
    HLISP_ATOM_BOOL,
    HLISP_ATOM_SYMBOL,
    HLISP_ATOM_INTERGER,
    HLISP_ATOM_FLOAT,
    HLISP_ATOM_LIST,
    HLISP_ATOM_STRING,
} hlisp_atom_type_t;

typedef struct _hlisp_atom
{
    hlisp_atom_type_t type;

    union {
        char *symbol;
        long integer;
        double real;
        char *string;
        unsigned char boolean;
        struct _hlisp_list *list;
    } value;
} hlisp_atom;

typedef struct _hlisp_list
{
    hlisp_atom value;
    struct _hlisp_list *next;
} hlisp_list;

#define NILP(x) ((x).type == HLISP_ATOM_NIL)

hlisp_list *make_list(const hlisp_atom value);
void list_append(hlisp_list *list, const hlisp_atom value);

void free_atom(hlisp_atom *atom);

extern const hlisp_atom nil;

#endif //_TYPES_H_