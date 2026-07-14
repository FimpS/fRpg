#include "npc.h"
#include "player.h"
#include "shop_data.h"
#include "quest_data.h"
#include "ui.h"

#define NPC_INTERACTION_RANGE 3.0


void npc_tick(NPC* self, GameState* state) //TODO change to npc->tick(), when adding different npcs
{
	const Vector2 mouse_cords = map_get_mouse_cords(state->map);

	if(self->menu.active)
	{
		ui_tick_npc_menu(self, state);
	}
	if(self->menu.quest_menu_active)
	{
		ui_tick_npc_quest_menu(self, state);
	}

	if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) &&
			entity_AAB(self->entity, mouse_cords) &&
			entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE))
	{
		self->menu.active = true;
	}

	if(IsKeyPressed(KEY_F1) || 
			(!entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE + 2.0)) )
	{
		self->inventory->active = false;
		state->player->inventory->active = false;
		self->menu.active = false;
		self->menu.quest_menu_active = false;
		self->menu.choice = -1;
		self->menu.quest_choice = -1;
	}

#if 0
	if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) &&
			entity_AAB(self->entity, mouse_cords) &&
			entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE))
	{
		quest_register_entry(state->player, QUEST_OBJECTIVE_TALK, (QuestObjectiveData) { //TODO Should be on talk button
				.talk.talk_to_type = self->type,
				} );

		self->inventory->active = true;
		state->player->inventory->active = true;
	}

	if(IsKeyPressed(KEY_F1) || 
			(!entity_in_range(self->entity->pos, state->player->entity->pos, NPC_INTERACTION_RANGE + 2.0) && 
			 self->inventory->active) )
	{
		self->inventory->active = false;
		state->player->inventory->active = false;
	}

	if(IsKeyPressed(KEY_F2) &&
			entity_in_range(self->entity->mid_pos, state->player->entity->mid_pos, NPC_INTERACTION_RANGE) )
	{
		if(quest_is_complete(state->player, QUEST_TYPE_KILL_5_IMPS))
		{
			quest_manager_delete_quest(state->player->quest_manager, QUEST_TYPE_KILL_5_IMPS);
		}
	}

	if(IsKeyPressed(KEY_SPACE) &&
			entity_in_range(self->entity->mid_pos, state->player->entity->mid_pos, NPC_INTERACTION_RANGE) )
	{
		quest_manager_push_quest(state->player->quest_manager, (Quest) {
				.data = quest_data_table[QUEST_TYPE_PLACEHOLDER],
				.quest_giver = self->type,
				.status = QUEST_STATUS_ACCEPTED,
				.quest_counters = { 0 },
				} );
	}
#endif
}

static void npc_give_quest_reward(NPC* npc, Quest* quest, GameState* state)
{
	Inventory* player_inventory = state->player->inventory;
	QuestManager* qm = state->player->quest_manager;
	//player->experience += quest->data.reward.experience;
	const u32 len = quest->data.reward.item_rewards_len;
	P_LOG("Quest Reward item count: %d\n", len);
	for(i32 i = 0; i < len; i++)
	{
		P_LOG("Quest Reward ItemType: %d\n", quest->data.reward.item_rewards[i]);
		ui_inventory_add_item(player_inventory, (Item) {
				.type = quest->data.reward.item_rewards[i],
				.info = item_info_table[ quest->data.reward.item_rewards[i] ],
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
			.info = item_info_table[type],
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
		.choice = -1,
		.quest_menu_active = false,
		.quest_choice = -1,
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
