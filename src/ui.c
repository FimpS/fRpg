#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "ui_data.h"
#include "shop_data.h"
#include "entity.h"
#include "global.h"
#include "quest_data.h"
#include "npc.h"
#include "player.h"
#include "../include/entity_data.h"

void ui_draw_text(const u8* text, Vector2 pos, 
		u32 font_size, 
		Color outline_color, 
		Color color, 
		GameState* state)
{
	Gfx* gfx = state->gfx;
	DrawTextEx(gfx->ui_font, text, 
			(Vector2) {pos.x, pos.y}, 
			font_size, 0.0, color);

}
void ui_draw_outline_text(const u8* text, Vector2 pos, 
		u32 font_size, 
		Color outline_color, 
		Color color, 
		GameState* state)
{
	Gfx* gfx = state->gfx;
	DrawTextEx(gfx->ui_font, text, 
			(Vector2) { pos.x + 1, pos.y }, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->ui_font, text, 
			(Vector2) {pos.x, pos.y + 1}, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->ui_font, text, 
			(Vector2) {pos.x - 1, pos.y}, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->ui_font, text, 
			(Vector2) {pos.x, pos.y - 1}, 
			font_size, 0.0, outline_color);
	DrawTextEx(gfx->ui_font, text, 
			(Vector2) {pos.x, pos.y}, 
			font_size, 0.0, color);

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
			Vector2 line_width = MeasureTextEx(gfx->ui_font, current_string, font_size, 0.0);
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
	const i32 size = gfx_to_monitor(16);
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

static u32 ui_inventory_cap(Inventory* inventory)
{
	return inventory->rows * inventory->cols;
}

static void ui_inventory_swap_items(InventoryCell* c1, InventoryCell* c2)
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

void ui_inventory_toggle_fire(Inventory* inventory, GameState* state)
{
	Gfx* gfx = state->gfx;
	gfx->mouse.type = gfx->mouse.type == MOUSE_TYPE_HAMMER ? MOUSE_TYPE_STANDARD : MOUSE_TYPE_HAMMER;
	inventory->mode = inventory->mode == INVENTORY_MODE_DELETE ? INVENTORY_MODE_NONE : INVENTORY_MODE_DELETE;
}

static Item ui_inventory_get_empty_cell()
{
	return (Item) {
		.type = ITEM_TYPE_NONE,
		.info = item_info_table[ITEM_TYPE_NONE],
		.enchant = { - 1 },
	};
}

static void ui_inventory_init_cells(Inventory* inventory)
{
	const u32 cap = ui_inventory_cap(inventory);

	Vector2 dim = gfx_to_monitor_vector( (Vector2) {50, 50} );
#if 0
	Vector2 pos = gfx_to_monitor_vector( (Vector2) { 
			inventory->hitbox.x - dim.x + inventory->hitbox.width + 100, 
			inventory->hitbox.y + inventory->hitbox.height - dim.y * inventory->rows - 30
			} );
#endif
	Vector2 pos = {
		.x = inventory->hitbox.x + inventory->hitbox.width - inventory->cell_offset.x,
		.y = inventory->hitbox.y + inventory->hitbox.height - inventory->cell_offset.y,
	};
	for(i32 i = 0; i < cap; i++)
	{
		inventory->cells[i] = (InventoryCell) {
			.hitbox = {
				pos.x + ( ( (i % inventory->cols) * dim.x) ), 
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

static void ui_inventory_init_buttons(Inventory* inventory)
{
	const u32 len = TOTAL_INVENTORY_BUTTONS;
	for(i32 i = 0; i < TOTAL_INVENTORY_BUTTONS; i++)
	{
		InventoryButton* button = &inventory->buttons[i];
		(*button) = ui_buttons_table[i];
		button->hitbox = ui_buttons_table[i].hitbox;
		Vector2 offset = { button->hitbox.x, button->hitbox.y };
		Vector2 inv_pos = { inventory->hitbox.x + inventory->hitbox.width, 
							inventory->hitbox.y + inventory->hitbox.height };

		button->hitbox = (Rectangle) {
			.x = inv_pos.x - offset.x,
			.y = inv_pos.y - offset.y,
			.width = button->hitbox.width,
			.height = button->hitbox.height,
		};
	}
}



Inventory* ui_inventory_new(InventoryType type)
{
	Inventory* inv_new = malloc(sizeof(Inventory));
	InventoryData data = inventory_data_table[type];
	Vector2 pos = { data.hitbox.x, data.hitbox.y };
	Vector2 dim = { data.hitbox.width, data.hitbox.height };
	Vector2 screen_pos = gfx_to_monitor_vector( (Vector2) { pos.x - dim.x, pos.y } );
	*inv_new = (Inventory) {
		.cols = data.cols,
		.rows = data.rows,
		.hitbox = { screen_pos.x, screen_pos.y, dim.x, dim.y },
		.mode = INVENTORY_MODE_NONE,
		.moved_id = -1,
		.focus_id = -1,
		.buttons = {0},
		.active = false,
		.cell_offset = data.cell_offset,
		.tick = data.tick,
	};
	ui_inventory_init_cells(inv_new);
	if(type == INVENTORY_TYPE_PLAYER)
		ui_inventory_init_buttons(inv_new);


	if(type == INVENTORY_TYPE_PLAYER)
	{
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
	}

	return inv_new;
}

void ui_inventory_destroy(Inventory* inventory)
{
	free(inventory);
}

static InventoryCell ui_inventory_get_cell(Inventory* inventory, Vector2 pos)
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

static void ui_inventory_cell_equip_item(InventoryCell* cell, Inventory* inventory, GameState* state)
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

static void ui_inventory_cell_enable_move(InventoryCell* cell, Inventory* inventory, GameState* state)
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

static void ui_inventory_cell_delete_item(InventoryCell* cell, Inventory* inventory, GameState* state)
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

static void ui_inventory_cell_tick(InventoryCell* cell, Inventory* inventory, GameState* state)
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

static void ui_inventory_switch_items(Inventory* inventory, GameState* state)
{
	i32 focus_id = inventory->focus_id;
	i32 moved_id = inventory->moved_id;
	Item tmp = inventory->cells[focus_id].item;
	inventory->cells[focus_id].item = inventory->cells[moved_id].item;
	inventory->cells[moved_id].item = tmp;
}

static void ui_inventory_move_mode(Inventory* inventory, GameState* state)
{
	if(inventory->mode == INVENTORY_MODE_MOVE && IsMouseButtonReleased(0))
	{
		inventory->mode = INVENTORY_MODE_NONE;
		ui_inventory_switch_items(inventory, state);
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

void ui_inventory_player_toggle(Inventory* inventory, GameState* state)
{
	if(IsKeyPressed(KEY_I)) inventory->active = !inventory->active;
}

void ui_inventory_tick(Inventory* inventory, GameState* state)
{
	if(!inventory->active) return;

	const Vector2 mp = GetMousePosition();
	const u32 cap = ui_inventory_cap(inventory);

	for(i32 i = 0; i < cap; i++)
	{
		ui_inventory_cell_tick(&inventory->cells[i], inventory, state);
	}
	ui_inventory_buttons_tick(inventory, state, mp);
	ui_inventory_move_mode(inventory, state);
}



static void ui_inventory_cell_background_render(InventoryCell* cell, GameState* state)
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

static void ui_inventory_cell_render(InventoryCell cell, GameState* state)
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
					22,
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

static void ui_inventory_background_render(Inventory* inventory, GameState* state)
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

static void ui_item_cell_text_render(Inventory* inventory, GameState* state, const i32 id)
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

static void ui_inventory_render_moved_item(Inventory* inventory, GameState* state)
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

static void ui_inventory_render_buttons(Inventory* inventory, GameState* state)
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


void ui_tick_npc_menu(NPC* npc, GameState* state)
{
	NPCMenu* menu = &npc->menu;
	Inventory* player_inventory = state->player->inventory;
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && menu->choice != -1)
	{
		P_LOG("choice: %d\n", menu->choice);
		if(!strcmp(menu->option_strings[menu->choice], "Shop")) 
		{
			npc->inventory->active = true;
			player_inventory->active = true;
			
		}
		if(!strcmp(menu->option_strings[menu->choice], "Quests")) 
		{
			menu->quest_menu_active = true;
		}
		if(!strcmp(menu->option_strings[menu->choice], "Conversations"))
		{
			quest_register_entry(state->player, QUEST_OBJECTIVE_TALK, (QuestObjectiveData) {
					.talk.talk_to_type = npc->type,
					} );
		}
		menu->active = false;
	}
}



void ui_render_npc_menu(NPC* npc, GameState* state)
{
	Gfx* gfx = state->gfx;
	NPCMenu* menu = &npc->menu;
	const f32 font_size = 16.0f;
	const Vector2 npc_dim = map_convert_map_to_screen(npc->entity->data.dim, state->map);
	const Vector2 first_pos = Vector2Add(map_convert_map_to_screen(npc->entity->pos, state->map), 
			(Vector2) { npc_dim.x + 10.0, npc_dim.y / 2.0 });

	const f32 box_height = font_size + 2.0;
	const f32 box_width = 120.0;

	const Vector2 mouse_pos = GetMousePosition();

	i8 pick = -1;

	for(i32 i = 0; i < menu->len; i++)
	{
		const u8* str = menu->option_strings[i];
		const Vector2 str_len = MeasureTextEx(gfx->ui_font, str, font_size, 0.0);

		const Vector2 pos = { first_pos.x, first_pos.y + i * (font_size + 1) };

		const Rectangle box = { pos.x, pos.y, box_width, box_height }; 
		const Rectangle src = { 0, 0, 16, 16 }; 

		const Vector2 text_pos = { pos.x + box.width / 2.0 - str_len.x / 2.0, pos.y };

		Color tint = WHITE;

		if(AAB(box, mouse_pos))
		{
			tint = (Color) { 130, 130, 130, 100 };
			pick = i;
		}


		DrawTexturePro(state->gfx->texs[TEXTURE_GAME_UI], src, box, (Vector2) { 0 }, 0.0f, tint);
		//P_LOG("String length of option [%d]: (%f,%f)\n", i, str_len.x, str_len.y);

		ui_draw_text(str, text_pos, font_size, WHITE, WHITE, state);
	}

	menu->choice = pick;

}


/* UI QUESTING */

static const Color quest_class_to_color_table[] = {
	{255, 0, 0, 255},
	{255, 255, 0, 255},
	{0, 0, 0, 255},
	{0, 0, 0, 255},
};

static const Color quest_status_to_color_table[] = {
	{0, 0, 0, 255},
	{255, 255, 0, 255},
	{0, 0, 0, 255},
	{0, 0, 0, 255},
};

static const u8* quest_status_text[] = {
	"[Ongoing]",
	"[Complete]",
	"[Finished]",
};

static void ui_player_quest_get_objective_kill_string(Player* player, QuestObjective quest_objective, char* dst)
{
	strcpy(dst, "Slay: ");
	strcat(dst, entity_get_name(quest_objective.objective.kill.kill_type));
}

static void ui_player_quest_get_objective_talk_string(Player* player, QuestObjective quest_objective, char* dst)
{
	strcpy(dst, "Talk to: ");
	strcat(dst,	npc_get_name(quest_objective.objective.talk.talk_to_type));
}

static void (*const objective_to_string_function_table[])(Player* player, QuestObjective quest_objective, char* dst) = {
	ui_player_quest_get_objective_kill_string,
	ui_player_quest_get_objective_talk_string,
};

f32 ui_player_quest_render(Player* player, GameState* state, Quest* quest, f32 y_offset)
{
	f32 offset = y_offset;
	const f32 x_offset = 52.0;
	const f32 font_size = 14.0;

	u8 objective_string[COMMON_UI_LABEL_MAX_LEN];

	for(i32 i = 0; i < quest->data.objectives_len; i++)
	{
		if(i != 0) offset += font_size;
		QuestObjective objective = quest->data.objectives[i];
		const i32 target = objective.target;
		const i32 current = quest->quest_counters[i];
		objective_to_string_function_table[objective.type](player, objective, objective_string);
		ui_draw_text(
				TextFormat("%5.20s (%.2d/%.2d)", objective_string, current, target), 
				(Vector2) { x_offset, offset }, 
				font_size, 
				BLACK,
				WHITE,
				state);
	}
	offset += font_size;
	return offset;
}

void ui_player_quest_log_render(Player* player, GameState* state)
{
	QuestManager* qm = player->quest_manager;
	const u32 quest_amount = qm->len;

	const f32 x_offset = 50.0;
	f32 y_offset = 50.0;
	const f32 font_size = 20.0;

	char text[COMMON_UI_LABEL_MAX_LEN];
	for(i32 i = 0; i < quest_amount; i++)
	{
		Quest* quest = &qm->quests[i];
		const Color text_color = quest_class_to_color_table[quest->data.class];
		strcpy(text, quest->data.name);
		strcat(text, " ");
		strcat(text, quest_status_text[quest->status]);
		ui_draw_text(
				text, 
				(Vector2) { x_offset, y_offset }, 
				font_size, 
				BLACK,
				text_color,
				state);
		y_offset += font_size;
		y_offset = ui_player_quest_render(player, state, quest, y_offset);
	}
}

void ui_tick_npc_quest_menu(NPC* npc, GameState* state)
{
	NPCMenu* menu = &npc->menu;
	QuestManager* qm = state->player->quest_manager;
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && menu->quest_choice != -1)
	{
		QuestData quest_data = npc->quests.data[menu->quest_choice];
		Quest* player_quest = quest_manager_get_quest(qm, quest_data.type);
		if(player_quest != NULL)
		{
			switch(player_quest->status)
			{
				case QUEST_STATUS_COMPLETE: 
					npc_finish_quest(npc, player_quest, menu->quest_choice, state);
					break;
				case QUEST_STATUS_ACCEPTED:
					P_LOG("Quest Already Accepted\n");
					break;
			}
		} 
		else
		{
			quest_manager_push_quest(qm, (Quest) {
					.status = QUEST_STATUS_ACCEPTED,
					.data = quest_data_table[quest_data.type],
					.quest_counters = { 0 },
					.quest_giver = npc->type,
					} );
		}
	}
}

void ui_render_npc_quest_menu(NPC* npc, GameState* state)
{
	Gfx* gfx = state->gfx;
	QuestManager* qm = state->player->quest_manager;
	NPCQuestData* quests = &npc->quests;

	const f32 font_size = 16.0f;
	const Vector2 npc_dim = map_convert_map_to_screen(npc->entity->data.dim, state->map);
	const Vector2 first_pos = Vector2Add(map_convert_map_to_screen(npc->entity->pos, state->map), 
			(Vector2) { npc_dim.x + 10.0, npc_dim.y / 2.0 });

	const f32 box_height = font_size + 2.0;
	const f32 box_width = 240.0;

	const Vector2 mouse_pos = GetMousePosition();

	u8 str[COMMON_UI_LABEL_MAX_LEN];

	i8 pick = -1;
	i32 completed_counter = 0;

	for(i32 i = 0; i < quests->len; i++)
	{
		const Vector2 pos = { first_pos.x, first_pos.y + (i - completed_counter) * (font_size + 1) };
		const Rectangle box = { pos.x, pos.y, box_width, box_height }; 
		const Rectangle src = { 0, 0, 16, 16 }; 
		Color tint = WHITE;

		if(i == 0) 
		{
			Rectangle title_box = { box.x, box.y - font_size, box.width, box.height };
			const u8* title_str = "[Quests]";
			const Vector2 title_str_len = MeasureTextEx(gfx->ui_font, title_str, font_size, 0.0);

			const Vector2 title_text_pos = { pos.x + title_box.width / 2.0 - title_str_len.x / 2.0, pos.y - font_size};

			DrawTexturePro(state->gfx->texs[TEXTURE_GAME_UI], src, title_box, (Vector2) { 0 }, 0.0f, tint);
			ui_draw_text(title_str, title_text_pos, font_size, WHITE, WHITE, state);
		}

		if(quests->completed[i]) 
		{
			completed_counter ++;
			continue;
		}
		//const u8* str = quests->data[i].name;
		Color text_tint = WHITE;

		strcpy(str, "");
		for(i32 j = 0; j < qm->len; j++)
		{
			if(quests->data[i].type == qm->quests[j].data.type)
			{
				if(qm->quests[j].status == QUEST_STATUS_COMPLETE) 
				{
					text_tint = (Color) { 10, 210, 10, 255 };
				}
				if(qm->quests[j].status == QUEST_STATUS_ACCEPTED) 
				{
					text_tint = (Color) { 20, 20, 210, 255 };
				}
				strcpy(str, quest_status_text[qm->quests[j].status]);
				strcat(str, " ");
				j = qm->len;
			}
		}
		strcat(str, quests->data[i].name);

		const Vector2 str_len = MeasureTextEx(gfx->ui_font, str, font_size, 0.0);

		const Vector2 text_pos = { pos.x + box.width / 2.0 - str_len.x / 2.0, pos.y };

		

		if(AAB(box, mouse_pos))
		{
			pick = i;
			tint = (Color) { 130, 130, 130, 100 };
		}



		DrawTexturePro(state->gfx->texs[TEXTURE_GAME_UI], src, box, (Vector2) { 0 }, 0.0f, tint);
		//P_LOG("String length of option [%d]: (%f,%f)\n", i, str_len.x, str_len.y);
		ui_draw_text(str, text_pos, font_size, text_tint, text_tint, state);
	}
	npc->menu.quest_choice = pick;
}

/* UI QUESTING */




void ui_render(GameState* state)
{
	Player* player = state->player;
	DynList* npcs = state->map->npcs;
	Inventory* player_inventory = state->player->inventory;
	ui_player_quest_log_render(player, state);

	npcs_ui_render(npcs, state);

	if(player_inventory->active)
	{
		ui_inventory_render(player_inventory, state);
	}
	gfx_render_mouse(state->gfx);
}

//TODO money logic
static void ui_shop_buy_item(InventoryCell* cell, Inventory* shop_inventory, GameState* state)
{
	Item item_to_buy = cell->item;
	const Vector2 mouse_cords = GetMousePosition();
	Inventory* player_inventory = state->player->inventory;
	//Player* p = state->player;

	//P_LOG("%f %f\n", cell->hitbox.x, cell->hitbox.y);
	if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && 
			AAB(cell->hitbox, mouse_cords) )
	{
		P_LOG("Added\n");
		ui_inventory_add_item(player_inventory, item_to_buy);
	}
}

static void ui_shop_cell_tick(InventoryCell* cell, Inventory* shop_inventory, GameState* state)
{
	const Vector2 mouse_cords = GetMousePosition();
	if(AAB(cell->hitbox, mouse_cords))
	{
		cell->focused = cell->id;
	} 
	else 
	{
		cell->focused = -1;
	}

	ui_shop_buy_item(cell, shop_inventory, state);

}

void ui_shop_tick(Inventory* shop_inventory, GameState* state)
{
	if(!state->player->inventory->active) return;

	const Vector2 mp = GetMousePosition();
	const u32 cap = ui_inventory_cap(shop_inventory);

	for(i32 i = 0; i < cap; i++)
	{
		ui_shop_cell_tick(&shop_inventory->cells[i], shop_inventory, state);
	}
}

static void ui_smith_cell_tick(InventoryCell* cell, Inventory* smith_inventory, GameState* state)
{
	const Vector2 mouse_cords = GetMousePosition();
	cell->focused = -1;
	if(AAB(cell->hitbox, mouse_cords) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && cell->item.type != ITEM_TYPE_NONE)
	{
		ui_inventory_add_item(state->player->inventory, cell->item);
		cell->item = ui_inventory_get_empty_cell();
	} 

}

static void ui_smith_select_item(Inventory* smith_inventory, GameState* state)
{
	Inventory* player_inventory = state->player->inventory;
	const Vector2 mouse_cords = GetMousePosition();
	for(i32 i = 0; i < ui_inventory_cap(player_inventory); i++)
	{
		InventoryCell* cell = &player_inventory->cells[i];
		if(AAB(cell->hitbox, mouse_cords) &&
				IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			ui_inventory_swap_items(&smith_inventory->cells[0], cell);
		}
	}
}

void ui_smith_tick(Inventory* smith_inventory, GameState* state)
{
	if(!state->player->inventory->active) return;
	const Vector2 mp = GetMousePosition();
	const u32 cap = ui_inventory_cap(smith_inventory);

	for(i32 i = 0; i < cap; i++)
	{
		ui_smith_cell_tick(&smith_inventory->cells[i], smith_inventory, state);
	}
	ui_smith_select_item(smith_inventory, state);
}


