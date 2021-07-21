#include "env.h"
#include <stdlib.h>
#include "llist.h"
#include "htree.h"

struct env_list;

typedef struct env {
	struct env *parent;
	struct htree *sym_tbl;
	struct env_list *children;
} env_t;

LIST_IMPL_LIST(env_list, struct env)
LIST_IMPL_FN(env_list, struct env)

void add_child(env_t *parent, env_t *child)
{
	parent->children = env_list_append(parent->children, child);
}

env_t *env_make(env_t *parent)
{
	env_t *env = (env_t *)malloc(sizeof(env_t));
	env->parent = parent;
	env->sym_tbl = htree_make();
	env->children = NULL;
	if (parent != NULL)
		add_child(parent, env);

	return env;
}

void env_free_ht(struct env *env)
{
	htree_free(env->sym_tbl);
	env_list *child = env->children;
	while (child != NULL) {
		env_free_ht(child->value);
		child = child->next;
	}
}

void env_free(struct env *env)
{
	env_free_ht(env);

	if (env->parent != NULL)
		env_list_remove(env->parent->children, env);

	env_list_free(env->children);
	free(env);
}
