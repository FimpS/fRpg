#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "global.h"
#include "../include/entity_info.h"

void ui_inventory_sort_cells(Inventory* inventory, GameState* state);

u32 ui_inventory_cap(Inventory* inventory)
{
	return inventory->rows * inventory->cols;
}

void ui_inventory_init_cells(Inventory* inventory)
{
	const u32 cap = ui_inventory_cap(inventory);

	Vector2 dim = gfx_to_monitor_vector( (Vector2) {50, 50} );
	P_LOG("%f\n", dim.x);
	Vector2 pos = gfx_to_monitor_vector( (Vector2) { GetScreenWidth() - 0 - dim.x - 30, 100 + inventory->hitbox.height - dim.y * inventory->rows - 30} );

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
				.enchant = { -1 },
				.type = ITEM_TYPE_NONE,
			},
			.focused = false,
			.id = i,
		};
	}
}

void ui_inventory_toggle_fire(Inventory* inventory, GameState* state)
{
	Gfx* gfx = state->gfx;
	gfx->mouse.type = gfx->mouse.type == MOUSE_TYPE_HAMMER ? MOUSE_TYPE_STANDARD : MOUSE_TYPE_HAMMER;
	inventory->mode = inventory->mode == INVENTORY_MODE_DELETE ? INVENTORY_MODE_NONE : INVENTORY_MODE_DELETE;
}

static const InventoryButton ui_buttons_table[] = {
	(InventoryButton) {
		.hitbox = {1425, 800, 64, 64},
		.src_rec = {0, 48, 16, 16},
		.hover_src_rec = {16, 48, 16, 16},
		.on_click = ui_inventory_sort_cells,
	},
	(InventoryButton) {
		.hitbox = {1425, 800 - 64 - 16, 64, 64},
		.src_rec = {32, 48, 16, 16},
		.hover_src_rec = {48, 48, 16, 16},
		.on_click = ui_inventory_toggle_fire,
	},
};

void ui_inventory_init_buttons(Inventory* inventory)
{
	const u32 len = TOTAL_INVENTORY_BUTTONS;
	for(i32 i = 0; i < TOTAL_INVENTORY_BUTTONS; i++)
	{
		InventoryButton* button = &inventory->buttons[i];
		(*button) = ui_buttons_table[i];
		button->hitbox = gfx_to_monitor_rectangle(ui_buttons_table[i].hitbox);
	}
}

Item ui_inventory_get_empty_cell()
{
	return (Item) {
		.type = ITEM_TYPE_NONE,
		.info = item_info_table[ITEM_TYPE_NONE],
		.enchant = { - 1 },
	};
}

Inventory* ui_inventory_new()
{
	Inventory* inv_new = malloc(sizeof(Inventory));
	Vector2 dim = gfx_to_monitor_vector( (Vector2) {400, 800} );
	Vector2 screen_pos = gfx_to_monitor_vector( (Vector2) { GetScreenWidth() - 100 - dim.x, 100 } );
	*inv_new = (Inventory) {
		.cols = 6,
			.rows = 10,
			.hitbox = { screen_pos.x, screen_pos.y, dim.x, dim.y },
			.mode = INVENTORY_MODE_NONE,
			.moved_id = -1,
			.focus_id = -1,
			.buttons = {0},
	};
	ui_inventory_init_cells(inv_new);
	ui_inventory_init_buttons(inv_new);

	ui_inventory_add_item(inv_new, (Item) {
			.type = ITEM_TYPE_PLACEHOLDER,
			.info = item_info_table[ITEM_TYPE_PLACEHOLDER],
			.enchant = { 1 },
			} );
	for(i32 i = 4; i < 20; i+=2)
	{
		inv_new->cells[i].item = (Item) {
			.type = ITEM_TYPE_HELMET,
				.info = item_info_table[ITEM_TYPE_HELMET],
				.enchant = { rand() % 4 },
		};
	}


	return inv_new;
}

void ui_draw_outline_text(const u8* text, Vector2 pos, 
		u32 font_size, 
		Color outline_color, 
		Color color, 
		GameState* state)
{
	Gfx* gfx = state->gfx;
	DrawTextEx(gfx->font, text, 
			(Vector2) { pos.x + 1, pos.y }, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->font, text, 
			(Vector2) {pos.x, pos.y + 1}, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->font, text, 
			(Vector2) {pos.x - 1, pos.y}, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->font, text, 
			(Vector2) {pos.x, pos.y - 1}, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->font, text, 
			(Vector2) {pos.x, pos.y}, 
			font_size, 0.0, color);

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

void ui_inventory_cell_equip_item(InventoryCell* cell, Inventory* inventory, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(inventory->mode == INVENTORY_MODE_NONE &&
			cell->item.type != ITEM_TYPE_NONE &&
			AAB(cell->hitbox, mouse_cords) &&
			IsKeyDown(KEY_LEFT_SHIFT) &&
			IsMouseButtonPressed(0)
	  )
	{
		P_LOG("TODO: Equip\n");
	}
}

void ui_inventory_cell_enable_move(InventoryCell* cell, Inventory* inventory, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(inventory->mode == INVENTORY_MODE_NONE && 
			cell->item.type != ITEM_TYPE_NONE &&
			!IsKeyDown(KEY_LEFT_SHIFT) &&
			AAB(cell->hitbox, mouse_cords) && 
			IsMouseButtonDown(0))
	{
		inventory->mode = INVENTORY_MODE_MOVE;
		inventory->moved_id = cell->id;
	}
}

void ui_inventory_cell_delete_item(InventoryCell* cell, Inventory* inventory, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(inventory->mode == INVENTORY_MODE_DELETE &&
		cell->item.type != ITEM_TYPE_NONE &&
		IsMouseButtonPressed(0) &&
		AAB(cell->hitbox, mouse_cords))
	{
		cell->item = ui_inventory_get_empty_cell();
	}
}

void ui_inventory_cell_tick(InventoryCell* cell, Inventory* inventory, GameState* state)
{
	Vector2 mouse_cords = GetMousePosition();
	if(AAB(cell->hitbox, mouse_cords))
	{
		cell->focused = cell->id;
	} else cell->focused = -1;

	ui_inventory_cell_delete_item(cell, inventory, state);
	ui_inventory_cell_enable_move(cell, inventory, state);
	ui_inventory_cell_equip_item(cell, inventory, state);
}

void ui_inventory_switch_items(Inventory* inventory, GameState* state)
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
		ui_inventory_switch_items(inventory, state);
	}
}

void ui_inventory_swap_items(InventoryCell* c1, InventoryCell* c2)
{
	Item tmp = c1->item;
	c1->item = c2->item;
	c2->item = tmp;
}

void ui_inventory_sort_cells(Inventory* inventory, GameState* state)
{
	const u32 len = ui_inventory_cap(inventory);
	for(i32 i = 0; i < len; i++)
	{
		for(i32 j = i + 1; j < len; j++)
		{
			InventoryCell* cell_i = &inventory->cells[i];
			InventoryCell* cell_j = &inventory->cells[j];
			if(cell_i->item.enchant.level < cell_j->item.enchant.level ||
					(cell_i->item.enchant.level == cell_j->item.enchant.level &&
					 cell_i->item.type < cell_j->item.type))
			{
				ui_inventory_swap_items(cell_i, cell_j);
			}
		}
	}

}

void ui_inventory_buttons_tick(Inventory* inventory, GameState* state, Vector2 mp)
{
	const u32 len = TOTAL_INVENTORY_BUTTONS;
	for(i32 i = 0; i < len; i++)
	{
		InventoryButton* button = &inventory->buttons[i];
		if(AAB(button->hitbox, mp))
		{
			button->hovered = true;
			if(IsMouseButtonPressed(0) && button->on_click != NULL)
			{
				button->on_click(inventory, state);
			}
		} else button->hovered = false;
	}
}

void ui_inventory_tick(Inventory* inventory, GameState* state)
{
	const Vector2 mp = GetMousePosition();
	const u32 cap = ui_inventory_cap(inventory);
	for(i32 i = 0; i < cap; i++)
	{
		ui_inventory_cell_tick(&inventory->cells[i], inventory, state);
	}
	ui_inventory_buttons_tick(inventory, state, mp);
	ui_inventory_move_mode(inventory, state);
}

static const Color enchant_to_color_table[] =
{
	{255, 255, 255, 255},
	{20, 220, 35, 255},
	{50, 10, 150, 255},
	{235, 25, 200, 255},
	{200, 200, 10, 255},
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
		if(cell.item.info.stackable)
		{
			ui_draw_outline_text(TextFormat("x%d", cell.amount),
					(Vector2) { cell.hitbox.x + cell.hitbox.width - 25, 
					cell.hitbox.y + cell.hitbox.height - 25 }, 
					32,
					BLACK,
					WHITE,
					state);
		}
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
		if(cell.item.type == ITEM_TYPE_NONE) return;
		const Vector2 rpos = gfx_to_monitor_vector( (Vector2) { -200.0, 10.0 } );
		// Maybe force it to be top left of the box instead of dependent on cellhitbox
		ui_text_box_render(cell.item.info.name, cell.item.info.description, 
				(Vector2) { cell.hitbox.x + rpos.x, cell.hitbox.y + rpos.y}, 
				state);
	}
}

void ui_inventory_render_moved_item(Inventory* inventory, GameState* state)
{
	Gfx* gfx = state->gfx;
	if(inventory->mode == INVENTORY_MODE_MOVE)
	{
		Vector2 mouse_cords = GetMousePosition();
		Rectangle src_rec = inventory_item_table[inventory->cells[inventory->moved_id].item.type];
		Rectangle dst_rec = inventory->cells[inventory->focus_id].hitbox;
		dst_rec.x = mouse_cords.x; dst_rec.y = mouse_cords.y;

		DrawTexturePro(gfx->texs[TEXTURE_GAME_UI],
				(Rectangle) {0, 16, 16, 16},
				dst_rec,
				(Vector2) {0}, 
				0.0, 
				enchant_to_color_table[inventory->cells[inventory->moved_id].item.enchant.level]);
		DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
				(Rectangle) {0, 32, 16, 16}, 
				dst_rec, 
				(Vector2) {0}, 
				0.0, 
				WHITE);
		DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
				src_rec, 
				dst_rec, 
				(Vector2) {0}, 
				0.0, 
				WHITE);
	}
}

void ui_inventory_render_buttons(Inventory* inventory, GameState* state)
{
	Gfx* gfx = state->gfx;
	const u32 len = TOTAL_INVENTORY_BUTTONS;
	for(i32 i = 0; i < len; i++)
	{
		InventoryButton* button = &inventory->buttons[i];
		if(button->hovered)
		{
			DrawTexturePro(gfx->texs[TEXTURE_GAME_UI],
					button->hover_src_rec,
					button->hitbox,
					(Vector2) {0},
					0.0,
					WHITE);
		}
		else
		{
			DrawTexturePro(gfx->texs[TEXTURE_GAME_UI],
					button->src_rec,
					button->hitbox,
					(Vector2) {0},
					0.0,
					WHITE);
		}
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
	ui_inventory_render_buttons(inventory, state);
	ui_item_cell_text_render(inventory, state, focused_cell_id);
	ui_inventory_render_moved_item(inventory, state);
}

void ui_wrap_text_render(const u8* text, 
		Vector2 pos, 
		const i32 max_width, 
		const u32 padding,
		const u32 font_size,
		GameState* state
		)
{
	Gfx* gfx = state->gfx;
	const u32 text_len = strlen(text);
	const u32 max_str_len = 128;
	i32 index = 0;
	u8 current_string[max_str_len];

	i32 y_offset = 0;
	for(i32 i = 0; i < text_len; i++)
	{
		u8 current_char = text[i];
		current_string[index ++] = text[i];

		if(current_char == ' ')
		{

			current_string[index] = '\0';
			Vector2 line_width = MeasureTextEx(gfx->font, current_string, font_size, 0.0);
			if( line_width.x >= max_width )
			{
				ui_draw_outline_text(current_string, 
						(Vector2) { pos.x + padding, 
						pos.y + padding + y_offset },
						font_size,
						BLACK,
						WHITE,
						state);
				y_offset += font_size + 8;
				index = 0;
			}
		}
		if(current_char == '\n') 
		{
			u32 newline_index = i;
			current_string[index] = '\0';

			ui_draw_outline_text(current_string, 
					(Vector2) { pos.x + padding, pos.y + padding + y_offset },
					font_size,
					BLACK,
					WHITE,
					state);
			while(text[newline_index++] == '\n') 
			{
				y_offset += font_size + 8;
			}
			index = 0;

		}
	}
	if(index > 0)
	{
		current_string[index] = '\0';
		ui_draw_outline_text(current_string, 
				(Vector2) { pos.x + padding, pos.y + padding + y_offset },
				font_size,
				BLACK,
				WHITE,
				state);
	}
}

void ui_text_box_render(const u8* title, const u8* text, Vector2 pos, GameState* state)
{
	Gfx* gfx = state->gfx;
	Vector2 box_dim = gfx_to_monitor_vector( (Vector2) {200, 400} );
	const i32 size = gfx_to_monitor(20);
	const u32 text_len = strlen(text);
	const i32 line_len = box_dim.x - 40;

	const i32 padding = 20;
	const i32 max_width = box_dim.x - padding * 2 -40;

	DrawTexturePro(gfx->texs[TEXTURE_GAME_UI], 
			(Rectangle) {0, 80, 32, 48},
			(Rectangle) {pos.x, pos.y, box_dim.x, box_dim.y}, 
			(Vector2) {0},
			0.0,
			WHITE);
	ui_wrap_text_render(title, pos, max_width, padding, size, state);
	pos.y += padding * 4;
	ui_wrap_text_render(text, pos, max_width, padding, size, state);
}


void ui_render(GameState* state)
{
	if(IsKeyDown(KEY_I))
	ui_inventory_render(state->inventory, state);
	gfx_render_mouse(state->gfx);
}

