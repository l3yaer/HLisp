#ifndef _HT_H_
#define _HT_H_

#include <stddef.h>

struct ht;

struct ht *ht_make();
void ht_free(struct ht *ht);
void ht_add(struct ht *ht, const char *key, const void *val, size_t val_size);
void *ht_get(struct ht *ht, const char *key);

#ifdef HT_DEBUG
void ht_dump(const struct ht *ht);
#define HT_PRINT(x) ht_dump(x);
#else
#define HT_PRINT(x)
#endif

#endif //_HT_H_
