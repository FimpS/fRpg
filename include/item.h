#ifndef ITEM_H
#define ITEM_H

#include "../lib/types.h"

#define MAX_NAME_LEN 32
#define MAX_DESC_LEN 128

typedef enum 
{
	ITEM_TYPE_NONE,
	ITEM_TYPE_PLACEHOLDER,
	ITEM_TYPE_HELMET,
} ItemType;

typedef enum
{
	ITEM_CLASS_NONE,
	ITEM_CLASS_RING,
	ITEM_CLASS_HELMET,
} ItemClass;

typedef struct ItemInfo
{
	u8 name[MAX_NAME_LEN];
	u8 description[MAX_DESC_LEN];
	ItemClass class;
	bool stackable;
} ItemInfo;

typedef struct ItemEnchant
{
	i32 level; //maybe this is a ratio of the level of the enemy?
} ItemEnchant;

typedef struct Item
{
	ItemType type;
	ItemInfo info;
	ItemEnchant enchant;
} Item;

extern const ItemInfo item_info_table[];

#endif
