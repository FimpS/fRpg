#include <stdlib.h>

#include "dynList.h"

#define START_CAP 4
#define MEM_ALLOC(l) if (l->len + 1 > l->cap) { l->cap *=2; l->v = realloc(l->v, l->cap * sizeof(void*)); }

DynList* dynList_new()
{
	DynList* new = malloc(sizeof(DynList));
	*new = (DynList){malloc(sizeof(void*) * START_CAP), 0, START_CAP};
	return new;
}

unsigned dynList_len(DynList* l)
{
	return l->len;
}

bool dynList_empty(DynList* l)
{
	return l->len == 0;
}

void* dynList_get(DynList* l, unsigned index)
{
	if( !(index <= l->len && index >= 0) )
		return NULL;
	return l->v[index];
}

void dynList_set(DynList* l, void* key, unsigned index)
{
	if ( index <= l->len && index >= 0 )
	{
		l->v[index] = key;
	}
}

void dynList_push(DynList* l, void* key)
{
	MEM_ALLOC(l);
	l->v[l->len ++] = key;
}

void dynList_pop(DynList* l)
{
	if(!dynList_empty(l))
	{
		l->len --;
	}
}

void dynList_add(DynList* l, void* key, unsigned index)
{
	if( index <= l->len && index >= 0)
	{
		MEM_ALLOC(l);
		l->len ++;
		unsigned len = dynList_len(l);
		for(int i = len; i >= index + 1; i--)
		{
			l->v[i] = l->v[i - 1];
		}
		l->v[index] = key;
	}
}

void dynList_del(DynList *l, unsigned index)
{
	if( index <= l->len && index >= 0 )
	{
		unsigned len = dynList_len(l);
		for(int i = index; i < len; i++)
		{
			l->v[i] = l->v[i + 1];
		}
		l->len --;
	}
}

void dynList_clear(DynList* l)
{
	l->len = 0;
	l->cap = START_CAP;
	l->v = realloc(l->v, sizeof(void*) * START_CAP);
}

void dynList_destroy(DynList* l)
{
	free(l->v);
	free(l);
}
