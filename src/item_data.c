#include <stdbool.h>

#include "item_data.h"

const ItemData item_info_table[] = {
	(ItemData) {
		.name = "None",  
		.description = "No Description",
		.class = ITEM_CLASS_NONE,
		.stackable = false,
	},
	(ItemData) {
		.name = "Placeholder Ring",
		.description = "Description\nfor Place\nholder the gods may not test us yet\nchildren but its for us Ring",
		.class = ITEM_CLASS_RING,
		.stackable = false,
	},
	(ItemData) {
		.name = "Placeholder Helmet",
		.description = "Description0000 for Placeholder Helmet",
		.class = ITEM_CLASS_HELMET,
		.stackable = true,
	},
	(ItemData) {
		.name = "Placeholder Sword",
		.description = "Some kind of god of war sword",
		.class = ITEM_CLASS_WEAPON,
		.equipment = {
			.attack = { 100, 0, 0, 0, 0, 0 },
			.defense = { 0 },
			.equipable = true,
		},
	},
};
