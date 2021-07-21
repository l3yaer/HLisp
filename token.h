#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <stddef.h>

struct hlisp_reader;

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
} hlisp_tok_err_t;

typedef struct hlisp_tok {
	hlisp_type_t type;
	long end;
	long start;
	long size;
	long parent;
} hlisp_tok_t;

int parse_tok(struct hlisp_reader *reader, const char *str, size_t len);

#endif //_TOKEN_H_
