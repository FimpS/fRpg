#pragma once

#include "global.h"
#include "struct.h"
#include "enum.h"
#include "state.h"

/* Player related functions */
void player_tick(Player* player, GameState* state);
void player_render(Player* player, GameState* state);
/* Player related functions */

/* QuestManager functions */
bool quest_is_complete(Player* player, QuestType key);
void quest_register_entry(Player* player, QuestObjectiveType type, QuestObjectiveData data);
Quest* quest_manager_get_quest(QuestManager* manager, QuestType key);
void quest_manager_push_quest(QuestManager* manager, Quest quest);
void quest_manager_delete_quest(QuestManager* manager, QuestType type);
/* QuestManager functions */

Player* player_new();
void player_destroy(Player* player);
