#include "reader.h"

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

static hlisp_tok *make_token(hlisp_reader *reader, long start, long end,
			     hlisp_type_t type, long parent)
{
	hlisp_tok *tok = reader_next(reader);
	tok->start = start;
	tok->end = end;
	tok->size = end - start;
	tok->type = type;
	tok->parent = parent;
	return tok;
}

hlisp_err_t parse_tok(hlisp_reader *reader, const char *str, size_t len)
{
	hlisp_tok *curr_tok = NULL;
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
