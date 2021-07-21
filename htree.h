#ifndef _HTREE_H_
#define _HTREE_H_

#include <stddef.h>

struct htree;

struct htree *htree_make();
void htree_free(struct htree *htree);
void htree_add(struct htree *htree, const char *key, const void *val, size_t val_size);
void *htree_get(struct htree *htree, const char *key);

#ifdef HTREE_DEBUG
void htree_dump(const struct htree *htree);
#define HTREE_PRINT(x) htree_dump(x);
#else
#define HTREE_PRINT(x)
#endif

#endif //_HTREE_H_
