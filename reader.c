#include "reader.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "htree.h"
#include "token.h"
#include "types.h"
#include "parser.h"

typedef struct hlisp_reader {
	unsigned int pos;
	unsigned int tok_cap;
	unsigned int tok_count;
	hlisp_tok_t *tok;
} hlisp_reader_t;

hlisp_reader_t reader_make(unsigned int tok_cap)
{
	hlisp_reader_t reader;
	reader.pos = 0;
	reader.tok_count = 0;
	reader.tok_cap = tok_cap;
	reader.tok = (hlisp_tok_t *)malloc(tok_cap * sizeof(hlisp_tok_t));
	return reader;
}

hlisp_tok_t *reader_next(hlisp_reader_t *reader)
{
	if (reader->pos < reader->tok_count)
		return reader->tok + (reader->pos++);

	if (reader->tok_count >= reader->tok_cap)
		return NULL;

	++reader->tok_count;
	return reader->tok + (reader->pos++);
}

hlisp_tok_t *reader_peek(const hlisp_reader_t *reader)
{
	return reader->tok + reader->pos - 1;
}

struct hlisp_atom *read_str(const char *str, const size_t len,
			    const unsigned int n_tok)
{
	hlisp_reader_t reader = reader_make(n_tok);
	parse_tok(&reader, str, len);
	hlisp_atom_t *atom = read_form(&reader, str);
	free(reader.tok);
	return atom;
}

unsigned int reader_pos(const struct hlisp_reader *reader)
{
	return reader->pos;
}

struct hlisp_tok *reader_tok(const struct hlisp_reader *reader)
{
	return reader->tok;
}

void reader_reset(struct hlisp_reader *reader)
{
	reader->pos = 0;
}

unsigned int reader_max_pos(const struct hlisp_reader *reader)
{
	return reader->tok_count;
}
