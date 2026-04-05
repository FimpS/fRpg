#ifndef ITEM_H
#define ITEM_H

typedef enum 
{
	ITEM_TYPE_NONE,
	ITEM_TYPE_PLACEHOLDER,
} ItemType;

typedef struct Item
{
	ItemType type;
} Item;


#endif
