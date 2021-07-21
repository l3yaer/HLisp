#include "parser.h"
#include "reader.h"
#include "token.h"
#include "types.h"

static hlisp_atom_t *read_atom(struct hlisp_reader *reader, const char *str);

static hlisp_atom_t *read_symbol(struct hlisp_reader *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_symbol(str + tok->start, tok->size);
}

static hlisp_atom_t *read_string(struct hlisp_reader *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_string(str + tok->start, tok->size);
}

static hlisp_atom_t *read_list(struct hlisp_reader *reader, const char *str)
{
	hlisp_atom_t *atom = make_atom_list();
	while (reader_pos(reader) < reader_max_pos(reader)) {
		hlisp_atom_t *val = read_atom(reader, str);
		atom->value.list = hlisp_list_append(atom->value.list, val);
		free(val);
	}
	return atom;
}

static hlisp_atom_t *read_integer(struct hlisp_reader *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_integer(strtol(str + tok->start, NULL, 0));
}

static hlisp_atom_t *read_float(struct hlisp_reader *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_float(strtod(str + tok->start, NULL));
}

static hlisp_atom_t *read_bool(struct hlisp_reader *reader, const char *str)
{
	hlisp_tok_t *tok = reader_peek(reader);
	return make_atom_bool(strncmp(str + tok->start, "true", 4) == 0 ? 1 :
										0);
}

static hlisp_atom_t *read_atom(struct hlisp_reader *reader, const char *str)
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

struct hlisp_atom *read_form(struct hlisp_reader *reader, const char *str)
{
	reader_reset(reader);
	return read_atom(reader, str);
}
