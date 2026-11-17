
#include "item.h"
#include "item_data.h"
#include "entity.h"


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

//TODO Instead of mapping the equipment to the specific equipmentslot, let's isntead adda a field and have the ring1,2,3,4 be an exception
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
	reciever->physical += 	power.physical;
	reciever->magic += 		power.magic;
	reciever->fire += 		power.fire;
	reciever->bolt += 		power.bolt;
	reciever->holy += 		power.holy;
	reciever->dark += 		power.dark;
}

void elemental_power_sub(ElementalPower* reciever, const ElementalPower power)
{
	reciever->physical -= 	power.physical;
	reciever->magic -= 		power.magic;
	reciever->fire -= 		power.fire;
	reciever->bolt -= 		power.bolt;
	reciever->holy -= 		power.holy;
	reciever->dark -=	 	power.dark;
}

void elemental_power_log(const ElementalPower power)
{
	P_LOG("Physical: %d\n", 	power.physical);
	P_LOG("Magic: %d\n", 		power.magic);
	P_LOG("Fire: %d\n", 		power.fire);
	P_LOG("Bolt: %d\n", 		power.bolt);
	P_LOG("Holy: %d\n", 		power.holy);
	P_LOG("Dark: %d\n", 		power.dark);
}

ItemEnchant item_get_drop_enchant(Entity* self, GameState* state)
{
	//TODO
	

	return (ItemEnchant) { .level = 1 };
}

static Vector2 item_drop_pos(Entity* entity, GameState* state)
{
	f32 lower = -1.0, upper = 1.0;
	const f32 delta = 0.2;
	Vector2 pos;
	const u32 max_tries = 4;
	for(i32 i = 0; i < max_tries; i++)
	{
		const Vector2 offset = { frand(lower, upper), frand(lower, upper) };
		pos = (Vector2) { entity->pos.x + offset.x, entity->pos.y + offset.y};
		lower += delta;
		upper -= delta;
		if(!map_get_tile(state->map, Vector2V2(pos)).solid) break;
	}
	P_LOG("Drop pos: (%f,%f)\n", pos.x, pos.y);
	return pos;
}

void item_drop_push(Entity* entity, GameState* state)
{
	DynList* entities = state->map->entities;
	if(entity->type == ENTITY_ITEM_DROP) return;

	const u32 len = entity_drop_list_len(&entity->data.drops);
	for(i32 i = 0; i < len; i++)
	{
		ItemType type = *entity_drop_list_get(&entity->data.drops, i);
		//Item item = item_init(type, item_get_drop_enchant(entity, state)); Do this in death i guess
		Entity* item_entity = entity_new(ENTITY_ITEM_DROP, item_drop_pos(entity, state));
	
		entity_drop_list_push(&item_entity->data.drops, type);
		item_entity->stats.level = entity->stats.level;
		item_entity->target = state->player->entity;
		item_entity->data.name = item_info_table[type].name;
		dynList_push(entities, item_entity);
	}
	
}

bool item_can_equip(Item* item, Entity* equiper, GameState* state)
{
	return  equiper->stats.level >= item->data.min_lvl &&
			item->data.equipment.equipable;
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


















