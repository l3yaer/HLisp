#include "ht.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// TODO: Make it dynamic maybe https://attractivechaos.wordpress.com/2009/09/29/khash-h/
#define TABLE_SIZE 10000

typedef struct _ht_elem {
	char *key;
	void *val;
#ifdef HT_DEBUG
	size_t val_size;
#endif
	struct _ht_elem *next;
} ht_elem_t;

typedef struct ht {
	ht_elem_t **elems;
} ht_t;

//http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(const char *key)
{
	unsigned long hash = 5381;
	int c;
	while ((c = *key++))
		hash = ((hash << 5) + hash) + c;

	return hash % TABLE_SIZE;
}

ht_t *ht_make()
{
	ht_t *ht = (ht_t *)malloc(sizeof(ht_t));
	ht->elems = malloc(sizeof(ht_elem_t) * TABLE_SIZE);

	for (int i = 0; i < TABLE_SIZE; ++i)
		ht->elems[0] = NULL;

	return ht;
}

void ht_free(ht_t *ht)
{
	for (size_t i = 0; i < TABLE_SIZE; ++i) {
		ht_elem_t *elem = ht->elems[i];
		while (elem != NULL) {
			ht_elem_t *next = elem->next;
			free(elem->val);
			free(elem);
			elem = next;
		}
	}
	free(ht->elems);
	free(ht);
}

ht_elem_t *ht_pair(const char *key, const void *val, size_t val_size)
{
	ht_elem_t *elem = (ht_elem_t *)malloc(sizeof(ht_elem_t));
	elem->key = strdup(key);
	elem->val = malloc(val_size);
#ifdef HT_DEBUG
	elem->val_size = val_size;
#endif
	memcpy(elem->val, val, val_size);
	return elem;
}

void ht_add(ht_t *ht, const char *key, const void *val, size_t val_size)
{
	unsigned long slot = hash(key);
	ht_elem_t *elem = ht->elems[slot];

	if (elem == NULL) {
		ht->elems[slot] = ht_pair(key, val, val_size);
		return;
	}

	ht_elem_t *prev;
	while (elem != NULL) {
		if (strcmp(key, elem->key) == 0) {
			free(elem->val);
			elem->val = malloc(val_size);
			memcpy(elem->val, val, val_size);
#ifdef HT_DEBUG
			elem->val_size = val_size;
#endif
			return;
		}

		prev = elem;
		elem = prev->next;
	}

	prev->next = ht_pair(key, val, val_size);
}

void *ht_get(ht_t *ht, const char *key)
{
	unsigned long slot = hash(key);

	ht_elem_t *elem = ht->elems[slot];

	if (elem == NULL)
		return NULL;

	while (elem != NULL) {
		if (strcmp(key, elem->key) == 0)
			return elem->val;
		elem = elem->next;
	}

	return NULL;
}

#ifdef HT_DEBUG
#include <ctype.h>
#include <stdio.h>

void ht_dump_elem(const ht_elem_t *elem)
{
    printf("\t%s[%zu]\n", elem->key, elem->val_size);
    char *val = elem->val;
    for (size_t j = 0; j < elem->val_size;) {
        printf("\t\t%07zu0 ", j/8);
        char k = 0;
        for (; j < elem->val_size && k < 8; ++j, ++k)
            printf("%02X ", val[j]);
        for (char l = k; l < 8; ++l)
            printf("   ");
        for (char l = k; l > 0; --l)
            printf("%c",
                   isprint(val[j - l]) == 0 ?
                   '.' :
                   val[j - l]);
        printf("\n");
    }
}

void ht_dump(const struct ht *ht)
{
	for (int i = 0; i < TABLE_SIZE; ++i) {
		ht_elem_t *elem = ht->elems[i];
		if (elem == NULL)
			continue;
		printf("slot[%d]\n", i);
        ht_dump_elem(elem);
		while (elem != NULL) {

			elem = elem->next;
		}
		printf("\n");
	}
}
#endif
