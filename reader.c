#include "reader.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ht.h"
#include "token.h"
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

typedef struct hlisp_tok {
	hlisp_type_t type;
	long end;
	long start;
	long size;
	long parent;
} hlisp_tok_t;

typedef struct hlisp_reader {
	unsigned int pos;
	unsigned int tok_cap;
	unsigned int tok_count;
	hlisp_tok_t *tok;
} hlisp_reader_t;

// ====================
// READER
// ====================

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
	return read_form(&reader, str);
}

// ====================
// LEXXER
// ====================

static hlisp_err_t lex(const char *str, size_t *start, size_t *end)
{
	const char *ws = " \t\n";
	*start += strspn(str, ws);
	if (str[(*start)] == '\0')
		return HLISP_ERR_PART;

	const char *pfx = "()";
	const char *dlm = "() \t\n";

	if (strchr(pfx, str[(*start)]) != NULL)
		*end = *start + 1;
	else
		*end = *start + strcspn(str + (*start), dlm);

	return HLISP_ERR_OK;
}

static hlisp_type_t read_number_type(const char *str, size_t start, size_t end)
{
	const char *flt = strchr(str + start, '.');
	if (flt == NULL || flt > (str + end))
		return HLISP_TOKEN_NUMBER_INT;
	return HLISP_TOKEN_NUMBER_FLOAT;
}

static hlisp_tok_t *make_token(hlisp_reader_t *reader, long start, long end,
			     hlisp_type_t type, long parent)
{
	hlisp_tok_t *tok = reader_next(reader);
	tok->start = start;
	tok->end = end;
	tok->size = end - start;
	tok->type = type;
	tok->parent = parent;
	return tok;
}

hlisp_err_t parse_tok(hlisp_reader_t *reader, const char *str, size_t len)
{
	hlisp_tok_t *curr_tok = NULL;
	long curr_tok_pos = -1;
	size_t start, end;
	int r = 0;
	for (size_t i = 0; i < len;) {
		char c;
		c = str[i];
		start = i;
		r = lex(str, &start, &end);

		switch (c) {
		case '\0':
			return HLISP_ERR_OK;
		case ' ':
		case '\t':
		case '\n':
		case ',':
		case ';':
			end = start + 1;
			break;
		case '"':
			if (r == HLISP_ERR_OK)
				make_token(reader, start, end,
					   HLISP_TOKEN_STRING, curr_tok_pos);
			break;
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (r == HLISP_ERR_OK)
				make_token(reader, start, end,
					   read_number_type(str, start, end),
					   curr_tok_pos);
			break;
		case '(':
			curr_tok = make_token(reader, start, end,
					      HLISP_TOKEN_LIST, curr_tok_pos);
			curr_tok_pos = reader->pos - 1;
			break;
		case ')':
			curr_tok->end = i;
			curr_tok->size = i - curr_tok->start;
			curr_tok_pos = curr_tok->parent;
			if (curr_tok_pos == -1)
				curr_tok = NULL;
			else
				curr_tok = reader->tok + curr_tok_pos;
			break;

		default:
			if (r != HLISP_ERR_OK)
				break;

			if (strncmp(str + start, "nil", 3) == 0) {
				make_token(reader, start, end, HLISP_TOKEN_NIL,
					   curr_tok_pos);
				break;
			}

			if (strncmp(str + start, "true", 4) == 0 ||
			    strncmp(str + start, "false", 5) == 0) {
				make_token(reader, start, end, HLISP_TOKEN_BOOL,
					   curr_tok_pos);
				break;
			}

			make_token(reader, start, end, HLISP_TOKEN_SYMBOL,
				   curr_tok_pos);
			break;
		}

		i = end;
	}
	return HLISP_ERR_OK;
}

// ====================
// PARSER
// ====================

static hlisp_atom_t *read_atom(hlisp_reader_t *reader, const char *str);

static hlisp_atom_t *read_symbol(hlisp_reader_t *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_symbol(str + tok->start, tok->size);
}

static hlisp_atom_t *read_string(hlisp_reader_t *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_string(str + tok->start, tok->size);
}

static hlisp_atom_t *read_list(hlisp_reader_t *reader, const char *str)
{
	hlisp_atom_t *atom = make_atom_list();
	while (reader->pos < reader->tok_count)
		list_append(atom->value.list, read_atom(reader, str));
	return atom;
}

static hlisp_atom_t *read_integer(hlisp_reader_t *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_integer(strtol(str + tok->start, NULL, 0));
}

static hlisp_atom_t *read_float(hlisp_reader_t *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_float(strtod(str + tok->start, NULL));
}

static hlisp_atom_t *read_bool(hlisp_reader_t *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_bool(strncmp(str + tok->start, "true", 4) == 0 ? 1 : 0);
}

static hlisp_atom_t *read_atom(hlisp_reader_t *reader, const char *str)
{
	hlisp_tok_t *tok = reader_next(reader);
	switch (tok->type) {
	case HLISP_TOKEN_SYMBOL:
		return read_symbol(reader, str);
	case HLISP_TOKEN_NIL:
		return make_atom_nil();
	case HLISP_TOKEN_NUMBER_INT:
		return read_integer(reader, str);
	case HLISP_TOKEN_NUMBER_FLOAT:
		return read_float(reader, str);
	case HLISP_TOKEN_STRING:
		return read_string(reader, str);
	case HLISP_TOKEN_LIST:
		return read_list(reader, str);
	case HLISP_TOKEN_BOOL:
		return read_bool(reader, str);
	default:
		break;
	}
	return NULL;
}

struct hlisp_atom *read_form(hlisp_reader_t *reader, const char *str)
{
	reader->pos = 0;
	return read_atom(reader, str);
}
