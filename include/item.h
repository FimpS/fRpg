

#include "struct.h"
#include "enum.h"

void elemental_power_add(ElementalPower* reciever, const ElementalPower power);
void elemental_power_sub(ElementalPower* reciever, const ElementalPower power);
void elemental_power_log(const ElementalPower power);

void item_equip_stats(Item* item, GameState* state);
void item_unequip_stats(Item* item, GameState* state);

EntityEquipment* entity_equipment_new();
Item item_init(ItemType type, ItemEnchant enchant);

/* Checks if user matches required level, and if item is equipable */
bool item_can_equip(Item* item, Entity* equiper, GameState* state);
Item item_empty_init();

/* pushes a itemdrop entities based on 'entity' drop types */
void item_drop_push(Entity* entity, GameState* state);
ItemEnchant item_get_drop_enchant(Entity* self, GameState* state);

/* Replaces current item in Equipment-inventory, returns the item that 
 * previously occupied that spot or nothing if slot was already empty */
Item item_equip_to_equipment_slot(Item* item, EntityEquipment* equipment, GameState* state);
