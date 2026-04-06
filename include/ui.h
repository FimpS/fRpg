#ifndef UI_H
#define UI_H

#include "dynList.h"
#include "item.h"
#include "state.h"
#include "../lib/types.h"
#include "../lib/v2.h"

#define TOTAL_INVENTORY_CELLS 60

typedef enum
{
	INVENTORY_MODE_NONE,
	INVENTORY_MODE_MOVE,
} InventoryMode;

typedef struct InventoryCell
{
	Item item;
	Rectangle hitbox;
	i32 focused;
	u32 id;
} InventoryCell;

typedef struct Inventory
{
	InventoryCell cells[TOTAL_INVENTORY_CELLS];
	Rectangle hitbox;
	u32 cols;
	u32 rows;
	V2 focused_cells;
	i32 focus_id;
	i32 moved_id;
	InventoryMode mode;
} Inventory;

Inventory* ui_inventory_new();

void ui_inventory_add_item(Inventory* inventroy, Item item);
void ui_inventory_tick(Inventory* inventory, GameState* state);
void ui_inventory_render(Inventory* inventory, GameState* state);


#endif
