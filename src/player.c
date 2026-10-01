
#include "player.h"
#include "entity.h"
#include "skilltree.h"
#include "quest_data.h"
#include "skills.h"

static bool quests_are_updated = false;

QuestManager* player_quest_manager_new(Player* player)
{
	QuestManager* manager = malloc(sizeof(QuestManager));
	*manager = (QuestManager) {
		.quests = quest_manager_list_init(),
		.completed = { 0 },
	};

	for(i32 i = 0; i < MAX_QUESTS_IN_GAME; i++)
	{
		manager->completed[i].type = i;
	}
	//TODO load finished quests from savefile

#if 0
	for(i32 i = 0; i < 1; i++)
	{
		 quest_manager_push_quest(manager, (Quest) {
			.data = quest_data_table[QUEST_TYPE_KILL_5_IMPS],
			.quest_counters = { 0 },
			.status = QUEST_STATUS_ACCEPTED,
			.quest_giver = NPC_TYPE_TOWN_MERCHANT,
		} );
	}
#endif 

	return manager;
}

bool quest_is_complete(Player* player, QuestType key)
{
	QuestManager* qm = player->quest_manager;
	const u32 manager_len = quest_manager_list_len(&qm->quests);
	for(i32 i = 0; i < manager_len; i++)
	{
		Quest* q = &qm->quests.data[i];
		P_LOG("Quest of Type %d; is complete: %d\n", q->data.type, q->status);
		if(q->data.type == key && q->status == QUEST_STATUS_COMPLETE) return true;
	}

	return false;
}

void quest_check_completion(Player* player) //Check after and if any of the quest_register functions executed, maybe global var
{
	QuestManager* qm = player->quest_manager;
	const u32 manager_len = quest_manager_list_len(&qm->quests);

	for(i32 i = 0; i < manager_len; i++)
	{
		Quest* q = &qm->quests.data[i];
		u32 objective_counter = 0;
		for(i32 j = 0; j < q->data.objectives_len; j++)
		{
			P_LOG("Checking if quest[%d]: %d is complete\n", i, q->data.type);
			if(q->data.objectives[j].target <= q->quest_counters[j]) 
			{
				objective_counter ++;
			}
		}
		if(objective_counter == q->data.objectives_len)
		{
			q->status = QUEST_STATUS_COMPLETE;
		}
	}
}

static bool quest_objective_data_match(QuestObjective objective, QuestObjectiveData data)
{
	switch(objective.type)
	{
		case QUEST_OBJECTIVE_KILL: return objective.objective.kill.kill_type == data.kill.kill_type; break;
		case QUEST_OBJECTIVE_TALK: return objective.objective.talk.talk_to_type == data.talk.talk_to_type; break;
	}
	return false;
}

void quest_register_entry(Player* player, QuestObjectiveType type, QuestObjectiveData data) //Do this or some kind of registry that get checked at the end of tick
{
	QuestManager* qm = player->quest_manager;

	const u32 manager_len = quest_manager_list_len(&qm->quests);
	for(i32 i = 0; i < manager_len; i++)
	{
		Quest* q = &qm->quests.data[i];
		for(i32 j = 0; j < q->data.objectives_len; j++)
		{
			QuestObjective objective = q->data.objectives[j];	
			if(objective.type == type && quest_objective_data_match(objective, data) &&
				objective.target > q->quest_counters[j])
			{
				q->quest_counters[j] ++;
				P_LOG("Incremented counter for quest[%d]: %d, current counter[%d] = %d\n", 
						i,
						q->data.type, 
						j, 
						q->quest_counters[j]);
			}
		}
	}
	P_LOG("Entry happened\n");
	quests_are_updated = true;
}

void quest_manager_push_quest(QuestManager* manager, Quest quest)
{
	QuestManager* qm = manager;
	if(quest_manager_list_len(&qm->quests) >= MAX_QUESTS_IN_MANAGER)
	{
		P_ERROR("Too many quests in manager\n");
		return;
	}

	quest_manager_list_push(&qm->quests, quest);
}

Quest* quest_manager_get_quest(QuestManager* manager, QuestType key)
{
	const u32 len = quest_manager_list_len(&manager->quests);
	for(i32 i = 0; i < len; i++)
	{
		Quest* q = &manager->quests.data[i];
		if(q->data.type == key)
		{
			return q;
		}
	}
	return NULL;
}

void quest_manager_delete_quest(QuestManager* manager, QuestType type)
{
	QuestManager* qm = manager;
	const u32 len = quest_manager_list_len(&manager->quests);

	i32 index = 0;
	for(i32 i = 0; i < len; i++)
	{
		index = i;
		if(quest_manager_list_get(&qm->quests, i)->data.type == type) break;
	}
	quest_manager_list_del(&qm->quests, index);

#if 0
	for(i32 i = index; i < qm->len; i++)
	{
		qm->quests[i] = qm->quests[i + 1];
	}
	qm->len --;
#endif 
}


void player_tick_level(Player* self, GameState* state)
{
	Entity* player_entity = self->entity;

}

Player* player_new()
{
	Player* player = malloc(sizeof(Player));
	player->entity = entity_new(ENTITY_PLAYER, (Vector2) { 10.0, 10.0 } );
	player->inventory = ui_inventory_new(INVENTORY_TYPE_PLAYER);	
	player->quest_manager = player_quest_manager_new(player);
	player->skill_tree = skill_tree_new();
	player->class = CHARACTER_CLASS_WARLOCK;

	for(i32 i = 0; i < player->quest_manager->quests.len; i++)
	{
		Quest* q = quest_manager_list_get(&player->quest_manager->quests, i);
		P_LOG("Quest type: %d, with status %d\n", q->data.class, q->status);
	}

	return player;
}

void player_tick(Player* player, GameState* state)
{
	Entity* player_entity = player->entity;
	player_entity->mid_pos = Vector2Midpoint(player_entity->pos, player_entity->data.dim);

	//ui_inventory_tick(player->inventory, state);
	ui_inventory_player_toggle(player->inventory, state);
	ui_skill_tree_toggle(state);

	ui_inventory_tick(player->inventory, state);

	if(quests_are_updated)
	{
		quest_check_completion(player);
		quests_are_updated = false;
	}

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
