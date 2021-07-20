#ifndef _ENV_H_
#define _ENV_H_

struct env;

struct env *env_make(struct env *parent);
void env_free(struct env *env);

#endif //_ENV_H_
