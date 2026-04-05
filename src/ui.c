#include <stdlib.h>

#include "ui.h"
#include "global.h"

u32 ui_inventory_len(Inventory* inventory)
{
	return inventory->rows * inventory->cols;
}

void ui_inventory_init_cells(Inventory* inventory)
{
	const u32 len = ui_inventory_len(inventory);
	for(i32 i = 0; i < len; i++)
	{
		inventory->cells[i] = (InventoryCell) {
			.hitbox = {16,16,16,16},
		};
	}
}

Inventory* ui_inventory_new()
{
	Inventory* inv_new = malloc(sizeof(Inventory));
	Vector2 screen_pos = GetScreenPosition();
	*inv_new = (Inventory) {
		.cols = 6,
			.rows = 10,
			.hitbox = { screen_pos.x - screen_pos.x / 4, screen_pos.y - 40, 100, 800 }
	};
	ui_inventory_init_cells(inv_new);
	return inv_new;
}

void ui_inventory_cell_tick(InventoryCell* cell, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(AAB(cell->hitbox, mouse_cords))
	{
		// Show text or smth
	}
}

void ui_inventory_tick(Inventory* inventory, GameState* state)
{
	const u32 len = ui_inventory_len(inventory);
	for(i32 i = 0; i < len; i++)
	{
		ui_inventory_cell_tick(&inventory->cells[i], state);
	}
}

void ui_inventory_cell_render(InventoryCell cell, GameState* state)
{
	if(cell.item.type == ITEM_TYPE_NONE)
	{

	}
}

void ui_inventory_background_render(Inventory* inventory, GameState* state)
{

}

void ui_inventory_render(Inventory* inventory, GameState* state)
{
	const u32 len = ui_inventory_len(inventory);
	ui_inventory_background_render(inventory, state);
	for(i32 i = 0; i < len; i++)
	{
		ui_inventory_cell_render(inventory->cells[i], state);
	}
}

