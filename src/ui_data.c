

#include "ui.h"
#include "ui_data.h"


const InventoryButton ui_buttons_table[] = {
	(InventoryButton) {
		.hitbox = {375, 115, 64, 64},
		.src_rec = {0, 48, 16, 16},
		.hover_src_rec = {16, 48, 16, 16},
		.on_click = ui_inventory_sort_cells,
	},
	(InventoryButton) {
		.hitbox = {375, 189, 64, 64},
		.src_rec = {32, 48, 16, 16},
		.hover_src_rec = {48, 48, 16, 16},
		.on_click = ui_inventory_toggle_fire,
	},
};

const Color enchant_to_color_table[] =
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

const Rectangle inventory_item_table[] =
{
	{0, 0, 16, 16},
	{16, 0, 16, 16},
	{16, 16, 16, 16},
};

const InventoryData inventory_data_table[] = {
	{ui_inventory_tick, 				{1820,100,400,800},	 		10,		6,		{300,550} },
	{ui_shop_tick,						{450,000,400,400},			5,		6,		{350,275} },
	{ui_smith_tick, 					{450,600,400,400},			1,		1,		{350,275} },
};

