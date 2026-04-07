#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
#include "global.h"

u32 ui_inventory_cap(Inventory* inventory)
{
	return inventory->rows * inventory->cols;
}

void ui_inventory_init_cells(Inventory* inventory)
{
	const u32 cap = ui_inventory_cap(inventory);

	Vector2 dim = gfx_to_monitor( (Vector2) {50, 50} );
	Vector2 pos = gfx_to_monitor( (Vector2) { GetScreenWidth() - 100 - dim.x, 100 } );

	for(i32 i = 0; i < cap; i++)
	{
		inventory->cells[i] = (InventoryCell) {
			.hitbox = {
				pos.x + ( ( (i % inventory->cols) * dim.x) ) - (inventory->hitbox.width - dim.x), 
				pos.y + ( (i / inventory->cols) * dim.y ), 
				dim.x, 
				dim.y },
			.item = (Item) {
				.info = item_info_table[ITEM_TYPE_NONE],
				.type = ITEM_TYPE_NONE,
			},
			.focused = false,
			.id = i,
		};
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
			.hitbox = { screen_pos.x, screen_pos.y, dim.x, dim.y },
			.mode = INVENTORY_MODE_NONE,
			.moved_id = -1,
			.focus_id = -1,
	};
	ui_inventory_init_cells(inv_new);

	ui_inventory_add_item(inv_new, (Item) {
			.type = ITEM_TYPE_PLACEHOLDER,
			.info = item_info_table[ITEM_TYPE_PLACEHOLDER],
			.enchant = { 1 },
			} );
	inv_new->cells[13].item = (Item) {
			.type = ITEM_TYPE_HELMET,
			.info = item_info_table[ITEM_TYPE_HELMET],
			.enchant = { rand() % 4 },
			};


	return inv_new;
}

InventoryCell ui_inventory_get_cell(Inventory* inventory, Vector2 pos)
{
	const u32 cap = ui_inventory_cap(inventory);
	for(i32 i = 0; i < cap; i++)
	{
		InventoryCell cell = inventory->cells[i];
		if(AAB(cell.hitbox, pos))
		{
			return cell;
		}
	}
}

void ui_inventory_cell_tick(InventoryCell* cell, Inventory* inventory, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(AAB(cell->hitbox, mouse_cords))
	{
		cell->focused = cell->id;
	} else cell->focused = -1;

	if(inventory->mode == INVENTORY_MODE_NONE && 
			cell->item.type != ITEM_TYPE_NONE &&
			AAB(cell->hitbox, mouse_cords) && 
			IsMouseButtonDown(0))
	{
		inventory->mode = INVENTORY_MODE_MOVE;
		inventory->moved_id = cell->id;
	}
}

void ui_inventory_swap_items(Inventory* inventory, GameState* state)
{
	i32 focus_id = inventory->focus_id;
	i32 moved_id = inventory->moved_id;
	Item tmp = inventory->cells[focus_id].item;
	inventory->cells[focus_id].item = inventory->cells[moved_id].item;
	inventory->cells[moved_id].item = tmp;
}

void ui_inventory_move_mode(Inventory* inventory, GameState* state)
{
	if(inventory->mode == INVENTORY_MODE_MOVE && IsMouseButtonReleased(0))
	{
		inventory->mode = INVENTORY_MODE_NONE;
		ui_inventory_swap_items(inventory, state);
	}
}

void ui_inventory_tick(Inventory* inventory, GameState* state)
{
	const u32 cap = ui_inventory_cap(inventory);
	for(i32 i = 0; i < cap; i++)
	{
		ui_inventory_cell_tick(&inventory->cells[i], inventory, state);
	}
	ui_inventory_move_mode(inventory, state);
}

static const Color enchant_to_color_table[] =
{
	WHITE,
	{50, 10, 150, 255},
	{235, 25, 200, 255},
	{200, 200, 10, 255},
	{},
	{},
	{},
	{},
	{},
};

static const Rectangle inventory_item_table[] =
{
	{0, 0, 16, 16},
	{16, 0, 16, 16},
	{16, 16, 16, 16},
};

void ui_inventory_cell_background_render(InventoryCell* cell, GameState* state)
{
	Gfx* gfx = state->gfx;
	Rectangle source_background = (Rectangle) {0, 16, 16, 16};
	Rectangle source_border = (Rectangle) {0, 32, 16, 16};

	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			source_background, 
			cell->hitbox, 
			(Vector2) {0}, 
			0.0, 
			enchant_to_color_table[cell->item.enchant.level]);
	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			source_border, 
			cell->hitbox, 
			(Vector2) {0}, 
			0.0, 
			WHITE);
}

void ui_inventory_cell_render(InventoryCell cell, GameState* state)
{
	Gfx* gfx = state->gfx;
	ui_inventory_cell_background_render(&cell, state);
	if(cell.item.type != ITEM_TYPE_NONE)
	{
		DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
				inventory_item_table[cell.item.type], 
				cell.hitbox, 
				(Vector2) {0}, 
				0.0, 
				WHITE);
	}

}

void ui_inventory_add_item(Inventory* inventory, Item item)
{
	const u32 cap = ui_inventory_cap(inventory);
	for(i32 i = 0; i < cap; i++)
	{
		InventoryCell* cell = &inventory->cells[i];
		if(cell->item.type == ITEM_TYPE_NONE)
		{
			cell->item = item; break;
		}
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
		DrawText(TextFormat("%s\n%s", cell.item.info.name, cell.item.info.description), cell.hitbox.x, cell.hitbox.y, 20, GREEN);
	}
}

void ui_inventory_render(Inventory* inventory, GameState* state)
{
	Gfx* gfx = state->gfx;
	const u32 cap = ui_inventory_cap(inventory);
	i32 focused_cell_id = -1;
	ui_inventory_background_render(inventory, state);
	for(i32 i = 0; i < cap; i++)
	{
		ui_inventory_cell_render(inventory->cells[i], state);
		if(inventory->cells[i].focused != -1) 
		{
			inventory->focus_id = inventory->cells[i].id;
			focused_cell_id = inventory->cells[i].id;
		}
	}
	ui_item_cell_text_render(inventory, state, focused_cell_id);
#if 1
	//TODO fix this its static now
	if(inventory->mode == INVENTORY_MODE_MOVE)
	{
		Vector2 mouse_cords = GetMousePosition();
		Rectangle r = {16, 0, 16, 16};
		Rectangle tr = inventory->cells[inventory->focus_id].hitbox;
		tr.x = mouse_cords.x; tr.y = mouse_cords.y;
		DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
				r, 
				tr, 
				(Vector2) {0}, 
				0.0, 
				WHITE);
	}
#endif
}

void ui_text_box_render()
{

}

