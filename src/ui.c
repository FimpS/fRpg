#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
#include "global.h"

u32 ui_inventory_len(Inventory* inventory)
{
	return inventory->rows * inventory->cols;
}

void ui_inventory_init_cells(Inventory* inventory)
{
	const u32 len = ui_inventory_len(inventory);

	Vector2 dim = gfx_to_monitor( (Vector2) {50, 50} );
	Vector2 pos = gfx_to_monitor( (Vector2) { GetScreenWidth() - 100 - dim.x, 100 } );

	for(i32 i = 0; i < len; i++)
	{
		inventory->cells[i] = (InventoryCell) {
			.hitbox = {
				pos.x + ( ( (i % inventory->cols) * dim.x) ) - (inventory->hitbox.width - dim.x), 
				pos.y + ( (i / inventory->cols) * dim.y ), 
				dim.x, 
				dim.y },
			.item = {0},
			.focused = false,
			.id = i,
		};
		//P_LOG("%f %f %f %f\n", pos.x, pos.y, dim.x, dim.y);
	}
}

Inventory* ui_inventory_new()
{
	Inventory* inv_new = malloc(sizeof(Inventory));
	Vector2 dim = gfx_to_monitor( (Vector2) {400, 800} );
	Vector2 screen_pos = gfx_to_monitor( (Vector2) { GetScreenWidth() - 100 - dim.x, 100 } );
	*inv_new = (Inventory) {
		.cols = 6,
			.rows = 10,
			.hitbox = { screen_pos.x, screen_pos.y, dim.x, dim.y }
	};
	ui_inventory_init_cells(inv_new);
	return inv_new;
}

void ui_inventory_cell_tick(InventoryCell* cell, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(AAB(cell->hitbox, mouse_cords))
	{
		cell->focused = true;
	} else cell->focused = false;
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
	Gfx* gfx = state->gfx;
	Rectangle r;
	if(cell.item.type == ITEM_TYPE_NONE)
	{
		r = (Rectangle) {0, 16, 16, 16};
		DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			r, 
			cell.hitbox, 
			(Vector2) {0}, 
			0.0, 
			WHITE);
	}

	
}

void ui_inventory_background_render(Inventory* inventory, GameState* state)
{
	Gfx* gfx = state->gfx;
	Rectangle r = {0, 0, 16, 16};
	Rectangle tr = inventory->hitbox;
	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			r, 
			inventory->hitbox, 
			(Vector2) {0}, 
			0.0, 
			WHITE);
}

void ui_item_cell_text_render(Inventory* inventory, GameState* state, const i32 id)
{
	if(id != -1) 
	{
		InventoryCell cell = inventory->cells[id];
		DrawText(TextFormat("ID: %d", cell.id), cell.hitbox.x, cell.hitbox.y, 20, GREEN);
	}
}

void ui_inventory_render(Inventory* inventory, GameState* state)
{
	const u32 len = ui_inventory_len(inventory);
	i32 focused_cell_id = -1;
	ui_inventory_background_render(inventory, state);
	for(i32 i = 0; i < len; i++)
	{
		ui_inventory_cell_render(inventory->cells[i], state);
		if(inventory->cells[i].focused) focused_cell_id = inventory->cells[i].id;
	}
	ui_item_cell_text_render(inventory, state, focused_cell_id);
}

