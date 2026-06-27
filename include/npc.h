#pragma once

#include "entity.h"
#include "struct.h"
#include "enum.h"

void npc_tick(NPC* npc, GameState* state);
void npcs_tick(DynList* npcs, GameState* state);
void npc_render(NPC* npc, GameState* state);
void npcs_render(DynList* npcs, GameState* state);
void npcs_ui_render(DynList* npcs, GameState* state);
NPC* npc_new(NPCType type, Vector2 pos);
