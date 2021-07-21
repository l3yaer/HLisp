#ifndef _READER_H_
#define _READER_H_
#include <stddef.h>

struct hlisp_reader;
struct hlisp_tok;
struct hlisp_atom;

struct hlisp_tok *reader_peek(const struct hlisp_reader *reader);
struct hlisp_tok *reader_next(struct hlisp_reader *reader);

unsigned int reader_pos(const struct hlisp_reader *reader);
unsigned int reader_max_pos(const struct hlisp_reader *reader);
struct hlisp_tok *reader_tok(const struct hlisp_reader *reader);
void reader_reset(struct hlisp_reader *reader);

struct hlisp_atom *read_str(const char *str, const size_t len,
			    const unsigned int n_tok);

#endif //_READER_H_
