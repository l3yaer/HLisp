#ifndef _ENV_H_
#define _ENV_H_

struct hlisp_atom;
struct hlisp_list;
struct env;

struct env *env_make(struct env *parent);
void env_free(struct env *env);

struct env *env_set(struct env *env, struct hlisp_atom *sym,
		    struct hlisp_atom *val);
struct env *env_set_C(struct env *env, const char *sym,
		      struct hlisp_atom *(*fn)(struct hlisp_list *));
struct hlisp_atom *env_get(const struct env *env, struct hlisp_atom *sym);

#endif //_ENV_H_
