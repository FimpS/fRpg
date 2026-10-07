#include "npc.h"
#include "player.h"
#include "shop_data.h"
#include "quest_data.h"
#include "ui.h"

#define NPC_INTERACTION_RANGE 3.0


static bool npc_busy(NPC* self)
{
	return self->menu.quest_menu_active || self->menu.active || self->inventory->active || self->menu.talking;
}

static void npc_menu_disable(NPC* self, GameState* state)
{
	self->inventory->active = false;
	state->player->inventory->active = false;
	self->menu.active = false;
	self->menu.quest_menu_active = false;
	self->menu.choice = 0;
	self->menu.quest_choice = 0;
}

static void npc_menu_enable(NPC* self, GameState* state)
{
	self->menu.active = true;
	self->menu.choice = 0;
}

static bool npc_menu_can_disable(NPC* self, GameState* state)
{
	return npc_busy(self) && 
					(IsKeyPressed(KEY_F1) || 
		 			(!entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE + 2.0)));
}

static bool npc_menu_can_enable(NPC* self, GameState* state)
{
	const Vector2 mouse_cords = map_get_mouse_cords(state->map);
	return (IsKeyPressed(KEY_SPACE) || (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) &&
				entity_AAB(self->entity, mouse_cords))) &&
				!npc_busy(self) &&
				entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE);
}

static void npc_menu_toggle(NPC* self, GameState* state)
{
	if(npc_menu_can_enable(self, state))
	{
		npc_menu_enable(self, state);
	}

	if(npc_menu_can_disable(self, state))
	{
		npc_menu_disable(self, state);
	}
}

void npc_tick(NPC* self, GameState* state) //TODO change to npc->tick(), when adding different npcs
{
	const Vector2 mouse_cords = map_get_mouse_cords(state->map);

	 
	if(self->menu.active)
	{
		ui_tick_npc_menu(self, state);
	}
	else if(self->menu.quest_menu_active)
	{
		ui_tick_npc_quest_menu(self, state);
	}
	
	npc_menu_toggle(self, state);


}

static void npc_give_quest_reward(NPC* npc, Quest* quest, GameState* state)
{
	Inventory* player_inventory = state->player->inventory;
	QuestManager* qm = state->player->quest_manager;
	QuestReward reward = quest_reward_table[quest->data.type];

	entity_gain_experience(state->player->entity, reward.experience);

	const u32 len = reward.item_rewards_len;
	for(i32 i = 0; i < len; i++)
	{
		ui_inventory_add_item(player_inventory, (Item) {
				.type = reward.item_rewards[i],
				.data = item_info_table[ reward.item_rewards[i] ],
				.enchant = 1,
				} );
	}
}

void npc_finish_quest(NPC* npc, Quest* quest, const i32 quest_index, GameState* state)
{
	QuestManager* qm = state->player->quest_manager;
	npc_give_quest_reward(npc, quest, state);
	quest_manager_delete_quest(qm, quest->data.type);
	npc->quests.completed[quest_index] = true;	
	state->player->quest_manager->completed[quest->data.type].completed = true;
}

bool npc_is_quest_available(NPC* npc, const QuestData* data, GameState* state)
{
	Player* player = state->player;
	QuestPrerequisite prerequisite = quest_prerequisite_table[ data->type ];

	//if(! (player->level >= data->prerequisite.player_level) ) return false;

	const u32 len = quest_pre_list_len(&prerequisite.quests);

	// Need to have quest length to 0 to disable prereqs
	for(i32 i = 0; i < len; i++)
	{
		QuestType type = *quest_pre_list_get(&prerequisite.quests, i);
		if(!player->quest_manager->completed[type].completed) return false;
	}
	return true;
}

void npcs_tick(DynList* npcs, GameState* state)
{
	const u32 len = npcs->len;

	for(i32 i = 0; i < len; i++)
	{
		NPC* npc = dynList_get(npcs, i);
		if(npc->inventory->active)
		{
			npc->inventory->tick(npc->inventory, state);
		}
		npc_tick(npc, state);
	}
}

void npcs_ui_render(DynList* npcs, GameState* state)
{
	const u32 len = npcs->len;

	for(i32 i = 0; i < len; i++)
	{
		NPC* npc = dynList_get(npcs, i);
		if(npc->menu.active)
		{
			ui_render_npc_menu(npc, state);	
		}
		if(npc->inventory->active) 
		{
			ui_inventory_render(npc->inventory, state);
		}

		if(npc->menu.quest_menu_active)
		{
			ui_render_npc_quest_menu(npc, state);
		}
	}

}

void npcs_render(DynList* npcs, GameState* state)
{
	const u32 len = npcs->len;

	for(i32 i = 0; i < len; i++)
	{
		NPC* npc = dynList_get(npcs, i);

		entity_render(npc->entity, state);
	}
}

static void npc_add_shop_stock(NPC* npc)
{
	Inventory* stock = npc->inventory;
	ShopData data = shop_data_table[npc->type];

	if(data.total_item >= TOTAL_INVENTORY_CELLS) data.total_item = TOTAL_INVENTORY_CELLS;

	for(i32 i = 0; i < data.total_item; i++)
	{
		ItemType type = data.shop_item_types[i];
		stock->cells[i].item = (Item) {
			.type = type,
			.data = item_info_table[type],
			.enchant = 1, //TODO
		};
	}
}

static NPCQuestData npc_quests_init(NPC* npc)
{
	const NPCType npc_type = npc->type;
	const NPCQuestDataTypes data = npc_quest_data_types_table[npc_type];
	const u32 len = data.len;
	NPCQuestData quests = { 0 };

	quests.len = len;
	for(i32 i = 0; i < len; i++)
	{
		quests.data[i] = quest_data_table[ data.types[i] ];
	}

	return quests;
}

static NPCMenu npc_menu_init(NPC* npc)
{
	const InventoryType inv_type = npc->data.inventory;
	const u32 has_quests = npc->quests.len;

	NPCMenu menu = (NPCMenu) {
		.active = false,
		.choice = 0,
		.quest_menu_active = false,
		.quest_choice = 0,
		.talking = false,
		.len = 0,
	};

	if(has_quests)
	{
		menu.option_strings[ menu.len ++ ] = "Quests";		
	}

	switch(inv_type)
	{
		case INVENTORY_TYPE_SHOP: 
			menu.option_strings[ menu.len ++ ] = "Shop";  //TODO check if this is actually safe
			break;
		case INVENTORY_TYPE_SMITH:
			menu.option_strings[ menu.len ++] = "Enchant";
			break;
		default: break;
	}

	menu.option_strings[ menu.len ++ ] = "Conversations";

	return menu;

}

const u8* npc_get_name(NPCType type)
{
	return entity_get_name(npc_data_table[type].entity);
}


//Add talk types here and for quests maybe ( saj DanyCide )
const NPCData npc_data_table[] = {
	{ENTITY_SHOP_NPC,		INVENTORY_TYPE_SHOP },
	{ENTITY_SHOP_NPC,		INVENTORY_TYPE_SHOP },
	{ENTITY_SHOP_NPC,		INVENTORY_TYPE_SMITH },
};


NPC* npc_new(NPCType type, Vector2 pos)
{
	NPC* npc = malloc(sizeof(NPC));
	npc->type = type;
	npc->data = npc_data_table[type];
	npc->text = npc_text_data_table[type];

	npc->entity = entity_new(npc->data.entity, pos);
	npc->inventory = ui_inventory_new(npc->data.inventory);
	npc->quests = npc_quests_init(npc);

	npc->menu = npc_menu_init(npc);

	for(i32 i = 0; i < npc->menu.len; i++)
	{
		P_LOG("[%d] - %s\n", i, npc->menu.option_strings[i]);
	}

	if(npc->data.inventory == INVENTORY_TYPE_SHOP)
	{
		npc_add_shop_stock(npc);
	}

	return npc;
}

void npc_destroy(NPC* npc)
{
	entity_destroy(npc->entity);
	free(npc->inventory);
	free(npc);
}
