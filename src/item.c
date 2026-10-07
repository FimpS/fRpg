
#include "item.h"
#include "item_data.h"


EntityEquipment* entity_equipment_new()
{
	EntityEquipment* equipment = malloc(sizeof(EntityEquipment));

	const u32 len = MAX_SLOTS_IN_EQUIPMENT;
	for(i32 i = 0; i < len; i++)
	{
		equipment->items[i] = item_init(ITEM_TYPE_NONE, (ItemEnchant) { 0 });
	}

	return equipment;
}

Item item_init(ItemType type, ItemEnchant enchant)
{
	return (Item) {
		.data = item_info_table[type],
		.type = type,
		.enchant = enchant,
	};
}

Item item_empty_init()
{
	return (Item) {
		.data = item_info_table[ITEM_TYPE_NONE],
		.type = ITEM_TYPE_NONE,
		.enchant = { 0 },
	};
}

static i32 item_get_equip_index(EntityEquipment* equipment, ItemClass item_class)
{
	i32 final_index = -1;
	const u32 ring_len = 4;
	switch(item_class)
	{
		case ITEM_CLASS_HELMET:
			final_index = ITEM_SLOT_HEAD;
			break;
		case ITEM_CLASS_RING:
			final_index = ITEM_SLOT_RING1;
			for(i32 i = 0; i < ring_len; i++)
			{
				if(equipment->items[ITEM_SLOT_RING1 + i].type == ITEM_TYPE_NONE)
				{
					final_index = ITEM_SLOT_RING1 + i;
				}
			}
			break;
		case ITEM_CLASS_NONE:
			final_index = -1;
			break;
	}
	return final_index;
}

Item item_equip_to_equipment_slot(Item* item, EntityEquipment* equipment)
{
	const ItemClass class = item->data.class;
	const i32 equip_index = item_get_equip_index(equipment, class);
	if(equip_index == -1) return item_empty_init();

	Item return_item = equipment->items[equip_index];
	memcpy(&equipment->items[equip_index], item, sizeof(Item));

	return return_item;
}


















