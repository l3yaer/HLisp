#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <stddef.h>

struct hlisp_tok;
struct hlisp_reader;

int parse_tok(struct hlisp_reader *reader, const char *str, size_t len);

#endif //_TOKEN_H_
