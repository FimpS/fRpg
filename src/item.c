
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
		case ITEM_CLASS_WEAPON:
			final_index = ITEM_SLOT_WEAPON;
			break;
		case ITEM_CLASS_HELMET:
			final_index = ITEM_SLOT_HEAD;
			break;
		case ITEM_CLASS_TORSO:
			final_index = ITEM_SLOT_TORSO;
			break;
		case ITEM_CLASS_HANDS:
			final_index = ITEM_SLOT_HANDS;
			break;
		case ITEM_CLASS_LEGS:
			final_index = ITEM_SLOT_LEGS;
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
		default: final_index = -1; break;
	}
	return final_index;
}

void elemental_power_add(ElementalPower* reciever, const ElementalPower power)
{
	reciever->physical += power.physical;
	reciever->magic += power.magic;
	reciever->fire += power.fire;
	reciever->bolt += power.bolt;
	reciever->holy += power.holy;
	reciever->dark += power.dark;
}

void elemental_power_sub(ElementalPower* reciever, const ElementalPower power)
{
	reciever->physical -= power.physical;
	reciever->magic -= power.magic;
	reciever->fire -= power.fire;
	reciever->bolt -= power.bolt;
	reciever->holy -= power.holy;
	reciever->dark -= power.dark;
}

void elemental_power_log(const ElementalPower power)
{
	P_LOG("Physical: %d\n", power.physical);
	P_LOG("Magic: %d\n", power.magic);
	P_LOG("Fire: %d\n", power.fire);
	P_LOG("Bolt: %d\n", power.bolt);
	P_LOG("Holy: %d\n", power.holy);
	P_LOG("Dark: %d\n", power.dark);
}

void item_equip_stats(Item* item, GameState* state)
{
	Entity* player = state->player->entity;

	elemental_power_add(&player->stats.attack, item->data.equipment.attack);
	elemental_power_add(&player->stats.defense, item->data.equipment.defense);
}

void item_unequip_stats(Item* item, GameState* state)
{
	Entity* player = state->player->entity;

	elemental_power_sub(&player->stats.attack, item->data.equipment.attack);
	elemental_power_sub(&player->stats.defense, item->data.equipment.defense);
}

Item item_equip_to_equipment_slot(Item* item, EntityEquipment* equipment, GameState* state)
{
	const ItemClass class = item->data.class;
	const i32 equip_index = item_get_equip_index(equipment, class);
	if(equip_index == -1) return item_empty_init();

	Item return_item = equipment->items[equip_index];
	item_unequip_stats(&return_item, state);

	memcpy(&equipment->items[equip_index], item, sizeof(Item));
	item_equip_stats(item, state);

	return return_item;
}


















