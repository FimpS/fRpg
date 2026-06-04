
#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "types.h"

typedef struct HashMapNode HashMapNode;
typedef struct HashMapNode
{
	void* item;	
	char* key;
	HashMapNode* next;
} HashMapNode;


typedef struct HashMap
{
	HashMapNode** nodes;
	u32 len;
} HashMap;

HashMap* hmap_new(const u32 len);
void hmap_destroy(HashMap* hmnode);
void hmap_add(HashMap* hmap, char* key, void* item);
void hmap_del(HashMap* hmap, char* key);
void* hmap_get(HashMap* hmap, const char* key);

static HashMapNode* hmnode_new(char* key, void* item)
{
	HashMapNode* node_new = malloc(sizeof(HashMapNode));
	node_new->item = item;
	node_new->key = malloc(strlen(key) + 1);
	strcpy(node_new->key, key);
	node_new->next = NULL;
}

static void hmnode_destroy(HashMapNode* hmnode)
{
	free(hmnode->key);
	free(hmnode);
}

HashMap* hmap_new(const u32 len)
{
	HashMap* hmap_new = malloc(sizeof(HashMap));	
	hmap_new->len = len;
	hmap_new->nodes = malloc(sizeof(HashMapNode) * len);
	memset(hmap_new->nodes, 0, len);
	return hmap_new;
}

void hmap_destroy(HashMap* hmap)
{
	u32 s = hmap->len;
	for(int i = 0; i < s; i++) 
	{
		HashMapNode* hmnode = hmap->nodes[i];
		while(hmnode)
		{
			HashMapNode* deleted = hmnode;
			hmnode = hmnode->next;
			hmnode_destroy(deleted);
		}
	}
	free(hmap);
}

HashMap* hmap_resize(HashMap* hmap, const u32 new_len)
{
	HashMap* new_map = hmap_new(new_len);
	u32 old_len = hmap->len;
	for(int i = 0; i < old_len; i++)
	{
		HashMapNode* hmnode = hmap->nodes[i];
		while(hmnode)
		{
			hmap_add(new_map, hmnode->key, hmnode->item);
			hmnode = hmnode->next;
		}
	}
	hmap_destroy(hmap);
	return new_map;
}

static u32 hmap_hash(HashMap* hmap, const char* key)
{
	u32 hash = 0;

	for(int i = 0; key[i] != 0; i++) (hash += key[i] * 113 + 10) / 3;

	return hash % hmap->len;
}

bool hmap_empty(HashMap* hmap)
{
	u32 len = hmap->len;
	for(int i = 0; i < len; i++) if(!hmap->nodes[i]) return false;
	return true;
}



static void add_hmnode(HashMapNode* hmn, char* key, void* item)
{
	HashMapNode* row_node = hmn;
	while(row_node->next)
	{
		row_node = row_node->next;
	}
	HashMapNode* new_node = hmnode_new(key, item);
	row_node->next = new_node;
}

void hmap_add(HashMap* hmap, char* key, void* item)
{
	u32 index = hmap_hash(hmap, key);
	HashMapNode* selected = hmap->nodes[index];
	if(selected == NULL)
	{
		hmap->nodes[index] = hmnode_new(key, item);
	}
	else
	{
		add_hmnode(selected, key, item);
	}
}

bool node_exists(HashMapNode* start, char* key)
{
	while(start)
	{
		if(!strcmp(start->key, key)) return true;
		start = start->next;
	}
	return false;
}

void hmap_del(HashMap* hmap, char* key)
{
	const u32 index = hmap_hash(hmap, key);
	HashMapNode* selected = hmap->nodes[index];
	HashMapNode* start = hmap->nodes[index];

	if(selected == NULL || !node_exists(start, key))
	{
		return;
	}
	if(selected->next == NULL)
	{
		hmnode_destroy(selected);
		hmap->nodes[index] = NULL;
		return;
	}
	selected = selected->next;
	while(strcmp(selected->key, key) != 0)
	{
		if(selected->next == NULL) 
		{
			return;
		}
		selected = selected->next;
		hmap->nodes[index] = hmap->nodes[index]->next;
	}

	HashMapNode* deleted = selected;
	hmap->nodes[index]->next = selected->next;
	hmap->nodes[index] = start;
	hmnode_destroy(deleted);
}

void* hmap_get(HashMap* hmap, const char* key)
{
	const u32 index = hmap_hash(hmap, key);
	HashMapNode* selected = hmap->nodes[index];
	while(strcmp(selected->key, key) != 0)
	{
		selected = selected->next;
		if(selected == NULL) return NULL;
	}

	return selected->item;
}


void hmap_print(HashMap* hmap)
{
	u32 len = hmap->len;
	for(int i = 0; i < len; i++)
	{
		HashMapNode* n = hmap->nodes[i];
		printf("%.2d: |- ", i);
		if(n)
		{
			while(n)
			{
				printf("\"%s\"->", n->key);
				n = n->next;
			}
		}
		printf("\n");
	}
}
