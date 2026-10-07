

#include "struct.h"
#include "enum.h"


EntityEquipment* entity_equipment_new();
Item item_init(ItemType type, ItemEnchant enchant);
Item item_empty_init();

Item item_equip_to_equipment_slot(Item* item, EntityEquipment* state);
