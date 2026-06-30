
#include "shop_data.h"
#include "item.h"

const NPCShopType npc_to_shop_table[] = {
	[NPC_TYPE_TOWN_MERCHANT] = NPC_SHOP_TOWN,
	[NPC_TYPE_CAVE_MERCHANT] = NPC_SHOP_CAVE,
};

const ShopData shop_data_table[] = {
// 	itemcount		Items
	{2,				{ITEM_TYPE_HELMET, ITEM_TYPE_PLACEHOLDER,	} },
	{1,				{ITEM_TYPE_HELMET,	} },
};
