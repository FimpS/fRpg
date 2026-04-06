#ifndef ITEM_H
#define ITEM_H

#include "../lib/types.h"

#define MAX_NAME_LEN 32
#define MAX_DESC_LEN 128

typedef enum 
{
	ITEM_TYPE_NONE,
	ITEM_TYPE_PLACEHOLDER,
} ItemType;

typedef enum
{
	ITEM_CLASS_NONE,
	ITEM_CLASS_RING,
} ItemClass;

typedef struct ItemInfo
{
	u8 name[MAX_NAME_LEN];
	u8 description[MAX_DESC_LEN];
	ItemClass class;
} ItemInfo;

typedef struct Item
{
	ItemType type;
	ItemInfo info;
} Item;

extern const ItemInfo item_info_table[];

#endif
