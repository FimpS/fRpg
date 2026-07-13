#pragma once

#include "dynList.h"
#include "item.h"
#include "state.h"
#include "../lib/types.h"
#include "../lib/v2.h"

#define COMMON_UI_LABEL_MAX_LEN 64

void ui_wrap_text_render(const u8* text, Vector2 pos, const i32 max_width, const u32 padding, const u32 font_size, GameState* state);
void ui_text_box_render(const u8* title, const u8* text, Vector2 pos, GameState* state);

Inventory* ui_inventory_new(InventoryType type);

void ui_inventory_add_item(Inventory* inventroy, Item item);
void ui_inventory_tick(Inventory* inventory, GameState* state);
void ui_shop_tick(Inventory* shop_inventory, GameState* state);
void ui_smith_tick(Inventory* smith_inventory, GameState* state);
void ui_inventory_render(Inventory* inventory, GameState* state);
void ui_inventory_player_toggle(Inventory* inventory, GameState* state);
void ui_render_npc_menu(NPC* npc, GameState* state);
void ui_render_npc_quest_menu(NPC* npc, GameState* state);
void ui_tick_npc_menu(NPC* npc, GameState* state);

void ui_render(GameState* state);




void ui_inventory_toggle_fire(Inventory* inventory, GameState* state);
void ui_inventory_sort_cells(Inventory* inventory, GameState* state);
