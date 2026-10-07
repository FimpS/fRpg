#include <stdbool.h>

#include "item_data.h"

const ItemInfo item_info_table[] = {
	(ItemInfo) {
		.name = "None",  
		.description = "No Description",
		.class = ITEM_CLASS_NONE,
		.stackable = false,
	},
	(ItemInfo) {
		.name = "Placeholder Ring",
		.description = "Description\nfor Place\nholder the gods may not test us yet\nchildren but its for us Ring",
		.class = ITEM_CLASS_RING,
		.stackable = false,
	},
	(ItemInfo) {
		.name = "Placeholder Helmet",
		.description = "Description0000 for Placeholder Helmet",
		.class = ITEM_CLASS_HELMET,
		.stackable = true,
	},
};
