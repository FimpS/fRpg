#pragma once

#include <stdbool.h>

typedef struct DynList
{
	void** v;
	unsigned len;
	unsigned cap;
} DynList;

DynList* dynList_new();
unsigned dynList_len(DynList* l);
bool dynList_empty(DynList* l);
void* dynList_get(DynList* l, unsigned index);
void dynList_set(DynList* l, void* key, unsigned index);
void dynList_push(DynList* l, void* key);
void dynList_pop(DynList* l);
void dynList_add(DynList* l, void* key, unsigned index);
void dynList_del(DynList* l, unsigned index);
void dynList_clear(DynList *l);
void dynList_destroy(DynList *l);

