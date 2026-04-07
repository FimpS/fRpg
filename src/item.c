#include "item.h"

const ItemInfo item_info_table[] = {
	(ItemInfo) {
		.name = "None", //Here i dont get it 
		.description = "No Description",
		.class = ITEM_CLASS_NONE,
	},
	(ItemInfo) {
		.name = "Placeholder Ring",
		.description = "Description for Placeholder Ring",
		.class = ITEM_CLASS_RING,
	},
	(ItemInfo) {
		.name = "Placeholder Helmet",
		.description = "Description for Placeholder Helmet",
		.class = ITEM_CLASS_HELMET,
	},
};
