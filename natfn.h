#ifndef _NAT_FN_H_
#define _NAT_FN_H_

struct hlisp_atom;
struct hlisp_list;

#define FN_C_DECL(name) struct hlisp_atom *fn_##name(struct hlisp_list *args);
#define FN_C(name) fn_##name

FN_C_DECL(add)
FN_C_DECL(sub)
FN_C_DECL(mul)
FN_C_DECL(div)
#endif //_NAT_FN_H_
