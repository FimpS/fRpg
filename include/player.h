#pragma once

#include "global.h"
#include "struct.h"
#include "enum.h"
#include "state.h"


void player_tick(Player* player, GameState* state);
void player_render(Player* player, GameState* state);

Player* player_new();
void player_destroy(Player* player);
