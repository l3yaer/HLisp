#ifndef _PARSER_H_
#define _PARSER_H_

struct hlisp_atom;
struct hlisp_reader;

struct hlisp_atom *read_form(struct hlisp_reader *reader, const char *str);

#endif //_PARSER_H_
