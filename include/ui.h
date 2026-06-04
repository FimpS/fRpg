#pragma once

#include "dynList.h"
#include "item.h"
#include "state.h"
#include "../lib/types.h"
#include "../lib/v2.h"

typedef struct GameState GameState;

#define TOTAL_INVENTORY_CELLS 60
#define TOTAL_INVENTORY_BUTTONS 2

typedef enum
{
	INVENTORY_MODE_NONE,
	INVENTORY_MODE_MOVE,
	INVENTORY_MODE_DELETE,
} InventoryMode;

typedef struct InventoryCell
{
	Item item;
	Rectangle hitbox;
	i32 focused;
	u32 id;
	bool stackable; //maybe not usefull
	u32 amount;
} InventoryCell;

typedef struct Inventory Inventory;

typedef struct InventoryButton
{
	Rectangle hitbox;
	Rectangle src_rec;
	Rectangle hover_src_rec;
	bool hovered;
	void (*on_click)(Inventory* inventroy, GameState* state);
} InventoryButton;

typedef struct Inventory
{
	InventoryCell cells[TOTAL_INVENTORY_CELLS];
	Rectangle hitbox;
	u32 cols;
	u32 rows;

	i32 focus_id;
	i32 moved_id;
	InventoryMode mode;

	InventoryButton buttons[TOTAL_INVENTORY_BUTTONS];
} Inventory;


void ui_wrap_text_render(const u8* text, Vector2 pos, const i32 max_width, const u32 padding, const u32 font_size, GameState* state);
void ui_text_box_render(const u8* title, const u8* text, Vector2 pos, GameState* state);

Inventory* ui_inventory_new();

void ui_inventory_add_item(Inventory* inventroy, Item item);
void ui_inventory_tick(Inventory* inventory, GameState* state);
void ui_inventory_render(Inventory* inventory, GameState* state);

void ui_render(GameState* state);

