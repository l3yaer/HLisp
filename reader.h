#ifndef _READER_H_
#define _READER_H_

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"

typedef enum {
	HLISP_TOKEN_SYMBOL = 0,
	HLISP_TOKEN_NIL = 1,
	HLISP_TOKEN_NUMBER_INT = 2,
	HLISP_TOKEN_NUMBER_FLOAT = 3,
	HLISP_TOKEN_STRING = 4,
	HLISP_TOKEN_LIST = 5,
	HLISP_TOKEN_BOOL = 6
} hlisp_type_t;

typedef enum {
	HLISP_ERR_OK = 0,
	HLISP_ERR_NOMEM = -1,
	HLISP_ERR_INVAL = -2,
	HLISP_ERR_PART = -3,
} hlisp_err_t;

typedef struct _hlisp_tok {
	hlisp_type_t type;
	long end;
	long start;
	long size;
	long parent;
} hlisp_tok;

typedef struct _hlisp_reader {
	unsigned int pos;
	unsigned int tok_cap;
	unsigned int tok_count;
	hlisp_tok *tok;
	hlisp_atom root;
} hlisp_reader;

hlisp_reader reader_make(unsigned int tok_cap);
hlisp_tok *reader_peek(const hlisp_reader *reader);
hlisp_tok *reader_next(hlisp_reader *reader);
void reader_free(hlisp_reader *reader);

hlisp_atom read_str(const char *str, const size_t len,
		      const unsigned int n_tok);
hlisp_err_t parse_tok(hlisp_reader *reader, const char *str, size_t len);
hlisp_atom *read_form(hlisp_reader *reader, const char *str);

#endif //_READER_H_
