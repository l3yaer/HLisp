#include "reader.h"
#include <stddef.h>
#include <string.h>
#include <ctype.h>

static hlisp_atom read_atom(hlisp_reader *reader, const char *str);

static char *copy_str(const char *str, size_t n)
{
	char *new_str = (char *)malloc(n + 1);
	strncpy(new_str, str, n);
	new_str[n] = '\0';
	return new_str;
}

static void to_upper_str(char *str, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		str[i] = toupper(str[i]);
}

static hlisp_atom read_symbol(hlisp_reader *reader, const char *str)
{
	hlisp_tok *tok = reader_peek(reader);
	hlisp_atom atom;
	atom.type = HLISP_ATOM_SYMBOL;
	atom.value.symbol = copy_str(str + tok->start, tok->size);
	to_upper_str(atom.value.symbol, tok->size);
	return atom;
}

static hlisp_atom read_string(hlisp_reader *reader, const char *str)
{
	hlisp_tok *tok = reader_peek(reader);
	hlisp_atom atom;
	atom.type = HLISP_ATOM_STRING;
	atom.value.string = copy_str(str + tok->start, tok->size);
	return atom;
}

static hlisp_atom read_list(hlisp_reader *reader, const char *str)
{
	hlisp_atom atom;
	atom.type = HLISP_ATOM_LIST;
	atom.value.list = make_list(nil);
	while (reader->pos < reader->tok_count) {
		list_append(atom.value.list, read_atom(reader, str));
	}
	return atom;
}

static hlisp_atom read_integer(hlisp_reader *reader, const char *str)
{
	hlisp_tok *tok = reader_peek(reader);
	hlisp_atom atom;
	atom.type = HLISP_ATOM_INTERGER;
	atom.value.integer = strtol(str + tok->start, NULL, 0);
	return atom;
}

static hlisp_atom read_float(hlisp_reader *reader, const char *str)
{
	hlisp_tok *tok = reader_peek(reader);
	hlisp_atom atom;
	atom.type = HLISP_ATOM_FLOAT;
	atom.value.real = strtod(str + tok->start, NULL);
	return atom;
}

static hlisp_atom read_bool(hlisp_reader *reader, const char *str)
{
	hlisp_tok *tok = reader_peek(reader);
	hlisp_atom atom;
	atom.type = HLISP_ATOM_BOOL;
	atom.value.boolean = strncmp(str + tok->start, "true", 4) == 0 ? 1 : 0;
	return atom;
}

static hlisp_atom read_atom(hlisp_reader *reader, const char *str)
{
	hlisp_tok *tok = reader_next(reader);
	switch (tok->type) {
	case HLISP_TOKEN_SYMBOL:
		return read_symbol(reader, str);
	case HLISP_TOKEN_NIL:
		return nil;
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
	return nil;
}

hlisp_atom *read_form(hlisp_reader *reader, const char *str)
{
	reader->pos = 0;
	reader->root = read_atom(reader, str);
	return &reader->root;
}
