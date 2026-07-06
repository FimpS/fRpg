
#include "player.h"
#include "entity.h"
#include "quest_data.h"

QuestManager* player_quest_manager_new(Player* player)
{
	QuestManager* manager = malloc(sizeof(QuestManager));
	*manager = (QuestManager) {
		.len =  3,
		.quests = { 0 },
	};

#if 1
	manager->len = 3;
	for(i32 i = 0; i < manager->len; i++)
	{
		manager->quests[i] = (Quest) {
			.data = quest_data_table[QUEST_TYPE_KILL_5_IMPS],
			.status = QUEST_STATUS_ACCEPTED,
		};
	}
#endif 

	return manager;
}


Player* player_new()
{
	Player* player = malloc(sizeof(Player));
	player->entity = entity_new(ENTITY_PLAYER, (Vector2) { 10.0, 10.0 } );
	player->inventory = ui_inventory_new(INVENTORY_TYPE_PLAYER);	
	player->quest_manager = player_quest_manager_new(player);

	for(i32 i = 0; i < player->quest_manager->len; i++)
	{
		Quest q = player->quest_manager->quests[i];
		P_LOG("Quest type: %d, with status %d\n", q.data.class, q.status);
	}

	return player;
}

void player_tick(Player* player, GameState* state)
{
	Entity* player_entity = player->entity;

	//ui_inventory_tick(player->inventory, state);
	ui_inventory_player_toggle(player->inventory, state);
	ui_inventory_tick(player->inventory, state);
	if(player_entity->state.tick != NULL) player_entity->state.tick(player_entity, state);
}

void player_render(Player* player, GameState* state)
{
	entity_render(player->entity, state);
}


void player_destroy(Player* player)
{
	entity_destroy(player->entity);
	free(player->quest_manager);
	free(player->inventory);
	free(player);
}
