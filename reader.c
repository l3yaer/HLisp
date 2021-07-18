#include "reader.h"

hlisp_reader reader_make(unsigned int tok_cap)
{
    hlisp_reader reader;
    reader.pos = 0;
    reader.tok_count = 0;
    reader.tok_cap = tok_cap;
    reader.tok = (hlisp_tok *) malloc(tok_cap * sizeof(hlisp_tok));
    return reader;
}

hlisp_tok *reader_next(hlisp_reader *reader)
{
    if(reader->pos < reader->tok_count)
        return reader->tok + (reader->pos++);

    if(reader->tok_count >= reader->tok_cap)
        return NULL;

    ++reader->tok_count;
    return reader->tok + (reader->pos++);
}

hlisp_tok *reader_peek(const hlisp_reader *reader)
{
    return reader->tok + reader->pos - 1;
}

hlisp_reader read_str(const char *str, const size_t len, const unsigned int n_tok)
{
    hlisp_reader reader = reader_make(n_tok);
    parse_tok(&reader, str, len);
    read_form(&reader, str);
    return reader;
}