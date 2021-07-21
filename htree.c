#include "htree.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// TODO: Make it dynamic maybe htreetps://attractivechaos.wordpress.com/2009/09/29/khash-h/
#define TABLE_SIZE 10000

typedef struct _htree_elem {
	char *key;
	void *val;
#ifdef HTREE_DEBUG
	size_t val_size;
#endif
	struct _htree_elem *next;
} htree_elem_t;

typedef struct htree {
	htree_elem_t **elems;
} htree_t;

//htreetp://www.cse.yorku.ca/~oz/hash.htreeml
unsigned long hash(const char *key)
{
	unsigned long hash = 5381;
	int c;
	while ((c = *key++))
		hash = ((hash << 5) + hash) + c;

	return hash % TABLE_SIZE;
}

htree_t *htree_make()
{
	htree_t *htree = (htree_t *)malloc(sizeof(htree_t));
	htree->elems = malloc(sizeof(htree_elem_t) * TABLE_SIZE);

	for (int i = 0; i < TABLE_SIZE; ++i)
		htree->elems[i] = NULL;

	return htree;
}

void htree_free(htree_t *htree)
{
	for (size_t i = 0; i < TABLE_SIZE; ++i) {
		htree_elem_t *elem = htree->elems[i];
		while (elem != NULL) {
			htree_elem_t *next = elem->next;
			free(elem->val);
			free(elem->key);
			free(elem);
			elem = next;
		}
	}
	free(htree->elems);
	free(htree);
}

htree_elem_t *htree_pair(const char *key, const void *val, size_t val_size)
{
	htree_elem_t *elem = (htree_elem_t *)malloc(sizeof(htree_elem_t));
	elem->key = (char *)malloc(strlen(key) + 1);
	strcpy(elem->key, key);
	elem->val = malloc(val_size);
#ifdef HTREE_DEBUG
	elem->val_size = val_size;
#endif
	memcpy(elem->val, val, val_size);
	elem->next = NULL;
	return elem;
}

void htree_add(htree_t *htree, const char *key, const void *val,
	       size_t val_size)
{
	unsigned long slot = hash(key);
	htree_elem_t *elem = htree->elems[slot];

	if (elem == NULL) {
		htree->elems[slot] = htree_pair(key, val, val_size);
		return;
	}

	htree_elem_t *prev;
	while (elem != NULL) {
		if (strcmp(key, elem->key) == 0) {
			free(elem->val);
			elem->val = malloc(val_size);
			memcpy(elem->val, val, val_size);
#ifdef HTREE_DEBUG
			elem->val_size = val_size;
#endif
			return;
		}

		prev = elem;
		elem = prev->next;
	}

	prev->next = htree_pair(key, val, val_size);
}

void *htree_get(htree_t *htree, const char *key)
{
	unsigned long slot = hash(key);

	htree_elem_t *elem = htree->elems[slot];

	if (elem == NULL)
		return NULL;

	while (elem != NULL) {
		if (strcmp(key, elem->key) == 0)
			return elem->val;
		elem = elem->next;
	}

	return NULL;
}

#ifdef HTREE_DEBUG
#include <ctype.h>
#include <stdio.h>

void htree_dump_elem(const htree_elem_t *elem)
{
	printf("\t%s[%zu]\n", elem->key, elem->val_size);
	char *val = elem->val;
	for (size_t j = 0; j < elem->val_size;) {
		printf("\t\t%07zu0 ", j / 8);
		char k = 0;
		for (; j < elem->val_size && k < 8; ++j, ++k)
			printf("%02X ", val[j]);
		for (char l = k; l < 8; ++l)
			printf("   ");
		for (char l = k; l > 0; --l)
			printf("%c",
			       isprint(val[j - l]) == 0 ? '.' : val[j - l]);
		printf("\n");
	}
}

void htree_dump(const struct htree *htree)
{
	for (int i = 0; i < TABLE_SIZE; ++i) {
		htree_elem_t *elem = htree->elems[i];
		if (elem == NULL)
			continue;
		printf("slot[%d]\n", i);
		htree_dump_elem(elem);
		while (elem != NULL) {
			elem = elem->next;
		}
		printf("\n");
	}
}
#endif
