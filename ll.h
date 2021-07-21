#ifndef _LL_H_
#define _LL_H_

#include <stdio.h>
#include <string.h>
#define LL_DECL(name, type)                                                    \
	struct name;                                                           \
	struct name *name##_make(type *value);                                 \
	struct name *name##_append(struct name *list, type *value);            \
	void name##_free(struct name *list);                                   \
	void name##_remove(struct name *list, type *value);                    \
	void name##_print(struct name *list, const char *format);

#define LL_IMPL_LIST(name, type)                                               \
	typedef struct name {                                                  \
		type *value;                                                   \
		struct name *next;                                             \
	} name;

#define LL_IMPL_FN(name, type)                                                 \
	name *name##_make(type *value)                                         \
	{                                                                      \
		if (value == NULL)                                             \
			return NULL;                                           \
		name *list = (name *)malloc(sizeof(name));                     \
		list->value = (type *)malloc(sizeof(type));                    \
		memcpy(list->value, value, sizeof(type));                      \
		list->next = NULL;                                             \
		return list;                                                   \
	}                                                                      \
                                                                               \
	name *name##_append(name *list, type *value)                           \
	{                                                                      \
		if (list == NULL)                                              \
			return name##_make(value);                             \
                                                                               \
		name *head = list;                                             \
		while (head->next != NULL)                                     \
			head = head->next;                                     \
                                                                               \
		head->next = name##_make(value);                               \
                                                                               \
		return list;                                                   \
	}                                                                      \
                                                                               \
	void name##_free(name *list)                                           \
	{                                                                      \
		name *head;                                                    \
		while (list != NULL) {                                         \
			head = list;                                           \
			free(head->value);                                     \
			list = list->next;                                     \
			free(head);                                            \
		}                                                              \
	}                                                                      \
                                                                               \
	void name##_remove(name *list, type *value)                            \
	{                                                                      \
		name *prev = list;                                             \
		while (list != NULL &&                                         \
		       memcmp(list->value, value, sizeof(type)) != 0) {        \
			prev = list;                                           \
			list = list->next;                                     \
		}                                                              \
                                                                               \
		if (list == NULL)                                              \
			return;                                                \
                                                                               \
		prev->next = list->next;                                       \
		list->next = NULL;                                             \
		name##_free(list);                                             \
	}                                                                      \
                                                                               \
	void name##_print(name *list, const char *format)                      \
	{                                                                      \
		while (list != NULL) {                                         \
			printf(format, *(list->value));                        \
			list = list->next;                                     \
		}                                                              \
		printf("\n");                                                  \
	}

#endif //_LL_H_
