#ifndef UI_H
#define UI_H

#include "dynList.h"
#include "item.h"
#include "state.h"
#include "../lib/types.h"

#define TOTAL_INVENTORY_CELLS 60

typedef struct InventoryCell
{
	Item item;
	Rectangle hitbox;
	
} InventoryCell;

typedef struct Inventory
{
	InventoryCell cells[TOTAL_INVENTORY_CELLS];
	Rectangle hitbox;
	u32 cols;
	u32 rows;
} Inventory;

Inventory* ui_inventory_new();

void ui_inventory_render(Inventory* inventory, GameState* state);


#endif
